#ifndef TREEPLACE_H
#define TREEPLACE_H

#include "../Core/Matrix.h"
#include "BaseElement.h"
#include "../Core/Render.h"
#include "../Core/Animation.h"
#include "CircleProgress.h"
#include "PopupMenu.h"
#include "ServingElement.h"

enum TreePlaceState {
	tps_no_plant,
	tps_waiting_seed,
	tps_plant_growing1,
	tps_waiting_water,
	tps_plant_growing2,
	tps_waiting_harvest,
	tps_plant_fade
};

class TreePlace 
	: public BaseElement
	, public ServingElement
{
public:
	TreePlace(int placeType);
	void Set(const std::string &treeType);
	virtual void DrawBottom();
	virtual void Draw();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	virtual void OnMouseMove(const FPoint2D &mousePos);
	FPoint2D annaPos;
	FPoint2D bobPos;
	FPoint2D grandpaPos;
	void SomeAction();
	TreePlaceState WhatToDo(std::string &product);
private:
	StaticSprite _bedDown;
	StaticSprite _bedUp;
	int _level;
	std::vector<Animation *> _levels;
	float _timeCounter;
	float _riseEffect;
	FPoint2D _treePos;
	float _animationCounter;
	SplinePath _riseUp;
	SplinePath _riseUp2;
	SplinePath _riseUp3;
	CircleProgress _progress;
	PopupMenu _popupMenu;
	std::string _productType;
	float _shakeTimeCounter;
	FPoint2D _lastMousePos;
	bool _treeUnderCursor;

	TreePlaceState _state;
	float _waitingCounter;
	float _fadeCounter;
	float _showWaiingActionIcon;
	float WATER_TIME;
	float CHOP_TIME;
};

#endif//TREEPLACE_H