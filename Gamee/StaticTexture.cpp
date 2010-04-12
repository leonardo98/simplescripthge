// StaticTexture.cpp: implementation of the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "StaticTexture.h"
#include "..\Core\types.h"
#include "..\Core\Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


StaticTexture::StaticTexture(TiXmlElement *xe) {
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
}

void StaticTexture::Draw() {
	_texture->Render(_pos);
}
