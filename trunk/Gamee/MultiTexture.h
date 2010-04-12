// MultiTexture.h: interface for the MultiTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MULTITEXTURE_INCLUDED_)
#define _MULTITEXTURE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\InputSystem.h"
#include "..\Helpers\Counter.h"
#include "..\Core\LuaScript.h"

struct State {
	hgeVector pos;
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
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMouseDown(hgeVector mousePos);
private:
	typedef std::vector<State> States;
	States _states;
	std::string _stateVariableName;
	LuaScript *_luaScript;
};

#endif // !defined(_MULTITEXTURE_INCLUDED_)
