// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TESTOBJECT_INCLUDED_)
#define _TESTOBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Parser.h"
#include "Variables.h"

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
	Parser _parser;
	Variable::Ptr _visible;
};

#endif // !defined(_TESTOBJECT_INCLUDED_)
