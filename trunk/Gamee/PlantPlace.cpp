#include "PlantPlace.h"
#include "GameField.h"
#include "../Core/Core.h"

PlantPlace::PlantPlace(int placeType) {
	Matrix transform;
	transform.Scale(-1.f, 1.f);
	transform.Move(1024, 0);

	_plantUnderCursor = false;

	if (placeType == 1) {
		_bedDown.Set(Core::getTexture("bed_down_1"), 399, 521);
		_bedUp.Set(Core::getTexture("bed_up_1"), 408, 523);
		_plantPos.x = 454; _plantPos.y = 556;
	} else if (placeType == 2) {
		_bedDown.Set(Core::getTexture("bed_down_1"), 399, 521);
		_bedUp.Set(Core::getTexture("bed_up_1"), 408, 523);
		_bedDown.PushTransform(transform);
		_bedUp.PushTransform(transform);
		_plantPos.x = 1024 - 454; _plantPos.y = 556;
	} else if (placeType == 3) {
		_bedDown.Set(Core::getTexture("bed_down_2"), 381, 592);
		_bedUp.Set(Core::getTexture("bed_up_2"), 389, 596);
		_plantPos.x = 441; _plantPos.y = 632;
	} else if (placeType == 4) {
		_bedDown.Set(Core::getTexture("bed_down_2"), 381, 592);
		_bedUp.Set(Core::getTexture("bed_up_2"), 389, 596);
		_bedDown.PushTransform(transform);
		_bedUp.PushTransform(transform);
		_plantPos.x = 1024 - 441; _plantPos.y = 632;
	} else {
		assert(false);
	}
	_pos = _plantPos;

	_level = -1;

	_riseUp.Clear();
	_riseUp.addKey(1.0f);
	_riseUp.addKey(1.025f);
	_riseUp.addKey(0.85f);
	_riseUp.CalculateGradient();

	_progress.Move(_plantPos.x, _plantPos.y - 30.f);
	_popupMenu.SetPos(_plantPos.x, _plantPos.y);
	_popupMenu.SetIcons("onion", "beet", "pumpkin");

	_riseEffect = 0.f;
	_showProgress = 0.f;

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

	if (_riseEffect > 0.f) {
		if (_level == 0) {
			DWORD color = 0xFFFFFF | ((0xFF - char(_riseEffect * 0xFF)) << 24);
			Render::SetColor(color);
			Matrix t;
			t.Scale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			t.Move(_plantPos.x, _plantPos.y);
			_state[_level]->SetPos(t);
			_state[_level]->Draw(_animationCounter / _state[_level]->Time());
			Render::SetColor(0xFFFFFFFF);
		} else if (0 < _level && _level < 4) {
			DWORD color1 = 0xFFFFFF | (char(_riseEffect * 0xFF) << 24);
			DWORD color2 = 0xFFFFFF | ((0xFF - char(_riseEffect * 0xFF)) << 24);
			Render::SetColor(color1);
			Matrix t;
			t.Scale(1.25f - 0.25f * _riseEffect, 1.25f - 0.25f * _riseEffect);
			t.Move(_plantPos.x, _plantPos.y);
			_state[_level - 1]->SetPos(t);
			_state[_level - 1]->Draw(_animationCounter / _state[_level]->Time());
			Render::SetColor(color2);
			t.Unit();
			t.Scale(_riseUp.getGlobalFrame(_riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			t.Move(_plantPos.x, _plantPos.y);
			_state[_level]->SetPos(t);
			_state[_level]->Draw(_animationCounter / _state[_level]->Time());
			Render::SetColor(0xFFFFFFFF);
		}
		_plantUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| (((_waitWater && _level == 1)|| _level == 3) && _progress.PixelCheck(_lastMousePos))
			|| (_level >= 0 && _level < _state.size() && _state[_level]->PixelCheck(_lastMousePos));
		return;
	}
	if (_level >= 0) {
		_state[_level]->SetPos(_plantPos, false);
		_state[_level]->Draw(_animationCounter / _state[_level]->Time());
		_plantUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| (((_waitWater && _level == 1)|| _level == 3) && _progress.PixelCheck(_lastMousePos))
			|| (_level >= 0 && _level < _state.size() && _state[_level]->PixelCheck(_lastMousePos));
	} else {
		_plantUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| (((_waitWater && _level == 1)|| _level == 3) && _progress.PixelCheck(_lastMousePos));
	}
	if ((_waitWater && _level == 1)|| _level == 3) {
		if (_showProgress > 0.f) {
			DWORD color = 0xFFFFFF | ((0xFF - char(_showProgress * 0xFF)) << 24);
			Render::SetColor(color);
			_progress.Draw(1.f - _timeCounter);
			Render::SetColor(0xFFFFFFFF);
		} else {
			_progress.Draw(1.f - _timeCounter);
		}
	}
}

void PlantPlace::Update(float dt) {
	_popupMenu.Update(dt);
	if (_level == -1) {
		return;
	}
	if (_level >= 0 && _animationCounter < _state[_level]->Time()) {
		_animationCounter += dt;
		while (_animationCounter >= _state[_level]->Time()) {
			_animationCounter -= _state[_level]->Time();
		}
	}
	if (_riseEffect > 0.f) {
		_riseEffect -= dt * 2;
		return;
	}
	if (_showProgress > 0.f) {
		_showProgress -= dt * 2;
		return;
	}
	if (_level == 0) {
		_timeCounter -= dt * 0.2f;
		if (_timeCounter <= 0.f) {
			_level = 1;
			_timeCounter = 1.f;
			_riseEffect = 1.f;
			_waitWater = true;
			_showProgress = 1.f;
		}
	} else if (_waitWater) {
		assert(_level == 1);
		if (_timeCounter > 0.f) {
			_timeCounter -= dt * 0.025f;
			if (_timeCounter <= 0.f) {
				_level = -1;
			}
		}
	} else if (_level == 2) {
		_timeCounter -= dt * 0.2f;
		if (_timeCounter <= 0.f) {
			_level = 3;
			_timeCounter = 1.f;
			_riseEffect = 1.f;
			_product = 1;
			_showProgress = 1.f;
		}
	} else if (_level == 3) {
		_timeCounter -= dt * 0.025f;
		if (_timeCounter <= 0.f) {
			_level = -1;
		}
	}
}

void PlantPlace::OnMouseMove(const FPoint2D &mousePos) {
	_popupMenu.OnMouseMove(mousePos);
	_lastMousePos = mousePos;
}

void PlantPlace::OnMouseDown(const FPoint2D &mousePos) {
	if (_popupMenu.IsUnderMouse(mousePos)) {
		std::string id = _popupMenu.GetActive(mousePos);
		Set(id);
		GameField::HideAllPopupMenu();
		return;
	}
	if (_showProgress > 0.f || _riseEffect > 0.f) {
		return;
	}
	if (_level == -1) {
		GameField::HideAllPopupMenu();
		_popupMenu.Show();
	} else if (_waitWater) {
		_waitWater = false;
		_riseEffect = 1.f;
		_timeCounter = 1.f;
		_level = 2;
	} else if (_level == 3) {
		_level = -1;
	}
}

bool PlantPlace::IsUnderMouse(const FPoint2D &mousePos) {
	return _plantUnderCursor || _popupMenu.IsUnderMouse(mousePos);
}

void PlantPlace::Set(const std::string &plantType) {
	_state.clear();
	_state.resize(4);
	_state[0] = Core::getAnimation(plantType + "_level1");
	_state[1] = Core::getAnimation(plantType + "_level2");
	_state[2] = Core::getAnimation(plantType + "_level3");
	_state[3] = Core::getAnimation(plantType + "_level4");
	for (int i = 0; i < 4; ++i) {
		_state[i]->SetPos(_plantPos, false);
	}
	_level = 0;
	_timeCounter = 1.f;
	_riseEffect = 1.f;
	_showProgress = 0.f;
	_waitWater = false;
}
