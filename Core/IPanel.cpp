#include "types.h"
#include "IPanel.h"
#include "Render.h"
#include "Variables.h"
#include "..\Gamee\ObjectFactory.h"

IPanel::IPanel(TiXmlElement *xe)
	: Messager(xe)
	, LuaScript(Variables::l)
{
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	Show(xe->Attribute("visible") == NULL?true:atoi(xe->Attribute("visible"))!=0);
	_needDraw = xe->Attribute("needDraw") == NULL?true:atoi(xe->Attribute("needDraw"))!=0;
	TiXmlElement *element = xe->FirstChildElement("gui")->FirstChildElement();
	while (element != NULL) {
		InputSystem *obj = (InputSystem *)ObjectFactory::Create(element);
		obj->SetSystemInput(false); //Елементы панели обрабатывает сама панель
		_objects.push_back(obj);
//		RemoveFromList(_objects.back()); //избавляемся от плохой функции
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
	if (!isVisible()) {
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
	if (!isVisible()) {
		return;
	}
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->Update(deltaTime);
	}
}

bool IPanel::IsMouseOver(FPoint2D mousePos) {
	if (!isVisible()) {
		return false;
	}
	return (isVisible() && _pos.x < mousePos.x && mousePos.x < _pos.x + _width && _pos.y < mousePos.y && mousePos.y < _pos.y + _height);
}

void IPanel::OnMouseDown(FPoint2D mousePos) {
	if (!isVisible()) {
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
	if (!isVisible()) {
		return;
	}
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->OnMouseUp();
	}
}

void IPanel::OnMouseMove(FPoint2D mousePos) {
	if (!isVisible()) {
		return;
	}
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->OnMouseMove(mousePos);
	}
}

void IPanel::OnMessage(const std::string &message) {
	if (message == "hide") {
		Show(!isVisible());
	} 
}

bool IPanel::GetBoolValue(const std::string &variableName) {
	return isVisible();
}

void IPanel::SetValue(const std::string &variableName, const bool &value) {
	Show(value);
}