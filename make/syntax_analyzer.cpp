#include "syntax_analyzer.h"
#include "my_exceptions.h"
#include <iostream>
#include <iomanip>

unsigned countWords(const std::string &str);
std::string getFirtsWord(const std::string &str);
std::string getWorkingDirectory();

SyntaxAnalyzer::SyntaxAnalyzer(LexAnalyzer &lex) : m_lex(lex)
{
	
}

SyntaxAnalyzer::~SyntaxAnalyzer()
{

}

void SyntaxAnalyzer::expandDependencyTree(DependencyTree &tree)
{
	while (true)
	{
		Lex c_lex = m_lex.getNextLexeme();
		if (c_lex.get_type() == EOFILE)
			break;
		initial(tree, c_lex);
	}
	tree.buildTemplates(m_templates);
}

void SyntaxAnalyzer::includeFile(const std::string &filename)
{
	if (m_included_files.find(filename) == m_included_files.end())
		m_included_files.insert(filename);
}

const SyntaxAnalyzer& SyntaxAnalyzer::operator = (const SyntaxAnalyzer &another)
{
	m_templates = another.m_templates;
	m_included_files = another.m_included_files;
	m_variables = another.m_variables;
	return *this;
}

const std::string& SyntaxAnalyzer::getVar(const std::string &var)
{
	std::map<std::string, std::string>::const_iterator it = m_variables.find(var);
	if (it == m_variables.end())
		error(m_lex.getNextLexeme().get_lin(), m_lex.getNextLexeme().get_col(), "variable \"" + var + "\" is undefined");
	return it->second;
}

void SyntaxAnalyzer::addVar(const std::string &var, const std::string &value)
{
	std::map<std::string, std::string>::iterator it = m_variables.find(var);
	if (it != m_variables.end())
		error(m_lex.getNextLexeme().get_lin(), m_lex.getNextLexeme().get_col(), "redefinition of a variable \"" + var + "\"");
	m_variables.insert(std::pair<std::string, std::string>(var, value));
}

void SyntaxAnalyzer::removeVar(const std::string &var)
{
	std::map<std::string, std::string>::iterator it = m_variables.find(var);
	if (it != m_variables.end())
		m_variables.erase(it);
}

std::string SyntaxAnalyzer::toString(const Lex &lex)
{
	if (lex.get_type() == DOLLAR)
		return getVar(lex.get_buf());
	if (lex.get_type() == DECL)
		return (lex.get_buf() + std::string("="));
	return lex.get_buf();
}

void SyntaxAnalyzer::error(int line, int col, const std::string &msg, int errcode) const
{
	std::cerr<<"Line "<<line<<", column "<<col<<": "<<msg<<std::endl;
	throw Exception(Exception::SYNTAX_ANALYZER_ERROR, errcode);
}

void SyntaxAnalyzer::initial(DependencyTree &tree, const Lex &lex)
{
	switch (lex.get_type())
	{
		case NEWLINE:
			return;
		case COMMENT:
			return;
		case INCLUDE_KEYWORD:
			include(tree, lex); break;
		case WHITESPACE:
			if ((lex.get_buf()[0] == '\t') && (lex.get_lin() == 1))
				error(lex.get_lin(), lex.get_col(), "action outside of a rule");
			break;
		case DECL:
			variable(lex); break;
		case VARIABLE:
			if (lex.get_buf().find('%') == std::string::npos)
				rule(tree, lex);
			else
				templ(tree, lex);
			break;
		case EOFILE:
			break;
		default:
			error(lex.get_lin(), lex.get_col(), "unexpected lexeme");
			break;
	}
}

void SyntaxAnalyzer::rule(DependencyTree &tree, const Lex &lex)
{
	addVar("@", lex.get_buf());
	Lex column = m_lex.getNextLexeme();
	if (column.get_type() == WHITESPACE)
		column = m_lex.getNextLexeme();
	if (column.get_type() != COLUMN)
		error(lex.get_lin(), lex.get_col(), "cannot find column after rule declaration");
	DependencyTree::Node cur_node(lex.get_buf());
	std::string deps = lineOfText();
	cur_node.addDependency(deps);
	addVar("^", deps);
	Lex v_lex = m_lex.getNextLexeme();
	while ((v_lex.get_type() == WHITESPACE) && (v_lex.get_buf()[0] == '\t'))
	{
		cur_node.addAction(lineOfText());
		v_lex = m_lex.getNextLexeme();
	}
	tree.insertNode(cur_node);
	removeVar("@");
	removeVar("^");
	initial(tree, v_lex);
}

void SyntaxAnalyzer::templ(DependencyTree &tree, const Lex &lex)
{
	addVar("@", lex.get_buf());
	Lex column = m_lex.getNextLexeme();
	if (column.get_type() == WHITESPACE)
		column = m_lex.getNextLexeme();
	if (column.get_type() != COLUMN)
		error(lex.get_lin(), lex.get_col(), "cannot find column after template declaration");
	TemplateBuilder::Node cur_node(lex.get_buf());
	std::string deps = lineOfText();
	cur_node.addDependency(deps);
	addVar("^", deps);
	Lex v_lex = m_lex.getNextLexeme();
	while ((v_lex.get_type() == WHITESPACE) && (v_lex.get_buf()[0] == '\t'))
	{
		cur_node.addAction(lineOfText());
		v_lex = m_lex.getNextLexeme();
	}
	m_templates.addTemplate(cur_node);
	removeVar("@");
	removeVar("^");
	initial(tree, v_lex);
}

void SyntaxAnalyzer::variable(const Lex &lex)
{
	addVar(lex.get_buf(), lineOfText());
}

void SyntaxAnalyzer::include(DependencyTree &tree, const Lex &lex)
{
	Lex n_lex = m_lex.getNextLexeme();
	std::string filename = lineOfText();
	if (countWords(filename) != 1)
		error(lex.get_lin(), lex.get_col(), "only one filename can be placed after \"include\"");
	if (m_included_files.find(filename) != m_included_files.end())
	{
		std::cerr<<"Line "<<lex.get_lin()<<", column "<<lex.get_col()<<": "<<"file "<<filename<<" is already included"<<std::endl;
		throw Exception(Exception::SEMANTIC_ERROR);
	}
	try
	{
		m_included_files.insert(filename);
		LexAnalyzer lex_an(filename);
		SyntaxAnalyzer syn_an(lex_an);
		syn_an = *this;
		syn_an.expandDependencyTree(tree);
		*this = syn_an;
	}
	catch(...)
	{
		std::cerr << "In file : \" " << filename << "\" error" << std::endl;
		throw ;
	}
}

std::string SyntaxAnalyzer::lineOfText()
{
	std::string result("");
	Lex lex = m_lex.getNextLexeme();
	if ((lex.get_type() != NEWLINE) && (lex.get_type() != EOFILE))
		result += toString(lex) + lineOfText();
	return result;
}

void SyntaxAnalyzer::print() const
{
	std::cout<<"Variables ("<<m_variables.size()<<"): "<<std::endl<<std::endl;
	std::map<std::string, std::string>::const_iterator it = m_variables.begin();
	while (it != m_variables.end())
	{
		std::cout<<std::setw(16)<<(it->first)<<" = \""<<std::setw(0)<<(it->second)<<"\""<<std::endl;
		++it;
	}
	std::cout<<std::endl;
}
