#include "lispy.hh"
#include <boost/lexical_cast.hpp>

namespace {

ValPtr eval_qexpr(const ast_t& tree) 
{
	return std::make_shared<Val>(tree);
}

ValPtr eval_id(const ast_t& tree)
{
	Buildin build_in{tree.value};
	if(build_in)
		return std::make_shared<Val>(build_in);
	ValPtr val = Parser::symbol_table.look(tree.value);
	if(val)
		return val;
	throw Error("EVAL: " + tree.value + " is undefined");
}

ValPtr eval_exprs(const ast_t& tree)
{
	if(tree.children.size() == 0)
		return std::make_shared<Val>(nil_t{});

	std::vector<ValPtr> params;
	for(size_t i = 1; i < tree.children.size(); ++i)
		params.push_back(eval(tree.children[i]));

	ValPtr func = eval(tree.children[0]);
	if(func->type() == typeid(Buildin)) {
		return boost::get<Buildin>(*func).apply(params);
	} else if(func->type() == typeid(Lambda)) {
		return boost::get<Lambda>(*func).apply(func, params);
	} else {
		std::cout << "unexpected " << to_string(func) << std::endl;
		throw Error("EVAL: expect a function name");
	}
}

ValPtr eval_sexpr(const ast_t& tree)
{
	return eval_exprs(tree.children[0]);
}

ValPtr eval_expr(const ast_t& tree) 
{
	switch(tree.children[0].tag) {
		case ast_tag::INT:
			return std::make_shared<Val>(boost::lexical_cast<int>(tree.children[0].value));
		case ast_tag::FLOAT:
			return std::make_shared<Val>(boost::lexical_cast<float>(tree.children[0].value));
		case ast_tag::ID:
			return eval_id(tree.children[0]);
		case ast_tag::STRING:
			return std::make_shared<Val>(std::string{tree.children[0].value.begin() + 1, tree.children[0].value.end() - 1});
		case ast_tag::SEXPR:
			return eval_sexpr(tree.children[0]);
		case ast_tag::QEXPR:
			return eval_qexpr(tree.children[0]);
		default:
			assert(0);			
	}
}
}

ValPtr eval(const ast_t& tree, bool qexpr_as_sexpr)
{
/*
	switch(tree.tag) {
		case ast_tag::INT:
			std::cout << "INT: " << tree.value << std::endl;
			break;
		case ast_tag::FLOAT:
			std::cout << "FLOAT: " << tree.value << std::endl;
			break;
		case ast_tag::ID:
			std::cout << "ID: " << tree.value << std::endl;
			break;
		case ast_tag::STRING:
			std::cout << "STRING: " << tree.value << std::endl;
			break;
		case ast_tag::SEXPR:
			std::cout << "SEXPR" << std::endl;
			for(auto& t : tree.children)
				eval(t);
			break;
		case ast_tag::QEXPR:
			std::cout << "QEXPR" << std::endl;
			for(auto& t : tree.children)
				eval(t);
			break;
		case ast_tag::EXPR:
			std::cout << "EXPR" << std::endl;
			for(auto& t : tree.children)
				eval(t);
			break;
		case ast_tag::EXPRS:
			std::cout << "EXPS" << std::endl;
			for(auto& t : tree.children)
				eval(t);
			break;
			
	}
	return nullptr;
*/
	switch(tree.tag) {
		case ast_tag::INT:
			return std::make_shared<Val>(boost::lexical_cast<int>(tree.value));
		case ast_tag::FLOAT:
			return std::make_shared<Val>(boost::lexical_cast<float>(tree.value));
		case ast_tag::ID:
			return eval_id(tree);
		case ast_tag::STRING:
			return std::make_shared<Val>(std::string{tree.value.begin() + 1, tree.value.end() - 1});
		case ast_tag::SEXPR:
			return eval_sexpr(tree);
		case ast_tag::QEXPR:
			if(qexpr_as_sexpr) return eval_sexpr(tree);
			return eval_qexpr(tree);
		case ast_tag::EXPR:
			return eval_expr(tree); 
		case ast_tag::EXPRS:
			return eval_exprs(tree);
	}
	assert(false);
}
