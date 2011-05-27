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
	BACKGROUND = Render::IniFileGetUnsignedInt("interface_common", "background", 0xFFFFFFFF);
	TEXT = Render::IniFileGetUnsignedInt("interface_common", "text", 0xFF000000);
	STYLE = Render::IniFileGetString("interface_common", "style", "none");
	BORDER_HIGH = Render::IniFileGetUnsignedInt("interface_common", "border_hight", 0xFF000000);
	BORDER_LOW = Render::IniFileGetUnsignedInt("interface_common", "border_low", 0xFFFFFFFF);

	BACKGROUND_NORMAL = Render::IniFileGetUnsignedInt("interface_buttons", "normal", 0xFFFFFFFF);
	BACKGROUND_PRESSED = Render::IniFileGetUnsignedInt("interface_buttons", "pressed", 0xFFFFFFFF);
	BUTTON_TEXT = Render::IniFileGetUnsignedInt("interface_buttons", "text", 0xFFFFFFFF);

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

