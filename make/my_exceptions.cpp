#include "my_exceptions.h"

std::ostream& operator <<(std::ostream &str, const Exception &ex)
{
	switch (ex.m_type)
	{
		case Exception::CANNOT_OPEN_FILE:
			str<<"Wrong file name.";
			break;
		case Exception::WRONG_ARGUMENTS:
			str<<"Wrong command line arguments. Usage: ./make.out [-f file_name] [rule_name].";
			break;
		case Exception::LEXIC_ANALYZER_ERROR:
			str<<"Lexical error.";
			break;
		case Exception::SYNTAX_ANALYZER_ERROR:
			str<<"Syntax error.";
			break;
		case Exception::RUNTIME_ERROR:
			str<<"Runtime error.";
			break;
		case Exception::SEMANTIC_ERROR:
			str<<"Semantic error.";
			break;
		default:
			str<<"An unknown exception caught.";
			break;
	}
	if (ex.m_code != 0)
		str<<"Error code: "<<ex.m_code;
	str<<std::endl;
	return str;
}

Exception::Exception(ExType t, int code) : m_type(t), m_code(code)
{

}

Exception::~Exception()
{

}

Exception::ExType Exception::getType() const
{
	return m_type;
}

int Exception::getCode() const
{
	return m_code;
}