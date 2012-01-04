// Mask.h: interface for the Mask class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_MASK_H
#define MYENGINE_MASK_H

#include "../Core/InputSystem.h"
#include "../Core/LuaScript.h"
#include "../Core/Variables.h"
#include "../Core/Render.h"

class Mask 
	: public InputSystem  
{
public:
	Mask(TiXmlElement *xe);
	~Mask();
private:
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	FPoint2D _pos;
	int _width;
	int _height;
	Texture *_texture;
	LuaScript *_luaScript;
};

#endif//MYENGINE_MASK_H