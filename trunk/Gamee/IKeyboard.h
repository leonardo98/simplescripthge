#ifndef IKeyboard_h
#define IKeyboard_h

#include "InputSystem.h"
#include "Interface.h"
#include "LuaScript.h"
#include "Messager.h"

// класс клавиатуры(по нажатию выполняется скрипт)
// рисуется примитивами без текстур

class LittleButton {
public:
	LittleButton(const FPoint &pos, const char *text, unsigned char code);
	void Draw();
	unsigned char Code();
private:
	FPoint2D _pos;
	bool _down;
	const static int WIDTH = 50;
	const static int HEIGHT = 50;
	std::string _caption;
	unsigned char _code;
	enum {
		BUTTON_PRESSED,
		BUTTON_RELEASED
	} _state;
};

class IKeyboard
	: public Interface
	, public InputSystem
	, public Messager
{
public:
	IKeyboard(TiXmlElement *xe);
	~IKeyboard();
	void Draw();
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(FPoint2D mousePos);
	virtual void OnMouseIn();
	virtual void OnMouseOut();

private:
	FPoint2D _oldMousePos;
	LuaScript *_luaScript;
};

#endif//IKeyboard_h