// Inventory.cpp: implementation of the Inventory class.
//
//////////////////////////////////////////////////////////////////////

#include "Inventory.h"
#include "..\Core\Core.h"
#include "..\Core\Variables.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Inventory::Inventory(TiXmlElement *xe)
	: Messager(xe)
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
	//Variables::Set("inHand", "None");
	//_variableInHand->SetValue("None");
	_dc = hgeCreate(HGE_VERSION);
	_active = NULL;
}

Inventory::~Inventory()
{
	_dc->Release();
}

void Inventory::OnMessage(const std::string &message) {
	std::string msg;
	if (CanCut(message, "add ", msg)) {
		_tools.push_back(Tool());
		_tools.back().id = msg;
		_tools.back().texture = Core::getTexture(msg + "_icon");
	} if (message == "remove from hand") {
		for (Tools::iterator i = _tools.begin(), e = _tools.end(); i != e; i++) {
			if (i->id == Variables::Get("inHand")) {
				_tools.erase(i);
				Variables::Set("inHand", "None");
				return;
			}
		}
	}
}

void Inventory::Draw() {
	_texture->Render(_pos);
	FPoint2D iconPressed(35, 13);
	iconPressed += _pos;
	std::string inHand_value = Variables::Get("inHand");
	for (Tools::iterator i = _tools.begin(), e = _tools.end(); i != e; i++) {
		if (i->id != inHand_value) {
			i->texture->Render(iconPressed);
		}
		iconPressed.x += 72;
	}
	if (inHand_value != "None" && _active != NULL) {
		_active->Render(_lastMousePos.x, _lastMousePos.y + 20);
	}
}

void Inventory::Update(float deltaTime) {
	if (_dc->Input_KeyDown(HGEK_RBUTTON)) {
		Variables::Set("inHand", "None");
		_active = NULL;
	}
}

bool Inventory::IsMouseOver(FPoint2D mousePos) {
	FPoint2D local = mousePos - _pos;
	return _texture->IsNotTransparent(int(local.x), int(local.y));
}

void Inventory::OnMouseDown(FPoint2D mousePos) {
	FPoint2D local = mousePos - (_pos + FPoint2D(35, 13));
	int width = 70;
	for (Tools::iterator i = _tools.begin(), e = _tools.end(); i != e; i++) {
		if (local.x > 0 && local.x < width && Variables::Get("inHand") != i->id) {
			Variables::Set("inHand", i->id.c_str());
			_active = i->texture;
			return;
		}
		local.x -= 72;
	}
	Variables::Set("inHand", "None");
	_active = NULL;
}

void Inventory::OnMouseMove(FPoint2D mousePos) {
	_lastMousePos = mousePos;
}