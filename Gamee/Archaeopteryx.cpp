#include "Archaeopteryx.h"
#include "../Core/Core.h"
#include "BirdsManager.h"
#include "ProductManager.h"
#include "GameField.h"
#include "../Core/Math.h"

const float Archaeopteryx::SPEED = 80.f;

Archaeopteryx::Archaeopteryx(const std::string &birdId) 
: _mirror(rand() % 2 == 1)
{
    _waterPos = FPoint2D(771, 628);
    _foodPos = FPoint2D(733, 540);
    _turnPoint = FPoint2D(900, 595);
	_eggPos = FPoint2D(900, 595);
	_puhPos = FPoint2D(890, 575);

	BirdsManager::_birds.push_back(this);

	if (birdId != "") {
		_birdsType = birdId.substr(0, birdId.length() - 2);
		_sex = birdId[birdId.length() - 1];
		_boy = (_sex == "b" || _sex == "m");
		if (_sex == "b" || _sex == "g") {
			_lifeTimeCounter = Math::random(0.1f, 2.f);
			_age = age_young;
		} else {
			_lifeTimeCounter = Math::random(0.5f, 2.f);
			_age = age_adult;
			_waterPos.x += 10;
			_foodPos.x += 10;
		}
	} else {
		if (rand() % 2 == 1 && false) {// test/debug only
			_birdsType = "archaeopteryx";
		} else {
			_birdsType = "dodo";
		}

		_boy = (rand() % 2 == 1) && false;// test/debug only

		if (rand() % 2 < 1 || true) {// test/debug only
			_age = age_young;
			if (_boy) {
				_sex = "b";
			} else {
				_sex = "g";            
			}
			_lifeTimeCounter = Math::random(0.1f, 2.f);
		} else {
			_age = age_adult;
			_waterPos.x += 10;
			_foodPos.x += 10;
			if (_boy) {
				_sex = "m";
			} else {
				_sex = "w";
			}
			_lifeTimeCounter = 0.f;
		}
	}
    
    SetupAnimation();

	_shadow = Core::getTexture("persshadow");
	_endTarget = _currentTarget = _pos;
	_timeCounter = 0.f;

	_region.push_back(FPoint2D(765, 460));
	_region.push_back(FPoint2D(890, 460));
	_region.push_back(FPoint2D(910, 520));
	_region.push_back(FPoint2D(1000, 540));
	_region.push_back(FPoint2D(1000, 680));
	_region.push_back(FPoint2D(790, 680));
	_region.push_back(FPoint2D(735, 500));

	float WIDTH_RUN = 65;
	float HEIGHT_RUN = 40;
	_runAway.push_back(FPoint2D(865 - WIDTH_RUN, 550));
	_runAway.push_back(FPoint2D(865, 550 - HEIGHT_RUN));
	_runAway.push_back(FPoint2D(865 + WIDTH_RUN, 550));
	_runAway.push_back(FPoint2D(865, 550 + HEIGHT_RUN));
	_runAway.push_back(FPoint2D(865 - WIDTH_RUN, 550));
	_runAway.push_back(FPoint2D(865, 550 - HEIGHT_RUN));
	_runAway.push_back(FPoint2D(865 + WIDTH_RUN, 550));
	_runAway.push_back(FPoint2D(865, 550 + HEIGHT_RUN));
	_runAway.push_back(FPoint2D(1100, 550 + HEIGHT_RUN));
    
	_pos = _region[rand() % _region.size()];
	_pos = (_pos + _region[rand() % _region.size()]) / 2;

	_actionTimeCounter = Math::random(1.5f, 4.f);

	_state = state_idle;
	_current = _leftFrontIdle;
	_current->SetPos(_pos, _mirror);
    
    _waitingTimeCounter = 0.f;
    
	if (_age == age_young) {
		_adultCircleStates.push_back(state_want_drink);
		_adultCircleStates.push_back(state_want_eat);
		_adultCircleStates.push_back(state_want_drink);
		_adultCircleStates.push_back(state_want_eat);
	}

	_productCircleStates.push_back(state_want_drink);
    _productCircleStates.push_back(state_want_eat);
    _productCircleStates.push_back(state_want_drink);
    _productCircleStates.push_back(state_want_eat);
    
    _nextState = state_none;
    _runAwayTimeCounter = 0.f;
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
	if (_active) {
		float b = GameField::SELECTION_BORDER;
		Render::SetBlendMode(3);
		Render::PushMatrix();
		Render::MatrixMove(-b, 0);
		InnerDraw();
		Render::MatrixMove(2 * b, 0);
		InnerDraw();
		Render::MatrixMove(-b, b);
		InnerDraw();
		Render::MatrixMove(0, -2 * b);
		InnerDraw();
		Render::PopMatrix();
		Render::SetBlendMode(BLEND_DEFAULT);
	}
	InnerDraw();
}

