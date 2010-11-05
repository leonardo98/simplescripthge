#pragma once

#include "..\Core\types.h"
#include "Box2D\Box2D.h"

struct BodyTemplate 
{
public:
	enum Types{
		TYPE_UNBREAKABLE,
		TYPE_BLUE,
		TYPE_BALL,
		TYPE_EXPLOSION
	};
	Types _type;
	float _restitution;
	float _friction;
	std::string _shape;
	float _radius, _height, _width;
	BodyTemplate(TiXmlElement *xe);
};