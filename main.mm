#include <iostream>

#import <Cocoa/Cocoa.h>

#define _HGE_TARGET_OSX_

#include <math.h>

#include "Core/types.h"

#include "Core/Variables.h"
#include "Core/Messager.h"
#include "Core/Core.h"
#include "Core/InputSystem.h"
#include "Core/Interface.h"
#include "Core/Render.h"

float TimeMul = 1.f;

DWORD BACKGROUND_FILL = 0xFFFFFFFF;

bool RenderFunc()
{
	Render::GetDC()->Gfx_BeginScene();
	Render::GetDC()->Gfx_Clear(BACKGROUND_FILL);
	if (Render::GetDC()) {
		Core::Draw();
	}
	char buff[20];
	sprintf(buff, "%f", TimeMul);
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
	return InputSystem::CheckForEvent(dt);
}


int main (int argc, char * const argv[])
{
    
#define PROJECT_PATH "bin/giperion"
    
	if ( Render::InitApplication(FrameFunc, RenderFunc, PROJECT_PATH) ) {
		//LOG(argv);
		Interface::Init();
		BACKGROUND_FILL = Render::IniFileGetUnsignedInt("system", "background", BACKGROUND_FILL);

		Core::Init();
		Core::Load("start.xml");
		LOG("application start");
		InputSystem::Init();
		Render::RunApplication();
		Core::Release();
	} else {	
		LOG(Render::Error().c_str());
	}
	Render::ExitApplication();
    
	return 0;
}
