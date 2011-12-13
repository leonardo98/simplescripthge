#include "BaseField.h"
#include "../Core/Core.h"
#include "GameField.h"
#include "ProductManager.h"

BaseField::BaseField() {
	_capacity = -1;
	_effect = 0.f;
	_persActionWaiting = 0.f;
}

void BaseField::DrawBottom() {
	if (_active) {
		float b = GameField::SELECTION_BORDER;
		Render::SetBlendMode(3);
		Render::PushMatrix();
		Render::MatrixMove(-b, 0);
		InnerDraw();
		if (_capacity == -1) {
			_tab.Render();
		}
		Render::MatrixMove(2 * b, 0);
		InnerDraw();
		if (_capacity == -1) {
			_tab.Render();
		}
		Render::MatrixMove(-b, b);
		InnerDraw();
		if (_capacity == -1) {
			_tab.Render();
		}
		Render::MatrixMove(0, -2 * b);
		InnerDraw();
		if (_capacity == -1) {
			_tab.Render();
		}
		Render::PopMatrix();
		Render::SetBlendMode(BLEND_DEFAULT);
	}
	InnerDraw();
}

void BaseField::Draw() {
	if (_effect > 0.f) {
		if (_capacity == -1) {
			DWORD alpha = 0xFF - char(_effect * 0xFF);
			Render::SetAlpha(alpha);
			_tabShadow.Render();
			_tab.Render();
			Render::SetAlpha(0xFF);
		} else if (_capacity == 6) {
			DWORD alpha = char(_effect * 0xFF);
			Render::SetAlpha(alpha);
			_tabShadow.Render();
			_tab.Render();
			Render::SetAlpha(0xFF);
		}
		return;
	}
	if (_capacity == -1) {
		_tabShadow.Render();
		_tab.Render();
	}
}

void BaseField::InnerDraw() {
	if (_effect > 0.f) {
		_empty.Render();
		if (_capacity == 6) {
			DWORD alpha = 0xFF - char(_effect * 0xFF);
			Render::SetAlpha(alpha);
			_state[_capacity - 1].Render();
			Render::SetAlpha(0xFF);
		} else if (0 <= _capacity && _capacity < 6) {
			_cut.Render();
			if (_capacity > 0) {
				_state[_capacity - 1].Render();
			}
			DWORD alpha = char(_effect * 0xFF);
			Render::SetAlpha(alpha);
			_state[_capacity].Render();
			Render::SetAlpha(0xFF);
		} else if (-1 == _capacity) {
			DWORD alpha = char(_effect * 0xFF);
			Render::SetAlpha(alpha);
			_cut.Render();
			Render::SetAlpha(0xFF);
		}
		return;
	}
	_empty.Render();
	if (_capacity == 0) {
		_cut.Render();
	} else if (_capacity > 0) {
		assert(_capacity <= 6);
		_cut.Render();
		_state[_capacity - 1].Render();
	}
}

void BaseField::Update(float dt) {
	if (_persActionWaiting > 0.f) {
		_persActionWaiting -= dt;
		return;
	}
	if (_effect > 0.f) {
		if (_capacity == -1) {
			_effect -= dt * 2;
		} else {
			_effect -= dt * 4;
		}
		if (_effect <= 0.f && _capacity != 6 && _capacity != -1) {
			ProductManager::PlaceProduct(_placeType, _productName, _pos.x);
		}
		return;
	}
	if (_capacity == 0) {
		_capacity = -1;
		_effect = 1.f;
	}
}

void BaseField::OnMouseDown(const FPoint2D &mousePos) {
	if (_effect > 0.f || _persActionWaiting > 0.f) {
		return;
	}
	if (_capacity == -1) {
		SomeAction();
	} else {
		GrandpaPers::NewAction("", this);
	}
}

bool BaseField::IsUnderMouse(const FPoint2D &mousePos) {
	return (_empty.PixelCheck(mousePos) || (1 <= _capacity && _capacity <= 6 && _state[_capacity - 1].PixelCheck(mousePos)));
}

PlantPlaceState BaseField::WhatToDo(std::string &product) {
	if (_capacity != -1) {
		product = _productName;
	}
	if (_capacity == -1 || _capacity == 0) {
		return no_plant;
	} 
	return harvesting;
}

void BaseField::SomeAction(float persActionWaiting) {
	_persActionWaiting = persActionWaiting;
	if (_capacity == -1 || _capacity == 0) {
		_oldCapacity = _capacity;
		_effect = 1.f;
		_capacity = 6;
	} else {
		_oldCapacity = _capacity;
		_effect = 1.f;
		--_capacity;
	}
}

FPoint2D BaseField::GetGrandpaPos() {
	if (_capacity == -1 || _capacity == 0) {
		return grandpaPos[3];
	}
	return grandpaPos[_capacity - 1];
}
