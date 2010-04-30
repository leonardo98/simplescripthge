#include "IButton.h"
#include "..\Core\Core.h"
#include "..\Core\Variables.h"

IButton::IButton(TiXmlElement *xe)
	: _luaScript(new LuaScript(Variables::l, xe))
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
	hgeQuad quad;
	quad.blend = BLEND_ALPHABLEND;
	quad.tex = NULL;
	quad.v[0].x = _pos.x;
	quad.v[0].y = _pos.y;
	quad.v[1].x = _pos.x + _width;
	quad.v[1].y = _pos.y;
	quad.v[2].x = _pos.x + _width;
	quad.v[2].y = _pos.y + _height;
	quad.v[3].x = _pos.x;
	quad.v[3].y = _pos.y + _height;
	if (_state == BUTTON_RELEASED) {
		for (int i = 0; i < 4; i++) {
			quad.v[i].col = Interface::BACKGROUND_NORMAL;
			quad.v[i].tx = 0;
			quad.v[i].ty = 0;
			quad.v[i].z = 0;
		}
		Core::GetDC()->Gfx_RenderQuad(&quad);
		Core::GetDC()->Gfx_RenderLine(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Core::GetDC()->Gfx_RenderLine(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Core::GetDC()->Gfx_RenderLine(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_HIGH);
		Core::GetDC()->Gfx_RenderLine(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_HIGH);
		Interface::Font()->SetColor(Interface::BUTTON_TEXT);
		Interface::Font()->Render(_pos.x + _width / 2, _pos.y + _height / 2 - Interface::Font()->GetHeight() / 2, HGETEXT_CENTER, _caption.c_str());
	} else if (_state == BUTTON_PRESSED) {
		for (int i = 0; i < 4; i++) {
			quad.v[i].col = Interface::BACKGROUND_PRESSED;
			quad.v[i].tx = 0;
			quad.v[i].ty = 0;
			quad.v[i].z = 0;
		}
		Core::GetDC()->Gfx_RenderQuad(&quad);
		Core::GetDC()->Gfx_RenderLine(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_LOW);
		Core::GetDC()->Gfx_RenderLine(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_LOW);
		Core::GetDC()->Gfx_RenderLine(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Core::GetDC()->Gfx_RenderLine(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Interface::Font()->SetColor(Interface::BUTTON_TEXT);
		Interface::Font()->Render(_pos.x + _width / 2, _pos.y + _height / 2 - Interface::Font()->GetHeight() / 2, HGETEXT_CENTER, _caption.c_str());
	} else {
		assert(false);
	}
}

bool IButton::IsMouseOver(hgeVector mousePos) {
	mousePos -= _pos;
	return (0 <= mousePos.x && mousePos.x < _width 
			&& 0 <= mousePos.y && mousePos.y < _height);
}

void IButton::OnMouseDown(hgeVector mousePos) {
	if (IsMouseOver(mousePos)) {
		_state = BUTTON_PRESSED;
		_down = true;
	}
}

void IButton::OnMouseMove(hgeVector mousePos) {
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
