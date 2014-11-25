// InventoryObject.cpp: implementation of the InventoryObject class.
//
//////////////////////////////////////////////////////////////////////

#include "InventoryObject.h"
#include "../Core/Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InventoryObject::InventoryObject(TiXmlElement *xe)
{
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	_id = xe->Attribute("id"); 
	_texture = Core::getTexture(_id);
	_found = false;
}

InventoryObject::~InventoryObject()
{

}

void InventoryObject::Draw() {
	if (_found) {
		return;
	}
	_texture->Render(_pos);
}

void InventoryObject::OnMouseDown(const FPoint2D &mousePos) {
	if (_found) {
		return;
	}
	if (_texture->IsNotTransparent((int)(mousePos.x - _pos.x), (int)(mousePos.y - _pos.y))) {
		_found = true;	
		Messager::SendMessage("inventory", "add " + _id);
	}
}

bool InventoryObject::IsMouseOver(const FPoint2D &mousePos) {
	if (_found) {
		return false;
	}
	return _texture->IsNotTransparent((int)(mousePos.x - _pos.x), (int)(mousePos.y - _pos.y));
}
