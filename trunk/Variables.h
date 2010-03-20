// Variables.h: interface for the Variables class.

#if !defined(_VARIABLES_INCLUDED_)
#define _VARIABLES_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Variable.h"

class Variables  
{
public:
	static void Set(std::string variableName, std::string value);
	static std::string Get(std::string variableName);
	static Variable::Ptr GetLink(std::string variableName);
	static void Remove(std::string variableName);
private:
	static std::map<std::string, Variable> _map;
};

#endif // !defined(_VARIABLES_INCLUDED_)
