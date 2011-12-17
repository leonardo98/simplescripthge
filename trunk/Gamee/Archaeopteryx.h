#ifndef MYENGINE_ARCHAEOPTERYX_H
#define MYENGINE_ARCHAEOPTERYX_H

#include "BaseElement.h"
#include "../Core/Animation.h"
#include "CircleProgress.h"

class Archaeopteryx : public BaseElement
{
public:
	Archaeopteryx();
	virtual void Draw();
	//virtual void DrawBottom();
	virtual void Update(float dt);
	FPoint2D GetDirection();
	static const float SPEED;
private:
	FPoint2D GetNewDirection();
	void SwitchAnimation();
	void SwitchToIdle();
	void SwitchToWalk();
    void SetupAnimation();
    std::string _birdsType;
    std::string _sex;
	Animation *_current;
	Animation *_leftFront;
	Animation *_leftFrontIdle;
	Animation *_left;
	Animation *_leftIdle;
	Animation *_leftBack;
	Animation *_lefteat;
	Texture *_shadow;
	bool _mirror;
	std::vector<FPoint2D> _region;

	FPoint2D _currentTarget;
	FPoint2D _endTarget;
	float _timeCounter;
	float _actionTimeCounter;
	enum BirdsStates {
		state_walk,
		state_idle,
		state_eat0,
		state_eat,
		state_drink,
		state_happy,
		state_hangry,
		state_angry,
		state_want_eat,
		state_want_drink,
		state_none
	};
    BirdsStates _state;
    BirdsStates _nextState;
	bool _boy;
	enum {
		age_young,
		age_adult
	} _age;
    std::list<BirdsStates> _adultCircleStates;
    float _lifeTimeCounter;
    float _waitingTimeCounter;
    CircleProgress _waitingProgress;
    FPoint2D _foodPos;
    FPoint2D _waterPos;
    FPoint2D _turnPoint;
    bool _waitBasePosition;
};

#endif//MYENGINE_ARCHAEOPTERYX_H