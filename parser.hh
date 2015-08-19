#pragma once
#include <functional>
#include "symbol_table.hh"
#include "syntax.hh"

class Parser {
public:
	Parser() : msg_bus_(), lexical_(msg_bus_), syntax_(msg_bus_) {
		msg_bus_.attach([this]{ 
			stoped_ = true; 
			std::cout << "parser stoped" << std::endl;
		}, "stop");	
	}

	bool feed(char c) {
		if(stoped_) return false;
		msg_bus_.send<char>(c, "char");
		return true;	
	}

	void parse(std::istream& in) {
		char ch;
		while((ch = in.get()) != EOF && feed(ch));
		if(in.eof())
			msg_bus_.send("input eof");
	}

	static SymbolTable symbol_table;		
private:
	MessageBus msg_bus_;
	Lexical lexical_;
	Syntax syntax_;
	bool stoped_ = false;
};

struct SymbolLayer {
	SymbolLayer() {
		Parser::symbol_table.push();
	}
	~SymbolLayer() {
		Parser::symbol_table.pop();
	}
};
