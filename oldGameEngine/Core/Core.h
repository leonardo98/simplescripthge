// Core.h: interface for the Lexems class.

#ifndef MYENGINE_CORE_H
#define MYENGINE_CORE_H

#include "types.h"
#include "TextureManager.h"
#include "LuaScript.h"
#include "Object.h"
#include "Animation.h"
#include "../2dga_api/Animation.h"


class Core
	: public TextureManager
{
public:
	static void Init();
	static void Load(const char *fileName);
	static void Unload();
	static void Draw();
	static void Update(float deltaTime);
	static void Release();
	static bool DoLua(const char *code);
	static bool DoScript(const std::string &name);
	static My::Animation *getAnimation(const std::string &animationId);
	static void DrawBar(float x, float y, float width, float height, DWORD color);// Debug only
	static void OnMessage(const std::string &message);
	static bool LoadAnimations(const char *fileName);
	static void GetAnimationsList(std::vector<std::string> &names);

	typedef std::list<Object *> Objects;
	typedef std::map<std::string, LuaScript *> ScriptMap;
	typedef std::map<std::string, My::Animation *> AnimationMap;

protected:

	static Objects _objects;
	static ScriptMap _scripts;
	static AnimationMap _animations;
	static std::list<std::string> _messages;
	static lua_State *lua;

	friend class AnimationEditor;
};

#endif//MYENGINE_CORE_H