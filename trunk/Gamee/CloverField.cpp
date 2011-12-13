#include "CloverField.h"
#include "../Core/Core.h"
#include "PersPaths.h"

CloverField::CloverField() {
	Matrix transform;
	transform.Move(1024, 0);
	transform.Scale(-1.f, 1.f);
	_pos.x = 598;
	_pos.y = 193;// must be grater then GrainField::_pos.y

	_empty.Set(Core::getTexture("Field_s1"), 310, 136);
	_empty.PushTransform(transform);
	_tab.Set(Core::getTexture("textures\\env_jungle_fields\\tab_field.png"));
	_tab.PushTransform(transform);
	_tabShadow.Set(Core::getTexture("textures\\env_jungle_fields\\tab_field_sh.png"));
	_tabShadow.PushTransform(transform);

	_cut.Set(Core::getTexture("textures\\env_jungle_fields\\clover_cut.png"));
	_state.resize(6); 
	_state[0].Set(Core::getTexture("textures\\env_jungle_fields\\cloverfield_16.png"));
	_state[1].Set(Core::getTexture("textures\\env_jungle_fields\\cloverfield_26.png"));
	_state[2].Set(Core::getTexture("textures\\env_jungle_fields\\cloverfield_36.png"));
	_state[3].Set(Core::getTexture("textures\\env_jungle_fields\\cloverfield_46.png"));
	_state[4].Set(Core::getTexture("textures\\env_jungle_fields\\cloverfield_56.png"));
	_state[5].Set(Core::getTexture("textures\\env_jungle_fields\\cloverfield_66.png"));
	_capacity = -1;
	_effect = 0.f;
	_placeType = pt_clover;
	_productName = "clover";

	grandpaPos.push_back(FPoint2D(1024 - 450, 140));
	grandpaPos.push_back(FPoint2D(1024 - 370, 140));
	grandpaPos.push_back(FPoint2D(1024 - 460, 170));
	grandpaPos.push_back(FPoint2D(1024 - 380, 180));
	grandpaPos.push_back(FPoint2D(1024 - 460, 205));
	grandpaPos.push_back(FPoint2D(1024 - 400, 210));
	for (int i = 0; i < grandpaPos.size(); ++i) {
		grandpaPos[i] = PersPaths::SearchNearest(grandpaPos[i]);
	}

	_stoneshelve_set1_c.Set(Core::getTexture("stoneshelve_set1_c"), 379, 232);
}

void CloverField::DrawBottom() {
	BaseField::DrawBottom();
	_stoneshelve_set1_c.Render();// hack
}