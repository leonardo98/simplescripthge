// Lexem.h: interface for the Lexems class.

#if !defined(_LEXEM_INCLUDED_)
#define _LEXEM_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"


static enum LEXEM_TYPES
{
	ltResWord = 0, //ключевое (зарезервированное) слово
		// if, else, send to, {, }
	ltOperator,    //оператор
		// =, ==, !=, (, ), and
	ltStrConstant, //строковая константа
		// "hello"
	ltVariable,    //переменная
		// $variableName
	ltDevider,     //разделитель
		// ;
	ltIntConstant, //числовая константа
	ltUnknown      //тип не определен, либо не известен
};

class Lexem  
{

private:

    LEXEM_TYPES  _type;//тип лексемы
	std::string         _text;//текст лексемы
 
public:
 
    LEXEM_TYPES Type(void)const {return _type;}
	std::string Text(void)const {return _text;}

    Lexem(void);
    Lexem(const Lexem&);
    Lexem(LEXEM_TYPES, const std::string&);
	Lexem& operator=(const Lexem& other);
	bool operator==(const Lexem& other);
 
};

#endif // !defined(_LEXEM_INCLUDED_)
