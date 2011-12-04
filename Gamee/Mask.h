// Mask.h: interface for the Mask class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MASK_H
#define MASK_H

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
	Texture *_texture;
	LuaScript *_luaScript;
};

#endif//MASK_H