#ifndef BYKER_H
#define BYKER_H

#include "Box2D/Box2D.h"
#include "../Core/types.h"
#include "../2dga_api/Animation.h"

class Byker {
public:
	Byker();
	~Byker();
	void Draw();
	void Update(float dt);
	void SetPos(FPoint2D pos);
	b2Body *_attachedBody;
	b2Body *_attachedBody2;
	b2Body *_rama;
	b2Body *_head;
private:
	FPoint2D _pos;
	My::Animation *_ride;
	float _timer;
};

#endif//BYKER_H