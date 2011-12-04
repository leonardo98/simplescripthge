// InventoryObject.h: interface for the InventoryObject class.
//
//////////////////////////////////////////////////////////////////////

#ifndef INVENTORYOBJECT_H
#define INVENTORYOBJECT_H

#include "../Core/Messager.h"
#include "../Core/InputSystem.h"
#include "../Core/Render.h"

class InventoryObject : 
	public InputSystem 
{
public:
	InventoryObject(TiXmlElement *xe);
	virtual ~InventoryObject();
	virtual void Draw();
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
private:
	FPoint2D _pos;
	Texture *_texture;
	bool _found;
	std::string _id;
};

#endif//INVENTORYOBJECT_H