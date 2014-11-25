#ifndef BYKER_H
#define BYKER_H

#include "Box2D/Box2D.h"
#include "../Core/types.h"
#include "../2dga_api/Animation.h"
#include "../Helpers/LittleHero.h"

class Byker {
public:
	Byker();
	~Byker();
	void Draw(float angle);
	void Update(float dt);
	void SetPos(FPoint2D pos);
	//b2Body *_attachedBody;
	//b2Body *_attachedBody2;
	//b2Body *_rama;
	//b2Body *_head;
	FPoint2D _pos;
	LittleHero physic;
private:
	My::Animation *_ride_wheels;
	My::Animation *_byker_balans_2;
	float _timer;
};

#endif//BYKER_H