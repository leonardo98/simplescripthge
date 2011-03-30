#include "ButtonList.h"
#include "..\Core\Core.h"
#include "..\Core\Interface.h"

#define OFFSET ((_items.size() / _itemsInRow > 3) ? 4.f * _stepDown - (_stepDown * (_items.size() / _itemsInRow) + _height) : 0.f)

ButtonList::ButtonList(TiXmlElement *xe)
	: Messager(xe)
	, MOVE_ACTION_TIME(0.2f)
	, _moving(false)
	, _slideDown(0.f)
	, _timer(0.f)
{
	_receiver = xe->Attribute("receiver");
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	_down = false;
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_stepDown = atoi(xe->Attribute("stepDown"));
	_itemsInRow = atoi(xe->Attribute("itemsInRow"));
	assert(_itemsInRow > 0);
	if (_itemsInRow > 1) {
		_stepRight = atoi(xe->Attribute("stepRight"));
	}
}

void ButtonList::Draw() {
	Interface::Font()->SetColor(Interface::BUTTON_TEXT);
	for (unsigned int i = 0; i < _items.size(); ++i) {
		FPoint2D pos = _pos;
		pos.x += _stepRight * (i % _itemsInRow);
		pos.y += _stepDown * (i / _itemsInRow) + _slideDown;
		if (_items[i].second) {
			Core::DrawBar(pos.x, pos.y, _width, _height, 0xFFFF9F9F);
		} else {
			Core::DrawBar(pos.x, pos.y, _width, _height, 0xFF7F7F7F);
		}
		Interface::Font()->Render(pos.x + _width / 2, pos.y + _height / 2 - Interface::Font()->GetHeight() / 2, HGETEXT_CENTER, _items[i].first.c_str());		
	}
}

void ButtonList::OnMessage(const std::string &message) {
	std::string msg;
	if (message == "clear") {
		_items.clear();
	} else if (CanCut(message, "add ", msg)) {
		_items.push_back(std::make_pair(msg, false));
	} else if (CanCut(message, "special add ", msg)) {
		_items.push_back(std::make_pair(msg, true));
	}
}


bool ButtonList::IsMouseOver(FPoint2D mousePos) {
	return _items.size() > 0;
}

void ButtonList::OnMouseDown(FPoint2D mousePos) {
	if (IsMouseOver(mousePos)) {
		_down = true;
		_timer = 0.f;
	}
	_oldMousePos = mousePos;
	_counter.Init(0.f);
}

void ButtonList::OnMouseMove(FPoint2D mousePos) {
	if (_down && (_moving || (!_moving && abs(mousePos.y - _oldMousePos.y) > 15.f) || _timer > MOVE_ACTION_TIME)) {
		_moving = true;
		_slideDown += (mousePos.y - _oldMousePos.y);
		_oldMousePos = mousePos;
	}
}

void ButtonList::OnMouseUp() {
	if (_down && !_moving) {
		_down = false;
		// значит тапнули:)
		for (unsigned int i = 0; i < _items.size(); ++i) {
			FPoint2D pos = _pos;
			pos.x += _stepRight * (i % _itemsInRow);
			pos.y += _stepDown * (i / _itemsInRow) + _slideDown;
			if (_oldMousePos.x >= pos.x && _oldMousePos.x <= (pos.x + _width) && 
				_oldMousePos.y >= pos.y && _oldMousePos.y <= (pos.y + _height)) {
				SendMessage(_receiver, "button pressed " + _items[i].first);
				return;
			}
		}
	} else {
		_moving = false;
		_down = false;
	}
	if (_slideDown > 0.f || _slideDown < OFFSET) {
		_counter.Init(1.5f);
	}
}

void ButtonList::Update(float dt) {
	if (_down) {
		if (dt > 0.07f) {
			_timer += 0.07f;
		} else {
			_timer += dt;
		}
	}
	if (_counter.Action()) {
		_counter.Update(dt);
		float t = OFFSET;
		if (_counter.Done()) {
			if (_slideDown > 0.f) {
				_slideDown = 0.f;
			} else if (_slideDown < t) {
				_slideDown = t;
			}
			return;
		}
		if (_slideDown > 0.f) {
			_slideDown = (1.f - sin(_counter.Progress() * M_PI_2)) * _slideDown;
		} else if (_slideDown < t) {
			_slideDown -= t;
			_slideDown = (1.f - sin(_counter.Progress() * M_PI_2)) * _slideDown;
			_slideDown += t;
		}
	}
}