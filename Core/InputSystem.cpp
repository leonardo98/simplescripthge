// InputSystem.cpp: implementation of the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "InputSystem.h"
#include "Render.h"
#include "Messager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


float InputSystem::DOUBLE_CLICK_TIME; // ������������ ����� �� ������ ���� - ���� ����� ������ - ������� "������������"
float InputSystem::LONG_TAP_EPS; // ����������� �� ��������� �������� ���� �� ����� �������, ���� �������� ����� ������� - ������� "������������"
float InputSystem::LONG_TAP_TIME; // ����������� ����� �� ����� ������� - ���� ����� ������ - ������� "������������"
float InputSystem::_timeCounter; // ������� �������(����������� ����� ���� � ���������� ������� ������� ��� ���������� ������ ����
bool InputSystem::_longTap; // true - ���� ����� ������� "������� ���"
bool InputSystem::_doubleClick; // true - ���� ����� ��� ������� "������� ����"
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
	if (i != _listeners.end()) {// ��� � ������ (��� ������)
		_listeners.erase(i);
	}
}

void InputSystem::OnMouseDown(const FPoint2D &mousePos) {}

void InputSystem::OnMouseUp() {}

void InputSystem::OnMouseMove(const FPoint2D &mousePos) {}

void InputSystem::OnLongTap(const FPoint2D &mousePos) {}

void InputSystem::OnDoubleClick(const FPoint2D &mousePos) {}

bool InputSystem::OnMouseWheel(int direction) {return false;}

bool InputSystem::OnKey(int key) {return false;}

void InputSystem::MouseDown(const FPoint2D &mousePos) {
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

void InputSystem::DoubleClick(const FPoint2D &mousePos) {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnDoubleClick(mousePos);
			_locked = *i;
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

void InputSystem::Reset() {
	_locked = NULL;
}

void InputSystem::MouseMove(const FPoint2D &mousePos) {
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
	if (i == _listeners.end()) {// ��� � ������ (��� ������)
		LOG("������ ��� Listener ��� ��������� �� ����������� �� InputSystem");
		exit(-4);
	}
	_listeners.erase(i);
}

void OnLongTap(int direction) {
}

void OnDoubleClick(int direction) {
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

bool InputSystem::CheckForEvent(float dt) {
	_timeCounter += dt;
	_doubleClick &= (_timeCounter < DOUBLE_CLICK_TIME);
	hgeInputEvent event;
	while (Render::GetDC()->Input_GetEvent(&event)) {
		if (event.type == INPUT_MBUTTONDOWN && event.key == HGEK_LBUTTON) {
			_longTapPos = FPoint2D(event.x, event.y);
			MouseDown(_longTapPos);
			if (_doubleClick) {
				DoubleClick(_longTapPos);
				_doubleClick = false;
			}
			_timeCounter = dt / 2.f;
			_longTap = true;
			_doubleClick = true;
		} else if (event.type == INPUT_MBUTTONUP && event.key == HGEK_LBUTTON) {
			MouseUp();
			_longTap = false;
		} else if (event.type == INPUT_MOUSEMOVE) {
			FPoint2D pos = FPoint2D(event.x, event.y);
			MouseMove(pos);
			_longTap &= (_longTapPos - pos).Length() > LONG_TAP_EPS;
		} else if (event.type == INPUT_MOUSEWHEEL) {
			MouseWheel(event.wheel);
		} else if (event.type == INPUT_KEYDOWN /*&& event.key == HGEK_ESCAPE*/) {
			for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
				if ((*i)->OnKey(event.key)) {
					break;
				}
			}
		//	return true;
		}
	}
	if (_longTap) {
		LongTap();
		_longTap = false;
	}
	return false;
}

void InputSystem::Init() {
	LONG_TAP_EPS = 10.f;
	LONG_TAP_TIME = 0.2f;
	DOUBLE_CLICK_TIME = 0.2f;
	_timeCounter = 0.f;
	_longTap = false;
	_doubleClick = false; 
	_locked = NULL;
}

