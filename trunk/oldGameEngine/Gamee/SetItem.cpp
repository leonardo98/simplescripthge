#include "SetItem.h"
#include "../Core/Math.h"

SetItem::~SetItem() {
	delete _sprite;
	Render::GetDC()->Texture_Free(_texture);
}

SetItem::SetItem(Types type) 
: _type(type)
, _drawOrder(0)
{
}

SetItem::SetItem(Types type, const char *fileName, const FPoint2D &pos, float angle) 
: _type(type)
, _drawOrder(0)
{
	_filePath = fileName;
	_angle = angle;
	_pos = pos;

	_texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + _filePath).c_str());
	_sprite = new Sprite(_texture, 0, 0, Render::GetDC()->Texture_GetWidth(_texture), Render::GetDC()->Texture_GetHeight(_texture));
}

bool SetItem::HasPixel(float x, float y) {
	return _sprite->HasPixel(x, y);
}

FPoint2D &SetItem::Pos() {
	return _pos;
}

float &SetItem::Angle() {
	return _angle;
}

SetItem::Types SetItem::Type() {
	return _type;
}

void SetItem::SaveToXml(TiXmlElement *xe) const {
	xe->SetAttribute("filePath", _filePath.c_str());
	char s[16];
	sprintf(s, "%f", _angle);
	xe->SetAttribute("angle", s);
	sprintf(s, "%f", _pos.x);
	xe->SetAttribute("x", s);
	sprintf(s, "%f", _pos.y);
	xe->SetAttribute("y", s);
	sprintf(s, "%i", _drawOrder);
	xe->SetAttribute("draw_order", s);
}

void SetItem::Draw() const {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixRotate(_angle / 180 * M_PI);
	_sprite->Render(- _sprite->Width() / 2, - _sprite->Height());
	Render::PopMatrix();
}

void SetItem::LoadFromXml(TiXmlElement *xe) {
	_filePath = xe->Attribute("filePath");
	_angle = atof(xe->Attribute("angle"));
	_pos.x = atof(xe->Attribute("x"));
	_pos.y = atof(xe->Attribute("y"));
	_drawOrder = (xe->Attribute("draw_order") ? atoi(xe->Attribute("draw_order")) : 0);

	_texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + _filePath).c_str());
	_sprite = new Sprite(_texture, 0, 0, Render::GetDC()->Texture_GetWidth(_texture), Render::GetDC()->Texture_GetHeight(_texture));
}

GroundLine::GroundLine() 
: SetItem(beauty_item)
, _maxIndex(19)
{}

GroundLine::GroundLine(const FPoint2D &pos, float angle, float scale, bool mirror) 
: SetItem(beauty_cover, std::string("data\\covers\\a_grass_part01.png").c_str(), pos, angle)
, _scale(scale)
, _mirror(mirror)
, _index(0)
, _maxIndex(19)
{	
}

void GroundLine::SaveToXml(TiXmlElement *xe) const {
	SetItem::SaveToXml(xe);
	xe->SetAttribute("mirror", _mirror ? "1" : "0");
	char s[16];
	sprintf(s, "%f", _scale);
	xe->SetAttribute("scale", s);
	sprintf(s, "%i", _index);
	xe->SetAttribute("index", s);
}

void GroundLine::Draw() const {
	Render::PushMatrix();
	Render::MatrixMove(Math::round(_pos.x), Math::round(_pos.y));
	Render::MatrixMove(_scale, _scale);
	Render::MatrixRotate(_angle / 180 * M_PI);
	if (_mirror) {
		Render::MatrixScale(-1.f, 1.f);
	}
	Render::GetDC()->System_SetState(HGE_TEXTUREFILTER, false);
	_sprite->Render(- _sprite->Width() / 2, - _sprite->Height() / 2);
	Render::GetDC()->System_SetState(HGE_TEXTUREFILTER, true);
	Render::PopMatrix();
}

void GroundLine::LoadFromXml(TiXmlElement *xe) {
	SetItem::LoadFromXml(xe);
	_mirror = atoi(xe->Attribute("mirror")) != 0;
	_scale = atof(xe->Attribute("scale"));
	_index = atoi(xe->Attribute("index"));
}

void GroundLine::Change(int p) {
	/* рабочий вариант но почему-то не понравился А.Митчанину, поэтому и закомментировал @Леня
	delete _sprite;
	Render::GetDC()->Texture_Free(_texture);
	
	_index = (_index + p) % _maxIndex;
	while (_index < 0) {
		_index += _maxIndex;
	}
	
	char buff[10];
	sprintf(buff, "%i", _index + 1);
	std::string s(buff);
	if (!(1 <= _index + 1 && _index + 1 <= 19)) {
		_index = _index;
	}
	_filePath = std::string("data\\beauty\\a_grass_part") + (s.size() == 1 ? "0" : "") + s + ".png";
	_texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + _filePath).c_str());
	_sprite = new Sprite(_texture, 0, 0, Render::GetDC()->Texture_GetWidth(_texture), Render::GetDC()->Texture_GetHeight(_texture));
	*/
}

void GroundLine::SetFileName(const std::string &fileName) {
	delete _sprite;
	Render::GetDC()->Texture_Free(_texture);
	
	_filePath = std::string("data\\covers\\") + fileName;
	_texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + _filePath).c_str());
	_sprite = new Sprite(_texture, 0, 0, Render::GetDC()->Texture_GetWidth(_texture), Render::GetDC()->Texture_GetHeight(_texture));
}
