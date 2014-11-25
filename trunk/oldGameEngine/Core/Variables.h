// Variables.h: interface for the Variables class.

#ifndef MYENGINE_VARIABLES_H
#define MYENGINE_VARIABLES_H

#include "types.h"

class Variables  
{
public:
	static void Init(lua_State *L);
	static void Set(const char *variableName, const char *value);
	static void SetAsInt(const char *variableName, int value);
	static std::string Get(const char *variableName);
	static int GetAsInt(const char *variableName);
private:
	static lua_State *l;
	friend class Mask;
	friend class MultiTexture;
	friend class Button;
	friend class IButton;
	friend class IPanel;
	friend class TestObject;
};

#endif//MYENGINE_VARIABLES_H