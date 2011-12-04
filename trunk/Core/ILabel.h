// StaticTexture.h: interface for the StaticTexture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILABEL_H
#define ILABEL_H

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

#endif//ILABEL_H
