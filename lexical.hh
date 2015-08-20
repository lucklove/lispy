#pragma once
#include <list>
#include <vector>
#include "message_bus.hh"
#include "token.hh"

class Lexical {
public:
	Lexical(MessageBus& mb) : message_bus_(mb) {
		message_bus_.attach([this](char c) { feed(c); }, "char");
		message_bus_.attach([this]{ stoped_ = true; }, "stop");
		message_bus_.attach([this]{ 
			reaction_map[cur_state](); message_bus_.send("token eof"); 
		}, "input eof");	
	}
private:
	using state_t = unsigned short;
	using case_t = unsigned char;

	case_t index_of(char c) {
		if(c > 0) return c;
		return 0;
	}

	bool feed(char c) {
		if(stoped_) return false;
		state_t next_state = 0;
		case_t cur_case = index_of(c);
		if((next_state = dfa_trans_table[cur_state][cur_case])) {
			token_.push_back(c);
			cur_state = next_state;
		} else {
			reaction_map[cur_state]();
			prev_ = std::move(token_);
			token_.clear();
			cur_state = 1;
			return feed(c);
		}
		return true;	
	}

	void error(const std::string& msg) {
		std::cout << msg << std::endl;
		std::cout << "prev token: " << prev_ << std::endl;
		message_bus_.send("stop");
	}

	MessageBus& message_bus_;
	std::string token_;
	std::string prev_;
	state_t cur_state = 1;
	bool stoped_ = false;
	static state_t dfa_trans_table[][128];
	std::vector<std::function<void()>> reaction_map = {
	/*0*/	[this]{ error("stop on state 0"); },
	/*1*/	[this]{ error("stop on state 1"); },
	/*2*/	[this]{ message_bus_.send<Token, std::string>(Token::ID, token_, "token"); },
	/*3*/	[this]{ message_bus_.send<Token, std::string>(Token::INT, token_, "token"); },
	/*4*/	[this]{ message_bus_.send<Token, std::string>(Token::FLOAT, token_, "token"); },
	/*5*/	[this]{ error("stop on state 5"); },
	/*6*/	[this]{ error("stop on state 6"); },
	/*7*/	[this]{ message_bus_.send<Token, std::string>(Token::STRING, token_, "token"); },
	/*8*/	[this]{ /* nop */ },
	/*9*/	[this]{ /* nop */ },
	/*10*/	[this]{ message_bus_.send<Token, std::string>(Token::RPAREN, token_, "token"); },
	/*11*/	[this]{ message_bus_.send<Token, std::string>(Token::LPAREN, token_, "token"); },
	/*12*/	[this]{ message_bus_.send<Token, std::string>(Token::RBRACE, token_, "token"); },
	/*13*/	[this]{ message_bus_.send<Token, std::string>(Token::LBRACE, token_, "token"); }
	};
};
