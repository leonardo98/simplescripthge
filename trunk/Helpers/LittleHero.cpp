#include "LittleHero.h"
#include "../Core/Math.h"

Lines::Lines(const DotsList &dots, int index) 
: _offset(0.f, 0.f)
, splineIndex(index)
, _dots(dots)
{	
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

bool Lines::MoveMeIfContact(const FPoint2D &oldPos, FPoint2D &pos, float radius, FPoint2D &currentSpeed, bool &ground) {
	if ((oldPos - pos).Length() > radius) {
		for (unsigned int i = 0, e = _dots.size(); i != e; ++i) {
			const FPoint2D &a = _dots[i];
			const FPoint2D &b = _dots[(i + 1) % _dots.size()];
			FPoint2D r;
			if (Math::Intersection(a, b, oldPos, pos, &r)) {
				FPoint2D o(pos - r);
				float angle = o.Angle(&(a - b));
				o.Rotate(angle);
				currentSpeed.Rotate(angle);
				o.x *= -1;
				currentSpeed.x *= -1;
				o.Rotate(-angle);
				currentSpeed.Rotate(-angle);
				float angleTest = currentSpeed.Angle(&o);
				currentSpeed *= 0.1f;
				pos = o * 0.1f + r;
				return true;
			}
		}
	}

	for (unsigned int i = 0, e = _dots.size(); i != e; ++i) {
		const FPoint2D &a = _dots[i];
		const FPoint2D &b = _dots[(i + 1) % _dots.size()];
		const FPoint2D &c = _dots[(i + 2) % _dots.size()];
		FPoint2D r;
		if (ProjectPointAgainstLine(a, c, pos, r)) {
			FPoint2D shift(pos - r);
			if (shift.Length() < radius - 1e-3) {
				float angle = (pos - oldPos).Angle();
				pos = shift * radius / shift.Length() + r;
				if (shift.y < 0) {
					angle -= (pos - oldPos).Angle();
					float modSpeed = currentSpeed.Length();
					if (modSpeed > 1e-3 && shift.Length() > 1e-3) {
						currentSpeed = shift;
						currentSpeed.Rotate(M_PI_2);
						currentSpeed.Normalize();
						currentSpeed *= modSpeed * (1.f - 0.9f * min(fabs(angle) / M_PI_2, 1.f));
					}
					ground = true;

					if ((a.x <= b.x && r.x <= b.x) || (a.x >= b.x && r.x >= b.x)) {
						splinePos = (i + fabs(r.x - a.x) / fabs(b.x - a.x)) / _dots.size();
					} else {
						splinePos = (i + 1 + fabs(r.x - b.x) / fabs(c.x - b.x)) / _dots.size();
					}
				}
				return true;
			}
		}
	}
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
, _wasGround(false)
{
}

void LittleHero::SetPosition(const FPoint2D &position) {
	_pos = position;
}

FPoint2D LittleHero::GetPosition() const {
	return _pos;
}

bool LittleHero::IsGround() {
	return _wasGround;
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
	//if (!_wasGround) {
		_currentSpeed += _gravity * dt;
	//}
	FPoint2D newPos(_pos + _currentSpeed * dt);
	// корректируем положение вверх или вних если произошло столкновение с линиями
	FPoint2D oldPos(_pos);
	FPoint2D beforeContact(newPos);
	_wasGround = false;
	_splinePos = 0.f;
	for (AllLines::iterator i = _allLines.begin(), e = _allLines.end(); i != e; ++i) {
		bool oldGround(_wasGround);
		if ((*i)->MoveMeIfContact(oldPos, newPos, _radius, _currentSpeed, _wasGround)) {
			//_wasGround = delta.Length() > 1e-3 && _gravity.Dot(&delta) < 0.f;
			i = _allLines.begin();
			oldPos = beforeContact;
			beforeContact = newPos;
			if (_wasGround && !oldGround) {
				_splinePos = (*i)->splinePos;
				_splineIndex = (*i)->splineIndex;
			}
		}
	}
	FPoint2D delta(_pos - newPos);
	_pos = newPos;
}

Lines * LittleHero::AddLinesSet(const DotsList &dots, int index) {
	Lines *polygon = new Lines(dots, index);
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

FPoint2D & LittleHero::GetSpeedVector() {
	return _currentSpeed;
}

const FPoint2D & LittleHero::GetGravity() {
	return _gravity;
}

float LittleHero::GetMass() {
	return _mass;
}

float LittleHero::GetSplinePos(int &index) {
	index = _splineIndex;
	return _splinePos;
}
