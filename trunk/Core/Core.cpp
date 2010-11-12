// Core.cpp: implementation of the Lexems class.

#include "types.h"
#include "Core.h"
#include "..\Gamee\ObjectFactory.h"
#include "Variables.h"

static int LuaSendMessage(lua_State *L) {
	int err = lua_gettop(L);
	const char *name = lua_tostring(L, 1);
	const char *message = lua_tostring(L, 2);
	Messager::SendMessage(name, message);
	assert(err == lua_gettop(L));
	return 0;
}

static int LuaSetNumberValue(lua_State *L) {
	int err = lua_gettop(L);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	float value = lua_tonumber(L, 1);
	int result = Messager::SetValue(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaSetStringValue(lua_State *L) {
	int err = lua_gettop(L);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	std::string value = lua_tostring(L, 3);
	int result = Messager::SetValue(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaSetBoolValue(lua_State *L) {
	int err = lua_gettop(L);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	bool value = lua_toboolean(L, 3);
	int result = Messager::SetValue(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaGetBoolValue(lua_State *L) {
	int err = lua_gettop(L);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	bool result = Messager::GetBoolValue(name, variableName);
	lua_pushboolean(L, result);
	return 1;
}

static int LuaGetNumberValue(lua_State *L) {
	int err = lua_gettop(L);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	float result = Messager::GetNumberValue(name, variableName);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaGetStringValue(lua_State *L) {
	int err = lua_gettop(L);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 1);
	const char *value = lua_tostring(L, 2);
	std::string result = Messager::GetValue(name, variableName);
	lua_pushstring(L, result.c_str());
	return 1;
}

static int LuaOpenFile(lua_State *L) {
	int err = lua_gettop(L);
	const char *path = lua_tostring(L, 1);
	const char *mask = lua_tostring(L, 2);
	std::string file;
	OPENFILENAME fn;
	fn.lStructSize = sizeof(fn);
	fn.hInstance = 0;
	fn.hwndOwner = Core::GetDC()->System_GetState(HGE_HWND);
	fn.lpstrInitialDir = path;
	fn.lpstrFilter = mask;
	fn.nFilterIndex = 0;
	fn.lpstrFile = new char[1024];
	fn.lpstrFile[0] = 0;
	fn.nMaxFile = 1024 - 1;
	fn.lpstrFileTitle = NULL;
	fn.nMaxFileTitle = NULL;
	fn.lpstrTitle = NULL;
	fn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	fn.lpstrCustomFilter = NULL;
	fn.nMaxCustFilter = 0;
	fn.nFileOffset = 0;
	fn.nFileExtension = 0;
	fn.lpstrDefExt = NULL;
	fn.lCustData = NULL;
	fn.lpfnHook = NULL;
	fn.FlagsEx = 0;
	fn.lpTemplateName = "";
	fn.dwReserved = 0;
	fn.pvReserved = 0;
	bool res = GetOpenFileName(&fn);
	assert(err == lua_gettop(L));
	if (res) {
		lua_pushstring(L, fn.lpstrFile);
	} else {
		lua_pushstring(L, "null");
	}
	return 1;
}

HGE *Core::_hge;

Core::Core()
	: Messager("Core")
{
	lua = lua_open();
	lua_register(lua, "SendMessage", LuaSendMessage);
	lua_register(lua, "OpenFile", LuaOpenFile);
	lua_register(lua, "SetNumberValue", LuaSetNumberValue);
	lua_register(lua, "SetStringValue", LuaSetStringValue);
	lua_register(lua, "SetBoolValue", LuaSetBoolValue);
	lua_register(lua, "GetNumberValue", LuaGetNumberValue);
	lua_register(lua, "GetStringValue", LuaGetStringValue);
	lua_register(lua, "GetBoolValue", LuaGetBoolValue);
	Variables::Init(lua);
	assert(_keyCodes.size() == 0); // нельз€ создать два экземпл€ра
	_keyCodes["BACKSPACE"] = HGEK_BACKSPACE;
	_keyCodes["TAB"] = HGEK_TAB;
	_keyCodes["ENTER"] = HGEK_ENTER;
	_keyCodes["SPACE"] = HGEK_SPACE;
}

std::map<std::string, DWORD> Core::_keyCodes;

DWORD Core::getKeyCode(const std::string &id) {
	return _keyCodes[id];
}

void Core::Load(const std::string &fileName)
{
	Release();
	TiXmlDocument doc;
	bool result = doc.LoadFile(fileName.c_str());
	if (result) {
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *element = root->FirstChildElement();
		while (element != NULL) {
			std::string name = element->Value();
			if (name == "script") {
				_scripts[element->Attribute("name")] = new LuaScript(lua, element);
			} else if (name == "Resources") {
				ReadDescriptions(element->Attribute("fileName"));
			} else {
				_objects.push_back(ObjectFactory::Create(element));
			}
			element = element->NextSiblingElement();
		}
	}
	if (_scripts.find("onLoad") != _scripts.end()) {
		_scripts["onLoad"]->Execute();
	}
}

Texture * Core::getTexture(const std::string &textureId)
{
	return TextureManager::GetTexture(textureId);
}

void Core::OnMessage(const std::string &message)
{
	_messages.push_back(message);
}

void Core::Draw()
{
	for (unsigned int i = 0; i < _objects.size(); i++) {
		_objects[i]->Draw();
	}
}

void Core::Update(float deltaTime)
{
	for (unsigned int i = 0; i < _objects.size(); i++) {
		_objects[i]->Update(deltaTime);
	}
	while (_messages.begin() != _messages.end()) {
		std::string message = *_messages.begin();
		_messages.pop_front();
		if (CanCut(message, "loadGroup ")) {
			LoadGroup(message);
		} else if (CanCut(message, "UnloadGroup ")) {
			UnloadGroup(message);
		} else if (CanCut(message, "load xml ")) {
			Load(message);
		}
	}
}

Core::~Core()
{
	lua_close(lua);
}

void Core::Release()
{
	for (unsigned int i = 0; i < _objects.size(); i++) {
		delete _objects[i];
		_objects[i] = NULL;
	}
	for (ScriptMap::iterator i = _scripts.begin(), e = _scripts.end(); i != e; i++) {
		delete _scripts.begin()->second;
		_scripts.begin()->second = NULL;
	}
	_objects.clear();
	_scripts.clear();
}

HGE *Core::GetDC() {
	return _hge;
}
void Core::SetDC(HGE *hge) {
	_hge = hge;
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

void Core::DrawBar(float x, float y, float width, float height, DWORD color) {
	hgeQuad quad;
	quad.blend = BLEND_ALPHABLEND;
	quad.tex = NULL;
	quad.v[0].x = x;
	quad.v[0].y = y;
	quad.v[1].x = x + width;
	quad.v[1].y = y;
	quad.v[2].x = x + width;
	quad.v[2].y = y + height;
	quad.v[3].x = x;
	quad.v[3].y = y + height;
	for (int i = 0; i < 4; i++) {
		quad.v[i].col = color;
		quad.v[i].tx = 0;
		quad.v[i].ty = 0;
		quad.v[i].z = 0;
	}
	Core::GetDC()->Gfx_RenderQuad(&quad);
}