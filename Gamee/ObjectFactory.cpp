// ObjectFactory.cpp: implementation of the ObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "ObjectFactory.h"
//#include "..\Helpers\TestObject.h"
#include "..\Helpers\FpsCounter.h"
/*#include "StaticTexture.h"
#include "Mask.h"
#include "TextBox.h"
#include "Inventory.h"
#include "InventoryObject.h"
#include "MultiTexture.h"
#include "StarsRect.h"
#include "..\Helpers\Tester.h"
#include "Button.h"*/
#include "..\Core\Messager.h"
#include "..\Core\IButton.h"
#include "..\Core\IPanel.h"
#include "..\Core\ISlider.h"
#include "..\Core\IRadioBtns.h"
#include "Simulator.h"
#include "ButtonList.h"
#include "..\Core\Render.h"

#define REG_TYPE(a, b) if (name == a) return new b(xe)

Object * ObjectFactory::Create(TiXmlElement *xe)
{
	std::string name = xe->Value();
	//REG_TYPE("TestObject", TestObject);
	REG_TYPE("Simulator", Simulator);
	/*REG_TYPE("StaticTexture", StaticTexture);
	REG_TYPE("Mask", Mask);
	REG_TYPE("TextBox", TextBox);
	REG_TYPE("Inventory", Inventory);
	REG_TYPE("InventoryObject", InventoryObject);
	REG_TYPE("MultiTexture", MultiTexture);
	REG_TYPE("StarsRect", StarsRect);
	REG_TYPE("StarsRect", StarsRect);
	REG_TYPE("Tester", Tester);
	REG_TYPE("Button", Button);*/
	REG_TYPE("IButton", IButton);
	REG_TYPE("IPanel", IPanel);
	REG_TYPE("ButtonList", ButtonList);
	REG_TYPE("ISlider", ISlider);
	REG_TYPE("IRadioBtns", IRadioBtns);
	if(name == "FpsCounter") {
		return new FpsCounter();
	}
	LOG("Неизвестный тип данных в XML файле");
	exit(-5);
	return NULL;
}