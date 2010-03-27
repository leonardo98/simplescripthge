// SimpleScript.cpp : Defines the entry point for the console application.
#include "Variables.h"
#include "Messager.h"
#include "Core.h"
#include "InputSystem.h"

HGE *hge;

Core core;


class Log
	: public Messager
{
public:
	Log() : Messager("log") {
	}
	virtual void OnMessage(std::string message) {
		hge->System_Log(("message : " + Variables::Get("$second") + message + "\n").c_str());
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
	hge->System_SetState(HGE_LOGFILE, "log.txt");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, 800);
	hge->System_SetState(HGE_SCREENHEIGHT, 600);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_SHOWSPLASH, false);
	hge->System_SetState(HGE_HIDEMOUSE, false);
	hge->System_SetState(HGE_TITLE, "Simple script HGE application");

	if(hge->System_Initiate()) {
		Log log;
		// запускаем ядро
		core.Load("start.xml");
		hge->System_Log("hge start");
		hge->System_Start();
	} else {	
		MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}
	InputSystem::Release();
	core.Release();
	hge->System_Shutdown();
	hge->Release();

	return 0;
}

