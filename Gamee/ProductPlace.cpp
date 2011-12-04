#include "ProductPlace.h"
#include "../Core/Core.h"
#include "GameField.h"

ProductPlace::ProductPlace(int index) 
: _effect(0.f)
{
	Matrix m;
	m.Scale(-1.f, 1.f);
	m.Move(1024, 0);
	if (index == 1) {
		_shadow.Set(Core::getTexture("textures\\env_well01\\well1_waterplace_sh.png"));
		_place.Set(Core::getTexture("textures\\env_well01\\well1_waterplace.png"));
		_product = NULL;
		_pos.y = 478;
	} else if (index == 2) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_02_f05.png"));
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_02.png"));
		_product = NULL;
		_pos.y = 310;
	} else if (index == 3) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_03_f04.png"));
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_03.png"));
		_product = NULL;
		_pos.y = 267;
	} else if (index == 4) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_03_f04.png"));
		_shadow.PushTransform(m);
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_04.png"));
		_product = NULL;
		_pos.y = 267;
	} else if (index == 5) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_02_f05.png"));
		_shadow.PushTransform(m);
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_05.png"));
		_product = NULL;
		_pos.y = 310;
	} else if (index == 6) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\storage_sh.png"));
		_place.Set(Core::getTexture("textures\\env_jungle\\storage_strawx1.png"));
		_product = NULL;
		_pos.y = 343;
	} else if (index == 7) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\storage_sh.png"), 13, 61);
		_place.Set(Core::getTexture("textures\\env_jungle\\storage_strawx2.png"));
		_product = NULL;
		_pos.y = 403;
	}
}

void ProductPlace::DrawBottom() {
	_shadow.Render();
	if (_active) {
		float b = GameField::SELECTION_BORDER;
		Render::SetBlendMode(3);
		Render::PushMatrix();
		Render::MatrixMove(-b, 0);
		_place.Render();
		Draw();
		Render::MatrixMove(2 * b, 0);
		_place.Render();
		Draw();
		Render::MatrixMove(-b, b);
		_place.Render();
		Draw();
		Render::MatrixMove(0, -2 * b);
		_place.Render();
		Draw();
		Render::PopMatrix();
		Render::SetBlendMode(BLEND_DEFAULT);
	}
	_place.Render();
}

void ProductPlace::Draw() {
	if (_product) {
		_product->Render();
	}
}

void ProductPlace::Update(float dt) {
	if (_effect > 0.f) {
		_effect -= dt * 4;
	}
}

void ProductPlace::OnMouseDown(const FPoint2D &mousePos) {
}

bool ProductPlace::IsUnderMouse(const FPoint2D &mousePos) {
	return (_place.PixelCheck(mousePos) || (_product && _product->PixelCheck(mousePos)));
}
