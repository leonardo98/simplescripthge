#include <algorithm>
#include "../Core/Variables.h"
#include "GameField.h"
#include "BirdsManager.h"
#include "AnimalsManager.h"

const float GameField::SELECTION_BORDER = 1.8f;
AnnaPers *GameField::_annaPtr;
BobPers *GameField::_bobPtr;
GrandpaPers *GameField::_grandpaPtr;


bool CmpBaseElement(BaseElement *one, BaseElement *two) {
	return (one->_pos.y < two->_pos.y);
}

void GameField::SortElements() {
	bool swap;
	int startFrom = 0;
	do {
		swap = false;
		for (int i = startFrom; i < _elementList.size() - 1; ++i) {
			if (CmpBaseElement(_elementList[i + 1], _elementList[i])) {
				BaseElement *tmp = _elementList[i];
				_elementList[i] = _elementList[i + 1];
				_elementList[i + 1] = tmp;
				swap = true;
			}
		}
		++startFrom;
	} while (swap);
}

bool GameField::IsMouseOver(const FPoint2D &mousePos) {
	return true;
}

void GameField::OnMouseDown(const FPoint2D &mousePos) {
	if (_persPaths.IsMouseOver(mousePos)) {
		_persPaths.OnMouseDown(mousePos);
		return;
	}
	if (_vipClients.IsUnderMouse(mousePos)) {
		_vipClients.OnMouseDown(mousePos);
		return;
	}
	for (ElementList::reverse_iterator i = _elementList.rbegin(), e = _elementList.rend(); i != e; ++i) {
		if ((*i)->IsUnderMouse(mousePos)) {
			(*i)->OnMouseDown(mousePos);
			return;
		}
	}
	HideAllPopupMenu();
	if (_persIndexDebug == 0) {
		//_anna.Goto(mousePos);
		//_persIndexDebug = 1;
	//} else if (_persIndexDebug == 1) {
	//	_bob.Goto(mousePos);
	//	_persIndexDebug = 2;
	//} else if (_persIndexDebug == 2) {
	//	_grandpa.Goto(mousePos);
	//	_persIndexDebug = 0;
	} else {
		assert(false);
	}
	_clientsManager.OnMouseDown(mousePos);
}

GameField::GameField(TiXmlElement *xe)
: _treeBed1(1)
, _treeBed2(2)
, _plantBed1(1)
, _plantBed2(2)
, _plantBed3(3)
, _plantBed4(4)
, _storagePlace1(pt_free1)
, _storagePlace2(pt_free2)
, _productPlace2(pt_tree)
, _productPlace3(pt_grain)
, _productPlace4(pt_clover)
, _productPlace5(pt_unknown)
, _productPlace6(pt_egg)
, _waterPan1(1)
, _waterPan2(2)
, _foodPan1(1)
, _foodPan2(2)
{
	_annaPtr = &_anna;
	_bobPtr = &_bob;
	_grandpaPtr = &_grandpa;
	Variables::Set("money", "1000");
	BirdsManager::Init(&_waterPan2, &_foodPan2);
	AnimalsManager::Init(&_waterPan1, &_foodPan1);
	_buckPlace = new ProductPlace(pt_water);
	_well = new EnvWell();
	//_testBackground.Set(Core::getTexture("data\\EnvironmentJungle_Bob_HT.jpg"));
	_persIndexDebug = 0;

	Matrix transform;
	transform.Move(1024, 0);
	transform.Scale(-1.f, 1.f);

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

	_elementList.push_back(&_anna);
	_elementList.push_back(&_bob);
	_elementList.push_back(&_grandpa);
	_elementList.push_back(&_grainField);
	_elementList.push_back(&_cloverField);
	_elementList.push_back(&_treeBed1);
	_elementList.push_back(&_treeBed2);
	_elementList.push_back(&_plantBed1);
	_elementList.push_back(&_plantBed2);
	_elementList.push_back(&_plantBed3);
	_elementList.push_back(&_plantBed4);
	_elementList.push_back(_well);
	_elementList.push_back(&_storagePlace1);
	_elementList.push_back(&_storagePlace2);
	_elementList.push_back(_buckPlace);
	_elementList.push_back(&_productPlace2);
	_elementList.push_back(&_productPlace3);
	_elementList.push_back(&_productPlace4);
	_elementList.push_back(&_productPlace5);
	_elementList.push_back(&_productPlace6);
	_elementList.push_back(&_swampPlace);
	_elementList.push_back(&_waterPan1);
	_elementList.push_back(&_waterPan2);
	_elementList.push_back(&_foodPan1);
	_elementList.push_back(&_foodPan2);

	// test/debug only
	AddBird("dodo_b");
	AddBird("dodo_w");
	AddBird("archaeopteryx_w");
	AddAnimal("rabbit_b");
	AddAnimal("rabbit_g");
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

	//_grainField.Draw();

	//_cloverField.Draw();

	_stoneshelve_set1_a.Render();
	_stoneshelve_set1_a_m.Render();
	_stoneshelve_set1_a2.Render();
	_stoneshelve_set1_a2_m.Render();
	_stoneshelve_set1_b.Render();
	_stoneshelve_set1_b_m.Render();
	_stoneshelve_set1_d.Render();
	_stoneshelve_set1_d_m.Render();
	_stoneshelve_set1_e.Render();

	_grass_set1_e.Render();
	_grass_set1_e_m.Render();
	_grass_set1_f.Render();

	SortElements();
	for (ElementList::iterator i = _elementList.begin(), e = _elementList.end(); i != e; ++i) {
		(*i)->DrawBottom();
	}
	for (ElementList::iterator i = _elementList.begin(), e = _elementList.end(); i != e; ++i) {
		(*i)->Draw();
	}

	Render::PushMatrix();
	Render::SetColor(0xFFA791D5);
	Render::MatrixMove(-3, 0);
	DrawBushes();
	Render::MatrixMove(6, 0);
	DrawBushes();
	Render::MatrixMove(-3, -3);
	DrawBushes();
	Render::SetColor(0xFFFFFFFF);
	Render::PopMatrix();
	DrawBushes();
	_clientsManager.Draw();
	for (ElementList::iterator i = _cover.begin(), e = _cover.end(); i != e; ++i) {
		(*i)->Draw();
	}
	_vipClients.Draw();
	_moneypod.Draw();

	_persPaths.Draw();
}

