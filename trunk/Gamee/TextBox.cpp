// TextBox.cpp: implementation of the TextBox class.
//
//////////////////////////////////////////////////////////////////////

#include "TextBox.h"
#include "../Core/Core.h"
#include "../Core/Render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextBox::TextBox(TiXmlElement *xe)
	: Messager(xe)
	, _clickMousePos(-100, -100) // за экраном
{
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_width = static_cast<float>(atoi(xe->Attribute("width")));
	_hideTime = static_cast<float>(atof(xe->Attribute("hideTime")));
	_font = xe->Attribute("font");
	TiXmlElement *background = xe->FirstChildElement("background");
	_background = Core::getTexture(background->Attribute("texture"));
	_backgroundPos.x = static_cast<float>(atof(background->Attribute("x")));
	_backgroundPos.y = static_cast<float>(atof(background->Attribute("y")));
}

TextBox::~TextBox()
{
}

void TextBox::Draw() {
	float y = 50.f;
	if (_show.Action()) {
		float f = 1  - _show.Progress();
		y = y * f * f;
		DWORD alpha = DWORD(255 * (1 - f)) << 24;
		//_background->SetColor(alpha | 0x00FFFFFF);
		_fontColor = (alpha | 0x00FFFFFF);
	} else if (_text.size() > 0 && !_hide.Action()) {
		y = 0;
		//_background->SetColor(0xFFFFFFFF);
		_fontColor = 0xFFFFFFFF;
	} else if (_hide.Action()) {
		y = 0;
		y = 10 * _hide.Progress();
		DWORD alpha = DWORD(255 * (1  - _hide.Progress())) << 24;
		//_background->SetColor(alpha | 0x00FFFFFF);
		_fontColor = alpha | 0x00FFFFFF;
	} else {
		return;
	}
	_background->Render(_backgroundPos.x, _backgroundPos.y + y);
	for (Texts::iterator i = _text.begin(), e = _text.end(); i != e; i++) {
		Render::PrintString(static_cast<int>(_pos.x), static_cast<int>(_pos.y + y), _font, i->c_str(), _fontColor);
		y += Render::GetFontHeight(_font);
	}
}

void TextBox::Update(float deltaTime) {
	if (!_hide.Done() && (_lastMousePos - _clickMousePos).Length() > 10.f) {
		_hide.Update(deltaTime);
		if (_hide.Done()) {
			_text.clear();
			_clickMousePos = FPoint2D(-100, -100);
		}
	}
	_show.Update(deltaTime);
}

void TextBox::OnMessage(const std::string &message) {
	if (_text.size() == 0 || _hide.Action()) {
		_show.Init(0.25f);
	}
	_hide.Init(0.2f, _hideTime);
	_clickMousePos = _lastMousePos;
	_text.clear();
	std::string temp(message);
	_text.push_back("");
	while (temp.find(' ') != std::string::npos) {
		if (Render::GetStringWidth(_font, (_text.back() + temp.substr(0, temp.find(' '))).c_str()) <= _width) {
			_text.back() += temp.substr(0, temp.find(' ') + 1);
			temp = temp.substr(temp.find(' ') + 1);
		} else {
			_text.push_back("");
		}
	}
	if (Render::GetStringWidth(_font, (_text.back() + temp).c_str()) <= _width) {
		_text.back() += temp;
	} else {
		_text.push_back(temp);
	}
}

bool TextBox::IsMouseOver(const FPoint2D &mousePos) {
	return false;
}

void TextBox::OnMouseMove(const FPoint2D &mousePos) {
	_lastMousePos = mousePos;
}