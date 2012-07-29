#include "Byker.h"

Byker::Byker() 
: physic(0.f, 0.f, 0.8f)
{
	_ride = My::AnimationManager::getAnimation("ride");
	_timer = 0.f;
	_attachedBody = NULL;
}

Byker::~Byker() {
}

void Byker::Draw() {
	_ride->Draw(_timer);
}

void Byker::Update(float dt) {
	_timer += dt / _ride->Time();
	while (_timer > 1.f) {
		_timer -= 1.f;
	}
}

void Byker::SetPos(FPoint2D pos) {
	_ride->SetPos(pos, false);
}
