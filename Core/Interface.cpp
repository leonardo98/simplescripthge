#include "Interface.h"

// цвета интерфейса
DWORD Interface::BACKGROUND;
DWORD Interface::TEXT;
std::string Interface::STYLE;
DWORD Interface::BORDER_HIGH;
DWORD Interface::BORDER_LOW;

// цвета кнопок
DWORD Interface::BACKGROUND_NORMAL;
DWORD Interface::BACKGROUND_PRESSED;
DWORD Interface::BUTTON_TEXT;

hgeFont *Interface::_font;


DWORD Interface::Parse(const std::string &s) {
	assert(s.size() <= 10);
	DWORD d = 0;
	int i = int(s.size()) - 1;
	unsigned char offset = 0;
	while (i >= 0) {
		unsigned char c = s[i];
		if ('0' <= c && c <= '9') {
			c -= '0';
		} else if ('a' <= c && c <= 'f') {
			c = c - 'a'+ 10;
		} else if ('A' <= c && c <= 'F') {
			c = c - 'A'+ 10;
		} else if (c == 'x' || c == 'X') {
			assert(i == 1 && s[0] == '0');
			return d;
		} else {
			assert("Parse - unknown simbol");
		}
		d = c << offset | d;
		offset += 4;
		--i;
	}
	assert(false);
	return 0;
}

void Interface::Init(DeviceContext dc)
{
	BACKGROUND = Parse(dc->Ini_GetString("interface_common", "background", "0xFFFFFF"));
	TEXT = Parse(dc->Ini_GetString("interface_common", "text", "0x000000"));
	STYLE = dc->Ini_GetString("interface_common", "style", "none");
	BORDER_HIGH = Parse(dc->Ini_GetString("interface_common", "border_hight", "0x000000"));
	BORDER_LOW = Parse(dc->Ini_GetString("interface_common", "border_low", "0xFFFFFF"));
	std::string font = dc->Ini_GetString("interface_common", "font", "");
	_font = new hgeFont(font.c_str());

	BACKGROUND_NORMAL = Parse(dc->Ini_GetString("interface_buttons", "normal", "0xFFFFFF"));
	BACKGROUND_PRESSED = Parse(dc->Ini_GetString("interface_buttons", "pressed", "0xFFFFFF"));
	BUTTON_TEXT = Parse(dc->Ini_GetString("interface_buttons", "text", "0xFFFFFF"));
}

void Interface::Release() {
	delete _font;
}

hgeFont *Interface::Font() {
	return _font;
}