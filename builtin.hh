#pragma once

#include <unordered_map>
#include <functional>
#include <cassert>
#include "val.hh"
#include "lambda.hh"

class Buildin {
public:
	using operator_t = std::function<ValPtr(const std::vector<ValPtr>&)>;
	using operator_ptr = operator_t*;
private:
	static std::unordered_map<std::string, operator_t> builtin_table;
public:
	Buildin(const std::string& name) {
		auto op = builtin_table.find(name);
		if(op != builtin_table.end())
			oper_ptr = &op->second;
	}

	Buildin(operator_t o) : oper(std::make_shared<operator_t>(o)) {}

	operator bool() {
		return oper || oper_ptr;
	}

	ValPtr apply(const std::vector<ValPtr>& params);

	operator_ptr addr() {
		if(oper) {
			return oper.get();
		} else if(oper_ptr) {
			return oper_ptr;
		}
		assert(false && "invalid builtin");
	}
private:
	std::shared_ptr<operator_t> oper = nullptr;
	operator_ptr oper_ptr = nullptr;
};	
