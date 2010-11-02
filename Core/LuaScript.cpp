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
		if(script->QueryStringAttribute("scriptfile", &scriptfile)==TIXML_NO_ATTRIBUTE)
		{
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

static int report (lua_State *l, int status) {
	if (status == LUA_ERRRUN) {
		MessageBox(0, "������ ��� ���������� chunk", "Error!", MB_OK);
	} else if (status == LUA_ERRSYNTAX) {
		MessageBox(0, "������ ���������� � ������� �������������", "Error!", MB_OK);
	} else if (status == LUA_ERRMEM) {
		MessageBox(0, "������ ������������� ������", "Error!", MB_OK);
	} else if (status == LUA_ERRERR) {
		MessageBox(0, "������ ��� ���������� _ERRORMESSAGE", "Error!", MB_OK);
	} else if (status == LUA_ERRFILE) {
		MessageBox(0, "������ �������� �����", "Error!", MB_OK);
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
