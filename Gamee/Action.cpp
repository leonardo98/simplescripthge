#include "Action.h"

Action::Action() {
	Reset();
	type = "undefined";
}

Action::~Action() {
	Reset();
}

void Action::Reset() {
	for (AnimationList::iterator i = animations.begin(), e = animations.end(); i != e; ++i) {
		delete i->second;
	}
	animations.clear();
	productPlace = NULL;
	plantPlace = NULL;
	treePlace = NULL;
	wellPlace = NULL;
	foodPlace = NULL;
	waterPlace = NULL;
	fieldPlace = NULL;
	archaeopteryx = NULL;
	animal = NULL;
	type = "has reset";
	complete = false;
}

Action &Action::operator=(Action &one) {
	Reset();
	productPlace = one.productPlace;
	plantPlace = one.plantPlace;
	treePlace = one.treePlace;
	wellPlace = one.wellPlace;
	foodPlace = one.foodPlace;
	waterPlace = one.waterPlace;
	fieldPlace = one.fieldPlace;
	archaeopteryx = one.archaeopteryx;
	animal = one.animal;
	pos = one.pos;
	type = one.type;
	complete = one.complete;
	assert(!complete);
	for (AnimationList::iterator i = one.animations.begin(), e = one.animations.end(); i != e; ++i) {
		animations.push_back(std::make_pair<float, Animation *>(i->first, new Animation(*(i->second))));
	}
	return *this;
}
