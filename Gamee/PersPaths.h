#ifndef PERSPATH_H
#define PERSPATH_H

#include "../Core/types.h"
#include "../Core/InputSystem.h"
#include "../Core/Render.h"

struct OneDot;

typedef std::vector<OneDot *> AllDots;

struct OneDot {
	FPoint2D pos;
	AllDots connections;
	OneDot(float x, float y);
};

class PersPaths 
{
public:
	PersPaths(void);
	virtual ~PersPaths(void);
	void Draw();
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseMove(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	static FPoint2D NextPointTo(const FPoint2D &from, const FPoint2D &to);
	void SetVisible_debug(bool active);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
private:
	void SetPair(int one, int two);
	int IndexOf(const OneDot *dot);
	static int Find(const FPoint2D &pos);
	void Remove(const FPoint2D &pos);
	void Add(const FPoint2D &pos);
	void Save();
	static AllDots _dots;
	bool _visible;
	bool _mouseDown;
	int _selected;
	Texture *_texture;
	FPoint2D _lastMousePos;
	bool _changes;
};

#endif///PERSPATH_H