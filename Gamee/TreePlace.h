#ifndef TREEPLACE_H
#define TREEPLACE_H

#include "../Core/Matrix.h"
#include "BaseElement.h"
#include "../Core/Render.h"
#include "../Core/Animation.h"
#include "CircleProgress.h"
#include "PopupMenu.h"

class TreePlace 
	: public BaseElement
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
private:
	StaticSprite _bedDown;
	StaticSprite _bedUp;
	int _level;
	std::vector<Animation *> _state;
	float _timeCounter;
	float _riseEffect;
	FPoint2D _treePos;
	float _animationCounter;
	SplinePath _riseUp;
	SplinePath _riseUp2;
	SplinePath _riseUp3;
	bool _waitWater;
	float _showProgress;
	int _product;
	CircleProgress _progress;
	PopupMenu _popupMenu;
	std::string _debug_type;
	float _shakeTimeCounter;
	FPoint2D _lastMousePos;
	bool _treeUnderCursor;
};

#endif//TREEPLACE_H