#include "AnnaPers.h"
#include "../Core/Core.h"
#include "PersPaths.h"

AnnaPers::AnnaPers() 
: SPEED(100.f)
, _mirror(true)
{
	_pos = FPoint2D(100.f, 200.f);

	_front = Core::getAnimation("anna_basic_front");
	_leftFront = Core::getAnimation("anna_basic_leftfront");
	_leftFrontIdle = Core::getAnimation("anna_basic_leftfront_idle");
	_left = Core::getAnimation("anna_basic_left");
	_back = Core::getAnimation("anna_basic_back");
	_leftBack = Core::getAnimation("anna_basic_leftback");

	_shadow = Core::getTexture("persshadow");

	_prevTarget = _endTarget = _currentTarget = _pos;
	_current = _leftFrontIdle;
	_current->SetPos(_pos, _mirror);
}

void AnnaPers::Draw() {
	_shadow->Render(_pos.x - _shadow->Width() / 2, _pos.y - _shadow->Height() / 2);
	_current->Draw();
}

void AnnaPers::Update(float dt) {
	_current->Update(dt);
	if ((_pos - _currentTarget).Length() > SPEED * dt) {
		_pos += *(_currentTarget - _pos).Normalize() * SPEED * dt;
		_mirror = (_pos.x < _currentTarget.x);
		_current->SetPos(_pos, _mirror);
		return;
	}
	if ((_pos - _endTarget).Length() > SPEED * dt) {
		FPoint2D newTarget = PersPaths::NextPointTo(_currentTarget, _endTarget);
		if ((newTarget - _prevTarget).Length() < 10.f) {
			_endTarget = _currentTarget = _pos;
			return;
		}
		_prevTarget = _currentTarget;
		_currentTarget = newTarget;
		assert((_pos - _currentTarget).Length() > 1.f);

		float angle = atan2(_currentTarget.y - _pos.y, _currentTarget.x - _pos.x);
		if (angle < 0) {
			angle += 2 * M_PI;
		}
		float ENLARGE = M_PI / 8 / 4;
		if (M_PI / 8 - ENLARGE <= angle && angle <= 3 * M_PI / 8 + ENLARGE) {
			_current = _leftFront;
		} else if (5 * M_PI / 8 - ENLARGE <= angle && angle <= 7 * M_PI / 8 + ENLARGE) {
			_current = _leftFront;
		} else if (9 * M_PI / 8 - ENLARGE <= angle && angle <= 11 * M_PI / 8 + ENLARGE) {
			_current = _leftBack;
		} else if (13 * M_PI / 8 - ENLARGE <= angle && angle <= 15 * M_PI / 8 + ENLARGE) {
			_current = _leftBack;
		} else if (angle <= M_PI / 8) {
			_current = _left;
		} else if (3 * M_PI / 8 <= angle && angle <= 5 * M_PI / 8) {
			_current = _front;
		} else if (11 * M_PI / 8 <= angle && angle <= 13 * M_PI / 8) {
			_current = _back;
		} else {
			_current = _left;
		}
		_pos += *(_currentTarget - _pos).Normalize() * SPEED * dt;
		_mirror = (_pos.x < _currentTarget.x);
		_current->SetPos(_pos, _mirror);
		return;
	}
	_pos = _endTarget;
	if (_current != _leftFrontIdle) {
		_current = _leftFrontIdle;
		_current->SetPos(_pos, _mirror);
	}
}

void AnnaPers::Goto(FPoint2D position) {
	_endTarget = position;
}