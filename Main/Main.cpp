#include <iostream>
#include <vector>
#include <string>
import Program;

int main(int argc, char* argv[])
{
	try {
		Program program;

		std::vector<std::string> args;
		args.reserve(argc);
		for (int i = 0; i < argc; ++i)
		{
			args.emplace_back(argv[i]);
		}

		program.Run(args);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
