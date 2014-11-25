#ifndef MYENGINE_IPANEL_H
#define MYENGINE_IPANEL_H

#include "Object.h"
#include "Interface.h"
#include "InputSystem.h"
#include "Core.h"
#include "Messager.h"

class IPanel 
	: public InputSystem
	, public Interface
	, public Messager
	, public LuaScript
{
public:
	IPanel(TiXmlElement *xe);
	virtual void Draw();
	virtual void Update(float deltaTime);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(const FPoint2D &mousePos);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual void OnMessage(const std::string &message);
	bool GetBoolValue(const std::string &variableName);
	void SetValue(const std::string &variableName, const bool &value);
	virtual ~IPanel(void);
private:
	FPoint2D _pos;
	int _width;
	int _height;
	bool _visible;
	bool _needDraw;
	typedef std::list<InputSystem *> Objects;
	Objects _objects;
};

#endif//MYENGINE_IPANEL_H