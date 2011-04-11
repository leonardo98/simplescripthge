// Inventory.h: interface for the Inventory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_INVENTORY_INCLUDED_)
#define _INVENTORY_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\Messager.h"
#include "..\Core\InputSystem.h"
#include "..\Core\Render.h"

struct Tool {
	std::string id;
	PTexture texture;
};

class Inventory 
	: public InputSystem
	, public Messager  
{
public:
	Inventory(TiXmlElement *xe);
	virtual ~Inventory();
	virtual void OnMessage(const std::string &message);
	virtual void Draw();
	virtual void Update(float deltaTime);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseDown(FPoint2D mousePos);
	virtual void OnMouseMove(FPoint2D mousePos);
private:
	FPoint2D _pos;
	FPoint2D _lastMousePos;
	PTexture _texture;
	typedef std::list<Tool> Tools;
	Tools _tools;
	PTexture _active;
};

#endif // !defined(_INVENTORY_INCLUDED_)
