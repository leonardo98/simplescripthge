// CoreFactory.cpp: implementation of the CoreFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "CoreFactory.h"
#include "IPanel.h"
#include "IButton.h"
#include "ISlider.h"
#include "IRadioBtns.h"
#include "ILabel.h"

#define REG_TYPE(a, b) if (name == a) return new b(xe)

Object * CoreFactory::Create(TiXmlElement *xe)
{
	std::string name = xe->Value();
	REG_TYPE("ILabel", ILabel);
	REG_TYPE("IButton", IButton);
	REG_TYPE("IPanel", IPanel);
	REG_TYPE("ISlider", ISlider);
	REG_TYPE("IRadioBtns", IRadioBtns);
	LOG("Неизвестный тип данных в XML файле");
	exit(-5);
	return NULL;
}