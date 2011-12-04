#include "AnimationViewer.h"
#include "../Core/Core.h"

AnimationViewer::AnimationViewer(TiXmlElement *xe)
: _mirror(false)
{
	_pos.x = atof(xe->Attribute("x"));
	_pos.y = atof(xe->Attribute("y"));
	_animation = Core::getAnimation(xe->Attribute("animationId"));
	_animation->SetPos(_pos, _mirror);
}

AnimationViewer::~AnimationViewer() {
}

void AnimationViewer::Draw() {
	_animation->Draw();
}

void AnimationViewer::Update(float dt) {
	_animation->Update(dt);
	if (Render::GetDC()->Input_KeyDown(HGEK_BACKSPACE)) {
		_mirror = !_mirror;
		_animation->SetPos(_pos, _mirror);
	}
}
