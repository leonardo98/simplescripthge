#ifndef GAME1_MONEYPOD
#define GAME1_MONEYPOD

#include "../Core/types.h"
#include "../Core/Render.h"

class Moneypod {
public:
	Moneypod();
	void Draw();
	void Update(float dt);
	void AddCoins(int amount);
private:
	//hgeFont *_font;
	StaticSprite _back;
	StaticSprite _front;
	StaticSprite _silverCoin;
	StaticSprite _goldenCoin;
	std::string _moneyStrValue;
};

#endif//GAME1_MONEYPOD