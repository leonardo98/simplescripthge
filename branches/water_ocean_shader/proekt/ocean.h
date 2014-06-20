#pragma once

#include "windows.h"
#include <glew.h>
#include <gl.h>
#include <glaux.h>
#include <glext.h>
#include <vector>
#include "vec4.h"
#include "Program.h"

#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

typedef unsigned int uint;

struct UV
{
	float u;
	float v;
};

class Ocean
{
private:
	int width;
	int height;
	Program *render;
	Program *sky;
	Program *skymap;
	Program *clouds;
	unsigned int skyTexSize;
	GLuint skyTex;
	GLuint noiseTex;
	bool cloudLayer;
	float octaves;
	float lacunarity;
	float gain;
	float norm;
	float clamp1;
	float clamp2;
	float cloudColor[4];
	GLuint fbo;
	GLuint vbo;
	GLuint vboIndices;
	vec4f vboParams;
	int vboSize;
	float sunTheta;
	float sunPhi;
	float cameraHeight;
	float cameraTheta;

	// RENDERING OPTIONS
	float gridSize;
	float nyquistMin;
	float nyquistMax;
	float seaColor[4];
	float hdrExposure;
	bool grid;
	bool animate;
	bool seaContrib;
	bool sunContrib;
	bool skyContrib;
	bool manualFilter;

	// WAVES PARAMETERS (INPUT)
	GLuint waveTex;
	int nbWaves;
	vec4f *waves;
	float lambdaMin;
	float lambdaMax;
	float heightMax;
	float waveDirection;
	float U0;
	float waveDispersion;

	// WAVE STATISTICS (OUTPUT)
	float sigmaXsq;
	float sigmaYsq;
	float meanHeight;
	float heightVariance;
	float amplitudeMax;

    GLuint transmittanceTex;
    GLuint inscatterTex;

	void generateWaves();
	void generateMesh();
	void loadPrograms(bool all);

	float time;
public:
	~Ocean();
	Ocean(float w, float h);
	void Draw(float cameraD, float cameraT);
	void Update(float dt) { time += dt; }
};

