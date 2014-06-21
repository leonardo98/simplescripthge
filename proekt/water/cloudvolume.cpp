#include <fstream>
#include "cloudvolume.h"

namespace SkyBox
{

CloudVolume::CloudVolume()
{
	sectors = 0;
}

CloudVolume::~CloudVolume()
{
	if (sectors != 0)
	{
		for (int s = 0; s < sectors; s++)
		{
			for (int f = 0; f < sector[s].faces; f++)
			{
				delete [] sector[s].face[f].index;
			}
			delete [] sector[s].face;
			delete [] sector[s].vertex;
		}
		delete [] sector;
	}
}

void CloudVolume::LoadVolume(char *filename, vector origin)
{
	std::ifstream in;
//	in.open(filename, ios::in | ios::nocreate);
	in.open(filename, std::ios::in);

	char temp[1024];
	
	in >> temp >> sectors;
	sector = new Sector [sectors];
	for (int s = 0; s < sectors; s++)
	{
		in >> temp >> temp;
		in >> temp >> sector[s].vertices;
		sector[s].vertex = new vector [sector[s].vertices];
		for (int v = 0; v < sector[s].vertices; v++)
		{
			in >> sector[s].vertex[v].v[0] >> sector[s].vertex[v].v[1] >> sector[s].vertex[v].v[2];
		}
		in >> temp >> sector[s].faces;
		sector[s].face = new Face [sector[s].faces];
		for (int f = 0; f < sector[s].faces; f++)
		{
			in >> temp >> temp;
			in >> sector[s].face[f].vertices;
			sector[s].face[f].index = new int [sector[s].face[f].vertices];
			for (int i = 0; i < sector[s].face[f].vertices; i++)
			{
				in >> sector[s].face[f].index[i];
			}
		}
	}

	in.close();

	for (int s = 0; s < sectors; s++)
	{
		for (int f = 0; f < sector[s].faces; f++)
		{
			sector[s].face[f].normal = Cross(sector[s].vertex[sector[s].face[f].index[1]] - sector[s].vertex[sector[s].face[f].index[0]],
				sector[s].vertex[sector[s].face[f].index[1]] - sector[s].vertex[sector[s].face[f].index[2]]).normalize();
		}
	}

	radius = 0;
	for (int s = 0; s < sectors; s++)
	{
		for (int v = 0; v < sector[s].vertices; v++)
		{
			float d = sector[s].vertex[v].length();
			if (d > radius)
				radius = d;
		}
	}

	SetOrigin(origin);
}

void CloudVolume::SetOrigin(vector origin)
{
	for (int s = 0; s < sectors; s++)
	{
		for (int f = 0; f < sector[s].faces; f++)
		{
			sector[s].face[f].d = Dot(sector[s].face[f].normal, sector[s].vertex[sector[s].face[f].index[0]] + origin);
		}
	}
}

int CloudVolume::PointInside(vector p)
{
	for (int s = 0; s < sectors; s++)
	{
		bool inside = true;
		for (int f = 0; f < sector[s].faces; f++)
		{
			if (Dot(sector[s].face[f].normal, p) > sector[s].face[f].d)
			{
				inside = false;
				break;
			}
		}
		if (inside)
			return s;
	}

	return -1;
}

}