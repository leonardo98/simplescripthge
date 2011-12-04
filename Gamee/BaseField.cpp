#include "BaseField.h"
#include "../Core/Core.h"
#include "GameField.h"

BaseField::BaseField() {}

void BaseField::Draw() {
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

void BaseField::InnerDraw() {
	if (_effect > 0.f) {
		if (_capacity == 6) {
			DWORD color2 = 0xFFFFFF | ((0xFF - char(_effect * 0xFF)) << 24);
			_empty.Render();
			_tabShadow.Render();
			_tab.Render();
			Render::SetColor(color2);
			_state[_capacity - 1].Render();
			Render::SetColor(0xFFFFFFFF);
		} else if (1 <= _capacity && _capacity < 6) {
			_empty.Render();
			_cut.Render();
			_state[_capacity - 1].Render();
			DWORD color1 = 0xFFFFFF | (char(_effect * 0xFF) << 24);
			Render::SetColor(color1);
			_state[_capacity].Render();
			Render::SetColor(0xFFFFFFFF);
		} else {
			_empty.Render();
			_tabShadow.Render();
			_tab.Render();
			DWORD color1 = 0xFFFFFF | (char(_effect * 0xFF) << 24);
			Render::SetColor(color1);
			_cut.Render();
			_state[_capacity].Render();
			Render::SetColor(0xFFFFFFFF);
		}
		return;
	}
	if (_capacity == -1 || _capacity == 0) {
		_empty.Render();
		_tabShadow.Render();
		_tab.Render();
	} else {
		assert(1 <= _capacity && _capacity <= 6);
		_empty.Render();
		_cut.Render();
		_state[_capacity - 1].Render();
	}
}

void BaseField::Update(float dt) {
	if (_effect > 0.f) {
		_effect -= dt * 4;
	}
}

void BaseField::OnMouseDown(const FPoint2D &mousePos) {
	if (_effect > 0.f) {
		return;
	}
	if (_capacity == -1 || _capacity == 0) {
		_oldCapacity = _capacity;
		_effect = 1.f;
		_capacity = 6;
	} else {
		_oldCapacity = _capacity;
		_effect = 1.f;
		--_capacity;
	}}

bool BaseField::IsUnderMouse(const FPoint2D &mousePos) {
	return (_empty.PixelCheck(mousePos) || (1 <= _capacity && _capacity <= 6 && _state[_capacity - 1].PixelCheck(mousePos)));
}
