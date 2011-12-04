// StaticTexture.h: interface for the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef STATICTEXTURE_H
#define STATICTEXTURE_H

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

#endif//STATICTEXTURE_H