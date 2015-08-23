#include "lispy.hh"
#include <iostream>
#include <fstream>

namespace {

template <typename T, typename C, typename F>
bool is_in(T&& item, C&& container, F&& f)
{
	for(auto&& it : container) {
		if(f(it, item))
			return true;
	}
	return false;
}

void add_alias(const std::string& prefix, ast_t& ast)
{
	if(ast.tag == ast_tag::ID)
		ast.alias = prefix + ast.value;
	for(ast_t& c : ast.children)
		add_alias(prefix, c);
}

void add_alias(const std::string& prefix, std::vector<std::pair<std::string, ValPtr>>& pair_list)
{
	for(std::pair<std::string, ValPtr>& p : pair_list)
		p.first = prefix + p.first;
}

void add_alias(const std::string& prefix, std::vector<std::string>& param_list)
{
	for(std::string& s : param_list)
		s = prefix + s;
}

template <typename F>
void add_alias(const std::string& prefix, ast_t& ast, F&& f)
{
	if(f(ast))
		ast.alias = prefix + ast.value;
	for(ast_t& t : ast.children)
		add_alias(prefix, t, f);
}

/** print */
ValPtr builtin_print(const std::vector<ValPtr>& params) {
	for(const ValPtr& v : params) {
		std::cout << to_string(v);
	}
	return std::make_shared<Val>(nil_t{});
}

/** println */
ValPtr builtin_println(const std::vector<ValPtr>& params)
{
	builtin_print(params);
	std::cout << std::endl;
	return std::make_shared<Val>(nil_t{});
}

/** set */
ValPtr builtin_set(const std::vector<ValPtr>& params)
{
	if(!Parser::symbol_table.is_top())
		throw Error("set: set can only be used at top level");
	if(params.size() != 2) 
		throw Error("set: usage: set {ID} value");
	if(params[0]->type() != typeid(ast_t))
		throw Error("set: usage: set {ID} value");
	const ast_t& ast = boost::get<ast_t>(*params[0]);		/**< QEXPR */
	if(ast.children[0].children.size() != 1)			/**< EXPRS可能没有children */
		throw Error("set: usage: set {ID} value");
	const ast_t& id = ast.children[0].children[0].children[0];	/**< 不用判断是否有children, 因为EXPR一定有 */
	if(id.tag != ast_tag::ID)
		throw Error("set: usage: set {ID} value");
	Parser::symbol_table.add_global(id.value, params[1]);
	return std::make_shared<Val>(nil_t{});
}

/** let */
ValPtr builtin_let(const std::vector<ValPtr>& params)
{
	if(params.size() == 0)
		throw Error("let need at least one param");
	for(const ValPtr& ptr : params) {
		if(ptr->type() != typeid(ast_t))
			throw Error("let: usage: let {(ID1 v1)(ID2 v2)} { body }...");		
	}
						/*QEXPR*/  /*EXPRS*/   /*EXPR*/
	std::vector<std::pair<std::string, ValPtr>> list;
	for(const ast_t& t : boost::get<ast_t>(*params[0]).children[0].children) {
		     /*SEXPR*/    /*EXPRS*/     	  /*SEXPR*/     /*EXPRS*/   /*EXPR*/                            
		if(t.children[0].children.size() == 0 || t.children[0].children[0].children.size() != 2)
			throw Error("let: usage: let {(ID1 v1)(ID2 v2)...} { body }...");
		if(t.children[0].children[0].children[0].children[0].tag != ast_tag::ID)		
			throw Error("let: usage: let {(ID1 v1)(ID2 v2)...} { body }...");
		list.push_back(std::make_pair(t.children[0].children[0].children[0].children[0].value,
			eval(t.children[0].children[0].children[1])));
	}

	std::vector<ast_t> ast_list;
	std::string prefix = "~" + gen_unique_id() + "~";
	for(size_t i = 1; i < params.size(); ++i) {
		ast_t ast = boost::get<ast_t>(*params[i]);
		add_alias(prefix, ast, [&](const ast_t& a) -> bool {
			return a.tag == ast_tag::ID && is_in(a.value, list, [](auto p, auto s) {
				return s == p.first;
			});
		});
		ast_list.push_back(ast);
	}
	add_alias(prefix, list);
	SymbolLayer layer;
	for(const std::pair<std::string, ValPtr>& p : list)
		Parser::symbol_table.add(p.first, p.second);
	ValPtr ret = std::make_shared<Val>(nil_t{});
	for(ast_t& t : ast_list) 
		ret = eval(t, true);
	return ret;
}

void fill_param_list(std::vector<std::string>& param_list, const ast_t& ast)
{
	if(ast.children[0].children.size() == 0)
		return;
	for(const ast_t& t : ast.children[0].children) {		/**< EXPR */
		if(t.children[0].tag != ast_tag::ID) {
			std::cout << "expect ID" << std::endl;
			throw Error("lambda: usage: lambda {param list} {body}");
		}
		param_list.push_back(t.children[0].value);
	}
}

void fill_capture_list(const std::vector<std::string>& param_list, 
	std::vector<std::pair<std::string, ValPtr>>& capture_list, const ast_t& ast)
{
	if(ast.children.size()) {
		for(const ast_t& t : ast.children)
			fill_capture_list(param_list, capture_list, t);
	} else if(ast.tag == ast_tag::ID) {
		ValPtr val = nullptr;
		if(ast.alias != "") {
			val = Parser::symbol_table.look(ast.alias);
		} else {
			val = Parser::symbol_table.look(ast.value);
		}
		if(!Buildin(ast.value) && !is_in(ast.value, param_list, 
			[](auto x, auto y) {
				return x == y;
			}
		)) {
			if(val != nullptr)
				capture_list.push_back(std::make_pair(ast.value, val));
		}
	}
}

/** lambda */
ValPtr builtin_lambda(const std::vector<ValPtr>& params)
{
	if(params.size() != 2)
		throw Error("lambda: expect two params");
	std::vector<std::string> param_list;
	std::vector<std::pair<std::string, ValPtr>> capture_list;
	if(params[0]->type() != typeid(ast_t) || params[1]->type() != typeid(ast_t))	
		throw Error("lambda: usage: lambda {param list} {body}");
	fill_param_list(param_list, boost::get<ast_t>(*params[0]));
	fill_capture_list(param_list, capture_list, boost::get<ast_t>(*params[1]));
	ast_t ast = boost::get<ast_t>(*params[1]);
	std::string prefix = "~" + gen_unique_id() + "~";
	add_alias(prefix, param_list);
	add_alias(prefix, capture_list);
	add_alias(prefix, ast);
	return std::make_shared<Val>(Lambda{param_list, capture_list, ast});
}

ValPtr builtin_lt(const std::vector<ValPtr>& params)
{
	if(params.size() != 2) 
		throw Error("lt: usage: = value value");
	if(params[0]->type() == typeid(int)) {
		int pv0 = boost::get<int>(*params[0]);
		if(params[1]->type() == typeid(int)) {
			if(pv0 < boost::get<int>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else if(params[1]->type() == typeid(float)) {
			if(pv0 < boost::get<float>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else {
			throw Error("lt: the first param is a number but the second isn't");
		}
	} else if(params[0]->type() == typeid(float)) {
		float pv0 = boost::get<float>(*params[0]);
		if(params[1]->type() == typeid(int)) {
			if(pv0 < boost::get<int>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else if(params[1]->type() == typeid(float)) {
			if(pv0 < boost::get<float>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else {
			throw Error("lt: the first param is a number but the second isn't");
		}
	} else if(params[0]->type() == typeid(std::string) && params[1]->type() == typeid(std::string)) {
		if(boost::get<std::string>(*params[0]) < boost::get<std::string>(*params[1]))
			return std::make_shared<Val>("T");
		return std::make_shared<Val>(nil_t{});
	}
	throw Error("lt: can't compare such two items");
}

ValPtr builtin_eval(const std::vector<ValPtr>& params)
{
	if(params.size() != 1)
		throw Error("eval: take exactly one param");
	if(params[0]->type() == typeid(ast_t)) {
		return eval(boost::get<ast_t>(*params[0]), true);
	} else {
		return params[0];
	}
}

ValPtr builtin_load(const std::vector<ValPtr>& params)
{
	for(const ValPtr& v : params) {
		if(v->type() != typeid(std::string)) {
			std::cout << "WARNING: " << to_string(v) << " is unlikely to be a path name, skiped" << std::endl;
			continue;
		}
		std::ifstream file(boost::get<std::string>(*v));
		if(!file) {
			std::cout << "ERROR: open " << boost::get<std::string>(*v) << " failed" << std::endl;
			continue;
		}
		Parser p;
		p.parse(file);
	}
	return std::make_shared<Val>(nil_t{});
}

template <typename R>
R op_helper(char op, const std::vector<ValPtr>& params)
{
	R base = 0;
	if(params[0]->type() == typeid(float)) {
		base = boost::get<float>(*params[0]);
	} else {
		base = boost::get<int>(*params[0]);
	}
	std::function<R(R, R)> oper;
	switch(op) {
		case '+': oper = [](auto p1, auto p2) { return p1 + p2; }; break;
		case '-': oper = [](auto p1, auto p2) { return p1 - p2; }; break;
		case '*': oper = [](auto p1, auto p2) { return p1 * p2; }; break;
		case '/': oper = [](auto p1, auto p2) { return p1 / p2; }; break;
	}
	for(size_t i = 1; i < params.size(); ++i) {
		if(params[i]->type() == typeid(float)) {
			base = oper(base, boost::get<float>(*params[i]));
		} else {
			base = oper(base, boost::get<int>(*params[i]));
		}
	}
	return base;
}

ValPtr builtin_op(char op, const std::vector<ValPtr>& params)
{
	if(params.size() == 0)
		throw Error("op: operation + - * / need at least one param");
	bool float_flag = false;
	for(const ValPtr& v : params) {
		if(v->type() != typeid(int)) {
			if(v->type() == typeid(float)) {
				float_flag = true;
			} else {
std::cout << "is " << to_string(v) << std::endl;
				throw Error("op: all params should be number");
			}
		}
	}
	if(float_flag) {
		return std::make_shared<Val>(op_helper<float>(op, params));
	} else {
		return std::make_shared<Val>(op_helper<int>(op, params));
	}	
}

ValPtr builtin_list(const std::vector<ValPtr>& params)
{
	ValPtr li = std::make_shared<Val>(ast_t{ast_tag::QEXPR});
	ast_t& t = boost::get<ast_t>(*li);
	t.children.push_back(ast_t{ast_tag::EXPRS});
	for(const ValPtr& v : params) {
		if(v->type() == typeid(int)) {
			t.children[0].children.push_back(ast_t{ast_tag::INT});
			t.children[0].children.back().value = std::to_string(boost::get<int>(*v));
		} else if(v->type() == typeid(float)) {
			t.children[0].children.push_back(ast_t{ast_tag::FLOAT});
			t.children[0].children.back().value = std::to_string(boost::get<float>(*v));
		} else if(v->type() == typeid(std::string)) {
			t.children[0].children.push_back(ast_t{ast_tag::STRING});
			t.children[0].children.back().value = "\"" + boost::get<std::string>(*v) + "\"";
		} else if(v->type() == typeid(ast_t)) {
			t.children[0].children.push_back(boost::get<ast_t>(*v));
		} else {
		 	throw Error("list: " + to_string(v) + " has no value");
		}
	}
	return li;
}

ValPtr builtin_first(const std::vector<ValPtr>& params)
{
	if(params.size() != 1)
		throw Error("first: need exactly one param");
	if(params[0]->type() != typeid(ast_t))
		throw Error("first: need list");
	const ast_t& t = boost::get<ast_t>(*params[0]);
	if(t.children[0].children.size() == 0)
		return std::make_shared<Val>(nil_t{});
	return eval(t.children[0].children[0]);
}

ValPtr builtin_rest(const std::vector<ValPtr>& params)
{
	if(params.size() != 1)
		throw Error("rest: need exactly one param");
	if(params[0]->type() != typeid(ast_t))
		throw Error("rest: need qexpr");
	const ast_t& t = boost::get<ast_t>(*params[0]);
	if(t.children[0].children.size() == 0)
		return std::make_shared<Val>(nil_t{});
	ast_t tr = t;
	tr.children[0].children.erase(tr.children[0].children.begin());
	return eval(tr);
}

ValPtr builtin_if(const std::vector<ValPtr>& params)
{
	if(params.size() < 2 || params.size() > 3)
		throw Error("if: need 2 or 3 params");
	if(params[0]->type() != typeid(nil_t)) {
		if(params[1]->type() == typeid(ast_t)) {
			return eval(boost::get<ast_t>(*params[1]), true);
		} else {
			return params[1];
		}
	} else if(params.size() == 3) {
		if(params[2]->type() == typeid(ast_t)) {
			return eval(boost::get<ast_t>(*params[2]), true);
		} else {
			return params[2];
		}
	} else {
		return std::make_shared<Val>(nil_t{});
	}
}

ValPtr builtin_assign(const std::vector<ValPtr>& params)
{
	if(params.size() != 2)
		throw Error("assign: need exactly 2 params");
	*params[0] = *params[1];
	return params[0];
}

ValPtr builtin_append(const std::vector<ValPtr>& params)
{
	if(params.size() != 2) 
		throw Error("append: need exactly two params");
	if(params[0]->type() != typeid(ast_t))
		throw Error("append: usage: append {list} val");
	ast_t t = boost::get<ast_t>(*params[0]);
	if(params[1]->type() == typeid(int)) {
		t.children[0].children.push_back(ast_t{ast_tag::INT});
		t.children[0].children.back().value = std::to_string(boost::get<int>(*params[1]));
	} else if(params[1]->type() == typeid(float)) {
		t.children[0].children.push_back(ast_t{ast_tag::FLOAT});
		t.children[0].children.back().value = std::to_string(boost::get<float>(*params[1]));
	} else if(params[1]->type() == typeid(std::string)) {
		t.children[0].children.push_back(ast_t{ast_tag::STRING});
		t.children[0].children.back().value = "\"" + boost::get<std::string>(*params[1]) + "\"";
	} else if(params[1]->type() == typeid(ast_t)) {
		t.children[0].children.push_back(boost::get<ast_t>(*params[1]));
	} else {
	 	throw Error("list: " + to_string(params[1]) + " has no value");
	}
	return std::make_shared<Val>(t);
}

ValPtr builtin_type(const std::type_index& type, const std::vector<ValPtr>& params)
{
	if(params.size() != 1)
		throw Error("type detector need exactly one param");
	if(type == params[0]->type())
		return std::make_shared<Val>("T");
	return std::make_shared<Val>(nil_t{});
}

ValPtr builtin_exit(const std::vector<ValPtr>&)
{
	exit(0);
}
}

std::unordered_map<std::string, Buildin::operator_t> Buildin::builtin_table = 
{
	{ "print", builtin_print },
	{ "println", builtin_println },
	{ "set", builtin_set },
	{ "let", builtin_let },
	{ "lambda", builtin_lambda },
	{ "<", builtin_lt },
	{ "eval", builtin_eval },
	{ "load", builtin_load },
	{ "+", std::bind(builtin_op, '+', std::placeholders::_1) },
	{ "-", std::bind(builtin_op, '-', std::placeholders::_1) },
	{ "*", std::bind(builtin_op, '*', std::placeholders::_1) },
	{ "/", std::bind(builtin_op, '/', std::placeholders::_1) },
	{ "list", builtin_list },
	{ "first", builtin_first },
	{ "rest", builtin_rest },
	{ "if", builtin_if },
	{ "as", builtin_assign },
	{ "append", builtin_append },
	{ "is_nil", std::bind(builtin_type, std::ref(typeid(nil_t)), std::placeholders::_1)},
	{ "is_int", std::bind(builtin_type, std::ref(typeid(int)), std::placeholders::_1)},
	{ "is_float", std::bind(builtin_type, std::ref(typeid(float)), std::placeholders::_1)},
	{ "is_string", std::bind(builtin_type, std::ref(typeid(std::string)), std::placeholders::_1)},
	{ "is_list", std::bind(builtin_type, std::ref(typeid(ast_t)), std::placeholders::_1)},
	{ "is_lambda", std::bind(builtin_type, std::ref(typeid(Lambda)), std::placeholders::_1)},
	{ "is_builtin", std::bind(builtin_type, std::ref(typeid(Buildin)), std::placeholders::_1)},
	{ "exit", builtin_exit },
	{ "native", builtin_native }
};
