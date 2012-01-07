#ifndef GAME1_VIPCLIENT
#define GAME1_VIPCLIENT

#include "../Core/types.h"
#include "../Core/Core.h"
#include "../Core/Render.h"
#include "../Core/SplinePath.h"
#include "CircleProgress.h"

enum VipClientStates {
	vip_state_invisible,
	vip_state_show,
	vip_state_hide,
	vip_state_wait_product,
	vip_state_wait_dropeffect
};

class VipClient {
public:
	~VipClient();
	VipClient();
	void Draw();
	void Update(float dt);
	bool IsUnderMouse(const FPoint2D &mousePos);
	void OnMouseMove(const FPoint2D &mousePos);
	void OnMouseDown(const FPoint2D &mousePos);
	void SetProduct(float time);
	std::string _productWant;

private:
	void Init(int index);
	void Hide();
	void SetActive(bool active);
	void DrawClient(float x, float y);
	void DrawLeftHand();
	void DrawRightHand();

	StaticSprite _body;
	StaticSprite _head;
	StaticSprite _angry;
	StaticSprite _smile;
	StaticSprite _eyes;
	Texture *_product;

	StaticSprite _rightUpperArm;
	StaticSprite _rightForearm;
	StaticSprite _rightHand;
	StaticSprite _rightHandDrop;
	StaticSprite _leftUpperArm;
	StaticSprite _leftForearm;
	StaticSprite _leftHand;

	StaticSprite _basketBack[3];
	StaticSprite _basketFront[3];

	StaticSprite _bagSmall;
	StaticSprite _bagMiddle;
	StaticSprite _bagLarge;
	StaticSprite *_oldLastBag;
	StaticSprite *_lastBag;

	int _basketType;

	CircleProgress _progress;

	VipClientStates _state;
	float _waitProductCounter;
	float _time;
	const float WAIT_PRODUCT_TIME;
	float _eyesCounter;
	SplinePath _headAngle;
	float _headCounter;
	float MUL_HEAD_COUNTER;
	float _busketUpCounter;
	bool _iActive;
	float _stableTime;
	float _basketWaveCounter;
	bool _hasBasket;
	float _bagEffect;
	float _effect;
	float _invisibleTimeCounter;
	SplinePath _posx;
	SplinePath _posAngle;
	float _pauseTime;

	FPoint2D _pos;
};

#endif//GAME1_VIPCLIENT