#ifndef ANIMEDITOR_H
#define ANIMEDITOR_H

#include "../Core/Animation.h"

class AnimEditor {
public:
	bool exitPressed;
	AnimEditor();
	void Draw();
	void Update(float dt);
	void CreateTree(void *parent, CallBones myCall);
	bool SetCurrent(const char *name);
private:
	Animation *_currentAnimation;
	float _timeCounter;
	FPoint2D _position;
};

#endif//ANIMEDITOR_H