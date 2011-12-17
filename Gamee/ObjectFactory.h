// ObjectFactory.h: interface for the ObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_ObjectFactory_h
#define MYENGINE_ObjectFactory_h

#include "../Core/Object.h"

class ObjectFactory  
{
public:
	static Object * Create(TiXmlElement *xe);
};

#endif//MYENGINE_ObjectFactory_h
