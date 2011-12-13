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
	static int Size();
};

#endif//BIRDSMANAGER_H