#include "GameField.h"
#include <algorithm>

const float GameField::SELECTION_BORDER = 1.8f;

bool CmpBaseElement(BaseElement *one, BaseElement *two) {
	return (one->_pos.y < two->_pos.y);
}

bool GameField::IsMouseOver(const FPoint2D &mousePos) {
	return true;
}

void GameField::OnMouseDown(const FPoint2D &mousePos) {
	if (_persPaths.IsMouseOver(mousePos)) {
		_persPaths.OnMouseDown(mousePos);
		return;
	}
	for (RenderList::iterator i = _updateList.begin(), e = _updateList.end(); i != e; ++i) {
		if ((*i)->IsUnderMouse(mousePos)) {
			(*i)->OnMouseDown(mousePos);
			return;
		}
	}
	HideAllPopupMenu();
	if (_persIndexDebug == 0) {
		_anna.Goto(mousePos);
		_persIndexDebug = 1;
	} else if (_persIndexDebug == 1) {
		_bob.Goto(mousePos);
		_persIndexDebug = 2;
	} else if (_persIndexDebug == 2) {
		_grandpa.Goto(mousePos);
		_persIndexDebug = 0;
	} else {
		assert(false);
	}
}

GameField::GameField(TiXmlElement *xe)
: _treeBed1(1)
, _treeBed2(2)
, _plantBed1(1)
, _plantBed2(2)
, _plantBed3(3)
, _plantBed4(4)
, _storagePlace1(6)
, _storagePlace2(7)
, _buckPlace(1)
, _productPlace2(2)
, _productPlace3(3)
, _productPlace4(4)
, _productPlace5(5)
{
	_persIndexDebug = 0;

	Matrix transform;
	transform.Scale(-1.f, 1.f);
	transform.Move(1024, 0);

	_field_s1.Set(Core::getTexture("Field_s1"), 310, 136);

	_field_s1_m.Set(Core::getTexture("Field_s1"), 310, 136);
	_field_s1_m.PushTransform(transform);

	_grass_set1_a.Set(Core::getTexture("grass_set1_a"), 0, 152);

	_grass_set1_a_m.Set(Core::getTexture("grass_set1_a"), 0, 152);
	_grass_set1_a_m.PushTransform(transform);

	_grass_set1_e.Set(Core::getTexture("grass_set1_e"), 0, 433);

	_grass_set1_e_m.Set(Core::getTexture("grass_set1_e"), 0, 433);
	_grass_set1_e_m.PushTransform(transform);

	_grass_set1_f.Set(Core::getTexture("grass_set1_f"), 327, 520);

	//_groundup_s1.Set(Core::getTexture("groundup_s1"));

	_ground_s1.Set(Core::getTexture("ground_s1"), 0, 225);
	
	_road_set1_a.Set(Core::getTexture("road_set1_a"), 294, 155);
	
	_road_set1_a_m.Set(Core::getTexture("road_set1_a"), 294, 155);
	_road_set1_a_m.PushTransform(transform);

	_road_set1_b.Set(Core::getTexture("road_set1_b"), 496, 138);

	_road_set1_c.Set(Core::getTexture("road_set1_c"), 273, 504);
	
	_road_set1_c_m.Set(Core::getTexture("road_set1_c"), 273, 504);
	_road_set1_c_m.PushTransform(transform);

	_s01_bush.Set(Core::getTexture("s01_bush"), -12, 682);

	_stoneshelve_set1_a.Set(Core::getTexture("stoneshelve_set1_a"), 0, 295);

	_stoneshelve_set1_a_m.Set(Core::getTexture("stoneshelve_set1_a"), 0, 295);
	_stoneshelve_set1_a_m.PushTransform(transform);

	_stoneshelve_set1_a2.Set(Core::getTexture("stoneshelve_set1_a"), -21, 427);

	_stoneshelve_set1_a2_m.Set(Core::getTexture("stoneshelve_set1_a"), -21, 427);
	_stoneshelve_set1_a2_m.PushTransform(transform);

	_stoneshelve_set1_b.Set(Core::getTexture("stoneshelve_set1_b"), 305, 259);

	_stoneshelve_set1_b_m.Set(Core::getTexture("stoneshelve_set1_b"), 305, 259);
	_stoneshelve_set1_b_m.PushTransform(transform);

	_stoneshelve_set1_c.Set(Core::getTexture("stoneshelve_set1_c"), 379, 232);

	_stoneshelve_set1_d.Set(Core::getTexture("stoneshelve_set1_d"), 286, 436);

	_stoneshelve_set1_d_m.Set(Core::getTexture("stoneshelve_set1_d"), 286, 436);
	_stoneshelve_set1_d_m.PushTransform(transform);

	_stoneshelve_set1_e.Set(Core::getTexture("stoneshelve_set1_e"), 405, 504);

	_bush1.Set(Core::getTexture("bush"), -12, 682);
	_bush2.Set(Core::getTexture("bush"), 120, 682);
	_bush3.Set(Core::getTexture("bush"), 252, 682);
	_bush4.Set(Core::getTexture("bush"), 384, 682);
	_bush5.Set(Core::getTexture("bush"), 516, 682);
	_bush6.Set(Core::getTexture("bush"), 648, 682);
	_bush7.Set(Core::getTexture("bush"), 772, 682);

	_renderList.push_back(&_anna);
	_renderList.push_back(&_bob);
	_renderList.push_back(&_grandpa);
	_renderList.push_back(&_treeBed1);
	_renderList.push_back(&_treeBed2);
	_renderList.push_back(&_plantBed1);
	_renderList.push_back(&_plantBed2);
	_renderList.push_back(&_plantBed3);
	_renderList.push_back(&_plantBed4);
	_renderList.push_back(&_well);
	_renderList.push_back(&_storagePlace1);
	_renderList.push_back(&_storagePlace2);
	_renderList.push_back(&_buckPlace);
	_renderList.push_back(&_productPlace2);
	_renderList.push_back(&_productPlace3);
	_renderList.push_back(&_productPlace4);
	_renderList.push_back(&_productPlace5);
	_renderList.push_back(&_archaeopteryx);

	_updateList.push_back(&_anna);
	_updateList.push_back(&_bob);
	_updateList.push_back(&_grandpa);
	_updateList.push_back(&_grainField);
	_updateList.push_back(&_cloverField);
	_updateList.push_back(&_treeBed1);
	_updateList.push_back(&_treeBed2);
	_updateList.push_back(&_plantBed1);
	_updateList.push_back(&_plantBed2);
	_updateList.push_back(&_plantBed3);
	_updateList.push_back(&_plantBed4);
	_updateList.push_back(&_well);
	_updateList.push_back(&_storagePlace1);
	_updateList.push_back(&_storagePlace2);
	_updateList.push_back(&_buckPlace);
	_updateList.push_back(&_productPlace2);
	_updateList.push_back(&_productPlace3);
	_updateList.push_back(&_productPlace4);
	_updateList.push_back(&_productPlace5);
	_updateList.push_back(&_archaeopteryx);

	std::sort(_updateList.rbegin(), _updateList.rend(), CmpBaseElement);
}

