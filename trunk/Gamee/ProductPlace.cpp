#include <algorithm>
#include "ProductPlace.h"
#include "../Core/Core.h"
#include "GameField.h"
#include "ProductManager.h"

ProductPlace::ProductPlace(PlaceType pt) 
: _effect(0.f)
{
	Matrix m;
	m.Scale(-1.f, 1.f);
	m.Move(1024, 0);
	if (pt == pt_water) {
		_shadow.Set(Core::getTexture("textures\\env_well01\\well1_waterplace_sh.png"));
		_place.Set(Core::getTexture("textures\\env_well01\\well1_waterplace.png"));
		_pos.x = 512; _pos.y = 478;
		annaPos.x = 452; annaPos.y = 486;
		bobPos.x = 452; bobPos.y = 486;
		grandpaPos.x = 576; grandpaPos.y = 486;
	} else if (pt == pt_tree) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_02_f05.png"));
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_02.png"));
		_pos.x = 369; _pos.y = 310;
		annaPos.x = 320; annaPos.y = 344;
		bobPos.x = 420; bobPos.y = 344;
		grandpaPos.x = 371; grandpaPos.y = 276;
	} else if (pt == pt_grain) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_03_f04.png"));
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_03.png"));
		_product = NULL;
		_pos.x = 455; _pos.y = 267;
		annaPos.x = 512; annaPos.y = 295;
		bobPos.x = 512; bobPos.y = 295;
		grandpaPos.x = 512; grandpaPos.y = 295;
	} else if (pt == pt_clover) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_03_f04.png"));
		_shadow.PushTransform(m);
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_04.png"));
		_pos.x = 569; _pos.y = 267;
		annaPos.x = 512; annaPos.y = 295;
		bobPos.x = 512; bobPos.y = 295;
		grandpaPos.x = 512; grandpaPos.y = 295;
	} else if (pt == pt_unknown) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\place_sh_02_f05.png"));
		_shadow.PushTransform(m);
		_place.Set(Core::getTexture("textures\\env_jungle\\place_st_05.png"));
		_pos.x = 655; _pos.y = 310;
		annaPos.x = 705; annaPos.y = 344;
		bobPos.x = 603; bobPos.y = 344;
		grandpaPos.x = 646; grandpaPos.y = 276;
	} else if (pt == pt_free1) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\storage_sh.png"));
		_place.Set(Core::getTexture("textures\\env_jungle\\storage_strawx1.png"));
		_pos.x = 757; _pos.y = 343;
		annaPos.x = 705; annaPos.y = 344;
		bobPos.x = 705; bobPos.y = 344;
		grandpaPos.x = 705; grandpaPos.y = 344;
	} else if (pt == pt_free2) {
		_shadow.Set(Core::getTexture("textures\\env_jungle\\storage_sh.png"), 13, 61);
		_place.Set(Core::getTexture("textures\\env_jungle\\storage_strawx2.png"));
		_pos.x = 769; _pos.y = 403;
		annaPos.x = 712; annaPos.y = 402;
		bobPos.x = 712; bobPos.y = 402;
		grandpaPos.x = 712; grandpaPos.y = 402;
	} else if (pt == pt_egg) {
		_pos.x = 900; _pos.y = 595;
		annaPos.x = 870; annaPos.y = 595;
		bobPos.x = 870; bobPos.y = 595;
		grandpaPos.x = 870; grandpaPos.y = 595;
	}
	_product = NULL;
	_placeType = pt;
	ProductManager::Register(_placeType, this);
	annaPos = PersPaths::SearchNearest(annaPos);
	bobPos = PersPaths::SearchNearest(bobPos);
	grandpaPos = PersPaths::SearchNearest(grandpaPos);
}

void ProductPlace::DrawBottom() {
	if (_placeType == pt_egg) {
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
	} else {
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
}

void ProductPlace::Draw() {
	if (_product) {
		if (_effect > 0.f) {
			Render::PushMatrix();
			float fx = 20.f * _effect * _effect;
			float fy = 20.f * _effect * fabs(sin(3 * M_PI_2 * _effect));
			if (_mirrorEffect) {
				fx = -fx;
			}
			float f = _effect * 2 - 1.f;
			Render::SetAlpha(0xFF - 0xFF * max(0.f, f));
			Render::MatrixMove(_pos.x - fx, _pos.y - fy);
			Render::MatrixRotate(_effect * 0.3f * sin(M_PI * 2 * _effect));
			Render::MatrixMove(-_pos.x, -_pos.y);
			_product->Render();
			Render::SetAlpha(0xFF);
			Render::PopMatrix();
		} else {
			_product->Render();
		}
	}
}

void ProductPlace::Update(float dt) {
	if (_effect > 0.f) {
		_effect -= dt * 3;
	}
}

void ProductPlace::OnMouseDown(const FPoint2D &mousePos) {
	if (_placeType == pt_water || _placeType == pt_grain || _placeType == pt_clover) {
		BobPers::NewAction("", this);
	} else {
		AnnaPers::NewAction("", this);
	}
}

std::string ProductPlace::GetProduct() {
	if (_product) {
		delete _product;
		_product = NULL;
		return _productType;
	}
	return "";
}

bool ProductPlace::IsUnderMouse(const FPoint2D &mousePos) {
	return (_place.PixelCheck(mousePos) || (_product && _product->PixelCheck(mousePos)));
}

bool ProductPlace::IsBusy() {
	return (_product != NULL);
}

void ProductPlace::SetProduct(const std::string &product, bool mirrorEffect) {
	Texture *texture = Core::getTexture("gui_" + product);
	_product = new StaticSprite();
	_product->Set(texture, _pos.x - texture->Width() / 2, _pos.y - 2 * texture->Height() / 3);
	_productType = product;
	if (_placeType == pt_egg) {
		_effect = 0.f;
	} else {
		_mirrorEffect = mirrorEffect;
		_effect = 1.f;
	}
}

std::string ProductPlace::WhatToDo() {
	if (_product == NULL && (_placeType == pt_free1 || _placeType == pt_free2)) {
		return "put";
	} else {
		return "take";
	}
}

