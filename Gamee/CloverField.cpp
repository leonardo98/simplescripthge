#include "CloverField.h"
#include "../Core/Core.h"

CloverField::CloverField() {
	Matrix transform;
	transform.Scale(-1.f, 1.f);
	transform.Move(1024, 0);

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
}