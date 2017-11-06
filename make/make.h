#ifndef MAKE_H
#define MAKE_H

#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <set>
#include <map>
#include <list>
extern int lin;
extern int col;

enum type_of_lex 
{
	H,                  
	ERROR,              //error type
	COMMENT,
	NEWLINE,            // \n
	WHITESPACE,
	EOFILE,
	TAB,                // \t             
	DOLLAR,             // $
	DOLLAR_S,
	DOLLAR_C,
	DOLLAR_K,
	DOLLAR_ERROR,
	COLUMN,
	NAME,
	INCLUDE_KEYWORD,
	VARIABLE,
	DECL
};

class Lex  {
	type_of_lex type;
	std::string buf;
	int line1, line2;
	int column1, column2;
public:
	Lex() {}
	Lex (type_of_lex t, const std::string &str, int l1, int c1, int l2, int c2)
	{
		type = t;
		buf = str;
		line1 = l1;
		line2 = l2;
		column1 = c1;
		column2 = c2;
	}
	
	~Lex()
	{
		
	}
	void print()
	{
		std::cout << line1 << "." << column1 << " | " << line2 << "." << column2 << " | ";
		switch(type)
		{
			case 0: std::cout << "H" ; break;
			case 1: std::cout << "ERROR"; break;
			case 2: std::cout << "COMMENT"; break;
			case 3: std::cout << "NEWLINE"; break;
			case 4: std::cout << "WHITESPACE" ; break;
			case 5: std::cout << "EOFILE" ; break;
			case 6: std::cout << "TAB" ; break;
			case 7: std::cout << "DOLLAR"; break;
			case 8: std::cout << "DOLLAR_S"; break;
			case 9: std::cout << "DOLLAR_C"; break;
			case 10:std::cout << "DOLLAR_K" ; break;
			case 11:std::cout << "DOLLAR_ERROR" ; break;
			case 12:std::cout << "COLUMN"; break;
			case 13:std::cout << "NAME" ; break;
			case 14:std::cout << "INCLUDE_KEYWORD"; break;
			case 15:std::cout << "VARIABLE" ; break;
			case 16:std::cout << "DECL" ; break;

		}
		std::cout << " | ";
		std::cout << buf << std::endl;
		//std::cout << "first line: " << line1 <<", first column: " << column1 << std::endl;
		//std::cout << "last line: " << line2 <<", last column: " << column2 << std::endl;
	}
	type_of_lex get_type() const
	{
		return type;
	}
	std::string get_buf() const
	{
		std::string str = buf;
		return str;
	}
	int get_col() const
	{
		return column1;
	}
	int get_lin() const 
	{
		return line1;
	}
	friend Lex get_lex(FILE*);
};

Lex get_lex(FILE*);

class Exceptions {
	char * str;
	int length;
	int line, column;
public: 
	Exceptions(const char *s, int l, int c) 
	{
		line = l;
		column = c;
		length = strlen(s); 
		str = new char [length+1]; 
		strcpy(str, s);
	}
	Exceptions(const Exceptions &ex) 
	{
		line = ex.line;
		column = ex.column;
		length = ex.length; 
		str = new char [length+1]; 
		strcpy(str, ex.str);
	}
	~Exceptions() 
	{
		delete[] str;
	}
	void getex()
	{
		std::cout << str  << line << " | " << column << std::endl;
	};

};

class LexAnalyzer {
public:
	LexAnalyzer(const std::string &filename)
	{
		f = fopen(filename.c_str(),"r");
		if (f == NULL)
			throw Exceptions("File cannot open\n", lin, col);
	}
	~LexAnalyzer()
	{
		if (f != NULL)
			fclose(f);
	}
	Lex getNextLexeme()
	{
		Lex lexem = get_lex(f);
		if (lexem.get_type() == COMMENT)
		{
			lexem = get_lex(f);
			if (lexem.get_type() == NEWLINE)
				lexem = get_lex(f);
		}
		return lexem;
	}
private:
	FILE *f;
};

#endif