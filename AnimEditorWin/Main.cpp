#include "stdafx.h"

#include "Dialogs.h"

#include "Core/Render.h"
#include "Core/Messager.h"
#include "Core/Core.h"
#include "Core/Variables.h"
#include "Core/InputSystem.h"
#include "Core/Interface.h"
#include "AnimEditor.h"

float TimeMul = 1.f;

DWORD BACKGROUND_FILL = 0xFFFFFFFF;

bool RenderFunc()
{
	Render::GetDC()->Gfx_BeginScene();
	Render::GetDC()->Gfx_Clear(BACKGROUND_FILL);
	if (Render::GetDC()) {
		Core::Draw();
		Draw();
	}
	char buff[20];
	sprintf_s(buff, "%f", TimeMul);
	Render::PrintString(100, 10, "", buff);
	Render::GetDC()->Gfx_EndScene();
	return false;
}

bool FrameFunc()
{
	if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT)) {
		if (Render::GetDC()->Input_KeyDown(HGEK_UP)) {
			TimeMul *= 1.1f;
		} else if (Render::GetDC()->Input_KeyDown(HGEK_DOWN)) {
			TimeMul /= 1.1f;
		}
	}
	float dt = Render::GetDC()->Timer_GetDelta() * TimeMul;
	Core::Update(dt);
	Update(dt);
	return InputSystem::CheckForEvent(dt) || Exit();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR argv, int argc)
{
	// инициализируем 
	if(Render::InitApplication(FrameFunc, RenderFunc)) {
		Render::GetDC()->System_SetState(HGE_FOCUSGAINFUNC, SetDialogsOnTop);
		HWND h = InitDialogs(hInstance);
		Render::GetDC()->System_SetState(HGE_HWNDPARENT, h);
		LOG(argv);
		Interface::Init();
		BACKGROUND_FILL = Render::IniFileGetUnsignedInt("system", "background", BACKGROUND_FILL);
		// запускаем ядро
		Core::Init();
		LOG("application start");
		InputSystem::Init();

		MSG  msg;
		DWORD time1 = GetTickCount();
		DWORD time2;
		float dt = 0.f;
		while (!Exit())
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && !IsDialogMessage (h, & msg)) {
				TranslateMessage ( & msg );
				if(msg.message == WM_DESTROY
					|| msg.message == WM_CLOSE
					|| msg.message == WM_QUIT) {
					Exit(true);
				}
				DispatchMessage ( & msg );
			}
			if(Render::GetDC()->System_GetState(HGE_HWND)) {
				time2 = GetTickCount();
				dt += (time2 - time1) / 1000.f;
				time1 = time2;
				if (dt > 1.f / 50) {
					Core::Update(dt);
					Update(dt);
					RenderFunc();
					dt = 0.f;
				}
			}
			Sleep(1);
		}
		CloseDialogs();
		Core::Release();
	} else {	
		LOG(Render::Error().c_str());
	}
	Render::ExitApplication();
	return 0;
}
