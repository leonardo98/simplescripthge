// MultiTexture.h: interface for the MultiTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MULTITEXTURE_INCLUDED_)
#define _MULTITEXTURE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InputSystem.h"
#include "Variable.h"
#include "Parser.h"
#include "Counter.h"

struct State {
	hgeVector pos;
	Texture *texture;
	std::string name;
	bool visible;
	State(TiXmlElement *xe);
	void Draw();
	void SetAlpha(float alpha);
};

class MultiTexture 
	: public InputSystem  
{
public:
	MultiTexture(TiXmlElement *xe);
	virtual ~MultiTexture();
	virtual void Draw();
	virtual void Update(float deltaTime);
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMouseDown(hgeVector mousePos);
private:
	typedef std::vector<State> States;
	States _states;
	Variable::Ptr _stateVariable;
	Parser _parser;
	Counter _counter;
	std::string _stateName;
};

#endif // !defined(_MULTITEXTURE_INCLUDED_)
