// InputSystem.h: interface for the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "types.h"
#include "Object.h"
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
	virtual void OnMouseDown(const FPoint2D &mousePos);
	// отпустили кнопку мыши(неважно где)
	virtual void OnMouseUp();
	// движение мыши
	virtual void OnMouseMove(const FPoint2D &mousePos);
	// колесико мыши
	virtual bool OnMouseWheel(int direction);
	// длительное нажатие и удержание
	virtual void OnLongTap(const FPoint2D &mousePos);
	// двойной клик
	virtual void OnDoubleClick(const FPoint2D &mousePos);

	// ќЅя«ј≈Ћ№Ќќ нужно определить дл€ корректной работы OnMouseDown() и прочих функций
	virtual bool IsMouseOver(const FPoint2D &mousePos) = 0;
	
protected:
	// дл€ сложных(вложенных) элементов ввода - может понадобитьс€, переписать общий способ
	void RemoveFromList(InputSystem *listener); 

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;

	static void MouseDown(const FPoint2D &mousePos);
	static void MouseUp();
	static void MouseMove(const FPoint2D &mousePos);
	static void MouseWheel(int direction);
	static void LongTap();
	static void DoubleClick(const FPoint2D &mousePos);

public:
	static void Reset();
	static void Init();
	static bool CheckForEvent(float dt);

};

#endif//INPUTSYSTEM_H