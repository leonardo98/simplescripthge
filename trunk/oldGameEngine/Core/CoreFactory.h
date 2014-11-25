// CoreFactory.h: interface for the CoreFactory class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_CoreFactory_h
#define MYENGINE_CoreFactory_h

#include "../Core/Object.h"

class CoreFactory  
{
public:
	static Object * Create(TiXmlElement *xe);
};

#endif//MYENGINE_CoreFactory_h
