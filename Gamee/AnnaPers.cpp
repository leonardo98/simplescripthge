#include "AnnaPers.h"
#include "../Core/Core.h"
#include "PersPaths.h"

AnnaPers::~AnnaPers() 
{
	delete _front[1];
	delete _leftFront[1];
	delete _leftFrontIdle[1];
	delete _left[1];
	delete _back[1];
	delete _leftBack[1];
	_current = NULL;
}

AnnaPers::AnnaPers() 
: SPEED(150.f)
, _harvesting(NULL)
{
	_showAnimationCounter = 0.f;
	assert(_current == NULL);
	_product = 0;
	_productType = "none";

	_pos = PersPaths::RandomPos();

	_front[0] = Core::getAnimation("anna_basic_front");
	_leftFront[0] = Core::getAnimation("anna_basic_leftfront");
	_leftFrontIdle[0] = Core::getAnimation("anna_basic_leftfront_idle");
	_left[0] = Core::getAnimation("anna_basic_left");
	_back[0] = Core::getAnimation("anna_basic_back");
	_leftBack[0] = Core::getAnimation("anna_basic_leftback");
	_downup = Core::getAnimation("anna_plant_downup");
	_seeding = Core::getAnimation("anna_plant");

	_front[1] = new Animation(*Core::getAnimation("anna_basic_front_hold"));
	_leftFront[1] = new Animation(*Core::getAnimation("anna_basic_leftfront_hold"));
	_leftFrontIdle[1] = new Animation(*Core::getAnimation("anna_basic_leftfront_idle_hold"));
	_left[1] = new Animation(*Core::getAnimation("anna_basic_left_hold"));
	_back[1] = new Animation(*Core::getAnimation("anna_basic_back_hold"));
	_leftBack[1] = new Animation(*Core::getAnimation("anna_basic_leftback_hold"));

	_shadow = Core::getTexture("persshadow");

	_endTarget = _currentTarget = _pos;
	_current = _leftFrontIdle[0];
	_current->SetPos(_pos, _mirror);
	_currentAction.complete = true;
	_mirror = _pos.x < 512.f;
	_productOffset = FPoint2D(-24, -59);
}

void AnnaPers::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	_shadow->Render( - _shadow->Width() / 2,  - _shadow->Height() / 2);
	if (_mirror) {
		Render::MatrixScale(-1.f, 1.f);
	}
	_current->Draw();
	Render::PopMatrix();
	if (_harvesting) {
		_harvesting->Draw();
	}
}

void AnnaPers::Update(float dt) {
	if (_showAnimationCounter > 0.f) {
		_showAnimationCounter -= dt;
		_current->Update(dt);
		if (_showAnimationCounter < 0.f) {
			_currentAction.animations.pop_front();
			if (_currentAction.animations.size()) {
				_showAnimationCounter = _currentAction.animations.front().first;
				_current = _currentAction.animations.front().second;
				return;
			} else {
				_currentAction.complete = true;
				_harvesting = NULL;
			}
		} else {
			return;
		}
	}
	if (_currentAction.complete && ReadAction()) {
		if (_currentAction.plantPlace) {
			if ((_pos - _currentAction.plantPlace->annaPos).Length() < 16.f) {
				if (CheckAction()) {
					return;
				}
			} else {
				Goto(_currentAction.plantPlace->annaPos);
			}
		} else if (_currentAction.productPlace) {
			std::string action = _currentAction.productPlace->WhatToDo();
			if (action == "put" && _product == 1 && _productType != "eggs_dodo") {
				//_currentAction.productPlace->SetProduct(_productType, _pos.x > _currentAction.productPlace->_pos.x);
				//DropProduct();
				_currentAction.productPlace->OnMouseDown(_currentAction.productPlace->_pos);// hack
				_currentAction.complete = true;
			} else if ((_pos - _currentAction.productPlace->annaPos).Length() < 16.f) {
				CheckAction();
			} else {
				Goto(_currentAction.productPlace->annaPos);
			}
		} else {
			assert(false);
		}
	}
	_current->Update(dt);
	if ((_pos - _currentTarget).Length() > SPEED * dt) {
		_pos += *(_currentTarget - _pos).Normalize() * SPEED * dt;
		_mirror = (_pos.x < _currentTarget.x);
		return;
	}
	if ((_pos - _endTarget).Length() > SPEED * dt) {
		FPoint2D newTarget = PersPaths::NextPointTo(_currentTarget, _endTarget);
		_currentTarget = newTarget;
		if ((_pos - _currentTarget).Length() <= 1.f) {
			_endTarget = _currentTarget = _pos;
			_current = _leftFrontIdle[_product];
			_mirror = (_pos.x < 512);
			CheckAction();
			return;
		}

		float angle = atan2(_currentTarget.y - _pos.y, _currentTarget.x - _pos.x);
		if (angle < 0) {
			angle += 2 * M_PI;
		}
		float ENLARGE = M_PI / 8 / 4;
		if (M_PI / 8 - ENLARGE <= angle && angle <= 3 * M_PI / 8 + ENLARGE) {
			_current = _leftFront[_product];
		} else if (5 * M_PI / 8 - ENLARGE <= angle && angle <= 7 * M_PI / 8 + ENLARGE) {
			_current = _leftFront[_product];
		} else if (9 * M_PI / 8 - ENLARGE <= angle && angle <= 11 * M_PI / 8 + ENLARGE) {
			_current = _leftBack[_product];
		} else if (13 * M_PI / 8 - ENLARGE <= angle && angle <= 15 * M_PI / 8 + ENLARGE) {
			_current = _leftBack[_product];
		} else if (angle <= M_PI / 8) {
			_current = _left[_product];
		} else if (3 * M_PI / 8 <= angle && angle <= 5 * M_PI / 8) {
			_current = _front[_product];
		} else if (11 * M_PI / 8 <= angle && angle <= 13 * M_PI / 8) {
			_current = _back[_product];
		} else {
			_current = _left[_product];
		}
		_pos += *(_currentTarget - _pos).Normalize() * SPEED * dt;
		_mirror = (_pos.x < _currentTarget.x);
		return;
	}
	_pos = _endTarget;
	if (_current != _leftFrontIdle[_product]) {
		_current = _leftFrontIdle[_product];
		_mirror = (_pos.x < 512);
		CheckAction();
	}
}

