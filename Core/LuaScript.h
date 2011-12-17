#ifndef MYENGINE_LUASCRIPT_H
#define MYENGINE_LUASCRIPT_H

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
	static int report(lua_State *l, int status);
private:
	lua_State *l;
	char *code;
	string scriptfile;
};

#endif//MYENGINE_LUASCRIPT_H