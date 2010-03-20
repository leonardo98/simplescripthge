// InventoryObject.cpp: implementation of the InventoryObject class.
//
//////////////////////////////////////////////////////////////////////

#include "InventoryObject.h"
#include "Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InventoryObject::InventoryObject(TiXmlElement *xe)
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
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

void InventoryObject::OnMouseDown(hgeVector mousePos) {
	if (_found) {
		return;
	}
	mousePos -= _pos;
	if (_texture->IsNotTransparent((int)mousePos.x, (int)mousePos.y)) {
		_found = true;	
		Messager::SendMessage("inventory", "add " + _id);
	}
}

bool InventoryObject::IsMouseOver(hgeVector mousePos) {
	if (_found) {
		return false;
	}
	mousePos -= _pos;
	return _texture->IsNotTransparent((int)mousePos.x, (int)mousePos.y);
}
