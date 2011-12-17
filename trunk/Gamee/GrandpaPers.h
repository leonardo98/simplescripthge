#ifndef MYENGINE_GRANDPAPERS_H
#define MYENGINE_GRANDPAPERS_H

#include "../Core/Animation.h"
#include "BaseElement.h"
#include "Action.h"

class GrandpaPers : public BaseElement
{
public:
	~GrandpaPers();
	GrandpaPers();
	virtual void Draw();
	void Update(float dt);
	void Goto(FPoint2D position);
	static bool SetProduct(const std::string &productType);
	static std::string DropProduct();
	static void NewAction(const std::string &action, ProductPlace *productPlace);
	static void NewAction(const std::string &action, TreePlace *plantPlace);
	static void NewAction(const std::string &action, EnvWell *wellPlace);
	static void NewAction(const std::string &action, BaseField *fieldPlace);
private:
	static Animation *_current;
	static Animation *_front[2];
	static Animation *_leftFront[2];
	static Animation *_leftFrontIdle[2];
	static Animation *_left[2];
	static Animation *_back[2];
	static Animation *_leftBack[2];
	static Animation *_seeding;
	static Animation *_scythe;
	Texture *_shadow;
	bool _mirror;

	FPoint2D _currentTarget;
	FPoint2D _endTarget;
	const float SPEED;
	static int _product;
	static std::string _productType;

	bool ReadAction();
	bool CheckAction();
	static Actions _actions;
	Action _currentAction;
	float _showAnimationCounter;
};

#endif//MYENGINE_GRANDPAPERS_H