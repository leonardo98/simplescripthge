// StaticTexture.cpp: implementation of the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "StaticTexture.h"
#include "../Core/types.h"
#include "../Core/Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


StaticTexture::StaticTexture(TiXmlElement *xe) {
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_textureId = xe->Attribute("texture"); 
	_texture = Core::getTexture(_textureId);
}

void StaticTexture::Draw() {
	_texture->Render(_pos);
}

StaticTexture::~StaticTexture() {}