#include <string>
#include <unistd.h>
#include "my_exceptions.h"

extern char *optarg;
extern int optind, opterr, optopt;

void getArguments(int argc, char* const *argv, std::string &filename, std::string &rule)
{
	filename = "Makefile";
	rule = "";
	int c = getopt(argc, argv, "f:");
	if (c == 'f')
	{
		if (optarg == NULL)
			throw Exception(Exception::WRONG_ARGUMENTS);
		filename = optarg;
		c = getopt(argc, argv, "f:");
		if (argv[optind] != NULL)
			rule = argv[optind];
	}
	else if (c == -1)
	{
		if (argv[optind] != NULL)
			rule = argv[optind];
	}
	else
		throw Exception(Exception::WRONG_ARGUMENTS);
}