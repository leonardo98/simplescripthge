// MultiTexture.cpp: implementation of the MultiTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "MultiTexture.h"
#include "../Core/Variables.h"
#include "../Core/Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

State::State(TiXmlElement *xe) {
	name = xe->Attribute("value");
	if (xe->Attribute("mask") != NULL) {
		visible = false;
		pos.x = static_cast<float>(atoi(xe->Attribute("x")));
		pos.y = static_cast<float>(atoi(xe->Attribute("y")));
		texture = Core::getTexture(xe->Attribute("mask"));
		return;
	} else if (xe->Attribute("texture") != NULL) {
		visible = true;
		pos.x = static_cast<float>(atoi(xe->Attribute("x")));
		pos.y = static_cast<float>(atoi(xe->Attribute("y")));
		texture = Core::getTexture(xe->Attribute("texture"));
		return;
	}
	texture = NULL;
}

void State::Draw() {
	if (visible) {
		texture->Render(pos);
	}
}

void State::SetAlpha(float alpha) {
	if (visible) {
		if (alpha < 0.f) {
			alpha = 0;
		} else if (alpha >= 1.f) {
			alpha = 0.999f;
		}
		//texture->SetColor(int(0xFF * alpha) << 24 | 0xFFFFFF);
	}
}

MultiTexture::MultiTexture(TiXmlElement *xe)
	: _luaScript(new LuaScript(Variables::l, xe->FirstChildElement("script")))
{
	_stateVariableName = xe->Attribute("variableName");
	TiXmlElement *ifXml = xe->FirstChildElement("states")->FirstChildElement("if");
	while (ifXml != NULL) {
		_states.push_back(State(ifXml));
		ifXml = ifXml->NextSiblingElement();
	}
}

MultiTexture::~MultiTexture() {
	delete _luaScript;
}

void MultiTexture::Update(float deltaTime) {
}

void MultiTexture::Draw() {
	std::string varvalue = Variables::Get(_stateVariableName.c_str());
	for (States::reverse_iterator i = _states.rbegin(), e = _states.rend(); i != e; i++) {
		if (i->name == varvalue) {
			i->Draw();
		}
	}
}

bool MultiTexture::IsMouseOver(const FPoint2D &mousePos) {
	std::string varvalue = Variables::Get(_stateVariableName.c_str());
	for (States::reverse_iterator i = _states.rbegin(), e = _states.rend(); i != e; i++) {
		FPoint2D t = mousePos - i->pos;
		if (i->name == varvalue) {			
			return (i->texture != NULL && i->texture->IsNotTransparent((int)t.x, (int)t.y)); 
		}
	}
	return false;
}

void MultiTexture::OnMouseDown(const FPoint2D &mousePos) {
	std::string varvalue = Variables::Get(_stateVariableName.c_str());
	for (States::reverse_iterator i = _states.rbegin(), e = _states.rend(); i != e; i++) {
		FPoint2D t = mousePos - i->pos;
		if (i->name == varvalue && i->texture != NULL && i->texture->IsNotTransparent((int)t.x, (int)t.y)) 
		{
			_luaScript->Execute();
			return; 
		}
	}
}
