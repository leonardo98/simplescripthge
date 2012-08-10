/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
**
** Класс подвижного спрайта - "сердце" моего формата анимации
** 
** При портировании нужно заменить:
**     HGE * - указатель на рендер-контекст, нужен для отрисовки hgeQuad
** hgeVector - это класс точки в двумерном пространстве
**  HTEXTURE - указатель на используемую текстуру
**   hgeQuad - структура для вывода спрайта на экран,
**             имеет 4 вершины, для которых можно задать 
**             текстурные и экранные координаты
**
*/

#ifndef MOVINGPART_H
#define MOVINGPART_H

#include <string>
#include "TinyXml.h"
#include "../Core/MotionValues.h"
#include "../Core/Matrix.h"

namespace My {

// определяем тип вектора, который будет выполнять роль стека матриц
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
	// Деструктор
	//
	~MovingPart();

	//
	// Конструктор - читаем данные спрайта из XML
	// запоминаем текстуру, спрайты из которой рисуем
	//
	MovingPart(TiXmlElement * xe, HTEXTURE hTexture);

	//
	// Выводим спрайт на экран
	// положение во времени анимации - p, [ 0<= p <= 1 ]
	// положение на экране - stack
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
	// Этот блок использует типы движка HGE, подробности в начале файла
	//
	void CreateQuad(HTEXTURE hTexture, const std::string &texture);
	bool _hasTexture;
	hgeQuad _quad;
	hgeVector _origin[4];	
	HGE *_hge;

};

};

#endif//MOVINGPART_H