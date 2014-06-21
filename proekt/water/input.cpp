#include "skybox.h"

namespace SkyBox
{


LPDIRECTINPUT8			g_DI;
LPDIRECTINPUTDEVICE8	g_KDIDev;
LPDIRECTINPUTDEVICE8	g_pMouse;
//LPDIRECTINPUTDEVICE8	g_Joy;
DIMOUSESTATE2			dims;
DIMOUSESTATE2			lastdims;
DIJOYSTATE				dijs;
BYTE					keybuffer[256];
BYTE					keybuffer2[256];

float					mousex = wsizex / 2, mousey = wsizey / 2;

/*BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	g_DI->CreateDevice(lpddi->guidInstance, &g_Joy, NULL);
	g_Joy->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	g_Joy->SetDataFormat(&c_dfDIJoystick);
	g_Joy->Acquire();

	return DIENUM_STOP;
}*/

int SetupInput()
{
	memset(keybuffer2, 0, 256);

//	if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_DI, NULL)))
	{
		return 0;
	}

	//g_DI->CreateDevice(GUID_SysKeyboard, &g_KDIDev, NULL);
	//g_KDIDev->SetDataFormat(&c_dfDIKeyboard);
	//g_KDIDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//g_KDIDev->Acquire();
	//DIPROPDWORD  dipdw; 
	//dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
	//dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	//dipdw.diph.dwObj = 0; 
	//dipdw.diph.dwHow = DIPH_DEVICE; 
	//dipdw.dwData = 10; 
	//g_KDIDev->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph); 

	//g_DI->CreateDevice(GUID_SysMouse, &g_pMouse, NULL);
	//g_pMouse->SetDataFormat(&c_dfDIMouse2);
	//g_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	//g_pMouse->Acquire();

    return 1;
}

void ShutdownInput()
{
    if (g_DI)
    {
        if (g_KDIDev)
        {
            g_KDIDev->Unacquire();
            g_KDIDev->Release();
            g_KDIDev = NULL;
        }
		if (g_pMouse)
		{
			g_pMouse->Unacquire();
			g_pMouse->Release();
			g_pMouse = NULL;
		}
        g_DI->Release();
        g_DI = NULL;
    }
}

void Input()
{

}

}