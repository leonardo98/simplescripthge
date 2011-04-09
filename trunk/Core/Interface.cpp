#include "types.h"
#include "Render.h"
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

void Interface::Init()
{
	BACKGROUND = Render::IniFileGetUnsignedInt("interface_common", "background", 0xFFFFFF);
	TEXT = Render::IniFileGetUnsignedInt("interface_common", "text", 0x000000);
	STYLE = Render::IniFileGetString("interface_common", "style", "none");
	BORDER_HIGH = Render::IniFileGetUnsignedInt("interface_common", "border_hight", 0x000000);
	BORDER_LOW = Render::IniFileGetUnsignedInt("interface_common", "border_low", 0xFFFFFF);

	BACKGROUND_NORMAL = Render::IniFileGetUnsignedInt("interface_buttons", "normal", 0xFFFFFF);
	BACKGROUND_PRESSED = Render::IniFileGetUnsignedInt("interface_buttons", "pressed", 0xFFFFFF);
	BUTTON_TEXT = Render::IniFileGetUnsignedInt("interface_buttons", "text", 0xFFFFFF);

	/*
	BACKGROUND = 0xFFFFFF;
	s3eConfigGetInt("interface_common", "background", &BACKGROUND);

	char tmp[20];
	STYLE = "none";
	s3eConfigGetString("interface_common", "style", tmp);
	if (tmp) {
		STYLE = tmp;
	}*/
}

