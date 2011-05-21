// device specified function
#pragma once

#include <assert.h>
#include <string>
#include <map>
#include "types.h"
#include "IwTexture.h "
#include "IwMaterial.h"

#ifndef HGE_COMPILE_KEY

class Render
{
private:
	//static HGE *_hge;
	//static std::map<std::string, hgeFont*> _fonts;
public:
	//static HGE *GetDC();
	static DWORD Parse(const std::string &s);	
	class Texture 
	{
#ifdef HGE_COMPILE_KEY
	public:
		//Texture(HTEXTURE h, int x, int y, int width, int height);
		//HTEXTURE GetTexture();
	private:
		//HTEXTURE hTexture;
		//hgeSprite *texture;
#endif
	public:
		virtual ~Texture();
		bool IsNotTransparent(int x, int y);
		void Render(float x, float y);
		void Render(const FPoint2D &pos);
		void SetBlendMode(DWORD mode);
		void SetColor(DWORD color);
	private:
		Texture(const char *fileName);
        CIwSVec2 xy3[4];
        CIwSVec2 uvs[4];
	    CIwSVec2 xy[4];
		CIwTexture *s_Texture;
		friend class TextureManager;
		friend class Render;
	};
	static void IniFile(const std::string &fileName);
	static int IniFileGetUnsignedInt(const char *section, const char *variable, unsigned int defaultValue);
	static std::string IniFileGetString(const char *section, const char *variable, const char *defaultValue);
	static void PrintString(int x, int y, std::string fontName, const std::string &text, DWORD color);
	//static bool InitApplication(hgeCallback frameFunc, hgeCallback renderFunc);
	static void RunApplication();
	static void ExitApplication();
	static std::string Error();
	static int GetFontHeight(const std::string &fontName);
	static int GetStringWidth(const std::string &fontName, const char *s);
	static void ShowMessage(const char *str, const char *caption);
	static bool ShowAskMessage(const char *str, const char *caption);
	static bool IsRightMouseButton();
	static bool IsLeftMouseButton();
	static void DrawBar(float x, float y, float width, float height, DWORD color);
	static void Line(float x1, float y1, float x2, float y2, DWORD color);
	static CIwMaterial *StartVertexBuffer(PTexture texture);
	static void FinishVertexBuffer(CIwSVec2 *xy, CIwSVec2 *uvs, unsigned int counter);
};

#else

#include <hge.h>
#include <hgeFont.h>

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
	static int IniFileGetUnsignedInt(const char *section, const char *variable, unsigned int defaultValue);
	static std::string IniFileGetString(const char *section, const char *variable, const char *defaultValue);
	static void PrintString(int x, int y, std::string fontName, const std::string &text, DWORD color);
	static bool InitApplication(hgeCallback frameFunc, hgeCallback renderFunc);
	static void RunApplication();
	static void ExitApplication();
	static std::string Error();
	static int GetFontHeight(const std::string &fontName);
	static int GetStringWidth(const std::string &fontName, const char *s);
	static void ShowMessage(const char *str, const char *caption);
	static bool ShowAskMessage(const char *str, const char *caption);
	static bool IsRightMouseButton();
	static bool IsLeftMouseButton();
	static void DrawBar(float x, float y, float width, float height, DWORD color);
	static void Line(float x1, float y1, float x2, float y2, DWORD color);
};

#endif // HGE_COMPILE_KEY
