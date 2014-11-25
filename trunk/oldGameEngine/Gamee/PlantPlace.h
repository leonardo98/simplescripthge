#ifndef MYENGINE_PLANTPLACE_H
#define MYENGINE_PLANTPLACE_H

#include "../Core/Matrix.h"
#include "BaseElement.h"
#include "../Core/Render.h"
#include "../Core/Animation.h"
#include "CircleProgress.h"
#include "PopupMenu.h"

class PlantPlace 
	: public BaseElement
{
public:
	PlantPlace(int placeType);
	void Set(const std::string &plantType);
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseMove(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	FPoint2D annaPos;
	FPoint2D bobPos;
	FPoint2D grandpaPos;
	void SomeAction(float persActionWaiting = 0.f);
	PlantPlaceState WhatToDo(std::string &product);
private:
	StaticSprite _bedDown;
	StaticSprite _bedUp;
	int _level;
	std::vector<Animation *> _levels;
	float _timeCounter;
	float _riseEffect;
	FPoint2D _plantPos;
	float _animationCounter;
	SplinePath _riseUp;
	CircleProgress _progress;
	PopupMenu _popupMenu;
	bool _plantUnderCursor;
	FPoint2D _lastMousePos;
	std::string _plantType;
	float _persActionWaiting;

	PlantPlaceState _state;
	float _waitingCounter;
	float _fadeCounter;
	float _showWaiingActionIcon;
	float WATER_TIME;
	float CHOP_TIME;
};

#endif//MYENGINE_PLANTPLACE_H