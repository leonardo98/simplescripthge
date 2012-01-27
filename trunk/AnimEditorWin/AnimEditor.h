#ifndef ANIMEDITOR_H
#define ANIMEDITOR_H

#include "../Core/Animation.h"
#include "../Core/SplinePath.h"

class AnimEditor {
public:
	AnimEditor();
	~AnimEditor();
	void Draw();
	void Update(float dt);
	void CreateTree(void *parent, CallBones myCall);
	bool SetCurrent(const char *name);
	Animation * Anim();
	struct key {
		float x;
		float y;
		float angle;
		float sx;
		float sy;
	};
	std::vector<key> poses;
private:
	Animation *_currentAnimation;
	float _timeCounter;
	FPoint2D _position;
	HTEXTURE h_base;
	hgeSprite *red_dot;
	hgeFont *font;
};

#endif//ANIMEDITOR_H