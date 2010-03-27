// Mask.h: interface for the Mask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MASK_INCLUDED_)
#define _MASK_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InputSystem.h"
#include "LuaScript.h"
#include "Variables.h"

class Mask 
	: public InputSystem  
{
public:
	Mask(TiXmlElement *xe);
	~Mask();
private:
	virtual void OnMouseDown(hgeVector mousePos);
	virtual bool IsMouseOver(hgeVector mousePos);
	hgeVector _pos;
	Texture *_texture;
	LuaScript *_luaScript;
};

#endif // !defined(_MASK_INCLUDED_)
