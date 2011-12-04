#include "PopupMenu.h"
#include "../Core/Core.h"
#include "GameField.h"

PopupMenu::PopupMenu() {
	_firstTab.Set(Core::getTexture("textures\\popup_menu\\gui_vegetabletab01.png"));
	_secondTab.Set(Core::getTexture("textures\\popup_menu\\gui_vegetabletab02.png"));
	_thirdTab.Set(Core::getTexture("textures\\popup_menu\\gui_vegetabletab03.png"));
	_state = state_invisible;
	_timeCounter = 0.f;

	_riseUp.Clear();
	_riseUp.addKey(0.5f);
	_riseUp.addKey(1.05f);
	_riseUp.addKey(1.0f);
	_riseUp.CalculateGradient();

	_selected = NULL;

	GameField::AddPopupMenu(this);
}

PopupMenu::~PopupMenu() {
	GameField::RemovePopupMenu(this);
}

void PopupMenu::Draw() {
	if (_state == state_invisible) {
		return;
	}

	if (_state == state_visible) {
		if (_selected != NULL) {
			float b = GameField::SELECTION_BORDER;
			Render::SetBlendMode(1);
			Render::PushMatrix();
			Render::MatrixMove(-b, 0);
			_selected->Render();
			Render::MatrixMove(2 * b, 0);
			_selected->Render();
			Render::MatrixMove(-b, b);
			_selected->Render();
			Render::MatrixMove(0, -2 * b);
			_selected->Render();
			Render::PopMatrix();
			Render::SetBlendMode(BLEND_DEFAULT);
		}	
		_firstTab.Render();
		_firstIcon.Render();
		_thirdTab.Render();
		_thirdIcon.Render();
		_secondTab.Render();
		_secondIcon.Render();
	} else if (_state == state_show || _state == state_hide) {
		Matrix m, f, s, t;
		m.Move(_position.x, _position.y);
		f.Move(-29, -75);
		f.Rotate(-_riseUp.getGlobalFrame(_timeCounter));
		f.Mul(m);
		s.Move(-28, -79);
		s.Mul(m);
		t.Move(-27, -75);
		t.Rotate(_riseUp.getGlobalFrame(_timeCounter));
		t.Mul(m);

		_firstTab.SetTransform(f);
		_secondTab.SetTransform(s);
		_thirdTab.SetTransform(t);
		_firstIcon.SetTransform(f);
		_secondIcon.SetTransform(s);
		_thirdIcon.SetTransform(t);

		DWORD color = 0xFFFFFF | (char(_timeCounter * 0xFF) << 24);
		Render::SetColor(color);
		_firstTab.Render();
		_firstIcon.Render();
		_thirdTab.Render();
		_thirdIcon.Render();
		_secondTab.Render();
		_secondIcon.Render();
		Render::SetColor(0xFFFFFFFF);
	}
}

void PopupMenu::SetPos(float x, float y) {
	_position.x = x;
	_position.y = y;
}

void PopupMenu::SetIcons(const std::string &first, const std::string &second, const std::string &third) {
	_selected = NULL;

	_iconsName.clear();
	_firstIcon.Set(Core::getTexture("textures\\popup_menu\\gui_vt_icon_" + first + ".png"));
	_iconsName.push_back(first);
	if (second != "") {
		_secondIcon.Set(Core::getTexture("textures\\popup_menu\\gui_vt_icon_" + second + ".png"));
		_iconsName.push_back(second);
	}
	if (third != "") {
		_thirdIcon.Set(Core::getTexture("textures\\popup_menu\\gui_vt_icon_" + third + ".png"));
		_iconsName.push_back(third);
	}
}

std::string PopupMenu::GetActive(const FPoint2D &mousePos) {
	assert(_state == state_visible);
	if (_firstTab.PixelCheck(mousePos)) {
		return _iconsName[0];
	}
	if (_iconsName.size() >= 2 && _secondTab.PixelCheck(mousePos)) {
		return _iconsName[1];
	}
	if (_iconsName.size() >= 3 && _thirdTab.PixelCheck(mousePos)) {
		return _iconsName[2];
	}
	return "";
}

void PopupMenu::Update(float dt) {
	if (_state == state_show && _timeCounter < 1.f) {
		_timeCounter += dt * 4;
		if (_timeCounter >= 1.f) {
			_state = state_visible;

			Matrix m, f, s, t;
			m.Move(_position.x, _position.y);
			f.Move(-29, -75);
			f.Rotate(-1.f);
			f.Mul(m);
			s.Move(-28, -79);
			s.Mul(m);
			t.Move(-27, -75);
			t.Rotate(1.f);
			t.Mul(m);

			_firstTab.SetTransform(f);
			_secondTab.SetTransform(s);
			_thirdTab.SetTransform(t);
			_firstIcon.SetTransform(f);
			_secondIcon.SetTransform(s);
			_thirdIcon.SetTransform(t);

			_timeCounter = 1.f;
		}
	} else if (_state == state_hide && _timeCounter > 0.f) {
		_timeCounter -= dt * 4;
		if (_timeCounter <= 0.f) {
			_state = state_invisible;
			_timeCounter = 0.f;
		}
	}
}

bool PopupMenu::IsUnderMouse(const FPoint2D &mousePos) {
	return (_state != state_invisible && (
		(_iconsName.size() >= 1 && _firstTab.PixelCheck(mousePos))
		|| (_iconsName.size() >= 2 && _secondTab.PixelCheck(mousePos))
		|| (_iconsName.size() >= 3 && _thirdTab.PixelCheck(mousePos))
		));
}

void PopupMenu::OnMouseDown(const FPoint2D &mousePos) {
}

void PopupMenu::OnMouseMove(const FPoint2D &mousePos) {
	if (_iconsName.size() >= 1 && _firstTab.PixelCheck(mousePos)) {
		_selected = &_firstTab;
	} else if (_iconsName.size() >= 2 && _secondTab.PixelCheck(mousePos)) {
		_selected = &_secondTab;
	} else if (_iconsName.size() >= 3 && _thirdTab.PixelCheck(mousePos)) {
		_selected = &_thirdTab;
	} else {
		_selected = NULL;
	}
}

void PopupMenu::Hide() {
	if (_state == state_invisible) {
		return;
	} else if (_timeCounter == 0.f) {
		_state = state_invisible;
		return;
	}
	_state = state_hide;
}

void PopupMenu::Show() {
	if (_state == state_visible) {
		return;
	} else if (_timeCounter == 1.f) {
		_state = state_visible;
		return;
	}
	_state = state_show;
}

bool PopupMenu::IsVisible() {
	return (_state != state_invisible);
}
