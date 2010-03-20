// MultiTexture.cpp: implementation of the MultiTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "MultiTexture.h"
#include "Variables.h"
#include "Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

State::State(TiXmlElement *xe) {
	name = xe->Attribute("value");
	if (xe->Attribute("mask") != NULL) {
		visible = false;
		pos.x = atoi(xe->Attribute("x"));
		pos.y = atoi(xe->Attribute("y"));
		texture = Core::getTexture(xe->Attribute("mask"));
		return;
	} else if (xe->Attribute("texture") != NULL) {
		visible = true;
		pos.x = atoi(xe->Attribute("x"));
		pos.y = atoi(xe->Attribute("y"));
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
		texture->SetColor(int(0xFF * alpha) << 24 | 0xFFFFFF);
	}
}

MultiTexture::MultiTexture(TiXmlElement *xe)
	:  _parser(xe->FirstChildElement("script"))
{
	std::string variableName = xe->Attribute("variableName");
	_stateVariable = Variables::GetLink(variableName);
	TiXmlElement *ifXml = xe->FirstChildElement("states")->FirstChildElement("if");
	while (ifXml != NULL) {
		_states.push_back(State(ifXml));
		ifXml = ifXml->NextSiblingElement();
	}
	_stateName = _stateVariable->GetValue();
}

MultiTexture::~MultiTexture() {

}

void MultiTexture::Update(float deltaTime) {
	if (_counter.Action()) {
		_counter.Update(deltaTime);
		if (_counter.Done()) {
			_stateName = _stateVariable->GetValue();
		}
	} 
}

void MultiTexture::Draw() {
	if (!_counter.Action() && _stateName != _stateVariable->GetValue()) {
		_counter.Init(0.0f);
	}
	if (_counter.Action()) {
		for (int i = _states.size() - 1; i >= 0; i--) {
			if (_states[i].name == _stateVariable->GetValue()) {
				_states[i].SetAlpha(_counter.Progress());
				_states[i].Draw();
				//_states[i].SetAlpha(1.f);
			} 
			if (_states[i].name == _stateName) {
				_states[i].SetAlpha(1 - _counter.Progress());
				_states[i].Draw();
				//_states[i].SetAlpha(1.f);
			}
		}
		return;
	} else {
		for (int i = _states.size() - 1; i >= 0; i--) {
			if (_states[i].name == _stateVariable->GetValue()) {
				_states[i].Draw();
				return;
			}
		}
	}
	// сюда не должен доходить
	{
		for (int i = _states.size() - 1; i >= 0; i--) {
			if (_states[i].name == _stateVariable->GetValue()) {
				_states[i].texture->SetColor(0xFF000000);
				_states[i].Draw();
				_states[i].texture->SetColor(0xFFFFFFFF);
			}
		}
	}
}

bool MultiTexture::IsMouseOver(hgeVector mousePos) {
	for (int i = _states.size() - 1; i >= 0; i--) {
		hgeVector t = mousePos - _states[i].pos;
		if (_states[i].name == _stateVariable->GetValue()) {			
			return (_states[i].texture != NULL && _states[i].texture->IsNotTransparent((int)t.x, (int)t.y)); 
		}
	}
	return false;
}

void MultiTexture::OnMouseDown(hgeVector mousePos) {
	if (_counter.Action()) {
		return;
	}
	for (int i = _states.size() - 1; i >= 0; i--) {
		hgeVector t = mousePos - _states[i].pos;
		if (_states[i].name == _stateVariable->GetValue() && _states[i].texture != NULL && _states[i].texture->IsNotTransparent((int)t.x, (int)t.y)) 
		{
			_parser.Execute();
			return; 
		}
	}
}
