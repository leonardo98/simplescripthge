// StaticTexture.h: interface for the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_ILABEL_H
#define MYENGINE_ILABEL_H

#include "Object.h"
#include "Render.h"

class ILabel : public Object  
{
public:
	ILabel(TiXmlElement *xe);
	virtual ~ILabel();
	void Draw();
private:
	FPoint2D _pos;
	std::string _variableName;
};

#endif//MYENGINE_ILABEL_H
