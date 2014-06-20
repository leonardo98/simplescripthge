#ifndef PARTICLE_H
#define PARTICLE_H

#include "vector.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <gl.h>
#include <glu.h>
#include <math.h>

class CParticle
{
public:
	//CParticle();//конструктор
	bool InitParticle(const CPos &pos);
	bool InitParticleBum(const CPos &pos);

	void RenderParticle();
	void RenderParticleBum();

	void UpdateParticle(float &g_fElpasedTime);
	void UpdateParticleBum(float &g_fElpasedTime);

	CPos pos;
	float rotationx;
	float rotationy;
	float rotationz;

	float smx;
	float smz;

	float gr;

};

#endif