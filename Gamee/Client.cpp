#include "Client.h"
#include "../Core/Math.h"
#include "../Core/Variables.h"

Client::Client(float positionX) 
: SPEED(100.f)
, WAIT_PRODUCT_TIME(180.f)
, _stopEffect(0.f)
, MAX_COINS(8)
, STEP_UP(8)
, _busketUpCounter(0.f)
, _iActive(false)
, _stableTime(0.f)
, _basketWaveCounter(0.f)
, _product(NULL)
, _coinsEffect(0.f)
{
	_oldCoinsCounter = _coinsCounter = MAX_COINS;
	_positionX = 1100.f;
	SetPos(positionX);
	_state = state_go_first;
	_waitProductCounter = 0.f;

	std::string id("Clients1set\\Client101\\c101");
	int r = rand() % 10 + 1;
	if (2 <= r && r <= 9) {
		id[25] = id[20] = '0' + r;
	} else {
		id = "Clients1set\\Client110\\c110";
	}
	_body.Set(Core::getTexture(id + "_body.png"));
	_head.Set(Core::getTexture(id + "_normal.png"));
	_angry.Set(Core::getTexture(id + "_angry.png"));
	_smile.Set(Core::getTexture(id + "_smile.png"));
	_eyes.Set(Core::getTexture(id + "_eyes.png"));
	_hair.Set(Core::getTexture(id + "_hired.png"));

	_hasBasket = true;//(rand() % 2 == 1);

	if (r == 5 || r == 7) {
		_rightUpperArm.Set(Core::getTexture("Clients1set\\Client107\\cb_r_upperarm.png"));
		_rightForearm.Set(Core::getTexture("Clients1set\\Client107\\cb_r_forearm.png"));
		_leftHand.Set(Core::getTexture("Clients1set\\Client107\\cb_l_hand.png"));
		if (_hasBasket) {
			_rightHand.Set(Core::getTexture("Clients1set\\Client107\\cb_r_hand.png"));
		} else {
			_rightHand.Set(Core::getTexture("Clients1set\\Client107\\cb_r_hand_hold.png"));
		}
		//.Set(Core::getTexture("Clients1set\\Client107\\cb_r_upperarm_hold.png"));
		//.Set(Core::getTexture("Clients1set\\Client107\\cb_l_forearm_hold.png"));
		//.Set(Core::getTexture("Clients1set\\Client107\\cb_r_hand_drop.png"));
		//.Set(Core::getTexture("Clients1set\\Client107\\cb_l_hand_hold.png"));
	} else {
		_rightUpperArm.Set(Core::getTexture("Clients1set\\Client101\\cw_r_upperarm.png"));
		_rightForearm.Set(Core::getTexture("Clients1set\\Client101\\cw_r_forearm.png"));
		_leftHand.Set(Core::getTexture("Clients1set\\Client101\\cw_l_hand.png"));
		if (_hasBasket) {
			_rightHand.Set(Core::getTexture("Clients1set\\Client101\\cw_r_hand.png"));
		} else {
			_rightHand.Set(Core::getTexture("Clients1set\\Client101\\cw_r_hand_hold.png"));
		}
		//.Set(Core::getTexture("Clients1set\\Client101\\cw_r_upperarm_hold.png"));
		//.Set(Core::getTexture("Clients1set\\Client101\\cw_l_forearm_hold.png"));
		//.Set(Core::getTexture("Clients1set\\Client101\\cw_r_hand_drop.png"));
		//.Set(Core::getTexture("Clients1set\\Client101\\cw_l_hand_hold.png"));
	}

	if (_hasBasket)
	{
		// вынести в статик инициализацию
		_basketBack[0].Set(Core::getTexture("Clients1set\\Client101\\yellow_basket_dn.png"));
		_basketFront[0].Set(Core::getTexture("Clients1set\\Client101\\yellow_basket_up.png"));
		_basketWire[0].Set(Core::getTexture("Clients1set\\Client101\\yellow_basket_wire.png"));

		_basketBack[1].Set(Core::getTexture("Clients1set\\Client101\\cm_basket_yellow_dn.png"));
		_basketFront[1].Set(Core::getTexture("Clients1set\\Client101\\cm_basket_yellow_up.png"));
		_basketWire[1].Set(Core::getTexture("Clients1set\\Client101\\basket_wire.png"));

		_basketBack[2].Set(Core::getTexture("Clients1set\\Client101\\cmw_basket_green_dn.png"));
		_basketFront[2].Set(Core::getTexture("Clients1set\\Client101\\cmw_basket_green_up.png"));
		_basketWire[2].Set(Core::getTexture("Clients1set\\Client101\\basket_wire.png"));

		_basketBack[3].Set(Core::getTexture("Clients1set\\Client101\\cmw_basket_grey_dn.png"));
		_basketFront[3].Set(Core::getTexture("Clients1set\\Client101\\cmw_basket_grey_up.png"));
		_basketWire[3].Set(Core::getTexture("Clients1set\\Client101\\basket_wire.png"));

		_basketBack[4].Set(Core::getTexture("Clients1set\\Client101\\cw_basket_blue_dn.png"));
		_basketFront[4].Set(Core::getTexture("Clients1set\\Client101\\cw_basket_blue_up.png"));
		_basketWire[4].Set(Core::getTexture("Clients1set\\Client101\\basket_wire.png"));

		_basketBack[5].Set(Core::getTexture("Clients1set\\Client101\\cw_basket_brown_dn.png"));
		_basketFront[5].Set(Core::getTexture("Clients1set\\Client101\\cw_basket_brown_up.png"));
		_basketWire[5].Set(Core::getTexture("Clients1set\\Client101\\basket_wire.png"));
		_basketType = rand() % 6;
	} 

	_coinGold = Core::getTexture("Clients1set\\Client101\\coin_g.png");
	_coinSilver = Core::getTexture("Clients1set\\Client101\\coin_s.png");

	_hatBack = NULL;
	int h = rand() % 6;
	//_hat = Core::getTexture("Clients1set\\Client101\\builder_decorator_head_cl_hat.png");
	if (h == 0) {
		_hat = Core::getTexture("Clients1set\\Client101\\cm_head_cl_grayhat.png");
	} else if (h == 1) {
		_hat = Core::getTexture("Clients1set\\Client101\\cmw_head_cl_whitehat.png");
	} else if (h == 2) {
		_hat = Core::getTexture("Clients1set\\Client101\\cw_head_cl_greenhat.png");
	} else if (h == 3) {
		_hat = Core::getTexture("Clients1set\\Client101\\cw_head_cl_yellowhat.png");
	} else if (h == 4){
		_hat = Core::getTexture("Clients1set\\Client101\\seller_cmw_head_cl_hat.png");
		_hatBack = Core::getTexture("Clients1set\\Client101\\seller_cmw_head_cl_hat_dn.png");
	} else {
		_hat = NULL;
	}

	if ((r == 3
		|| r == 4//?
		|| r == 5
		|| r == 9
		) && rand() % 2 == 0) {
		_eyesGlass = (Core::getTexture("Clients1set\\Client101\\cmw_head_cl_eyeglasses.png"));
		if (r == 2) {
			_eyesGlassX = 1.f;//2 перс
		} else if (r == 3) {
			_eyesGlassX = 4.f;//3 перс
		} else if (r == 4) {
			_eyesGlassX = 2.f;//4 перс
		} else {
			_eyesGlassX = 0.f;
		}
	} else {
		_eyesGlass = NULL;
	}

	_headAngle.Clear();
	_headAngle.addKey(0.f);
	_headAngle.addKey(0.02f);
	_headAngle.addKey(-0.025f);
	_headAngle.addKey(0.f);
	_headAngle.CalculateGradient(true);
	_headCounter = 1.f;
	MUL_HEAD_COUNTER = 0.4f;
	_eyesCounter = 0.f;

	r = rand() % 6;
	if (r == 0) {
		_productWant = "onion";
	} else if (r == 1) {
		_productWant = "beet";
	} else if (r == 2) {
		_productWant = "pumpkin";
	} else if (r == 3) {
		_productWant = "avocado";
	} else if (r == 4) {
		_productWant = "banana";
	} else {
		_productWant = "peach";
	}
	_progress.SetIcon("gui_" + _productWant);
	_progress.Move(37, 44);
}

