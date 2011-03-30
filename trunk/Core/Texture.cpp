// Texture.cpp: implementation of the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include "Texture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Texture::Texture(hgeSprite &sprite)
	: hgeSprite(sprite)
{
	_dc = hgeCreate(HGE_VERSION);
}

Texture::Texture(HTEXTURE h, int x, int y, int width, int height)
	: hgeSprite(h, x, y, width, height)
{
	_dc = hgeCreate(HGE_VERSION);
}

Texture::~Texture()
{
	_dc->Release();
}

bool Texture::IsNotTransparent(int x, int y)
{
	if (x < 0 || y < 0) {
		return false;
	}
	HTEXTURE h;
	h = GetTexture();
	if (x >= _dc->Texture_GetWidth(h) || y >= _dc->Texture_GetHeight(h)) {
		return false;
	}
	DWORD *dw;
	dw = _dc->Texture_Lock(h, true, x, y, 1, 1);
	bool result = ((*dw) >> 24) > 0x7F;
	_dc->Texture_Unlock(h);
	return result;
}

void Texture::Render(const FPoint2D &pos) {
	Render(pos.x, pos.y);
}

void Texture::Render(float x, float y) {
	hgeSprite::Render(x, y);
}