void Archaeopteryx::DrawBottom() {
	_shadow->Render(_pos.x - _shadow->Width() / 2, _pos.y - _shadow->Height() / 2);
}

void Archaeopteryx::InnerDraw() {
	assert(0 <= _pos.x && _pos.x <= 1124 + 10.f);
	assert(0 <= _pos.y && _pos.y <= 768);
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
    } else if (_state == state_archaeopteryx_puh) {
        _waitingProgress.Move(_pos.x, _pos.y - 30);
        _waitingProgress.Draw(1.f - _waitingTimeCounter);
    }
    //if (_nextState == state_want_drink || _nextState == state_want_eat) {
    //    _waitingProgress.Move(_pos.x, _pos.y - 80);
    //    _waitingProgress.Draw(0.f);
    //}
	
	//char buffer[100];
	//sprintf(buffer, "lifeTimer:%f", _lifeTimeCounter);
	//Render::PrintString(_pos.x, _pos.y - 50, "", buffer);
	
}

void Archaeopteryx::Update(float dt) {
	_timeCounter += (dt / _current->Time());
	while (_timeCounter >= 1.f) {
		_timeCounter -= 1.f;
	}
	if (_runAwayTimeCounter > 0.f) {
		_runAwayTimeCounter -= dt;
	}
    if (_waitingTimeCounter > 0.f) {
        _waitingTimeCounter -= dt / 60.f; // время ожидания(60)
		if (_state == state_want_drink && !BirdsManager::IsWaterEmpty()) {
			_state = state_idle;
			_current = _lefteat;
			_actionTimeCounter = 1.f;
			_waitingTimeCounter = 0.f;
			BirdsManager::DrinkWater();
		} else if (_state == state_want_eat && !BirdsManager::IsFoodEmpty()) {
			_state = state_idle;
			_current = _lefteat;
			_actionTimeCounter = 1.f;
			_waitingTimeCounter = 0.f;
			BirdsManager::EatFood();
		} else if (_waitingTimeCounter > 0.f) {
            return;
		} else {
			if (_state != state_archaeopteryx_puh) {
				_adultCircleStates.push_front(_state);// hack - if miss any action - it will restart in 10 second
			}
			_state = state_walk;
			_runAwayPoint = 0;
			_runAwayTimeCounter = 10.f;
			_actionTimeCounter = dt / 2.f;
		}
    }
	if (_runAwayTimeCounter > 0.f) {
		_pos += *(_currentTarget - _pos).Normalize() * SPEED * 2 * dt;
//		BirdsManager::UpdatePosition(this, dt);
		if ((_currentTarget - _pos).Length() < SPEED * 2 * dt) {
			if (_runAwayPoint < _runAway.size()) {
				_currentTarget = _runAway[_runAwayPoint++];
				SwitchAnimation();
			}
		}
		return;
	} else if (_actionTimeCounter >= 0.f) {
		_actionTimeCounter -= dt;
		if (_state == state_walk) {
			_pos += *(_currentTarget - _pos).Normalize() * SPEED * dt;
            if ((_nextState == state_want_eat && (_pos - _foodPos).Length() < SPEED * dt) 
				|| (_nextState == state_want_drink && (_pos - _waterPos).Length() < SPEED * dt)) {
				_state = _nextState;
				_current = _leftIdle;
				_nextState = state_none;
				_mirror = false;
				_waitingTimeCounter = 1.f;
				return;
			} else if (_nextState == state_dodo_egg && (_pos - _eggPos).Length() < SPEED * dt) {
				ProductManager::PlaceProduct(pt_egg, "eggs_dodo", 0.f);
				_nextState = state_none;
				return;
			} else if (_nextState == state_archaeopteryx_puh && (_pos - _puhPos).Length() < SPEED * dt) {
				SwitchToIdle();
				_state = state_archaeopteryx_puh;
				_waitingProgress.SetIcon("hand");
				_waitingTimeCounter = 1.f;
				_nextState = state_none;
				return;
			}
		}
		if (_actionTimeCounter <= 0.f || (_state == state_walk && (_currentTarget - _pos).Length() < SPEED * dt)) {
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
        }
	}
	BirdsManager::UpdatePosition(this, dt);
    
    if (_lifeTimeCounter > 0.f && _nextState == state_none) {
        _lifeTimeCounter -= dt * 0.5f;
        if (_lifeTimeCounter <= 0.f) {
            _lifeTimeCounter = Math::random(0.5f, 2.f);
			BirdsStates ns;
			CircleStates *currentStates = NULL;
            if (_adultCircleStates.size()) {
                ns = _adultCircleStates.front();
				currentStates = &_adultCircleStates;
			} else if (_sex == "b" || _sex == "g") {
				if (_sex == "b") {
					_sex = "m";
	            } else {
		            _sex = "w";
				}
				_age = age_adult;
				_waterPos.x += 10;
				_foodPos.x += 10;
			    SetupAnimation();
				return;
			} else if (_productCircleStates.size()) {
                ns = _productCircleStates.front();
				currentStates = &_productCircleStates;
			} else {
				if (_sex == "w") {
					_productCircleStates.push_back(state_want_drink);
					_productCircleStates.push_back(state_want_eat);
					_productCircleStates.push_back(state_want_drink);
					_productCircleStates.push_back(state_want_eat);
					if (_birdsType == "dodo") {
						_nextState = state_dodo_egg;
					} else {
						_nextState = state_archaeopteryx_puh;
					}
					_actionTimeCounter = 20.f;
					SwitchToWalk();
				}
				return;
			}
            if (ns == state_want_eat && !BirdsManager::IsFoodBusy()) {
                _waitingProgress.SetIcon("grain");
                SwitchToWalk();
                _actionTimeCounter = 20.f;
				_nextState = ns;
                currentStates->pop_front();
            } else if (ns == state_want_drink && !BirdsManager::IsWaterBusy()) {
                _waitingProgress.SetIcon("water");
                SwitchToWalk();
                _actionTimeCounter = 20.f;
				_nextState = ns;
                currentStates->pop_front();
            }
        }
    }
}

