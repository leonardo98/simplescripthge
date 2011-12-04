// MultiTexture.h: interface for the MultiTexture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MULTITEXTURE_H
#define MULTITEXTURE_H

#include "../Core/InputSystem.h"
#include "../Helpers/Counter.h"
#include "../Core/LuaScript.h"
#include "../Core/Render.h"

struct State {
	FPoint2D pos;
	Texture *texture;
	std::string name;
	bool visible;
	State(TiXmlElement *xe);
	void Draw();
	void SetAlpha(float alpha);
};

class MultiTexture 
	: public InputSystem  
{
public:
	MultiTexture(TiXmlElement *xe);
	virtual ~MultiTexture();
	virtual void Draw();
	virtual void Update(float deltaTime);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual void OnMouseDown(const FPoint2D &mousePos);
private:
	typedef std::list<State> States;
	States _states;
	std::string _stateVariableName;
	LuaScript *_luaScript;
};

#endif//MULTITEXTURE_H