void GameField::OnMouseUp() {
	_persPaths.OnMouseUp();
}

void GameField::OnMouseMove(const FPoint2D &mousePos) {
	_persPaths.OnMouseMove(mousePos);

	_vipClients.OnMouseMove(mousePos);

	_lastMousePos = mousePos;
	bool isActive = false;
	for (ElementList::iterator i = _elementList.begin(), e = _elementList.end(); i != e; ++i) {
		if (!isActive && (*i)->IsUnderMouse(mousePos)) {
			(*i)->SetActive(isActive = true);
		} else {
			(*i)->SetActive(false);
		}
		(*i)->OnMouseMove(mousePos);
	}
	_clientsManager.OnMouseMove(mousePos);
}

void GameField::Update(float dt) {
	_vipClients.Update(dt);
	for (PopupMenus::iterator i = _popupMenus.begin(), e = _popupMenus.end(); i != e; ++i) {
		(*i)->Update(dt);
	}
	for (ElementList::iterator i = _elementList.begin(); i != _elementList.end(); ) {
		(*i)->Update(dt);
		if ((*i)->Dead()) {
			delete (*i);
			i = _elementList.erase(i);
		} else if ((*i)->_pos.y >= 768.f) {
			_cover.push_back(*i);
			i = _elementList.erase(i);
		} else {
			++i;
		}
	}
	for (ElementList::iterator i = _cover.begin(); i != _cover.end(); ) {
		(*i)->Update(dt);
		if ((*i)->Dead()) {
			delete (*i);
			i = _cover.erase(i);
		} else {
			++i;
		}
	}
/*	int MAX = 10;
	for (int k = 0; k < MAX; ++k) {
		for (Birds::iterator i = _archaeopteryx.begin(), e = _archaeopteryx.end(); i != e; ++i) {
			(*i)->Update(dt / MAX);
		}
	}
*/
	if (Render::GetDC()->Input_KeyDown(HGEK_D)) {
		_persPaths.SetVisible_debug(Render::GetDC()->Input_GetKeyState(HGEK_SHIFT));
	} else if (Render::GetDC()->Input_KeyDown(HGEK_A)) {
		Archaeopteryx *a = new Archaeopteryx();
		_archaeopteryx.push_back(a);
		_elementList.push_back(a);
	}
	_clientsManager.Update(dt);
	_moneypod.Update(dt);
}

GameField::~GameField(void) {
	for (unsigned int i = 0; i < _archaeopteryx.size(); ++i) {
		delete _archaeopteryx[i];
	}
	delete _buckPlace;
	delete _well;
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

ProductPlace * GameField::GetBuckPlace() {
	return _buckPlace;
}

EnvWell * GameField::GetWell() {
	return _well;
}

GameField::Birds GameField::_archaeopteryx;
GameField::AnimalPtrs GameField::_animalPtrs;
GameField::ElementList GameField::_elementList;

void GameField::AddBird(const std::string &birdId) {
	Archaeopteryx *a = new Archaeopteryx(birdId);
	_archaeopteryx.push_back(a);
	_elementList.push_back(a);
}

void GameField::AddAnimal(const std::string &animalId) {
	Animal *a = new Animal(animalId);
	_animalPtrs.push_back(a);
	_elementList.push_back(a);
}


ProductPlace * GameField::_buckPlace;
EnvWell * GameField::_well;

float GameField::AddDropEffect(const std::string &persName, const FPoint2D &endPos, float height, unsigned char endAlpha, float endScale) {
	Pers *pers = NULL;
	std::string product;
	if (persName == "Anna") {
		pers = _annaPtr;
		product = _annaPtr->GetProductType();
	} else if (persName == "Bob") {
		pers = _bobPtr;
		product = "";//_bobPtr->GetProductType();
	} else if (persName == "Grandpa") {
		pers = _grandpaPtr;
		product = "";//_grandpaPtr->GetProductType();
	} else {
		assert(false);
	}
	DropEffect *e = new DropEffect(pers->_pos, 
						FPoint2D(pers->_mirror ? - pers->_productOffset.x : pers->_productOffset.x, pers->_productOffset.y),
						endPos, 
						FPoint2D(0, - height), product, 
						endAlpha, endScale);	
	_elementList.push_back(e);
	return e->Time();
}
