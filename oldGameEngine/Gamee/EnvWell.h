#ifndef MYENGINE_ENVWELL_H
#define MYENGINE_ENVWELL_H

#include "BaseElement.h"
#include "../Core/Render.h"

class EnvWell 
	: public BaseElement
{
public:
	EnvWell();
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	FPoint2D annaPos;
	FPoint2D bobPos;
	FPoint2D grandpaPos;
protected:
	StaticSprite _shadow;
	StaticSprite _well;
	StaticSprite _buck;
	bool _drawBuck;
	float _effect;
};

#endif//MYENGINE_ENVWELL_H