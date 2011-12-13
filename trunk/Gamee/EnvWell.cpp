#include "EnvWell.h"
#include "../Core/Core.h"
#include "GameField.h"

EnvWell::EnvWell() 
: _drawBuck(true)
, _effect(0.f)
{
	_shadow.Set(Core::getTexture("textures\\env_well01\\well1_sh.png"));
	_well.Set(Core::getTexture("textures\\env_well01\\well1_basis.png"));
	_buck.Set(Core::getTexture("textures\\env_well01\\well1_buck.png"));
	_pos.y = 400.f;
	annaPos.x = 589; annaPos.y = 409;
	bobPos.x = 589; bobPos.y = 409;
	grandpaPos.x = 589; grandpaPos.y = 409;
	annaPos = PersPaths::SearchNearest(annaPos);
	bobPos = PersPaths::SearchNearest(bobPos);
	grandpaPos = PersPaths::SearchNearest(grandpaPos);
}

void EnvWell::DrawBottom() {
	_shadow.Render();
	if (_active) {
		float b = GameField::SELECTION_BORDER;
		Render::SetBlendMode(3);
		Render::PushMatrix();
		Render::MatrixMove(-b, 0);
		Draw();
		Render::MatrixMove(2 * b, 0);
		Draw();
		Render::MatrixMove(-b, b);
		Draw();
		Render::MatrixMove(0, -2 * b);
		Draw();
		Render::PopMatrix();
		Render::SetBlendMode(BLEND_DEFAULT);
	}
}

void EnvWell::Draw() {
	_well.Render();
	if (_drawBuck) {
		_buck.Render();
	}
}

void EnvWell::Update(float dt) {
	if (_effect > 0.f) {
		_effect -= dt * 4;
	}
}

void EnvWell::OnMouseDown(const FPoint2D &mousePos) {
	GrandpaPers::NewAction("", this);
	GrandpaPers::NewAction("", GameField::GetBuckPlace());
}

bool EnvWell::IsUnderMouse(const FPoint2D &mousePos) {
	return (_well.PixelCheck(mousePos) || (_drawBuck && _buck.PixelCheck(mousePos)));
}
