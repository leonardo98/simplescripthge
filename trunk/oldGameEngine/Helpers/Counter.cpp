// Counter.cpp: implementation of the Counter class.
//
//////////////////////////////////////////////////////////////////////

#include "Counter.h"

Counter::Counter()
	: _actionTime(0.f)
	, _actionTimeCounter(0.f)
{
}

Counter::~Counter()
{
}

void Counter::Update(float deltaTime) {
	if (_actionTimeCounter > 0) {
		_actionTimeCounter -= deltaTime;
	}
}

bool Counter::Action() {
	return (0 < _actionTimeCounter && _actionTimeCounter <= _actionTime);
}

bool Counter::Done() {
	return _actionTimeCounter <= 0.f;
}

void Counter::Init(float actionTime, float startTime) {
	_actionTime = actionTime;
	_actionTimeCounter = actionTime + startTime;
}

float Counter::Progress() {
	if (Action()) {
		return 1.f - _actionTimeCounter / _actionTime;
	} else if (!Done()) {
		return 0.f;
	} else {
		return 1.f;
	}
}
