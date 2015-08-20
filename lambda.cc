#include "lispy.hh"

ValPtr Lambda::apply(const std::vector<ValPtr>& params)
{
	if(params.size() != param_list_.size()) {
		std::cout << "lambda: need ";
		for(const std::string& p : param_list_)
			std::cout << p << " ";
		std::cout << std::endl;
		std::cout << "lambda: privide ";
		for(const ValPtr& p : params)
			std::cout << to_string(p) << " ";
		std::cout << std::endl;
		throw Error("lambda: param number not match");
	}
	SymbolLayer layer;
	for(const auto& l : capture_list_)
		Parser::symbol_table.add(l.first, l.second);
	for(size_t i = 0; i < params.size(); ++i)
		Parser::symbol_table.add(param_list_[i], params[i]);
	return eval(abstract_syntax_tree_, true);				
}
