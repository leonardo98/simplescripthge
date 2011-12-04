#ifndef BODYTEMPLATE_H
#define BODYTEMPLATE_H

#include "../Core/types.h"
#include "../../CppLib/Box2D_v2.1.2/Box2D/Box2D/Box2D.h"

struct UV {
	float u, v;
};

struct BodyTemplate // �������� �������� �������� - ��� �������� ��������� ��� � ������ ��� ��. MyBody, BodyState
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
	bool _hero;

	UV _uv[4];
	//static const int MAX = 300;
	//FPoint2D _positions[MAX][4]; 
	// ������������� ��� iphone, ����� �� ������� ����� �� ������� � �������� �������
	// ������� ��� ���������� ������ �������� ��� ������� ������ � ������
};

#endif//BODYTEMPLATE_H