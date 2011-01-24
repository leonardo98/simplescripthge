#pragma once
#include "..\Core\InputSystem.h"
#include "..\Core\Messager.h"

class IRadioBtns :
	public InputSystem, 
	public Messager
{
public:
	IRadioBtns(TiXmlElement *xe);
	void Draw();
	virtual void OnMouseDown(hgeVector mousePos);
	virtual bool IsMouseOver(hgeVector mousePos);

	virtual void OnMessage(const std::string &message);

	virtual void SetValue(const std::string &variableName, const float &value);
	virtual float GetNumberValue(const std::string &variableName);

private:
	hgeVector _pos;
	float _stepRight;
	float _stepDown;
	int _itemsInRow;
	float _height;
	float _width;
	int _selected;
	std::vector<std::string> _items;
	std::string _receiver;
};
