#include "stdafx.h"
#include "AnimEditor.h"
#include "../Core/Core.h"

AnimEditor::AnimEditor() 
: _currentAnimation(NULL)
, _timeCounter(0.f)
, exitPressed(false)
{
}

void AnimEditor::Draw() {
	if (_currentAnimation) {
		Render::PushMatrix();
		Render::MatrixMove(400, 300);
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
	return (_currentAnimation != NULL);
}
