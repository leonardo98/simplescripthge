#pragma once

#include "types.h"
#include <string>
using std::string;

class LuaScript
{
public:
	LuaScript(lua_State *L);
	LuaScript(lua_State *L, TiXmlElement *xe);
	~LuaScript();
	void Read(char *text);
	void Read(TiXmlElement *script);
	void Execute();
private:
	lua_State *l;
	char *code;
	string scriptfile;
};
