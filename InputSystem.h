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

class InputSystem 
	: public Object  
{
public:
	InputSystem();
	virtual ~InputSystem();
	virtual void OnKeyDown(int key);
	virtual void OnMouseDown(hgeVector mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(hgeVector mousePos);
	virtual bool IsMouseOver(hgeVector mousePos) = 0;

	static void Release();

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
