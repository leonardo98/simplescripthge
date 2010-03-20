// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OBJECT_INCLUDED_)
#define _OBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

class Object  
{
public:
	virtual ~Object();
	virtual void Draw();
	virtual void Update(float);
};

#endif // !defined(_OBJECT_INCLUDED_)
