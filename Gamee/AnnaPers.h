#ifndef MYENGINE_ANNAPERS_H
#define MYENGINE_ANNAPERS_H

#include "../Core/Animation.h"
#include "Pers.h"
#include "Action.h"

class AnnaPers : public Pers
{
public:
	~AnnaPers();
	AnnaPers();
	virtual void Draw();
	void Update(float dt);
	void Goto(FPoint2D position);
	static bool SetProduct(const std::string &productType);
	static std::string GetProductType();
	static std::string DropProduct();
	static void NewAction(const std::string &action, ProductPlace *productPlace);
	static void NewAction(const std::string &action, PlantPlace *plantPlace);
private:
	static Animation *_current;
	static Animation *_front[2];
	static Animation *_leftFront[2];
	static Animation *_leftFrontIdle[2];
	static Animation *_left[2];
	static Animation *_back[2];
	static Animation *_leftBack[2];
	static Animation *_downup;
	static Animation *_seeding;
	Texture *_shadow;
	PlantPlace *_harvesting;

	static float _showAnimationCounter;
	FPoint2D _currentTarget;
	FPoint2D _endTarget;
	const float SPEED;
	static int _product;
	static std::string _productType;

	bool ReadAction();
	bool CheckAction();
	static std::list<Action> _actions;
	Action _currentAction;

};

#endif//MYENGINE_ANNAPERS_H