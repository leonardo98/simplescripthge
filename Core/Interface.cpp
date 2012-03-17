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
	BACKGROUND = Render::GetDC()->Ini_GetInt("interface_common", "background", 0xFFFFFFFF);
	TEXT = Render::GetDC()->Ini_GetInt("interface_common", "text", 0xFF000000);
	STYLE = Render::GetDC()->Ini_GetString("interface_common", "style", "none");
	BORDER_HIGH = Render::GetDC()->Ini_GetInt("interface_common", "border_hight", 0xFF000000);
	BORDER_LOW = Render::GetDC()->Ini_GetInt("interface_common", "border_low", 0xFFFFFFFF);

	BACKGROUND_NORMAL = Render::GetDC()->Ini_GetInt("interface_buttons", "normal", 0xFFFFFFFF);
	BACKGROUND_PRESSED = Render::GetDC()->Ini_GetInt("interface_buttons", "pressed", 0xFFFFFFFF);
	BUTTON_TEXT = Render::GetDC()->Ini_GetInt("interface_buttons", "text", 0xFFFFFFFF);
}

