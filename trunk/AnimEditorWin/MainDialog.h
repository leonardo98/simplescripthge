#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "Controls.h"
#include <string>

class MainDialog {
public:
	MainDialog(HWND hWnd);
	void OnCommand(HWND hWnd, int controlID, int command);
	ComboBox _animationList;
	TreeView _bonesTree;
	Edit _pivotX;
	Edit _pivotY;
	Edit _time;
	CheckBox _loop;
	bool exitPressed;
	bool waitingNewAnimationId;
	bool updateAnimationState;

private:
	std::string CutFileName(std::string filePath);
	std::string _lastOpenedDir;
	HWND _hWnd;
};

#endif//MAINDIALOG_H