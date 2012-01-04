#ifndef MYENGINE_TYPES_H
#define MYENGINE_TYPES_H

#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>

extern "C" 
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

#ifdef MY_ENGINE_IOS
#define _IOS
#else
#define _HGE_TARGET_OSX_
#endif

#include "hge.h"
#include "hgesprite.h"
#include "hgeVector.h"
#include "hgefont.h"

#include "tinyxml.h"
typedef hgeVector FPoint2D;
typedef hgeVertex Vertex;
#define LOG(a) Render::GetDC()->System_Log((std::string("message : ") + a + "\n").c_str());

//#undef assert
//#define assert(exp) if (!(exp)) {int j = 0; j = 1 / j;}

#include "Object.h"

#endif//MYENGINE_TYPES_H