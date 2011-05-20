#include "IRadioBtns.h"
#include "Render.h"
#include "..\Core\Core.h"
#include "..\Core\Interface.h"

IRadioBtns::IRadioBtns(TiXmlElement *xe)
	: Messager(xe)
	, _selected(0)
{
	_receiver = xe->Attribute("receiver");
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_width = static_cast<float>(atoi(xe->Attribute("width")));
	_height = static_cast<float>(atoi(xe->Attribute("height")));
	_stepDown = static_cast<float>(atoi(xe->Attribute("stepDown")));
	_itemsInRow = atoi(xe->Attribute("itemsInRow"));
	assert(_itemsInRow > 0);
	if (_itemsInRow > 1) {
		_stepRight = static_cast<float>(atoi(xe->Attribute("stepRight")));
	}
}

void IRadioBtns::Draw() {
	unsigned int counter = 0;
	for (Items::iterator i = _items.begin(), e = _items.end(); i != e; ++i) {
		FPoint2D pos = _pos;
		pos.x += _stepRight * (counter % _itemsInRow);
		pos.y += _stepDown * (counter / _itemsInRow);
		if (counter == _selected) {
			Render::DrawBar(pos.x, pos.y, _width, _height, 0xFFFF9F9F);
		} else {
			Render::DrawBar(pos.x, pos.y, _width, _height, 0xFF7F7F7F);
		}
		Render::PrintString(static_cast<int>(pos.x + _width / 2), static_cast<int>(pos.y + _height / 2)/* - Interface::Font()->GetHeight() / 2*/, "",i->c_str(), Interface::BUTTON_TEXT);		
		++counter;
	}
}

void IRadioBtns::OnMessage(const std::string &message) {
	std::string msg;
	if (message == "clear") {
		_items.clear();
		_selected = 0;
	} else if (CanCut(message, "add ", msg)) {
		_items.push_back(msg);
	}
}

bool IRadioBtns::IsMouseOver(FPoint2D mousePos) {
	for (unsigned int i = 0; i < _items.size(); ++i) {
		FPoint2D pos = _pos;
		pos.x += _stepRight * (i % _itemsInRow);
		pos.y += _stepDown * (i / _itemsInRow);
		if (mousePos.x >= pos.x && mousePos.x <= (pos.x + _width) && 
			mousePos.y >= pos.y && mousePos.y <= (pos.y + _height)) {
			return true;
		}
	}
	return false;
}

void IRadioBtns::OnMouseDown(FPoint2D mousePos) {
	unsigned int old = _selected;
	for (unsigned int i = 0; i < _items.size(); ++i) {
		FPoint2D pos = _pos;
		pos.x += _stepRight * (i % _itemsInRow);
		pos.y += _stepDown * (i / _itemsInRow);
		if (mousePos.x >= pos.x && mousePos.x <= (pos.x + _width) && 
			mousePos.y >= pos.y && mousePos.y <= (pos.y + _height)) {
				_selected = i;
				if (old != _selected) {
					SendMessage(_receiver, "changes");
				}
			return;
		}
	}
}

void IRadioBtns::SetValue(const std::string &variableName, const float &value) {
	_selected = static_cast<int>(value);
}

float IRadioBtns::GetNumberValue(const std::string &variableName){
	return static_cast<float>(_selected);
}
