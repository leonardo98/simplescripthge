// Inventory.h: interface for the Inventory class.
//
//////////////////////////////////////////////////////////////////////

#ifndef INVENTORY_H
#define INVENTORY_H

#include "../Core/Messager.h"
#include "../Core/InputSystem.h"
#include "../Core/Render.h"

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
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseMove(const FPoint2D &mousePos);
private:
	FPoint2D _pos;
	FPoint2D _lastMousePos;
	Texture *_texture;
	typedef std::list<Tool> Tools;
	Tools _tools;
	Texture *_active;
};

#endif//INVENTORY_H