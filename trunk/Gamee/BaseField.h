#ifndef BASEFIELD_H
#define BASEFIELD_H

#include "BaseElement.h"
#include "../Core/Render.h"

class BaseField 
	: public BaseElement
{
public:
	BaseField();
	virtual void Draw();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
protected:
	void InnerDraw();
	StaticSprite _empty;
	StaticSprite _tab;
	StaticSprite _tabShadow;
	StaticSprite _cut;
	std::vector<StaticSprite> _state;
	int _capacity;
	int _oldCapacity;
	float _effect;
};

#endif//BASEFIELD_H