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
	const char *tmp = xe->Attribute("texture");
	if (tmp) {
		_texture = Core::getTexture(tmp);
	} else {
		_texture = NULL;
		_width = static_cast<float>(atoi(xe->Attribute("width")));;
		_height = static_cast<float>(atoi(xe->Attribute("height")));;
	}
}

Mask::~Mask()
{
	delete _luaScript;
}

void Mask::OnMouseDown(const FPoint2D &mousePos) {
	_luaScript->Execute();
}

bool Mask::IsMouseOver(const FPoint2D &mousePos) {
	return (_texture != NULL && _texture->IsNotTransparent((int)(mousePos.x - _pos.x), (int)(mousePos.y - _pos.y)))
		|| (_pos.x <= mousePos.x && mousePos.x < _pos.x + _width && _pos.y <= mousePos.y && mousePos.y < _pos.y + _height);
}
