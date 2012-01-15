#include "Animal.h"
#include "../Core/Core.h"
#include "AnimalsManager.h"
#include "ProductManager.h"
#include "GameField.h"
#include "../Core/Math.h"

const float Animal::SPEED = 120.f;

Animal::Animal(const std::string &animalId) 
: _mirror(rand() % 2 == 1)
{
    _waterPos = FPoint2D(1024 - 771, 628);
    _foodPos = FPoint2D(1024 - 733, 540);
    _turnPoint = FPoint2D(1024 - 900, 595);
	_eggPos = FPoint2D(1024 - 900, 595);
	_puhPos = FPoint2D(1024 - 890, 575);

	AnimalsManager::_animals.push_back(this);

	if (animalId != "") {
		_animalsType = animalId.substr(0, animalId.length() - 2);
		_sex = animalId[animalId.length() - 1];
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
			_animalsType = "animal";
		} else {
			_animalsType = "dodo";
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

	_region.push_back(FPoint2D(1024 - 765, 460));
	_region.push_back(FPoint2D(1024 - 890, 460));
	_region.push_back(FPoint2D(1024 - 910, 520));
	_region.push_back(FPoint2D(1024 - 1000, 540));
	_region.push_back(FPoint2D(1024 - 1000, 680));
	_region.push_back(FPoint2D(1024 - 790, 680));
	_region.push_back(FPoint2D(1024 - 735, 500));

	float WIDTH_RUN = 65;
	float HEIGHT_RUN = 40;
	_runAway.push_back(FPoint2D(1024 - (865 - WIDTH_RUN), 550));
	_runAway.push_back(FPoint2D(1024 - 865, 550 - HEIGHT_RUN));
	_runAway.push_back(FPoint2D(1024 - (865 + WIDTH_RUN), 550));
	_runAway.push_back(FPoint2D(1024 - 865, 550 + HEIGHT_RUN));
	_runAway.push_back(FPoint2D(1024 - (865 - WIDTH_RUN), 550));
	_runAway.push_back(FPoint2D(1024 - 865, 550 - HEIGHT_RUN));
	_runAway.push_back(FPoint2D(1024 - (865 + WIDTH_RUN), 550));
	_runAway.push_back(FPoint2D(1024 - 865, 550 + HEIGHT_RUN));
	_runAway.push_back(FPoint2D(1024 - 1100, 550 + HEIGHT_RUN));
    
	_pos = _region[rand() % _region.size()];
	_pos = (_pos + _region[rand() % _region.size()]) / 2;

	_actionTimeCounter = Math::random(1.5f, 4.f);

	_state = state_idle;
	_current = _rightFrontIdle;
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

void Animal::SetupAnimation() {
    std::string id = _animalsType + "_" + _sex;
	//id = "animal_b";
    
	_right = Core::getAnimation(id + "_right");
	_rightIdle = Core::getAnimation(id + "_rightidle");
	_rightFront = Core::getAnimation(id + "_rightfront");
	_rightFrontIdle = Core::getAnimation(id + "_rightfrontidle");
	_rightBack = Core::getAnimation(id + "_rightback");
	_righteat = Core::getAnimation(id + "_rightdrink");
}

void Animal::Draw() {
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

void Animal::DrawBottom() {
	_shadow->Render(_pos.x - _shadow->Width() / 2, _pos.y - _shadow->Height() / 2);
}

void Animal::InnerDraw() {
	assert(-100 - 10.f <= _pos.x && _pos.x <= 1124 + 10.f);
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
    } else if (_state == state_animal_puh) {
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

void Animal::Update(float dt) {
	_timeCounter += (dt / _current->Time());
	while (_timeCounter >= 1.f) {
		_timeCounter -= 1.f;
	}
	if (_runAwayTimeCounter > 0.f) {
		_runAwayTimeCounter -= dt;
	}
    if (_waitingTimeCounter > 0.f) {
        _waitingTimeCounter -= dt / 60.f; // время ожидания(60)
		if (_state == state_want_drink && !AnimalsManager::IsWaterEmpty()) {
			_state = state_idle;
			_current = _righteat;
			_actionTimeCounter = 1.f;
			_waitingTimeCounter = 0.f;
			AnimalsManager::DrinkWater();
		} else if (_state == state_want_eat && !AnimalsManager::IsFoodEmpty()) {
			_state = state_idle;
			_current = _righteat;
			_actionTimeCounter = 1.f;
			_waitingTimeCounter = 0.f;
			AnimalsManager::EatFood();
		} else if (_waitingTimeCounter > 0.f) {
            return;
		} else {
			if (_state != state_animal_puh) {
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
//		AnimalsManager::UpdatePosition(this, dt);
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
				_current = _rightIdle;
				_nextState = state_none;
				_mirror = false;
				_waitingTimeCounter = 1.f;
				return;
			} else if (_nextState == state_dodo_egg && (_pos - _eggPos).Length() < SPEED * dt) {
				ProductManager::PlaceProduct(pt_egg, "eggs_dodo", 0.f);
				_nextState = state_none;
				return;
			} else if (_nextState == state_animal_puh && (_pos - _puhPos).Length() < SPEED * dt) {
				SwitchToIdle();
				_state = state_animal_puh;
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
					_current = _rightIdle;
				} else {
					SwitchToWalk();
				}
			} else if (_state == state_idle) {
				if (Math::random(0.f, 1.f) < 0.5f && _current == _rightIdle) {
					_state = state_eat0;
					_current = _righteat;
					_actionTimeCounter = _righteat->Time();
				} else {
					SwitchToWalk();
				}
			} else if (_state == state_walk) {
				if (AnimalsManager::FreePosition(this) && _nextState != state_want_drink && _nextState != state_want_eat) {
					SwitchToIdle();
					_actionTimeCounter *= static_cast<int>(sqrt(float(AnimalsManager::Size())));
				} else {
					SwitchToWalk();
				}
			}
        }
	}
	AnimalsManager::UpdatePosition(this, dt);
    
    if (_lifeTimeCounter > 0.f && _nextState == state_none) {
        _lifeTimeCounter -= dt * 0.5f;
        if (_lifeTimeCounter <= 0.f) {
            _lifeTimeCounter = Math::random(0.5f, 2.f);
			AnimalStates ns;
			CircleStates *currentStates = NULL;
            if (_adultCircleStates.size()) {
                ns = _adultCircleStates.front();
				currentStates = &_adultCircleStates;
			} else if (_sex == "b" || _sex == "g") {
				{// temporary - no animation for adult rabbit
					_adultCircleStates.push_back(state_want_drink);
					_adultCircleStates.push_back(state_want_eat);
					_adultCircleStates.push_back(state_want_drink);
					_adultCircleStates.push_back(state_want_eat);
					return;
				}
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
					if (_animalsType == "dodo") {
						_nextState = state_dodo_egg;
					} else {
						_nextState = state_animal_puh;
					}
					_actionTimeCounter = 20.f;
					SwitchToWalk();
				}
				return;
			}
            if (ns == state_want_eat && !AnimalsManager::IsFoodBusy()) {
                _waitingProgress.SetIcon("clover");
                SwitchToWalk();
                _actionTimeCounter = 20.f;
				_nextState = ns;
                currentStates->pop_front();
            } else if (ns == state_want_drink && !AnimalsManager::IsWaterBusy()) {
                _waitingProgress.SetIcon("water");
                SwitchToWalk();
                _actionTimeCounter = 20.f;
				_nextState = ns;
                currentStates->pop_front();
            }
        }
    }
}

void Animal::SwitchToIdle() {
	_state = state_idle;
	if (_current == _right) {
		_current = _rightIdle;
	} else {
		_current = _rightFrontIdle;
	}
}

void Animal::SwitchToWalk() {
	_state = state_walk;
	FPoint2D newPos;
	do {
		newPos = GetNewDirection();
	} while (_nextState != state_want_eat 
		&& _nextState != state_want_drink 
		&& _nextState != state_dodo_egg 
		&& _nextState != state_animal_puh 
		&& fabs(newPos.x - _pos.x) * 1.7f < fabs(newPos.y - _pos.y));
	_currentTarget = newPos;
	SwitchAnimation();
}

FPoint2D Animal::GetDirection() {
	if (_state == state_walk) {
		return (_currentTarget - _pos);
	}
	return FPoint2D(0, 0);
}

FPoint2D Animal::GetNewDirection() {
	if (_nextState == state_want_drink) {
        return _waterPos;
	} else if (_nextState == state_want_eat) {
        return _foodPos;
	} else if (_nextState == state_dodo_egg) {
        return _eggPos;
	} else if (_nextState == state_animal_puh) {
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

void Animal::SwitchAnimation() {
	float angle = atan2(_currentTarget.y - _pos.y, _currentTarget.x - _pos.x);
	if (angle < 0) {
		angle += 2 * M_PI;
	}
	float ENLARGE = M_PI / 8 / 4;
	if (M_PI / 8 - ENLARGE <= angle && angle <= 3 * M_PI / 8 + ENLARGE) {
		_current = _rightFront;
	} else if (5 * M_PI / 8 - ENLARGE <= angle && angle <= 7 * M_PI / 8 + ENLARGE) {
		_current = _rightFront;
	} else if (9 * M_PI / 8 - ENLARGE <= angle && angle <= 11 * M_PI / 8 + ENLARGE) {
		_current = _rightBack;
	} else if (13 * M_PI / 8 - ENLARGE <= angle && angle <= 15 * M_PI / 8 + ENLARGE) {
		_current = _rightBack;
	} else if (angle <= M_PI / 8) {
		_current = _right;
	} else if (3 * M_PI / 8 <= angle && angle <= 5 * M_PI / 8) {
		_current = _rightFront;
	} else if (11 * M_PI / 8 <= angle && angle <= 13 * M_PI / 8) {
		_current = _rightBack;
	} else {
		_current = _right;
	}
	_mirror = (_pos.x > _currentTarget.x);
}

bool Animal::IsUnderMouse(const FPoint2D &mousePos) {
	return _state == state_animal_puh && hgeRect(_pos.x - 50, _pos.y - 100, _pos.x + 50, _pos.y).TestPoint(mousePos.x, mousePos.y);
}

void Animal::OnMouseDown(const FPoint2D &mousePos) {
	AnnaPers::NewAction("", this);
}

Animal::AnimalStates Animal::GetState() {
	return _state;
}

void Animal::CutFluff() {
	_waitingTimeCounter = 0.f;
	SwitchToWalk();
}
