#pragma once

#include "lispy.hh"
#include <unordered_map>

struct symbol_node {
	std::unordered_map<std::string, ValPtr> table;
	symbol_node *prev;
	void *operator new(size_t count) {
		return allocator.allocate(count);
	}
	void operator delete(void *ptr, size_t count) {
		allocator.deallocate(static_cast<symbol_node *>(ptr), count);
	}
private:
	static std::allocator<symbol_node> allocator;
};

class SymbolTable {
public:
	SymbolTable() {
		point_ = new symbol_node();
		point_ -> prev = nullptr;
		root_ = point_;
	}
	~SymbolTable() {
		while(point_)
			pop();
	}
	ValPtr look(const std::string& name) {
		symbol_node *node = point_;
		while(node) {
			auto dest = node->table.find(name);
			if(dest != node->table.end())
				return dest->second;
			node = node->prev;
		}
		return nullptr;
	}
	void push() {
		symbol_node *node = new symbol_node();
		node->prev = point_;
		point_ = node;	
	}
	void pop() {
		symbol_node *node = point_;
		point_ = node->prev;
		delete node;
	}
	void add(const std::string& name, ValPtr val) {
		point_->table[name] = val;
	}
	void add_global(const std::string& name, ValPtr val) {
		root_->table[name] = val;
	}
	bool is_top() {
		return root_ == point_;
	}
	/** DEBUG */
	void dump() {
		symbol_node *node = point_;
		std::cout << "============symbol table===========" << std::endl;
		while(node) {
			std::cout << "------------------------------" << std::endl;
			for(auto&& it : node->table)
				std::cout << it.first << std::endl;
			node = node->prev;
		}
	}
private:
	symbol_node *root_;
	symbol_node *point_;
};
