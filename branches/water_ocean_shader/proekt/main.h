#ifndef _MAIN_H
#define _MAIN_H
 
// Хидеры, необходимые для работы программы
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
 
    // Конструктор, инициализирующий данные
    CVector3(float X, float Y, float Z)
    {
        x = X; y = Y; z = Z;
    }

	// Перегружаем оператор +, чтобы прибавлять друг к другу векторы
    CVector3 operator+(CVector3 vVector)
    {
        // Возвращаем +добавленный вектор
        return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
    }
 
    // перегружаем оператор -
    CVector3 operator-(CVector3 vVector)
    {
        return CVector3(x-vVector.x, y-vVector.y, z-vVector.z);
    }
 
    // Перегружаем оператор *
    CVector3 operator*(float num)
    {
        return CVector3(x*num, y*num, z*num);
    }
 
    // Перегружаем оператор /
    CVector3 operator/(float num)
    {
        return CVector3(x/num,y/num,z/num);
    }


	float x,y,z;
};

struct CVector2{
	float x,y;
};

// Это структура полигона. Она используется для индексирования массивов координат
// вершин и текстур. Эта информация сообщает нам о том, какие номера вершин в массиве
// какому полигону принадлежат. То же самое касается текстурных координат.
struct tFace
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};
 
 
// Эта структура хранит информацию о материале. Это может быть текстурная карта света.
// Некоторые значения не используются, но я оставил их, чтобы могли увидеть их для
// примера.
struct tMaterialInfo
{
	char  strName[255];			// Имя текстуры
	char  strFile[255];			// Имя файла текстуры
	BYTE  color[3];				// Цвет обьекта (R, G, B)
	int   texureId;				// ID текстуры
	float uTile;				// u-tiling текстуры (Сейчас не используется)
	float vTile;				// v-tiling текстуры (Сейчас не используется)
	float uOffset;				// u-offset текстуры (Сейчас не используется)
	float vOffset;				// v-offset текстуры (Сейчас не используется)
};
 
 
 
 
// Содержит всю информацию о модели/сцене.
// В реальном проекте лучше оберните всё это в класс с 
// функциями вроде LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject
{
	int  numOfVerts;			// Число вершин в модели
	int  numOfFaces;			// Число полигонов в модели
	int  numTexVertex;			// Число текстурных координат
	int  materialID;			// ID текстуры для использования, индекс массива текстур
	bool bHasTexture;			// TRUE если есть текстурная карта для этого обьекта
	char strName[255];			// Имя обьекта
	CVector3  *pVerts;			// Массив вершин обьекта
	CVector3  *pNormals;			// Нормали обьекта
	CVector2  *pTexVerts;			// Текстурные координаты
	tFace *pFaces;				// Полигоны обьекта
};
 
 
 
// Содержит информацию о модели. Тоже неплохо бы обернуть в класс. Мы будем использовать
// класс вектора из STL (Standart Template Library) чтобы уменьшить трудности при связывании
// параметров.
struct t3DModel
{
	int numOfObjects;			// Число обьектов в модели
	int numOfMaterials;			// Число материалов модели
	vector<tMaterialInfo> pMaterials;	// Число обьектов материалов (текстуры и цвета)
	vector<t3DObject> pObject;		// Список обьектов в модели
};
 
 

 
#endif
