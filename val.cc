#include "lispy.hh"
#include <sstream>

#define R(c)					\
do {						\
	iter = str.erase(iter, iter + 2);	\
	iter = str.insert(iter, c);		\
} while(0)
std::string trim_string(std::string str)
{
	for(auto iter = str.begin(); iter < str.end(); ++iter) {
		if(*iter == '\\') {
			switch(*(iter + 1)) {
				case '0': R('\0'); break;
				case 'a': R('\a'); break;
				case 'b': R('\b'); break;
				case 't': R('\t'); break;
				case 'n': R('\n'); break;
				case 'v': R('\v'); break;
				case 'f': R('\f'); break;
				case 'r': R('\r'); break;
				case 'e': R('\e'); break;
				case '\\': R('\\'); break;
				default:
					R(*(iter + 1));
			}
		}
	}
	return str;
}

std::string to_string(const ValPtr& ptr)
{
	assert(ptr != nullptr);
	if(ptr->Is<nil_t>()) {
		return "nil";
	} else if(ptr->Is<int>()) {
		return std::to_string(ptr->Get<int>());
	} else if(ptr->Is<float>()) {
		return std::to_string(ptr->Get<float>());
	} else if(ptr->Is<std::string>()) {
		return trim_string(ptr->Get<std::string>());
	} else if(ptr->Is<ast_t>()) {
		return ptr->Get<ast_t>().to_string();
	} else if(ptr->Is<Lambda>()) {
		std::stringstream ss;
		ss << ptr;
		return "lambda@" + ss.str();
	} else if(ptr->Is<Buildin>()) {
		std::stringstream ss;
		ss << reinterpret_cast<void *>(ptr->Get<Buildin>().addr());
		return "builtin@" + ss.str();
	} else {
		assert(0);
	} 
}
