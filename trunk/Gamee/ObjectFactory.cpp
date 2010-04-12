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

Object * ObjectFactory::Create(TiXmlElement *xe)
{
	std::string name = xe->Value();
	if(name == "TestObject") {
		return new TestObject(xe);
	} else if(name == "FpsCounter") {
		return new FpsCounter();
	} else if(name == "StaticTexture") {
		return new StaticTexture(xe);
	} else if(name == "Mask") {
		return new Mask(xe);
	} else if(name == "TextBox") {
		return new TextBox(xe);
	} else if(name == "Inventory") {
		return new Inventory(xe);
	} else if(name == "InventoryObject") {
		return new InventoryObject(xe);
	} else if(name == "MultiTexture") {
		return new MultiTexture(xe);
	} else if(name == "StarsRect") {
		return new StarsRect(xe);
	} else if(name == "VideoFrame") {
		return new VideoFrame(xe);
	} else if(name == "Tester") {
		return new Tester(xe);
	} else {
		Messager::SendMessage("log", "����������� ��� � XML");
		exit(-5);
	}
	return NULL;
}