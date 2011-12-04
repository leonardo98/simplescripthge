// ObjectFactory.h: interface for the ObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ObjectFactory_h
#define ObjectFactory_h

#include "../Core/Object.h"

class ObjectFactory  
{
public:
	static Object * Create(TiXmlElement *xe);
};

#endif//ObjectFactory_h
