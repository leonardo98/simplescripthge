// Core.cpp: implementation of the Lexems class.

#include "types.h"
#include "Core.h"
#include "InputSystem.h"
#include "Render.h"
#include "../Gamee/ObjectFactory.h"
#include "Variables.h"
#include "Messager.h"

	
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
	assert(err == 0);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	float value = static_cast<float>(lua_tonumber(L, 1));
	int result = Messager::SetValueF(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaSetStringValue(lua_State *L) {
	int err = lua_gettop(L);
	assert(err == 0);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	std::string value = lua_tostring(L, 3);
	int result = Messager::SetValueS(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaSetBoolValue(lua_State *L) {
	int err = lua_gettop(L);
	assert(err != 0);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	bool value = (lua_toboolean(L, 3) != 0);
	int result = Messager::SetValueB(name, variableName, value);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaGetBoolValue(lua_State *L) {
	int err = lua_gettop(L);
	assert(err == 0);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	bool result = Messager::GetBoolValue(name, variableName);
	lua_pushboolean(L, result);
	return 1;
}

static int LuaGetNumberValue(lua_State *L) {
	int err = lua_gettop(L);
	assert(err == 0);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	float result = Messager::GetNumberValue(name, variableName);
	lua_pushnumber(L, result);
	return 1;
}

static int LuaGetStringValue(lua_State *L) {
	int err = lua_gettop(L);
	assert(err == 0);
	const char *name = lua_tostring(L, 1);
	const char *variableName = lua_tostring(L, 2);
	const char *value = lua_tostring(L, 2);// не помню для чего это сделано - может это и лишнее????
	assert(value != 0);
	std::string result = Messager::GetValue(name, variableName);
	lua_pushstring(L, result.c_str());
	return 1;
}


static int LuaOpenFile(lua_State *L) {
    assert(false);
    /*
	int err = lua_gettop(L);
	//assert(err == 0);
	const char *path = lua_tostring(L, 1);
	const char *mask = lua_tostring(L, 2);
	std::string file;
	OPENFILENAME fn;
	fn.lStructSize = sizeof(fn);
	fn.hInstance = 0;
	fn.hwndOwner = Render::GetDC()->System_GetState(HGE_HWND);
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
     */
	return 0;
}

static int LuaDoFile(lua_State *L) {
	int err = lua_gettop(L);
	assert(err == 1);
	const char *name = lua_tostring(L, 1);
	err = luaL_dofile(L, Render::GetDC()->Resource_MakePath((Render::GetDataDir() + name).c_str()));
    assert(err == 0);
	return 0;
}

static int LuaLoadGroup(lua_State *L) {
	int err = lua_gettop(L);
	assert(err == 1);
	const char *name = lua_tostring(L, 1);
	Core::LoadGroup(name);
	return 0;
}

static int LuaUnloadGroup(lua_State *L) {
	int err = lua_gettop(L);
	assert(err == 1);
	const char *name = lua_tostring(L, 1);
	Core::UnloadGroup(name);
	return 0;
}

Core::Objects Core::_objects;
Core::ScriptMap Core::_scripts;
Core::AnimationMap Core::_animations;
std::list<std::string> Core::_messages;
lua_State *Core::lua;

void Core::Init() {
	lua = lua_open();
	luaopen_base(lua);
	lua_register(lua, "SendMessage", LuaSendMessage);
	lua_register(lua, "SetNumberValue", LuaSetNumberValue);
	lua_register(lua, "SetStringValue", LuaSetStringValue);
	lua_register(lua, "SetBoolValue", LuaSetBoolValue);
	lua_register(lua, "GetNumberValue", LuaGetNumberValue);
	lua_register(lua, "GetStringValue", LuaGetStringValue);
	lua_register(lua, "GetBoolValue", LuaGetBoolValue);
	lua_register(lua, "OpenFile", LuaOpenFile);
	lua_register(lua, "LoadGroup", LuaLoadGroup);
	lua_register(lua, "UnloadGroup", LuaUnloadGroup);
	lua_register(lua, "MyDoFile", LuaDoFile);
	//lua_register(lua, "CutImages", LuaCutImages);
	Variables::Init(lua);
}

void Core::LoadAnimations(const char *fileName) {
	TiXmlDocument doc;
    std::string s =Render::GetDC()->Resource_MakePath((Render::GetDataDir() + fileName).c_str());
	if ((fileName[1] == ':' && doc.LoadFile(fileName)) || doc.LoadFile(s.c_str())) {
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *animation = root->FirstChildElement("Animation");
		while (animation) {
			const char *id = animation->Attribute("id");
			if (_animations.find(id) != _animations.end()) {
				LOG("animation already exist: " + id);
				assert(false);
			}
			_animations[id] = new Animation(animation);
			animation = animation->NextSiblingElement("Animation");
		}
	} else {
		LOG("file not found: " + fileName);
	}
}

Animation *Core::getAnimation(const std::string &animationId) {
	AnimationMap::iterator find = _animations.find(animationId);
	if (find != _animations.end()) {
		if (find->second->TextureLoaded() == false) {
			find->second->LoadTextures();
		}
		return (find->second);
	}
	LOG("animation " + animationId + " not found.");
	return NULL;
}

void Core::addAnimation(const std::string &id, Animation *animation) {
	assert(_animations.find(id) == _animations.end());
	_animations[id] = animation;
}


#ifndef ENGINE_AS_LIBRARY

void Core::Load(const char *fileName)
{
	TiXmlDocument doc;
    std::string s(Render::GetDataDir() + fileName);
    s = Render::GetDC()->Resource_MakePath(s.c_str());
	bool result = doc.LoadFile(s.c_str());
	if (result) {
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *element = root->FirstChildElement();
		while (element != NULL) {
			std::string name = element->Value();
			if (name == "script") {
				const char *name = element->Attribute("name");
				if (name != NULL) {
					assert(_scripts.find(name) == _scripts.end());
					_scripts[name] = new LuaScript(lua, element);
				} else {
					LuaScript l(lua, element);
					l.Execute();
				}
			} else if (name == "Resources") {
				TextureManager::ReleaseAllTextures();
				ReadDescriptions(element->Attribute("fileName"));
			} else if (name == "Animations") {
				LoadAnimations(element->Attribute("fileName"));
			} else {
				_objects.push_back(ObjectFactory::Create(element));
			}
			element = element->NextSiblingElement();
		}
	} else {
		LOG(std::string("Core ") + fileName + " - file not found");
	}
	if (_scripts.find("onLoad") != _scripts.end()) {
		_scripts["onLoad"]->Execute();
	}
}

#endif//ENGINE_AS_LIBRARY

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
		if (Messager::CanCut(message, "loadGroup ", message)) {
			LoadGroup(message);
		} else if (Messager::CanCut(message, "UnloadGroup ", message)) {
			UnloadGroup(message);
#ifndef ENGINE_AS_LIBRARY
		} else if (Messager::CanCut(message, "load xml ", message)) {
			InputSystem::Reset();
			Unload();
			Load(message.c_str());
#endif//ENGINE_AS_LIBRARY
		} else {
			assert(false);
		}
	}
}

void Core::Unload() {
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		delete (*i);
		(*i) = NULL;
	}
	for (ScriptMap::iterator i = _scripts.begin(), e = _scripts.end(); i != e; i++) {
		delete _scripts.begin()->second;
		_scripts.begin()->second = NULL;
	}
	for (AnimationMap::iterator i = _animations.begin(), e = _animations.end(); i != e; i++) {
		delete _animations.begin()->second;
		_animations.begin()->second = NULL;
	}
	_objects.clear();
	_animations.clear();
	_scripts.clear();
}

void Core::GetAnimationsList(std::vector<std::string> &names) {
	names.clear();
	for (AnimationMap::iterator i = _animations.begin(), e = _animations.end(); i != e; i++) {
		names.push_back(i->first);
	}
}

void Core::Release()
{
	Unload();
	lua_close(lua);
	ReleaseAllTextures();
}

bool Core::DoLua(const char *code) {
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

void Core::RenameAnimation(const std::string &oldName, const std::string &newName) {
	AnimationMap::iterator i = _animations.find(oldName);
	assert(i != _animations.end());
	Animation *tmp = i->second;
	_animations.erase(i);

	i = _animations.find(newName);
	assert(i == _animations.end());

	_animations[newName] = tmp;
}

void Core::RemoveAnimation(const std::string &name) {
	AnimationMap::iterator i = _animations.find(name);
	assert(i != _animations.end());
	delete i->second;
	_animations.erase(i);
}
