// Variable.cpp: implementation of the Variable class.
#include "Variable.h"


void Variable::SetValue(std::string value)
{
	_value = value;
}

std::string Variable::GetValue()
{
	return _value;
}

Variable::Ptr Variable::GetAdress()
{
	return this;
}
