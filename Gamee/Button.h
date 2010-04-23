#pragma once
#include "..\Core\InputSystem.h"
#include "..\Core\LuaScript.h"

class Button :
	public InputSystem
{
public:
	Button(TiXmlElement *xe);
	void Draw();
	virtual void OnMouseDown(hgeVector mousePos);
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(hgeVector mousePos);

	virtual void OnMouseIn();
	virtual void OnMouseOut();
private:
	hgeVector _pos;
	hgeVector _oldMousePos;
	bool _down;
	Texture *_normal;
	Texture *_pressed;
	Texture *_current;
	LuaScript *_luaScript;
};
