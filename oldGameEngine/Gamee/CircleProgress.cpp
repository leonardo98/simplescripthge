#include "CircleProgress.h"
#include "../Core/Core.h"

CircleProgress::CircleProgress()
: _text("")
{
	Texture *texture = Core::getTexture("textures\\clouds\\icon_b_bubble.png");
	_origin.Set(texture);
	_center.x = 59;
	_center.y = 45;
	texture = Core::getTexture("textures\\clouds\\icon_b_bubble_in.png");
	_cover.resize(8);
	float x[9] = {28.f, 57.f, 57.f, 57.f, 28.f, 0.f, 0.f, 0.f, 28.f};

	float y[9] = {57.f, 57.f,  83.f,  108.f, 108.f, 108.f, 83.f,  57.f,  57.f};
	_cx = 28;
	_cy = 82;
	float w = Render::GetDC()->Texture_GetWidth(texture->GetTexture(), true);
	float h = Render::GetDC()->Texture_GetHeight(texture->GetTexture(), true);
	for (int i = 0; i < _cover.size(); ++i) {
		_cover[i].tex = texture->GetTexture();
		_cover[i].blend = BLEND_DEFAULT;
		_cover[i].v[0].x = _cx;
		_cover[i].v[0].y = _cy;
		_cover[i].v[0].z = 0.f;
		_cover[i].v[0].tx = _cx / w;
		_cover[i].v[0].ty = _cy / h;
		_cover[i].v[0].col = 0xFFFFFFFF;

		_cover[i].v[1].x = x[i];
		_cover[i].v[1].y = y[i];
		_cover[i].v[1].z = 0.f;
		_cover[i].v[1].tx = x[i] / w;
		_cover[i].v[1].ty = y[i] / h;
		_cover[i].v[1].col = 0xFFFFFFFF;

		_cover[i].v[2].x = x[i + 1];
		_cover[i].v[2].y = y[i + 1];
		_cover[i].v[2].z = 0.f;
		_cover[i].v[2].tx = x[i + 1] / w;
		_cover[i].v[2].ty = y[i + 1] / h;
		_cover[i].v[2].col = 0xFFFFFFFF;
	}
}

void CircleProgress::DrawShow(float p) {
	Matrix t;
	t.Move(_offset.x, _offset.y);
	t.Scale(p, p);
	t.Move(- _center.x, - _center.y);
	_origin.SetTransform(t);
	_origin.Render();
	Render::PushMatrix();
	if (_text.size()) {
		Render::PrintString(_offset.x - _origin.SpriteWidth() / 2, _offset.y - _origin.SpriteHeight() / 2, "data\\fonts\\arialblack20.fnt", _text);
	} else {
		Render::MatrixMove(_offset.x - _icon.SpriteWidth() / 2, _offset.y - _icon.SpriteHeight() / 2 - 3);
		_icon.Render();
	}
	Render::PopMatrix();
}

void CircleProgress::Draw(float p) {
	assert(0.f <= p);
	if (p > 0.999f) {
		p = 0.999f;
	}
	DWORD red;
	DWORD green;
	DWORD blue;
	if (p < 0.25f) {
		float f = p / 0.25f;
		red   =  91 + ( 83 -  91) * f;
		green = 253 + (251 - 253) * f;
		blue  =  78 + ( 65 -  78) * f;
	} else if (p < 0.5f) {
		float f = (p - 0.25f) / 0.25f;
		red   =  83 + (255 -  83) * f;
		green = 251 + (227 - 251) * f;
		blue  =  65 + ( 25 -  65) * f;
	} else if (p < 0.75f) {
		float f = (p - 0.5f) / 0.25f;
		red   = 255 + (255 - 255) * f;
		green = 227 + (144 - 227) * f;
		blue  =  25 + (  7 -  25) * f;
	} else {
		float f = (p - 0.75f) / 0.25f;
		red   = 255 + (255 - 255) * f;
		green = 144 + (  0 - 144) * f;
		blue  =   7 + (  0 -   7) * f;
	}
	DWORD color = 0xFF000000 | red << 16 | green << 8 | blue;
	_origin.Render();
	hgeTriple t;
	for (int i = 0; i < static_cast<int>(8 * p); ++i) {
		t = _cover[i];
		for (int k = 0; k < 3; ++k) {
			t.v[k].x += (_offset.x - _cx);
			t.v[k].y += (_offset.y - _cy);
			t.v[k].col = color;
		}
		Render::Draw(t);
	}
	int last = static_cast<int>(8 * p);
	float x = sin(2 * M_PI * p);
	float y = cos(2 * M_PI * p);

	float angle = 2 * M_PI * p - last * M_PI / 4;
	if (last % 2 == 0) {
		p = tan(angle);
	} else {
		p = 1 - tan(M_PI / 4 - angle);
	}
	t = _cover[last];
	t.v[2].x = (t.v[2].x - t.v[1].x) * p + t.v[1].x;
	t.v[2].y = (t.v[2].y - t.v[1].y) * p + t.v[1].y;
	t.v[2].tx = (t.v[2].tx - t.v[1].tx) * p + t.v[1].tx;
	t.v[2].ty = (t.v[2].ty - t.v[1].ty) * p + t.v[1].ty;
	for (int k = 0; k < 3; ++k) {
		t.v[k].x += (_offset.x - _cx);
		t.v[k].y += (_offset.y - _cy);
		t.v[k].col = color;
	}
	Render::Draw(t);
	Render::PushMatrix();
	if (_text.size()) {
		Render::PrintString(_offset.x - _origin.SpriteWidth() / 2, _offset.y - _origin.SpriteHeight() / 2, "data\\fonts\\arialblack20.fnt", _text, 0xFF000000);
	} else {
		Render::MatrixMove(_offset.x - _icon.SpriteWidth() / 2, _offset.y - _icon.SpriteHeight() / 2 - 3);
		_icon.Render();
	}
	Render::PopMatrix();
}

void CircleProgress::Move(float x, float y) {
	Matrix t;
	t.Move(x - _center.x, y - _center.y);
	_origin.SetTransform(t);
	_offset.x = x;
	_offset.y = y;
}

bool CircleProgress::PixelCheck(const FPoint2D &pos) {
	return _origin.PixelCheck(pos);
}

void CircleProgress::SetIcon(const std::string &icon) {
	if (Core::isTexture("textures\\clouds\\icon_" + icon + ".png")) {
		_icon.Set(Core::getTexture("textures\\clouds\\icon_" + icon + ".png"));
	} else {
		_icon.Set(Core::getTexture(icon));
	}
}

void CircleProgress::SetText(const std::string &text) {
	_text = text;
}
