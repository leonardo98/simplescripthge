// Texture.h: interface for the Texture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TEXTURE_INCLUDED_)
#define _TEXTURE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
//#include "hgeSprite.h"
//#include "FPoint2D.h"

class Texture 
	: public  hgeSprite  
{
public:
	Texture(hgeSprite &sprite);
	Texture(HTEXTURE h, int x, int y, int width, int height);
	virtual ~Texture();
	bool IsNotTransparent(int x, int y);
	void Render(const FPoint2D &pos);
	void Render(float x, float y);
private:
	DeviceContext _dc;
};

#endif // !defined(_TEXTURE_INCLUDED_)