void Client::DrawLeftHand() {
	//_rightHandDrop.Render();
	if (_xOffset.size() == 0) {
		_silverAmount = Math::random(2, MAX_COINS - 2);
		_xOffset.resize(MAX_COINS);
		for (int i = 0; i < MAX_COINS; ++i) {
			_xOffset[i] = Math::random(-2.5f, 2.5f);
		}
	}
	if (_waitProductCounter > 0.f) {
		_coinsCounter = 1 + (MAX_COINS - 1) * min(1.f, 1.3 * _waitProductCounter / WAIT_PRODUCT_TIME);
		if (_oldCoinsCounter != _coinsCounter) {
			_coinsEffect = 1.f;
			_oldCoinsCounter = _coinsCounter;
		}
	} else if (_state == state_go_out) {
		_coinsCounter = 0;
	}
	int n = MAX_COINS;
	int i = MAX_COINS - _coinsCounter;
	if (i < MAX_COINS) {
		_xOffset[i] = 0.f;
	}
	float y = 0;
	if (_coinsEffect > 0.f) {
		const float UPEFFECT = 13.f;
		float effectStep = STEP_UP + 2.f * sinf(M_PI * _coinsEffect);
		if (_coinsEffect > 0.5f) {
			i--; 
			if (i < 0) {
				int f = 0;
			}
			y -= UPEFFECT * sin (M_PI * _coinsEffect);
		} else {
			y -= (UPEFFECT + STEP_UP) * sin (M_PI * _coinsEffect);
		}
		bool first = _coinsEffect > 0.5f;
		for (; i < n; ++i) {
			if (first) {
				Render::SetAlpha(0xFF * max(0.f, (_coinsEffect - 0.5f)) / 0.5f);
			}
			if (i < _silverAmount) {
				_coinGold->Render(_xOffset[i], y);
			} else {
				_coinSilver->Render(_xOffset[i], y);
			}
			y -= effectStep;
			if (first) {
				Render::SetAlpha(0xFF);
				first = false;
			}
		}
		Render::PushMatrix();
		Render::MatrixMove(0.f, -UPEFFECT * sin (M_PI * _coinsEffect));
		_leftHand.Render();
		Render::PopMatrix();
	} else {
		for (; i < n; ++i) {
			if (i < _silverAmount) {
				_coinGold->Render(_xOffset[i], y);
			} else {
				_coinSilver->Render(_xOffset[i], y);
			}
			y -= STEP_UP;
		}
		_leftHand.Render();
	}
}

