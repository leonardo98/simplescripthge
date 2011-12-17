#ifndef MYENGINE_ANIMATION_VIEWER
#define MYENGINE_ANIMATION_VIEWER

#include "../Core/Object.h"
#include "../Core/Render.h"
#include "../Core/Animation.h"

class AnimationViewer : public Object  
{
public:
	AnimationViewer(TiXmlElement *xe);
	virtual ~AnimationViewer();
	virtual void Draw();
	virtual void Update(float dt);
private:
	void Reload();
	FPoint2D _pos;
	bool _mirror;
	Animation *_animation;
};

#endif//MYENGINE_ANIMATION_VIEWER