#include "GrandpaPers.h"
#include "../Core/Core.h"
#include "PersPaths.h"
#include "GameField.h"
#include "ProductManager.h"

GrandpaPers::~GrandpaPers() 
{
	delete _front[1];
	delete _leftFront[1];
	delete _leftFrontIdle[1];
	delete _left[1];
	delete _back[1];
	delete _leftBack[1];
	_current = NULL;
}

GrandpaPers::GrandpaPers() 
: SPEED(150.f)
, _mirror(true)
, _showAnimationCounter(0.f)
{
	_pos = PersPaths::RandomPos();

	_front[0] = Core::getAnimation("grandpa_basic_front");
	_leftFront[0] = Core::getAnimation("grandpa_basic_leftfront");
	_leftFrontIdle[0] = Core::getAnimation("grandpa_basic_leftfront_idle");
	_left[0] = Core::getAnimation("grandpa_basic_left");
	_back[0] = Core::getAnimation("grandpa_basic_back");
	_leftBack[0] = Core::getAnimation("grandpa_basic_leftback");

	_front[1] = new Animation(*Core::getAnimation("grandpa_basic_front_hold"));
	_leftFront[1] = new Animation(*Core::getAnimation("grandpa_basic_leftfront_hold"));
	_leftFrontIdle[1] = new Animation(*Core::getAnimation("grandpa_basic_leftfront_idle_hold"));
	_left[1] = new Animation(*Core::getAnimation("grandpa_basic_left_hold"));
	_back[1] = new Animation(*Core::getAnimation("grandpa_basic_back_hold"));
	_leftBack[1] = new Animation(*Core::getAnimation("grandpa_basic_leftback_hold"));

	_scythe = Core::getAnimation("grandpa_scythe");
	_seeding = Core::getAnimation("grandpa_basic_left");

	_shadow = Core::getTexture("persshadow");

	_endTarget = _currentTarget = _pos;
	_current = _leftFrontIdle[0];
	_current->SetPos(_pos, _mirror);
	_currentAction.complete = true;
}

void GrandpaPers::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	_shadow->Render( - _shadow->Width() / 2,  - _shadow->Height() / 2);
	if (_mirror) {
		Render::MatrixScale(-1.f, 1.f);
	}
	_current->Draw();
	Render::PopMatrix();
}

