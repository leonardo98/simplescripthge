#ifndef MYENGINE_BIRDSMANAGER_H
#define MYENGINE_BIRDSMANAGER_H

#include "../Core/types.h"
#include "Archaeopteryx.h"
#include "Waterpan.h"
#include "Foodpan.h"

class BirdsManager {
public:
	static void BirdsManager::Init(Waterpan *waterpan, Foodpan *foodpan);
	typedef std::vector<Archaeopteryx *> Birds;
	static Birds _birds;
	static float Distance(const Archaeopteryx *a, const Archaeopteryx *b);
	static void UpdatePosition(Archaeopteryx *a, float dt);
	static bool FreePosition(Archaeopteryx *a);

	static bool IsWaterBusy();
	static bool IsWaterEmpty();
	static void DrinkWater();

	static bool IsFoodBusy();
	static bool IsFoodEmpty();
	static void EatFood();
	static int Size();
private:
	static Waterpan *_waterpan;
	static Foodpan *_foodpan;
};

#endif//MYENGINE_BIRDSMANAGER_H