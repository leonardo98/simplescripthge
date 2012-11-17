#ifndef BEAUTY_H
#define BEAUTY_H

#include "BeautyBase.h"

class Beauty : public BeautyBase
{
public:

	virtual ~Beauty();
	Beauty(const Beauty &b);
	Beauty(TiXmlElement *xe);

	virtual void Draw();
	virtual void DebugDraw();
	virtual void Update(float dt) {}
	virtual bool PixelCheck(FPoint2D point);

	virtual void SaveToXml(TiXmlElement *xe);
	virtual std::string Type();

	virtual int Width();
	virtual int Height();

private:
	std::string _fileName;
	Sprite *_sprite;
};


#endif//BEAUTY_H