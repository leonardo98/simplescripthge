#include "SwampPlace.h"
#include "../Core/Core.h"
#include "GameField.h"
#include "ProductManager.h"

SwampPlace::SwampPlace() {
	_grass.Set(Core::getTexture("textures\\env_jungle\\swamp_grass.png"));
	_swamp.Set(Core::getTexture("textures\\env_jungle\\swamp.png"));
	_tabDn.Set(Core::getTexture("textures\\env_jungle\\swamp_tab_dn.png"));
	_tabUp.Set(Core::getTexture("textures\\env_jungle\\swamp_tab_up.png"));
	_shakeTimeCounter = 0.f;
	_waitShakeTimeCounter = 0.f;
	_pos = FPoint2D(968, 498);
}

void SwampPlace::InnerDraw() {
	_swamp.Render();
	if (_shakeTimeCounter > 0.f) {
		Render::PushMatrix();
		Render::MatrixMove(984.f, 496.f);
		Render::MatrixRotate(_shakeTimeCounter * 0.075f * sin(M_PI * 4 * _shakeTimeCounter));
		Render::MatrixMove(-984.f, -496.f);
		_tabDn.Render();
		_tabUp.Render();
		Render::PopMatrix();
	} else {
		_tabDn.Render();
		_tabUp.Render();
	}
}

void SwampPlace::DrawBottom() {
	_grass.Render();
	if (_active) {
		float b = GameField::SELECTION_BORDER;
		Render::SetBlendMode(3);
		Render::PushMatrix();
		Render::MatrixMove(-b, 0);
		InnerDraw();
		Render::MatrixMove(2 * b, 0);
		InnerDraw();
		Render::MatrixMove(-b, b);
		InnerDraw();
		Render::MatrixMove(0, -2 * b);
		InnerDraw();
		Render::PopMatrix();
		Render::SetBlendMode(BLEND_DEFAULT);
	}
	InnerDraw();
}

void SwampPlace::Update(float dt) {
	if (_waitShakeTimeCounter > 0.f) {
		_waitShakeTimeCounter -= dt;
		if (_waitShakeTimeCounter <= 0.f) {
			_shakeTimeCounter = 1.f;
		}
	} else if (_shakeTimeCounter > 0.f) {
		_shakeTimeCounter -= dt;
	}
}

bool SwampPlace::IsUnderMouse(const FPoint2D &mousePos) {
	return _swamp.PixelCheck(mousePos) || _tabUp.PixelCheck(mousePos) || _tabDn.PixelCheck(mousePos);
}

void SwampPlace::OnMouseDown(const FPoint2D &mousePos) {
	_shakeTimeCounter = 1.f;
	if (AnnaPers::GetProductType() != "") {
		_waitShakeTimeCounter = GameField::AddDropEffect("Anna", _pos, 20.f);
		AnnaPers::DropProduct();
	}
}

void SwampPlace::OnMouseMove(const FPoint2D &mousePos) {
}
