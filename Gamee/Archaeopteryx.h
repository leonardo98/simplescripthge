#ifndef ARCHAEOPTERYX_H
#define ARCHAEOPTERYX_H

#include "BaseElement.h"
#include "../Core/Animation.h"

class Archaeopteryx : public BaseElement
{
public:
	Archaeopteryx();
	virtual void Draw();
	//virtual void DrawBottom();
	virtual void Update(float dt);
private:
	Animation *_current;
	Animation *_leftFront;
	Animation *_leftFrontIdle;
	Animation *_left;
	Animation *_leftBack;
	Texture *_shadow;
	bool _mirror;
	std::vector<FPoint2D> _region;

	FPoint2D _currentTarget;
	FPoint2D _endTarget;
	FPoint2D _prevTarget;
	const float SPEED;
	float _timeCounter;
	float _actionTimeCounter;
	enum {
		walk,
		idle,
		eat,
		drink,
		happy,
		hangry,
		angry,
		want_eat,
		want_drink
	} _state;
};

#endif//ARCHAEOPTERYX_H