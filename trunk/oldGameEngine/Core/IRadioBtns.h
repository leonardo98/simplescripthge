#ifndef MYENGINE_IRADIOBTNS_H
#define MYENGINE_IRADIOBTNS_H

#include "../Core/InputSystem.h"
#include "../Core/Messager.h"

class IRadioBtns :
	public InputSystem, 
	public Messager
{
public:
	IRadioBtns(TiXmlElement *xe);
	void Draw();
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsMouseOver(const FPoint2D &mousePos);

	virtual void OnMessage(const std::string &message);

	virtual void SetValue(const std::string &variableName, const float &value);
	virtual float GetNumberValue(const std::string &variableName);

private:
	FPoint2D _pos;
	float _stepRight;
	float _stepDown;
	int _itemsInRow;
	float _height;
	float _width;
	typedef std::list<std::string> Items;
	Items _items;
	unsigned int _selected;
	std::string _receiver;
};

#endif//MYENGINE_IRADIOBTNS_H