#include "../Core/Render.h"
#include "MyMessageBox.h"
#include "../Core/Core.h"
#include "../Core/Variables.h"

MyMessageBox::~MyMessageBox() {
	_myMessageBox = NULL;
}

MyMessageBox::MyMessageBox(TiXmlElement *xe)
: Messager("MessageBox")
, _yes(xe->FirstChildElement("Yes"))
, _no(xe->FirstChildElement("No"))
, _ok(xe->FirstChildElement("Ok"))
{
	RemoveFromList((InputSystem *)&_yes);
	RemoveFromList((InputSystem *)&_no);
	RemoveFromList((InputSystem *)&_ok);
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_textY = atoi(xe->Attribute("textY"));
	_myMessageBox = this;
}

void MyMessageBox::Draw() {
	if (_buttons.size() == 0) {
		return;
	}
	Render::DrawBar(_pos.x, _pos.y, static_cast<float>(_width), static_cast<float>(_height), Interface::BACKGROUND_NORMAL);
	Render::Line(_pos.x + _width, _pos.y, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
	Render::Line(_pos.x, _pos.y + _height, _pos.x + _width, _pos.y + _height, Interface::BORDER_LOW);
	Render::Line(_pos.x - 1, _pos.y, _pos.x + _width, _pos.y, Interface::BORDER_HIGH);
	Render::Line(_pos.x, _pos.y, _pos.x, _pos.y + _height, Interface::BORDER_HIGH);
	Render::PrintString(static_cast<int>(_pos.x + _width / 2), static_cast<int>(_pos.y + _textY)/* - Interface::Font()->GetHeight() / 2*/, "", _text.c_str(), Interface::BUTTON_TEXT);
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		_buttons[i]->Draw();
	}
}

bool MyMessageBox::IsMouseOver(const FPoint2D &mousePos) {
	return (_buttons.size() > 0);
}

void MyMessageBox::OnMouseDown(const FPoint2D &mousePos) {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		_buttons[i]->OnMouseDown(mousePos);
	}
}

void MyMessageBox::OnMouseMove(const FPoint2D &mousePos) {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		_buttons[i]->OnMouseMove(mousePos);
	}
}

void MyMessageBox::OnMouseUp() {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		_buttons[i]->OnMouseUp();
	}
}

void MyMessageBox::SetValue(const std::string &variableName, const std::string &value) {
	if (variableName == "question") {
		_text = value;
	} else if (variableName == "answer") {
		_answerReceiver = value;
	} else if (variableName == "buttons") {
		if (value == "yesno") {
			_buttons.clear();
			_buttons.push_back(&_yes);
			_buttons.push_back(&_no);
		} else {
			_buttons.clear();
			_buttons.push_back(&_ok);
		}
	} else {
		assert(false);
	}
}

void MyMessageBox::OnMessage(const std::string &message) {
	_buttons.clear();
	SendMessage(_answerReceiver, message);
}

MyMessageBox *MyMessageBox::_myMessageBox = NULL;

void MyMessageBox::Show(const std::string &text, const std::string &buttons, const std::string &receiver) {
	//assert(_myMessageBox != NULL);
	Messager::SetValueS("MessageBox", "question", text);
	Messager::SetValueS("MessageBox", "answer", receiver);
	Messager::SetValueS("MessageBox", "buttons", buttons);
}