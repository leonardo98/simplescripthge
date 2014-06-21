#pragma warning (disable: 4244)

#include <math.h>
#include "random.h"


void Noise2DInit(float *grid, int dimx, int dimy)
{
	for (int x = 0; x < dimx; x++)
	{
		for (int y = 0; y < dimy; y++)
		{
			grid[x * dimy + y] = genrand();
		}
	}
}

// S-Curve
float Sample2DS(float v[2], float *grid, int dimx, int dimy, float scale = 1)
{
	unsigned int xi, yi;
	float xr, yr, v2[2];

	v2[0] = fmodf(v[0] * scale, dimx);
	v2[1] = fmodf(v[1] * scale, dimy);
	if (v2[0] < 0)
		v2[0] += dimx;
	if (v2[1] < 0)
		v2[1] += dimy;

	xi = v2[0];
	yi = v2[1];
	xr = v2[0] - xi;
	yr = v2[1] - yi;
	xr = xr * xr * (3 - 2 * xr);
	yr = yr * yr * (3 - 2 * yr);


	return (grid[xi * dimy + yi] * (1 - xr) + grid[(xi + 1) % dimx * dimy + yi] * xr) * (1 - yr)
		+ (grid[xi * dimy + (yi + 1) % dimy] * (1 - xr) + grid[(xi + 1) % dimx * dimy + (yi + 1) % dimy] * xr) * yr;
}

float Sample2D(float v[2], float *grid, int dimx, int dimy, float scale = 1)
{
	unsigned int xi, yi;
	float xr, yr, v2[2];

	v2[0] = fmodf(v[0] * scale, dimx);
	v2[1] = fmodf(v[1] * scale, dimy);
	if (v2[0] < 0)
		v2[0] += dimx;
	if (v2[1] < 0)
		v2[1] += dimy;

	xi = v2[0];
	yi = v2[1];
	xr = v2[0] - xi;
	yr = v2[1] - yi;


	return (grid[xi * dimy + yi] * (1 - xr) + grid[(xi + 1) % dimx * dimy + yi] * xr) * (1 - yr)
		+ (grid[xi * dimy + (yi + 1) % dimy] * (1 - xr) + grid[(xi + 1) % dimx * dimy + (yi + 1) % dimy] * xr) * yr;
}

// non-branching compare for signed integers
//r = y + ((x - y) & ((x - y) >> (sizeof(int) * 8 - 1)); /* min(x, y) */
//r = x - ((x - y) & ((x - y) >> (sizeof(int) * 8 - 1)); /* max(x, y) */


float Sample2D64(const float *v, const float *grid)
{
	unsigned short xi, yi, i2, i3;
	float xr, yr, v2[2];

	v2[0] = fmodf(v[0], 64);
	v2[1] = fmodf(v[1], 64);
	if (v2[0] < 0)
		v2[0] += 64;
	if (v2[1] < 0)
		v2[1] += 64;

	xi = v2[0];
	yi = v2[1];
	xr = v2[0] - xi;
	yr = v2[1] - yi;


	i2 = ((xi + 1) & 0x3f) << 6;
	i3 = (yi + 1) & 0x3f;

	return (grid[(xi << 6) + yi] * (1 - xr) + grid[i2 + yi] * xr) * (1 - yr)
		+ (grid[(xi << 6) + i3] * (1 - xr) + grid[i2 + i3] * xr) * yr;
}

float Perlin2D(float v[2], float *grid1, float *grid2, float *grid3, float *grid4, int dimx, int dimy, float turbulence = 0)
{
	float v2[2];
	v2[0] = v[0];
	v2[1] = v[1];
	float r = 0;

	r = fabsf(Sample2D(v2, grid4, dimx, dimy) * .5f - turbulence) * 2 * .0666667f;

	dimx /= 2;
	dimy /= 2;
	v2[0] *= .5f;
	v2[1] *= .5f;
	r += fabsf(Sample2D(v2, grid3, dimx, dimy) * .5f - turbulence) * 2 * .133333f;

	dimx /= 2;
	dimy /= 2;
	v2[0] *= .5f;
	v2[1] *= .5f;
	r += fabsf(Sample2D(v2, grid2, dimx, dimy) * .5f - turbulence) * 2 * .266667f;

	dimx /= 2;
	dimy /= 2;
	v2[0] *= .5f;
	v2[1] *= .5f;
	r += fabsf(Sample2D(v2, grid1, dimx, dimy) * .5f - turbulence) * 2 * .533333f;

	return r;
}





