#include "AnimationArea.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "hgeRect.h"

AnimationArea::~AnimationArea() {
	delete _icon;
}

AnimationArea::AnimationArea(TiXmlElement *xe) 
: BeautyBase(xe)
{
	_icon = new Sprite(Core::getTexture("data/coloredpolygon.png")->GetTexture());
	_lib = xe->Attribute("file");
	_id = xe->Attribute("id");
	My::AnimationManager::Load(Render::GetDataDir() + _lib);
	_animation = My::AnimationManager::getAnimation(_id);
	_time = 0.f;
	_play = false;
	CalcWidthAndHeight();
	_shouldUpdateScreenRect = true;
}

AnimationArea::AnimationArea(const AnimationArea &b)
: BeautyBase(b)
{
	_icon = new Sprite(Core::getTexture("data/coloredpolygon.png")->GetTexture());
	_lib = b._lib;
	_id = b._id;
	_animation = My::AnimationManager::getAnimation(_id);
	_time = 0.f;
	_play = false;
	_width = b._width;
	_height = b._height;
	_shiftX = b._shiftX;
	_shiftY = b._shiftY;
	_shouldUpdateScreenRect = true;
}

void AnimationArea::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(Math::round(_pos.x), Math::round(_pos.y));
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_sx, _sy);
	Render::MatrixMove(_shiftX, _shiftY);
	_drawMatrix = Render::GetCurrentMatrix();
	_animation->Draw(_time);
	Render::PopMatrix();
	BeautyBase::Draw();
}

void AnimationArea::DebugDraw(bool onlyControl) {
	if (!onlyControl) {
		Draw();
	}
	BeautyBase::DebugDraw();
}

bool AnimationArea::PixelCheck(const FPoint2D &point) {
	Matrix m;
	m.MakeRevers(_drawMatrix);
	FPoint2D r(point);
	m.Mul(r);
	if (_screenRect.TestPoint(r.x, r.y)) {
		return true;
		//_animation->EncapsulateAllDots(
		//return _animation->PixelCheck(point) || BeautyBase::PixelCheck(point); 
	}
	return BeautyBase::PixelCheck(point); 
}

void AnimationArea::SaveToXml(TiXmlElement *xe) {
	BeautyBase::SaveToXml(xe);
	xe->SetAttribute("file", _lib.c_str());
	xe->SetAttribute("id", _id.c_str());
}

std::string AnimationArea::Type() { 
	return "Animation"; 
}

int AnimationArea::Width() {
	return _width;
}

int AnimationArea::Height() {
	return _height;
}

Sprite *AnimationArea::LinkToSprite() {
	return _icon;
}

void AnimationArea::Update(float dt) {
	if (_shouldUpdateScreenRect) {
		UpdateScreenRect();
		_shouldUpdateScreenRect = false;
	}
	if (_play) {
		_time += dt / _animation->Time();
		if (_time >= 1.f) {
			_time = 0.f;
			_play = false;
		}
	}
}

bool AnimationArea::Command(const std::string &cmd) {
	if (cmd == "play") {
		_play = true;
		_time = 0.f;
		return true;
	} else if (cmd == "rewind") {
		_time += 0.2499f;
		if (_time > 1.f) {
			_time = 0.f;
		}
		return true;
	}
	return (cmd == "" || BeautyBase::Command(cmd));
}

void AnimationArea::CalcWidthAndHeight() {
	std::vector<FPoint2D> dots;
	hgeRect rect;	
	rect.Clear();
	_animation->EncapsulateAllDots(0.0f, rect);
	_animation->EncapsulateAllDots(0.5f, rect);
	_animation->EncapsulateAllDots(1.0f, rect);
	_width = fabs(rect.x2 - rect.x1);
	_height = fabs(rect.y2 - rect.y1);
	_shiftX = -(rect.x2 + rect.x1) / 2;
	_shiftY = -(rect.y2 + rect.y1) / 2;
}

void AnimationArea::UpdateScreenRect() {
	_screenRect.Clear();
	_animation->EncapsulateAllDots(0.0f, _screenRect);
	_animation->EncapsulateAllDots(0.5f, _screenRect);
	_animation->EncapsulateAllDots(1.0f, _screenRect);
}