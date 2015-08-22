#include <iostream>
#include <map>

using namespace std;
#define STATE_NUM 14
#define CASE_NUM 128

using state_t = unsigned short;
using case_t = unsigned char;
case_t index_of(char c) {
	if(c > 0) return c;
	return 0;
}

int main()
{
	map<state_t, map<case_t, state_t>> lmap;

	for(size_t i = 0; i < STATE_NUM; ++i) {
		for(size_t j = 0; j < CASE_NUM; ++j)
			lmap[i][j] = 0;
	}

	#define T(From, Case, To)		\
	do {					\
		lmap[From][index_of(Case)] = To;	\
	} while(0)

	std::string alpha_symbol("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_+-*/\\=<>!&");
	std::string alpha_number_symbol("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_+-*/\\=<>!&");
	std::string number("0123456789");

	for(char c : alpha_symbol) {
		T(1, c, 2);
		T(3, c, 2);
	}
	for(char c : alpha_number_symbol)
		T(2, c, 2);
	for(char c : number) {
		T(1, c, 3);
		T(3, c, 3);
		T(4, c, 4);
	}
	T(3, '.', 4);
	T(1, '"', 5);
	for(unsigned char c = 0; c < CASE_NUM; ++c) {
		T(5, c, 5);
		T(6, c, 5);
		T(8, c, 8);
	}
	T(5, '\\', 6);
	T(5, '"', 7);
	T(1, ';', 8);
	T(8, '\n', 9);
	T(1, ' ', 9);
	T(1, '\t', 9);
	T(1, '\n', 9);
	T(1, ')', 10);
	T(1, '(', 11);
	T(1, '}', 12);
	T(1, '{', 13);

	std::cout << "{" << std::endl;
	for(size_t i = 0; i < STATE_NUM; ++i) {
		std::cout << "\t{" << std::endl;
		std::cout << "\t\t";
		for(size_t j = 0; j < lmap[i].size() / 4 * 1; ++j) {
			std::cout << lmap[i][j];
			std::cout << ",";
		}
		std::cout << std::endl << "\t\t";
		for(size_t j = lmap[i].size() / 4 * 1; j < lmap[i].size() / 4 * 2; ++j) {
			std::cout << lmap[i][j];
			std::cout << ",";
		}
		std::cout << std::endl << "\t\t";
		for(size_t j = lmap[i].size() / 4 * 2; j < lmap[i].size() / 4 * 3; ++j) {
			std::cout << lmap[i][j];
			std::cout << ",";
		}
		std::cout << std::endl << "\t\t";
		for(size_t j = lmap[i].size() / 4 * 3; j < lmap[i].size() / 4 * 4; ++j) {
			std::cout << lmap[i][j];
			if(j != CASE_NUM - 1)
				std::cout << ",";
		}
		std::cout << std::endl;
		std::cout << "\t}";
		if(i != STATE_NUM - 1)
			std::cout << ",";
		std::cout << std::endl;
	}
	std::cout << "}" << std::endl;
}
