#include "stdafx.h"
#include "BonePropDialog.h"
#include "Resource.h"
#include "Commdlg.h"
#include "MainDialog.h"


BonePropDialog::BonePropDialog(HWND hWnd)
: _order(hWnd, IDC_COMBO_ORDER)
, _movingType(hWnd, IDC_COMBO_MOVINGTYPE)
, _pivotX(hWnd, IDC_EDIT_X)
, _pivotY(hWnd, IDC_EDIT_Y)
, _boneSprite(hWnd, IDC_LOAD_IMAGE)
, after("draw AFTER the parent")
, before("draw BEFORE the parent")
, spline("spline path")
, discontinuous("discontinuous")
, loadimage("<load image...>")
, _hWnd(hWnd)
{
	_boneSprite.SetName(loadimage);

	_order.AddString(before);
	_order.AddString(after);

	_order.SetString(after);

	_movingType.AddString(spline);
	_movingType.AddString(discontinuous);
	//_movingType.AddString("line path");

	_movingType.SetString(spline);

	_pivotX.SetFloat(0.f);
	_pivotY.SetFloat(0.f);

}

void BonePropDialog::OnCommand(HWND hWnd, int controlID, int command) {
	switch (controlID)
	{
		case IDC_LOAD_IMAGE:
		{
			std::string file;
			OPENFILENAME fn;
			fn.lStructSize = sizeof(fn);
			fn.hInstance = 0;
			fn.hwndOwner = _hWnd;
			fn.lpstrInitialDir = _lastOpenedDir.c_str();
			fn.lpstrFilter = "Image PNG(*.PNG)\0*.PNG\0All(*.*)\0*.*\0";
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
				_lastOpenedDir = MainDialog::CutFileName(fn.lpstrFile);
				std::string name(std::string(fn.lpstrFile).substr(_lastOpenedDir.length() + 1));
				std::string tmp(MainDialog::CutFileName(_lastOpenedDir));
				std::string dir;
				if (tmp.size() != 0) {
					dir = _lastOpenedDir.substr(tmp.length() + 1);
					CreateDirectory((MainDialog::CurrentFile() + "_files\\" + dir).c_str(), NULL);
					dir += "\\";
				}
				if (!CopyFile(fn.lpstrFile, (MainDialog::CurrentFile() + "_files\\" + dir + name).c_str(), TRUE)) {
					// todo
					// сравнить побайтам и если одинаковые - то пропускаем, если разные - говорим "у вас уже есть другой спрайт с таким же именем и каталогом - заменить? да/нет"
					assert(false);
				}
				_boneSprite.SetName((dir + name).c_str());
			}
		}
		break;

	}
}

void BonePropDialog::SetEnable(bool enable) {
	if (enable) {
		_order.Enable();
		_movingType.Enable();
		_pivotX.Enable();
		_pivotY.Enable();
		_boneSprite.Enable();
	} else {
		_order.Disable();
		_movingType.Disable();
		_pivotX.Disable();
		_pivotY.Disable();
		_boneSprite.Disable();
	}
}