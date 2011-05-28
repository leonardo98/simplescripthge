#include "ISlider.h"
#include "Core.h"
#include "Render.h"
#include "Variables.h"

ISlider::ISlider(TiXmlElement *xe)
	: Messager(xe)
	, _value(0.f)
	, _down(false)
	, _pos(static_cast<float>(atoi(xe->Attribute("x"))), static_cast<float>(atoi(xe->Attribute("y"))))
	, _horizontal(atoi(xe->Attribute("horizontal")) != 0)
	, _length(static_cast<float>(atoi(xe->Attribute("length"))))
	, _width(atoi(xe->Attribute("width")))	
{}

void ISlider::Draw() {
	if (_horizontal) {
		Render::DrawBar(_pos.x, _pos.y, _length, 1, Interface::BORDER_LOW);
		FPoint2D pos(_pos);
		pos.x += _value * _length;
		Render::DrawBar(pos.x - _width / 2, pos.y - _width / 2, static_cast<float>(_width), static_cast<float>(_width), Interface::BACKGROUND_NORMAL);
		Render::Line(pos.x - _width/2, pos.y - _width/2, pos.x + _width/2, pos.y - _width/2, Interface::BORDER_HIGH);
		Render::Line(pos.x + _width/2, pos.y - _width/2, pos.x + _width/2, pos.y + _width/2, Interface::BORDER_LOW);
		Render::Line(pos.x + _width/2, pos.y + _width/2, pos.x - _width/2, pos.y + _width/2, Interface::BORDER_LOW);
		Render::Line(pos.x - _width/2, pos.y + _width/2, pos.x - _width/2, pos.y - _width/2, Interface::BORDER_HIGH);
	} else {
		Render::DrawBar(_pos.x, _pos.y, 1.f, _length, Interface::BORDER_LOW);
		FPoint2D pos(_pos);
		pos.y += _value * _length;
		Render::DrawBar(pos.x - _width / 2, pos.y - _width / 2, static_cast<float>(_width), static_cast<float>(_width), Interface::BACKGROUND_NORMAL);
		Render::Line(pos.x - _width/2, pos.y - _width/2, pos.x + _width/2, pos.y - _width/2, Interface::BORDER_HIGH);
		Render::Line(pos.x + _width/2, pos.y - _width/2, pos.x + _width/2, pos.y + _width/2, Interface::BORDER_LOW);
		Render::Line(pos.x + _width/2, pos.y + _width/2, pos.x - _width/2, pos.y + _width/2, Interface::BORDER_LOW);
		Render::Line(pos.x - _width/2, pos.y + _width/2, pos.x - _width/2, pos.y - _width/2, Interface::BORDER_HIGH);
	}
}

bool ISlider::IsMouseOver(FPoint2D mousePos) {
	mousePos -= _pos;
	if (_horizontal) {
		return ((- _width / 2) <= mousePos.x && mousePos.x < (_length + _width / 2)
				&& (- _width / 2) <= mousePos.y && mousePos.y < (_width / 2));
	} else {
		return ((- _width / 2) <= mousePos.x && mousePos.x < (_width / 2)
				&& (- _width / 2) <= mousePos.y && mousePos.y < (_length + _width / 2));
	}
}

void ISlider::OnMouseDown(FPoint2D mousePos) {
	_down = true;
}

void ISlider::OnMouseMove(FPoint2D mousePos) {
	if (_down) {
#ifndef HGE_COMPILE_KEY
		if (_horizontal) {
			_value = std::min(1.f, std::max(0.f, (mousePos.x - _pos.x) / _length));
		} else {
			_value = std::min(1.f, std::max(0.f, (mousePos.y - _pos.y) / _length));
		}
#else
		if (_horizontal) {
			_value = min(1.f, max(0.f, (mousePos.x - _pos.x) / _length));
		} else {
			_value = min(1.f, max(0.f, (mousePos.y - _pos.y) / _length));
		}
#endif // HGE_COMPILE_KEY
	}
}

void ISlider::OnMouseUp() {
	_down = false;
}

void ISlider::SetValue(const std::string &variableName, const float &value) {
	_value = value;
}

float ISlider::GetNumberValue(const std::string &variableName){
	return _value;
}
