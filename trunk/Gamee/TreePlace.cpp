#include "TreePlace.h"
#include "../Core/Core.h"
#include "GameField.h"
#include "ProductManager.h"

TreePlace::TreePlace(int placeType) {

	_animationCounter = 0.f;
	_shakeTimeCounter = 0.f;
	_treeUnderCursor = false;
	SetWaiting(false);

	if (placeType == 1) {
		_bedDown.Set(Core::getTexture("bed_down_1"), 78, 184);
		_bedUp.Set(Core::getTexture("bed_up_1"), 87, 186);
		_treePos.x = 132; _treePos.y = 221;
		annaPos.x = 78; annaPos.y = 217;
		bobPos.x = 78; bobPos.y = 217;
		grandpaPos.x = 78; grandpaPos.y = 217;
	} else if (placeType == 2) {
		_bedDown.Set(Core::getTexture("bed_down_2"), 199, 200);
		_bedUp.Set(Core::getTexture("bed_up_2"), 207, 204);
		_treePos.x = 256; _treePos.y = 244;
		annaPos.x = 195; annaPos.y = 238;
		bobPos.x = 195; bobPos.y = 238;
		grandpaPos.x = 195; grandpaPos.y = 238;
	}
	_pos = _treePos;

	_level = -1;

	_riseUp.Clear();
	_riseUp.addKey(1.0f);
	_riseUp.addKey(1.05f);
	_riseUp.addKey(0.80f);
	_riseUp.CalculateGradient();
	_riseUp2.Clear();
	_riseUp2.addKey(1.0f);
	_riseUp2.addKey(1.05f);
	_riseUp2.addKey(0.85f);
	_riseUp2.CalculateGradient();
	_riseUp3.Clear();
	_riseUp3.addKey(1.25f);
	_riseUp3.addKey(1.3f);
	_riseUp3.addKey(1.0f);
	_riseUp3.CalculateGradient();

	_riseEffect = 0.f;

	_popupMenu.SetPos(_treePos.x, _treePos.y);
	_popupMenu.SetIcons("avocado", "banana", "peach");
	
	_state = tps_no_plant;
	WATER_TIME = 0.33f;
	annaPos = PersPaths::SearchNearest(annaPos);
	bobPos = PersPaths::SearchNearest(bobPos);
	grandpaPos = PersPaths::SearchNearest(grandpaPos);
    _fadeCounter = 0.f;
    _waitingCounter = 0.f;
}

void TreePlace::DrawBottom() {
	_bedDown.Render();
	
	if (_active && !_popupMenu.IsVisible()) {
		float b = GameField::SELECTION_BORDER;
		Render::SetBlendMode(3);
		Render::PushMatrix();
		Render::MatrixMove(-b, 0);
		_bedUp.Render();
		Render::SetColor(0xFF00FF00);
		Draw();
		Render::SetColor(0xFFFFFFFF);
		Render::MatrixMove(2 * b, 0);
		_bedUp.Render();
		Render::SetColor(0xFF00FF00);
		Draw();
		Render::SetColor(0xFFFFFFFF);
		Render::MatrixMove(-b, b);
		_bedUp.Render();
		Render::SetColor(0xFF00FF00);
		Draw();
		Render::SetColor(0xFFFFFFFF);
		Render::MatrixMove(0, -2 * b);
		_bedUp.Render();
		Render::SetColor(0xFF00FF00);
		Draw();
		Render::SetColor(0xFFFFFFFF);
		Render::PopMatrix();
		Render::SetBlendMode(BLEND_DEFAULT);
	}

	_bedUp.Render();
}

void TreePlace::Draw() {
	_treeUnderCursor = false;
	_popupMenu.Draw();
	Render::PushMatrix();
	Render::MatrixMove(_treePos.x, _treePos.y);
	if (_riseEffect > 0.f) {
		if (_level == 0) {
			DWORD color = 0xFF - char(sin(_riseEffect * M_PI_2) * 0xFF);
			Render::SetAlpha(color);
			Render::MatrixScale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			_levels[_level]->Draw(_animationCounter);
			Render::SetAlpha(0xFF);
		} else if (0 < _level && _level < 4) {
			DWORD color1 = char(sin(_riseEffect * M_PI_2) * 0xFF);
			DWORD color2 = 0xFF - char(sin(_riseEffect * M_PI_2) * 0xFF);
			Render::SetAlpha(color1);
			Render::PushMatrix();
			if (_productType == "avocado") {
				Render::MatrixScale(0.9 + 0.1f * cos(2 * M_PI * _riseEffect), 1.15f - 0.15f * _riseEffect);
			} else {
				Render::MatrixScale(0.9 + 0.1f * cos(2 * M_PI * _riseEffect), _riseUp3.getGlobalFrame(_riseEffect));
			}
			_levels[_level - 1]->Draw(_animationCounter);
			Render::PopMatrix();
			Render::SetAlpha(color2);
			if (_productType == "avocado") {
				Render::MatrixScale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			} else {
				Render::MatrixScale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp2.getGlobalFrame(_riseEffect));
			}
			_levels[_level]->Draw(_animationCounter);
			Render::SetAlpha(0xFF);
		}
		_treeUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| ((_state == tps_waiting_harvest || _state == tps_waiting_water) && _progress.PixelCheck(_lastMousePos))
			|| (_level >= 0 && _level < _levels.size() && _levels[_level]->PixelCheck(_lastMousePos));
		Render::PopMatrix();
		return;
	} else if (_state == tps_plant_fade) {
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

		_treeUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| _levels[_level]->PixelCheck(_lastMousePos);
		Render::PopMatrix();
		return;
	}
	if (_level >= 0) {
		if (_shakeTimeCounter > 0.f) {
			if (_level == 0 || _level == 1) {
				Render::MatrixRotate(_shakeTimeCounter * 0.05f * sin(M_PI * 4 * _shakeTimeCounter));
			} else {
				Render::MatrixRotate(_shakeTimeCounter * 0.025f * sin(M_PI * 4 * _shakeTimeCounter));
			}
		}
		_levels[_level]->Draw(_animationCounter);
		_treeUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| ((_state == tps_waiting_harvest || _state == tps_waiting_water) && _progress.PixelCheck(_lastMousePos))
			|| (_level >= 0 && _level < _levels.size() && _levels[_level]->PixelCheck(_lastMousePos));
	} else {
		_treeUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| ((_state == tps_waiting_harvest || _state == tps_waiting_water) && _progress.PixelCheck(_lastMousePos));
	}
	if ((_state == tps_waiting_harvest || _state == tps_waiting_water)) {
		if (_showWaiingActionIcon > 0.f) {
			DWORD color = 0xFFFFFF | ((0xFF - char(_showWaiingActionIcon * 0xFF)) << 24);
			Render::SetAlpha(color);
			_progress.DrawShow(_riseUp.getGlobalFrame(_showWaiingActionIcon));
			Render::SetAlpha(0xFF);
		} else {
			_progress.Draw(1.f - _waitingCounter);
		}
	}
	Render::PopMatrix();
}

