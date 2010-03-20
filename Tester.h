// Tester.h: interface for the Tester class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_Tester_INCLUDED_)
#define _Tester_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"

class Tester : public Object  
{
public:
	Tester(TiXmlElement *xe);
	virtual ~Tester();
	void Draw();
	void Update(float deltaTime);
private:
	hgeVector _pos;
	float _time;
	float _timeCounter;
	float _acc;
	float _speed;
	float _x;
	float _x0;
	Texture *_texture;
	HGE *_hge;
};

#endif // !defined(_Tester_INCLUDED_)