void AnnaPers::Goto(FPoint2D position) {
	_endTarget = position;
}

int AnnaPers::_product;
std::string AnnaPers::_productType;
Animation *AnnaPers::_current = NULL;
Animation *AnnaPers::_front[2];
Animation *AnnaPers::_leftFront[2];
Animation *AnnaPers::_leftFrontIdle[2];
Animation *AnnaPers::_left[2];
Animation *AnnaPers::_back[2];
Animation *AnnaPers::_leftBack[2];
Animation *AnnaPers::_downup;
Animation *AnnaPers::_seeding;
float AnnaPers::_showAnimationCounter;

std::string AnnaPers::GetProductType() {
	if (_product == 0 || _showAnimationCounter > 0.f) {
		return "";
	}
	return _productType;
}

bool AnnaPers::SetProduct(const std::string &productType) {
	if (_product != 0) {
		return false;
	}
	_productType = productType;
	_product = 1;
	std::string id = "gui_" + productType;
	_front[1]->ReplaceTexture("product", id.c_str());
	_leftFront[1]->ReplaceTexture("product", id.c_str());
	_leftFrontIdle[1]->ReplaceTexture("product", id.c_str());
	_left[1]->ReplaceTexture("product", id.c_str());
	_back[1]->ReplaceTexture("product", id.c_str());
	_leftBack[1]->ReplaceTexture("product", id.c_str());
	if (_current == _front[0]) {
		_current = _front[1];
	} else if (_current == _leftFront[0]) {
		_current = _leftFront[1];
	} else if (_current == _leftFrontIdle[0]) {
		_current = _leftFrontIdle[1];
	} else if (_current == _left[0]) {
		_current = _left[1];
	} else if (_current == _back[0]) {
		_current = _back[1];
	} else if (_current == _leftBack[0]) {
		_current = _leftBack[1];
	} else {
		assert(false);
	}
	return true;
}

std::string AnnaPers::DropProduct() {
	if (_product == 0) {
		return "none";
	}
	if (_current == _front[1]) {
		_current = _front[0];
	} else if (_current == _leftFront[1]) {
		_current = _leftFront[0];
	} else if (_current == _leftFrontIdle[1]) {
		_current = _leftFrontIdle[0];
	} else if (_current == _left[1]) {
		_current = _left[0];
	} else if (_current == _back[1]) {
		_current = _back[0];
	} else if (_current == _leftBack[1]) {
		_current = _leftBack[0];
	} else {
		//assert(false);
	}
	_product = 0;
	return _productType;
}

bool AnnaPers::ReadAction() {
	if (_actions.size()) {
		_currentAction = _actions.front();
		_currentAction.complete = false;
		_actions.pop_front();
		return true;
	}
	return false;
}

std::list<Action> AnnaPers::_actions;

void AnnaPers::NewAction(const std::string &action, ProductPlace *productPlace) {
	Action a;
	a.type = action;
	a.plantPlace = NULL;
	a.productPlace = productPlace;
	_actions.push_back(a);
}

void AnnaPers::NewAction(const std::string &action, PlantPlace *plantPlace) {
	Action a;
	a.type = action;
	a.plantPlace = plantPlace;
	a.productPlace = NULL;
	_actions.push_back(a);
}

bool AnnaPers::CheckAction() {
	if (!_currentAction.complete) {
		_currentAction.complete = true;
		if (_currentAction.productPlace) {
			std::string action = _currentAction.productPlace->WhatToDo();
			if (action == "put" && _product == 1) {
				//_currentAction.productPlace->OnMouseDown(_currentAction.productPlace->_pos);
				_currentAction.productPlace->SetProduct(_productType, _pos.x > _currentAction.productPlace->_pos.x);
				DropProduct();
			} else if (action == "take" && _product == 0) {
				std::string pt = _currentAction.productPlace->GetProduct();
				if (pt != "") {
					SetProduct(pt);
				}
			}
		} else if (_currentAction.plantPlace) {
			std::string product;
			PlantPlaceState s = _currentAction.plantPlace->WhatToDo(product);
			if (s == waiting_harvest && _product == 0) {
				SetProduct(product);
				_showAnimationCounter = 3.f;
				_harvesting = _currentAction.plantPlace;
				_harvesting->SomeAction(_showAnimationCounter);
				_mirror = _currentAction.plantPlace->_pos.x > _pos.x;
				_currentAction.Reset();
				_current = _seeding;
				_currentAction.animations.push_back(std::make_pair(_showAnimationCounter, _current));
				return true;
			} else if (s == waiting_water && _product == 1 && _productType == "water") {
				assert(false);// bob should do it
				//DropProduct();
				//_currentAction.plantPlace->SomeAction();
				_currentAction.productPlace->OnMouseDown(_currentAction.productPlace->_pos);
			} else if (s == waiting_seed) {
				_showAnimationCounter = 2.f;
				_currentAction.plantPlace->SomeAction(_showAnimationCounter);
				_mirror = _currentAction.plantPlace->_pos.x > _pos.x;
				_currentAction.Reset();
				_current = _seeding;
				_currentAction.animations.push_back(std::make_pair(_showAnimationCounter, _current));
				return true;
			}
		}
	}
	return false;
}
