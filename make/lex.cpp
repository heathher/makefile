#include "make.h"
extern int col;
extern int lin;

char* insert (char *str, char c)
{
	char *tmp = new char [strlen(str)+2];
	strcpy(tmp, str);

	tmp[strlen(str)] = c;
	tmp[strlen(str)+1] = '\0';
	delete[] str;
	str = tmp;
	return str;
}

Lex get_lex (FILE *f)
{
	int c = fgetc(f);
	//std::cout << c << std::endl;
	type_of_lex CS = H;
	do {
		int count = 1;
		int lcount = 0;
		std::string characters(" \n\t\r:=$()");
		switch(CS)
		{
			case H:
			{
				switch(c)
				{
					case '#':      CS = COMMENT; break;
					case '\n':     CS = NEWLINE; break;
					case ' ':      CS = WHITESPACE; break;
					case '\r':     CS = WHITESPACE; break;
					case EOF:      CS = EOFILE; break;
					case '\t':     CS = TAB; break;
					case '$':      CS = DOLLAR; break;
					case ':':      CS = COLUMN; break;
					
					default: 
					if ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || (c=='-') || (c=='.') || (c == '*') || (c == '%') || (c == ':') || (c == '"'))
						CS = NAME;
					else if (c == '\\')
					{
						if ((c = fgetc(f)) != '\n')
						{
							count++;
						}
						else
						{ 
							lcount++;
							col = 1;
							count = 0;
						}
						c = fgetc(f);
						count++;
						CS = H;
					} 
					else CS = ERROR; 
				}
			} break;

			case COMMENT:
			{
				char* str = new char[2];
				str[0] = '#';
				str[1] = '\0';
				while ((c=fgetc(f)) != '\n' && c != EOF)
				{
					if (c == '\\')
					{
						if ((c = fgetc(f)) != '\n')
						{
							count++;
						}
						else
						{ 
							lcount++;
							col = 1;
							count = 0;
						}
						c = fgetc(f);
						count++;
						if (c == '\n' || c == '#' || c == EOF)
							break;
					}
					str = insert(str,c);
					count++;
				}
				ungetc(c, f);
				Lex lexem = Lex(COMMENT, str, lin, col, lin, col+count);
				col = col + count;
				delete[] str;		
				return lexem;         
			}break;

			case COLUMN:
			{
				char *str = new char[2];
				str[0] = ':';
				str[1] = '\0';
				Lex lexem = Lex(COLUMN, str, lin, col, lin, col+1);
				col++;
				delete[] str;
				return lexem; 
			}
			case NEWLINE:
			{
				char* str = new char[1];
				str[0] = '\0';
				Lex lexem = Lex(NEWLINE, str, lin, col, lin+1, 1);
				lin++;
				col = 1;
				delete[] str;
				return lexem;
			}break;

			case WHITESPACE:
			{
				char *str = new char[2];
				str[0] = c;
				str[1] = '\0';
				while ((c = fgetc(f)) == ' ' || c == '\r')
				{
					str = insert(str,c);
					count++;
				}
				ungetc(c, f);
				Lex lexem = Lex(WHITESPACE, str, lin, col, lin, col+count);
				col = col + count;
				delete[]str;
				return lexem;
			} break;
			
			case TAB:
			{
				char *str = new char[2];
				str[0] = '\t';
				str[1] = '\0';
				count = count + 4;
				Lex lexem = Lex(WHITESPACE, str, lin, col, lin, col+4);
				col = col + 4;
				delete[]str;
				return lexem;
			} break;
			
			case DOLLAR:
			{
				c = fgetc(f);
				switch (c)
				{
					case '@': CS = DOLLAR_S; break;
					case '^': CS = DOLLAR_K; break;
					case '(': CS = DOLLAR_C; break;
					default: CS = DOLLAR_ERROR;
				} break;
			}break;

			case DOLLAR_S:
			{
				char* str = new char[3];
				//str[0] = '$';
				str[0] = '@';
				str[1] = '\0';
				Lex lexem = Lex(DOLLAR, str, lin, col, lin, col+2);
				col = col + 2;
				delete[]str;
				return lexem;
			}break;

			case DOLLAR_K:
			{
				char* str = new char[3];
				//str[0] = '$';
				str[0] = '^';
				str[1] = '\0';
				Lex lexem = Lex(DOLLAR, str, lin, col, lin, col+2);
				col = col + 2;
				delete[]str;
				return lexem;
			}break;
			
			case DOLLAR_C:
			{
				char* str = new char [1];
				str[0] = '\0';
				while ((c = fgetc(f)) != ')')
				{
					str = insert(str, c);
					count++;
				}
				Lex lexem = Lex(DOLLAR, str, lin, col, lin, col+count+2);
				col = col + count + 2;
				delete[]str;
				return lexem;
			}break;

			case NAME:
			{
				if (c == 'i')
				{
					char *str = new char [2];
					str[0] = 'i';
					str[1] = '\0';
					c = fgetc(f);
					for (int i = 0; i < 6; i++)
					{
						if (!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c<='0' && c>='9')))
							break;
						str = insert(str, c);
						count++;
						c = fgetc(f);
					}
					ungetc(c,f);
					if (!strcmp(str, "include"))
					{
						Lex lexem = Lex(INCLUDE_KEYWORD, str, lin, col, lin, col+count);
						col = col + count;
						delete[]str;
						return lexem;
					}
					else 
					{
						c = fgetc(f);
						while(characters.find(c) == std::string::npos && c != EOF)
						{
							if (c == '\\')
							{
								if ((c = fgetc(f)) != '\n')
								{
									count++;
								}
								else
								{ 
									lcount++;
									col = 1;
									count = 0;
								}
								c = fgetc(f);
								count++;
								if (characters.find(c)!= std::string::npos)
									break;
							}
							str = insert(str,c);
							count++;
							c = fgetc(f);
						}
						Lex lexem;
						if (c != '=')
						{
							lexem = Lex(VARIABLE, str, lin, col, lin+lcount, col+count);
							ungetc(c,f);
						}
						else 
						{
							lexem = Lex(DECL, str, lin, col, lin+lcount, col+count);
						}
						col = col + count;
						lin = lin + lcount;
						delete[]str;
						return lexem;
					}
				}
				else 
				{
					char *str = new char [2];
					str[0] = c;
					str[1] = '\0';
					c = fgetc(f);
					count++;
					while(characters.find(c) == std::string::npos && c != EOF)
					{
						if (c == '\\')
						{
							if ((c = fgetc(f)) != '\n')
							{
								count++;
							}
							else
							{ 
								lcount++;
								col = 1;
								count = 0;
							}
							c = fgetc(f);
							count++;
							if (characters.find(c)!= std::string::npos)
							break;
						}
						str = insert(str,c);
						count++;
						c = fgetc(f);
						
					}
					Lex lexem;
					if (c != '=')
					{
						lexem = Lex(VARIABLE, str, lin, col, lin+lcount, col+count);
						ungetc(c,f);
					}
					else 
					{
						lexem = Lex(DECL, str, lin, col, lin+lcount, col+count);
					}
					col = col + count;
					lin = lin + lcount;
					delete[]str;
					return lexem;
				}
			}

			case DOLLAR_ERROR:
			{
				throw Exceptions("Error in $: unexpected token\n", lin, col);
			}break;

			case EOFILE:
			{
				char* str = new char[1];
				str[0] = '\0';
				Lex lexem = Lex(EOFILE, str, lin, col, lin, col);
				delete[]str;
				return lexem;
			}break;

			case ERROR:
			{
				throw Exceptions("Error: unexpected token\n", lin, col);
			} break;

			case INCLUDE_KEYWORD:
			{

			}break;

			case VARIABLE:
			{

			}break;

			case DECL:
			{

			}break;

		}
	} while (true);
}
