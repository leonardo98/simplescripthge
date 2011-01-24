// InputSystem.h: interface for the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_INPUTSYSTEM_INCLUDED_)
#define _INPUTSYSTEM_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Object.h"

//
// ���������� ��� �������� �����,
// ���� ����� ������������ ������� ���� - ����������� �� ����� ������
// ���� ��� - �� Object.h
//

class InputSystem 
	: public Object  
{
private:
	static float DOUBLE_CLICK_TIME; // ������������ ����� �� ������ ���� - ���� ����� ������ - ������� "������������"
	static float LONG_TAP_EPS; // ����������� �� ��������� �������� ���� �� ����� �������, ���� �������� ����� ������� - ������� "������������"
	static float LONG_TAP_TIME; // ����������� ����� �� ����� ������� - ���� ����� ������ - ������� "������������"
	static float _timeCounter; // ������� �������(����������� ����� ���� � ���������� ������� ������� ��� ���������� ������ ����
	static bool _longTap; // true - ���� ����� ������� "������� ���"
	static bool _doubleClick; // true - ���� ����� ��� ������� "������� ����"
	static FPoint2D _longTapPos;
	static InputSystem *_locked;// ���� ���� ��� ������ ������������ ����(�������/����������) - ��������� �� ����� MouseMove
public:
	InputSystem();
	virtual ~InputSystem();
	// ��� ������ ������ �������������� 
	// ���� ����� ������������ �������������� �������

	// ������ ������� �� ����������(��� �� ����������)
	virtual void OnKeyDown(int key);
	// ���� �� ��������(������� ����� ������ ����)
	virtual void OnMouseDown(hgeVector mousePos);
	// ��������� ������ ����(������� ���)
	virtual void OnMouseUp();
	// �������� ����
	virtual void OnMouseMove(hgeVector mousePos);
	// �������� ����
	virtual bool OnMouseWheel(int direction);
	// ���������� ������� � ���������
	virtual void OnLongTap(hgeVector mousePos);
	// ������� ����
	virtual void OnDoubleClick(hgeVector mousePos);

	// ���������� ����� ���������� ��� ���������� ������ OnMouseDown() � ������ �������
	virtual bool IsMouseOver(hgeVector mousePos) = 0;
	
protected:
	// ��� �������(���������) ��������� ����� - ����� ������������, ���������� ����� ������
	void RemoveFromList(InputSystem *listener); 

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;
	static void KeyDown(int key);
	static void MouseDown(hgeVector mousePos);
	static void MouseUp();
	static void MouseMove(hgeVector mousePos);
	static void MouseWheel(int direction);
	static void LongTap();
	static void DoubleClick(hgeVector mousePos);
	friend void InitInputEvent();
	friend bool CheckForInputEvent(HGE *hge, float dt);
};

#endif // !defined(_INPUTSYSTEM_INCLUDED_)
