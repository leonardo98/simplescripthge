#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>
#include "Commctrl.h"

#define MAX_STR_LENGTH 256

class SimpleControl
{
public:
	SimpleControl(HWND hwndParent, int id, BOOL initialState = TRUE);
	void Hide();
	void Show();
	BOOL IsVisible();
	void SetFocus();
	void Enable();
	void Disable();
	HWND Hwnd() const;

protected:
	HWND _hWnd;
};

class Button : public SimpleControl
{
public:
	Button(HWND hwndParent, int id, BOOL initialState = TRUE);
	void SetName(char const *name);
	void GetName(char* buf, int len);
};

class CheckBox : public Button
{
public:
	CheckBox(HWND hwndParent, int id, BOOL initialState = TRUE);
	BOOL IsChecked();
	void Check();
	void UnCheck();
};

class RadioButton : public Button
{
public:
	RadioButton(HWND hwndParent, int id, BOOL initialState = TRUE);
	BOOL IsSelected();
	void Select();
};

class Edit : public SimpleControl
{
public:
	Edit(HWND hwndParent, int id, BOOL initialState = TRUE);
	void SetString(const char* s);
	void SetFloat(float f);
	// code is the HIWORD (wParam)
	static BOOL IsChanged(int code);
	int GetLength();
	void GetString(char* buf, int len);
	float GetFloat();
	void Select();
	void ClearSelection();
};

class ComboBox : public SimpleControl
{
public:
	ComboBox(HWND hwndParent, int id, BOOL initialState = TRUE);
	void Reset();
	void AddString(const char* s);
	BOOL IsChanged(int code);
	int GetString(char* s, int max);
	int SetString(const char* s);
};

class TreeView : public SimpleControl
{
public:
	TreeView(HWND hwndParent, int id, BOOL initialState = TRUE);
	HTREEITEM AddRoot(char* text);
	HTREEITEM AddChild(HTREEITEM parent, char* text, void *samedata);
	int DeleteItem(HTREEITEM item);
	void * GetSelection();
private:
	HWND _hwndParent;
	int _id;
};

#endif // CONTROLS_H
