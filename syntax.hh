#pragma once
#include <iostream>
#include <queue>
#include "lispy.hh"

class Syntax {
private:
	using token_queue_t = std::queue<std::pair<Token, std::string>>;
public:
	Syntax(MessageBus& mb) : message_bus_(mb) {
		message_bus_.attach([this](Token t, std::string content) {
/*
			switch(t) {
				case Token::LPAREN:
					std::cout << "LPAREN" << std::endl;
					break;
				case Token::RPAREN:
					std::cout << "RPAREN" << std::endl;
					break;
				case Token::ID:
					std::cout << "ID" << std::endl;
					break;
				case Token::INT:
					std::cout << "INT" << std::endl;
					break;
				case Token::FLOAT:
					std::cout << "FLOAT" << std::endl;
					break;
				case Token::STRING:
					std::cout << "STRING" << std::endl;
					break;
				case Token::RBRACE:
					std::cout << "RBRACE" << std::endl;
					break;
				case Token::LBRACE:
					std::cout << "LBRACE" << std::endl;
					break;
			}
			std::cout << "content: " << content << std::endl;
*/
			token_buf_.push(std::make_pair(t, content));
		}, "token");
		message_bus_.attach([this] {
			parse(token_buf_);
		}, "token eof");			
	}
private:
	void parse(token_queue_t& que) {
		try {
			ast_t tree = exprs(que);
			for(auto& t : tree.children)
				std::cout << to_string(eval(t)) << std::endl;
		} catch(Error& se) {
			std::cout << se.what() << std::endl;
			std::cout << "before:" << std::endl;
			size_t count = 30;
			while(!que.empty() && count--) {
				std::cout << que.front().second << " ";
				que.pop();
			}
			std::cout << std::endl;
		}
	}

	ast_t exprs(token_queue_t& que) {
		ast_t tree(ast_tag::EXPRS);
		while(true) {
			if(que.empty()) return tree;
			switch(que.front().first) {
				case Token::INT: 
				case Token::FLOAT: 
				case Token::ID:
				case Token::STRING:
				case Token::LPAREN: 
				case Token::LBRACE:
					tree.children.push_back(expr(que));
					break;
				default:
					return tree;
			}
		}
	}

	ast_t expr(token_queue_t& que) {
		ast_t tree(ast_tag::EXPR);
		switch(que.front().first) {
			case Token::INT:
				tree.children.push_back(ast_t{ast_tag::INT});
				tree.children.back().value = que.front().second;
				que.pop();
				return tree;
			case Token::FLOAT:
				tree.children.push_back(ast_t{ast_tag::FLOAT});
				tree.children.back().value = que.front().second;
				que.pop();
				return tree;
			case Token::ID:
				tree.children.push_back(ast_t{ast_tag::ID});
				tree.children.back().value = que.front().second;
				que.pop();
				return tree;
			case Token::STRING:
				tree.children.push_back(ast_t{ast_tag::STRING});
				tree.children.back().value = que.front().second;
				que.pop();
				return tree;
			case Token::LPAREN:
				tree.children.push_back(sexpr(que));
				return tree;
			case Token::LBRACE:
				tree.children.push_back(qexpr(que));
				return tree;
			default:
				throw Error("expect number, ID, '(' or '{'");
		}
	}		
	
	ast_t sexpr(token_queue_t& que) {
		if(que.front().first != Token::LPAREN)
			throw Error("expect '('");
		que.pop();
		ast_t tree(ast_tag::SEXPR);
		tree.children.push_back(exprs(que));
		if(que.empty() || que.front().first != Token::RPAREN)
			throw Error("expect ')'");
		que.pop();
		return tree;
	}

	ast_t qexpr(token_queue_t& que) {
		if(que.front().first != Token::LBRACE)
			throw Error("expect '{'");
		que.pop();
		ast_t tree(ast_tag::QEXPR);
		tree.children.push_back(exprs(que));
		if(que.empty() || que.front().first != Token::RBRACE)
			throw Error("expect '}'");
		que.pop();
		return tree;
	}

	std::queue<std::pair<Token, std::string>> token_buf_;
	MessageBus& message_bus_;
};
