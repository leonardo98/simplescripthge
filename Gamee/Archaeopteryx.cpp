#include "Archaeopteryx.h"
#include "../Core/Core.h"
#include "BirdsManager.h"
#include "../Core/Math.h"

const float Archaeopteryx::SPEED = 80.f;

Archaeopteryx::Archaeopteryx() 
: _mirror(rand() % 2 == 1)
{
	BirdsManager::_positions.push_back(this);

	if (rand() % 2 == 1) {
		_birdsType = "archaeopteryx";
	} else {
		_birdsType = "dodo";
	}

	_boy = (rand() % 2 == 1);

	if (rand() % 2 <= 1) {
		_age = age_young;
		if (_boy) {
			_sex = "b";
		} else {
			_sex = "g";            
		}
        _lifeTimeCounter = Math::random(0.1f, 2.f);
	} else {
		_age = age_adult;
		if (_boy) {
			_sex = "m";
		} else {
			_sex = "w";
		}
        _lifeTimeCounter = 0.f;
	}
    
    SetupAnimation();

	_shadow = Core::getTexture("persshadow");
	_endTarget = _currentTarget = _pos;
	_timeCounter = 0.f;

	_region.push_back(FPoint2D(750, 460));
	_region.push_back(FPoint2D(890, 460));
	_region.push_back(FPoint2D(910, 520));
	_region.push_back(FPoint2D(1000, 540));
	_region.push_back(FPoint2D(1000, 680));
	_region.push_back(FPoint2D(790, 680));
	_region.push_back(FPoint2D(720, 500));

	_pos = _region[rand() % _region.size()];
	_pos = (_pos + _region[rand() % _region.size()]) / 2;

	_actionTimeCounter = Math::random(1.5f, 4.f);

	_state = state_idle;
	_current = _leftFrontIdle;
	_current->SetPos(_pos, _mirror);
    
    _waitingTimeCounter = 0.f;
    
    _adultCircleStates.push_back(state_want_drink);
    _adultCircleStates.push_back(state_want_eat);
    _adultCircleStates.push_back(state_want_drink);
    _adultCircleStates.push_back(state_want_eat);
    
    _waterPos = FPoint2D(790, 680);
    _foodPos = FPoint2D(720, 500);
    _turnPoint = FPoint2D(1000, 590);
    
    _nextState = state_idle;
    _targetPosition = true;
}

void Archaeopteryx::SetupAnimation() {
    std::string id = _birdsType + "_" + _sex;
	//id = "archaeopteryx_b";
    
	_left = Core::getAnimation(id + "_left");
	_leftIdle = Core::getAnimation(id + "_leftidle");
	_leftFront = Core::getAnimation(id + "_leftfront");
	_leftFrontIdle = Core::getAnimation(id + "_leftfrontidle");
	_leftBack = Core::getAnimation(id + "_leftback");
	_lefteat = Core::getAnimation(id + "_leftdrink");
}


void Archaeopteryx::Draw() {
	assert(0 <= _pos.x && _pos.x <= 1024);
	assert(0 <= _pos.y && _pos.y <= 768);
	_shadow->Render(_pos.x - _shadow->Width() / 2, _pos.y - _shadow->Height() / 2);
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	if (_mirror) {
		Render::MatrixScale(-1.f, 1.f);
	}
	_current->Draw(_timeCounter);
	Render::PopMatrix();
    if (_state == state_want_drink || _state == state_want_eat) {
        _waitingProgress.Move(_pos.x, _pos.y - 80);
        _waitingProgress.Draw(1.f - _waitingTimeCounter);
    }
    if (_nextState == state_want_drink || _nextState == state_want_eat) {
        _waitingProgress.Move(_pos.x, _pos.y - 80);
        _waitingProgress.Draw(0.f);
    }
	/*
	char buffer[100];
	sprintf(buffer, "x:%i, y:%i", (int)_pos.x, (int)_pos.y);
	Render::PrintString(_pos.x, _pos.y, "", buffer);
	*/
}

