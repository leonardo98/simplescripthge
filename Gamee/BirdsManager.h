#ifndef MYENGINE_BIRDSMANAGER_H
#define MYENGINE_BIRDSMANAGER_H

#include "../Core/types.h"
#include "Archaeopteryx.h"
#include "Waterpan.h"
#include "Foodpan.h"

class BirdsManager {
public:
	static void BirdsManager::Init(Waterpan *waterpan, Foodpan *foodpan);
	static std::vector<Archaeopteryx *> _positions;
	static float Distance(const Archaeopteryx *a, const Archaeopteryx *b);
	static void UpdatePosition(Archaeopteryx *a, float dt);
	static bool FreePosition(Archaeopteryx *a);

	static void SetWaterBusy(bool busy);
	static bool IsWaterBusy();
	static bool IsWaterEmpty();
	static void DrinkWater();

	static void SetFoodBusy(bool busy);
	static bool IsFoodBusy();
	static bool IsFoodEmpty();
	static void EatFood();
	static int Size();
private:
    static bool _foodBusy;
    static bool _waterBusy;
	static Waterpan *_waterpan;
	static Foodpan *_foodpan;
};

#endif//MYENGINE_BIRDSMANAGER_H