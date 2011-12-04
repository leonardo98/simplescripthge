#ifndef ANNAPERS_H
#define ANNAPERS_H

#include "../Core/Animation.h"
#include "BaseElement.h"

class AnnaPers : public BaseElement
{
public:
	AnnaPers();
	virtual void Draw();
	void Update(float dt);
	void Goto(FPoint2D position);
private:
	Animation *_current;
	Animation *_front;
	Animation *_leftFront;
	Animation *_leftFrontIdle;
	Animation *_left;
	Animation *_back;
	Animation *_leftBack;
	Texture *_shadow;
	bool _mirror;

	FPoint2D _currentTarget;
	FPoint2D _endTarget;
	FPoint2D _prevTarget;
	const float SPEED;
};

#endif//ANNAPERS_H