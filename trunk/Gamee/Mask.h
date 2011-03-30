// Mask.h: interface for the Mask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MASK_INCLUDED_)
#define _MASK_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\InputSystem.h"
#include "..\Core\LuaScript.h"
#include "..\Core\Variables.h"

class Mask 
	: public InputSystem  
{
public:
	Mask(TiXmlElement *xe);
	~Mask();
private:
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual bool IsMouseOver(FPoint2D mousePos);
	FPoint2D _pos;
	Texture *_texture;
	LuaScript *_luaScript;
};

#endif // !defined(_MASK_INCLUDED_)
