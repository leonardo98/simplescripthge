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

HWND d_main = 0;
HWND d_bone = 0;

MainDialog *_mainDialog;

AnimEditor editor;
#define MAX_LENGTH 256
char lastAnimationId[MAX_LENGTH];
bool updateAnimationState = false;

HTREEITEM tree_root = 0;             


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
	d_main = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DialogProcMain);
	ShowWindow(d_main, SW_SHOW); 
	d_bone = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_BONE_PROP), NULL, (DLGPROC)DialogProcBone);
	ShowWindow(d_bone, SW_SHOW); 
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
	return d_main;
}

void CloseDialogs() {
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
}

bool SetDialogsOnTop() {
	SetWindowPos(d_main, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(d_bone, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	return true;
}

void Draw() {
	editor.Draw();
}

void * InsertEndChild(void *, char *, Bone *);

void SetAnimationParameters(AnimationInfo &info) {
	info.pivotPos.x = _mainDialog->_pivotX.GetFloat();
	info.pivotPos.y = _mainDialog->_pivotY.GetFloat();
	info.time = _mainDialog->_time.GetFloat();
	info.loop = _mainDialog->_loop.IsChecked() ? true : false;
	editor.Anim()->Set(info);
}

void ReadAnimationParameters() {
	AnimationInfo info;
	editor.Anim()->Get(info);
	_mainDialog->_pivotX.SetFloat(info.pivotPos.x);
	_mainDialog->_pivotY.SetFloat(info.pivotPos.y);
	_mainDialog->_time.SetFloat(info.time);
	if (info.loop) {
		_mainDialog->_loop.Check();
	} else {
		_mainDialog->_loop.UnCheck();
	}
}

void Update(float dt) {
	if (_mainDialog->waitingNewAnimationId) {
		char buffer[MAX_LENGTH];
		int len = _mainDialog->_animationList.GetString(buffer, MAX_LENGTH);
		if (len && strcmp(lastAnimationId, buffer) != 0) {
			editor.SetCurrent(buffer);
			strcpy_s(lastAnimationId, buffer);
			_mainDialog->waitingNewAnimationId = false;
			CreateAnimationTree();
			editor.CreateTree(tree_root, InsertEndChild);
			ReadAnimationParameters();
			UpdateWindow(d_main);
		}
	}
	if (_mainDialog->updateAnimationState && editor.Anim() != NULL) {
		AnimationInfo base;
		editor.Anim()->Get(base);
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
			editor.Anim()->Set(edit);
		}
		_mainDialog->updateAnimationState = false;
	}
	editor.Update(dt);
}

bool Exit(bool exit) {
	_mainDialog->exitPressed |= exit;
	return _mainDialog->exitPressed;
}

// creating child
void * InsertEndChild(void *parent, char *id, Bone *bone) {
	return _mainDialog->_bonesTree.AddChild((HTREEITEM)parent, id, bone);
}

void CreateAnimationTree() {
	//remove old
	HTREEITEM oldRoot = tree_root;
	if (tree_root) {
		//TreeView_DeleteItem(hAnimationTree, tree_root);
		//SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_DELETEITEM, 0, (LPARAM)(HTREEITEM)tree_root);
	}
	
	// create new
	tree_root = _mainDialog->_bonesTree.AddRoot(lastAnimationId);

	//HTREEITEM i = InsertEndChild(tree_root, "Hello");
	//i = InsertEndChild(tree_root, "Hello2");
	//i = InsertEndChild(tree_root, "Hello3");
	// child
	//tvinsert.hParent = tree_root;         // handle of the above data
	//tvinsert.hInsertAfter = TVI_FIRST;  // below parent
	//tvinsert.item.pszText = "Child 1";
	//HTREEITEM item = (HTREEITEM)SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
	if (oldRoot) {
		_mainDialog->_bonesTree.DeleteItem(oldRoot);
		//SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_DELETEITEM, 0, (LPARAM)(HTREEITEM)tree_root);
	}

	return;
}
