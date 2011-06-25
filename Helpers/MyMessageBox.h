#pragma once

#include "..\Core\InputSystem.h"
#include "..\Core\Interface.h"
#include "..\Core\LuaScript.h"
#include "..\Core\Messager.h"
#include "..\Core\IButton.h"

// класс окна сообщения(по нажатию выполняется скрипт)
// рисуется примитивами без текстур

class MyMessageBox
	: public Interface
	, public InputSystem
	, public Messager
{
public:
	MyMessageBox(TiXmlElement *xe);
	~MyMessageBox();
	void Draw();
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(FPoint2D mousePos);

	virtual void SetValue(const std::string &variableName, const std::string &value);
	virtual void OnMessage(const std::string &message);

private:
	unsigned int _width;
	unsigned int _height;
	IButton _yes;
	IButton _no;
	IButton _ok;
	std::string _text;
	std::vector<IButton *> _buttons;
	std::string _answerReceiver;
	FPoint2D _pos;
	unsigned int _textY;
	static MyMessageBox *_myMessageBox;
public:
	static void Show(const std::string &text, const std::string &buttons, const std::string &receiver);
};

#define OkMessageShow(text) MyMessageBox::Show(text, "ok", GetMyName())
#define AskMessageShow(text) MyMessageBox::Show(text, "yesno", GetMyName())
