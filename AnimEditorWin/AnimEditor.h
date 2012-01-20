#ifndef ANIMEDITOR_H
#define ANIMEDITOR_H

#include "../Core/Animation.h"

class AnimEditor {
public:
	bool exitPressed;
	AnimEditor();
	void Draw();
	void Update(float dt);
	bool SetCurrent(const char *name);
private:
	Animation *_currentAnimation;
	float _timeCounter;
};

#endif//ANIMEDITOR_H