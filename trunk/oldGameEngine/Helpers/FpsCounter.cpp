// FpsCounter.cpp: implementation of the FpsCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "FpsCounter.h"
#include "../Core/Render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FpsCounter::FpsCounter()
{
}

FpsCounter::~FpsCounter()
{
}

void FpsCounter::Draw() {
	char buff[10];
	sprintf(buff, "fps: %d", Render::GetDC()->Timer_GetFPS());
	Render::PrintString(0, 0, "", buff, 0xFFFFFFFF);
}