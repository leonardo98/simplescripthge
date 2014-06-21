#include "main.h"

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

	if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_DI, NULL)))
	{
		return 0;
	}

	g_DI->CreateDevice(GUID_SysKeyboard, &g_KDIDev, NULL);
	g_KDIDev->SetDataFormat(&c_dfDIKeyboard);
	g_KDIDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	g_KDIDev->Acquire();
	DIPROPDWORD  dipdw; 
	dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwObj = 0; 
	dipdw.diph.dwHow = DIPH_DEVICE; 
	dipdw.dwData = 10; 
	g_KDIDev->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph); 

	g_DI->CreateDevice(GUID_SysMouse, &g_pMouse, NULL);
	g_pMouse->SetDataFormat(&c_dfDIMouse2);
	g_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	g_pMouse->Acquire();

	
//	g_DI->EnumDevices(DI8DEVCLASS_GAMECTRL, DIEnumDevicesCallback, NULL, DIEDFL_ATTACHEDONLY);

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
	lastdims = dims;

	g_pMouse->Acquire();
	g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &dims);
	g_KDIDev->Acquire();
	g_KDIDev->GetDeviceState(sizeof(keybuffer), &keybuffer);
//	g_Joy->Acquire();
//	g_Joy->Poll();
//	g_Joy->GetDeviceState(sizeof(DIJOYSTATE), &dijs);

	mousex += dims.lX;
	mousey += dims.lY;
	if (mousex >= wsizex)
		mousex = wsizex - 1;
	else if (mousex < 0)
		mousex = 0;
	if (mousey >= wsizey)
		mousey = wsizey - 1;
	else if (mousey < 0)
		mousey = 0;

	camera.rot.v[0] -= dims.lY * .1;
	camera.rot.v[1] -= dims.lX * .1;

	float xzscale = cosf(camera.rot.v[0] * PIOVER180);

	float speed = 50;
	if (keybuffer[DIK_W] & 0x80)
	{
		camera.pos.v[0] -= sinf(camera.rot.v[1] * PIOVER180) * speed * frametime * xzscale;
		camera.pos.v[1] += sinf(camera.rot.v[0] * PIOVER180) * speed * frametime;
		camera.pos.v[2] -= cosf(camera.rot.v[1] * PIOVER180) * speed * frametime * xzscale;
	}
	if (keybuffer[DIK_S] & 0x80)
	{
		camera.pos.v[0] += sinf(camera.rot.v[1] * PIOVER180) * speed * frametime;
		camera.pos.v[1] -= sinf(camera.rot.v[0] * PIOVER180) * speed * frametime;
		camera.pos.v[2] += cosf(camera.rot.v[1] * PIOVER180) * speed * frametime;
	}
	if (keybuffer[DIK_A] & 0x80)
	{
		camera.pos.v[0] -= sinf((camera.rot.v[1] + 90) * PIOVER180) * speed * frametime;
		camera.pos.v[2] -= cosf((camera.rot.v[1] + 90) * PIOVER180) * speed * frametime;
	}
	if (keybuffer[DIK_D] & 0x80)
	{
		camera.pos.v[0] -= sinf((camera.rot.v[1] - 90) * PIOVER180) * speed * frametime;
		camera.pos.v[2] -= cosf((camera.rot.v[1] - 90) * PIOVER180) * speed * frametime;
	}
	if (keybuffer[DIK_LCONTROL] & 0x80)
	{
		camera.pos.v[1] -= speed * frametime;
	}
	if (keybuffer[DIK_SPACE] & 0x80)
	{
		camera.pos.v[1] += speed * frametime;
	}
}