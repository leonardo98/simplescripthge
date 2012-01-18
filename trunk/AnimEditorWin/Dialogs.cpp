#include "stdafx.h"
#include "Dialogs.h"
#include "Resource.h"
#include "../Core/types.h"
#include "../Core/Render.h"
#include "../Core/Core.h"
#include "Commdlg.h"

HWND d_main = 0;
HWND d_animation = 0;
HWND d_bone = 0;
std::string lastOpenedDir = "c:\\";

std::string CutFileName(std::string filePath) {
	std::string result;
	std::string::size_type index = filePath.find("\\");
	while (index != std::string::npos) {
		result += filePath.substr(0, index + 1);
		filePath = filePath.substr(index + 1);
		index = filePath.find("\\");
	}
	return result;
} 

BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) // what are we doing ?
	{ 	 
		// this messages are the heart of the dialogs 
		case WM_COMMAND:
			switch (LOWORD(wParam)) // what we pressed on?
			{ 	 
				case IDM_OPEN_XML:
				{
					const char *mask = "*.xml";
					std::string file;
					OPENFILENAME fn;
					fn.lStructSize = sizeof(fn);
					fn.hInstance = 0;
					fn.hwndOwner = d_main;
					fn.lpstrInitialDir = lastOpenedDir.c_str();
					fn.lpstrFilter = mask;
					fn.nFilterIndex = 0;
					fn.lpstrFile = new char[1024];
					fn.lpstrFile[0] = 0;
					fn.nMaxFile = 1024 - 1;
					fn.lpstrFileTitle = NULL;
					fn.nMaxFileTitle = NULL;
					fn.lpstrTitle = NULL;
					fn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
					fn.lpstrCustomFilter = NULL;
					fn.nMaxCustFilter = 0;
					fn.nFileOffset = 0;
					fn.nFileExtension = 0;
					fn.lpstrDefExt = NULL;
					fn.lCustData = NULL;
					fn.lpfnHook = NULL;
					fn.FlagsEx = 0;
					fn.lpTemplateName = "";
					fn.dwReserved = 0;
					fn.pvReserved = 0;
					if (GetOpenFileName(&fn)) {
						lastOpenedDir = CutFileName(fn.lpstrFile);
						Core::LoadAnimations(fn.lpstrFile);
					}
				}
				break;
			}
		break;
	}
	return 0;
}

void InitDialogs(HINSTANCE hInstance) {
	d_main = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DialogProc);
	d_animation = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_ANIMATION_PROP), NULL, (DLGPROC)DialogProc);
	d_bone = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_BONE_PROP), NULL, (DLGPROC)DialogProc);
	SetWindowPos(d_main, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_animation, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_bone, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

bool SetDialogsOnTop() {
	SetWindowPos(d_main, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_animation, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_bone, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	return true;
}