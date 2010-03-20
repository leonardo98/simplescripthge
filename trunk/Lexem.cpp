// Lexem.cpp: implementation of the Lexems class.

#include "types.h"
#include "Lexem.h"


Lexem::Lexem(void)
	: _type(ltUnknown)
	, _text("unknown")
{}
 
Lexem::Lexem(const Lexem& other)
	: _type(other._type)
	, _text(other._text) 
{}
 
Lexem::Lexem(LEXEM_TYPES type, const std::string& text)
	: _type(type)
	, _text(text) 
{}

Lexem& Lexem::operator=(const Lexem& other)
{
    _type = other._type;
    _text = other._text;
    return *this;
}
 
bool Lexem::operator==(const Lexem& other)
{
	return (_type == other._type && _text == other._text);
}