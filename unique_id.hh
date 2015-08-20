#pragma once
#include <sstream>

inline std::string gen_unique_id() {
	static size_t id = 0;
	std::stringstream ss;
	ss << id++;
	return ss.str();
}	
