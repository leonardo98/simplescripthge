#pragma once
#include "InputSystem.h"
#include "Interface.h"
#include "LuaScript.h"
#include "Messager.h"

// класс кнопки(по нажатию выполняется скрипт)
// рисуется примитивами без текстур

class IButton
	: public Interface
	, public InputSystem
	, public Messager
{
public:
	IButton(TiXmlElement *xe);
	void Draw();
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(FPoint2D mousePos);

	virtual void OnMouseIn();
	virtual void OnMouseOut();

	virtual void SetValue(const std::string &variableName, const std::string &value);

private:
	FPoint2D _pos;
	FPoint2D _oldMousePos;
	bool _down;
	unsigned int _width;
	unsigned int _height;
	std::string _caption;
	enum {
		BUTTON_PRESSED,
		BUTTON_RELEASED
	} _state;
	LuaScript *_luaScript;
};
