#ifndef CLOUDVOLUME_H
#define CLOUDVOLUME_H

#include "vector.h"

struct Face
{
	int vertices;
	int *index;
	vector normal;
	float d;
};

struct Sector
{
	int faces;
	Face *face;
	int vertices;
	vector *vertex;
};

class CloudVolume
{
private:
public:
	int sectors;
	Sector *sector;

	float radius;

	CloudVolume();
	~CloudVolume();

	void LoadVolume(char *filename, vector origin);
	void SetOrigin(vector origin);
	int PointInside(vector p);
};

#endif