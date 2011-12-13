#include "PlantPlace.h"
#include "GameField.h"
#include "../Core/Core.h"

PlantPlace::PlantPlace(int placeType) {
	Matrix transform;
	transform.Move(1024, 0);
	transform.Scale(-1.f, 1.f);

	_plantUnderCursor = false; 
	_persActionWaiting = 0.f;

	if (placeType == 1) {
		_bedDown.Set(Core::getTexture("bed_down_1"), 399, 521);
		_bedUp.Set(Core::getTexture("bed_up_1"), 408, 523);
		_plantPos.x = 454; _plantPos.y = 556;
		annaPos.x = 394; annaPos.y = 557;
		bobPos.x = 455; bobPos.y = 522;
		grandpaPos.x = 404; grandpaPos.y = 553;
	} else if (placeType == 2) {
		_bedDown.Set(Core::getTexture("bed_down_1"), 399, 521);
		_bedUp.Set(Core::getTexture("bed_up_1"), 408, 523);
		_bedDown.PushTransform(transform);
		_bedUp.PushTransform(transform);
		_plantPos.x = 1024 - 454; _plantPos.y = 556;
		annaPos.x = 625; annaPos.y = 557;
		bobPos.x = 567; bobPos.y = 522;
		grandpaPos.x = 615; grandpaPos.y = 553;
	} else if (placeType == 3) {
		_bedDown.Set(Core::getTexture("bed_down_2"), 381, 592);
		_bedUp.Set(Core::getTexture("bed_up_2"), 389, 596);
		_plantPos.x = 441; _plantPos.y = 632;
		annaPos.x = 377; annaPos.y = 633;
		bobPos.x = 512; bobPos.y = 672;
		grandpaPos.x = 387; grandpaPos.y = 631;
	} else if (placeType == 4) {
		_bedDown.Set(Core::getTexture("bed_down_2"), 381, 592);
		_bedUp.Set(Core::getTexture("bed_up_2"), 389, 596);
		_bedDown.PushTransform(transform);
		_bedUp.PushTransform(transform);
		_plantPos.x = 1024 - 441; _plantPos.y = 632;
		annaPos.x = 627; annaPos.y = 633;
		bobPos.x = 643; bobPos.y = 672;
		grandpaPos.x = 617; grandpaPos.y = 631;
	} else {
		assert(false);
	}
	_pos = _plantPos;
	bobPos = annaPos;

	_level = -1;

	_riseUp.Clear();
	_riseUp.addKey(1.0f);
	_riseUp.addKey(1.025f);
	_riseUp.addKey(0.85f);
	_riseUp.CalculateGradient();

	_popupMenu.SetPos(_plantPos.x, _plantPos.y);
	_popupMenu.SetIcons("onion", "beet", "pumpkin");

	_riseEffect = 0.f;
	_waitingCounter = 0.f;
	_animationCounter = 0.f;

	annaPos = PersPaths::SearchNearest(annaPos);
	bobPos = PersPaths::SearchNearest(bobPos);
	grandpaPos = PersPaths::SearchNearest(grandpaPos);

	_state = no_plant;
	WATER_TIME = 0.33f;
	CHOP_TIME = 0.66f;
}

void PlantPlace::DrawBottom() {
	_bedDown.Render();
	if (_active && !_popupMenu.IsVisible()) {
		float b = GameField::SELECTION_BORDER;

		Render::SetBlendMode(1);
		Render::PushMatrix();
		Render::MatrixMove(-b, 0);
		_bedUp.Render();
		Draw();
		Render::MatrixMove(2 * b, 0);
		_bedUp.Render();
		Draw();
		Render::MatrixMove(-b, b);
		_bedUp.Render();
		Draw();
		Render::MatrixMove(0, -2 * b);
		_bedUp.Render();
		Draw();
		Render::PopMatrix();
		Render::SetBlendMode(BLEND_DEFAULT);
	}

	_bedUp.Render();
}

