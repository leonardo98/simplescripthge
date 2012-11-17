#include "Beauty.h"
#include "../Core/Core.h"

Beauty::~Beauty() {
	delete _sprite;
}

Beauty::Beauty(TiXmlElement *xe) 
: BeautyBase(xe)
{
	_fileName = xe->Attribute("texture");
	_sprite = new Sprite(Core::getTexture(_fileName)->GetTexture());
}

void Beauty::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixRotate(_angle);
	Render::MatrixMove(_sx, _sy);
	_sprite->Render();
	Render::PopMatrix();
	BeautyBase::Draw();
}

void Beauty::DebugDraw() {
	Draw();
	BeautyBase::DebugDraw();
}

bool Beauty::PixelCheck(FPoint2D point) { 
	return _sprite->HasPixel(point.x, point.y) || BeautyBase::PixelCheck(point); 
}

void Beauty::SaveToXml(TiXmlElement *xe) {
	BeautyBase::SaveToXml(xe);
	xe->SetAttribute("fileName", _fileName.c_str());
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
