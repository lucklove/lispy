#include "builtin.hh"

#include "lispy.hh"
#include <fstream>

int main(int argc, char *argv[])
{
	if(argc > 1) {
		for(int i = 1; i < argc; ++i) {
			Parser p;
			std::ifstream file(argv[i]);
			if(file)
				p.parse(file);
		}
	} else {
		Parser p;
		p.parse(std::cin);
	}
}