void PlantPlace::Draw() {
	_popupMenu.Draw();

	Render::PushMatrix();
	Render::MatrixMove(_plantPos.x, _plantPos.y);
	if (_riseEffect > 0.f) {
		if (_level == 0) {
			DWORD color = 0xFFFFFF | ((0xFF - char(_riseEffect * 0xFF)) << 24);
			Render::SetColor(color);
			Render::MatrixScale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			_levels[_level]->Draw(_animationCounter);
			Render::SetColor(0xFFFFFFFF);
		} else if (0 < _level && _level < 4) {
			DWORD color1 = 0xFFFFFF | (char(_riseEffect * 0xFF) << 24);
			DWORD color2 = 0xFFFFFF | ((0xFF - char(_riseEffect * 0xFF)) << 24);
			Render::SetColor(color1);
			Render::PushMatrix();
			Render::MatrixScale(1.25f - 0.25f * _riseEffect, 1.25f - 0.25f * _riseEffect);
			_levels[_level - 1]->Draw(_animationCounter);
			Render::PopMatrix();
			Render::SetColor(color2);
			Render::MatrixScale(_riseUp.getGlobalFrame(_riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			_levels[_level]->Draw(_animationCounter);
			Render::SetColor(0xFFFFFFFF);
		}
		_plantUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| ((_state == waiting_harvest || _state == waiting_water || _state == waiting_chop) && _progress.PixelCheck(_lastMousePos))
			|| (_level >= 0 && _level < _levels.size() && _levels[_level]->PixelCheck(_lastMousePos));
		Render::PopMatrix();
		return;
	} else if (_state == plant_fade) {
		assert(_fadeCounter >= 0.f && _level >= 0); 

		DWORD r = 0x60 + 0x9F * _fadeCounter;
		DWORD g = 0x30 + 0xCF * _fadeCounter;
		DWORD b = 0x00 + 0xFF * _fadeCounter;
		DWORD color = r << 16 | g << 8 | b;
		DWORD color1;
		float f;
		if (_fadeCounter > 0.3f) {
			f = 0.7f + 0.3f * _fadeCounter;
			color1 = color | 0xFF000000;
		} else {
			f = 0.7f + 0.3f * 0.3f;
			color1 = color | (char(_fadeCounter / 0.3f * 0xFF) << 24);
		}
		Render::SetColor(color1);
		Render::MatrixScale(f, f);
		_levels[_level]->Draw(_animationCounter);
		Render::SetColor(0xFFFFFFFF);

		_plantUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| _levels[_level]->PixelCheck(_lastMousePos);
		Render::PopMatrix();
		return;
	}
	if (_level >= 0) {
		_levels[_level]->SetPos(_plantPos, false);
		_levels[_level]->Draw(_animationCounter);
	}
	_plantUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
		|| ((_state == waiting_harvest || _state == waiting_water || _state == waiting_chop) && _progress.PixelCheck(_lastMousePos))
		|| (_level >= 0 && _level < _levels.size() && _levels[_level]->PixelCheck(_lastMousePos));

	if (_state == waiting_harvest || _state == waiting_water || _state == waiting_chop) {
		if (_showWaiingActionIcon > 0.f) {
			DWORD color = 0xFFFFFF | ((0xFF - char(_showWaiingActionIcon * 0xFF)) << 24);
			Render::SetColor(color);
			_progress.DrawShow(_riseUp.getGlobalFrame(_showWaiingActionIcon));
			Render::SetColor(0xFFFFFFFF);
		} else {
			_progress.Draw(1.f - _waitingCounter);
		}
	}
	Render::PopMatrix();
}

void PlantPlace::Update(float dt) {
	_popupMenu.Update(dt);
	if (_state == no_plant || _state == waiting_seed) {
		return;
	}
	assert(_level >= 0);
	if (_level >= 0 && _animationCounter < 1.f) {
		_animationCounter += (dt / _levels[_level]->Time());
		while (_animationCounter >= 1.f) {
			_animationCounter -= 1.f;
		}
	}
	
	if (_fadeCounter > 0.f) {
		_fadeCounter -= dt / 5.f;
		if (_fadeCounter < 0.f) {
			_state = no_plant;
			_level = -1;
		}
		return;
	}

	if (_persActionWaiting > 0.f) {
		_persActionWaiting -= dt;
		if (_persActionWaiting < 0.f && _state == harvesting) {
			_state = no_plant;
			_waitingCounter = 0.f;
			_level = -1;
		}
		return;
	}
	
	if (_riseEffect > 0.f) {
		_riseEffect -= dt * 2;
	}
	
	if (_showWaiingActionIcon > 0.f) {
		_showWaiingActionIcon -= dt * 3.333f;
	}

	if (_waitingCounter > 0.f) {
		_waitingCounter -= dt / 20.f;
		if (_waitingCounter < 0.f) {
			_state = plant_fade;
			_fadeCounter = 1.f;
		}
		return;
	}

	assert(_state != waiting_water && _state != waiting_chop && _state != waiting_harvest && _state != plant_fade);

	_timeCounter += dt / 20.f;

	if (_state == plant_growing1 && _timeCounter >= WATER_TIME) {
		_state = waiting_water;
		_showWaiingActionIcon = 1.f;
		_waitingCounter = 1.f;
		_progress.SetIcon("water");
		_progress.Move(0, - 30.f);
	} else if (_state == plant_growing2 && _timeCounter >= CHOP_TIME) {
		_state = waiting_chop;
		_showWaiingActionIcon = 1.f;
		_waitingCounter = 1.f;
		_progress.SetIcon("hoe");
		_progress.Move(0, - 30.f);
	} else if (_state == plant_growing3 && _timeCounter >= 1.f) {
		_state = waiting_harvest;
		_showWaiingActionIcon = 1.f;
		_waitingCounter = 1.f;
		_progress.SetIcon("hand");
		_progress.Move(0, - 30.f);
	}

	if (_state != waiting_harvest && _timeCounter > 1.f / _levels.size() * (_level + 1)) {
		++_level;
		_riseEffect = 1.f;
		assert(_level < _levels.size());
	}
}

void PlantPlace::OnMouseMove(const FPoint2D &mousePos) {
	_popupMenu.OnMouseMove(mousePos);
	_lastMousePos = mousePos;
}

void PlantPlace::OnMouseDown(const FPoint2D &mousePos) {
	if (_popupMenu.IsUnderMouse(mousePos)) {
		_plantType = _popupMenu.GetActive(mousePos);
		_state = waiting_seed;
		AnnaPers::NewAction("seed", this);
		GameField::HideAllPopupMenu();
		return;
	}
	if (_state == no_plant) {
		assert(_level == -1);
		GameField::HideAllPopupMenu();
		_popupMenu.Show();
		return;
	}
	if (_state == waiting_water || _state == waiting_chop) {
		BobPers::NewAction("", this);
	} else {
		AnnaPers::NewAction("", this);
	}
}

bool PlantPlace::IsUnderMouse(const FPoint2D &mousePos) {
	return _plantUnderCursor || _popupMenu.IsUnderMouse(mousePos);
}

void PlantPlace::Set(const std::string &plantType) {
	assert(_state == waiting_seed);
	_levels.clear();
	_levels.resize(4);
	_levels[0] = Core::getAnimation(plantType + "_level1");
	_levels[1] = Core::getAnimation(plantType + "_level2");
	_levels[2] = Core::getAnimation(plantType + "_level3");
	_levels[3] = Core::getAnimation(plantType + "_level4");
/*
	for (int i = 0; i < 4; ++i) {
		_levels[i]->SetPos(_plantPos, false);
	}
*/
	_level = 0;
	_riseEffect = 1.f;
	_showWaiingActionIcon = 0.f;
	_fadeCounter = 0.f;
}

void PlantPlace::SomeAction(float persActionWaiting) {
	_persActionWaiting = persActionWaiting;
	if (_state == waiting_seed) {
		Set(_plantType);
		_state = plant_growing1;
		_timeCounter = 0.f;
	} else if (_state == waiting_water) {
		_state = plant_growing2;
		_waitingCounter = 0.f;
	} else if (_state == waiting_chop) {
		_state = plant_growing3;
		_waitingCounter = 0.f;
	} else if (_state == waiting_harvest) {
		_state = harvesting;
		_waitingCounter = 0.f;
	}
}

PlantPlaceState PlantPlace::WhatToDo(std::string &product) {
	if (_state == waiting_harvest) {
		product = _plantType;
	} else {
		product = "";
	}
	return _state;
}