#include "TreePlace.h"
#include "../Core/Core.h"
#include "GameField.h"

TreePlace::TreePlace(int placeType) {

	_shakeTimeCounter = 0.f;
	_treeUnderCursor = false;

	if (placeType == 1) {
		_bedDown.Set(Core::getTexture("bed_down_1"), 78, 184);
		_bedUp.Set(Core::getTexture("bed_up_1"), 87, 186);
		_treePos.x = 132; _treePos.y = 221;
	} else if (placeType == 2) {
		_bedDown.Set(Core::getTexture("bed_down_2"), 199, 200);
		_bedUp.Set(Core::getTexture("bed_up_2"), 207, 204);
		_treePos.x = 256; _treePos.y = 244;
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

	_progress.Move(_treePos.x, _treePos.y - 100.f);

	_riseEffect = 0.f;
	_showProgress = 0.f;

	_popupMenu.SetPos(_treePos.x, _treePos.y);
	_popupMenu.SetIcons("avocado", "banana", "peach");
	
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

	if (_riseEffect > 0.f) {
		if (_level == 0) {
			DWORD color = 0xFF - char(sin(_riseEffect * M_PI_2) * 0xFF);
			Render::SetAlpha(color);
			Matrix t;
			t.Scale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			t.Move(_treePos.x, _treePos.y);
			_state[_level]->SetPos(t);
			_state[_level]->Draw(_animationCounter / _state[_level]->Time());
			Render::SetAlpha(0xFF);
		} else if (0 < _level && _level < 4) {
			DWORD color1 = char(sin(_riseEffect * M_PI_2) * 0xFF);
			DWORD color2 = 0xFF - char(sin(_riseEffect * M_PI_2) * 0xFF);
			Render::SetAlpha(color1);
			Matrix t;
			if (_debug_type == "avocado") {
				t.Scale(0.9 + 0.1f * cos(2 * M_PI * _riseEffect), 1.15f - 0.15f * _riseEffect);
			} else {
				t.Scale(0.9 + 0.1f * cos(2 * M_PI * _riseEffect), _riseUp3.getGlobalFrame(_riseEffect));
			}
			t.Move(_treePos.x, _treePos.y);
			_state[_level - 1]->SetPos(t);
			_state[_level - 1]->Draw(_animationCounter / _state[_level]->Time());
			Render::SetAlpha(color2);
			t.Unit();
			if (_debug_type == "avocado") {
				t.Scale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp.getGlobalFrame(_riseEffect));
			} else {
				t.Scale(1 - 0.2f * sin(M_PI * _riseEffect), _riseUp2.getGlobalFrame(_riseEffect));
			}
			t.Move(_treePos.x, _treePos.y);
			_state[_level]->SetPos(t);
			_state[_level]->Draw(_animationCounter / _state[_level]->Time());
			Render::SetAlpha(0xFF);
		}
		_treeUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| (((_waitWater && _level == 1)|| _level == 3) && _progress.PixelCheck(_lastMousePos))
			|| (_level >= 0 && _level < _state.size() && _state[_level]->PixelCheck(_lastMousePos));
		return;
	}
	if (_level >= 0) {
		if (_shakeTimeCounter > 0.f) {
			Matrix m;
			if (_level == 0 || _level == 1) {
				m.Rotate(_shakeTimeCounter * 0.05f * sin(M_PI * 4 * _shakeTimeCounter));
			} else {
				m.Rotate(_shakeTimeCounter * 0.025f * sin(M_PI * 4 * _shakeTimeCounter));
			}
			m.Move(_treePos.x, _treePos.y);
			_state[_level]->SetPos(m);
		} else {
			_state[_level]->SetPos(_treePos, false);
		}
		_state[_level]->Draw(_animationCounter / _state[_level]->Time());
		_treeUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| (((_waitWater && _level == 1)|| _level == 3) && _progress.PixelCheck(_lastMousePos))
			|| (_level >= 0 && _level < _state.size() && _state[_level]->PixelCheck(_lastMousePos));
	} else {
		_treeUnderCursor = _bedUp.PixelCheck(_lastMousePos) 
			|| (((_waitWater && _level == 1)|| _level == 3) && _progress.PixelCheck(_lastMousePos));
	}
	if ((_waitWater && _level == 1)|| _level == 3) {
		if (_showProgress > 0.f) {
			if (_showProgress < 0.5f) {
				DWORD color = 0xFF - char(_showProgress / 0.5f * 0xFF);
				Render::SetAlpha(color);
				_progress.Draw(1.f - _timeCounter);
				Render::SetAlpha(0xFF);
			}
		} else {
			_progress.Draw(1.f - _timeCounter);
		}
	}
}

void TreePlace::Update(float dt) {
	if (_shakeTimeCounter > 0.f) {
		_shakeTimeCounter -= dt;
	}
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

void TreePlace::OnMouseMove(const FPoint2D &mousePos) {
	_popupMenu.OnMouseMove(mousePos);
	_lastMousePos = mousePos;
}

void TreePlace::OnMouseDown(const FPoint2D &mousePos) {
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
	} else {
		_shakeTimeCounter = 1.f;
	}
}

bool TreePlace::IsUnderMouse(const FPoint2D &mousePos) {
	return _treeUnderCursor	|| _popupMenu.IsUnderMouse(mousePos);
}

void TreePlace::Set(const std::string &treeType) {
	_debug_type = treeType;
	_state.clear();
	_state.resize(4);
	_state[0] = Core::getAnimation(treeType + "_level1");
	_state[1] = Core::getAnimation(treeType + "_level2");
	_state[2] = Core::getAnimation(treeType + "_level3");
	_state[3] = Core::getAnimation(treeType + "_level4");
	for (int i = 0; i < 4; ++i) {
		_state[i]->SetPos(_treePos, false);
	}
	_level = 0;
	_timeCounter = 1.f;
	_riseEffect = 1.f;
	_showProgress = 0.f;
	_waitWater = false;
}