void GameField::DrawBushes() {
	_bush1.Render();
	_bush2.Render();
	_bush3.Render();
	_bush4.Render();
	_bush5.Render();
	_bush6.Render();
	_bush7.Render();
}

void GameField::Draw() {
	_ground_s1.Render();

	_road_set1_a.Render();
	_road_set1_a_m.Render();
	_road_set1_b.Render();
	_road_set1_c.Render();
	_road_set1_c_m.Render();

	_grass_set1_a.Render();
	_grass_set1_a_m.Render();

	_field_s1.Render();
	_field_s1_m.Render();

	_s01_bush.Render();

	_grainField.Draw();

	_cloverField.Draw();

	_stoneshelve_set1_a.Render();
	_stoneshelve_set1_a_m.Render();
	_stoneshelve_set1_a2.Render();
	_stoneshelve_set1_a2_m.Render();
	_stoneshelve_set1_b.Render();
	_stoneshelve_set1_b_m.Render();
	_stoneshelve_set1_c.Render();
	_stoneshelve_set1_d.Render();
	_stoneshelve_set1_d_m.Render();
	_stoneshelve_set1_e.Render();

	_grass_set1_e.Render();
	_grass_set1_e_m.Render();
	_grass_set1_f.Render();

	for (RenderList::iterator i = _renderList.begin(), e = _renderList.end(); i != e; ++i) {
		(*i)->DrawBottom();
	}
	std::sort(_renderList.begin(), _renderList.end(), CmpBaseElement);
	for (RenderList::iterator i = _renderList.begin(), e = _renderList.end(); i != e; ++i) {
		(*i)->Draw();
	}

	Render::PushMatrix();
	//Render::SetBlendMode(1);
	Render::SetColor(0xFFA791D5);
	Render::MatrixMove(-3, 0);
	DrawBushes();
	Render::MatrixMove(6, 0);
	DrawBushes();
	Render::MatrixMove(-3, -3);
	DrawBushes();
	Render::SetColor(0xFFFFFFFF);
	Render::PopMatrix();
	//Render::SetBlendMode(BLEND_DEFAULT);
	DrawBushes();

	_persPaths.Draw();
}

void GameField::OnMouseUp() {
	_persPaths.OnMouseUp();
}

void GameField::OnMouseMove(const FPoint2D &mousePos) {
	_persPaths.OnMouseMove(mousePos);

	_lastMousePos = mousePos;
	bool isActive = false;
	for (RenderList::iterator i = _updateList.begin(), e = _updateList.end(); i != e; ++i) {
		if (!isActive && (*i)->IsUnderMouse(mousePos)) {
			(*i)->SetActive(isActive = true);
		} else {
			(*i)->SetActive(false);
		}
		(*i)->OnMouseMove(mousePos);
	}
}

void GameField::Update(float dt) {
	for (RenderList::iterator i = _updateList.begin(), e = _updateList.end(); i != e; ++i) {
		(*i)->Update(dt);
	}
	if (Render::GetDC()->Input_KeyDown(HGEK_D)) {
		_persPaths.SetVisible_debug(Render::GetDC()->Input_GetKeyState(HGEK_SHIFT));
	}
}

GameField::~GameField(void)
{
}

GameField::PopupMenus GameField::_popupMenus;

void GameField::HideAllPopupMenu() {
	PopupMenus::iterator i, e;
	for (i = _popupMenus.begin(), e = _popupMenus.end(); i != e; ++i) {
		(*i)->Hide();
	}
}

void GameField::AddPopupMenu(PopupMenu *menu) {
	PopupMenus::iterator i, e;
	for (i = _popupMenus.begin(), e = _popupMenus.end(); i != e && menu != (*i); ++i) {}
	assert(i == e);
	_popupMenus.push_back(menu);
}

void GameField::RemovePopupMenu(PopupMenu *menu) {
	PopupMenus::iterator i, e;
	for (i = _popupMenus.begin(), e = _popupMenus.end(); i != e && menu != (*i); ++i) {}
	assert(i != e);
	_popupMenus.erase(i);
}
