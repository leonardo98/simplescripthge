/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
** 
** Класс для непосредственного воспроизведения анимации в игре
**
** При портировании нужно заменить:
** hgeVector - это класс точки в двумерном пространстве
** HTEXTURE  - указатель на используемую текстуру
**
*/

#ifndef ANIMATION_H
#define ANIMATION_H

#include "TinyXml.h"
#include <string>
#include <map>
#include "../Core/Matrix.h"
#include "MovingPart.h"
#include "hge.h"

namespace My {

class Animation
{
public:

	//
	// Конструктор - используется в менеджере загрузки анимации
	// параметры анимации читаются из XML - можно использовать любой парсер,
	// который умеет перебирать ноды и читать атрибуты,
	// здесь успользован TinyXml
	//
	Animation(TiXmlElement *xe, HTEXTURE hTexture);

	//
	// Деструктор - используется в менеджере загрузки анимации
	//
	virtual ~Animation();

	//
	// Настройить положение анимации на экране, 
	// "pos" - положение анимации на экране, 
	// чтобы нарисовать зеркально - ставим "mirror" в "true".
	// Метод лучше всего вызывать перед каждым вызывом Draw()
	// исключение может быть, если такая анимация на экране всего одна
	//
	void SetPos(const hgeVector &pos, bool mirror);

	//
	// Нарисовать анимацию на экране 
	// "position" - значение времени для анимации из диапазона [0, 1]
	//
	void Draw(float position);

	//
	// Время полного цикла анмиации 
	// эту функцию нужно использовать в Update для расчета 
	// значения "position" метода Draw
	// например, для зацикленой анимации можно считать так:
	// progress += deltaTime / anim->Time();
	// if(progress >= 1.f) progress = 0.f;
	//
	float Time()const;

private:
	float _time;
	hgeVector _pivotPos;
	std::vector<MovingPart *> _bones;
	Matrix _subPosition;
	
	// этот тот самый вектор, который будет выполнять роль стека матриц
	MatrixStack _matrixsStack;
};


//
// Пример примитивного менеджера анимаций
//
// К конкретных проектах рекомендую использовать его исходники 
// для встраивания в общую систему работы с ресурсами
//
class AnimationManager {
	typedef std::map<std::string, Animation *> AnimationMap;
	static AnimationMap _animations;
	static std::vector<HTEXTURE> _textures;
public:

	//
	// Загрузить файл анимации
	// в случае успеха - вернет "true"
	// можно загружать несколько файлов - менеджер "коллекционирует" анимации
	//
	static bool Load(const std::string &fileName);
	
	//
	// Выгрузить все анимации и текстуры из памяти
	//
	static void UnloadAll();

	//
	// Получить указатель на анимацию - animationId - это имя анимации в редакторе 
	// (ВАЖНО нельзя вызывать delete для нее!)
	//
	static Animation *getAnimation(const std::string &animationId);

};

};

#endif//ANIMATION_H

