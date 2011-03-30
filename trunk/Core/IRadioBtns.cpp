#include "IRadioBtns.h"
#include "..\Core\Core.h"
#include "..\Core\Interface.h"

IRadioBtns::IRadioBtns(TiXmlElement *xe)
	: Messager(xe)
	, _selected(0)
{
	_receiver = xe->Attribute("receiver");
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_stepDown = atoi(xe->Attribute("stepDown"));
	_itemsInRow = atoi(xe->Attribute("itemsInRow"));
	assert(_itemsInRow > 0);
	if (_itemsInRow > 1) {
		_stepRight = atoi(xe->Attribute("stepRight"));
	}
}

void IRadioBtns::Draw() {
	Interface::Font()->SetColor(Interface::BUTTON_TEXT);
	for (unsigned int i = 0; i < _items.size(); ++i) {
		FPoint2D pos = _pos;
		pos.x += _stepRight * (i % _itemsInRow);
		pos.y += _stepDown * (i / _itemsInRow);
		if (i == _selected) {
			Core::DrawBar(pos.x, pos.y, _width, _height, 0xFFFF9F9F);
		} else {
			Core::DrawBar(pos.x, pos.y, _width, _height, 0xFF7F7F7F);
		}
		Interface::Font()->Render(pos.x + _width / 2, pos.y + _height / 2 - Interface::Font()->GetHeight() / 2, HGETEXT_CENTER, _items[i].c_str());		
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
	int old = _selected;
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
	_selected = value;
}

float IRadioBtns::GetNumberValue(const std::string &variableName){
	return _selected;
}
