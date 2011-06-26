#pragma once

#include "..\Core\types.h"
#include "C:\CppLib\Box2D_v2.1.2\Box2D\Box2D\Box2D.h"

struct UV {
#ifndef HGE_COMPILE_KEY
	int16 u, v;
#else
	float u, v;
#endif
};

struct BodyTemplate // описание базового элемента - все исходные параметры тут и только они см. MyBody, BodyState
{
	BodyTemplate(TiXmlElement *xe);

	std::string _id;
	std::string _shape;
	float _restitution;
	float _friction;
	float _radius;
	float _explosionRadius;
	float _height;
	float _width;
	float _maxForce;
	bool _explosion;
	bool _breakable;
	bool _destroyOnTap;
	bool _fixed;

	UV _uv[4];
	//static const int MAX = 300;
	//FPoint2D _positions[MAX][4]; 
	// соптимизируем для iphone, чтобы не тратить время на расчеты в реальном времени
	// запишем все координаты вершин элемента под разными углами в массив
};