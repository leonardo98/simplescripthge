#ifndef MYENGINE_CIRCLEPROGRESS_H
#define MYENGINE_CIRCLEPROGRESS_H

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
	void SetText(const std::string &text);
private:
	std::vector<hgeTriple> _cover;
	FPoint2D _offset;
	FPoint2D _center;
	int _filled;
	StaticSprite _origin;
	StaticSprite _icon;
	std::string _text;
	float _cx;
	float _cy;
};

#endif//MYENGINE_CIRCLEPROGRESS_H