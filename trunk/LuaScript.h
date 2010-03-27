#pragma once

#include "types.h"

class LuaScript
{
public:
	LuaScript(lua_State *L);
	LuaScript(lua_State *L, TiXmlElement *xe);
	~LuaScript();
	void Read(char *text);
	void Execute();
private:
	lua_State *l;
	char *code;
};
