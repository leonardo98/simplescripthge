#include "Waterpan.h"
#include "../Core/Core.h"
#include "GameField.h"

Waterpan::Waterpan(int index) 
: _effect(0.f)
, MAX_LEVELS(3)
{
	Matrix m;
	m.Move(1024, 0);
	m.Scale(-1.f, 1.f);
	_shadow.Set(Core::getTexture("textures\\env_pans\\waterpan01_sh.png"));
	_down.Set(Core::getTexture("textures\\env_pans\\waterpan01_dn.png"));
	_water.Set(Core::getTexture("textures\\env_pans\\waterpan_water.png"));
	_up.Set(Core::getTexture("textures\\env_pans\\waterpan01_up.png"));
	_pos.y = 619;
	if (index == 1) {
		annaPos = grandpaPos = bobPos;
		bobPos.x = 370; bobPos.y = 760;
	} else if (index == 2) {
		_shadow.SetTransform(m);
		_down.SetTransform(m);
		_water.SetTransform(m);
		_up.SetTransform(m);
		bobPos.x = 650; bobPos.y = 760;
		annaPos = grandpaPos = bobPos;
	} else {
		assert(false);
	}
	annaPos = PersPaths::SearchNearest(annaPos);
	bobPos = PersPaths::SearchNearest(bobPos);
	grandpaPos = PersPaths::SearchNearest(grandpaPos);
	_oldLevel = _level = 0.f;
}

void Waterpan::DrawBottom() {
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

void Waterpan::Draw() {
	if (_effect > 0.f) {
		_down.Render();
		Render::PushMatrix();
		float f = _level + (_oldLevel - _level) * _effect;
		Render::MatrixMove(0.f, (1.f - f) * 9.f);
		Render::SetAlpha(static_cast<unsigned char>(0xFF * sin(M_PI_2 * f)));
		_water.Render();
		Render::SetAlpha(0xFF);
		Render::PopMatrix();
		_up.Render();
	} else {
		_down.Render();
		Render::PushMatrix();
		Render::MatrixMove(0.f, (1.f - _level) * 9.f);
		Render::SetAlpha(static_cast<unsigned char>(0xFF * sin(M_PI_2 * _level)));
		_water.Render();
		Render::SetAlpha(0xFF);
		Render::PopMatrix();
		_up.Render();
	}
}

void Waterpan::Update(float dt) {
	if (_effect > 0.f) {
		_effect -= dt * 3;
		if (_effect <= 0.f) {
			_oldLevel = _level;
		}
	}
}

void Waterpan::OnMouseDown(const FPoint2D &mousePos) {
	BobPers::NewAction("", this);
	/*
	if (_state == empty) {
		_effect = 1.f;
	} else if (_state == level) {
		_level -= 1.f / MAX_LEVELS;
		_effect = 1.f;
	}
	*/
}

bool Waterpan::IsUnderMouse(const FPoint2D &mousePos) {
	return (_up.PixelCheck(mousePos) || _down.PixelCheck(mousePos));
}

void Waterpan::SomeAction() {
	if (_level < MAX_LEVELS) {
		_effect = 1.f;
		_oldLevel = _level;
		_level = 1.f;
	}
}

//f = _level + _effect / MAX_LEVELS;
