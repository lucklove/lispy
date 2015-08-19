#pragma once
#include <exception>

class Error : public std::exception {
public:
	Error(const std::string& msg) : msg_(msg) {}
	const char* what() const noexcept override {
		try { return msg_.c_str(); } catch(...) { return ""; }
	}
private:
	std::string msg_;
};
