#include "BirdsManager.h"
#include "../Core/Math.h"

std::vector<Archaeopteryx *> BirdsManager::_positions;

BirdsManager::BirdsManager() {}

float BirdsManager::Distance(const Archaeopteryx *a, const Archaeopteryx *b) {
	FPoint2D r = (b->_pos - a->_pos);
	r.y *= 1.5f;
	return r.Length();
}

void BirdsManager::UpdatePosition(Archaeopteryx *a, float dt) {
	assert(0 <= a->_pos.x && a->_pos.x <= 1024);
	assert(0 <= a->_pos.y && a->_pos.y <= 768);
	FPoint2D force(0.f, 0.f);
	const float MIN_DIST = 35;
	for (unsigned int i = 0; i < _positions.size(); ++i) {
		float dist = Distance(_positions[i], a) + 1.f;
		if (_positions[i] != a && dist < 2 * MIN_DIST) {
			float f = 0.2f * MIN_DIST / dist;
			force += f * (a->_pos - _positions[i]->_pos);
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
	assert(0 <= a->_pos.x && a->_pos.x <= 1024);
	assert(0 <= a->_pos.y && a->_pos.y <= 768);
}

bool BirdsManager::FreePosition(Archaeopteryx *a) {
	const float MIN_DIST = 25;
	float dist;
	FPoint2D dir;
	for (unsigned int i = 0; i < _positions.size(); ++i) {
		dist = Distance(_positions[i], a) + 1.f;
		dir = _positions[i]->GetDirection();
		if (_positions[i] != a && dir.Length() < 1.f && dist < 2 * MIN_DIST) {
			return false;
		}
	}
	return true;
}

int BirdsManager::Size() {
	return _positions.size();
}
