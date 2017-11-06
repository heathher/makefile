#include "make.h"
#include "dependency_tree.h"
#include "syntax_analyzer.h"
#include <cstdlib>
#include <iostream>

int lin = 1;
int col = 1;
FILE *f = NULL;
Lex get_lex(FILE*);

void getArguments(int argc, char* const *argv, std::string &filename, std::string &rule);

int count_words(const std::string &str)
{
	if (str == "")
		return 0;
	bool in_word = !isspace(str[0]);
	unsigned result = in_word?1:0;
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!in_word && !isspace(str[i]))
		{
			in_word = true;
			result++;
		}
		if (isspace(str[i]))
			in_word = false;
	}
	return result;
}


int main (int argc, char* const * argv)
{
	std::string filename, rule;
	getArguments(argc, argv, filename, rule);

	std::cout << "BEGINNING:..." << std::endl;

	try
	{

		LexAnalyzer lex_an(filename);
		SyntaxAnalyzer syn(lex_an);
		DependencyTree tree;
		syn.expandDependencyTree(tree);
	//	tree.print();
		tree.run(rule);
    }
    catch (Exceptions ex)
    {
    	ex.getex();
	std::cerr << "Error in file " << filename << std::endl;
    }
	catch (...)
{
	std::cerr << "Error in file " << filename << std::endl;
}

	//fclose(f);
	return 0;
}
