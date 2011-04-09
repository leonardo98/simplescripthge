// FpsCounter.h: interface for the FpsCounter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_FPSCOUNTER_INCLUDED_)
#define _FPSCOUNTER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\Object.h"

class FpsCounter : public Object  
{
public:
	FpsCounter();
	virtual ~FpsCounter();
	virtual void Draw();
private:
};

#endif // !defined(_FPSCOUNTER_INCLUDED_)
