#ifndef CIRCLEPROGRESS_H
#define CIRCLEPROGRESS_H

#include "../Core/types.h"
#include "../Core/Render.h"

class CircleProgress {
public:
	CircleProgress();
	void Draw(float p);
	void DrawShow(float p);
	void Move(float x, float y);
	bool PixelCheck(const FPoint2D &pos);
	void SetIcon(const std::string &icon);
private:
	std::vector<hgeTriple> _cover;
	FPoint2D _offset;
	FPoint2D _center;
	int _filled;
	StaticSprite _origin;
	StaticSprite _icon;
	float _cx;
	float _cy;
};

#endif//CIRCLEPROGRESS_H