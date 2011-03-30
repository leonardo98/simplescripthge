// FpsCounter.cpp: implementation of the FpsCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "FpsCounter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FpsCounter::FpsCounter()
{
	_dc = hgeCreate(HGE_VERSION);
	_font = new hgeFont("data\\font1.fnt");
}

FpsCounter::~FpsCounter()
{
	delete _font;
	_dc->Release();
}

void FpsCounter::Draw() {
	_font->printf(0, 0, HGETEXT_LEFT, "fps: %d", _dc->Timer_GetFPS());
}