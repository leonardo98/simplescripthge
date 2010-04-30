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
	fn.Flags = OFN_FILEMUSTEXIST;
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
	char currentDirectory[1024];
	currentDirectory[0] = 0;
	DWORD cdres = GetCurrentDirectory(1024, currentDirectory);
	bool res = GetOpenFileName(&fn);
	assert(err == lua_gettop(L));
	if (res) {
		lua_pushstring(L, fn.lpstrFile);
	} else {
		lua_pushstring(L, "null");
	}
	if (cdres) {
		SetCurrentDirectory(currentDirectory);
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
	Variables::Init(lua);
}

void Core::Load(std::string fileName)
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

Texture * Core::getTexture(std::string textureId)
{
	return TextureManager::GetTexture(textureId);
}

void Core::OnMessage(std::string message)
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
