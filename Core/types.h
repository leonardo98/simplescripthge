#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>

extern "C" 
{
  #include "../../CppLib/lua/include/lua.h"
  #include "../../CppLib/lua/include/lualib.h"
  #include "../../CppLib/lua/include/lauxlib.h"
}

#ifdef MY_ENGINE_IOS
#define _IOS
#else
#define _HGE_TARGET_OSX_
#endif

#include "../../hge181-for-osx/trunk/include/hge.h"
#include "../../hge181-for-osx/trunk/include/hgesprite.h"
#include "../../hge181-for-osx/trunk/include/hgeVector.h"
#include "../../hge181-for-osx/trunk/include/hgefont.h"

#include "../../CppLib/tinyxml/tinyxml.h"
typedef hgeVector FPoint2D;
typedef hgeVertex Vertex;
#define LOG(a) Render::GetDC()->System_Log((std::string("message : ") + a + "\n").c_str())

#undef assert
#define assert(exp) if (!(exp)) {int j = 0; j = 1 / j;}

#include "Object.h"

#endif//TYPES_H