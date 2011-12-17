#ifndef MYENGINE_BUTTONLIST_H
#define MYENGINE_BUTTONLIST_H

#include "../Core/InputSystem.h"
#include "../Core/Messager.h"
#include "../Helpers/Counter.h"

class ButtonList :
	public InputSystem, 
	public Messager
{
public:
	ButtonList(TiXmlElement *xe);
	void Draw();
	void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(const FPoint2D &mousePos);

	virtual void OnMessage(const std::string &message);
private:
	FPoint2D _pos;
	float _slideDown;
	FPoint2D _oldMousePos;
	bool _down;
	bool _moving;
	float _stepRight;
	float _stepDown;
	int _itemsInRow;
	float _height;
	float _width;
	typedef std::list<std::pair<std::string, bool> > Items;
	Items _items;
	std::string _receiver;
	Counter _counter;
	float _timer;
	float MOVE_ACTION_TIME;
};

#endif//MYENGINE_BUTTONLIST_H