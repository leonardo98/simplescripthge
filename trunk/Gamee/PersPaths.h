#ifndef PERSPATH_H
#define PERSPATH_H

#include "../Core/types.h"
#include "../Core/InputSystem.h"
#include "../Core/Render.h"

struct OneDot;

typedef std::vector<OneDot *> AllDots;
typedef std::vector<unsigned int> DotsIndex;

typedef std::vector<std::vector<unsigned int> > FastIndex;
typedef std::vector<std::vector<float> > FastIndexHelp;

struct OneDot {
	FPoint2D pos;
	DotsIndex connections;
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
	static FPoint2D RandomPos();
	static FPoint2D SearchNearest(const FPoint2D &pos);
private:
	void SetPair(int one, int two);
	static int IndexOf(const OneDot *dot);
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

	// part for generation
	float Search(int i, int j, int n);
	void Generate();
	static FastIndex _fastIndex;
	static FastIndexHelp _indexDistance;
};

#endif///PERSPATH_H