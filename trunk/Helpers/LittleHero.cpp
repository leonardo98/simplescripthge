#include "LittleHero.h"

Lines::Lines(const DotsList &dots) 
: _offset(0.f, 0.f)
{
	_dots = dots;		
}

//float Lines::GetDistance(const FPoint2D &dot) const;
//bool Lines::Intersection(const FPoint2D &start, const FPoint2D &end) const;

void Lines::Move(const FPoint2D &offset) {
	for (DotsList::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		(*i) += offset;
	}
	_offset += offset;
}

const FPoint2D &Lines::GetOffset() const {
	return _offset;
}


bool ProjectPointAgainstLine(const FPoint2D &t1, const FPoint2D &t2, const FPoint2D &p, FPoint2D &p0)
{
	FPoint2D v1 = p - t1;
	FPoint2D v2 = t2 - t1;
	float v2QuadLen = v2.x * v2.x + v2.y * v2.y;
	p0 = t1 + v2  * ((v1.Dot(&v2)) / v2QuadLen);
	if((v1.Dot(&v2) >= 0.0f) && ((v1.Dot(&v2)) / (v2QuadLen) <= 1.0f)) {
		return true;
	} else {
		return false;
	}
}

bool Lines::MoveMeIfContact(FPoint2D &pos, float radius) {
	_dots.push_back(_dots.front());// замыкаем
	for (unsigned int i = 0, e = _dots.size() - 1; i != e; ++i) {
		const FPoint2D &a = _dots[i];
		const FPoint2D &b = _dots[i + 1];
		FPoint2D r;
		if (ProjectPointAgainstLine(a, b, pos, r)) {
			FPoint2D shift(pos - r);
			if (shift.Length() < radius) {
				pos = shift * radius / shift.Length() + r;
				_dots.pop_back();
				return true;
			}
		}
	}
	_dots.pop_back();
	return false;
}



LittleHero::LittleHero(float x, float y, float radius) 
: _pos(x, y)
, _impulse(0.f, 0.f)
, _currentSpeed(0.f, 0.f)
, _minSpeed(0.f)
, _radius(radius)
, _speed(0.f)
, _mass(1.f)
, _needRecalc(true)
{
}

void LittleHero::SetPosition(const FPoint2D &position) {
	_pos = position;
}

FPoint2D LittleHero::GetPosition() const {
	return _pos;
}

bool LittleHero::IsGround() {
	if (_needRecalc) {
		FPoint2D pos(_pos + _gravity * 1.f / 100.f);
		FPoint2D old = pos;
		for (AllLines::iterator i = _allLines.begin(), e = _allLines.end(); i != e; ++i) {
			if ((*i)->MoveMeIfContact(pos, _radius)) {
				break;
			}
		}
		pos -= old;
		_isContact = pos.Length() > 1e-3 && _gravity.Dot(&pos) < 0.f;
		_needRecalc = false;
	}
	return _isContact;
}

void LittleHero::SetImpulse(const FPoint2D &impulse) {
	_impulse = impulse;
}

void LittleHero::SetGravity(const FPoint2D &force) {
	_gravity = force;
}

void LittleHero::SetSpeed(float speed) {
	_speed = speed;
}

void LittleHero::Update(float dt) {
	float currentSpeed = _currentSpeed.Length();
	if (currentSpeed < 1e-3) {
		_currentSpeed.x = _minSpeed;// узнать угол земли и заменить на вектор параллельный ей
		_currentSpeed.y = 0.f;
	} else if (currentSpeed < (_minSpeed - 1e-3)) {
		_currentSpeed *= (_minSpeed / currentSpeed); // тут тоже надо корректировать с учетом угла наклона земли
	}
	if (_impulse.Length() > 1e-3) {
		_currentSpeed += _impulse / _mass;
		_impulse.x = 0.f;
		_impulse.y = 0.f;
	}
	if (!IsGround()) {
		_currentSpeed += _gravity * dt;
	}
	FPoint2D newPos(_pos + _currentSpeed * dt);
	// корректируем положение вверх или вних если произошло столкновение с линиями
	for (AllLines::iterator i = _allLines.begin(), e = _allLines.end(); i != e; ++i) {
		(*i)->MoveMeIfContact(newPos, _radius);
	}
	_pos = newPos;
	_needRecalc = true;
}

Lines * LittleHero::AddLinesSet(const DotsList &dots) {
	Lines *polygon = new Lines(dots);
	_allLines.push_back(polygon);
	return polygon;
}

void LittleHero::RemoveLinesSet(Lines *lines) {
	if (lines == NULL) {
		delete _allLines.front();
		_allLines.pop_front();
		return;
	}
	for (AllLines::iterator i = _allLines.begin(), e = _allLines.end(); i != e; ++i) {
		if (*i == lines) {
			delete (*i);
			return;
		}
	}
	assert(false);
}

void LittleHero::SetMinSpeed(float speed) {
	_minSpeed = speed;
}

LittleHero::AllLines &LittleHero::GetAllLines() {
	return _allLines;
}

void LittleHero::SetSpeedVector(const FPoint2D &speed) {
	_currentSpeed = speed;
}
