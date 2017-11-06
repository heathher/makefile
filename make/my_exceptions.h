#ifndef MY_EXCEPTIONS_H
#define MY_EXCEPTIONS_H

#include <ostream>

class Exception
{
public:
	enum ExType
	{
		UNKNOWN = 0,
		CANNOT_OPEN_FILE,
		WRONG_ARGUMENTS,
		LEXIC_ANALYZER_ERROR,
		SYNTAX_ANALYZER_ERROR,
		SEMANTIC_ERROR,
		RUNTIME_ERROR
	};
	friend std::ostream& operator <<(std::ostream &str, const Exception &ex);
	Exception(ExType t = UNKNOWN, int code = 0);
	~Exception();
	ExType getType() const;
	int getCode() const;
private:
	ExType m_type;
	int m_code;
};

#endif