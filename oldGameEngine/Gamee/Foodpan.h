#ifndef MYENGINE_FOODPAN_H
#define MYENGINE_FOODPAN_H

#include "BaseElement.h"
#include "../Core/Render.h"

class Foodpan 
	: public BaseElement
{
public:
	Foodpan(int index);
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	FPoint2D annaPos;
	FPoint2D bobPos;
	FPoint2D grandpaPos;
	bool Empty();
	void FillFood();
	void EatFood();
	std::string WhatToDo();
protected:
	StaticSprite _shadow;
	StaticSprite _pan;
	std::vector<StaticSprite> _food;
	float _effect;
	std::string _action;
	enum {
		empty,
		level
	} _state;
	int _level;
	const int MAX_LEVELS;
	float _pause;
};

#endif//MYENGINE_FOODPAN_H