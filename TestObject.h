// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TESTOBJECT_INCLUDED_)
#define _TESTOBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Variables.h"
#include "LuaScript.h"

class TestObject  
	: public Object	
{
public:
	TestObject(TiXmlElement* xe);
	~TestObject();
	virtual void Draw();
	virtual void Update(float);
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMouseDown(hgeVector mousePos);
private:
	hgeVector _pos;
	Texture *_texture;
	HGE *_hge;
	std::string _visibleVariableName;
	LuaScript *_luaScript;
};

#endif // !defined(_TESTOBJECT_INCLUDED_)
