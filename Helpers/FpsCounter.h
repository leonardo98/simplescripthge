// FpsCounter.h: interface for the FpsCounter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include "../Core/Object.h"

class FpsCounter : public Object  
{
public:
	FpsCounter();
	virtual ~FpsCounter();
	virtual void Draw();
private:
};

#endif//FPSCOUNTER_H