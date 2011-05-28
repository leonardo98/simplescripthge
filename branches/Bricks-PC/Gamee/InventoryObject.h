// InventoryObject.h: interface for the InventoryObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_INVENTORYOBJECT_INCLUDED_)
#define _INVENTORYOBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\Messager.h"
#include "..\Core\InputSystem.h"
#include "..\Core\Render.h"

class InventoryObject : 
	public InputSystem 
{
public:
	InventoryObject(TiXmlElement *xe);
	virtual ~InventoryObject();
	virtual void Draw();
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual bool IsMouseOver(FPoint2D mousePos);
private:
	FPoint2D _pos;
	PTexture _texture;
	bool _found;
	std::string _id;
};

#endif // !defined(_INVENTORYOBJECT_INCLUDED_)
