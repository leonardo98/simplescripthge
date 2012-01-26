#include "stdafx.h"
#include "MainDialog.h"
#include "Resource.h"
#include "Commdlg.h"
#include "../Core/Core.h"

std::string MainDialog::_currentFile;
std::string MainDialog::CurrentFile() {
	return _currentFile;
}


MainDialog::MainDialog(HWND hWnd) 
: _animationList(hWnd, IDC_ANIMATION_COMBO)
, _bonesTree(hWnd, IDC_ANIMATION_TREE)
, _pivotX(hWnd, IDC_EDIT1)
, _pivotY(hWnd, IDC_EDIT2)
, _time(hWnd, IDC_EDIT3)
, _loop(hWnd, IDC_CHECKBOX1)
, _hWnd(hWnd)
{
	_lastOpenedDir = "C:\\Projects\\MyEngineMac&Win\\AnimEditorWin\\TestFiles";
	exitPressed = false;
	waitingNewAnimationId = false;
	updateAnimationState = false;
	_currentFile = "";
}

std::string MainDialog::CutFileName(const std::string &filePath) {
	std::string result;
	std::string::size_type index = filePath.find("\\");
	std::string::size_type last = std::string::npos;
	while (index != std::string::npos) {
		last = index;
		index = filePath.find("\\", index + 1);
	}
	if (last == std::string::npos) {
		return "";
	}
	return filePath.substr(0, last);
} 

void MainDialog::OnCommand(HWND hWnd, int controlID, int command) {
    switch (controlID)
    {
        //case IDC_EDIT:
        //    if (_edit.IsChanged(command))
        //    {
        //        if (_edit.GetLength())
        //            _clear.Enable();
        //        else
        //            _clear.Disable();
        //    }
        //    break;
		case IDM_OPEN_XML:
		{
			std::string file;
			OPENFILENAME fn;
			fn.lStructSize = sizeof(fn);
			fn.hInstance = 0;
			fn.hwndOwner = _hWnd;
			fn.lpstrInitialDir = _lastOpenedDir.c_str();
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
				_lastOpenedDir = CutFileName(fn.lpstrFile);
				Core::Unload();
				Core::LoadAnimations(fn.lpstrFile);
				_currentFile = fn.lpstrFile;
				Render::SetDataDir(std::string(fn.lpstrFile) + "_files");

				std::vector<std::string> allNames;
				Core::GetAnimationsList(allNames);

				_animationList.Reset();
				for (unsigned int i = 0; i < allNames.size(); ++i) {
					_animationList.AddString((LPCSTR)allNames[i].c_str());
				}
			}
		}
		break;
		
		case IDM_EXIT:
		{
			exitPressed = true;
		}
		break;
		
		case IDC_ANIMATION_COMBO: 
		{
			waitingNewAnimationId = true;
		}
		break;

		case IDC_EDIT1: 
			if (_pivotX.IsChanged(command)) {
				updateAnimationState = true;
			}
		break;

		case IDC_EDIT2: 
			if (_pivotY.IsChanged(command)) {
				updateAnimationState = true;
			}
		break;

		case IDC_EDIT3: 
			if (_time.IsChanged(command)) {
				updateAnimationState = true;
			}
		break;

		//case IDC_ANIMATION_TREE: 
		//	if (_bonesTree.ChangeS IsVisible(command)) {
		//		updateAnimationState = true;
		//	}
		//break;

	}
}
