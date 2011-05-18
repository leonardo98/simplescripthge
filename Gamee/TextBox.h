// TextBox.h: interface for the TextBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TEXTBOX_INCLUDED_)
#define _TEXTBOX_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\Object.h"
#include "..\Core\Render.h"
#include "..\Core\Messager.h"
#include "..\Core\InputSystem.h"
#include "..\Helpers\Counter.h"

class TextBox : 
	public InputSystem, 
	public Messager  
{
public:
	TextBox(TiXmlElement *xe);
	virtual ~TextBox();
	virtual void Draw();
	virtual void Update(float deltaTime);
	virtual void OnMessage(const std::string &message);
	virtual bool IsMouseOver(FPoint2D mousePos);
	virtual void OnMouseMove(FPoint2D mousePos);
private:
	float _hideTime;
	float _width;
	typedef std::list<std::string> Texts;
	Texts _text;
	FPoint2D _pos;
	FPoint2D _lastMousePos;
	FPoint2D _clickMousePos;
	std::string _font;
	DWORD _fontColor;
	PTexture _background;
	FPoint2D _backgroundPos;
	Counter _show;
	Counter _hide;
};

#endif // !defined(_TEXTBOX_INCLUDED_)
