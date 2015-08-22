#include "lispy.hh"
#include <sstream>

std::string to_string(const ValPtr& ptr)
{
	assert(ptr != nullptr);
	if(ptr->type() == typeid(nil_t)) {
		return "nil";
	} else if(ptr->type() == typeid(int)) {
		return std::to_string(boost::get<int>(*ptr));
	} else if(ptr->type() == typeid(float)) {
		return std::to_string(boost::get<float>(*ptr));
	} else if(ptr->type() == typeid(std::string)) {
		return boost::get<std::string>(*ptr);
	} else if(ptr->type() == typeid(ast_t)) {
		return boost::get<ast_t>(*ptr).to_string();
	} else if(ptr->type() == typeid(Lambda)) {
		std::stringstream ss;
		ss << ptr;
		return "lambda@" + ss.str();
	} else if(ptr->type() == typeid(Buildin)) {
		std::stringstream ss;
		ss << reinterpret_cast<void *>(boost::get<Buildin>(*ptr).addr());
		return "buildin@" + ss.str();
	} else {
		assert(0);
	} 
}