void Client::DrawRightHand() {
	Render::PushMatrix();
	Render::MatrixMove(85, 120);// точка крепления плеча к телу
	float angle = 0.f;
	if (_busketUpCounter > 0.f) {
		if (_iActive) {
			angle = -_busketUpCounter;
		} else {
			angle = -(1.f - _busketUpCounter);
		}
	} else if (!_iActive) {
		angle = -1.f;
	}
	Render::MatrixRotate(angle);
	Render::MatrixMove(-85, -120);
	_rightUpperArm.Render();
	if (_state != state_go_first && _state != state_go_out) {
		_rightForearm.Render();
	}
	if (_basketWaveCounter > 0.f) {
		angle -= 0.15f * _basketWaveCounter * sin(M_PI * 2 * _basketWaveCounter);
	}
	Render::MatrixMove(43, 103);//точка качания правой руки с корзиной
	Render::MatrixRotate(-angle);
	Render::MatrixMove(-43, -103);
	if (_hasBasket) {
		_basketBack[_basketType].Render();
		_rightHand.Render();
		_basketWire[_basketType].Render();
	} else {
		_rightHand.Render();
	}
	if (_product) {
		_product->Render(11, 90);// иконка чуток не попадала - подвинул
	}
	if (_hasBasket) {
		_basketFront[_basketType].Render();
	}
	Render::PopMatrix();
}

void Client::DrawClient(float x, float y) {
	Render::PushMatrix();
	Render::MatrixMove(x, y);
	DrawRightHand();
	_body.Render(); 
	Render::PushMatrix();
	Render::MatrixMove(97.f, 103.f);// точка качания головы
	Render::MatrixRotate(_headAngle.getGlobalFrame(_headCounter));
	Render::MatrixMove(-97.f, -103.f);
	if (_hatBack) {
		_hatBack->Render(0.f, 0.f);
	}
	_head.Render();
	if (_hat) {
		_hat->Render(0.f, 0.f);
	} else {
		_hair.Render();
	}
	if (_waitProductCounter > 0.f) {
		float f = _waitProductCounter / WAIT_PRODUCT_TIME;
		if (f > 0.5f) {
			_smile.Render();
		} else if (f < 0.2f) {
			_angry.Render();
		}
	} else if (_state == state_go_first || _product != NULL) {
		_smile.Render();
	} else if (_product == NULL) {
		_angry.Render();
	}
	if (_eyesCounter <= 0.f) {
		_eyes.Render();
	}
	if (_eyesGlass) {
		_eyesGlass->Render(_eyesGlassX, 0.f);
	}
	Render::PopMatrix();
	DrawLeftHand();
	if (_waitProductCounter > 0.f) {
		_progress.Draw(0.f);
	}
	Render::PopMatrix();
}

void Client::Draw() {
	float f;
	if (_state == state_go_first || _state == state_go_out || _state == state_go) {
		 f = -fabs(5.5f * sinf(0 + 2 * M_PI * _walkCounter));
		_pos.x = _positionX + SPEED * _walkCounter - 100.f;
	} else {
		f = 0.f;
		if (_stopEffect > 0.f) {
			f = (fabs(2.f * sinf(M_PI_2 + M_PI * _stopEffect)) - 2.f);
		}
		_pos.x = _positionX - 100.f;
	}	
	_pos.y = f + 635.f;
	DrawClient(_pos.x, _pos.y);
}

