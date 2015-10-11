#include "lispy.hh"
#ifdef __linux__
#include <dlfcn.h>
#endif
#ifdef WIN32
#include <windows.h>
typedef int (__cdecl *MYPROC)(LPWSTR);
#endif

namespace {
#ifdef WIN32
typedef HMODULE lib_handle_t;
lib_handle_t open_dynamic_lib(const std::string& lib_name)
{
	lib_handle_t h;
	h = LoadLibrary(TEXT(lib_name.c_str()));
	DWORD error = GetLastError();
	//if(error)
	//	throw Error(error);
	return h;
}

void *look_symbol(lib_handle_t handle, const std::string& symbol_name)
{
	MYPROC f = (MYPROC) GetProcAddress(handle, TEXT(symbol_name.c_str()));
	DWORD error = GetLastError();
	//if(error)
		//throw Error(error);
	return (void *)f;
}
#else
using lib_handle_t = void*;
lib_handle_t open_dynamic_lib(const std::string& lib_name)
{
	lib_handle_t h = dlopen(lib_name.c_str(), RTLD_NOW);
	char *error = dlerror();
	if(error)
		throw Error(error);
	return h;
}
void *look_symbol(lib_handle_t handle, const std::string& symbol_name)
{
	void *f = dlsym(handle, symbol_name.c_str());
	char *error = dlerror();
	if(error)
		throw Error(error);
	return f;	
}
#endif
using lib_operator_ptr = void(*)(ValPtr&, const std::vector<ValPtr>& params);
ValPtr operator_wrapper(lib_operator_ptr lib_oper, const std::vector<ValPtr>& params)
{
	ValPtr ret = nullptr;
	lib_oper(ret, params);
	return ret;
}
}

ValPtr builtin_native(const std::vector<ValPtr>& params)
{
	if(params.size() != 2) 
		throw Error("native: need 2 params");
	if(!params[0]->Is<std::string>() ||
		!params[1]->Is<std::string>())
		throw Error("native: usage: native lib symbol");
	lib_handle_t h = open_dynamic_lib(params[0]->Get<std::string>());
	void *sym_ptr = look_symbol(h, params[1]->Get<std::string>());
	return std::make_shared<Val>(Buildin(std::bind(operator_wrapper, (lib_operator_ptr)sym_ptr, std::placeholders::_1)));
}	
