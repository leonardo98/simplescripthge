#ifndef BEAUTY_BASE_H
#define BEAUTY_BASE_H

#include "../Core/Render.h"

//
// Базовый класс для украшений - для статичной картинки или анимации или цветного многоугольника
//

class BeautyBase 
{
protected:
	// эти переменные нужны только для редактора
	bool _canBeRotated;
	bool _canBeScaled;
	bool _linearFiltering;

	// эти переменные общие для украшений
	FPoint2D _pos;
	float _angle;
	DWORD _color;
	float _sy, _sx;

private:
	bool _debugDraw;
	bool _mouseDown;
	bool _scaleX;
	bool _scaleY;
	FPoint2D _beforeDragScale;
	Matrix parent;

	FPoint2D _mouseDownPos;
	FPoint2D _mousePos;
	enum MoveStates{
		edit_none,
		edit_rotate,
		edit_scale,
		edit_move
	};

	MoveStates _state;
	Sprite *move;
	Sprite *scale[4];
	Sprite *scaleSide[4];
	Sprite *rotate;
	Sprite *center;

public:

	virtual ~BeautyBase();
	BeautyBase(TiXmlElement *xe);

	virtual void Update(float dt) {}
	virtual void Draw();
	virtual void DebugDraw();
	virtual void MouseDown(const FPoint2D &mouse);
	virtual void MouseMove(const FPoint2D &mouse);
	virtual void MouseUp(const FPoint2D &mouse);
	virtual bool PixelCheck(const FPoint2D &point);
	virtual int Width() = 0;
	virtual int Height() = 0;

	virtual std::string Type() = 0;
	virtual void SaveToXml(TiXmlElement *xe);

	void MoveTo(float x, float y);// для первоначальной установки на экран в редакторе
};

#endif//BEAUTY_BASE_H