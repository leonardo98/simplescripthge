/*
   Class Name:

      CCubeMap.

   Created by:

      Allen Sherrod (Programming Ace of www.UltimateGameProgramming.com).

   Description:

      This class loads 6 textures into a OpenGL cube map.
*/


#ifndef CCUBE_MAP_H
#define CCUBE_MAP_H


#include"CTGALoader.h"


class CCubeMap
{
   public:
      CCubeMap();
      ~CCubeMap();

      bool LoadTGAFile(char *fPosX, char *fNegX,
                       char *fPosY, char *fNegY,
                       char *fPosZ, char *fNegZ);

      void FreeImage();                   // Delete a image.

      unsigned int imageWidth;            // Width of a texture.
      unsigned int imageHeight;           // Height of a texture.
      unsigned int type;

      CTGALoader cubeMapPosX, cubeMapNegX,
               cubeMapPosY, cubeMapNegY,
               cubeMapPosZ, cubeMapNegZ;

      unsigned int ID;                    // ID used for generating the textures in OpenGl.
};

#endif


// Copyright October 2004
// All Rights Reserved!
// Allen Sherrod
// ProgrammingAce@UltimateGameProgramming.com
// www.UltimateGameProgramming.com