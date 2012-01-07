#include "VipClient.h"
#include "../Core/Math.h"
#include "../Core/Variables.h"

void VipClient::Init(int index) {
	_basketType = rand() % 3;
	std::string id("textures\\vip101\\vip101");
	int r = index;
	id[14] = id[21] = '0' + r;
	_body.Set(Core::getTexture(id + "_body.png"));
	_head.Set(Core::getTexture(id + "_normal.png"));
	_angry.Set(Core::getTexture(id + "_angry.png"));
	_smile.Set(Core::getTexture(id + "_smile.png"));
	_eyes.Set(Core::getTexture(id + "_eyes.png"));
	_leftUpperArm.Set(Core::getTexture(id + "_l_upperarm.png"));

	_rightUpperArm.Set(Core::getTexture("textures\\vip101\\vip1set_r_upperarm.png"));
	_rightForearm.Set(Core::getTexture("textures\\vip101\\vip1set_r_forearm.png"));
	_leftHand.Set(Core::getTexture("textures\\vip101\\vip1set_l_hand.png"));
	_leftForearm.Set(Core::getTexture("textures\\vip101\\vip1set_l_forearm.png"));
	
	_hasBasket = true;//(rand() % 2 == 1);
	if (_hasBasket) {
		_rightHand.Set(Core::getTexture("textures\\vip101\\vip1set_r_hand.png"));
	} else {
		_rightHand.Set(Core::getTexture("textures\\vip101\\vip1set_r_hand_hold.png"));
	}
	_lastBag = _oldLastBag = &_bagLarge;

	_state = vip_state_wait_product;
	_waitProductCounter = WAIT_PRODUCT_TIME;
}

VipClient::~VipClient() {
}

VipClient::VipClient() 
: WAIT_PRODUCT_TIME(10.f)
, _busketUpCounter(0.f)
, _iActive(false)
, _stableTime(0.f)
, _basketWaveCounter(0.f)
, _product(NULL)
, _bagEffect(0.f)
, _waitProductCounter(0.f)
{
	_state = vip_state_invisible;

	Init(rand() % 3 + 1);

	_basketBack[0].Set(Core::getTexture("textures\\vip101\\vip_basket_yellow_dn.png"));
	_basketFront[0].Set(Core::getTexture("textures\\vip101\\vip_basket_yellow_up.png"));

	_basketBack[1].Set(Core::getTexture("textures\\vip101\\vip_basket_yellow_dn.png"));
	_basketFront[1].Set(Core::getTexture("textures\\vip101\\vip_basket_yellow_up.png"));

	_basketBack[2].Set(Core::getTexture("textures\\vip101\\vip_basket_green_dn.png"));
	_basketFront[2].Set(Core::getTexture("textures\\vip101\\vip_basket_green_up.png"));

	_bagSmall.Set(Core::getTexture("textures\\vip101\\vip_bag_small.png"));
	_bagMiddle.Set(Core::getTexture("textures\\vip101\\vip_bag_middle.png"));
	_bagLarge.Set(Core::getTexture("textures\\vip101\\vip_bag_large.png"));

	_headAngle.Clear();
	_headAngle.addKey(0.f);
	_headAngle.addKey(0.02f);
	_headAngle.addKey(-0.025f);
	_headAngle.addKey(0.f);
	_headAngle.CalculateGradient(true);
	_headCounter = 1.f;
	MUL_HEAD_COUNTER = 0.4f;
	_eyesCounter = 0.f;

	int r = rand() % 6;
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
	_progress.Move(0, 44);

	_pos = FPoint2D(900, 200);
}

void VipClient::DrawLeftHand() {
	if (_waitProductCounter > 0.f) {
		StaticSprite *bag;
		if (_waitProductCounter / WAIT_PRODUCT_TIME > 0.5f) {
			bag = &_bagLarge;
		} else if (_waitProductCounter / WAIT_PRODUCT_TIME > 0.2f) {
			bag = &_bagMiddle;
		} else {
			bag = &_bagSmall;
		}
		if (_lastBag != bag) {
			_bagEffect = 1.f;
			_oldLastBag = _lastBag;
			_lastBag = bag;
		}
	}
	if (_bagEffect > 0.f) {
		Render::PushMatrix();
		Render::MatrixMove(100, 100);// ����� ��������� ����� ����� ����
		Render::MatrixRotate(0.05f * sin(M_PI * _bagEffect));
		Render::MatrixMove(-100, -100);
		_leftUpperArm.Render();
		Render::MatrixMove(110, 140);// ����� ��������� ����� ����� ����
		Render::MatrixRotate(0.4f * sin(M_PI * _bagEffect));
		Render::MatrixMove(-110, -140);
		_leftForearm.Render();
		Render::PopMatrix();
		const float UPEFFECT = 13.f;
		int y = - UPEFFECT * sin (M_PI * _bagEffect);
		Render::PushMatrix();
		Render::MatrixMove(0.f, 2 * y);
		if (_bagEffect > 0.5f) {
			float f = (_bagEffect - 0.5f) / 0.5f;
			if (_oldLastBag) {
				Render::SetAlpha(0xFF * f);
				_oldLastBag->Render();
				Render::SetAlpha(0xFF);
			}
			if (_lastBag) {
				Render::SetAlpha(0xFF * (1 - f));
				_lastBag->Render();
				Render::SetAlpha(0xFF);
			}
		} else {
			_lastBag->Render();
		}
		Render::MatrixMove(0.f, - y);
		_leftHand.Render();
		Render::PopMatrix();
	} else {
		_leftUpperArm.Render();
		_leftForearm.Render();
		_lastBag->Render();
		_leftHand.Render();
	}
}

