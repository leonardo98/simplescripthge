#include "Button.h"
#include "..\Core\Core.h"
#include "..\Core\Variables.h"

Button::Button(TiXmlElement *xe)
	: _luaScript(new LuaScript(Variables::l, xe))
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	std::string normal = xe->Attribute("normal"); 
	_normal = Core::getTexture(normal);
	std::string pressed = xe->Attribute("pressed"); 
	_pressed = Core::getTexture(pressed);
	_current = _normal;
	_down = false;
}

void Button::Draw() {
	_current->Render(_pos);
}

bool Button::IsMouseOver(hgeVector mousePos) {
	mousePos -= _pos;
	return _normal->IsNotTransparent((int)mousePos.x, (int)mousePos.y);
}

void Button::OnMouseDown(hgeVector mousePos) {
	if (IsMouseOver(mousePos)) {
		_current = _pressed;
		_down = true;
	}
}

void Button::OnMouseMove(hgeVector mousePos) {
	bool oldValue = IsMouseOver(_oldMousePos);
	_oldMousePos = mousePos;
	bool newValue = IsMouseOver(_oldMousePos);
	if (!oldValue && newValue) {
		OnMouseIn();
	} else if (oldValue && !newValue) {
		OnMouseOut();
	}
}

void Button::OnMouseUp() {
	if (_down && _current == _pressed) {
		_current = _normal;
		_down = false;
		_luaScript->Execute();
	}
}

void Button::OnMouseIn() {
	if (_down) {
		_current = _pressed;
	}
}

void Button::OnMouseOut() {
	if (_down) {
		_current = _normal;
	}
}
