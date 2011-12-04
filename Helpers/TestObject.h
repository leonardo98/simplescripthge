// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include "../Core/types.h"
#include "../Core/Variables.h"
#include "../Core/LuaScript.h"
#include "../Core/Render.h"

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
	Texture *_texture;
	std::string _visibleVariableName;
	LuaScript *_luaScript;
};

#endif//TESTOBJECT_H