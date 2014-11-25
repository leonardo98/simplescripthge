#include "AnimalsManager.h"
#include "../Core/Math.h"

std::vector<Animal *> AnimalsManager::_animals;

void AnimalsManager::Init(Waterpan *waterpan, Foodpan *foodpan) {
	_waterpan = waterpan;
	_foodpan = foodpan;
}

float AnimalsManager::Distance(const Animal *a, const Animal *b) {
	FPoint2D r = (b->_pos - a->_pos);
	r.y *= 1.5f;
	return r.Length();
}

void AnimalsManager::UpdatePosition(Animal *a, float dt) {
	assert(-100 - 10.f <= a->_pos.x && a->_pos.x <= 1124);
	assert(0 <= a->_pos.y && a->_pos.y <= 768);
	FPoint2D force(0.f, 0.f);
	const float MIN_DIST = 35;
	for (unsigned int i = 0; i < _animals.size(); ++i) {
		float dist = Distance(_animals[i], a) + 1.f;
		if (_animals[i] != a && dist < 2 * MIN_DIST) {
			float f = 0.2f * MIN_DIST / dist;
			force += f * (a->_pos - _animals[i]->_pos);
		}
	}
	FPoint2D dir = a->GetDirection();
	FPoint2D oldForce = force;
	if (force.Length() > 0.1f && dir.Length() > 0.1f) {
		float t = dir.y;
		dir.y = dir.x;
		dir.x = -t;
		force = Math::SMul(force, dir) * dir;
		if (force.Length() > 0.1f) {
			force *= ( (Animal::SPEED * dt * 0.5f) / force.Length() );
			a->_pos += force;
		}
	}
	assert(-100 - 10.f <= a->_pos.x && a->_pos.x <= 1124);
	assert(0 <= a->_pos.y && a->_pos.y <= 768);
}

bool AnimalsManager::FreePosition(Animal *a) {
	const float MIN_DIST = 25;
	float dist;
	FPoint2D dir;
	for (unsigned int i = 0; i < _animals.size(); ++i) {
		dist = Distance(_animals[i], a) + 1.f;
		dir = _animals[i]->GetDirection();
		if (_animals[i] != a && dir.Length() < 1.f && dist < 2 * MIN_DIST) {
			return false;
		}
	}
	return true;
}

int AnimalsManager::Size() {
	return _animals.size();
}

bool AnimalsManager::IsWaterBusy() {
	for (Animals::iterator i = _animals.begin(), e = _animals.end(); i != e; ++i) {
		if ((*i)->_state == Animal::state_want_drink ||(*i)->_nextState == Animal::state_want_drink) {
			return true;
		}
	}
    return false;
}

bool AnimalsManager::IsFoodBusy() {
	for (Animals::iterator i = _animals.begin(), e = _animals.end(); i != e; ++i) {
		if ((*i)->_state == Animal::state_want_eat ||(*i)->_nextState == Animal::state_want_eat) {
			return true;
		}
	}
    return false;
}

Waterpan *AnimalsManager::_waterpan = NULL;
Foodpan *AnimalsManager::_foodpan = NULL;

bool AnimalsManager::IsWaterEmpty() {
	assert(_waterpan != NULL);
	return _waterpan->Empty();
}

void AnimalsManager::DrinkWater() {
	assert(_waterpan != NULL);
	return _waterpan->DrinkWater();
}

bool AnimalsManager::IsFoodEmpty() {
	assert(_foodpan != NULL);
	return _foodpan->Empty();
}

void AnimalsManager::EatFood() {
	assert(_foodpan != NULL);
	return _foodpan->EatFood();
}
