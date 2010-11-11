#pragma once

#include "..\Core\types.h"
#include "Box2D\Box2D.h"

enum BodyTypes{
	BODY_TYPE_GROUND = -1,
	BODY_TYPE_UNBREAKABLE = 0,
	BODY_TYPE_BLUE = 1,
	BODY_TYPE_BALL = 2,
	BODY_TYPE_EXPLOSION = 3
};

struct BodyTemplate 
{
public:
	static const int MAX = 200;
	std::string _id;
	//Elements
	struct UV {
		float u, v;
	};
	UV _uv[4];
	FPoint2D _positions[MAX][4]; //позже можно соптимизировать для iphone 
	// записать все положения под разными углами в массив
	BodyTypes _type;
	float _restitution;
	float _friction;
	std::string _shape;
	float _radius, _height, _width, _maxForce;
	BodyTemplate(TiXmlElement *xe);
};