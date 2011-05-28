#pragma once
#include "InputSystem.h"
#include "Interface.h"
#include "..\Core\Messager.h"

// класс кнопки(по нажатию выполняется скрипт)
// рисуется примитивами без текстур

class ISlider
	: public Interface
	, public InputSystem
	, public Messager
{
public:
	ISlider(TiXmlElement *xe);
	void Draw();
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(FPoint2D mousePos);

private:
	FPoint2D _pos;
	bool _down;
	bool _horizontal;
	float _length;
	int _width;
	std::string _receiver;
	std::string _variableName;
	float _value;
	virtual void SetValue(const std::string &variableName, const float &value);
	virtual float GetNumberValue(const std::string &variableName);
};
