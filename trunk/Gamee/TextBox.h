// TextBox.h: interface for the TextBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TEXTBOX_INCLUDED_)
#define _TEXTBOX_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\Object.h"
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
	virtual void OnMessage(std::string message);
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual void OnMouseMove(hgeVector mousePos);
private:
	float _hideTime;
	float _width;
	std::vector<std::string> _text;
	hgeVector _pos;
	hgeVector _lastMousePos;
	hgeVector _clickMousePos;
	hgeFont *_font;
	Texture *_background;
	hgeVector _backgroundPos;
	Counter _show;
	Counter _hide;
};

#endif // !defined(_TEXTBOX_INCLUDED_)
