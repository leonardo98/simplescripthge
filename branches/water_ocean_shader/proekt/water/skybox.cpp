#include "skybox.h"

namespace SkyBox
{

bool		active = true;

//int			fullscreen = false;

Timer		timer;
float		frametime = 0;
float		elapsedtime = 0;

float		averagefps = 0;
float		fps[FPS_FRAMES];

Camera		camera;

//DEVMODE dmOriginalScreenSettings;

//LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg)
//	{
//		case WM_ACTIVATE:
//		{
//			if (!HIWORD(wParam))
//			{
//				active = true;
//			}
//			else
//			{
//				active = false;
//			}
//
//			return 0;
//		}
//
//		case WM_SYSCOMMAND:
//		{
//			switch (wParam)
//			{
//				case SC_SCREENSAVE:
//				case SC_MONITORPOWER:
//				return 0;
//			}
//			break;
//		}
//
//		case WM_CLOSE:
//		{
//			PostQuitMessage(0);
//			return 0;
//		}
//
//		case WM_KEYDOWN:
//			if (wParam == VK_ESCAPE && lParam >> 30 == 0)
//			{
//				PostMessage(hWnd, WM_QUIT, 0, 0);
//				return 0;
//			}
//			return 0;
//			
//
//		case WM_SIZE:
//		{
//			Resize(LOWORD(lParam), HIWORD(lParam), 0, 0, 60);
//			return 0;
//		}
//	}
//
//	return DefWindowProc(hWnd, uMsg, wParam, lParam);
//}
//
void Shutdown()
{
	ShutdownRender();

	//if (fullscreen)
	//{
	//	ChangeDisplaySettings(&dmOriginalScreenSettings, 0);
	//}

	//DeleteFont();

}

void Startup()
{
	timer.init();

	sgenrand(time(NULL));

	Resize(wsizex, wsizey, 0, 0, 60);

	SetupRender();
}


}