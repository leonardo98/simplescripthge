// InputSystem.h: interface for the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_INPUTSYSTEM_INCLUDED_)
#define _INPUTSYSTEM_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"
#include "Object.h"
#ifdef IOS_COMPILE_KEY
#include "s3ePointer.h"
#endif
//
// надстройка над системой ввода,
// если нужно обрабатывать событи€ мыши - наследуемс€ от этого класса
// если нет - от Object.h
//

class InputSystem 
	: public Object  
{
private:
	static float DOUBLE_CLICK_TIME; // максимальное врем€ на второй клик - если врем€ больше - событие "аннулируетс€"
	static float LONG_TAP_EPS; // погрешность на случайные движени€ мыши во врем€ нажати€, если движение более сильное - событие "аннулируетс€"
	static float LONG_TAP_TIME; // минимальное врем€ на вызов событи€ - если врем€ меньше - событие "аннулируетс€"
	static float _timeCounter; // счетчик времени(отсчитывает врем€ либо с последнего событи€ нажати€ или отпускани€ кнопки мыши
	static bool _longTap; // true - если ловим событие "ƒлинный “ап"
	static bool _doubleClick; // true - если ловим это событие "ƒвойной клик"
	static FPoint2D _longTapPos;
	static InputSystem *_locked;// пишу того кто вз€лс€ обрабатывать клик(нажатие/отпускание) - остальным не делаю MouseMove
public:
	InputSystem();
	virtual ~InputSystem();
	// эти методы класса переопредел€ем 
	// если хотим обрабатывать соотвествующее событие

	// клик по элементу(нажатие левой кнопки мыши)
	virtual void OnMouseDown(FPoint2D mousePos);
	// отпустили кнопку мыши(неважно где)
	virtual void OnMouseUp();
	// движение мыши
	virtual void OnMouseMove(FPoint2D mousePos);
	// колесико мыши
	virtual bool OnMouseWheel(int direction);
	// длительное нажатие и удержание
	virtual void OnLongTap(FPoint2D mousePos);
	// двойной клик
	virtual void OnDoubleClick(FPoint2D mousePos);

	// ќЅя«ј≈Ћ№Ќќ нужно определить дл€ корректной работы OnMouseDown() и прочих функций
	virtual bool IsMouseOver(FPoint2D mousePos) = 0;
	
protected:
	// дл€ сложных(вложенных) элементов ввода - может понадобитьс€, переписать общий способ
	void RemoveFromList(InputSystem *listener); 

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;
#ifdef IOS_COMPILE_KEY
	//Simple structure to track touches
	struct CTouch
	{
		int32 x; // position x
		int32 y; // position y
		bool active; // is touch active (currently down)
		int32 id; // touch's unique identifier
	};
	#define MAX_TOUCHES 10
	static CTouch g_Touches[MAX_TOUCHES];
	static CTouch* GetTouch(int32 id);
	static void MultiTouchButtonCB(s3ePointerTouchEvent* event);
	static void MultiTouchMotionCB(s3ePointerTouchMotionEvent* event);
	static void SingleTouchButtonCB(s3ePointerEvent* event);
	static void SingleTouchMotionCB(s3ePointerMotionEvent* event);
	static void MouseDown(FPoint2D mousePos);
	static void MouseUp();
	static void MouseMove(FPoint2D mousePos);
	static void LongTap();
	static void DoubleClick(FPoint2D mousePos);
	friend void InitInputEvent();
	friend void ReleaseInputEvent();
#else
	static void MouseDown(FPoint2D mousePos);
	static void MouseUp();
	static void MouseMove(FPoint2D mousePos);
	static void MouseWheel(int direction);
	static void LongTap();
	static void DoubleClick(FPoint2D mousePos);
	friend void InitInputEvent();
	friend bool CheckForInputEvent(float dt);
#endif//IOS_COMPLILE_KEY
};

#endif // !defined(_INPUTSYSTEM_INCLUDED_)
