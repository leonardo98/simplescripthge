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
	static std::string CutFileName(const std::string &filePath);
	static std::string CurrentFile();

private:
	std::string _lastOpenedDir;
	static std::string _currentFile;
	HWND _hWnd;
};

#endif//MAINDIALOG_H