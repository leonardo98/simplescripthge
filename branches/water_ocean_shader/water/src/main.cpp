#include "main.h"

HINSTANCE	hInstance = NULL;
HWND		hWnd = NULL;
HGLRC		hRC = NULL;
HDC			hDC = NULL;

bool		active = true;

int			fullscreen = false;

int			wsizex = 800, wsizey = 600, wbitdepth = 32;

Timer		timer;
float		frametime = 0;
float		elapsedtime = 0;

float		averagefps = 0;
float		fps[FPS_FRAMES];

FILE		*logger;

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

	if (hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		hRC = NULL;
	}
	if (hDC)
		ReleaseDC(hWnd, hDC);
	if (hWnd)
		DestroyWindow(hWnd);
	UnregisterClass("Viewer", hInstance);

	ShutdownInput();
}

void Startup()
{
	unsigned int pixelformat;
	PIXELFORMATDESCRIPTOR pfd;
	RECT window;
	DWORD style = WS_OVERLAPPEDWINDOW, exstyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	WNDCLASS wc;

	hInstance = GetModuleHandle(NULL);
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "Sky";

	RegisterClass(&wc);

	if (fullscreen)
	{
		memset(&dmOriginalScreenSettings, 0, sizeof(DEVMODE));
		dmOriginalScreenSettings.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmOriginalScreenSettings);

		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(DEVMODE));
		dmScreenSettings.dmSize = sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth = wsizex;
		dmScreenSettings.dmPelsHeight = wsizey;
		dmScreenSettings.dmBitsPerPel = wbitdepth;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		exstyle = WS_EX_APPWINDOW;
		style = WS_POPUP;
	}

	window.bottom = wsizey;
	window.top = 0;
	window.left = 0;
	window.right = wsizex;

	AdjustWindowRectEx(&window, style, false, exstyle);
	hWnd = CreateWindowEx(exstyle, "Sky", "Sky", style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, window.right - window.left, window.bottom - window.top, NULL, NULL, hInstance, NULL);
	hDC = GetDC(hWnd);

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = wbitdepth;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;

	pixelformat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelformat, &pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	ShowWindow(hWnd, SW_SHOW);

	Resize(wsizex, wsizey, 0, 0, 60);

	SetupInput();

	SetupRender();
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	timer.init();

	logger = fopen("log.txt", "wt");
	
	sgenrand(time(NULL));

	hInstance = hInst;
	MSG msg;


	Startup();
	InitExtensions();


	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (active)
			{
				frametime = timer.frametime();
				elapsedtime = timer.getelapsed();
				if (frametime > .1)
					frametime = .1;


				for (int n = 0; n < FPS_FRAMES - 1; n++)
					fps[n] = fps[n + 1];
				fps[FPS_FRAMES - 1] = 1 / frametime;
				averagefps = 0;
				for (int n = 0; n < FPS_FRAMES; n++)
					averagefps += fps[n];
				averagefps /= FPS_FRAMES;

				char title[256];

//				sprintf(title, "Sky Thing   %.0f", averagefps);
//				SetWindowText(hWnd, title);
				Input();
				SwapBuffers(hDC);
				Render();
			}
		}
	}

shutdown:

	Shutdown();

	fclose(logger);

	return (msg.wParam);
}