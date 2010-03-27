// Mask.cpp: implementation of the Mask class.
//
//////////////////////////////////////////////////////////////////////

#include "Mask.h"
#include "Core.h"

//////////////////////////////////////////////////////////////////////

Mask::Mask(TiXmlElement *xe)
	: _luaScript(new LuaScript(Variables::l, xe->FirstChildElement("script")))
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
}

Mask::~Mask()
{
	delete _luaScript;
}

void Mask::OnMouseDown(hgeVector mousePos) {
	mousePos -= _pos;
	if (_texture->IsNotTransparent((int)mousePos.x, (int)mousePos.y)) {
		_luaScript->Execute();
	}
}

bool Mask::IsMouseOver(hgeVector mousePos) {
	mousePos -= _pos;
	return _texture->IsNotTransparent((int)mousePos.x, (int)mousePos.y);
}
