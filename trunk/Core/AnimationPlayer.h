#ifndef ANIMATION_PLAYER_H
#define ANIMATION_PLAYER_H

#include "Render.h"
#include "Messager.h"
#include "InputSystem.h"

#include "../2dga_api/Animation.h"

class AnimationPlayer 
	: public InputSystem
	, public Messager
{
public:
	enum States {
		state_invisible,
		state_loop,
		state_play_and_hide,
		state_play_and_last,
		state_idle,
		state_play
	};
	AnimationPlayer(TiXmlElement *xe);
	virtual ~AnimationPlayer();
	void Draw();
	void Update(float dt);
	void OnMessage(const std::string &message);
	virtual bool IsMouseOver(const FPoint2D &mousePos) {return false;}
private:
	FPoint2D _pos;
	My::Animation *_animation;
	float _timeCounter;
	States _state;
	std::list<My::Animation *> _pushedList;
	/*
	bool _visible;
	int _loop;
	bool _playAndHide;
	bool _playAndLast;*/
};

#endif//ANIMATION_PLAYER_H