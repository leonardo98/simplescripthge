#ifndef BEAUTY_BASE_H
#define BEAUTY_BASE_H

#include "../Core/Render.h"

//
// ������� ����� ��� ��������� - ��� ��������� �������� ��� �������� ��� �������� ��������������
//

class BeautyBase 
{
protected:
	// ��� ���������� ����� ������ ��� ���������
	bool _canBeRotated;
	bool _canBeScaled;
	bool _mouseDown;

	// ��� ���������� ����� ��� ���������
	std::string _inGameType;
	FPoint2D _pos;
	float _angle;
	DWORD _color;
	float _sy, _sx;

	// ����� �� ����������� � �����������
	FPoint2D _mousePos;
	Matrix parent;
	bool _debugDraw;
	Sprite *scaleSide[4];
	Sprite *scale[4];

private:
	bool _scaleX;
	bool _scaleY;
	FPoint2D _beforeDragScale;
	Matrix rotateMatrix;
	Matrix scaleMatrix;

	FPoint2D _mouseDownPos;
	enum MoveStates{
		edit_none,
		edit_rotate,
		edit_scale,
		edit_move
	};

	MoveStates _state;
	Sprite *rotate;
	Sprite *center;

public:

	virtual ~BeautyBase();
	BeautyBase(const BeautyBase &b);
	BeautyBase(TiXmlElement *xe);

	virtual void Update(float dt) {}
	virtual void Draw();
	virtual void DebugDraw(bool onlyControl = false);
	virtual void MouseDown(const FPoint2D &mouse);
	virtual void MouseMove(const FPoint2D &mouse);
	virtual void MouseUp(const FPoint2D &mouse);
	virtual bool PixelCheck(const FPoint2D &point);
	virtual int Width() { return 1; }
	virtual int Height() { return 1; }
	virtual bool Command(const std::string &cmd) { return false; }

	virtual std::string Type() = 0;
	const std::string &InGameType() const { return _inGameType; }
	virtual void SaveToXml(TiXmlElement *xe);
	virtual void UpdateScreenRect() {}

	void MoveTo(float x, float y);// ��� �������������� ��������� �� ����� � ���������
	void ShiftTo(float dx, float dy);// ��� ����������� �����������
};

#endif//BEAUTY_BASE_H