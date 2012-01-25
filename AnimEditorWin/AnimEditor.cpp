#include "stdafx.h"
#include "AnimEditor.h"
#include "../Core/Core.h"

AnimEditor::AnimEditor() 
: _currentAnimation(NULL)
, _timeCounter(0.f)
{
}

void AnimEditor::Draw() {
	if (_currentAnimation) {
		Render::PushMatrix();
		Render::MatrixMove(_position.x, _position.y);
		_currentAnimation->Draw(_timeCounter / _currentAnimation->Time());
		Render::PopMatrix();
	}
}

void AnimEditor::Update(float dt) {
	if (_currentAnimation) {
		_timeCounter += dt;
		if (_timeCounter >= _currentAnimation->Time()) {
			_timeCounter -= static_cast<int>(_timeCounter / _currentAnimation->Time()) * _currentAnimation->Time();
		}
	}
}

bool AnimEditor::SetCurrent(const char *name) {
	if (_currentAnimation) {
		_currentAnimation->UnloadTextures();
		Core::ReleaseAllTextures();
	}
	_currentAnimation = Core::getAnimation(name);
	_position.x = Render::GetDC()->System_GetState(HGE_SCREENWIDTH) / 2.f;
	_position.y = Render::GetDC()->System_GetState(HGE_SCREENHEIGHT) / 2.f;
	return (_currentAnimation != NULL);
}

void AnimEditor::CreateTree(void *parent, CallBones myCall) {
	_currentAnimation->EditorCall(myCall, parent);
}

Animation * AnimEditor::Anim() {
	return _currentAnimation;
}
