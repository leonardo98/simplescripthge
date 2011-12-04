// ObjectFactory.cpp: implementation of the ObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "ObjectFactory.h"
//#include "../Helpers/TestObject.h"
#include "../Helpers/FpsCounter.h"
#include "../Helpers/MyMessageBox.h"
#include "StaticTexture.h"
#include "Mask.h"
#include "TextBox.h"
#include "Inventory.h"
#include "InventoryObject.h"
#include "MultiTexture.h"
#include "StarsRect.h"
#include "../Helpers/Tester.h"
#include "Button.h"
#include "../Core/Messager.h"
#include "../Core/IButton.h"
#include "../Core/IPanel.h"
#include "../Core/ISlider.h"
#include "../Core/IRadioBtns.h"
#include "../Core/ILabel.h"
#include "Simulator.h"
#include "ButtonList.h"
#include "../Core/Render.h"
#include "../Helpers/AnimationEditor.h"
#include "AnimationViewer.h"

#define REG_TYPE(a, b) if (name == a) return new b(xe)

Object * ObjectFactory::Create(TiXmlElement *xe)
{
	std::string name = xe->Value();
	//REG_TYPE("TestObject", TestObject);
	REG_TYPE("Simulator", Simulator);
	REG_TYPE("StaticTexture", StaticTexture);
	REG_TYPE("AnimationViewer", AnimationViewer);
	REG_TYPE("ILabel", ILabel);
	REG_TYPE("Mask", Mask);
	REG_TYPE("TextBox", TextBox);
	REG_TYPE("Inventory", Inventory);
	REG_TYPE("InventoryObject", InventoryObject);
	REG_TYPE("MultiTexture", MultiTexture);
	REG_TYPE("StarsRect", StarsRect);
	REG_TYPE("StarsRect", StarsRect);
	REG_TYPE("Tester", Tester);
	REG_TYPE("Button", Button);
	REG_TYPE("IButton", IButton);
	REG_TYPE("IPanel", IPanel);
	REG_TYPE("ButtonList", ButtonList);
	REG_TYPE("ISlider", ISlider);
	REG_TYPE("IRadioBtns", IRadioBtns);
	REG_TYPE("MyMessageBox", MyMessageBox);
	REG_TYPE("AnimationEditor", AnimationEditor);
	if(name == "FpsCounter") {
		return new FpsCounter();
	}
	LOG("����������� ��� ������ � XML �����");
	exit(-5);
	return NULL;
}