void VipClient::DrawRightHand() {
	Render::PushMatrix();
	Render::MatrixMove(56, 102);// ����� ��������� ����� � ����
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
	angle += 1;
	Render::MatrixRotate(angle);
	Render::MatrixMove(-56, -102);
	_rightUpperArm.Render();
	_rightForearm.Render();
	if (_basketWaveCounter > 0.f) {
		angle -= 0.15f * _basketWaveCounter * sin(M_PI * 2 * _basketWaveCounter);
	}
	Render::MatrixMove(42, 127);//����� ������� ������ ���� � ��������
	Render::MatrixRotate(-angle);
	Render::MatrixMove(-42, -127);
	if (_hasBasket) {
		_basketBack[_basketType].Render();
		_rightHand.Render();
	} else {
		_rightHand.Render();
	}	
	if (_product) {
		_product->Render(11, 90);// ������ ����� �� �������� - ��������
	}
	if (_hasBasket) {
		_basketFront[_basketType].Render();
	}
	Render::PopMatrix();
}

void VipClient::DrawClient(float x, float y) {
	Render::PushMatrix();
	Render::MatrixMove(x, y);
	DrawRightHand();
	_body.Render(); 
	Render::PushMatrix();
	Render::MatrixMove(64.f, 84.f);// ����� ������� ������
	Render::MatrixRotate(_headAngle.getGlobalFrame(_headCounter));
	Render::MatrixMove(-64.f, -84.f);
	_head.Render();
	if (_waitProductCounter > 0.f) {
		float f = _waitProductCounter / WAIT_PRODUCT_TIME;
		if (f > 0.5f) {
			_smile.Render();
		} else if (f < 0.2f) {
			_angry.Render();
		}
	} else if (_state == vip_state_wait_dropeffect || (_state == vip_state_hide && _product != NULL)) {
		_smile.Render();
	} else if (_product == NULL) {
		_angry.Render();
	}
	if (_eyesCounter <= 0.f) {
		_eyes.Render();
	}
	Render::PopMatrix();
	DrawLeftHand();
	if (_waitProductCounter > 0.f) {
		_progress.Draw(0.f);
	}
	Render::PopMatrix();
}

void VipClient::Draw() {
	float f;
	//if (_walkCounter > 0.f) {//_state == state_go_first || _state == state_go_out || _state == state_go) {
	//	 f = -fabs(5.5f * sinf(0 + 2 * M_PI * _walkCounter));
	//	_pos.x = _positionX + SPEED * _walkCounter - 100.f;
	//} else {
	//	f = 0.f;
	//	if (_stopEffect > 0.f) {
	//		f = (fabs(2.f * sinf(M_PI_2 + M_PI * _stopEffect)) - 2.f);
	//	}
	//	_pos.x = _positionX - 100.f;
	//}	
	//_pos.y = f + 635.f;
	DrawClient(_pos.x, _pos.y);
}

void VipClient::SetActive(bool active) {
	if (!active && (_state == vip_state_hide || _state == vip_state_wait_dropeffect)) {
		return;
	}
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

void VipClient::Update(float dt) {
	if (_bagEffect > 0.f) {
		_bagEffect -= dt * 3.f;
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
			_state = vip_state_hide;
		}
	}
}

void VipClient::SetProduct(float time) {
	_state = vip_state_wait_dropeffect;
	_waitProductCounter = 0.f;
	_bagEffect = 0.f;
	if (!_iActive) {
		_iActive = true;
		_busketUpCounter = 1.f - _busketUpCounter;
	}

	if (_lastBag == &_bagLarge) {
		Variables::SetAsInt("money", Variables::GetAsInt("money") + 30 * 5);
	} else if (_lastBag == &_bagMiddle) {
		Variables::SetAsInt("money", Variables::GetAsInt("money") + 20 * 5);
	} else if (_lastBag == &_bagSmall) {
		Variables::SetAsInt("money", Variables::GetAsInt("money") + 10 * 5);
	}
}

void VipClient::Hide() {
	//return _state;
}

bool VipClient::IsUnderMouse(const FPoint2D &mousePos) {
	if (hgeRect(_pos.x + 20, _pos.y + 50, _pos.x + 140, _pos.y + 200).TestPoint(mousePos.x, mousePos.y)) {
		return true;
	}
	return 
		(_progress.PixelCheck(mousePos)
		|| _head.PixelCheck(mousePos)
		|| _body.PixelCheck(mousePos)
		|| (_hasBasket && _basketBack[_basketType].PixelCheck(mousePos))
		|| (_hasBasket && _basketFront[_basketType].PixelCheck(mousePos))
		|| _leftHand.PixelCheck(mousePos)
		|| _rightHand.PixelCheck(mousePos)
		|| _rightUpperArm.PixelCheck(mousePos)
		);
}

void VipClient::OnMouseMove(const FPoint2D &mousePos) {
	bool b = IsUnderMouse(mousePos);
	if (!_iActive && b) {
		_iActive = true;
		_busketUpCounter = 1.f - _busketUpCounter;
		_stableTime = 0.f;
	} else if (_iActive && !b) {
		_iActive = false;
		_busketUpCounter = 1.f - _busketUpCounter;
		_stableTime = 0.3f;
	}
}

void VipClient::OnMouseDown(const FPoint2D &mousePos) {
	Init(rand() % 3 + 1);
}
