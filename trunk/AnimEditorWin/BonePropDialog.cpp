#include "stdafx.h"
#include "BonePropDialog.h"
#include "Resource.h"

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
{
	_boneSprite.SetName(loadimage);

	_order.AddString(before);
	_order.AddString(after);

	_order.SetString(after);

	_movingType.AddString(spline);
	_movingType.AddString(discontinuous);
	//_movingType.AddString("line path");

	_movingType.SetString(spline);
}

void BonePropDialog::OnCommand(HWND hWnd, int controlID, int command) {
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