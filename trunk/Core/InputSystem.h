// InputSystem.h: interface for the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_INPUTSYSTEM_INCLUDED_)
#define _INPUTSYSTEM_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Object.h"

//
// надстройка над системой ввода,
// если нужно обрабатывать события мыши - наследуемся от этого класса
// если нет - от Object.h
//

class InputSystem 
	: public Object  
{
public:
	InputSystem();
	virtual ~InputSystem();
	// эти методы класса переопределяем 
	// если хотим обрабатывать соотвествующее событие

	// нажали клавишу на клавиатуре(еще не реализован)
	virtual void OnKeyDown(int key);
	// клик по элементу(нажатие левой кнопки мыши)
	virtual void OnMouseDown(hgeVector mousePos);
	// отпустили кнопку мыши(неважно где)
	virtual void OnMouseUp();
	// движение мыши
	virtual void OnMouseMove(hgeVector mousePos);
	// обязательно нужно определить для корректной работы OnMouseDown() - ""
	virtual bool IsMouseOver(hgeVector mousePos) = 0;

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;
	static void MouseDown(hgeVector mousePos);
	static void MouseUp();
	static void MouseMove(hgeVector mousePos);
	friend bool CheckForInputEvent(HGE *hge);
};

bool CheckForInputEvent(HGE *hge);

#endif // !defined(_INPUTSYSTEM_INCLUDED_)
