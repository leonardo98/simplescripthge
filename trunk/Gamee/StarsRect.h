// StarsRect.h: interface for the StarsRect class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_STARTRECT_H
#define MYENGINE_STARTRECT_H

#include "../Core/Object.h"
#include "../Core/Render.h"

class StarsRect  
	: public Object
{
public:
	StarsRect(TiXmlElement *xe);
	virtual ~StarsRect();
	virtual void Draw();
	virtual void Update(float deltaTime);
private:
	FPoint2D _pos;
	int _width;
	int _height;
	int _number;
	Texture *_texture;
	struct Star 
	{
		FPoint2D getPos();
		Star(FPoint2D pos, int width, int height);
		void Update(float deltaTime);
		float pos;
	private:
		FPoint2D start;
		FPoint2D end;
		float speed;
	};
	std::list<Star> _stars;
};

#endif//MYENGINE_STARTRECT_H