// Parser.h: interface for the Parser class.

#if !defined(_PARSER_INCLUDED_)
#define _PARSER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OperatorsReader.h"

class Parser  
	: public OperatorsReader
{
public:
	Parser();
	Parser(TiXmlElement *xe);
	void Read(char *text);
	void Read(std::string fileName);
	void Execute();
private:
};

#endif // !defined(_PARSER_INCLUDED_)
