// ObjectFactory.cpp: implementation of the ObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "ObjectFactory.h"
#include "../Core/CoreFactory.h"
#include "../Helpers/FpsCounter.h"
#include "../Helpers/MyMessageBox.h"
#include "StaticTexture.h"
#include "Mask.h"
#include "TextBox.h"
#include "Inventory.h"
#include "InventoryObject.h"
#include "MultiTexture.h"
#include "StarsRect.h"
#include "Button.h"
#include "Simulator.h"
#include "BykerGame.h"
#include "ButtonList.h"
#include "AnimationViewer.h"

#define REG_TYPE(a, b) if (name == a) return new b(xe)

Object * ObjectFactory::Create(TiXmlElement *xe)
{
	std::string name = xe->Value();
	REG_TYPE("Simulator", Simulator);
	REG_TYPE("BykerGame", BykerGame);
	REG_TYPE("StaticTexture", StaticTexture);
	REG_TYPE("AnimationViewer", AnimationViewer);
	REG_TYPE("Mask", Mask);
	REG_TYPE("TextBox", TextBox);
	REG_TYPE("Inventory", Inventory);
	REG_TYPE("InventoryObject", InventoryObject);
	REG_TYPE("MultiTexture", MultiTexture);
	REG_TYPE("StarsRect", StarsRect);
	REG_TYPE("Button", Button);
	REG_TYPE("ButtonList", ButtonList);
	REG_TYPE("MyMessageBox", MyMessageBox);
	if(name == "FpsCounter") {
		return new FpsCounter();
	}
	return CoreFactory::Create(xe);
}