#ifndef WATERPAN_H
#define WATERPAN_H

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
	FPoint2D annaPos;
	FPoint2D bobPos;
	FPoint2D grandpaPos;
	void SomeAction();
protected:
	StaticSprite _shadow;
	StaticSprite _down;
	StaticSprite _up;
	StaticSprite _water;
	float _effect;
	const int MAX_LEVELS;
	float _oldLevel;
	float _level;
};

#endif//WATERPAN_H