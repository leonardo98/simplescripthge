#ifndef BONE_PROP_DIALOG_H
#define BONE_PROP_DIALOG_H

#include "Controls.h"

class BonePropDialog {
public:
	BonePropDialog(HWND hWnd);
	void OnCommand(HWND hWnd, int controlID, int command);
	void SetEnable(bool enable);
	const char *before;
	const char *after;
	const char *spline;
	const char *discontinuous;
	const char *loadimage;

	ComboBox _order;
	ComboBox _movingType;
	Edit _pivotX;
	Edit _pivotY;
	Button _boneSprite;
};

#endif//BONE_PROP_DIALOG_H