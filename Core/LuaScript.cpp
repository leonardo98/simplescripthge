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
		if(script->QueryStringAttribute("scriptfile", &scriptfile)==TIXML_NO_ATTRIBUTE)
		{
			TiXmlText *text = (TiXmlText *)script->FirstChild();
			if (text != NULL) {
				unsigned int size = strlen((char *)text->Value());
				code = new char[size + 1];
				strcpy(code, (char *)text->Value());
			}
		}
	} 
}

//Сохраняем совместимость со старыми версиями :)
void LuaScript::Read(char *text)
{
	if (text != NULL) {
		unsigned int size = strlen(text);
		code = new char[size + 1];
		strcpy(code, text);
	}
}

//Юзать если хотим считывать как с отдельного файла так и с ХМЛ
void LuaScript::Read(TiXmlElement *script)
{
	if (script != NULL) {
		scriptfile.clear();
		if(script->QueryStringAttribute("scriptfile", &scriptfile)==TIXML_NO_ATTRIBUTE)
		{
			TiXmlText *text = (TiXmlText *)script->FirstChild();
			if (text != NULL) {
				unsigned int size = strlen((char *)text->Value());
				//В старом варианте функции проверки нет, но будет лик если во время жизни обьекта
				//будут вызывать эту функцию более 1го раза (или даже если 1 раз при исплользовании 2го конструктора)
				if(code!=NULL) delete code;
				code = new char[size + 1];
				strcpy(code, (char *)text->Value());
			}
		}
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
	if (status == LUA_ERRRUN) {
		MessageBox(0, "ошибка при управлении chunk", "Error!", MB_OK);
	} else if (status == LUA_ERRSYNTAX) {
		MessageBox(0, "ошибка синтаксиса в течение прекомпиляции", "Error!", MB_OK);
	} else if (status == LUA_ERRMEM) {
		MessageBox(0, "ошибка распределения памяти", "Error!", MB_OK);
	} else if (status == LUA_ERRERR) {
		MessageBox(0, "ошибка при управлении _ERRORMESSAGE", "Error!", MB_OK);
	} else if (status == LUA_ERRFILE) {
		MessageBox(0, "ошибка открытия файла", "Error!", MB_OK);
	} else if (status) {
		const char *msg;
		msg = lua_tostring(l, -1);
			if (msg == NULL) msg = "(error with no message)";
		MessageBox(0, msg, "Error!", MB_OK);
	}
	return status;
}

void LuaScript::Execute() 
{
	int lua_dostring_result;

	//Выполняем с файла?
	if(!scriptfile.empty())
	{
		lua_dostring_result = luaL_dofile(l, scriptfile.c_str());
		report(l, lua_dostring_result);
		return;
	}

	//Есть ли в ХМЛ вообще код для скрипта?
	if (code == NULL) {
		return;
	}

	//Выполняем строку из ХМЛ
	lua_dostring_result = luaL_dostring(l, code);
	report(l, lua_dostring_result);
}
