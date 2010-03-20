// ObjectFactory.h: interface for the ObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OBJECTFACTORY_INCLUDED_)
#define _OBJECTFACTORY_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"

class ObjectFactory  
{
public:
	static Object * Create(TiXmlElement *xe);
};

#endif // !defined(_OBJECTFACTORY_INCLUDED_)
