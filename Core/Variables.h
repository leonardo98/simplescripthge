// Variables.h: interface for the Variables class.

#ifndef VARIABLES_H
#define VARIABLES_H

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
	friend class IPanel;
	friend class TestObject;
};

#endif//VARIABLES_H