#include "lispy.hh"
#include <iostream>
#include <fstream>

namespace {

/** print */
ValPtr buildin_print(const std::vector<ValPtr>& params) {
	for(const ValPtr& v : params) {
		std::cout << to_string(v);
	}
	return std::make_shared<Val>(nil_t{});
}

/** println */
ValPtr buildin_println(const std::vector<ValPtr>& params)
{
	buildin_print(params);
	std::cout << std::endl;
	return std::make_shared<Val>(nil_t{});
}

/** set */
ValPtr buildin_set(const std::vector<ValPtr>& params)
{
	if(params.size() != 2) 
		throw Error("set: useage: set {ID} value");
	if(params[0]->type() != typeid(ast_t))
		throw Error("set: useage: set {ID} value");
	const ast_t& ast = boost::get<ast_t>(*params[0]);		/**< QEXPR */
	if(ast.children[0].children.size() != 1)			/**< EXPRS可能没有children */
		throw Error("set: useage: set {ID} value");
	const ast_t& id = ast.children[0].children[0].children[0];	/**< 不用判断是否有children, 因为EXPR一定有 */
	if(id.tag != ast_tag::ID)
		throw Error("set: useage: set {ID} value");
	Parser::symbol_table.add_global(id.value, params[1]);
	return std::make_shared<Val>(nil_t{});
}

/** let */
ValPtr buildin_let(const std::vector<ValPtr>& params)
{
	if(params.size() == 0)
		throw Error("let need at least one param");
	for(const ValPtr& ptr : params) {
		if(ptr->type() != typeid(ast_t))
			throw Error("let: useage: let {(ID1 v1)(ID2 v2)} { body }...");		
	}
	SymbolLayer layer;
						/*QEXPR*/  /*EXPRS*/   /*EXPR*/
	for(const ast_t& t : boost::get<ast_t>(*params[0]).children[0].children) {
		     /*SEXPR*/    /*EXPRS*/     	  /*SEXPR*/     /*EXPRS*/   /*EXPR*/                            
		if(t.children[0].children.size() == 0 || t.children[0].children[0].children.size() != 2)
			throw Error("let: useage: let {(ID1 v1)(ID2 v2)...} { body }...");
		if(t.children[0].children[0].children[0].children[0].tag != ast_tag::ID)		
			throw Error("let: useage: let {(ID1 v1)(ID2 v2)...} { body }...");
		Parser::symbol_table.add(t.children[0].children[0].children[0].children[0].value, 
			eval(t.children[0].children[0].children[1]));
	}
	for(size_t i = 1; i < params.size(); ++i) {
		ValPtr ret = eval(boost::get<ast_t>(*params[i]), true);
		if(i == params.size() - 1)
			return ret;
	}
	return std::make_shared<Val>(nil_t{});
}

void fill_param_list(std::vector<std::string>& param_list, const ast_t& ast)
{
	if(ast.children[0].children.size() == 0)
		return;
	for(const ast_t& t : ast.children[0].children) {		/**< EXPR */
		if(t.children[0].tag != ast_tag::ID) {
			std::cout << "expect ID" << std::endl;
			throw Error("lambda: useage: lambda {param list} {body}");
		}
		param_list.push_back(t.children[0].value);
	}
}

bool is_param(const std::string& name, const std::vector<std::string>& param_list)
{
	for(const std::string& s : param_list) {
		if(name == s)
			return true;
	}
	return false;
}

void fill_capture_list(const std::vector<std::string>& param_list, 
	std::vector<std::pair<std::string, ValPtr>>& capture_list, const ast_t& ast)
{
	if(ast.children.size()) {
		for(const ast_t& t : ast.children)
			fill_capture_list(param_list, capture_list, t);
	} else if(ast.tag == ast_tag::ID) {
		ValPtr val = Parser::symbol_table.look(ast.value);
		if(ast.value != "self" && !Buildin(ast.value) && !is_param(ast.value, param_list)) {
			if(val != nullptr)
				capture_list.push_back(std::make_pair(ast.value, val));
		}
	}
}

/** lambda */
ValPtr buildin_lambda(const std::vector<ValPtr>& params)
{
	if(params.size() != 2)
		throw Error("lambda: expect two params");
	std::vector<std::string> param_list;
	std::vector<std::pair<std::string, ValPtr>> capture_list;
	if(params[0]->type() != typeid(ast_t) || params[1]->type() != typeid(ast_t))	
		throw Error("lambda: useage: lambda {param list} {body}");
	fill_param_list(param_list, boost::get<ast_t>(*params[0]));
	fill_capture_list(param_list, capture_list, boost::get<ast_t>(*params[1]));
	return std::make_shared<Val>(Lambda{param_list, capture_list, boost::get<ast_t>(*params[1])});
}

ValPtr buildin_eq(const std::vector<ValPtr>& params)
{
	if(params.size() != 2) 
		throw Error("eq: useage: = value value");
	if(params[0]->type() == typeid(nil_t) && params[1]->type() == typeid(nil_t))
		return std::make_shared<Val>("T");
	if(params[0]->type() == typeid(int)) {
		int pv0 = boost::get<int>(*params[0]);
		if(params[1]->type() == typeid(int)) {
			if(pv0 == boost::get<int>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else if(params[1]->type() == typeid(float)) {
			if(pv0 == boost::get<float>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else {
			throw Error("eq: the first param is a number but the second isn't");
		}
	} else if(params[0]->type() == typeid(float)) {
		float pv0 = boost::get<float>(*params[0]);
		if(params[1]->type() == typeid(int)) {
			if(pv0 == boost::get<int>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else if(params[1]->type() == typeid(float)) {
			if(pv0 == boost::get<float>(*params[1])) {
				return std::make_shared<Val>("T");
			} else {
				return std::make_shared<Val>(nil_t{});
			}
		} else {
			throw Error("eq: the first param is a number but the second isn't");
		}
	} else if(params[0]->type() == typeid(std::string) && params[1]->type() == typeid(std::string)) {
		if(boost::get<std::string>(*params[0]) == boost::get<std::string>(*params[1]))
			return std::make_shared<Val>("T");
		return std::make_shared<Val>(nil_t{});
	}
	throw Error("eq: can't compare such two items");
}

ValPtr buildin_eval(const std::vector<ValPtr>& params)
{
	if(params.size() != 1)
		throw Error("eval: take exactly one param");
	if(params[0]->type() == typeid(ast_t)) {
		return eval(boost::get<ast_t>(*params[0]), true);
	} else {
		return params[0];
	}
}

ValPtr buildin_load(const std::vector<ValPtr>& params)
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

ValPtr buildin_op(char op, const std::vector<ValPtr>& params)
{
	if(params.size() == 0)
		throw Error("op: operation + - * / need at least one param");
	bool float_flag = false;
	for(const ValPtr& v : params) {
		if(v->type() != typeid(int)) {
			if(v->type() == typeid(float)) {
				float_flag = true;
			} else {
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

ValPtr buildin_list(const std::vector<ValPtr>& params)
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

ValPtr buildin_first(const std::vector<ValPtr>& params)
{
	if(params.size() != 1)
		throw Error("first: need exactly one param");
	if(params[0]->type() != typeid(ast_t))
		throw Error("first: need qexpr");
	const ast_t& t = boost::get<ast_t>(*params[0]);
	if(t.children[0].children.size() == 0)
		return std::make_shared<Val>(nil_t{});
	return eval(t.children[0].children[0]);
}

ValPtr buildin_rest(const std::vector<ValPtr>& params)
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

ValPtr buildin_if(const std::vector<ValPtr>& params)
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

ValPtr buildin_assign(const std::vector<ValPtr>& params)
{
	if(params.size() != 2)
		throw Error("assign: need exactly 2 params");
	*params[0] = *params[1];
	return params[0];
}
}

std::unordered_map<std::string, Buildin::operator_t> Buildin::buildin_table = 
{
	{ "print", buildin_print },
	{ "println", buildin_println },
	{ "set", buildin_set },
	{ "let", buildin_let },
	{ "lambda", buildin_lambda },
	{ "eq", buildin_eq },
	{ "=", buildin_eq },
	{ "eval", buildin_eval },
	{ "load", buildin_load },
	{ "+", std::bind(buildin_op, '+', std::placeholders::_1) },
	{ "-", std::bind(buildin_op, '-', std::placeholders::_1) },
	{ "*", std::bind(buildin_op, '*', std::placeholders::_1) },
	{ "/", std::bind(buildin_op, '/', std::placeholders::_1) },
	{ "list", buildin_list },
	{ "first", buildin_first },
	{ "rest", buildin_rest },
	{ "if", buildin_if },
	{ "as", buildin_assign }
};