void Archaeopteryx::Update(float dt) {
	_timeCounter += (dt / _current->Time());
	while (_timeCounter >= 1.f) {
		_timeCounter -= 1.f;
	}
	if (_actionTimeCounter >= 0.f) {
		_actionTimeCounter -= dt;
		if (_state == state_walk) {
			_pos += *(_currentTarget - _pos).Normalize() * SPEED * dt;
//            if (!_targetPosition && (_currentTarget - _pos).Length() < 10.f) {
//                _current = _lefteat;
//                _state = _nextState;
//                _nextState = state_idle;
//                _waitingTimeCounter = 1.f;
//                return;
//            }
		}
        if (_waitingTimeCounter > 0.f) {
            _waitingTimeCounter -= dt / 20.f;
            if (_waitingTimeCounter > 0.f) {            
                return;
            } 
            dt += _waitingTimeCounter;
            if (_state == state_want_eat) {
                BirdsManager::SetWaterBusy(false);
            } else if (_state == state_want_drink) {
                BirdsManager::SetFoodBusy(false);
            }
            _state = state_walk;
        }
		if (_actionTimeCounter <= 0.f || (!_targetPosition && (_pos - _currentTarget).Length() > 10.f)) {
			_actionTimeCounter = Math::random(1.5f, 4.f);
			if (_state == state_eat0) {
				if (Math::random(0.f, 1.f) > 0.5f) {
					_state = state_idle;
					_current = _leftIdle;
				} else {
					SwitchToWalk();
				}
			} else if (_state == state_idle) {
				if (Math::random(0.f, 1.f) < 0.5f && _current == _leftIdle) {
					_state = state_eat0;
					_current = _lefteat;
					_actionTimeCounter = _lefteat->Time();
				} else {
					SwitchToWalk();
				}
			} else if (_state == state_walk) {
				if (BirdsManager::FreePosition(this) && _nextState != state_want_drink && _nextState != state_want_eat) {
					SwitchToIdle();
					_actionTimeCounter *= static_cast<int>(sqrt(float(BirdsManager::Size())));
				} else {
					SwitchToWalk();
				}
			}
		} else if (_nextState == state_want_eat && (_pos - _foodPos).Length() < 10.f) {            
            _state = state_want_eat;
            _nextState = state_idle; 
            _current = _lefteat;
            _mirror = false;
            _targetPosition = true;
		} else if (_nextState == state_want_drink && (_pos - _waterPos).Length() < 10.f) {
            _state = state_want_drink;
            _nextState = state_idle;
            _current = _lefteat;
            _mirror = false;
            _targetPosition = true;
        }
	}
	BirdsManager::UpdatePosition(this, dt);
    
    if (_lifeTimeCounter > 0.f && _targetPosition) {
        _lifeTimeCounter -= dt * 0.5f;
        if (_lifeTimeCounter <= 0.f) {
            _lifeTimeCounter = Math::random(0.5f, 2.f);
            if (_adultCircleStates.size()) {
                _nextState = _adultCircleStates.front();
                if (_nextState == state_want_eat && !BirdsManager::IsFoodBusy()) {
                    _waitingProgress.SetIcon("grain");
                    BirdsManager::SetFoodBusy(true);
                    SwitchToWalk();
                    _actionTimeCounter = 20.f;
                    _targetPosition = false;
                } else if (_nextState == state_want_drink && !BirdsManager::IsWaterBusy()) {
                    _waitingProgress.SetIcon("water");
                    BirdsManager::SetWaterBusy(true);
                    SwitchToWalk();
                    _actionTimeCounter = 20.f;
                    _targetPosition = false;
                } else {
                    _nextState = state_idle;
                    return;
                }
                _adultCircleStates.pop_front();
                return;
            } else if (_sex == "b") {
                _sex = "m";
            } else {
                _sex = "w";
            }
            SetupAnimation();
        }
    }
}

void Archaeopteryx::SwitchToIdle() {

	//SwitchToWalk();
	//return;

	_state = state_idle;
	if (_current == _left) {
		_current = _leftIdle;
	} else {
		_current = _leftFrontIdle;
	}
}

void Archaeopteryx::SwitchToWalk() {
	_state = state_walk;
	FPoint2D newPos;
	do {
		newPos = GetNewDirection();
	} while (fabs(newPos.x - _pos.x) < fabs(newPos.y - _pos.y));
	_currentTarget = newPos;
	SwitchAnimation();
}

FPoint2D Archaeopteryx::GetDirection() {
	if (_state == state_walk) {
		return (_currentTarget - _pos);
	}
	return FPoint2D(0, 0);
}

FPoint2D Archaeopteryx::GetNewDirection() {
	//i = rand() % 2;
	//return _region[1 + 2 * i];
    if (_nextState == state_want_drink && (fabs(_waterPos.x - _pos.x) >= fabs(_waterPos.y - _pos.y))) {
        return _waterPos;
    } else if (_nextState == state_want_eat && (fabs(_foodPos.x - _pos.x) >= fabs(_foodPos.y - _pos.y))) {
        return _foodPos;
    } else if (_nextState == state_want_eat || _nextState == state_want_drink) {
        return _turnPoint;
    }

	int i = rand() % _region.size();
	float f = (rand() % 10) / 10.f;
	FPoint2D a = _region[i];
	FPoint2D b;
	if (i == 0) {
		i = _region.size();
	} 
	b = _region[i - 1];
	return a + (b - a) * f;
}

void Archaeopteryx::SwitchAnimation() {
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
		assert(false);
	} else if (11 * M_PI / 8 <= angle && angle <= 13 * M_PI / 8) {
		assert(false);
	} else {
		_current = _left;
	}
	_mirror = (_pos.x < _currentTarget.x);
}