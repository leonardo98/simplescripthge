#ifndef ANIMATION_AREA_H
#define ANIMATION_AREA_H

#include "../2dga_api/Animation.h"
#include "BeautyBase.h"

class AnimationArea : public BeautyBase
{
public:

	virtual ~AnimationArea();
	AnimationArea(const AnimationArea &b);
	AnimationArea(TiXmlElement *xe);

	virtual void Draw();
	virtual void DebugDraw(bool onlyControl);
	virtual void Update(float dt);
	virtual bool PixelCheck(const FPoint2D &point);
	virtual bool Command(const std::string &cmd = "");

	virtual void SaveToXml(TiXmlElement *xe);
	virtual std::string Type();
	virtual void UpdateScreenRect();

	const std::string &Name() const { return _id; }

	virtual int Width();
	virtual int Height();

	virtual Sprite *LinkToSprite();

private:
	hgeRect _screenRect;
	Matrix _drawMatrix;
	std::string _id;
	std::string _lib;
	My::Animation *_animation;
	bool _play;
	float _time;
	int _width;
	int _height;
	int _shiftX;
	int _shiftY;
	void CalcWidthAndHeight();

	Sprite *_icon;
	bool _shouldUpdateScreenRect;
};


#endif//BEAUTY_H