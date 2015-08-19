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
			oper = &op->second;
	}
	operator bool() {
		return oper != nullptr;
	}
	ValPtr apply(const std::vector<ValPtr>& params) {
		return (*oper)(params);
	}
	operator_ptr addr() {
		return oper;
	}
private:
	operator_ptr oper = nullptr;
};	
