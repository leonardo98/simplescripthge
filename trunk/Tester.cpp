// Tester.cpp: implementation of the Tester class.
//
//////////////////////////////////////////////////////////////////////

#include "Tester.h"
#include "types.h"
#include "Core.h"

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
	_hge = hgeCreate(HGE_VERSION);
}

Tester::~Tester() {
	_hge->Release();
}

void Tester::Draw() {
	_texture->Render(_pos - hgeVector(_x, 0));
}

void Tester::Update(float deltaTime) {
	if ((_timeCounter += deltaTime) < _time) {
		_speed += _acc * deltaTime;
		_x -= _speed * deltaTime;
	}
	if (_hge->Input_KeyDown(HGEK_SPACE)) {
 		_speed = 200;
		_x = 400;
		float k = _x / _speed;
		float time = 2;
		_acc = - _speed / (time * (k * k));
		_timeCounter = 0;
		_time = time;
	}
}
