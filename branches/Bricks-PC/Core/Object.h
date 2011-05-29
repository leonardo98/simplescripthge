// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OBJECT_INCLUDED_)
#define _OBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

//
// базовый класс, наследуемся от него если нам нужны только два 
// метода и не нужно обрабатывать ввод, иначе см. InputSystem.h
//

class Object  
{
public:
	Object(bool show = true):bVisible(show) {} //По дефолту все обьекты видимые
	virtual ~Object();
	virtual void Draw();
	virtual void Update(float);

	void Show(bool show) { bVisible = show; } 
	bool isVisible() { return bVisible; }
private:
	bool bVisible;	//Скрытые обьекты не получают события от клавиатуры,
					//не рисуються и не апдейтятся если обрабатываються
					//системой (Core, InputSystem)
};

#endif // !defined(_OBJECT_INCLUDED_)
