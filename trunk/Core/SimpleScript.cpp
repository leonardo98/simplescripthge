#include "Variables.h"
#include "Messager.h"
#include "Core.h"
#include "InputSystem.h"
#include "Interface.h"
#include "Render.h"

float TimeMul = 1.f;

DWORD BACKGROUND_FILL = 0xFF7F7F7F;

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

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc)
{
	printf("SimpleScriptHGE Start");
	if (argc != 1) {
		printf(argv);
		printf("usage: SimpleScriptHGE <dir>");
		return (-13);
	}
	// инициализируем 
	Render::SetDataDir(argv);
	if(Render::InitApplication(FrameFunc, RenderFunc)) {
		LOG(argv);
		Interface::Init();
		//BACKGROUND_FILL = Render::IniFileGetUnsignedInt("system", "background", BACKGROUND_FILL);
		// запускаем ядро
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
