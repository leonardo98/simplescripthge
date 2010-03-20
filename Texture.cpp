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
	_hge = hgeCreate(HGE_VERSION);
}

Texture::Texture(HTEXTURE h, int x, int y, int width, int height)
	: hgeSprite(h, x, y, width, height)
{
	_hge = hgeCreate(HGE_VERSION);
}

Texture::~Texture()
{
	_hge->Release();
}

bool Texture::IsNotTransparent(int x, int y)
{
	if (x < 0 || y < 0) {
		return false;
	}
	HTEXTURE h;
	h = GetTexture();
	if (x >= _hge->Texture_GetWidth(h) || y >= _hge->Texture_GetHeight(h)) {
		return false;
	}
	DWORD *dw;
	dw = _hge->Texture_Lock(h, true, x, y, 1, 1);
	bool result = ((*dw) >> 24) > 0x7F;
	_hge->Texture_Unlock(h);
	return result;
}

void Texture::Render(const hgeVector &pos) {
	Render(pos.x, pos.y);
}

void Texture::Render(float x, float y) {
	hgeSprite::Render(x, y);
}