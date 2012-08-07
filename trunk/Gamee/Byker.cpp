#include "Byker.h"

Byker::Byker() 
: physic(0.f, 0.f, 0.65f)
{
	_byker_balans_2 = My::AnimationManager::getAnimation("byker_balans_2");
	_ride_wheels = My::AnimationManager::getAnimation("ride_wheels");
	_timer = 0.f;
//	_attachedBody = NULL;
}

Byker::~Byker() {
}

void Byker::Draw(float angle){
	_ride_wheels->Draw(_timer);
	angle = min(1.f, max(0.f, (angle + 0.7f) / 1.4f));
	_byker_balans_2->Draw(angle);
}

void Byker::Update(float dt) {
	_timer += dt / _ride_wheels->Time();
	while (_timer > 1.f) {
		_timer -= 1.f;
	}
}

void Byker::SetPos(FPoint2D pos) {
	_ride_wheels->SetPos(pos, false);
	_byker_balans_2->SetPos(pos, false);
}
