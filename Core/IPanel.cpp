#include "types.h"
#include "IPanel.h"
#include "Render.h"
#include "Variables.h"
#include "CoreFactory.h"
#include "../Gamee/ObjectFactory.h"

IPanel::IPanel(TiXmlElement *xe)
	: Messager(xe)
	, LuaScript(Variables::l)
{
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_visible = xe->Attribute("visible") == NULL?true:atoi(xe->Attribute("visible"))!=0;
	_needDraw = xe->Attribute("needDraw") == NULL?true:atoi(xe->Attribute("needDraw"))!=0;
	TiXmlElement *element = xe->FirstChildElement("gui")->FirstChildElement();
	while (element != NULL) {
		_objects.push_back((InputSystem *)ObjectFactory::Create(element));
		RemoveFromList(_objects.back());
		element = element->NextSiblingElement();
	}
}

IPanel::~IPanel(void)
{
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		delete (*i);
		(*i) = NULL;
	}
}

void IPanel::Draw() {
	if (!_visible) {
		return;
	}
	if (_needDraw) {
		Render::DrawBar(_pos.x, _pos.y, static_cast<float>(_width), static_cast<float>(_height), Interface::BACKGROUND);
		Render::Line(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::Line(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
		Render::Line(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_HIGH);
		Render::Line(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_HIGH);
	}
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->Draw();
	}
}

void IPanel::Update(float deltaTime) {
	if (!_visible) {
		return;
	}
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->Update(deltaTime);
	}
}

bool IPanel::IsMouseOver(const FPoint2D &mousePos) {
	if (!_visible) {
		return false;
	}
	if (_visible && _pos.x < mousePos.x && mousePos.x < _pos.x + _width && _pos.y < mousePos.y && mousePos.y < _pos.y + _height) {
		return true;
	} else {
		for (Objects::reverse_iterator i = _objects.rbegin(), e = _objects.rend(); i != e; i++) {
			if ((*i)->IsMouseOver(mousePos)) {
				return true;
			}
		}
		return false;
	} 
}

void IPanel::OnMouseDown(const FPoint2D &mousePos) {
	if (!_visible) {
		return;
	}
	for (Objects::reverse_iterator i = _objects.rbegin(), e = _objects.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnMouseDown(mousePos);
			return;
		}
	}
}

void IPanel::OnMouseUp() {
	if (!_visible) {
		return;
	}
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->OnMouseUp();
	}
}

void IPanel::OnMouseMove(const FPoint2D &mousePos) {
	if (!_visible) {
		return;
	}
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->OnMouseMove(mousePos);
	}
}

void IPanel::OnMessage(const std::string &message) {
	if (message == "changestate") {
		_visible = !_visible;
	} else if (message == "show") {
		_visible = true;
	} else if (message == "hide") {
		_visible = false;
	}
}

bool IPanel::GetBoolValue(const std::string &variableName) {
	return _visible;
}

void IPanel::SetValue(const std::string &variableName, const bool &value) {
	_visible = value;
}