#include "stdafx.h"
#include "Controls.h"
#include "Windowsx.h"
#include "stdio.h"

SimpleControl::SimpleControl(HWND hwndParent, int id, BOOL initialState)
: _hWnd (GetDlgItem (hwndParent, id))
{
	if (initialState == FALSE)
		Disable();
}

void SimpleControl::Hide() {
	::ShowWindow(_hWnd , SW_HIDE);
}

void SimpleControl::Show() {
	::ShowWindow(_hWnd , SW_SHOW);
}

BOOL SimpleControl::IsVisible() {
	return(::IsWindowVisible(_hWnd ));
}

void SimpleControl::SetFocus() {
	::SetFocus (_hWnd);
}

void SimpleControl::Enable() {
	::EnableWindow(_hWnd , TRUE);
}

void SimpleControl::Disable() {
	::EnableWindow(_hWnd , FALSE);
}

HWND SimpleControl::Hwnd() const { 
	return _hWnd; 
}

Button::Button(HWND hwndParent, int id, BOOL initialState)
: SimpleControl(hwndParent, id, initialState)
{}

void Button::SetName( char const * newName ) {
	SendMessage(_hWnd, WM_SETTEXT, 0, (LPARAM)newName );
}

CheckBox::CheckBox(HWND hwndParent, int id, BOOL initialState)
: Button(hwndParent, id, initialState)
{}

BOOL CheckBox::IsChecked() {
	return( SendMessage(_hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED );
}

void CheckBox::Check() {
	SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
}

void CheckBox::UnCheck() {
	SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_UNCHECKED, 0);
}

RadioButton::RadioButton(HWND hwndParent, int id, BOOL initialState)
: Button(hwndParent, id, initialState)
{}

BOOL RadioButton::IsSelected() {
	return( SendMessage( _hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED );
}

void RadioButton::Select() {
	SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
}

Edit::Edit(HWND hwndParent, int id, BOOL initialState)
:	SimpleControl(hwndParent, id, initialState)
{}

void Edit::SetString(char* buf) {
	SendMessage (_hWnd, WM_SETTEXT, 0, (LPARAM) buf);
}

void Edit::SetFloat(float f) {
	char tmp[MAX_STR_LENGTH];
	sprintf_s(tmp, "%f", f);
	int i = strlen(tmp);
	while (i > 2 && tmp[i - 1] == '0' && tmp[i - 2] == '0') {
		tmp[--i] = 0;
	}
	SetString(tmp);
}

BOOL Edit::IsChanged(int code) {
	return code == EN_CHANGE;
}

int Edit::GetLength() {
	return (int)(SendMessage (_hWnd, WM_GETTEXTLENGTH, 0, 0));
}

void Edit::GetString(char* buf, int len) {
	SendMessage (_hWnd, WM_GETTEXT, (WPARAM) len, (LPARAM) buf);
}

float Edit::GetFloat() {
	char tmp[MAX_STR_LENGTH];
	GetString(tmp, MAX_STR_LENGTH);
	return (float)atof(tmp);
}

void Edit::Select() {
	SendMessage (_hWnd, EM_SETSEL, 0, -1);
}

void Edit::ClearSelection() {
	SendMessage (_hWnd, EM_SETSEL, -1, 0);
}

ComboBox::ComboBox(HWND hwndParent, int id, BOOL initialState)
: Edit(hwndParent, id, initialState)
{}

BOOL ComboBox::IsChanged(int code) {
	return code == CBN_SELCHANGE;
}

void ComboBox::Reset() {
	ComboBox_ResetContent(_hWnd);
}

void ComboBox::AddString(const char* s) {
	ComboBox_AddString(_hWnd, (LPCSTR)s);
}

int ComboBox::GetString(char* s, int max) {
	return ComboBox_GetText(_hWnd, s, max);
}

TreeView::TreeView(HWND hwndParent, int id, BOOL initialState) 
: SimpleControl(hwndParent, id, initialState)
, _hwndParent(hwndParent)
, _id(id)
{}

HTREEITEM TreeView::AddRoot(char* text) {
	TVINSERTSTRUCT tvinsert;   // struct to config out tree control
	memset(&tvinsert, 0, sizeof(TVINSERTSTRUCT));
	tvinsert.hParent = NULL;			// top most level no need handle
	tvinsert.hInsertAfter = TVI_ROOT; // work as root level
    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvinsert.item.pszText = text;
	tvinsert.item.iImage = 0;
	tvinsert.item.iSelectedImage = 1;
	return (HTREEITEM)SendDlgItemMessage(_hwndParent, _id, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
}

HTREEITEM TreeView::AddChild(HTREEITEM parent, char* text, void *samedata) {
	TVINSERTSTRUCT tvinsert;   // struct to config out tree control
	memset(&tvinsert, 0, sizeof(TVINSERTSTRUCT));
	tvinsert.hParent = parent;			
	tvinsert.hInsertAfter = TVI_LAST; 
    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvinsert.item.pszText = text;
	tvinsert.item.iImage = 0;
	tvinsert.item.iSelectedImage = 1;
	tvinsert.item.lParam = (LPARAM)samedata;
	return (HTREEITEM)SendDlgItemMessage(_hwndParent, _id, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
}

int TreeView::DeleteItem(HTREEITEM item) {
	return TreeView_DeleteItem(_hWnd, item);
}

