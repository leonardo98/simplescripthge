#ifndef NOISE_H
#define NOISE_H

void Noise2DInit(float *grid, int dimx, int dimy);
float Sample2D(float v[2], float *grid, int dimx, int dimy, float scale = 1);
float Sample2DS(float v[2], float *grid, int dimx, int dimy, float scale = 1);
inline float Sample2D64(const float *v, const float *grid);
float Perlin2D(float v[2], float *grid1, float *grid2, float *grid3, float *grid4, int dimx, int dimy, float turbulence = 0);

void Noise3DInit(float *grid, int dimx, int dimy, int dimz);
float Sample3D(float v[3], float *grid, int dimx, int dimy, int dimz);
float Perlin3D(float v[3], float *grid1, float *grid2, float *grid3, float *grid4, int dimx, int dimy, int dimz);

#endif