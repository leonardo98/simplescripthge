// Lexem.h: interface for the Lexems class.

#if !defined(_LEXEM_INCLUDED_)
#define _LEXEM_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"


static enum LEXEM_TYPES
{
	ltResWord = 0, //�������� (�����������������) �����
		// if, else, send to, {, }
	ltOperator,    //��������
		// =, ==, !=, (, ), and
	ltStrConstant, //��������� ���������
		// "hello"
	ltVariable,    //����������
		// $variableName
	ltDevider,     //�����������
		// ;
	ltIntConstant, //�������� ���������
	ltUnknown      //��� �� ���������, ���� �� ��������
};

class Lexem  
{

private:

    LEXEM_TYPES  _type;//��� �������
	std::string         _text;//����� �������
 
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
