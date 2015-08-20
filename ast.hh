#pragma once
#include <vector>
#include <string>

enum class ast_tag { 
	EXPR, EXPRS, SEXPR, QEXPR, INT, FLOAT, STRING, ID
};

struct ast_t {
	ast_t(ast_tag t) : tag(t) {}
	ast_tag tag;
	std::string value;
	std::vector<ast_t> children;
	std::string to_string() const {
		switch(tag) {
			case ast_tag::EXPR:
				return children[0].to_string();
			case ast_tag::EXPRS: {
				std::string str{};
				for(size_t i = 0; i < children.size(); ++i) {
					str += children[i].to_string();
					if(i != children.size() - 1)
						str += " ";
				}
				return str;
			}
			case ast_tag::SEXPR:
				return "(" + children[0].to_string() + ")";
			case ast_tag::QEXPR:
				return "{" + children[0].to_string() + "}";
			default:
				return value;
		}	
	}
};
