#include "Variables.h"
#include "Messager.h"
#include "Core.h"
#include "InputSystem.h"
#include "Interface.h"

DeviceContext dc;

Core core;

class Log
	: public Messager
{
public:
	Log() : Messager("log") {
	}
	virtual void OnMessage(const std::string &message) {
		dc->System_Log(("message : " + message + "\n").c_str());
	}
};

bool RenderFunc()
{
	dc->Gfx_BeginScene();
	dc->Gfx_Clear(0);
	if (core.GetDC()) {
		core.Draw();
	}
	dc->Gfx_EndScene();
	return false;
}

bool FrameFunc()
{
	float dt = dc->Timer_GetDelta();
	core.Update(dt);
	return CheckForInputEvent(dc, dt);
}

void InitApplication_WIN()
{
	dc = hgeCreate(HGE_VERSION);
	dc->System_SetState(HGE_INIFILE, "settings.ini");
	dc->System_SetState(HGE_LOGFILE, dc->Ini_GetString("system", "logfile", "log.txt"));
	dc->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	dc->System_SetState(HGE_RENDERFUNC, RenderFunc);
	dc->System_SetState(HGE_WINDOWED, dc->Ini_GetInt("system", "fullscreen", 0) == 0);
	dc->System_SetState(HGE_SCREENWIDTH, dc->Ini_GetInt("system", "width", 800));
	dc->System_SetState(HGE_SCREENHEIGHT, dc->Ini_GetInt("system", "height", 600));
	dc->System_SetState(HGE_SCREENBPP, 32);
	dc->System_SetState(HGE_FPS, dc->Ini_GetInt("system", "vsync", 0) == 0?dc->Ini_GetInt("system", "fps", 0):HGEFPS_VSYNC);	
	dc->System_SetState(HGE_USESOUND, false);
	dc->System_SetState(HGE_SHOWSPLASH, false);
	dc->System_SetState(HGE_HIDEMOUSE, false);
	dc->System_SetState(HGE_ZBUFFER, false);
	dc->System_SetState(HGE_TITLE, "Simple script HGE application");
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc)
{
	// инициализируем HGE
	InitApplication_WIN();
	if(dc->System_Initiate()) {
		Log log;
		Interface::Init(dc);
		// запускаем ядро
		core.Load("start.xml");
		core.SetDC(dc);
		dc->System_Log("hge start");
		InitInputEvent();
		dc->System_Start();
		Interface::Release();
	} else {	
		MessageBox(NULL, dc->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}
	core.Release();
	dc->System_Shutdown();
	dc->Release();

	return 0;
}
