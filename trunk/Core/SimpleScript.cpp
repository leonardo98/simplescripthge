#include "Variables.h"
#include "Messager.h"
#include "Core.h"
#include "InputSystem.h"
#include "Interface.h"

HGE *hge;

Core core;

class Log
	: public Messager
{
public:
	Log() : Messager("log") {
	}
	virtual void OnMessage(std::string message) {
		hge->System_Log(("message : " + message + "\n").c_str());
	}
};

bool RenderFunc()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	core.Draw();
	hge->Gfx_EndScene();
	return false;
}

bool FrameFunc()
{
	core.Update(hge->Timer_GetDelta());
	return CheckForInputEvent(hge);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc)
{
	// инициализируем HGE
	hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_INIFILE, "settings.ini");
	hge->System_SetState(HGE_LOGFILE, hge->Ini_GetString("system", "logfile", "log.txt"));
	hge->System_SetState(HGE_WINDOWED, hge->Ini_GetInt("system", "fullscreen", 0) == 0);
	hge->System_SetState(HGE_SCREENWIDTH, hge->Ini_GetInt("system", "width", 800));
	hge->System_SetState(HGE_SCREENHEIGHT, hge->Ini_GetInt("system", "height", 600));
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_FPS, hge->Ini_GetInt("system", "vsync", 0) == 0?0:HGEFPS_VSYNC);	
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_SHOWSPLASH, false);
	hge->System_SetState(HGE_HIDEMOUSE, false);
	hge->System_SetState(HGE_ZBUFFER, false);
	hge->System_SetState(HGE_TITLE, "Simple script HGE application");

	if(hge->System_Initiate()) {
		Log log;
		Interface::Init(hge);
		// запускаем ядро
		core.Load("start.xml");
		core.SetDC(hge);
		hge->System_Log("this string starts hge");
		hge->System_Start();
		Interface::Release();
	} else {	
		MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}
	core.Release();
	hge->System_Shutdown();
	hge->Release();

	return 0;
}

