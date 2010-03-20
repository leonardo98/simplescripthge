// StarsRect.h: interface for the StarsRect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_STARSRECT_INCLUDED_)
#define _STARSRECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"

class StarsRect  
	: public Object
{
public:
	StarsRect(TiXmlElement *xe);
	virtual ~StarsRect();
	virtual void Draw();
	virtual void Update(float deltaTime);
private:
	hgeVector _pos;
	int _width;
	int _height;
	int _number;
	Texture *_texture;
	struct Star 
	{
		hgeVector getPos();
		Star(hgeVector pos, int width, int height);
		void Update(float deltaTime);
		float pos;
	private:
		hgeVector start;
		hgeVector end;
		float speed;
	};
	std::list<Star> _stars;
};

#endif // !defined(_STARSRECT_INCLUDED_)
