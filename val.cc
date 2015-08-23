#include "lispy.hh"
#include <sstream>

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
		return ptr->Get<std::string>();
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