void Noise3DInit(float *grid, int dimx, int dimy, int dimz)
{
	for (int x = 0; x < dimx; x++)
	{
		for (int y = 0; y < dimy; y++)
		{
			for (int z = 0; z < dimz; z++)
			{
				grid[x * dimy * dimz + y * dimz + z] = genrand();
			}
		}
	}
}

float Sample3D(float v[3], float *grid, int dimx, int dimy, int dimz)
{
	int xi, yi, zi;
	float xr, yr, zr;
	float v2[3];

	v2[0] = fmodf(v[0], dimx);
	v2[1] = fmodf(v[1], dimy);
	v2[2] = fmodf(v[2], dimz);
	if (v2[0] < 0)
		v2[0] += dimx;
	if (v2[1] < 0)
		v2[1] += dimy;
	if (v2[2] < 0)
		v2[2] += dimz;

	xi = v2[0];
	xr = v2[0] - xi;

	yi = v2[1];
	yr = v2[1] - yi;

	zi = v2[2];
	zr = v2[2] - zi;


	float x1, x2, x3, x4;
	x1 = grid[xi * dimy * dimz + yi * dimz + zi] * (1 - xr)
		+ grid[(xi + 1) % dimx * dimy * dimz + yi * dimz + zi] * xr;

	x2 = grid[xi * dimy * dimz + (yi + 1) % dimy * dimz + zi] * (1 - xr)
		+ grid[(xi + 1) % dimx * dimy * dimz + (yi + 1) % dimy * dimz + zi] * xr;

	x3 = grid[xi * dimy * dimz + yi * dimz + (zi + 1) % dimz] * (1 - xr)
		+ grid[(xi + 1) % dimx * dimy * dimz + yi * dimz + (zi + 1) % dimz] * xr;

	x4 = grid[xi * dimy * dimz + (yi + 1) % dimy * dimz + (zi + 1) % dimz] * (1 - xr)
		+ grid[(xi + 1) % dimx * dimy * dimz + (yi + 1) % dimy * dimz + (zi + 1) % dimz] * xr;

	float y1, y2;
	y1 = x1 * (1 - yr) + x2 * yr;
	y2 = x3 * (1 - yr) + x4 * yr;

	return y1 * (1 - zr) + y2 * zr;
}

float Perlin3D(float v[3], float *grid1, float *grid2, float *grid3, float *grid4, int dimx, int dimy, int dimz)
{
	float v2[3];
	v2[0] = v[0];
	v2[1] = v[1];
	v2[2] = v[2];
	float r = 0;

	r = Sample3D(v2, grid4, dimx, dimy, dimz) * .0666667f;

	dimx /= 2;
	dimy /= 2;
	dimz /= 2;
	v2[0] *= .5f;
	v2[1] *= .5f;
	v2[2] *= .5f;
	r += Sample3D(v2, grid3, dimx, dimy, dimz) * .133333f;

	dimx /= 2;
	dimy /= 2;
	dimz /= 2;
	v2[0] *= .5f;
	v2[1] *= .5f;
	v2[2] *= .5f;
	r += Sample3D(v2, grid2, dimx, dimy, dimz) * .266667f;

	dimx /= 2;
	dimy /= 2;
	dimz /= 2;
	v2[0] *= .5f;
	v2[1] *= .5f;
	v2[2] *= .5f;
	r += Sample3D(v2, grid1, dimx, dimy, dimz) * .533333f;

	return r;
}