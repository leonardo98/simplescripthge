#pragma once

#include <stdio.h>
#include <string>
#include <fstream>
//#include <vector>
#include <string>
#include <list>
#include <map>

#ifndef HGE_COMPILE_KEY
	extern "C" 
	{
	  #include "C:\Airplay SDK\update\modules\third_party\lua\modified\src\lua.h"
	  #include "C:\Airplay SDK\update\modules\third_party\lua\modified\src\lualib.h"
	  #include "C:\Airplay SDK\update\modules\third_party\lua\modified\src\lauxlib.h"
	}
	#include "C:\Airplay SDK\update\modules\third_party\tinyxml\modified\tinyxml.h"
	#include "std\math.h"
	#include "FPoint2D.h"
	typedef unsigned int DWORD;
	#define LOG(a) ;//I_w_Trace(DEFAULT, a)
	#define abort_1(a) {LOG(a); assert(false); }
	#define abort_2(a, b) {LOG(a); LOG(B); assert(false); }
	typedef unsigned int UINT;
	typedef unsigned char BYTE;
	typedef int Vertex;
#else
extern "C" 
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}
	#include "hge.h"
	#include "hgesprite.h"
	#include "hgeVector.h"
	#include "hgefont.h"
	#include "tinyxml.h"
	typedef hgeVector FPoint2D;
	typedef hgeVertex Vertex;
	#define LOG(a) Render::GetDC()->System_Log((std::string("message : ") + a + "\n").c_str())
#endif //HGE_COMPILE_KEY

#define PTexture Render::Texture *

#include "Object.h"
