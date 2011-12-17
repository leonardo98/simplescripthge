// StaticTexture.h: interface for the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_STATICTEXTURE_H
#define MYENGINE_STATICTEXTURE_H

#include "../Core/Object.h"
#include "../Core/Render.h"

class StaticTexture : public Object  
{
public:
	StaticTexture(TiXmlElement *xe);
	virtual ~StaticTexture();
	void Draw();
private:
	FPoint2D _pos;
	Texture *_texture;
	std::string _textureId;
};

#endif//MYENGINE_STATICTEXTURE_H