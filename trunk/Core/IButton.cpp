#include "Render.h"
#include "IButton.h"
#include "Core.h"
#include "Variables.h"

IButton::~IButton() {
	delete _luaScript;
	_luaScript = NULL;
}

IButton::IButton(TiXmlElement *xe)
	: _luaScript(new LuaScript(Variables::l, xe))
	, Messager(xe)
{
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_caption = xe->Attribute("caption"); 
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_state = BUTTON_RELEASED;
	_down = false;
}

void IButton::Draw() {
	if (_state == BUTTON_RELEASED) {
		Render::DrawBar(_pos.x, _pos.y, static_cast<float>(_width), static_cast<float>(_height), Interface::BACKGROUND_NORMAL);
		Render::Line(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::Line(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::Line(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_HIGH);
		Render::Line(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_HIGH);
	} else if (_state == BUTTON_PRESSED) {
		Render::DrawBar(_pos.x, _pos.y, static_cast<float>(_width), static_cast<float>(_height), Interface::BACKGROUND_PRESSED);
		Render::Line(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_LOW);
		Render::Line(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_LOW);
		Render::Line(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::Line(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
	} else {
		assert(false);
	}
	Render::PrintString(_width / 2 + _pos.x, (_height - Render::GetFontHeight("data\\font2.fnt")) / 2 + _pos.y, "data\\font2.fnt", _caption.c_str(), Interface::BUTTON_TEXT);
}

bool IButton::IsMouseOver(const FPoint2D &mousePos) {
	FPoint2D p = mousePos - _pos;
	return (0 <= p.x && p.x < _width 
			&& 0 <= p.y && p.y < _height);
}

void IButton::OnMouseDown(const FPoint2D &mousePos) {
	_state = BUTTON_PRESSED;
	_down = true;
}

void IButton::OnMouseMove(const FPoint2D &mousePos) {
	bool oldValue = IsMouseOver(_oldMousePos);
	_oldMousePos = mousePos;
	bool newValue = IsMouseOver(_oldMousePos);
	if (!oldValue && newValue) {
		OnMouseIn();
	} else if (oldValue && !newValue) {
		OnMouseOut();
	}
}

void IButton::OnMouseUp() {
	if (_down && _state == BUTTON_PRESSED) {
		_state = BUTTON_RELEASED;
		_down = false;
		_luaScript->Execute();
	} else if (_down) {
		_down = false;
	}
}

void IButton::OnMouseIn() {
	if (_down) {
		_state = BUTTON_PRESSED;
	}
}

void IButton::OnMouseOut() {
	if (_down) {
		_state = BUTTON_RELEASED;
	}
}

void IButton::SetValue(const std::string &variableName, const std::string &value) {
	_caption = value;
}