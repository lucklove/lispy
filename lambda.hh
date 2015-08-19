#pragma once
#include "val.hh"

class Lambda {
public:
	Lambda(std::vector<std::string> param_list,
		std::vector<std::pair<std::string, ValPtr>> capture_list,
		ast_t abstract_syntax_tree) 
	: param_list_(param_list), capture_list_(capture_list), abstract_syntax_tree_(abstract_syntax_tree)
	{}

	ValPtr apply(ValPtr self, const std::vector<ValPtr>& params); 	
private:
	std::vector<std::string> param_list_;	
	std::vector<std::pair<std::string, ValPtr>> capture_list_;
	ast_t abstract_syntax_tree_;
};
