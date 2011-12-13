#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "../Core/Render.h"
#include "../Core/SplinePath.h"
#include "CircleProgress.h"

class Client {
public:
	enum State {
		walk_in,
		wait_product,
		wait_over,
		walk_out,
		finish
	};
	Client(float positionX);
	void Draw();
	void Update(float dt);
	const float _positionX;
	State GetState();
	void SetState(State state);
	bool PixelCheck(const FPoint2D &mousePos);
	void SetActive(bool active);
	void SetProduct();
	std::string _productWant;
private:
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

	State _state;
	float _timeCounter;
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
};

class ClientManager {
public:
	~ClientManager();
	ClientManager();
	void Draw();
	void Update(float dt);
	void OnMouseMove(const FPoint2D &mousePos);
	void OnMouseDown(const FPoint2D &mousePos);
private:
	const int MAX;
	const int FIRST;
	const int STEP;
	typedef std::list<Client *> Clients;
	Client *_activeClient;
	std::vector<bool> _freePosition;
	Clients _clients;	
	float _newClientTimeCounter;
};

#endif//CLIENTMANAGER_H