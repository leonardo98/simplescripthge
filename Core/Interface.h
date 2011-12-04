#ifndef INTERFACE_H
#define INTERFACE_H

#include "types.h"

#include "../../hge181-for-osx/trunk/include/hgeFont.h"

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

#endif//INTERFACE_H