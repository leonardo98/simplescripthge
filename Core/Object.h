// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OBJECT_INCLUDED_)
#define _OBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

//
// ������� �����, ����������� �� ���� ���� ��� ����� ������ ��� 
// ������ � �� ����� ������������ ����, ����� ��. InputSystem.h
//

class Object  
{
public:
	Object(bool show = true):bVisible(show) {} //�� ������� ��� ������� �������
	virtual ~Object();
	virtual void Draw();
	virtual void Update(float);

	void Show(bool show) { bVisible = show; } 
	bool isVisible() { return bVisible; }
private:
	bool bVisible;	//������� ������� �� �������� ������� �� ����������,
					//�� ��������� � �� ���������� ���� ���������������
					//�������� (Core, InputSystem)
};

#endif // !defined(_OBJECT_INCLUDED_)
