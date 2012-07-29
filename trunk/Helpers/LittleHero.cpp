#include "LittleHero.h"
#include "../Core/Math.h"

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

bool Lines::MoveMeIfContact(const FPoint2D &oldPos, FPoint2D &pos, float radius, FPoint2D &currentSpeed, bool &ground) {
	_dots.push_back(_dots.front());// ��������

	if ((oldPos - pos).Length() > radius) {
		for (unsigned int i = 0, e = _dots.size() - 1; i != e; ++i) {
			const FPoint2D &a = _dots[i];
			const FPoint2D &b = _dots[i + 1];
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
				currentSpeed *= 0.1f;
				pos = o * 0.1f + r;
				_dots.pop_back();
				return true;
			}
		}
	}

	_dots.push_back(_dots[1]);
	for (unsigned int i = 0, e = _dots.size() - 2; i != e; ++i) {
		const FPoint2D &a = _dots[i];
		const FPoint2D &b = _dots[i + 1];
		const FPoint2D &c = _dots[i + 2];
		FPoint2D l1 = (a + b) * 0.5f;
		FPoint2D l2 = (b + c) * 0.5f;
		FPoint2D r;
		if (ProjectPointAgainstLine(a, c, pos, r)) {
			FPoint2D shift(pos - r);
			if (shift.Length() < radius - 1e-3) {
				float angle = (pos - oldPos).Angle();
				pos = shift * radius / shift.Length() + r;
				_dots.pop_back();
				_dots.pop_back();
				if (shift.y < 0) {
					angle -= (pos - oldPos).Angle();
					currentSpeed.Rotate(-angle);
					currentSpeed *= (1.f - 0.9f * min(fabs(angle) / M_PI_2, 1.f));
					ground = true;
				}
				return true;
			}
		//} else if ((a - pos).Length() < radius - 1e-3) {
		//	pos = (pos - a) * radius / (a - pos).Length() + a; 
		//	_dots.pop_back();
		//	return true;
		//} else if ((b - pos).Length() < radius - 1e-3) {
		//	pos = (pos - b) * radius / (b - pos).Length() + b; 
		//	_dots.pop_back();
		//	return true;
		}
	}
	_dots.pop_back();
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
		_currentSpeed.x = _minSpeed;// ������ ���� ����� � �������� �� ������ ������������ ��
		_currentSpeed.y = 0.f;
	} else if (currentSpeed < (_minSpeed - 1e-3)) {
		_currentSpeed *= (_minSpeed / currentSpeed); // ��� ���� ���� �������������� � ������ ���� ������� �����
	}
	if (_impulse.Length() > 1e-3) {
		_currentSpeed += _impulse / _mass;
		_impulse.x = 0.f;
		_impulse.y = 0.f;
	}
	if (!_wasGround) {
		_currentSpeed += _gravity * dt;
	}
	FPoint2D newPos(_pos + _currentSpeed * dt);
	// ������������ ��������� ����� ��� ���� ���� ��������� ������������ � �������
	FPoint2D oldPos(_pos);
	FPoint2D beforeContact(newPos);
	_wasGround = false;
	for (AllLines::iterator i = _allLines.begin(), e = _allLines.end(); i != e; ++i) {
		if ((*i)->MoveMeIfContact(oldPos, newPos, _radius, _currentSpeed, _wasGround)) {
			//_wasGround = delta.Length() > 1e-3 && _gravity.Dot(&delta) < 0.f;
			i = _allLines.begin();
			oldPos = beforeContact;
			beforeContact = newPos;
		}
	}
	FPoint2D delta(_pos - newPos);
	_pos = newPos;
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

FPoint2D & LittleHero::GetSpeedVector() {
	return _currentSpeed;
}
