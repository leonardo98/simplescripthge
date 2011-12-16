#ifndef BIRDSMANAGER_H
#define BIRDSMANAGER_H

#include "../Core/types.h"
#include "Archaeopteryx.h"

class BirdsManager {
public:
	BirdsManager();
	static std::vector<Archaeopteryx *> _positions;
	static float Distance(const Archaeopteryx *a, const Archaeopteryx *b);
	static void UpdatePosition(Archaeopteryx *a, float dt);
	static bool FreePosition(Archaeopteryx *a);
	static void SetWaterBusy(bool busy);
	static bool IsWaterBusy();
	static void SetFoodBusy(bool busy);
	static bool IsFoodBusy();
	static int Size();
private:
    static bool _foodBusy;
    static bool _waterBusy;
};

#endif//BIRDSMANAGER_H