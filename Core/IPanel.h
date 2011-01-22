#pragma once
#include "Object.h"
#include "Interface.h"
#include "InputSystem.h"
#include "Core.h"

class IPanel 
	: public InputSystem
	, public Interface
	, public Messager
	, public LuaScript
{
public:
	struct KeyState{
		std::string msg;
		std::string rsvr;
		char *code;
	};
	std::map<DWORD, KeyState> _keyStates;
	IPanel(TiXmlElement *xe);
	virtual void Draw();
	virtual void Update(float deltaTime);
	// нажали клавишу на клавиатуре(еще не реализован)
	virtual void OnKeyDown(int key);
	virtual void OnMouseDown(hgeVector mousePos);
	virtual void OnMouseUp();
	virtual void OnMouseMove(hgeVector mousePos);
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMessage(const std::string &message);
	bool GetBoolValue(const std::string &variableName);
	void SetValue(const std::string &variableName, const bool &value);
	virtual ~IPanel(void);
private:
	hgeVector _pos;
	int _width;
	int _height;
	bool _visible;
	bool _needDraw;
	std::vector<InputSystem *> _objects;
};
