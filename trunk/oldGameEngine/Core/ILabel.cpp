// StaticTexture.cpp: implementation of the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "ILabel.h"
#include "types.h"
#include "Core.h"
#include "Variables.h"
#include "Interface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ILabel::ILabel(TiXmlElement *xe) {
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_variableName = xe->Attribute("variableName");
}

void ILabel::Draw() {
	Render::PrintString(_pos.x, _pos.y, "", Variables::Get(_variableName.c_str()), Interface::BUTTON_TEXT);
}

ILabel::~ILabel() {}