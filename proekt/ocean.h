#pragma once

#include "windows.h"
#include <glew.h>
#include <gl.h>
#include <glaux.h>
#include <glext.h>
#include <vector>


class Ocean
{
private:

	float time;
public:
	~Ocean();
	Ocean(float w, float h);
	void Draw(float cameraD, float cameraT);
	void Update(float dt) { time += dt; }
};

