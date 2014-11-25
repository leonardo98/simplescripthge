#ifndef MYENGINE_BASEFIELD_H
#define MYENGINE_BASEFIELD_H

#include "BaseElement.h"
#include "ProductPlace.h"
#include "../Core/Render.h"

class BaseField 
	: public BaseElement
{
public:
	BaseField();
	virtual void DrawBottom();
	virtual void Draw();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	void SomeAction(float persActionWaiting = 0.f);
	PlantPlaceState WhatToDo(std::string &product);
	FPoint2D GetGrandpaPos();
	std::vector<FPoint2D> grandpaPos;
protected:
	float _persActionWaiting;
	void InnerDraw();
	StaticSprite _empty;
	StaticSprite _tab;
	StaticSprite _tabIcon;
	StaticSprite _tabShadow;
	StaticSprite _cut;
	std::vector<StaticSprite> _state;
	int _capacity;
	int _oldCapacity;
	float _effect;
	PlaceType _placeType;
	std::string _productName;
};

#endif//MYENGINE_BASEFIELD_H