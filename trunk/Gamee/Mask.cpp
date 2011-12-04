// Mask.cpp: implementation of the Mask class.
//
//////////////////////////////////////////////////////////////////////

#include "Mask.h"
#include "../Core/Core.h"

//////////////////////////////////////////////////////////////////////

Mask::Mask(TiXmlElement *xe)
	: _luaScript(new LuaScript(Variables::l, xe->FirstChildElement("script")))
{
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
}

Mask::~Mask()
{
	delete _luaScript;
}

void Mask::OnMouseDown(const FPoint2D &mousePos) {
	if (_texture->IsNotTransparent((int)(mousePos.x - _pos.x), (int)(mousePos.y - _pos.y))) {
		_luaScript->Execute();
	}
}

bool Mask::IsMouseOver(const FPoint2D &mousePos) {
	return _texture->IsNotTransparent((int)(mousePos.x - _pos.x), (int)(mousePos.y - _pos.y));
}
