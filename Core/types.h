#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>

extern "C" 
{
  #include "C:\CppLib\lua\include\lua.h"
  #include "C:\CppLib\lua\include\lualib.h"
  #include "C:\CppLib\lua\include\lauxlib.h"
}

#define IOS_COMPILE_KEY

#include "hge.h"
#include "hgesprite.h"
#include "hgeVector.h"
#include "hgefont.h"
//#include "hgeparticle.h"

#include "tinyxml.h"
#include "tinystr.h"

#include "Object.h"
#include "Texture.h"

#pragma 

//typedef hgeSprite Texture;

#define LOG_NOTE 1
#define LOG_STRING(note, msg) Messager::SendMessage("Core", msg)

typedef hgeVector FPoint2D;
typedef HGE * DeviceContext;