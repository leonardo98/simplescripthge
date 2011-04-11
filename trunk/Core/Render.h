// device specified function
#pragma once

#include <assert.h>
#include <hge.h>
#include <hgeFont.h>
#include <string>
#include <map>
#include "types.h"

class Render
{
private:
	static HGE *_hge;
	static std::map<std::string, hgeFont*> _fonts;
public:
	static HGE *GetDC();
	static DWORD Parse(const std::string &s);	
	class Texture 
	{
	public:
		Texture(HTEXTURE h, int x, int y, int width, int height);
		virtual ~Texture();
		bool IsNotTransparent(int x, int y);
		void Render(float x, float y);
		void Render(const FPoint2D &pos);
		HTEXTURE GetTexture();
		void SetBlendMode(DWORD mode);
		void SetColor(DWORD color);
	private:
		Texture(const std::string &fileName);
		void LoadFromFile(const std::string &fileName);
		HTEXTURE hTexture;
		hgeSprite *texture;
		friend class TextureManager;
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
	static void Line(float x1, float y1, float x2, float y2, DWORD color);
};
