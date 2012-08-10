/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
**
** ����� ���������� ������� - "������" ����� ������� ��������
** 
** ��� ������������ ����� ��������:
**     HGE * - ��������� �� ������-��������, ����� ��� ��������� hgeQuad
** hgeVector - ��� ����� ����� � ��������� ������������
**  HTEXTURE - ��������� �� ������������ ��������
**   hgeQuad - ��������� ��� ������ ������� �� �����,
**             ����� 4 �������, ��� ������� ����� ������ 
**             ���������� � �������� ����������
**
*/

#ifndef MOVINGPART_H
#define MOVINGPART_H

#include <string>
#include "TinyXml.h"
#include "../Core/MotionValues.h"
#include "../Core/Matrix.h"

namespace My {

// ���������� ��� �������, ������� ����� ��������� ���� ����� ������
typedef std::vector<Matrix> MatrixStack;

#define fatof(a) static_cast<float>(atof(a))

enum OffParentOrder {
	OffParentOrder_top,
	OffParentOrder_bottom
};

class MovingPart
{
public:

	//
	// ����������
	//
	~MovingPart();

	//
	// ����������� - ������ ������ ������� �� XML
	// ���������� ��������, ������� �� ������� ������
	//
	MovingPart(TiXmlElement * xe, HTEXTURE hTexture);

	//
	// ������� ������ �� �����
	// ��������� �� ������� �������� - p, [ 0<= p <= 1 ]
	// ��������� �� ������ - stack
	//
	void Draw(float p, MatrixStack & stack); 

private:

	std::string boneName;
	typedef std::vector<MovingPart *> List;
	
	hgeVector _center;
	MotionValues _x;
	MotionValues _y;
	MotionValues _angle;
	MotionValues _scaleX;
	MotionValues _scaleY;
	MotionValues::Motion _movingType;

	List _topBone;// links
	List _bottomBone;// links
	List _bones;
	OffParentOrder _offparent;
	
	void ResortBones();

	//
	// ���� ���� ���������� ���� ������ HGE, ����������� � ������ �����
	//
	void CreateQuad(HTEXTURE hTexture, const std::string &texture);
	bool _hasTexture;
	hgeQuad _quad;
	hgeVector _origin[4];	
	HGE *_hge;

};

};

#endif//MOVINGPART_H