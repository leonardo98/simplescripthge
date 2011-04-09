// Texture.cpp: implementation of the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include "Texture.h"
#include "Render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Texture::Texture(HTEXTURE h, int x, int y, int width, int height)
	: hgeSprite(h, x, y, width, height)
{
}

Texture::~Texture()
{
}

bool Texture::IsNotTransparent(int x, int y)
{
	if (x < 0 || y < 0) {
		return false;
	}
	HTEXTURE h;
	h = GetTexture();
	if (x >= Render::GetDC()->Texture_GetWidth(h) || y >= Render::GetDC()->Texture_GetHeight(h)) {
		return false;
	}
	DWORD *dw;
	dw = Render::GetDC()->Texture_Lock(h, true, x, y, 1, 1);
	bool result = ((*dw) >> 24) > 0x7F;
	Render::GetDC()->Texture_Unlock(h);
	return result;
}

void Texture::Render(const FPoint2D &pos) {
	Render(pos.x, pos.y);
}

void Texture::Render(float x, float y) {
	hgeSprite::Render(x, y);
}