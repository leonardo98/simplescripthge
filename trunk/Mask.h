// Mask.h: interface for the Mask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MASK_INCLUDED_)
#define _MASK_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InputSystem.h"
#include "Parser.h"

class Mask 
	: public InputSystem  
{
public:
	Mask(TiXmlElement *xe);
private:
	virtual void OnMouseDown(hgeVector mousePos);
	virtual bool IsMouseOver(hgeVector mousePos);
	Parser _parser;
	hgeVector _pos;
	Texture *_texture;
};

#endif // !defined(_MASK_INCLUDED_)
