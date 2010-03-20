// Core.h: interface for the Lexems class.

#if !defined(_CORE_INCLUDED_)
#define _CORE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Messager.h"
#include "TextureManager.h"
#include "Parser.h"
#include "Object.h"


class Core
	: public Messager
	, public TextureManager
	, public Parser
{
public:
	Core();
	~Core();
	void Load(std::string fileName);
	void Draw();
	void Update(float deltaTime);
	void Release();
	static Texture * getTexture(std::string textureId);
	virtual void OnMessage(std::string message); 
	typedef std::vector<Object *> Objects;
protected:
	Objects _objects;
	std::map<std::string, Parser> _scripts;
	std::list<std::string> _messages;
};

#endif // !defined(_CORE_INCLUDED_)