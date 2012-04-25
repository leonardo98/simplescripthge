#include "AnimationViewer.h"
#include "../Core/Core.h"

AnimationViewer::AnimationViewer(TiXmlElement *xe)
: _mirror(false)
{
	_pos.x = atof(xe->Attribute("x"));
	_pos.y = atof(xe->Attribute("y"));
//	_animation = Core::getAnimation(xe->Attribute("animationId"));
}

AnimationViewer::~AnimationViewer() {
}

void AnimationViewer::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	if (_mirror) {
		Render::MatrixScale(-1.f, 1.f);
	}
	//_animation->Draw();
	Render::PopMatrix();
}

void AnimationViewer::Update(float dt) {
//	_animation->Update(dt);
	if (Render::GetDC()->Input_KeyDown(HGEK_BACKSPACE)) {
		_mirror = !_mirror;
	}
}
