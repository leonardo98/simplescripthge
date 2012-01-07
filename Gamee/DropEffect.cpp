#include "DropEffect.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

DropEffect::DropEffect(const FPoint2D &startPos, const FPoint2D &offsetStartPos, const FPoint2D &endPos, const FPoint2D &offsetEndPos, const std::string &product, unsigned char endAlpha, float endScale) 
: _startPos(startPos)
, _endPos(endPos)
, _endAlpha(endAlpha)
, _endScale(endScale)
{
	_shadow = Core::getTexture("persshadow");
	_product = Core::getTexture("gui_" + product);
	const float SPEED = 600;
	_timeCounter = _time = ((startPos + offsetStartPos) - (endPos + offsetEndPos)).Length() / SPEED;
	_startPos.x += offsetStartPos.x;
	_endPos.x += offsetEndPos.x;
	_startHeight = - offsetStartPos.y;
	_endHeight = - offsetEndPos.y;
	_pos = _startPos;
	_dugaHeight = 50.f + (_startPos - _endPos).Length() / SPEED * 100.f;
}

void DropEffect::Draw() {
	Render::PushMatrix();
	float p = _timeCounter / _time;
	float h = Math::lerp(_endHeight, _startHeight, p);
	Render::MatrixMove(_pos.x, _pos.y  - _dugaHeight * sinf(M_PI * p) - h);
	Render::MatrixScale(Math::lerp(_endScale, 1.f, p), Math::lerp(_endScale, 1.f, p));
	_product->Render( - _product->Width() / 2, - _product->Height() / 2);
	Render::PopMatrix();
}

void DropEffect::DrawBottom() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	if (_endAlpha != 0xFF) {
		Render::SetAlpha(Math::lerp(_endAlpha / 255.f, 1.f, _timeCounter / _time) * 0xFF);
		_shadow->Render( - _shadow->Width() / 2,  - _shadow->Height() / 2);
		Render::SetAlpha(0xFF);
	} else {
		_shadow->Render( - _shadow->Width() / 2,  - _shadow->Height() / 2);
	}
	Render::PopMatrix();
}

void DropEffect::Update(float dt) {	
	if (_timeCounter > 0.f) {
		_timeCounter -= dt;
		if (_timeCounter <= 0.f) {
			_dead = true;
		} else {
			_pos = Math::lerp(_startPos, _endPos, 1.f - _timeCounter / _time);
		}
	}
}

float DropEffect::Time() {
	return _time;
}