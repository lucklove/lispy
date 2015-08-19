#include "lispy.hh"

ValPtr Lambda::apply(ValPtr self, const std::vector<ValPtr>& params)
{
	if(params.size() != param_list_.size())
		throw Error("lambda: param number not match");
	SymbolLayer layer;
	for(const auto& l : capture_list_)
		Parser::symbol_table.add(l.first, l.second);
	Parser::symbol_table.add("self", self);
	for(size_t i = 0; i < params.size(); ++i)
		Parser::symbol_table.add(param_list_[i], params[i]);
	return eval(abstract_syntax_tree_, true);				
}
