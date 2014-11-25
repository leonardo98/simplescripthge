#include "Moneypod.h"
#include "../Core/Core.h"
#include "../Core/Variables.h"

Moneypod::Moneypod() {
	_back.Set(Core::getTexture("textures\\env_jungle\\money_pod_dn.png"));
	_front.Set(Core::getTexture("textures\\env_jungle\\money_pod_up.png"));
}

void Moneypod::Draw() {
	_back.Render();
	_front.Render();
	_moneyStrValue = Variables::Get("money");
	Render::PrintString(100, 650, "data\\fonts\\arialblack20.fnt", _moneyStrValue.c_str(), 0xFFFFFFFF); 
}

void Moneypod::Update(float dt) {
}

void Moneypod::AddCoins(int amount) {
}
