#include "Beauty.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

Beauty::~Beauty() {
	delete _sprite;
}

Beauty::Beauty(TiXmlElement *xe) 
: BeautyBase(xe)
{
	_fileName = xe->Attribute("texture");
	_sprite = new Sprite(Core::getTexture(_fileName)->GetTexture());
}

void Beauty::SaveToXml(TiXmlElement *xe) {
	BeautyBase::SaveToXml(xe);
	xe->SetAttribute("texture", _fileName.c_str());
}

void Beauty::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(Math::round(_pos.x), Math::round(_pos.y));
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_sx, _sy);
	Render::MatrixMove(Math::round(- Width() / 2), Math::round(- Height() / 2));
	_sprite->Render();
	Render::PopMatrix();
	BeautyBase::Draw();
}

void Beauty::DebugDraw(bool onlyControl) {
	if (!onlyControl) {
		Draw();
	}
	BeautyBase::DebugDraw();
}

bool Beauty::PixelCheck(const FPoint2D &point) { 
	return _sprite->HasPixel(point.x, point.y) || BeautyBase::PixelCheck(point); 
}

std::string Beauty::Type() { 
	return "Beauty"; 
}

int Beauty::Width() {
	return _sprite->Width();
}

int Beauty::Height() {
	return _sprite->Height();
}

Beauty::Beauty(const Beauty &b)
: BeautyBase(b)
{
	_fileName = b._fileName;
	_sprite = new Sprite(Core::getTexture(_fileName)->GetTexture());
}

Sprite *Beauty::LinkToSprite() {
	return _sprite;
}
