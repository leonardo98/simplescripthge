// TestObject.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////

#include "TestObject.h"
#include "Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TestObject::TestObject(TiXmlElement* xe) 
	: Object()
{
	_visible = Variables::GetLink(xe->Attribute("variableName"));
	_hge = hgeCreate(HGE_VERSION);
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
	TiXmlElement *script = xe->FirstChildElement("script");
	TiXmlText *text = (TiXmlText *)script->FirstChild();
	_parser.Read((char *)text->Value());
}

void TestObject::Draw() {
	if (_visible->GetValue() != "true") {
		return;
	}
	_texture->Render(_pos);
}

void TestObject::Update(float) {}

TestObject::~TestObject() 
{
	_hge->Release();
}

bool TestObject::IsMouseOver(hgeVector mousePos)
{
	return false;
}

void TestObject::OnMouseDown(hgeVector mousePos)
{
	//_pos = mousePos;
	_parser.Execute();
}