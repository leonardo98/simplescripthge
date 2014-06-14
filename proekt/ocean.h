#pragma once

#include "windows.h"
#include <gl.h>
#include <glaux.h>
#include <glext.h>
#include <vector>

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
	GLuint _vboVert;
	GLuint _vboUV;
	uint _num;
public:
	~Ocean();
	Ocean(float x1, float z1, float x2, float z2, float y, uint xDiv, uint zDiv);
	void Draw();
};

