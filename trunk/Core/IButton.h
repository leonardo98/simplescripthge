#pragma once
#include "InputSystem.h"
#include "Interface.h"
#include "LuaScript.h"

// ����� ������(�� ������� ����������� ������)
// �������� ����������� ��� �������

class IButton
	: public Interface
	, public InputSystem
{
public:
	IButton(TiXmlElement *xe);
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
	unsigned int _width;
	unsigned int _height;
	std::string _caption;
	enum {
		BUTTON_PRESSED,
		BUTTON_RELEASED
	} _state;
	LuaScript *_luaScript;
};
