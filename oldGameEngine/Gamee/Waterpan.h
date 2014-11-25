#ifndef MYENGINE_WATERPAN_H
#define MYENGINE_WATERPAN_H

#include "BaseElement.h"
#include "../Core/Render.h"

class Waterpan 
	: public BaseElement
{
public:
	Waterpan(int index);
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	bool Empty();
	FPoint2D annaPos;
	FPoint2D bobPos;
	FPoint2D grandpaPos;
	void FillWater();
	void DrinkWater();
protected:
	StaticSprite _shadow;
	StaticSprite _down;
	StaticSprite _up;
	StaticSprite _water;
	float _effect;
	float _oldLevel;
	float _level;
	float _pause;
};

#endif//MYENGINE_WATERPAN_H