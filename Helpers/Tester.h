// Tester.h: interface for the Tester class.
//
//////////////////////////////////////////////////////////////////////

#ifndef TESTER_H
#define TESTER_H

#include "../Core/Object.h"
#include "../Core/Render.h"

class Tester : public Object  
{
public:
	Tester(TiXmlElement *xe);
	virtual ~Tester();
	void Draw();
	void Update(float deltaTime);
private:
	FPoint2D _pos;
	float _time;
	float _timeCounter;
	float _acc;
	float _speed;
	float _x;
	float _x0;
	Texture *_texture;
};

#endif//TESTER_H