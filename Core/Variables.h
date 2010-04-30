// Variables.h: interface for the Variables class.

#if !defined(_VARIABLES_INCLUDED_)
#define _VARIABLES_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

class Variables  
{
public:
	static void Init(lua_State *L);
	static void Set(const char *variableName, const char *value);
	static std::string Get(const char *variableName);
private:
	static lua_State *l;
	friend class Mask;
	friend class MultiTexture;
	friend class Button;
	friend class IButton;
	friend class TestObject;
};

#endif // !defined(_VARIABLES_INCLUDED_)