void GrandpaPers::Update(float dt) {
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
			}
		} else {
			return;
		}
	}
	if (_currentAction.complete && ReadAction()) {
		if (_currentAction.treePlace) {
			if ((_pos - _currentAction.treePlace->grandpaPos).Length() < 16.f) {
				CheckAction();
			} else {
				Goto(_currentAction.treePlace->grandpaPos);
			}
		} else if (_currentAction.productPlace) {
			if ((_pos - _currentAction.productPlace->grandpaPos).Length() < 16.f) {
				CheckAction();
			} else {
				Goto(_currentAction.productPlace->grandpaPos);
			}
		} else if (_currentAction.wellPlace) {
			if ((_pos - _currentAction.wellPlace->grandpaPos).Length() < 16.f) {
				CheckAction();
			} else {
				Goto(_currentAction.wellPlace->grandpaPos);
			}
		} else if (_currentAction.fieldPlace) {
			if ((_pos - _currentAction.fieldPlace->GetGrandpaPos()).Length() < 16.f) {
				CheckAction();
				return;
			} else {
				Goto(_currentAction.fieldPlace->GetGrandpaPos());
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

void GrandpaPers::Goto(FPoint2D position) {
	_endTarget = position;
}

int GrandpaPers::_product;
std::string GrandpaPers::_productType;
Animation *GrandpaPers::_current = NULL;
Animation *GrandpaPers::_front[2];
Animation *GrandpaPers::_leftFront[2];
Animation *GrandpaPers::_leftFrontIdle[2];
Animation *GrandpaPers::_left[2];
Animation *GrandpaPers::_back[2];
Animation *GrandpaPers::_leftBack[2];
Animation *GrandpaPers::_seeding;
Animation *GrandpaPers::_scythe;

bool GrandpaPers::SetProduct(const std::string &productType) {
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

std::string GrandpaPers::DropProduct() {
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
		assert(false);
	}
	_product = 0;
	return _productType;
}

bool GrandpaPers::ReadAction() {
	if (_actions.size()) {
		_currentAction = _actions.front();
		_currentAction.complete = false;
		_actions.pop_front();
		return true;
	}
	return false;
}

Actions GrandpaPers::_actions;

void GrandpaPers::NewAction(const std::string &action, ProductPlace *productPlace) {
	Action a;
	a.type = action;
	a.treePlace = NULL;
	a.productPlace = productPlace;
	a.wellPlace = NULL;
	_actions.push_back(a);
}

void GrandpaPers::NewAction(const std::string &action, TreePlace *treePlace) {
	Action a;
	a.type = action;
	a.treePlace = treePlace;
	a.productPlace = NULL;
	a.wellPlace = NULL;
	_actions.push_back(a);
}

void GrandpaPers::NewAction(const std::string &action, EnvWell *wellPlace) {
	Action a;
	a.type = action;
	a.wellPlace = wellPlace;
	a.treePlace = NULL;
	a.productPlace = NULL;
	_actions.push_back(a);
}

void GrandpaPers::NewAction(const std::string &action, BaseField *fieldPlace) {
	Action a;
	a.type = action;
	a.fieldPlace = fieldPlace;
	_actions.push_back(a);
}

bool GrandpaPers::CheckAction() {
	if (!_currentAction.complete) {
		_currentAction.complete = true;
		if (_currentAction.productPlace) {
			if (_currentAction.productPlace/* == GameField::GetBuckPlace()*/) {
				assert(_product == 1 && !_currentAction.productPlace->IsBusy());
				_currentAction.productPlace->SetProduct(_productType, _pos.x > _currentAction.productPlace->_pos.x);
				DropProduct();
			} else {
				assert(false);// вроде мертвая ветка - можно удалить, но пока оставлю, 
				// если не будет вылетать тут на ассерте можно удалить в январе 2012
				std::string action = _currentAction.productPlace->WhatToDo();
				if (action == "put" && _product == 1) {
					_currentAction.productPlace->SetProduct(_productType, _pos.x > _currentAction.productPlace->_pos.x);
					DropProduct();
				} else if (action == "take" && _product == 0) {
					std::string pt = _currentAction.productPlace->GetProduct();
					if (pt != "") {
						SetProduct(pt);
					}
				}
			}
		} else if (_currentAction.treePlace) {
			DropProduct();
			std::string product;
			TreePlaceState s = _currentAction.treePlace->WhatToDo(product);
			if (s == tps_waiting_harvest) {
				ProductPlace *p = ProductManager::GetPlaceProduct(pt_tree);
				if (p != NULL) {
					Action a;
					a.complete = false;
					a.foodPlace = NULL;
					a.waterPlace = NULL;
					a.treePlace = NULL;
					a.wellPlace = NULL;
					a.plantPlace = NULL;
					a.productPlace = p;
					a.type = "put";
					_actions.push_front(a);
					_currentAction.treePlace->SomeAction();
					SetProduct(product);
				} else {
					_currentAction.treePlace->SetWaiting(false);
				}
			} else {
				_currentAction.treePlace->SomeAction();
			}
		} else if (_currentAction.wellPlace) {
			if (GameField::GetBuckPlace()->IsBusy() && _actions.front().productPlace == GameField::GetBuckPlace()) {
				// выкидываем из очереди поход к месту с водой
				ReadAction();
				_currentAction.complete = true;
			} else {
				SetProduct("water");
			}
		} else if (_currentAction.fieldPlace) {
			std::string product;
			PlantPlaceState s = _currentAction.fieldPlace->WhatToDo(product);
			if (s == no_plant) {
				_showAnimationCounter = 3.f;
				_currentAction.fieldPlace->SomeAction(_showAnimationCounter);
				_mirror = _currentAction.fieldPlace->_pos.x > _pos.x;
				_currentAction.Reset();
				_current = _seeding;
				_currentAction.animations.push_back(std::make_pair(_showAnimationCounter, _current));
				return true;
			} else if (s == harvesting) {
				_showAnimationCounter = 2.f;
				_currentAction.fieldPlace->SomeAction(_showAnimationCounter);
				_mirror = false;
				_currentAction.Reset();
				_current = _scythe;
				_currentAction.animations.push_back(std::make_pair(_showAnimationCounter, _current));
				return true;
			}
		}
	}
	return false;
}
