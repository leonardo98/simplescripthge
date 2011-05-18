// TestObject.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////

#include "TestObject.h"
#include "..\Core\Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TestObject::TestObject(TiXmlElement* xe) 
	: Object()
{
	_visibleVariableName = xe->Attribute("variableName");
	_pos.x = static_cast<float>(atoi(xe->Attribute("x")));
	_pos.y = static_cast<float>(atoi(xe->Attribute("y")));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
	TiXmlElement *script = xe->FirstChildElement("script");
	TiXmlText *text = (TiXmlText *)script->FirstChild();
	_luaScript = new LuaScript(Variables::l);
	_luaScript->Read((char *)text->Value());
}

void TestObject::Draw() {
	if (Variables::Get(_visibleVariableName.c_str()) != "true") {
		return;
	}
	_texture->Render(_pos);
}

void TestObject::Update(float) {}

TestObject::~TestObject() 
{
	delete _luaScript;
}

bool TestObject::IsMouseOver(FPoint2D mousePos)
{
	return false;
}

void TestObject::OnMouseDown(FPoint2D mousePos)
{
	//_pos = mousePos;
	_luaScript->Execute();
}