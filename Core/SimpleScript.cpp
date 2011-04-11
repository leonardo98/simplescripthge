#include "Variables.h"
#include "Messager.h"
#include "Core.h"
#include "InputSystem.h"
#include "Interface.h"
#include "Render.h"

Core core;

bool RenderFunc()
{
	Render::GetDC()->Gfx_BeginScene();
	Render::GetDC()->Gfx_Clear(0);
	if (Render::GetDC()) {
		core.Draw();
	}
	Render::GetDC()->Gfx_EndScene();
	return false;
}

bool FrameFunc()
{
	float dt = Render::GetDC()->Timer_GetDelta();
	core.Update(dt);
	return CheckForInputEvent(dt);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc)
{
	// инициализируем 
	Render::InitApplication(FrameFunc, RenderFunc);
	if(Render::GetDC()->System_Initiate()) {
		Interface::Init();
		// запускаем ядро
		core.Load("start.xml");
		Render::GetDC()->System_Log("application start");
		InitInputEvent();
		Render::GetDC()->System_Start();
	} else {	
		Render::ShowMessage(Render::GetDC()->System_GetErrorMessage(), "Error");
	}
	core.Release();
	Render::GetDC()->System_Shutdown();
	Render::GetDC()->Release();

	return 0;
}
