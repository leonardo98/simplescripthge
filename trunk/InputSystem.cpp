// InputSystem.cpp: implementation of the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "InputSystem.h"
#include "Messager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InputSystem::InputSystem()
{
	_listeners.push_back(this);
}

void InputSystem::Release() {
}

InputSystem::~InputSystem()
{
	Listeners::iterator i;
	for (i = _listeners.begin(); i != _listeners.end() && *i != this; i++);
	if (i == _listeners.end()) {// нет в списке (уже удален)
		Messager::SendMessage("log", "удален уже Listener - деструктор вызван второй раз???");
		exit(-4);
	}
	_listeners.erase(i);
}

void InputSystem::OnKeyDown(int key) {}

void InputSystem::OnMouseDown(hgeVector mousePos) {}

void InputSystem::OnMouseUp() {}

void InputSystem::OnMouseMove(hgeVector mousePos) {}

void InputSystem::MouseDown(hgeVector mousePos) {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnMouseDown(mousePos);
			return;
		}
	}
}

void InputSystem::MouseUp() {
	for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
		(*i)->OnMouseUp();
	}
}

void InputSystem::MouseMove(hgeVector mousePos) {
	for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
		(*i)->OnMouseMove(mousePos);
	}
}

InputSystem::Listeners InputSystem::_listeners;

bool CheckForInputEvent(HGE *hge) {
	hgeInputEvent event;
	while (hge->Input_GetEvent(&event)) {
		if (event.type == INPUT_MBUTTONDOWN && event.key == HGEK_LBUTTON) {
			InputSystem::MouseDown(hgeVector(event.x, event.y));
		} else if (event.type == INPUT_MBUTTONUP && event.key == HGEK_LBUTTON) {
			InputSystem::MouseUp();
		} else if (event.type == INPUT_MOUSEMOVE) {
			InputSystem::MouseMove(hgeVector(event.x, event.y));
		} else if (event.type == INPUT_KEYDOWN && event.key == HGEK_ESCAPE) {
			return true;
		}
	}
	return false;
}