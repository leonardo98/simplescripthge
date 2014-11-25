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

//��������� ������������� �� ������� �������� :)
void LuaScript::Read(char *text)
{
	if (text != NULL) {
		unsigned int size = strlen(text);
		code = new char[size + 1];
		strcpy(code, text);
	}
}

//����� ���� ����� ��������� ��� � ���������� ����� ��� � � ���
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
				//� ������ �������� ������� �������� ���, �� ����� ��� ���� �� ����� ����� �������
				//����� �������� ��� ������� ����� 1�� ���� (��� ���� ���� 1 ��� ��� �������������� 2�� ������������)
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
		Render::ShowMessage("������ ��� ���������� chunk", "Error!");
	} else if (status == LUA_ERRSYNTAX) {
		Render::ShowMessage("������ ���������� � ������� �������������", "Error!");
	} else if (status == LUA_ERRMEM) {
		Render::ShowMessage("������ ������������� ������", "Error!");
	} else if (status == LUA_ERRERR) {
		Render::ShowMessage("������ ��� ���������� _ERRORMESSAGE", "Error!");
	} else if (status == LUA_ERRFILE) {
		Render::ShowMessage("������ �������� �����", "Error!");
	} else if (status) {
			if (msg == NULL) msg = "(error with no message)";
		Render::ShowMessage(msg, "Error!");
	}
	return status;
}

void LuaScript::Execute() 
{
	int lua_dostring_result;

	//��������� � �����?
	if(!scriptfile.empty())
	{
		lua_dostring_result = luaL_dofile(l, scriptfile.c_str());
		report(l, lua_dostring_result);
		return;
	}

	//���� �� � ��� ������ ��� ��� �������?
	if (code == NULL) {
		return;
	}

	//��������� ������ �� ���
	lua_dostring_result = luaL_dostring(l, code);
	report(l, lua_dostring_result);
}