void Archaeopteryx::SwitchToIdle() {
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
	} while (_nextState != state_want_eat 
		&& _nextState != state_want_drink 
		&& _nextState != state_dodo_egg 
		&& _nextState != state_archaeopteryx_puh 
		&& fabs(newPos.x - _pos.x) * 1.7f < fabs(newPos.y - _pos.y));
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
	if (_nextState == state_want_drink) {
        return _waterPos;
	} else if (_nextState == state_want_eat) {
        return _foodPos;
	} else if (_nextState == state_dodo_egg) {
        return _eggPos;
	} else if (_nextState == state_archaeopteryx_puh) {
        return _puhPos;
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
		_current = _leftFront;
	} else if (11 * M_PI / 8 <= angle && angle <= 13 * M_PI / 8) {
		_current = _leftBack;
	} else {
		_current = _left;
	}
	_mirror = (_pos.x < _currentTarget.x);
}

bool Archaeopteryx::IsUnderMouse(const FPoint2D &mousePos) {
	return _state == state_archaeopteryx_puh && hgeRect(_pos.x - 50, _pos.y - 100, _pos.x + 50, _pos.y).TestPoint(mousePos.x, mousePos.y);
}

void Archaeopteryx::OnMouseDown(const FPoint2D &mousePos) {
	AnnaPers::NewAction("", this);
}

Archaeopteryx::BirdsStates Archaeopteryx::GetState() {
	return _state;
}

void Archaeopteryx::CutFluff() {
	_waitingTimeCounter = 0.f;
	SwitchToWalk();
}
