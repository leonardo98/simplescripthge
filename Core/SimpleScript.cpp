#include "Variables.h"
#include "Messager.h"
#include "Core.h"
#include "InputSystem.h"
#include "Interface.h"
#include "Render.h"

#ifndef HGE_COMPILE_KEY 

#include <math.h>
#include <GLES/egl.h>
#include "SimpleScriptBase.h"

// main file
//-----------------------------------------------------------------------------

#include "s3e.h"
#include "IwDebug.h"
#include "Iw2D.h"

// Attempt to lock to 30 frames per second
#define	MS_PER_FRAME (1000 / 30)

//-----------------------------------------------------------------------------
// Main global function
//-----------------------------------------------------------------------------
int main() {	

	Iw2DInit();
	{
		Interface::Init();
		// запускаем ядро
		Core core;
		core.Load("start.xml");
		InitInputEvent();
		LOG("application start");

		while (1)
		{
			s3eDeviceYield(0);
			s3eKeyboardUpdate();
			s3ePointerUpdate();

			int64 start = s3eTimerGetMs();

			bool result = true;
			core.Update(1/25.f);
			if	(
				(result == false) ||
				(s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN) ||
				(s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN) ||
				(s3eDeviceCheckQuitRequest())
				)
				break;

			// Clear the screen & draw
			Iw2DSurfaceClear(0xff2f4f2f);
			core.Draw();
			Iw2DSurfaceShow();

			// Attempt frame rate
			while ((s3eTimerGetMs() - start) < MS_PER_FRAME)
			{
				int32 yield = (int32) (MS_PER_FRAME - (s3eTimerGetMs() - start));
				if (yield<0)
					break;
				s3eDeviceYield(yield);
			}
		}
		core.Release();
		ReleaseInputEvent();
	}
	Iw2DTerminate();
	return 0;
}

#else

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
	if(Render::InitApplication(FrameFunc, RenderFunc)) {
		Interface::Init();
		// запускаем ядро
		core.Load("start.xml");
		LOG("application start");
		InitInputEvent();
		Render::RunApplication();
		core.Release();
	} else {	
		LOG(Render::Error().c_str());
	}
	Render::ExitApplication();

	return 0;
}

#endif // HGE_COMPILE_KEY