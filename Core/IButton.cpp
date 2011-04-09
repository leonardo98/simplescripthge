#include "Render.h"
#include "IButton.h"
#include "Core.h"
#include "Variables.h"

IButton::IButton(TiXmlElement *xe)
	: _luaScript(new LuaScript(Variables::l, xe))
	, Messager(xe)
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	_caption = xe->Attribute("caption"); 
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_state = BUTTON_RELEASED;
	_down = false;
}

void IButton::Draw() {
	if (_state == BUTTON_RELEASED) {
		Render::DrawBar(_pos.x, _pos.y, _width, _height, Interface::BACKGROUND_NORMAL);
		Render::GetDC()->Gfx_RenderLine(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::GetDC()->Gfx_RenderLine(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::GetDC()->Gfx_RenderLine(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_HIGH);
		Render::GetDC()->Gfx_RenderLine(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_HIGH);
		Render::PrintString(_pos.x + _width / 2, _pos.y + _height / 2/* - Interface::Font()->GetHeight() / 2*/, "", _caption.c_str(), Interface::BUTTON_TEXT);
	} else if (_state == BUTTON_PRESSED) {
		Render::DrawBar(_pos.x, _pos.y, _width, _height, Interface::BACKGROUND_PRESSED);
		Render::GetDC()->Gfx_RenderLine(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_LOW);
		Render::GetDC()->Gfx_RenderLine(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_LOW);
		Render::GetDC()->Gfx_RenderLine(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::GetDC()->Gfx_RenderLine(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::PrintString(_pos.x + _width / 2, _pos.y + _height / 2 /*- Interface::Font()->GetHeight() / 2*/, "", _caption.c_str(), Interface::BUTTON_TEXT);
	} else {
		assert(false);
	}
}

bool IButton::IsMouseOver(FPoint2D mousePos) {
	mousePos -= _pos;
	return (0 <= mousePos.x && mousePos.x < _width 
			&& 0 <= mousePos.y && mousePos.y < _height);
}

void IButton::OnMouseDown(FPoint2D mousePos) {
	if (IsMouseOver(mousePos)) {
		_state = BUTTON_PRESSED;
		_down = true;
	}
}

void IButton::OnMouseMove(FPoint2D mousePos) {
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
