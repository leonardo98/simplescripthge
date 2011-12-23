#ifndef GAME1_CLIENT
#define GAME1_CLIENT

#include "../Core/types.h"
#include "../Core/Core.h"
#include "../Core/Render.h"
#include "../Core/SplinePath.h"
#include "CircleProgress.h"

enum ClientStates {
	state_go_first,
	state_go,
	state_stay,
	state_time_out,
	state_go_out,
	state_finish
};

class Client {
public:
	Client(float positionX);
	void Draw();
	void Update(float dt);
	ClientStates GetState();
	bool PixelCheck(const FPoint2D &mousePos);
	void SetActive(bool active);
	void SetProduct();
	void SetPos(float positionX);
	float GetPos();
	std::string _productWant;
	bool IsWaitProduct();

	bool Seller();

	void CreateSeller(const std::string &buyingType, int price);

private:
	float _positionX;
	void DrawClient(float x, float y);
	void DrawLeftHand();
	void DrawRightHand();
	StaticSprite _body;
	StaticSprite _head;
	StaticSprite _angry;
	StaticSprite _smile;
	StaticSprite _eyes;
	StaticSprite _hair;
	Texture *_hat;
	Texture *_hatBack;
	Texture *_product;
	Texture * _eyesGlass;
	float _eyesGlassX;

	StaticSprite _rightUpperArm;
	StaticSprite _rightForearm;
	StaticSprite _leftHand;
	StaticSprite _rightHand;
	StaticSprite _rightHandDrop;

	StaticSprite _basketBack[6];
	StaticSprite _basketFront[6];
	StaticSprite _basketWire[6];
	int _basketType;

	Texture *_coinGold;
	Texture *_coinSilver;
	std::vector<float> _xOffset;
	int _silverAmount;

	CircleProgress _progress;

	ClientStates _state;
	float _walkCounter;
	float _waitProductCounter;
	float _time;
	float _stopEffect;
	const float SPEED;
	const float WAIT_PRODUCT_TIME;
	float _eyesCounter;
	SplinePath _headAngle;
	float _headCounter;
	float MUL_HEAD_COUNTER;
	float _busketUpCounter;
	bool _iActive;
	float _stableTime;
	float _basketWaveCounter;
	float _coinsEffect;
	bool _hasBasket;

	const int MAX_COINS;
	const int STEP_UP;
	int _coinsCounter;
	int _oldCoinsCounter;
	FPoint2D _pos;

	// for rabbit seller
	int _price;
	std::string _buyingType;
	Animation *_buying;

	std::string _priceStr;
};

#endif//GAME1_CLIENT