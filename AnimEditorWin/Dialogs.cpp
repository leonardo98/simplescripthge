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
#include "MainDialog.h"
#include "BonePropDialog.h"

HWND d_main = 0;
HWND d_bone = 0;

MainDialog *_mainDialog;
BonePropDialog *_bonePropDialog;

AnimEditor *editor;
#define MAX_LENGTH 256
char lastAnimationId[MAX_LENGTH];
bool updateAnimationState = false;

HTREEITEM tree_root = 0;             
void * last_selected = 0;

BOOL CALLBACK DialogProcBone(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) // what are we doing ?
	{ 	 
		// this messages are the heart of the dialogs 
		case WM_INITDIALOG: 
		{
			_bonePropDialog = new BonePropDialog(hWnd);
			_bonePropDialog->SetEnable(false);
		}
		break;

		case WM_COMMAND :
			if (!_bonePropDialog->locked) {
				_bonePropDialog->OnCommand(hWnd, LOWORD(wParam), HIWORD (wParam));
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
			_mainDialog = new MainDialog(hWnd);
		}
		break;

		case WM_COMMAND :
			_mainDialog->OnCommand(hWnd, LOWORD(wParam), HIWORD (wParam));
		break;

	}
	return 0;
}

HWND InitDialogs(HINSTANCE hInstance) {
	editor = new AnimEditor();
	d_main = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DialogProcMain);
	d_bone = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_BONE_PROP), NULL, (DLGPROC)DialogProcBone);
	SetWindowPos(Render::GetDC()->System_GetState(HGE_HWND), HWND_TOP,
				Render::GetDC()->Ini_GetInt("dialogs", "renderx", 0),
				Render::GetDC()->Ini_GetInt("dialogs", "rendery", 0),
				0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	SetWindowPos(d_main, HWND_TOP,
				Render::GetDC()->Ini_GetInt("dialogs", "mainx", 0),
				Render::GetDC()->Ini_GetInt("dialogs", "mainy", 0),
				0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	SetWindowPos(d_bone, HWND_TOP, 
				Render::GetDC()->Ini_GetInt("dialogs", "bonex", 0),
				Render::GetDC()->Ini_GetInt("dialogs", "boney", 0),
				0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	ShowWindow(d_main, SW_SHOW); 
	ShowWindow(d_bone, SW_SHOW); 
	return d_main;
}

void CloseDialogs() {
	delete editor;
	RECT rect;
	if (GetWindowRect(d_main, &rect)) {
		Render::GetDC()->Ini_SetInt("dialogs", "mainx", rect.left);
		Render::GetDC()->Ini_SetInt("dialogs", "mainy", rect.top);
	}
	if (GetWindowRect(d_bone, &rect)) {
		Render::GetDC()->Ini_SetInt("dialogs", "bonex", rect.left);
		Render::GetDC()->Ini_SetInt("dialogs", "boney", rect.top);
	}
	if (GetWindowRect(Render::GetDC()->System_GetState(HGE_HWND), &rect)) {
		Render::GetDC()->Ini_SetInt("dialogs", "renderx", rect.left);
		Render::GetDC()->Ini_SetInt("dialogs", "rendery", rect.top);
	}
	DestroyWindow(d_main);
	DestroyWindow(d_bone);
	delete _mainDialog;
	delete _bonePropDialog;
}

bool SetDialogsOnTop() {
	SetWindowPos(d_main, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_bone, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	return true;
}

void Draw() {
	editor->Draw();
}

void * InsertEndChild(void *, char *, Bone *);

// заполняем структуру по данным из окна
void SetAnimationParameters(AnimationInfo &info) {
	info.pivotPos.x = _mainDialog->_pivotX.GetFloat();
	info.pivotPos.y = _mainDialog->_pivotY.GetFloat();
	info.time = _mainDialog->_time.GetFloat();
	info.loop = _mainDialog->_loop.IsChecked() ? true : false;
}

// заполняем данными окно
void ReadAnimationParameters() {
	AnimationInfo info;
	editor->Anim()->Get(info);
	_mainDialog->_pivotX.SetFloat(info.pivotPos.x);
	_mainDialog->_pivotY.SetFloat(info.pivotPos.y);
	_mainDialog->_time.SetFloat(info.time);
	if (info.loop) {
		_mainDialog->_loop.Check();
	} else {
		_mainDialog->_loop.UnCheck();
	}
}

// заполняем структуру по данным из окна
void SetBoneProp(MovingPartInfo &info) {
	info.center.x = _bonePropDialog->_pivotX.GetFloat();
	info.center.y = _bonePropDialog->_pivotY.GetFloat();
	char tmp[MAX_STR_LENGTH];
	int len = _bonePropDialog->_movingType.GetString(tmp, MAX_STR_LENGTH);
	info.discontinuous = std::string(tmp) == _bonePropDialog->discontinuous;
	len = _bonePropDialog->_order.GetString(tmp, MAX_STR_LENGTH);
	info.offparent = (std::string(tmp) == _bonePropDialog->after ? OffParentOrder::top : OffParentOrder::bottom);
	info.partsNames.clear();
	_bonePropDialog->_boneSprite.GetName(tmp, MAX_STR_LENGTH);
	if (std::string(tmp) != _bonePropDialog->loadimage) {
		info.partsNames.push_back(tmp);
	}	
	info.x.Clear();
	info.y.Clear();
	info.angle.Clear();
	info.scaleX.Clear();
	info.scaleY.Clear();
	for (unsigned int i = 0; i < editor->poses.size(); ++i) {
		info.x.addKey(editor->poses[i].x);
		info.y.addKey(editor->poses[i].y);
		info.angle.addKey(editor->poses[i].angle);
		info.scaleX.addKey(editor->poses[i].sx);
		info.scaleY.addKey(editor->poses[i].sy);
	}
	bool loop = (_mainDialog->_loop.IsChecked() ? true : false);
	info.x.CalculateGradient(loop);
	info.y.CalculateGradient(loop);
	info.angle.CalculateGradient(loop);
	info.scaleX.CalculateGradient(loop);
	info.scaleY.CalculateGradient(loop);
}

// заполняем данными окно
void ReadBoneProp(Bone* selected) {
	MovingPartInfo info;
	selected->Get(info);
	_bonePropDialog->_pivotX.SetFloat(info.center.x);
	_bonePropDialog->_pivotY.SetFloat(info.center.y);
	if (info.offparent == OffParentOrder::bottom) {
		_bonePropDialog->_order.SetString(_bonePropDialog->before);
	} else {
		_bonePropDialog->_order.SetString(_bonePropDialog->after);
	}
	if (info.discontinuous) {
		_bonePropDialog->_movingType.SetString(_bonePropDialog->discontinuous);
	} else {
		_bonePropDialog->_movingType.SetString(_bonePropDialog->spline);
	}
	if (info.partsNames.size()) {
		_bonePropDialog->_boneSprite.SetName(info.partsNames.front().c_str());
	}
	editor->poses.clear();
	for (unsigned int i = 0; i < info.angle.pushedKeys.size(); ++i) {
		editor->poses.push_back(AnimEditor::key());
		editor->poses.back().angle = info.angle.pushedKeys[i].first;
		editor->poses.back().x = info.x.pushedKeys[i].first;
		editor->poses.back().y = info.y.pushedKeys[i].first;
		editor->poses.back().sx = info.scaleX.pushedKeys[i].first;
		editor->poses.back().sy = info.scaleY.pushedKeys[i].first;
	}
}

bool Exit(bool exit) {
	_mainDialog->exitPressed |= exit;
	return _mainDialog->exitPressed;
}

void * InsertEndChild(void *parent, char *id, Bone *bone) {
	return _mainDialog->_bonesTree.AddChild((HTREEITEM)parent, id, bone);
}

void CreateAnimationTree() {
	HTREEITEM oldRoot = tree_root;
	
	// create new
	tree_root = _mainDialog->_bonesTree.AddRoot(lastAnimationId);

	//remove old
	if (oldRoot) {
		_mainDialog->_bonesTree.DeleteItem(oldRoot);
	}
}

void Update(float dt) {
	if (_mainDialog->waitingNewAnimationId) {
		char buffer[MAX_LENGTH];
		int len = _mainDialog->_animationList.GetString(buffer, MAX_LENGTH);
		if (len && strcmp(lastAnimationId, buffer) != 0) {
			editor->SetCurrent(buffer);
			strcpy_s(lastAnimationId, buffer);
			_mainDialog->waitingNewAnimationId = false;
			CreateAnimationTree();
			editor->CreateTree(tree_root, InsertEndChild);
			TreeView_Expand(_mainDialog->_bonesTree.Hwnd(), tree_root, TVE_EXPAND);
			ReadAnimationParameters();
			UpdateWindow(d_main);
		}
	}
	if (_mainDialog->updateAnimationState) {
		_mainDialog->updateAnimationState = false;
		if (editor->Anim() != NULL) {
			AnimationInfo base;
			editor->Anim()->Get(base);
			AnimationInfo edit;
			SetAnimationParameters(edit);
			if (base.loop != edit.loop
					|| fabs(base.time - edit.time) > 1e-3
					|| fabs(base.pivotPos.x - edit.pivotPos.x) > 1e-3
					|| fabs(base.pivotPos.y - edit.pivotPos.y) > 1e-3)
			{
				if (edit.time < 0.01f) {
					edit.time = base.time;
				}
				editor->Anim()->Set(edit);
			}
		}
	}
	void * selected = _mainDialog->_bonesTree.GetSelection();
	if (selected != last_selected) {
		if (selected != 0) {
			_bonePropDialog->locked = true;
			ReadBoneProp((Bone *)selected);
			_bonePropDialog->SetEnable(true);
			_bonePropDialog->locked = false;
		} else {
			_bonePropDialog->SetEnable(false);
		}
		last_selected = selected;
	}
	if (_bonePropDialog->updateBoneProp) {
		_bonePropDialog->updateBoneProp = false;
		if (last_selected) {
			MovingPartInfo edit;
			SetBoneProp(edit);
			((Bone*)last_selected)->Set(edit);
		}
	}
	editor->Update(dt);
}
