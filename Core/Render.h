// device specified function
#ifndef MYENGINE_RENDER_H
#define MYENGINE_RENDER_H

#include "types.h"

//#include <hge.h>
//#include <hgeFont.h>
#include "Matrix.h"

class Texture 
{
public:
	Texture(HTEXTURE hTexture, int x, int y, int w, int h, int offsetX, int offsetY, int frameWidth, int frameHeight);
	virtual ~Texture();
	bool IsNotTransparent(int x, int y) const;
	void Render(float x, float y);
	void Render(const FPoint2D &pos);
	void Render(const Matrix &transform);
	HTEXTURE GetTexture() const;
	//void SetBlendMode(DWORD mode);
	int Width() const;
	int Height() const;
private:
	void SetColor(DWORD color);
	//DWORD _currentColor;
	int _width;
	int _height;
	int _frameWidth;
	int _frameHeight;
    int _top;
    int _left;
	Texture(const std::string &fileName);
	void LoadFromFile(const std::string &fileName);
	HTEXTURE _hTexture;
	hgeSprite *_texture;
	float _offsetX;
	float _offsetY;
	hgeQuad _originQuad;
	hgeQuad _forCopyQuad;
	FPoint2D _screenVertex[4];
	friend class TextureManager;
	friend class StaticSprite;
};

class StaticSprite {
public:
	void Set(const Texture *texture, float x = 0.f, float y = 0.f);
	void Render();
	void SetTransform(const Matrix &transform);
	void PushTransform(const Matrix &transform);
	bool PixelCheck(const FPoint2D &pos);
	int SpriteWidth();
	int SpriteHeight();
	inline float GetOriginWidth() {
		return _originWidth;
	}
	inline float GetOriginHeight() {
		return _originHeight;
	}
private:
	hgeQuad _quad;
	Matrix _matrix;
	const Texture *_origin;
	float _originWidth;
	float _originHeight;
};

#define MAX_MATRIX_AMOUNT 32

class Render
{
	friend class Texture;
	friend class StaticSprite;

private:
	static HGE *_hge;
	static std::map<std::string, hgeFont*> _fonts;
	static std::map<HTEXTURE, int> _storageTextures;
	static DWORD _currentColor;
	static std::string _dataDir;
	char sdx[112];
	static Matrix _matrixStack[MAX_MATRIX_AMOUNT];
	static unsigned int _currentMatrix;
	static void DrawQuad(const hgeQuad &origin, hgeQuad &drawn);
	static void DrawTriangle(const hgeQuad &origin, hgeQuad &drawn);
	static DWORD _blendMode;
public:

	static void PushMatrix();
	static void PopMatrix();
	static void MatrixMove(float x, float y);
	static void MatrixRotate(float angle);
	static void MatrixScale(float sx, float sy);
	static void SetMatrixUnit();
	static void SetMatrix(const Matrix &matrix);

	static std::string GetDataDir();
	static void SetColor(DWORD color);
	static void SetAlpha(DWORD alpha);
	static void SetBlendMode(DWORD blendMode);
	static HGE *GetDC();
	static DWORD Parse(const std::string &s);	

	static void IniFile(const std::string &fileName);
	static int IniFileGetUnsignedInt(const char *section, const char *variable, unsigned int defaultValue);
	static std::string IniFileGetString(const char *section, const char *variable, const char *defaultValue);
	static void PrintString(int x, int y, std::string fontName, const std::string &text, DWORD color = 0xFFFFFFFF);
	static void PrintString(int x, int y, const std::string &text, int align);
	static bool InitApplication(hgeCallback frameFunc, hgeCallback renderFunc);
	static void SetDataDir(const std::string &dataDir);
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
	static void Draw(hgeTriple &triple);
	static void Line(float x1, float y1, float x2, float y2, DWORD color);
};


#endif//MYENGINE_RENDER_H