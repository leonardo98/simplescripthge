// Variable.h: interface for the Variable class.

#if !defined(_VARIABLE_INCLUDED_)
#define _VARIABLE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

class Variable  
{
public:
	typedef Variable * Ptr;
	void SetValue(std::string value);
	std::string GetValue();
	Ptr GetAdress();
private:
	std::string _value;
};


#endif // !defined(_VARIABLE_INCLUDED_)
