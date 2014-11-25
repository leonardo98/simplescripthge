#ifndef MYENGINE_BASEELEMENT_H
#define MYENGINE_BASEELEMENT_H

#include "../Core/types.h"

class BaseElement {
public:
	FPoint2D _pos;
	virtual ~BaseElement() {}
	BaseElement() : _active(false) , _dead(false) {}
	virtual void Draw() = 0;
	virtual void DrawBottom() {}
	virtual void Update(float dt) {}
	virtual bool IsUnderMouse(const FPoint2D &mousePos) {return false;}
	virtual void OnMouseDown(const FPoint2D &mousePos) {}
	virtual void OnMouseMove(const FPoint2D &mousePos) {}
	virtual void SetActive(bool active) { _active = active; }
	inline bool Dead() { return _dead; }
protected:
	bool _active;
	bool _dead;
};

enum PlantPlaceState {
	no_plant,
	waiting_seed,
	plant_growing1,
	waiting_water,
	plant_growing2,
	waiting_chop,
	plant_growing3,
	waiting_harvest,
	harvesting,
	plant_fade
};

#endif//MYENGINE_BASEELEMENT_H