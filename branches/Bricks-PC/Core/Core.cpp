// Core.cpp: implementation of the Lexems class.

#include "types.h"
#include "Core.h"
#include "Render.h"
#include "..\Gamee\ObjectFactory.h"
#include "Variables.h"

static int LuaSendMessage(lua_State *L) {
#ifdef IW_DEBUG
	int err = lua_gettop(L);
#endif
	const char *name = lua_tostring(L, 1);
	const char *message = lua_tostring(L, 2);
	Messager::SendMessage(name, message);
#ifdef IW_DEBUG
	assert(err == lua_gettop(L));
#endif
	return 0;
}

static int LuaSetNumberValue(lua_State *L) {
#ifdef IW_DEBUG
	int err = lua_gettop(L);
	assert(err == 0);
#endif
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	float value = static_cast<float>(lua_tonumber(L, 1));
	int result = Messager::SetValueF(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaSetStringValue(lua_State *L) {
#ifdef IW_DEBUG
	int err = lua_gettop(L);
	assert(err == 0);
#endif
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	std::string value = lua_tostring(L, 3);
	int result = Messager::SetValueS(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaSetBoolValue(lua_State *L) {
#ifdef IW_DEBUG
	int err = lua_gettop(L);
	assert(err != 0);
#endif
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	bool value = (lua_toboolean(L, 3) != 0);
	int result = Messager::SetValueB(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaGetBoolValue(lua_State *L) {
#ifdef IW_DEBUG
	int err = lua_gettop(L);
	assert(err == 0);
#endif
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	bool result = Messager::GetBoolValue(name, variableName);
	lua_pushboolean(L, result);
	return 1;
}

static int LuaGetNumberValue(lua_State *L) {
#ifdef IW_DEBUG
	int err = lua_gettop(L);
	assert(err == 0);
#endif
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	float result = Messager::GetNumberValue(name, variableName);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaGetStringValue(lua_State *L) {
#ifdef IW_DEBUG
	int err = lua_gettop(L);
	assert(err == 0);
#endif
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 1);
#ifdef IW_DEBUG
	const char *value = lua_tostring(L, 2);// не помню для чего это сделано - может это и лишнее????
	assert(value != 0);
#endif
	std::string result = Messager::GetValue(name, variableName);
	lua_pushstring(L, result.c_str());
	return 1;
}

Core::Core()
	: Messager("Core")
{
	lua = lua_open();
	lua_register(lua, "SendMessage", LuaSendMessage);
	lua_register(lua, "SetNumberValue", LuaSetNumberValue);
	lua_register(lua, "SetStringValue", LuaSetStringValue);
	lua_register(lua, "SetBoolValue", LuaSetBoolValue);
	lua_register(lua, "GetNumberValue", LuaGetNumberValue);
	lua_register(lua, "GetStringValue", LuaGetStringValue);
	lua_register(lua, "GetBoolValue", LuaGetBoolValue);
	Variables::Init(lua);
}

void Core::Load(const char *fileName)
{
	Release();
	TiXmlDocument doc;
	bool result = doc.LoadFile(fileName);
	if (result) {
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *element = root->FirstChildElement();
		while (element != NULL) {
			std::string name = element->Value();
			if (name == "script") {
				const char *name = element->Attribute("name");
				assert(_scripts.find(name) == _scripts.end());
				_scripts[name] = new LuaScript(lua, element);
			} else if (name == "Resources") {
				ReadDescriptions(element->Attribute("fileName"));
			} else {
				_objects.push_back(ObjectFactory::Create(element));
			}
			element = element->NextSiblingElement();
		}
	} 
#ifdef IW_DEBUG
	else {
		IwAssert(0, "file not found");
	}
#endif
	if (_scripts.find("onLoad") != _scripts.end()) {
		_scripts["onLoad"]->Execute();
	}
}

PTexture Core::getTexture(const std::string &textureId)
{
	return TextureManager::GetTexture(textureId);
}

void Core::OnMessage(const std::string &message)
{
	_messages.push_back(message);
}

void Core::Draw()
{
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->Draw();
	}
}

void Core::Update(float deltaTime)
{
	Messager::CoreSendMessages();
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->Update(deltaTime);
	}
	while (_messages.begin() != _messages.end()) {
		std::string message = *_messages.begin();
		_messages.pop_front();
		if (CanCut(message, "loadGroup ", message)) {
			LoadGroup(message);
		} else if (CanCut(message, "UnloadGroup ", message)) {
			UnloadGroup(message);
		} else if (CanCut(message, "load xml ", message)) {
			Load(message.c_str());
		} else {
			assert(false);
		}
	}
}

Core::~Core()
{
	lua_close(lua);
}

void Core::Release()
{
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		delete (*i);
		(*i) = NULL;
	}
	for (ScriptMap::iterator i = _scripts.begin(), e = _scripts.end(); i != e; i++) {
		delete _scripts.begin()->second;
		_scripts.begin()->second = NULL;
	}
	_objects.clear();
	_scripts.clear();
	TextureManager::Release();
}

bool Core::DoLua(char *code) {
	int lua_dostring_result = luaL_dostring(lua, code);
	LuaScript::report(lua, lua_dostring_result);
	return true;
}

bool Core::DoScript(const std::string &name) {
	if (_scripts.find(name) != _scripts.end()) {
		_scripts[name]->Execute();
		return true;
	}
	return false;
}