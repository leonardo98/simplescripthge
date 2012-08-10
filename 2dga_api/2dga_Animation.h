/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
** 
** ����� ��� ����������������� ��������������� �������� � ����
**
** ��� ������������ ����� ��������:
** hgeVector - ��� ����� ����� � ��������� ������������
** HTEXTURE  - ��������� �� ������������ ��������
**
*/

#ifndef MYANIMATION_H
#define MYANIMATION_H

#include "TinyXml.h"
#include <string>
#include <map>
#include "../Core/Matrix.h"
#include "2dga_MovingPart.h"
#include "hge.h"

class MyAnimation
{
public:

	//
	// ����������� - ������������ � ��������� �������� ��������
	// ��������� �������� �������� �� XML - ����� ������������ ����� ������,
	// ������� ����� ���������� ���� � ������ ��������,
	// ����� ����������� TinyXml
	//
	MyAnimation(TiXmlElement *xe, HTEXTURE hTexture);

	//
	// ���������� - ������������ � ��������� �������� ��������
	//
	virtual ~MyAnimation();

	//
	// ���������� ��������� �������� �� ������, 
	// "pos" - ��������� �������� �� ������, 
	// ����� ���������� ��������� - ������ "mirror" � "true".
	// ����� ����� ����� �������� ����� ������ ������� Draw()
	// ���������� ����� ����, ���� ����� �������� �� ������ ����� ����
	//
	void SetPos(const hgeVector &pos, bool mirror);

	//
	// ���������� �������� �� ������ 
	// "position" - �������� ������� ��� �������� �� ��������� [0, 1]
	//
	void Draw(float position);

	//
	// ����� ������� ����� �������� 
	// ��� ������� ����� ������������ � Update ��� ������� 
	// �������� "position" ������ Draw
	// ��������, ��� ���������� �������� ����� ������� ���:
	// progress += deltaTime / anim->Time();
	// if(progress >= 1.f) progress = 0.f;
	//
	float Time()const;

private:
	float _time;
	hgeVector _pivotPos;
	std::vector<MyMovingPart *> _bones;
	Matrix _subPosition;
	
	// ���� ��� ����� ������, ������� ����� ��������� ���� ����� ������
	MatrixStack _matrixsStack;
};


//
// ������ ������������ ��������� ��������
//
// � ���������� �������� ���������� ������������ ��� ��������� 
// ��� ����������� � ����� ������� ������ � ���������
//
class AnimationManager {
	typedef std::map<std::string, MyAnimation *> AnimationMap;
	static AnimationMap _animations;
	static std::vector<HTEXTURE> _textures;
public:

	//
	// ��������� ���� ��������
	// � ������ ������ - ������ "true"
	// ����� ��������� ��������� ������ - �������� "���������������" ��������
	//
	static bool Load(const std::string &fileName);
	
	//
	// ��������� ��� �������� � �������� �� ������
	//
	static void UnloadAll();

	//
	// �������� ��������� �� �������� - animationId - ��� ��� �������� � ��������� 
	// (����� ������ �������� delete ��� ���!)
	//
	static MyAnimation *getAnimation(const std::string &animationId);

};

#endif//ANIMATION_H

