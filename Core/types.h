#pragma once

#include <stdio.h>
#include <string>
#include <fstream>
//#include <vector>
#include <string>
#include <list>
#include <map>

#define IOS_COMPILE_KEY

/*
[ additional include directories ]
"C:\CppLib\Box2D_v2.1.2\Box2D\";"c:/airplay sdk/update/modules/derbh/h";"c:/airplay sdk/update/modules/iwanim/h";"c:/airplay sdk/update/modules/iw2d/h";"c:/airplay sdk/update/modules/iwgraphics/h";"c:/airplay sdk/update/modules/iwutil/h";"c:/airplay sdk/update/modules/iwgeom/h";"c:/airplay sdk/update/modules/iwresmanager/h";"c:/airplay sdk/update/modules/iwgl/h";"c:/airplay sdk/update/modules/iwgx/h";"c:/airplay sdk/update/s3e/h";"c:/airplay sdk/update/s3e/h/ext";"c:/airplay sdk/update/s3e/h/ext_legacy";"c:/airplay sdk/update/s3e/h/std";"c:/airplay sdk/update/s3e/h/std/c++"
[ additional library directories ]
"c:\airplay sdk\update\modules\derbh\lib\x86";"c:\airplay sdk\update\modules\iwanim\lib\x86";"c:\airplay sdk\update\modules\iwgraphics\lib\x86";"c:\airplay sdk\update\modules\iwutil\lib\x86";"c:\airplay sdk\update\modules\iwgeom\lib\x86";"c:\airplay sdk\update\modules\iwresmanager\lib\x86";"c:\airplay sdk\update\modules\iwgx\atitools";"c:\airplay sdk\update\modules\iw2d\lib\x86";"c:\airplay sdk\update\modules\iwgx\lib\x86";"c:\airplay sdk\update\modules\iwgl\lib\x86";"c:\airplay sdk\update\s3e\lib\x86";"c:\airplay sdk\update\modules\iwgxfont\lib\x86"
[ additional dependecies ]
iwgxfont_d.lib iw2d_d.lib vc6_compat.lib derbh_d.lib iwanim_d.lib iwgraphics_d.lib iwutil_d.lib iwgeom_d.lib iwresmanager_d.lib ATI_Compress_MT_VC7.lib iwgx_d.lib iwgl_d.lib s3e_cpp_vc8_d.lib s3e_d.lib "c:/airplay sdk/update/s3e/lib/x86/s3e_api.lib"
*/

#ifdef IOS_COMPILE_KEY
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

//#pragma comment( lib , DD_Path( Lib\\x86\\DDraw.LIB ) )
//#pragma comment (compiler, "/dir:c:\projects\myprog\speex_2\include")
//#pragma comment (lib, "C:/CppLib/Box2D_v2.1.2/Box2D")


#else
extern "C" 
{
  #include "C:\CppLib\lua\include\lua.h"
  #include "C:\CppLib\lua\include\lualib.h"
  #include "C:\CppLib\lua\include\lauxlib.h"
}
	#include "hge.h"
	#include "hgesprite.h"
	#include "hgeVector.h"
	#include "hgefont.h"
	#include "tinyxml.h"
	typedef hgeVector FPoint2D;
	typedef hgeVertex Vertex;
	#define LOG(a) Render::GetDC()->System_Log((std::string("message : ") + a + "\n").c_str())
#endif //IOS_COMPILE_KEY

#define PTexture Render::Texture *

#include "Object.h"
