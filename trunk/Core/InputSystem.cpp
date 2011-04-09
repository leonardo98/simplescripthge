// InputSystem.cpp: implementation of the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "InputSystem.h"
#include "Render.h"
#include "Messager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


float InputSystem::DOUBLE_CLICK_TIME; // максимальное время на второй клик - если время больше - событие "аннулируется"
float InputSystem::LONG_TAP_EPS; // погрешность на случайные движения мыши во время нажатия, если движение более сильное - событие "аннулируется"
float InputSystem::LONG_TAP_TIME; // минимальное время на вызов события - если время меньше - событие "аннулируется"
float InputSystem::_timeCounter; // счетчик времени(отсчитывает время либо с последнего события нажатия или отпускания кнопки мыши
bool InputSystem::_longTap; // true - если ловим событие "Длинный Тап"
bool InputSystem::_doubleClick; // true - если ловим это событие "Двойной клик"
FPoint2D InputSystem::_longTapPos;
InputSystem *InputSystem::_locked;

InputSystem::InputSystem()
{
	_listeners.push_back(this);
}

InputSystem::~InputSystem()
{
	Listeners::iterator i;
	for (i = _listeners.begin(); i != _listeners.end() && *i != this; i++);
	if (i == _listeners.end()) {// нет в списке (уже удален)
		Messager::SendMessage("log", "удален уже Listener - деструктор вызван второй раз???");
	} else {
		_listeners.erase(i);
	}
}

void InputSystem::OnMouseDown(FPoint2D mousePos) {}

void InputSystem::OnMouseUp() {}

void InputSystem::OnMouseMove(FPoint2D mousePos) {}

void InputSystem::OnLongTap(FPoint2D mousePos) {}

void InputSystem::OnDoubleClick(FPoint2D mousePos) {}

bool InputSystem::OnMouseWheel(int direction) {return false;}

void InputSystem::MouseDown(FPoint2D mousePos) {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnMouseDown(mousePos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::LongTap() {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(_longTapPos)) {
			(*i)->OnLongTap(_longTapPos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::DoubleClick(FPoint2D mousePos) {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnDoubleClick(mousePos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::MouseWheel(int direction) {
	if (_locked != NULL) {
		_locked->OnMouseWheel(direction);
		return;
	}
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->OnMouseWheel(direction)) {
			return;
		}
	}
}

void InputSystem::MouseUp() {
	for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
		(*i)->OnMouseUp();
	}
	_locked = NULL;
}

void InputSystem::MouseMove(FPoint2D mousePos) {
	if (_locked != NULL) {
		_locked->OnMouseMove(mousePos);
		return;
	}
	for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
		(*i)->OnMouseMove(mousePos);
	}
}

InputSystem::Listeners InputSystem::_listeners;

void InputSystem::RemoveFromList(InputSystem *listener) {
	Listeners::iterator i;
	for (i = _listeners.begin(); i != _listeners.end() && *i != listener; i++);
	if (i == _listeners.end()) {// нет в списке (уже удален)
		Messager::SendMessage("log", "удален уже Listener");
		exit(-4);
	}
	_listeners.erase(i);
}

void OnLongTap(int direction) {
}

void OnDoubleClick(int direction) {
}

bool CheckForInputEvent(float dt) {
	hgeInputEvent event;
	InputSystem::_timeCounter += dt;
	InputSystem::_doubleClick &= (InputSystem::_timeCounter < InputSystem::DOUBLE_CLICK_TIME);
	while (Render::GetDC()->Input_GetEvent(&event)) {
		if (event.type == INPUT_MBUTTONDOWN && event.key == HGEK_LBUTTON) {
			InputSystem::_longTapPos = FPoint2D(event.x, event.y);
			InputSystem::MouseDown(InputSystem::_longTapPos);
			if (InputSystem::_doubleClick) {
				InputSystem::DoubleClick(InputSystem::_longTapPos);
				InputSystem::_doubleClick = false;
			}
			InputSystem::_timeCounter = dt / 2.f;
			InputSystem::_longTap = true;
			InputSystem::_doubleClick = true;
		} else if (event.type == INPUT_MBUTTONUP && event.key == HGEK_LBUTTON) {
			InputSystem::MouseUp();
			InputSystem::_longTap = false;
		} else if (event.type == INPUT_MOUSEMOVE) {
			FPoint2D pos = FPoint2D(event.x, event.y);
			InputSystem::MouseMove(pos);
			InputSystem::_longTap &= (InputSystem::_longTapPos - pos).Length() > InputSystem::LONG_TAP_EPS;
		} else if (event.type == INPUT_MOUSEWHEEL) {
			InputSystem::MouseWheel(event.wheel);
		} else if (event.type == INPUT_KEYDOWN && event.key == HGEK_ESCAPE) {
			return true;
		}
	}
	if (InputSystem::_longTap) {
		InputSystem::LongTap();
		InputSystem::_longTap = false;
	}
	return false;
}

void InitInputEvent() {
	InputSystem::LONG_TAP_EPS = 10.f;
	InputSystem::LONG_TAP_TIME = 0.2f;
	InputSystem::DOUBLE_CLICK_TIME = 0.2f;
	InputSystem::_timeCounter = 0.f;
	InputSystem::_longTap = false;
	InputSystem::_doubleClick = false; 
	InputSystem::_locked = NULL;
}