// Core.h: interface for the Lexems class.

#if !defined(_CORE_INCLUDED_)
#define _CORE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Messager.h"
#include "TextureManager.h"
#include "LuaScript.h"
#include "Object.h"


class Core
	: public Messager
	, public TextureManager
{
public:
	Core();
	~Core();
	void Load(const std::string &fileName);
	void Draw();
	void Update(float deltaTime);
	void Release();
	bool DoLua(char *code);
	bool DoScript(const std::string &name);
	static Texture * getTexture(const std::string &textureId);
	static DWORD getKeyCode(const std::string &id);
	static DeviceContext GetDC();
	static void DrawBar(float x, float y, float width, float height, DWORD color);// Debug only
	static void SetDC(DeviceContext dc);
	virtual void OnMessage(const std::string &message); 
	typedef std::vector<Object *> Objects;
protected:
	Objects _objects;
	typedef std::map<std::string, LuaScript *> ScriptMap;
	ScriptMap _scripts;
	std::list<std::string> _messages;
	lua_State *lua;
	static DeviceContext _dc;
	static std::map<std::string, DWORD> _keyCodes;
};

#endif // !defined(_CORE_INCLUDED_)