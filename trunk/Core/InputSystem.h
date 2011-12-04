// InputSystem.h: interface for the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

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
	virtual void OnMouseDown(const FPoint2D &mousePos);
	// ��������� ������ ����(������� ���)
	virtual void OnMouseUp();
	// �������� ����
	virtual void OnMouseMove(const FPoint2D &mousePos);
	// �������� ����
	virtual bool OnMouseWheel(int direction);
	// ���������� ������� � ���������
	virtual void OnLongTap(const FPoint2D &mousePos);
	// ������� ����
	virtual void OnDoubleClick(const FPoint2D &mousePos);

	// ���������� ����� ���������� ��� ���������� ������ OnMouseDown() � ������ �������
	virtual bool IsMouseOver(const FPoint2D &mousePos) = 0;
	
protected:
	// ��� �������(���������) ��������� ����� - ����� ������������, ���������� ����� ������
	void RemoveFromList(InputSystem *listener); 

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;

	static void MouseDown(const FPoint2D &mousePos);
	static void MouseUp();
	static void MouseMove(const FPoint2D &mousePos);
	static void MouseWheel(int direction);
	static void LongTap();
	static void DoubleClick(const FPoint2D &mousePos);

public:
	static void Reset();
	static void Init();
	static bool CheckForEvent(float dt);

};

#endif//INPUTSYSTEM_H