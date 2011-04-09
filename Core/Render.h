// device specified function

#include <assert.h>
#include <hge.h>
#include <hgeFont.h>
#include <string>
#include <map>

class Render
{
private:
	static HGE *_hge;
	static std::map<std::string, hgeFont*> _fonts;
public:
	static HGE *GetDC();
	static DWORD Parse(const std::string &s);	
	class Texture 
		: public hgeSprite
	{
	public:
		Texture(HTEXTURE h, int x, int y, int width, int height);
		virtual ~Texture();
		bool IsNotTransparent(int x, int y);
		void Render(float x, float y);
	};
	static void IniFile(const std::string &fileName);
	static int IniFileGetUnsignedInt(const std::string &section, const std::string &variable, unsigned int defaultValue);
	static std::string IniFileGetString(const std::string &section, const std::string &variable, const std::string &defaultValue);
	static void PrintString(int x, int y, std::string fontName, const std::string &text, DWORD color);
	static void InitApplication(hgeCallback frameFunc, hgeCallback renderFunc);
	static int GetFontHeight(const std::string &fontName);
	static int GetStringWidth(const std::string &fontName, const char *s);
	static void ShowMessage(const char *str, const char *caption);
	static bool ShowAskMessage(const char *str, const char *caption);
	static bool IsRightMouseButton();
	static bool IsLeftMouseButton();
	static void DrawBar(float x, float y, float width, float height, DWORD color);
};
