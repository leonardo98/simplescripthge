#pragma once
#include "..\Core\InputSystem.h"
#include "..\Core\LuaScript.h"

class Button :
	public InputSystem
{
public:
	Button(TiXmlElement *xe);
	void Draw();
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(FPoint2D mousePos);

	virtual void OnMouseIn();
	virtual void OnMouseOut();
private:
	FPoint2D _pos;
	FPoint2D _oldMousePos;
	bool _down;
	Texture *_normal;
	Texture *_pressed;
	Texture *_current;
	LuaScript *_luaScript;
};
