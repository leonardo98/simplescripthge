#include "LuaScript.h"
#include "Render.h"

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
		const char *a = script->Attribute("scriptfile");
		if (a) {
			scriptfile = a;
		} else {
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
		const char *a = script->Attribute("scriptfile");
		if (a) {
			scriptfile = a;
		} else {
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

int LuaScript::report(lua_State *l, int status) {
	const char *msg;
	msg = lua_tostring(l, -1);
	if (status == LUA_ERRRUN) {
		Render::ShowMessage("ошибка при управлении chunk", "Error!");
	} else if (status == LUA_ERRSYNTAX) {
		Render::ShowMessage("ошибка синтаксиса в течение прекомпиляции", "Error!");
	} else if (status == LUA_ERRMEM) {
		Render::ShowMessage("ошибка распределения памяти", "Error!");
	} else if (status == LUA_ERRERR) {
		Render::ShowMessage("ошибка при управлении _ERRORMESSAGE", "Error!");
	} else if (status == LUA_ERRFILE) {
		Render::ShowMessage("ошибка открытия файла", "Error!");
	} else if (status) {
			if (msg == NULL) msg = "(error with no message)";
		Render::ShowMessage(msg, "Error!");
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
