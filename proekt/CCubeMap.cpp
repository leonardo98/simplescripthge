/*
   Class Name:

      CCubeMap.

   Created by:

      Allen Sherrod (Programming Ace of www.UltimateGameProgramming.com).

   Description:

      This class loads 6 textures into a OpenGL cube map.
*/


#include"CCubeMap.h"
#include<windows.h>           // The windows header file.
#include<glew.h>             // Standard opengl include.
#include<gl.h>             // Standard opengl include.
#include<glu.h>            // Opengl utilities.
#include<glext.h>          // Gl extensions.


CCubeMap::CCubeMap()
{
   ID = -1;
}


CCubeMap::~CCubeMap()
{
   FreeImage();
}


bool CCubeMap::LoadTGAFile(char *fPosX, char *fNegX, char *fPosY, char *fNegY, char *fPosZ, char *fNegZ)
{
   FreeImage();

   if(!cubeMapPosX.LoadTGA(fPosX) || !cubeMapNegX.LoadTGA(fNegX) ||
      !cubeMapPosY.LoadTGA(fPosY) || !cubeMapNegY.LoadTGA(fNegY) ||
      !cubeMapPosZ.LoadTGA(fPosZ) || !cubeMapNegZ.LoadTGA(fNegZ))
      return false;


	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, ID);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
   int type = cubeMapPosX.type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, type, cubeMapPosX.imageWidth, cubeMapPosX.imageHeight,
                     type, GL_UNSIGNED_BYTE, cubeMapNegX.image);
	
   type = cubeMapNegX.type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, type, cubeMapNegX.imageWidth, cubeMapNegX.imageHeight,
                     type, GL_UNSIGNED_BYTE, cubeMapNegX.image);

	type = cubeMapPosY.type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, type, cubeMapPosY.imageWidth, cubeMapPosY.imageHeight,
                     type, GL_UNSIGNED_BYTE, cubeMapPosY.image);
	
   type = cubeMapNegY.type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, type, cubeMapNegY.imageWidth, cubeMapNegY.imageHeight,
                     type, GL_UNSIGNED_BYTE, cubeMapNegY.image);
	
	type = cubeMapPosZ.type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, type, cubeMapPosZ.imageWidth, cubeMapPosZ.imageHeight,
                     type, GL_UNSIGNED_BYTE, cubeMapPosZ.image);
	
   type = cubeMapNegZ.type == 3 ? GL_RGB : GL_RGBA;
   gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, type, cubeMapNegZ.imageWidth, cubeMapNegZ.imageHeight,
                     type, GL_UNSIGNED_BYTE, cubeMapNegZ.image);
	
   glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   cubeMapPosX.FreeImage(); cubeMapNegX.FreeImage();
   cubeMapPosY.FreeImage(); cubeMapNegY.FreeImage();
   cubeMapPosZ.FreeImage(); cubeMapNegZ.FreeImage();

   return true;
}


void CCubeMap::FreeImage()
{
   glDeleteTextures(1, &ID);
   ID = -1;
}


// Copyright October 2004
// All Rights Reserved!
// Allen Sherrod
// ProgrammingAce@UltimateGameProgramming.com
// www.UltimateGameProgramming.com