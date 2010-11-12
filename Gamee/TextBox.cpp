// TextBox.cpp: implementation of the TextBox class.
//
//////////////////////////////////////////////////////////////////////

#include "TextBox.h"
#include "..\Core\Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextBox::TextBox(TiXmlElement *xe)
	: Messager(xe)
	, _clickMousePos(-100, -100) // за экраном
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	_width = atoi(xe->Attribute("width"));
	_hideTime = atof(xe->Attribute("hideTime"));
	_font = new hgeFont(xe->Attribute("font"));
	TiXmlElement *background = xe->FirstChildElement("background");
	_background = Core::getTexture(background->Attribute("texture"));
	_backgroundPos.x = atoi(background->Attribute("x"));
	_backgroundPos.y = atoi(background->Attribute("y"));
}

TextBox::~TextBox()
{
	delete _font;
}

void TextBox::Draw() {
	float y = 50.f;
	if (_show.Action()) {
		float f = 1  - _show.Progress();
		y = y * f * f;
		DWORD alpha = DWORD(255 * (1 - f)) << 24;
		_background->SetColor(alpha | 0x00FFFFFF);
		_font->SetColor(alpha | 0x00FFFFFF);
	} else if (_text.size() > 0 && !_hide.Action()) {
		y = 0;
		_background->SetColor(0xFFFFFFFF);
		_font->SetColor(0xFFFFFFFF);
	} else if (_hide.Action()) {
		y = 0;
		y = 10 * _hide.Progress();
		DWORD alpha = DWORD(255 * (1  - _hide.Progress())) << 24;
		_background->SetColor(alpha | 0x00FFFFFF);
		_font->SetColor(alpha | 0x00FFFFFF);
	} else {
		return;
	}
	_background->Render(_backgroundPos.x, _backgroundPos.y + y);
	for (unsigned int i = 0; i < _text.size(); i++) {
		_font->Render(_pos.x, _pos.y + y, HGETEXT_CENTER, _text[i].c_str());
		y += _font->GetHeight();
	}
}

void TextBox::Update(float deltaTime) {
	if (!_hide.Done() && (_lastMousePos - _clickMousePos).Length() > 10.f) {
		_hide.Update(deltaTime);
		if (_hide.Done()) {
			_text.clear();
			_clickMousePos = hgeVector(-100, -100);
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
		if (_font->GetStringWidth((_text.back() + temp.substr(0, temp.find(' '))).c_str()) <= _width) {
			_text.back() += temp.substr(0, temp.find(' ') + 1);
			temp = temp.substr(temp.find(' ') + 1);
		} else {
			_text.push_back("");
		}
	}
	if (_font->GetStringWidth((_text.back() + temp).c_str()) <= _width) {
		_text.back() += temp;
	} else {
		_text.push_back(temp);
	}
}

bool TextBox::IsMouseOver(hgeVector mousePos) {
	return false;
}

void TextBox::OnMouseMove(hgeVector mousePos) {
	_lastMousePos = mousePos;
}