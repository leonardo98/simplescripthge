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

struct Tool {
	std::string id;
	Texture *texture;
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
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMouseDown(hgeVector mousePos);
	virtual void OnMouseMove(hgeVector mousePos);
private:
	hgeVector _pos;
	hgeVector _lastMousePos;
	Texture *_texture;
	typedef std::list<Tool> Tools;
	Tools _tools;
	HGE *_hge;
	Texture *_active;
};

#endif // !defined(_INVENTORY_INCLUDED_)
