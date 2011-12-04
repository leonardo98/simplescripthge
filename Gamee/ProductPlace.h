#ifndef PRODUCTPLACE_H
#define PRODUCTPLACE_H

#include "BaseElement.h"
#include "../Core/Render.h"

class ProductPlace 
	: public BaseElement
{
public:
	ProductPlace(int index);
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
protected:
	StaticSprite _shadow;
	StaticSprite _place;
	StaticSprite *_product;
	float _effect;
};

#endif//PRODUCTPLACE_H