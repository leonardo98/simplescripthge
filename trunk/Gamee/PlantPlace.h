#ifndef PLANTPLACE_H
#define PLANTPLACE_H

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
private:
	StaticSprite _bedDown;
	StaticSprite _bedUp;
	int _level;
	std::vector<Animation *> _state;
	float _timeCounter;
	float _riseEffect;
	FPoint2D _plantPos;
	float _animationCounter;
	SplinePath _riseUp;
	bool _waitWater;
	float _showProgress;
	int _product;
	CircleProgress _progress;
	PopupMenu _popupMenu;
	bool _plantUnderCursor;
	FPoint2D _lastMousePos;
};

#endif//PLANTPLACE_H