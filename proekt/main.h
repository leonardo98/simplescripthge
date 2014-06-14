#ifndef _MAIN_H
#define _MAIN_H
 
// ������, ����������� ��� ������ ���������
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <vector>
#include <math.h>

using namespace std;

class CVector3{
	
public:
	CVector3() {}
 
    // �����������, ���������������� ������
    CVector3(float X, float Y, float Z)
    {
        x = X; y = Y; z = Z;
    }

	// ����������� �������� +, ����� ���������� ���� � ����� �������
    CVector3 operator+(CVector3 vVector)
    {
        // ���������� +����������� ������
        return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
    }
 
    // ����������� �������� -
    CVector3 operator-(CVector3 vVector)
    {
        return CVector3(x-vVector.x, y-vVector.y, z-vVector.z);
    }
 
    // ����������� �������� *
    CVector3 operator*(float num)
    {
        return CVector3(x*num, y*num, z*num);
    }
 
    // ����������� �������� /
    CVector3 operator/(float num)
    {
        return CVector3(x/num,y/num,z/num);
    }


	float x,y,z;
};

struct CVector2{
	float x,y;
};

// ��� ��������� ��������. ��� ������������ ��� �������������� �������� ���������
// ������ � �������. ��� ���������� �������� ��� � ���, ����� ������ ������ � �������
// ������ �������� �����������. �� �� ����� �������� ���������� ���������.
struct tFace
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};
 
 
// ��� ��������� ������ ���������� � ���������. ��� ����� ���� ���������� ����� �����.
// ��������� �������� �� ������������, �� � ������� ��, ����� ����� ������� �� ���
// �������.
struct tMaterialInfo
{
	char  strName[255];			// ��� ��������
	char  strFile[255];			// ��� ����� ��������
	BYTE  color[3];				// ���� ������� (R, G, B)
	int   texureId;				// ID ��������
	float uTile;				// u-tiling �������� (������ �� ������������)
	float vTile;				// v-tiling �������� (������ �� ������������)
	float uOffset;				// u-offset �������� (������ �� ������������)
	float vOffset;				// v-offset �������� (������ �� ������������)
};
 
 
 
 
// �������� ��� ���������� � ������/�����.
// � �������� ������� ����� �������� �� ��� � ����� � 
// ��������� ����� LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject
{
	int  numOfVerts;			// ����� ������ � ������
	int  numOfFaces;			// ����� ��������� � ������
	int  numTexVertex;			// ����� ���������� ���������
	int  materialID;			// ID �������� ��� �������������, ������ ������� �������
	bool bHasTexture;			// TRUE ���� ���� ���������� ����� ��� ����� �������
	char strName[255];			// ��� �������
	CVector3  *pVerts;			// ������ ������ �������
	CVector3  *pNormals;			// ������� �������
	CVector2  *pTexVerts;			// ���������� ����������
	tFace *pFaces;				// �������� �������
};
 
 
 
// �������� ���������� � ������. ���� ������� �� �������� � �����. �� ����� ������������
// ����� ������� �� STL (Standart Template Library) ����� ��������� ��������� ��� ����������
// ����������.
struct t3DModel
{
	int numOfObjects;			// ����� �������� � ������
	int numOfMaterials;			// ����� ���������� ������
	vector<tMaterialInfo> pMaterials;	// ����� �������� ���������� (�������� � �����)
	vector<t3DObject> pObject;		// ������ �������� � ������
};
 
 

 
#endif
