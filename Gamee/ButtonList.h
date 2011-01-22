#pragma once
#include "..\Core\InputSystem.h"
#include "..\Core\Messager.h"
#include "..\Helpers\Counter.h"

class ButtonList :
	public InputSystem, 
	public Messager
{
public:
	ButtonList(TiXmlElement *xe);
	void Draw();
	void Update(float dt);
	virtual void OnMouseDown(hgeVector mousePos);
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(hgeVector mousePos);

	virtual void OnMessage(const std::string &message);
private:
	hgeVector _pos;
	float _slideDown;
	hgeVector _oldMousePos;
	bool _down;
	bool _moving;
	float _stepRight;
	float _stepDown;
	int _itemsInRow;
	float _height;
	float _width;
	std::vector<std::pair<std::string, bool> > _items;
	std::string _receiver;
	Counter _counter;
	float _timer;
	float MOVE_ACTION_TIME;
};
