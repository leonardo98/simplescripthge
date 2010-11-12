// ObjectFactory.cpp: implementation of the ObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "ObjectFactory.h"
#include "..\Helpers\TestObject.h"
#include "..\Helpers\FpsCounter.h"
#include "StaticTexture.h"
#include "Mask.h"
#include "TextBox.h"
#include "Inventory.h"
#include "InventoryObject.h"
#include "MultiTexture.h"
#include "StarsRect.h"
#include "..\Helpers\Tester.h"
#include "..\Core\Messager.h"
#include "VideoFrame.h"
#include "Button.h"
#include "..\Core\IButton.h"
#include "..\Core\IPanel.h"
#include "..\Core\IPanel.h"
#include "Simulator.h"

#define REG_TYPE(a, b) if (name == a) return new b(xe)

Object * ObjectFactory::Create(TiXmlElement *xe)
{
	std::string name = xe->Value();
	REG_TYPE("TestObject", TestObject);
	REG_TYPE("Simulator", Simulator);
	REG_TYPE("StaticTexture", StaticTexture);
	REG_TYPE("Mask", Mask);
	REG_TYPE("TextBox", TextBox);
	REG_TYPE("Inventory", Inventory);
	REG_TYPE("InventoryObject", InventoryObject);
	REG_TYPE("MultiTexture", MultiTexture);
	REG_TYPE("StarsRect", StarsRect);
	REG_TYPE("VideoFrame", VideoFrame);
	REG_TYPE("StarsRect", StarsRect);
	REG_TYPE("Tester", Tester);
	REG_TYPE("Button", Button);
	REG_TYPE("IButton", IButton);
	REG_TYPE("IPanel", IPanel);
	if(name == "FpsCounter") {
		return new FpsCounter();
	}
	Messager::SendMessage("log", "Неизвестный тип данных в XML файле");
	exit(-5);
	return NULL;
}