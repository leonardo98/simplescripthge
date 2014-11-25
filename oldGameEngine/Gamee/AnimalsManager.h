#ifndef MYENGINE_ANIMALSMANAGER_H
#define MYENGINE_ANIMALSMANAGER_H

#include "../Core/types.h"
#include "Animal.h"
#include "Waterpan.h"
#include "Foodpan.h"

class AnimalsManager {
public:
	static void AnimalsManager::Init(Waterpan *waterpan, Foodpan *foodpan);
	typedef std::vector<Animal *> Animals;
	static Animals _animals;
	static float Distance(const Animal *a, const Animal *b);
	static void UpdatePosition(Animal *a, float dt);
	static bool FreePosition(Animal *a);

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

#endif//MYENGINE_ANIMALSMANAGER_H