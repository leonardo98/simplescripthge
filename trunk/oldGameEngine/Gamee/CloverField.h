#ifndef MYENGINE_CLOVERFIELD_H
#define MYENGINE_CLOVERFIELD_H

#include "BaseField.h"
#include "../Core/Render.h"

class CloverField 
	: public BaseField
{
public:
	CloverField();
	virtual void DrawBottom();
private:
	StaticSprite _stoneshelve_set1_c;
};

#endif//MYENGINE_CLOVERFIELD_H