#ifndef MYENGINE_ANIMATIONEDITOR_H
#define MYENGINE_ANIMATIONEDITOR_H

#include "../Core/InputSystem.h"
#include "../Core/Render.h"
#include "../Core/Animation.h"
#include "../Core/Messager.h"

class AnimationEditor 
	: public InputSystem  
	, public Messager
{
public:
	AnimationEditor(TiXmlElement *xe);
	virtual ~AnimationEditor();
	virtual void Draw();
	virtual void Update(float dt);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(const FPoint2D &mousePos);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual void OnMessage(const std::string &message);
private:
	const float STEP_DOWN;
	void DrawTree();
	float DrawChild(Bone *bone, float x, float y);
	std::string ShowLoadDialog();
	bool LoadFile(const std::string &fileName);
	//StaticSprite _background;
	Animation *_animation;
	FPoint2D _lastMousePos;
	float _timeCounter;
	bool _saved;
	bool _play;
	bool _mirror;
};

#endif//MYENGINE_ANIMATIONEDITOR_H