void Client::SetActive(bool active) {
	if ((_iActive != active && _product == NULL) || (_product != NULL && !_iActive && _iActive != active)) {
		_iActive = active;
		_busketUpCounter = 1.f - _busketUpCounter;
		if (!_iActive) {
			_stableTime = 0.3f;
		} else {
			_stableTime = 0.f;
		}
	}
}

void Client::Update(float dt) {
	if (_coinsEffect > 0.f) {
		_coinsEffect -= dt * 3.f;
	}
	if (_basketWaveCounter > 0.f) {
		_basketWaveCounter -= dt * 2;
	}
	if (_busketUpCounter > 0.f) {
		if (_stableTime > 0.f) {
			_stableTime -= dt;
		} else {
			_busketUpCounter -= dt * 5;
			if (_busketUpCounter < 0.f) {
				_busketUpCounter = 0.f;
				_basketWaveCounter = 1.f;
			}
		}
	}
	if (_stopEffect > 0.f) {
		_stopEffect -= dt * 3;
	}
	if (_eyesCounter > -0.1f) {
		_eyesCounter -= dt;
		if (_eyesCounter <= -0.1f) {
			_eyesCounter = Math::random(7.5f, 15.f);
		}
	}
	_headCounter -= dt * MUL_HEAD_COUNTER;
	if (_headCounter < 0.f) {
		_headCounter = 1.f;
		_headAngle.Clear();
		_headAngle.addKey(0.f);
		if (rand() % 4 != 0) {
			_headAngle.addKey(-0.02f);
			_headAngle.addKey(0.02f);
			MUL_HEAD_COUNTER = Math::random(0.2f, 0.4f);
		} else {
			float f = Math::random(0.02f, 0.1f);
			_headAngle.addKey(-f);
			_headAngle.addKey(-f);
			MUL_HEAD_COUNTER = 0.5f;
		}
		_headAngle.addKey(0.f);
		_headAngle.CalculateGradient(true);
	}
	if (_waitProductCounter > 0.f) {
		_waitProductCounter -= dt;
		if (_waitProductCounter <= 0.f) {
			//_state = state_time_out;
			SetPos(-100.f);
			_state = state_go_out;
		}
	}
	if (_walkCounter > 0.f) {
		_walkCounter -= dt;
		if (_walkCounter <= 0.f) {
			if (_state == state_go_first) {
				if (_product == NULL) {
					_waitProductCounter = WAIT_PRODUCT_TIME;
					_stopEffect = 1.f;
					_state = state_stay;
				}
			} else if (_state == state_go) {
				if (_product == NULL) {
					_stopEffect = 1.f;
					_state = state_stay;
				} else {
					assert(false);
				}
			} else if (_state == state_go_out) {
				_state = state_finish;
			} else {
				assert(false);
			}
		}
	}
}

void Client::SetProduct() {
	std::string id = "gui_" + _productWant;
	_product = Core::getTexture(id);
	SetPos(-100.f);
	_state = state_go_out;
	_waitProductCounter = 0.f;
	_coinsEffect = 0.f;

	if (_coinsCounter > 6) {
		Variables::SetAsInt("money", Variables::GetAsInt("money") + 30);
	} else if (_coinsCounter > 3) {
		Variables::SetAsInt("money", Variables::GetAsInt("money") + 20);
	} else {
		Variables::SetAsInt("money", Variables::GetAsInt("money") + 10);
	}
}

ClientStates Client::GetState() {
	return _state;
}

bool Client::PixelCheck(const FPoint2D &mousePos) {
	bool b = false;
	FPoint2D startPos(109, 82 - _coinsCounter * STEP_UP);
	startPos += _pos;
	float w = 45;
	float h = 32 + _coinsCounter * STEP_UP;
	if (hgeRect(startPos.x, startPos.y, startPos.x + w, startPos.y + h).TestPoint(mousePos.x, mousePos.y)) {
		return true;
	}
	return 
		(_body.PixelCheck(mousePos)
		|| _head.PixelCheck(mousePos)
		|| (_hasBasket && _basketBack[_basketType].PixelCheck(mousePos))
		|| (_hasBasket && _basketFront[_basketType].PixelCheck(mousePos))
		|| _leftHand.PixelCheck(mousePos)
		|| _rightHand.PixelCheck(mousePos)
		|| _rightUpperArm.PixelCheck(mousePos)
		|| _progress.PixelCheck(mousePos));
}

void Client::SetPos(float positionX) {
	if (_state == state_stay || _state == state_go) { // hack
		_state = state_go;
	}
	float f = _positionX;
	if (_walkCounter > 0.f) {
		f += _walkCounter * SPEED;
	}
	assert(positionX < f);
	_walkCounter = (f - positionX) / SPEED;
	_positionX = positionX;
}

float Client::GetPos() {
	return _positionX;
}

bool Client::IsWaitProduct() {
	return _waitProductCounter > 0.f;
}