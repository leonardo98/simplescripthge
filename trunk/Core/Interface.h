#pragma once

#include "types.h"

#ifndef IOS_COMPILE_KEY
#include "hgeFont.h"
#endif // IOS_COMPILE_KEY

class Interface
{
public:
	// цвета интерфейса
	static DWORD BACKGROUND;
	static DWORD TEXT;
	static std::string STYLE;
	static DWORD BORDER_HIGH;
	static DWORD BORDER_LOW;

	// цвета кнопок
	static DWORD BACKGROUND_NORMAL;
	static DWORD BACKGROUND_PRESSED;
	static DWORD BUTTON_TEXT;
	static DWORD Parse(const std::string &s);
	static void Init();
};
