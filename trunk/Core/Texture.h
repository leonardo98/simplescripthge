// Texture.h: interface for the Texture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TEXTURE_INCLUDED_)
#define _TEXTURE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

#ifndef IOS_COMPILE_KEY
class Texture 
	: public hgeSprite
{
public:
	Texture(HTEXTURE h, int x, int y, int width, int height);
	virtual ~Texture();
	bool IsNotTransparent(int x, int y);
	void Render(const FPoint2D &pos);
	void Render(float x, float y);
};
#else 
class Texture 
{
public:
	Texture(HTEXTURE h, int x, int y, int width, int height);
	virtual ~Texture();
	bool IsNotTransparent(int x, int y);
	void Render(const FPoint2D &pos);
	void Render(float x, float y);
};
#endif //IOS_COMPILE_KEY

#endif // !defined(_TEXTURE_INCLUDED_)
