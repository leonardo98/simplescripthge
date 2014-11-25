#ifndef MYENGINE_BUTTON_H
#define MYENGINE_BUTTON_H

#include "../Core/InputSystem.h"
#include "../Core/LuaScript.h"
#include "../Core/Render.h"

class Button :
	public InputSystem
{
public:
	Button(TiXmlElement *xe);
	void Draw();
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(const FPoint2D &mousePos);

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


#endif//MYENGINE_BUTTON_H