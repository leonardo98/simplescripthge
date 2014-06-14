#include "ocean.h"
#include "vector3f.h"

// VBO Extension Function Pointers

PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;                  // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;                  // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;                  // VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;                // VBO Deletion Procedure

void PushQuad(vector3f *buff, vector3f pos, float w, float h, UV *uv, float u1, float v1, float u2, float v2)
{
	*buff = pos;
	++buff;
	*buff = pos; buff->x += w;
	++buff;
	*buff = pos; buff->z += h;
	++buff;
	*buff = pos; buff->z += h; buff->x += w;
	++buff;
	*buff = pos; buff->z += h;
	++buff;
	*buff = pos; buff->x += w;

	uv->u = u1; uv->v = v1;
	++uv;
	uv->u = u2; uv->v = v1;
	++uv;
	uv->u = u1; uv->v = v2;
	++uv;
	uv->u = u2; uv->v = v2;
	++uv;
	uv->u = u1; uv->v = v2;
	++uv;
	uv->u = u2; uv->v = v1;
}

Ocean::~Ocean()
{
	glDeleteBuffersARB(1, &_vboVert);
	glDeleteBuffersARB(1, &_vboUV);
}

Ocean::Ocean(float x1, float z1, float x2, float z2, float y, uint xDiv, uint zDiv)
{
    glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
    glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
    glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
    glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");

	_num = xDiv * zDiv * 6;
	vector3f *vert = new vector3f[_num];
	UV *uv = new UV[_num];
	uint count = 0;
	for (uint i = 0; i < xDiv; ++i)
	{
		for (uint j = 0; j < zDiv; ++j)
		{
			PushQuad(&(vert[count])
				, vector3f(x1 + (x2 - x1) * i / xDiv, y, z1 + (z2 - z1) * j / zDiv)
				, (x2 - x1) / xDiv
				, (z2 - z1) / zDiv
				, &(uv[count])
				, static_cast<float>(i) / xDiv
				, static_cast<float>(j) / zDiv
				, static_cast<float>(i + 1) / xDiv
				, static_cast<float>(j + 1) / zDiv);

			count += 6;
		}
	}

	glGenBuffersARB(1, &_vboVert);
 
	glBindBufferARB(GL_ARRAY_BUFFER, _vboVert);
 	glBufferDataARB(GL_ARRAY_BUFFER,  _num * sizeof(vector3f), vert, GL_STATIC_DRAW);

	glGenBuffersARB(1, &_vboUV);
 
	glBindBufferARB(GL_ARRAY_BUFFER, _vboUV);
 	glBufferDataARB(GL_ARRAY_BUFFER,  _num * sizeof(UV), uv, GL_STATIC_DRAW);

	delete [] vert;
	delete [] uv;
}

void Ocean::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY); 
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );             

	glBindBufferARB(GL_ARRAY_BUFFER, _vboVert);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBufferARB(GL_ARRAY_BUFFER, _vboUV);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	//Actually draw the triangle, giving the number of vertices provided
	glDrawArrays(GL_TRIANGLES, 0, _num);
	//glDrawArrays(GL_LINES, 0, _num * 3);

	glDisableClientState( GL_VERTEX_ARRAY );                
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );             
}