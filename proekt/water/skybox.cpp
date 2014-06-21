#include "skybox.h"

namespace SkyBox
{

bool		active = true;

int			fullscreen = false;

Timer		timer;
float		frametime = 0;
float		elapsedtime = 0;

float		averagefps = 0;
float		fps[FPS_FRAMES];

Camera		camera;

DEVMODE dmOriginalScreenSettings;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_ACTIVATE:
		{
			if (!HIWORD(wParam))
			{
				active = true;
			}
			else
			{
				active = false;
			}

			return 0;
		}

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE && lParam >> 30 == 0)
			{
				PostMessage(hWnd, WM_QUIT, 0, 0);
				return 0;
			}
			return 0;
			

		case WM_SIZE:
		{
			Resize(LOWORD(lParam), HIWORD(lParam), 0, 0, 60);
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Shutdown()
{
	ShutdownRender();

	if (fullscreen)
	{
		ChangeDisplaySettings(&dmOriginalScreenSettings, 0);
	}

	DeleteFont();

	ShutdownInput();
}

void Startup()
{
	timer.init();

	sgenrand(time(NULL));

	Resize(wsizex, wsizey, 0, 0, 60);

	SetupInput();

	SetupRender();
}

//int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//	timer.init();
//	
//	sgenrand(time(NULL));
//
//	hInstance = hInst;
//	MSG msg;
//
//
//	Startup();
//	InitExtensions();
//
//
//	while (1)
//	{
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			if (msg.message==WM_QUIT)
//			{
//				break;
//			}
//			else
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//		}
//		else
//		{
//			if (active)
//			{
//				frametime = timer.frametime();
//				elapsedtime = timer.getelapsed();
//				if (frametime > .1)
//					frametime = .1;
//
//
//				for (int n = 0; n < FPS_FRAMES - 1; n++)
//					fps[n] = fps[n + 1];
//				fps[FPS_FRAMES - 1] = 1 / frametime;
//				averagefps = 0;
//				for (int n = 0; n < FPS_FRAMES; n++)
//					averagefps += fps[n];
//				averagefps /= FPS_FRAMES;
//
//				char title[256];
//
////				sprintf(title, "Sky Thing   %.0f", averagefps);
////				SetWindowText(hWnd, title);
//				Input();
//				SwapBuffers(hDC);
//				Render();
//			}
//		}
//	}
//
//shutdown:
//
//	Shutdown();
//
//	return (msg.wParam);
//}

}