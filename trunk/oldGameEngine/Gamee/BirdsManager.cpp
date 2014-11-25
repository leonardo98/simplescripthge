#include "BirdsManager.h"
#include "../Core/Math.h"

std::vector<Archaeopteryx *> BirdsManager::_birds;

void BirdsManager::Init(Waterpan *waterpan, Foodpan *foodpan) {
	_waterpan = waterpan;
	_foodpan = foodpan;
}

float BirdsManager::Distance(const Archaeopteryx *a, const Archaeopteryx *b) {
	FPoint2D r = (b->_pos - a->_pos);
	r.y *= 1.5f;
	return r.Length();
}

void BirdsManager::UpdatePosition(Archaeopteryx *a, float dt) {
	assert(0 <= a->_pos.x && a->_pos.x <= 1124);
	assert(0 <= a->_pos.y && a->_pos.y <= 768);
	FPoint2D force(0.f, 0.f);
	const float MIN_DIST = 35;
	for (unsigned int i = 0; i < _birds.size(); ++i) {
		float dist = Distance(_birds[i], a) + 1.f;
		if (_birds[i] != a && dist < 2 * MIN_DIST) {
			float f = 0.2f * MIN_DIST / dist;
			force += f * (a->_pos - _birds[i]->_pos);
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
			force *= ( (Archaeopteryx::SPEED * dt * 0.5f) / force.Length() );
			a->_pos += force;
		}
	}
	assert(0 <= a->_pos.x && a->_pos.x <= 1124);
	assert(0 <= a->_pos.y && a->_pos.y <= 768);
}

bool BirdsManager::FreePosition(Archaeopteryx *a) {
	const float MIN_DIST = 25;
	float dist;
	FPoint2D dir;
	for (unsigned int i = 0; i < _birds.size(); ++i) {
		dist = Distance(_birds[i], a) + 1.f;
		dir = _birds[i]->GetDirection();
		if (_birds[i] != a && dir.Length() < 1.f && dist < 2 * MIN_DIST) {
			return false;
		}
	}
	return true;
}

int BirdsManager::Size() {
	return _birds.size();
}

bool BirdsManager::IsWaterBusy() {
	for (Birds::iterator i = _birds.begin(), e = _birds.end(); i != e; ++i) {
		if ((*i)->_state == Archaeopteryx::state_want_drink ||(*i)->_nextState == Archaeopteryx::state_want_drink) {
			return true;
		}
	}
    return false;
}

bool BirdsManager::IsFoodBusy() {
	for (Birds::iterator i = _birds.begin(), e = _birds.end(); i != e; ++i) {
		if ((*i)->_state == Archaeopteryx::state_want_eat ||(*i)->_nextState == Archaeopteryx::state_want_eat) {
			return true;
		}
	}
    return false;
}

Waterpan *BirdsManager::_waterpan = NULL;
Foodpan *BirdsManager::_foodpan = NULL;

bool BirdsManager::IsWaterEmpty() {
	assert(_waterpan != NULL);
	return _waterpan->Empty();
}

void BirdsManager::DrinkWater() {
	assert(_waterpan != NULL);
	return _waterpan->DrinkWater();
}

bool BirdsManager::IsFoodEmpty() {
	assert(_foodpan != NULL);
	return _foodpan->Empty();
}

void BirdsManager::EatFood() {
	assert(_foodpan != NULL);
	return _foodpan->EatFood();
}
