// Variables.cpp: implementation of the Variables class.

#include "Variables.h"

lua_State *Variables::l;

void Variables::Init(lua_State *L)
{
	l = L;
}

void Variables::Set(const char *variableName, const char *value)
{
#ifdef DEBUG
	int err = lua_gettop(l);
#endif
	lua_pushstring(l, variableName);
	lua_pushstring(l, value);
	lua_settable(l, LUA_GLOBALSINDEX);
	assert(err == lua_gettop(l));
}

std::string Variables::Get(const char *variableName)
{
#ifdef DEBUG
	int err = lua_gettop(l);
#endif
	lua_getglobal(l, variableName);
	const char *result = lua_tostring(l, -1);
	// NULL - ���������� �� ���� ���������� � lua �������, � �������� ��� ����������
	assert(result != NULL); 
	std::string value(result);
	lua_pop(l, 1);
	assert(err == lua_gettop(l));
	return value;
}