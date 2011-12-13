#include "Foodpan.h"
#include "../Core/Core.h"
#include "GameField.h"
#include "../Core/Math.h"

Foodpan::Foodpan(int index) 
: _effect(0.f)
, MAX_LEVELS(6)
{
	Matrix m;
	m.Move(1024, 0);
	m.Scale(-1.f, 1.f);
	_shadow.Set(Core::getTexture("textures\\env_pans\\foodpan01_sh.png"));
	_pan.Set(Core::getTexture("textures\\env_pans\\foodpan01.png"));
	_level = 0;
	_pos.y = 514.f;
	_food.resize(MAX_LEVELS);
	char buff[10];
	if (index == 1) {
		for (int i = 1; i <= MAX_LEVELS; ++i) {
			sprintf(buff, "%i", i);
			_food[i - 1].Set(Core::getTexture(std::string("textures\\env_pans\\panclover_0") + buff + ".png"));
		}
		bobPos.x = 390; bobPos.y = 550;
		annaPos = grandpaPos = bobPos;
		_action = "clover";
	} else if (index == 2) {
		_shadow.SetTransform(m);
		_pan.SetTransform(m);
		for (int i = 1; i <= MAX_LEVELS; ++i) {
			sprintf(buff, "%i", i);
			_food[i - 1].Set(Core::getTexture(std::string("textures\\env_pans\\pangrain_0") + buff + ".png"));
		}
		bobPos.x = 630; bobPos.y = 550;
		annaPos = grandpaPos = bobPos;
		_action = "grain";
	} else {
		assert(false);
	}
	annaPos = PersPaths::SearchNearest(annaPos);
	bobPos = PersPaths::SearchNearest(bobPos);
	grandpaPos = PersPaths::SearchNearest(grandpaPos);
}

void Foodpan::DrawBottom() {
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

void Foodpan::Draw() {
	_pan.Render();
	if (_effect > 0.f) {
		int l = Math::round((1.f - _effect) * (MAX_LEVELS - 1));
		_food[l].Render();
	} else if (_level > 0) {
		_food[_level - 1].Render();
	} 
}

void Foodpan::Update(float dt) {
	if (_effect > 0.f) {
		_effect -= dt / 0.666;
	}
}

void Foodpan::OnMouseDown(const FPoint2D &mousePos) {
	BobPers::NewAction("", this);
}

bool Foodpan::IsUnderMouse(const FPoint2D &mousePos) {
	return (_pan.PixelCheck(mousePos) || (_level > 0 && _food[_level - 1].PixelCheck(mousePos)));
}

void Foodpan::SomeAction() {
	_effect = 1.f;
	_level = MAX_LEVELS;
}

std::string Foodpan::WhatToDo() {
	return _action;
}