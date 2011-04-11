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

#ifndef IOS_COMPILE_KEY
	#include "hge.h"
	#include "hgesprite.h"
	#include "hgeVector.h"
	#include "hgefont.h"
	#include "tinyxml.h"
	typedef hgeVector FPoint2D;
	typedef hgeVertex Vertex;
	#define LOG(a) Render::GetDC()->System_Log((std::string("message : ") + a + "\n").c_str())
	#define PTexture Render::Texture *
#else
	#include "Iw2D.h"
	#define TIXML_USE_STL
	#include "C:\Airplay SDK\4.2\modules\third_party\tinyxml\modified\tinyxml.h"
	typedef unsigned int DWORD;
	typedef CIwSVec2 FPoint2D;
#endif //IOS_COMPILE_KEY

#include "Object.h"