void TreePlace::Update(float dt) {
	if (_shakeTimeCounter > 0.f) {
		_shakeTimeCounter -= dt;
	}
	if (_state == tps_no_plant || _state == tps_waiting_seed) {
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
			_state = tps_no_plant;
			_level = -1;
			SetWaiting(false);
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
		_waitingCounter -= dt / 60.f;
		if (_waitingCounter < 0.f) {
			_state = tps_plant_fade;
			_fadeCounter = 1.f;
		}
		return;
	}
	
	assert(_state != tps_waiting_water && _state != tps_waiting_harvest && _state != tps_plant_fade);

	_timeCounter += dt / 12.f;

	if (_state == tps_plant_growing1 && _timeCounter >= WATER_TIME) {
		_state = tps_waiting_water;
		_showWaiingActionIcon = 1.f;
		_waitingCounter = 1.f;
		_progress.SetIcon("water");
		_progress.Move(0, - 100.f);
	} else if (_state == tps_plant_growing2 && _timeCounter >= 1.f) {
		_state = tps_waiting_harvest;
		_showWaiingActionIcon = 1.f;
		_waitingCounter = 1.f;
		_progress.SetIcon("hand");
		_progress.Move(0, - 100.f);
	}

	if (_state != tps_waiting_harvest && _timeCounter > 1.f / _levels.size() * (_level + 1)) {
		++_level;
		_riseEffect = 1.f;
		assert(_level < _levels.size());
	}
}

void TreePlace::OnMouseMove(const FPoint2D &mousePos) {
	_popupMenu.OnMouseMove(mousePos);
	_lastMousePos = mousePos;
}

void TreePlace::OnMouseDown(const FPoint2D &mousePos) {
	if (GetWaiting()) {
		return;
	}
	if (_popupMenu.IsUnderMouse(mousePos)) {
		_productType = _popupMenu.GetActive(mousePos);
		_state = tps_waiting_seed;
		GrandpaPers::NewAction("seed", this);
		GameField::HideAllPopupMenu();
		SetWaiting(true);
		return;
	}
	if (_state == tps_no_plant) {
		assert(_level == -1);
		GameField::HideAllPopupMenu();
		_popupMenu.Show();
		return;
	} else if (_state == tps_waiting_water) {
		GrandpaPers::NewAction("water", GameField::GetWell());
		GrandpaPers::NewAction("", this);
		SetWaiting(true);
	} else if (_state == tps_waiting_harvest) {
		GrandpaPers::NewAction("", this);
		SetWaiting(true);
	}
	_shakeTimeCounter = 1.f;
}

bool TreePlace::IsUnderMouse(const FPoint2D &mousePos) {
	return _treeUnderCursor	|| _popupMenu.IsUnderMouse(mousePos);
}

void TreePlace::Set(const std::string &treeType) {
	_levels.clear();
	_levels.resize(4);
	_levels[0] = Core::getAnimation(treeType + "_level1");
	_levels[1] = Core::getAnimation(treeType + "_level2");
	_levels[2] = Core::getAnimation(treeType + "_level3");
	_levels[3] = Core::getAnimation(treeType + "_level4");
	//for (int i = 0; i < 4; ++i) {
	//	_levels[i]->SetPos(_treePos, false);
	//}
	_level = 0;
	_timeCounter = 0.f;
	_riseEffect = 1.f;
	_showWaiingActionIcon = 0.f;
}

void TreePlace::SomeAction() {
	SetWaiting(false);
	if (_state == tps_waiting_seed) {
		Set(_productType);
		_state = tps_plant_growing1;
		_timeCounter = 0.f;
	} else if (_state == tps_waiting_water) {
		_state = tps_plant_growing2;
		_waitingCounter = 0.f;
	} else if (_state == tps_waiting_harvest) {
		_state = tps_no_plant;
		_waitingCounter = 0.f;
		_level = -1;
	}
}

TreePlaceState TreePlace::WhatToDo(std::string &product) {
	if (_state == tps_waiting_harvest) {
		product = _productType;
	} else {
		product = "";
	}
	return _state;
}