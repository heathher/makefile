#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include <string>
#include <map>
#include <list>
#include <set>

#include "make.h"
#include "dependency_tree.h"
#include "template_builder.h"

class SyntaxAnalyzer
{
public:
	SyntaxAnalyzer(LexAnalyzer &lex);
	~SyntaxAnalyzer();
	void expandDependencyTree(DependencyTree &tree);
	void includeFile(const std::string &filename);
	const SyntaxAnalyzer& operator = (const SyntaxAnalyzer &another);

	void print() const;
private:
	LexAnalyzer &m_lex; 
	TemplateBuilder m_templates;
	std::set<std::string> m_included_files;
	std::map<std::string, std::string> m_variables;

	const std::string& getVar(const std::string &var);
	void addVar(const std::string &var, const std::string &value);
	void removeVar(const std::string &var);
	std::string toString(const Lex &lex);
	void error(int line, int col, const std::string &msg, int errcode = 0) const;

	void initial(DependencyTree &tree, const Lex &lex);
	void rule(DependencyTree &tree, const Lex &lex);
	void templ(DependencyTree &tree, const Lex &lex);
	void variable(const Lex &lex);
	void include(DependencyTree &tree, const Lex &lex);
	std::string lineOfText();
};

#endif