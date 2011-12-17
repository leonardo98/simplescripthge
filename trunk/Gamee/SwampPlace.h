#ifndef MYENGINE_SWAMPPLACE_H
#define MYENGINE_SWAMPPLACE_H

#include "BaseElement.h"
#include "../Core/Render.h"

class SwampPlace : public BaseElement 
{
public:
	SwampPlace();
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseMove(const FPoint2D &mousePos);
private:
	StaticSprite _grass;
	StaticSprite _swamp;
	StaticSprite _tabDn;
	StaticSprite _tabUp;
	bool _isUnderMouse;
	float _shakeTimeCounter;
};

#endif//MYENGINE_SWAMPPLACE_H