#ifndef MYENGINE_ACTION_H
#define MYENGINE_ACTION_H

#include "../Core/types.h"
#include "PlantPlace.h"
#include "TreePlace.h"
#include "ProductPlace.h"
#include "EnvWell.h"
#include "Waterpan.h"
#include "Foodpan.h"
#include "BaseField.h"

struct Action {
	ProductPlace *productPlace;
	PlantPlace *plantPlace;
	TreePlace *treePlace;
	EnvWell *wellPlace;
	Foodpan *foodPlace;
	Waterpan *waterPlace;
	BaseField *fieldPlace;
	std::string type;
	bool complete;
	typedef std::list<std::pair<float, Animation *> > AnimationList;
	AnimationList animations;
	Action();
	~Action();
	Action &operator=(Action &one);
	void Reset();
};

typedef std::list<Action> Actions;

#endif//MYENGINE_ACTION_H