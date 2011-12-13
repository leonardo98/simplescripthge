#include "GrainField.h"
#include "../Core/Core.h"
#include "PersPaths.h"

GrainField::GrainField() {
	_empty.Set(Core::getTexture("Field_s1"), 310, 136);
	_tab.Set(Core::getTexture("textures\\env_jungle_fields\\tab_field.png"));
	_tabShadow.Set(Core::getTexture("textures\\env_jungle_fields\\tab_field_sh.png"));
	_cut.Set(Core::getTexture("textures\\env_jungle_fields\\grain_cut.png"));
	_state.resize(6); 
	_state[0].Set(Core::getTexture("textures\\env_jungle_fields\\grainfield_16.png"));
	_state[1].Set(Core::getTexture("textures\\env_jungle_fields\\grainfield_26.png"));
	_state[2].Set(Core::getTexture("textures\\env_jungle_fields\\grainfield_36.png"));
	_state[3].Set(Core::getTexture("textures\\env_jungle_fields\\grainfield_46.png"));
	_state[4].Set(Core::getTexture("textures\\env_jungle_fields\\grainfield_56.png"));
	_state[5].Set(Core::getTexture("textures\\env_jungle_fields\\grainfield_66.png"));
	_oldCapacity = _capacity = -1;
	_effect = 0.f;
	_placeType = pt_grain;
	_productName = "grain";
	_pos.x = 426;
	_pos.y = 192;

	grandpaPos.push_back(FPoint2D(450, 140));
	grandpaPos.push_back(FPoint2D(370, 140));
	grandpaPos.push_back(FPoint2D(460, 170));
	grandpaPos.push_back(FPoint2D(380, 180));
	grandpaPos.push_back(FPoint2D(460, 205));
	grandpaPos.push_back(FPoint2D(400, 210));
	for (int i = 0; i < grandpaPos.size(); ++i) {
		grandpaPos[i] = PersPaths::SearchNearest(grandpaPos[i]);
	}
}

