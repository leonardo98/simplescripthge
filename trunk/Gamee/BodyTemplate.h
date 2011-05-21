#pragma once

#include "..\Core\types.h"
#include "C:\CppLib\Box2D_v2.1.2\Box2D\Box2D\Box2D.h"

enum BodyTypes{
	BODY_TYPE_GROUND = -1,
	BODY_TYPE_UNBREAKABLE = 0,
	BODY_TYPE_BLUE = 1,
	BODY_TYPE_BALL = 2,
	BODY_TYPE_EXPLOSION = 3
};

struct BodyTemplate // описание базового элемента - все исходные параметры тут и только они см. MyBody, BodyState
{
public:
	static const int MAX = 300;
	std::string _id;
	struct UV {
#ifndef HGE_COMPILE_KEY
		int16 u, v;
#else
		float u, v;
#endif
	};
	UV _uv[4];
	FPoint2D _positions[MAX][4]; 
	// соптимизируем для iphone, чтобы не тратить время на расчеты в реальном времени
	// запишем все координаты вершин элемента под разными углами в массив
	BodyTypes _type;
	float _restitution;
	float _friction;
	std::string _shape;
	float _radius, _height, _width, _maxForce;
	BodyTemplate(TiXmlElement *xe);
};