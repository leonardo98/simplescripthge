#pragma once

#include "..\Core\types.h"
#include "Box2D\Box2D.h"

enum BodyTypes{
	BODY_TYPE_GROUND,
	BODY_TYPE_UNBREAKABLE,
	BODY_TYPE_BLUE,
	BODY_TYPE_BALL,
	BODY_TYPE_EXPLOSION
};

struct BodyTemplate 
{
public:
	BodyTypes _type;
	float _restitution;
	float _friction;
	std::string _shape;
	float _radius, _height, _width;
	BodyTemplate(TiXmlElement *xe);
	typedef std::map<BodyTypes, BodyTemplate *> Collection;
	static Collection _collection;
};