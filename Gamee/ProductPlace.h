#ifndef PRODUCTPLACE_H
#define PRODUCTPLACE_H

#include "BaseElement.h"
#include "ServingElement.h"
#include "../Core/Render.h"

enum PlaceType {
	pt_tree,
	pt_clover,
	pt_grain,
	pt_unknown,
	pt_free1,
	pt_free2,
	pt_water
};


class ProductPlace 
	: public BaseElement
	, public ServingElement
{
public:
	ProductPlace(PlaceType pt);
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	bool IsBusy();
	void SetProduct(const std::string &product, bool mirrorEffect);
	std::string GetProduct();
	std::string WhatToDo();
	FPoint2D annaPos;
	FPoint2D bobPos;
	FPoint2D grandpaPos;
protected:
	StaticSprite _shadow;
	StaticSprite _place;
	StaticSprite *_product;
	PlaceType _placeType;
	float _effect;
	std::string _productType;
	bool _mirrorEffect;
};

#endif//PRODUCTPLACE_H