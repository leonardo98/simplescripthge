#ifndef MAIN_H
#define MAIN_H

#pragma warning (disable: 4018 4244 4305 4102)

#pragma comment (lib, "opengl32")
#pragma comment (lib, "glu32")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "dinput8")

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <gl.h>
#include <glu.h>
#include <glext.h>
#include <dinput.h>
#include <time.h>
#include <io.h>
#include "timer.h"
#include "skybox_vector.h"
#include "matrix.h"
#include "extensions.h"
#include "random.h"
#include "cloudvolume.h"
#include "noise.h"
#include "fft.h"

extern int wsizex, wsizey, wbitdepth;

namespace SkyBox
{

#define FPS_FRAMES 200
extern float averagefps;
extern float fps[FPS_FRAMES];
extern float frametime;
extern float elapsedtime;
extern Timer timer;

struct Camera
{
	vector pos, rot;
};

extern Camera camera;

// render.cpp

struct TexInfo
{
	char filename[256];
	int width, height;
};

extern unsigned int		*texture;
extern unsigned int		textures;
extern TexInfo			*texinfo;

extern unsigned int		fonttex;

extern double modelview[16];
extern double projection[16];
extern int viewport[4];


void Render();
void ShutdownRender();
void SetupRender();
void Resize(int width, int height, int xoffset, int yoffset, float fov);
int CreateTexture(char filename[], bool clamp, bool duplicate);
bool LoadCubemap(char name[], unsigned int &texid);


// font.cpp

extern unsigned int fontbase;

void PrintULTRA(float scale, int x, int y, char format[], ...);
void Print(int x, int y, char format[], ...);
void DeleteFont();
void BuildFont(char *widthfile, int tex, unsigned int &base);


// input.cpp

extern DIMOUSESTATE2			dims;
extern DIMOUSESTATE2			lastdims;
//extern DIJOYSTATE				dijs;
extern BYTE					keybuffer[256];
extern BYTE					keybuffer2[256];

extern float				mousex, mousey;

void Input();
int SetupInput();
void ShutdownInput();

}

#endif