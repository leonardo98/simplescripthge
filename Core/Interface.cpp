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
	BACKGROUND = Render::GetDC()->Ini_GetInt("interface_common", "background", 0xFFD0D0D0);
	TEXT = Render::GetDC()->Ini_GetInt("interface_common", "text", 0x000000);
	STYLE = Render::GetDC()->Ini_GetString("interface_common", "style", "none");
	BORDER_HIGH = Render::GetDC()->Ini_GetInt("interface_common", "border_hight", 0xFFFFFFFF);
	BORDER_LOW = Render::GetDC()->Ini_GetInt("interface_common", "border_low", 0xFF707070);

	BACKGROUND_NORMAL = Render::GetDC()->Ini_GetInt("interface_buttons", "normal", 0xFFB0B0B0);
	BACKGROUND_PRESSED = Render::GetDC()->Ini_GetInt("interface_buttons", "pressed", 0xFF909090);
	BUTTON_TEXT = Render::GetDC()->Ini_GetInt("interface_buttons", "text", 0xFF000000);
}

