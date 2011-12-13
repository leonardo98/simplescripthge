#ifndef BOBPERS_H
#define BOBPERS_H

#include "../Core/Animation.h"
#include "BaseElement.h"
#include "Action.h"

class BobPers : public BaseElement
{
public:
	~BobPers();
	BobPers();
	virtual void Draw();
	void Update(float dt);
	void Goto(FPoint2D position);
	static bool SetProduct(const std::string &productType);
	static std::string DropProduct();
	static void NewAction(const std::string &action, ProductPlace *productPlace);
	static void NewAction(const std::string &action, PlantPlace *plantPlace);
	static void NewAction(const std::string &action, Foodpan *foodPlace);
	static void NewAction(const std::string &action, Waterpan *waterPlace);
private:
	static Animation *_current;
	static Animation *_front[2];
	static Animation *_leftFront[2];
	static Animation *_leftFrontIdle[2];
	static Animation *_left[2];
	static Animation *_back[2];
	static Animation *_leftBack[2];
	static Animation *_hoe;
	Texture *_shadow;
	bool _mirror;
	float _showAnimationCounter;

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

#endif BOBPERS_H