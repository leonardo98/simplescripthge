#ifndef BASEELEMENT_H
#define BASEELEMENT_H

#include "../Core/types.h"

class BaseElement {
public:
	FPoint2D _pos;
	BaseElement() : _active(false) {}
	virtual void Draw() = 0;
	virtual void DrawBottom() {}
	virtual void Update(float dt) {}
	virtual bool IsUnderMouse(const FPoint2D &mousePos) {return false;}
	virtual void OnMouseDown(const FPoint2D &mousePos) {}
	virtual void OnMouseMove(const FPoint2D &mousePos) {}
	virtual void SetActive(bool active) { _active = active; }
protected:
	bool _active;
};

#endif//BASEELEMENT_H