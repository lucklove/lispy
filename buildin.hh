#pragma once

#include <unordered_map>
#include <functional>
#include "val.hh"

class Buildin {
public:
	using operator_t = std::function<ValPtr(const std::vector<ValPtr>&)>;
	using operator_ptr = operator_t*;
private:
	static std::unordered_map<std::string, operator_t> buildin_table;
public:
	Buildin(const std::string& name) {
		auto op = buildin_table.find(name);
		if(op != buildin_table.end())
			oper_ptr = &op->second;
	}

	Buildin(operator_t o) : oper(o) {}

	operator bool() {
		return oper || oper_ptr;
	}

	ValPtr apply(const std::vector<ValPtr>& params) {
		if(oper) {
			return oper(params);
		} else if(oper_ptr) {
			return (*oper_ptr)(params);
		}
		assert(false && "invalid buildin");
	}

	operator_ptr addr() {
		if(oper) {
			return &oper;
		} else if(oper_ptr) {
			return oper_ptr;
		}
		assert(false && "invalid buildin");
	}
private:
	operator_t oper{};
	operator_ptr oper_ptr = nullptr;
};	
