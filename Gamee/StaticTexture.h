// StaticTexture.h: interface for the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_STATICTEXTURE_INCLUDED_)
#define _STATICTEXTURE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\Object.h"

class StaticTexture : public Object  
{
public:
	StaticTexture(TiXmlElement *xe);
	void Draw();
private:
	FPoint2D _pos;
	Texture *_texture;
};

#endif // !defined(_STATICTEXTURE_INCLUDED_)
