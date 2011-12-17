// FpsCounter.h: interface for the FpsCounter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_FPSCOUNTER_H
#define MYENGINE_FPSCOUNTER_H

#include "../Core/Object.h"

class FpsCounter : public Object  
{
public:
	FpsCounter();
	virtual ~FpsCounter();
	virtual void Draw();
private:
};

#endif//MYENGINE_FPSCOUNTER_H