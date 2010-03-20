// FpsCounter.cpp: implementation of the FpsCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "FpsCounter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FpsCounter::FpsCounter()
{
	_hge = hgeCreate(HGE_VERSION);
	_font = new hgeFont("data\\font1.fnt");
}

FpsCounter::~FpsCounter()
{
	delete _font;
	_hge->Release();
}

void FpsCounter::Draw() {
	_font->printf(0, 0, HGETEXT_LEFT, "fps: %d", _hge->Timer_GetFPS());
}