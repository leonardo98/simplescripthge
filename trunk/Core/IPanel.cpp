#include "types.h"
#include "IPanel.h"
#include "Variables.h"
#include "..\Gamee\ObjectFactory.h"

IPanel::IPanel(TiXmlElement *xe)
	: Messager(xe)
	, LuaScript(Variables::l)
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_visible = xe->Attribute("visible") == NULL?true:atoi(xe->Attribute("visible"))!=0;
	TiXmlElement *element = xe->FirstChildElement("gui")->FirstChildElement();
	while (element != NULL) {
		_objects.push_back((InputSystem *)ObjectFactory::Create(element));
		RemoveFromList(_objects.back());
		element = element->NextSiblingElement();
	}
	TiXmlElement *keys = xe->FirstChildElement("keys")->FirstChildElement();
	while (keys!= NULL) {
		int key = Core::getKeyCode(keys->Attribute("key"));
		KeyState k;
		TiXmlText *text = (TiXmlText *)keys->FirstChild();
		if (text != NULL) {
			unsigned int size = strlen((char *)text->Value());
			k.code = new char[size + 1];
			strcpy(k.code, (char *)text->Value());
		} else {
			k.code = NULL;
		}
		if (keys->Attribute("msg")) {
			k.msg = keys->Attribute("msg");
		}
		if (keys->Attribute("rsvr")) {
			k.rsvr = keys->Attribute("rsvr");
		}
		_keyStates[key] = k;
		keys = keys->NextSiblingElement();
	}
}

IPanel::~IPanel(void)
{
	for (std::vector<InputSystem *>::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		delete (*i);
		(*i) = NULL;
	}
}

void IPanel::Draw() {
	if (!_visible) {
		return;
	}
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
	for (int i = 0; i < 4; i++) {
		quad.v[i].col = Interface::BACKGROUND;
		quad.v[i].tx = 0;
		quad.v[i].ty = 0;
		quad.v[i].z = 0;
	}
	Core::GetDC()->Gfx_RenderQuad(&quad);
	Core::GetDC()->Gfx_RenderLine(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
	Core::GetDC()->Gfx_RenderLine(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
	Core::GetDC()->Gfx_RenderLine(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_HIGH);
	Core::GetDC()->Gfx_RenderLine(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_HIGH);
	for (std::vector<InputSystem *>::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->Draw();
	}
}

void IPanel::Update(float deltaTime) {
	if (!_visible) {
		return;
	}
	for (std::vector<InputSystem *>::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->Update(deltaTime);
	}
}

bool IPanel::IsMouseOver(hgeVector mousePos) {
	if (!_visible) {
		return false;
	}
	return (_visible && _pos.x < mousePos.x && mousePos.x < _pos.x + _width && _pos.y < mousePos.y && mousePos.y < _pos.y + _height);
}

void IPanel::OnKeyDown(int key) {
	if (_keyStates.find(key) != _keyStates.end()) {
		if (_keyStates[key].code) {
			Read(_keyStates[key].code);
			Execute();
		} else if (_keyStates[key].rsvr.size() > 0) {
			Messager::SendMessage(_keyStates[key].rsvr, _keyStates[key].msg);
		} else {
			OnMessage(_keyStates[key].msg);
		}
	}
}

void IPanel::OnMouseDown(hgeVector mousePos) {
	if (!_visible) {
		return;
	}
	for (std::vector<InputSystem *>::reverse_iterator i = _objects.rbegin(), e = _objects.rend(); i != e; i++) {
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
	for (std::vector<InputSystem *>::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->OnMouseUp();
	}
}

void IPanel::OnMouseMove(hgeVector mousePos) {
	if (!_visible) {
		return;
	}
	for (std::vector<InputSystem *>::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
		(*i)->OnMouseMove(mousePos);
	}
}

void IPanel::OnMessage(const std::string &message) {
	if (message == "hide") {
		_visible = !_visible;
	} 
}

bool IPanel::GetBoolValue(const std::string &variableName) {
	return _visible;
}

void IPanel::SetValue(const std::string &variableName, const bool &value) {
	_visible = value;
}