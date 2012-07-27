#ifndef LINE_FUNCTIONS_H
#define LINE_FUNCTIONS_H

// фейковая физика
// сильно упрощенная
// исхожу из предположения что герой не может никогда упереться в стену
// или затрять в узком тоннеле между полом и потолком
// что он может ВСЕГДА двигаться вперед, поэтому его положение в случае
//   касания препятствия может быть скорректировано изменением положения по 'y'
//   т.е. его положения по высоте 

#include "../Core/types.h"
#include <list>
#include <vector>

typedef std::vector<FPoint2D> DotsList;


// предполагаю что точки образуют замкнутый контур
class Lines {
public:
	Lines(const DotsList &dots);
	//float GetDistance(const FPoint2D &dot) const;
	//bool Intersection(const FPoint2D &start, const FPoint2D &end) const;
	void Move(const FPoint2D &offset);
	bool MoveMeIfContact(FPoint2D &pos, float radius);
	const FPoint2D &GetOffset() const;
private:
	DotsList _dots;
	FPoint2D _offset;
};

class LittleHero {
public:
	LittleHero(float x, float y, float radius);
	void SetPosition(const FPoint2D &position);
	FPoint2D GetPosition() const;
	bool IsGround();
	void SetImpulse(const FPoint2D &impulse);
	void SetSpeedVector(const FPoint2D &speed);
	void SetGravity(const FPoint2D &force);
	void SetSpeed(float speed);
	void Update(float dt);
	Lines * AddLinesSet(const DotsList &dots);
	void RemoveLinesSet(Lines *lines);
	void SetMinSpeed(float speed);
	typedef std::list<Lines *> AllLines;
	AllLines &GetAllLines();
private:
	AllLines _allLines;
	FPoint2D _pos;
	FPoint2D _impulse;
	FPoint2D _gravity;
	float _speed;
	FPoint2D _currentSpeed;
	float _radius;
	float _mass;
	float _minSpeed;
	bool _isContact;
	bool _needRecalc;
};

#endif//LINE_FUNCTIONS_H