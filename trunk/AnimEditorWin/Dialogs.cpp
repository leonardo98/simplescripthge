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
HWND hAnimationTree = 0;
std::string lastOpenedDir = "C:\\Projects\\MyEngineMac&Win\\AnimEditorWin\\TestFiles";
AnimEditor editor;
#define MAX_LENGTH 256
char lastAnimationId[MAX_LENGTH];
bool waitingNewAnimationId = false;

HTREEITEM tree_root = 0;             


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
			hAnimationTree = GetDlgItem(hWnd, IDC_ANIMATION_TREE);

			// creating image list and put it into the tree control
			//====================================================//
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

void * InsertEndChild(void *, char *, void *);
//void TreeCreater(HTREEITEM parent, char *name, void *ptr) {
//
//}

void Update(float dt) {
	if (waitingNewAnimationId) {
		char buffer[MAX_LENGTH];
		int len = ComboBox_GetText(hAnimationCombo, buffer, MAX_LENGTH);
		if (len && strcmp(lastAnimationId, buffer) != 0) {
			editor.SetCurrent(buffer);
			strcpy_s(lastAnimationId, buffer);
			waitingNewAnimationId = false;
			CreateAnimationTree();
			editor.CreateTree(tree_root, InsertEndChild);
			UpdateWindow(d_main);
		}
	}
	editor.Update(dt);
}

bool Exit() {
	return editor.exitPressed;
}

// creating root
HTREEITEM AddRoot(char *id) {
	TVINSERTSTRUCT tvinsert;   // struct to config out tree control
	memset(&tvinsert, 0, sizeof(TVINSERTSTRUCT));
	tvinsert.hParent = NULL;			// top most level no need handle
	tvinsert.hInsertAfter = TVI_ROOT; // work as root level
    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvinsert.item.pszText = id;
	tvinsert.item.iImage = 0;
	tvinsert.item.iSelectedImage = 1;
	return (HTREEITEM)SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
}

// creating child
void * InsertEndChild(void *parent, char *id, void *) {
	TVINSERTSTRUCT tvinsert;   // struct to config out tree control
	memset(&tvinsert, 0, sizeof(TVINSERTSTRUCT));
	tvinsert.hParent = (HTREEITEM)parent;			// top most level no need handle
	tvinsert.hInsertAfter = TVI_LAST; // work as root level
    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvinsert.item.pszText = id;
	tvinsert.item.iImage = 0;
	tvinsert.item.iSelectedImage = 1;
	return (void *)SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
}

void CreateAnimationTree() {
	//remove old
	HTREEITEM oldRoot = tree_root;
	if (tree_root) {
		//TreeView_DeleteItem(hAnimationTree, tree_root);
		//SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_DELETEITEM, 0, (LPARAM)(HTREEITEM)tree_root);
	}
	
	// create new
	tree_root = AddRoot(lastAnimationId);

	//HTREEITEM i = InsertEndChild(tree_root, "Hello");
	//i = InsertEndChild(tree_root, "Hello2");
	//i = InsertEndChild(tree_root, "Hello3");
	// child
	//tvinsert.hParent = tree_root;         // handle of the above data
	//tvinsert.hInsertAfter = TVI_FIRST;  // below parent
	//tvinsert.item.pszText = "Child 1";
	//HTREEITEM item = (HTREEITEM)SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
	if (oldRoot) {
		TreeView_DeleteItem(hAnimationTree, oldRoot);
		//SendDlgItemMessage(d_main, IDC_ANIMATION_TREE, TVM_DELETEITEM, 0, (LPARAM)(HTREEITEM)tree_root);
	}

	return;
}
