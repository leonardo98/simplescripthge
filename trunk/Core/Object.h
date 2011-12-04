// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"

//
// ������� �����, ����������� �� ���� ���� ��� ����� ������ ��� 
// ������ � �� ����� ������������ ����, ����� ��. InputSystem.h
//

class Object  
{
public:
	virtual ~Object();
	virtual void Draw();
	virtual void Update(float);
};

#endif//OBJECT_H
