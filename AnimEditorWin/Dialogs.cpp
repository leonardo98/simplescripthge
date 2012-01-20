#include "stdafx.h"
#include "Dialogs.h"
#include "Resource.h"
#include "../Core/types.h"
#include "../Core/Render.h"
#include "../Core/Core.h"
#include "Commdlg.h"
#include "Windowsx.h"
#include "Commctrl.h"
#include "AnimEditor.h"

HWND d_main = 0;
HWND d_bone = 0;
HWND hAnimationCombo = 0;
std::string lastOpenedDir = "C:\\Projects\\MyEngineMac&Win\\AnimEditorWin\\TestFiles";
AnimEditor editor;
std::string lastAnimationId;
bool waitingNewAnimationId = false;

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

BOOL CALLBACK DialogProcBone(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) // what are we doing ?
	{ 	 
		// this messages are the heart of the dialogs 
		case WM_INITDIALOG: 
		{
		}
		break;
	}
	return 0;
}

BOOL CALLBACK DialogProcMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) // what are we doing ?
	{ 	 
		// this messages are the heart of the dialogs 
		case WM_INITDIALOG: 
		{
			InitCommonControls();
			hAnimationCombo = GetDlgItem(hWnd, IDC_ANIMATION_COMBO);
		}
		break;

		case WM_PAINT:
		{
			if (waitingNewAnimationId) {
				const int MAX = 100;
				char buffer[MAX];
				int len = ComboBox_GetText(hAnimationCombo, buffer, MAX);
				if (len && lastAnimationId != buffer) {
					editor.SetCurrent(buffer);
					lastAnimationId = buffer;
					waitingNewAnimationId = false;
				}
			}
		}
		break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) // what we pressed on?
			{ 	 
				case IDM_OPEN_XML:
				{
					std::string file;
					OPENFILENAME fn;
					fn.lStructSize = sizeof(fn);
					fn.hInstance = 0;
					fn.hwndOwner = d_main;
					fn.lpstrInitialDir = lastOpenedDir.c_str();
					fn.lpstrFilter = "Animation XML(*.XML)\0*.XML\0All(*.*)\0*.*\0";
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
						Core::Unload();
						Core::LoadAnimations(fn.lpstrFile);
						Render::SetDataDir(std::string(fn.lpstrFile) + "_files");

						std::vector<std::string> allNames;
						Core::GetAnimationsList(allNames);

						ComboBox_ResetContent(hAnimationCombo);
						for (unsigned int i = 0; i < allNames.size(); ++i) {
							ComboBox_AddString(hAnimationCombo, (LPCSTR)allNames[i].c_str());
						}
					}
				}
				break;
				
				case IDM_EXIT:
				{
					editor.exitPressed = true;
				}
				break;
				
				case IDC_ANIMATION_COMBO: 
				{
					switch (HIWORD(wParam)) 
					{
						case CBN_SELCHANGE://CBN_SELENDOK:
						{
							waitingNewAnimationId = true;
						}
						break;
					}
				}
				break;

			}
		break;
	}
	return 0;
}

void InitDialogs(HINSTANCE hInstance) {
	d_main = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DialogProcMain);
	ShowWindow(d_main, SW_SHOW); 
	d_bone = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_BONE_PROP), NULL, (DLGPROC)DialogProcBone);
	ShowWindow(d_bone, SW_SHOW); 
	SetWindowPos(d_main, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_bone, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

bool SetDialogsOnTop() {
	SetWindowPos(d_main, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_bone, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	return true;
}

void Draw() {
	editor.Draw();
}

void Update(float dt) {
	editor.Update(dt);
}

bool Exit() {
	return editor.exitPressed;
}