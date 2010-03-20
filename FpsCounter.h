// FpsCounter.h: interface for the FpsCounter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_FPSCOUNTER_INCLUDED_)
#define _FPSCOUNTER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"

class FpsCounter : public Object  
{
public:
	FpsCounter();
	virtual ~FpsCounter();
	virtual void Draw();
private:
	HGE *_hge;
	hgeFont *_font;
};

#endif // !defined(_FPSCOUNTER_INCLUDED_)
