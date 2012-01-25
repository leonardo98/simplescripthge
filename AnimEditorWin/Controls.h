#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>

class SimpleControl
{
public:
    SimpleControl (HWND hwndParent, int id, BOOL initialState=TRUE)
        : _hWnd (GetDlgItem (hwndParent, id))
    {
        if (initialState == FALSE)
         Disable();
   }
   void Hide ()
   {
      ::ShowWindow(_hWnd , SW_HIDE);
   }
   void Show ()
   {
      ::ShowWindow(_hWnd , SW_SHOW);
   }
   BOOL IsVisible()
   {
      return(::IsWindowVisible(_hWnd ));
   }
   void SetFocus ()
   {
       ::SetFocus (_hWnd);
   }
   void Enable()
   {
       ::EnableWindow(_hWnd , TRUE);
   }
   void Disable()
   {
       ::EnableWindow(_hWnd , FALSE);
   }
	HWND Hwnd () const { return _hWnd; }

protected:
    HWND _hWnd;
};

class Button : public SimpleControl
{
public:
   Button(HWND hwndParent, int id, BOOL initialState=TRUE)
       : SimpleControl(hwndParent, id, initialState)
   {}
   void SetName( char const * newName )
   {
      SendMessage(_hWnd, WM_SETTEXT, 0, (LPARAM)newName );
   }
};

class CheckBox : public Button
{
public:
   CheckBox (HWND hwndParent, int id, BOOL initialState=TRUE)
       : Button(hwndParent, id, initialState)
   {}
   BOOL IsChecked()
   {
      return( SendMessage(_hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED );
   }
   void Check()
   {
      SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
   }
   void UnCheck()
   {
      SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_UNCHECKED, 0);
   }
};

class RadioButton: public Button
{
public:
   RadioButton (HWND hwndParent, int id, BOOL initialState=TRUE)
       : Button(hwndParent, id, initialState)
   {}
   BOOL IsSelected()
   {
      return( SendMessage( _hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED );
   }
   void Select()
   {
      SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
   }
};

class Edit: public SimpleControl
{
public:
   Edit (HWND hwndParent, int id, BOOL initialState=TRUE)
        :	SimpleControl (hwndParent, id, initialState)
   {}

   void SetString (char* buf)
   {
       SendMessage (_hWnd, WM_SETTEXT, 0, (LPARAM) buf);
   }

   // code is the HIWORD (wParam)
   static BOOL IsChanged (int code)
   {
       return code == EN_CHANGE;
   }

   int GetLength ()
   {
       return (int)(SendMessage (_hWnd, WM_GETTEXTLENGTH, 0, 0));
   }

   void GetString (char* buf, int len)
   {
       SendMessage (_hWnd, WM_GETTEXT, (WPARAM) len, (LPARAM) buf);
   }

   void Select ()
   {
       SendMessage (_hWnd, EM_SETSEL, 0, -1);
   }

   void ClearSelection ()
   {
       SendMessage (_hWnd, EM_SETSEL, -1, 0);
   }
};

#endif // CONTROLS_H
