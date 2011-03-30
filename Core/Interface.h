#pragma once

#include "types.h"
#include "hgeFont.h"

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
	static void Init(DeviceContext dc);
	static void Release();
	static hgeFont *Font();
private:
	static hgeFont *_font;
};
