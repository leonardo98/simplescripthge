#include "AnimationPlayer.h"
#include "Math.h"
#include "Core.h"

AnimationPlayer::AnimationPlayer(TiXmlElement *xe) 
: Messager(xe)
{
	_animation = Core::getMyAnimation(xe->Attribute("animation"));
	std::string value = xe->Attribute("state");
	if (value == "invisible") {
		_state = state_invisible;
	} else if (value == "loop") {
		_state = state_loop;
	} else if (value == "idle") {
		_state = state_idle;
	} else {
		LOG("AnimationPlayer - invalid state");
		assert(false);
	}
	_pos.x = Math::Read(xe, "x", 0.f);
	_pos.y = Math::Read(xe, "y", 0.f);
	_timeCounter = Math::Read(xe, "time", 0.f);
}

AnimationPlayer::~AnimationPlayer() {
}

void AnimationPlayer::Draw() {
	if (_state == state_invisible) {
		return;
	}
	_animation->SetPos(_pos, false);
	assert(0.f <= _timeCounter && _timeCounter <= 1.f);
	_animation->Draw(_timeCounter);
}

void AnimationPlayer::Update(float dt) {
	if (_state == state_invisible) {
		return;
	}
	if (_state == state_loop) {
		_timeCounter += dt / _animation->Time();
		while (_timeCounter >= 1.f) {
			_timeCounter -= 1.f;
		}
	} else if (_state == state_idle) {
		//_timeCounter = 0.f;
	} else if (_state == state_play) {
		_timeCounter += dt / _animation->Time();
		if (_timeCounter >= 1.f) {
			if (_pushedList.size() > 0) {
				_animation = _pushedList.front();
				_pushedList.pop_front();
				if (_pushedList.size() == 0) {
					_state = state_loop;
				}
			} else {
				_state = state_idle;
			}
			_timeCounter = 0.f;
		}
	} else if (_state == state_play_and_hide) {
		_timeCounter += dt / _animation->Time();
		if (_timeCounter >= 1.f) {
			_state = state_invisible;
			_timeCounter = 0.f;
		}
	} else if (_state == state_play_and_last) {
		_timeCounter += dt / _animation->Time();
		if (_timeCounter >= 1.f) {
			_timeCounter = 1.f;
		}
	}
}

void AnimationPlayer::OnMessage(const std::string &message) {
	States _oldState = _state;
	float start = 0.f;
	if (message == "play" || message == "play_and_idle") {
		_state = state_play;
	} else if (message == "idle") {
		_state = state_idle;
	} else if (message == "play_in_loop") {
		_state = state_loop;
	} else if (message == "play_and_hide") {
		_state = state_play_and_hide;
	} else if (message == "hide") {
		_state = state_invisible;
	} else if (message == "play_and_last") {
		_state = state_play_and_last;
	} else if (message.substr(0, 13) == "play_and_hide") {
		_state = state_play_and_hide;
		start = atof(message.substr(14).c_str()) / _animation->Time();
	} else if (message.substr(0, 4) == "play") {
		_state = state_play;
		start = atof(message.substr(5).c_str()) / _animation->Time();
		_pushedList.clear();
	} else if (message.substr(0, 4) == "push") {
		_pushedList.push_back(Core::getMyAnimation(message.substr(5)));
	} else if (message.substr(0, 9) == "animation") {
		_animation = Core::getMyAnimation(message.substr(10));
	}
	if (_oldState != _state) {
		_timeCounter = start;
	}
}
