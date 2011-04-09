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

	// ���� �� ��������(������� ����� ������ ����)
	virtual void OnMouseDown(FPoint2D mousePos);
	// ��������� ������ ����(������� ���)
	virtual void OnMouseUp();
	// �������� ����
	virtual void OnMouseMove(FPoint2D mousePos);
	// �������� ����
	virtual bool OnMouseWheel(int direction);
	// ���������� ������� � ���������
	virtual void OnLongTap(FPoint2D mousePos);
	// ������� ����
	virtual void OnDoubleClick(FPoint2D mousePos);

	// ���������� ����� ���������� ��� ���������� ������ OnMouseDown() � ������ �������
	virtual bool IsMouseOver(FPoint2D mousePos) = 0;
	
protected:
	// ��� �������(���������) ��������� ����� - ����� ������������, ���������� ����� ������
	void RemoveFromList(InputSystem *listener); 

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;
	static void MouseDown(FPoint2D mousePos);
	static void MouseUp();
	static void MouseMove(FPoint2D mousePos);
	static void MouseWheel(int direction);
	static void LongTap();
	static void DoubleClick(FPoint2D mousePos);
	friend void InitInputEvent();
	friend bool CheckForInputEvent(float dt);
};

#endif // !defined(_INPUTSYSTEM_INCLUDED_)
