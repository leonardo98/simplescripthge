#include "LuaScript.h"

LuaScript::LuaScript(lua_State *L)
	: code(NULL)
{
	l = L;
}

LuaScript::LuaScript(lua_State *L, TiXmlElement *script)
	: code(NULL)
{
	l = L;
	if (script != NULL) {
		TiXmlText *text = (TiXmlText *)script->FirstChild();
		if (text != NULL) {
			unsigned int size = strlen((char *)text->Value());
			code = new char[size + 1];
			strcpy(code, (char *)text->Value());
		}
	} 
}

void LuaScript::Read(char *text)
{
	if (text != NULL) {
		unsigned int size = strlen(text);
		code = new char[size + 1];
		strcpy(code, text);
	}
}

LuaScript::~LuaScript(void)
{
	if (code != NULL) {
		delete code;
		code = NULL;
	}
}

static int report (lua_State *l, int status) {
  if (status) {
	  const char *msg;
	  msg = lua_tostring(l, -1);
		if (msg == NULL) msg = "(error with no message)";
		MessageBox(0, msg, "Error!", MB_OK);
    //lua_pop(l, 1);
  }
  return status;
}

void LuaScript::Execute() 
{
	if (code == NULL) {
		return;
	}
	int lua_dostring_result = lua_dostring(l, code);
	report(l, lua_dostring_result);
//	assert(lua_dostring_result == 0);
}
