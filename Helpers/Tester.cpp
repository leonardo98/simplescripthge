// Tester.cpp: implementation of the Tester class.
//
//////////////////////////////////////////////////////////////////////

#include "Tester.h"
#include "..\Core\types.h"
#include "..\Core\Core.h"
#include "..\Core\Render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Tester::Tester(TiXmlElement *xe) {
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
	_acc = 0;
	_speed = 0;
	_x = 0;
}

Tester::~Tester() {
}

void Tester::Draw() {
	_texture->Render(_pos - FPoint2D(_x, 0));
}

void Tester::Update(float deltaTime) {
	if ((_timeCounter += deltaTime) < _time) {
		_speed += _acc * deltaTime;
		_x -= _speed * deltaTime;
	}
}
