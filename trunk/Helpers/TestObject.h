// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TESTOBJECT_INCLUDED_)
#define _TESTOBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\types.h"
#include "..\Core\Variables.h"
#include "..\Core\LuaScript.h"
#include "..\Core\Render.h"

class TestObject  
	: public Object	
{
public:
	TestObject(TiXmlElement* xe);
	~TestObject();
	virtual void Draw();
	virtual void Update(float);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseDown(FPoint2D mousePos);
private:
	FPoint2D _pos;
	PTexture _texture;
	std::string _visibleVariableName;
	LuaScript *_luaScript;
};

#endif // !defined(_TESTOBJECT_INCLUDED_)
