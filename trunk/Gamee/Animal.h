#ifndef GAME1_ANIMAL_H
#define GAME1_ANIMAL_H

#include "BaseElement.h"
#include "../Core/Animation.h"
#include "CircleProgress.h"

class Animal : public BaseElement
{
public:
	enum AnimalStates {
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
		state_none,
		state_run_away,
		state_dodo_egg,
		state_animal_puh
	};
	Animal(const std::string &animalId = "");
	void InnerDraw();
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
	virtual bool IsUnderMouse(const FPoint2D &mousePos);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	FPoint2D GetDirection();
	static const float SPEED;
	AnimalStates GetState();
	void CutFluff();
private:
	FPoint2D GetNewDirection();
	void SwitchAnimation();
	void SwitchToIdle();
	void SwitchToWalk();
    void SetupAnimation();
    std::string _animalsType;
    std::string _sex;
	Animation *_current;
	Animation *_rightFront;
	Animation *_rightFrontIdle;
	Animation *_right;
	Animation *_rightIdle;
	Animation *_rightBack;
	Animation *_righteat;
	Texture *_shadow;
	bool _mirror;
	std::vector<FPoint2D> _region;

	FPoint2D _currentTarget;
	FPoint2D _endTarget;
	float _timeCounter;
	float _actionTimeCounter;

    AnimalStates _state;
    AnimalStates _nextState;
	bool _boy;
	enum {
		age_young,
		age_adult
	} _age;
	typedef std::list<AnimalStates> CircleStates;
    CircleStates _adultCircleStates;
    CircleStates _productCircleStates;
    float _lifeTimeCounter;
    float _waitingTimeCounter;
    CircleProgress _waitingProgress;
    FPoint2D _foodPos;
    FPoint2D _waterPos;
    FPoint2D _turnPoint;
	FPoint2D _eggPos;
	FPoint2D _puhPos;
    bool _waitBasePosition;
	std::vector<FPoint2D> _runAway;
	int _runAwayPoint;
	float _runAwayTimeCounter;

	friend class AnimalsManager;
};

#endif//GAME1_ANIMAL_H