// device specified function

#include "Render.h"
#include "s3e.h"

DWORD Render::Parse(const std::string &s) {
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

#ifdef HGE_COMPILE_KEY

HGE *Render::_hge = NULL;
std::map<std::string, hgeFont*> Render::_fonts;

HGE *Render::GetDC() {
	return _hge;
}

Render::Texture::Texture(HTEXTURE h, int x, int y, int width, int height)
{
	assert(false);
}

Render::Texture::Texture(const std::string &fileName) {
	hTexture = GetDC()->Texture_Load(fileName.c_str());
	if (hTexture == NULL) {
//		LOG("Не могу открыть файл " + fileName);
		exit(-7);
	}
	texture = new hgeSprite(hTexture, 0, 0, GetDC()->Texture_GetWidth(hTexture), GetDC()->Texture_GetHeight(hTexture));
}

Render::Texture::~Texture() {
	delete texture;
	GetDC()->Texture_Free(hTexture);
}

bool Render::Texture::IsNotTransparent(int x, int y) {
	if (x < 0 || y < 0) {
		return false;
	}
	HTEXTURE h;
	h = texture->GetTexture();
	if (x >= GetDC()->Texture_GetWidth(h) || y >= GetDC()->Texture_GetHeight(h)) {
		return false;
	}
	DWORD *dw;
	dw = GetDC()->Texture_Lock(h, true, x, y, 1, 1);
	bool result = ((*dw) >> 24) > 0x7F;
	GetDC()->Texture_Unlock(h);
	return result;
}

void Render::Texture::Render(float x, float y) {
	texture->Render(x, y);
}

void Render::Texture::Render(const FPoint2D &pos)  {
	texture->Render(pos.x, pos.y);
}

HTEXTURE Render::Texture::GetTexture() {
	return texture->GetTexture();
}

void Render::Texture::SetBlendMode(DWORD mode) {
	texture->SetBlendMode(mode);
}

void Render::Texture::SetColor(DWORD color) {
	texture->SetColor(color);
}

void Render::IniFile(const std::string &fileName) {
	_hge->System_SetState(HGE_INIFILE, fileName.c_str());
}

int Render::IniFileGetUnsignedInt(const char *section, const char *variable, unsigned int defaultValue) {
	char buff[10];
	sprintf(buff, "%d", defaultValue);
	return Parse(_hge->Ini_GetString(section, variable, buff));
}

std::string Render::IniFileGetString(const char *section, const char *variable, const char *defaultValue) {
	return _hge->Ini_GetString(section, variable, defaultValue);
}

void Render::PrintString(int x, int y, std::string fontName, const std::string &text, DWORD color = 0xFFFFFFFF) {
	if (fontName == "") {
		fontName = "data\\font2.fnt";
	}
	hgeFont *font;
	if (_fonts.find(fontName) == _fonts.end()) {
		_fonts[fontName] = new hgeFont(fontName.c_str());
	}
	font = _fonts[fontName];
	font->SetColor(color);
	font->Render(x, y, HGETEXT_CENTER, text.c_str());
}

int Render::GetFontHeight(const std::string &fontName) {
	if (_fonts.find(fontName) == _fonts.end()) {
		_fonts[fontName] = new hgeFont(fontName.c_str());
	}
	hgeFont *font = _fonts[fontName];
	return font->GetHeight();
}

int Render::GetStringWidth(const std::string &fontName, const char *s) {
	if (_fonts.find(fontName) == _fonts.end()) {
		_fonts[fontName] = new hgeFont(fontName.c_str());
	}
	hgeFont *font = _fonts[fontName];
	return font->GetStringWidth(s);
}

bool Render::InitApplication(hgeCallback frameFunc, hgeCallback renderFunc) {
	_hge = hgeCreate(HGE_VERSION);
	_hge->System_SetState(HGE_INIFILE, "settings.ini");
	_hge->System_SetState(HGE_LOGFILE, _hge->Ini_GetString("system", "logfile", "log.txt"));
	_hge->System_SetState(HGE_FRAMEFUNC, frameFunc);
	_hge->System_SetState(HGE_RENDERFUNC, renderFunc);
	_hge->System_SetState(HGE_WINDOWED, _hge->Ini_GetInt("system", "fullscreen", 0) == 0);
	_hge->System_SetState(HGE_SCREENWIDTH, _hge->Ini_GetInt("system", "width", 800));
	_hge->System_SetState(HGE_SCREENHEIGHT, _hge->Ini_GetInt("system", "height", 600));
	_hge->System_SetState(HGE_SCREENBPP, 32);
	_hge->System_SetState(HGE_FPS, _hge->Ini_GetInt("system", "vsync", 0) == 0?_hge->Ini_GetInt("system", "fps", 0):HGEFPS_VSYNC);	
	_hge->System_SetState(HGE_USESOUND, false);
	_hge->System_SetState(HGE_SHOWSPLASH, false);
	_hge->System_SetState(HGE_HIDEMOUSE, false);
	_hge->System_SetState(HGE_ZBUFFER, false);
	_hge->System_SetState(HGE_TITLE, "Simple script application");
	return _hge->System_Initiate();
}

void Render::RunApplication() {
	_hge->System_Start();
}

std::string Render::Error() {
	return _hge->System_GetErrorMessage();
}

void Render::ExitApplication() {
	_hge->System_Shutdown();
	_hge->Release();
}

void Render::ShowMessage(const char *str, const char *caption) {
	MessageBox(GetDC()->System_GetState(HGE_HWND), str, caption, MB_OK | MB_APPLMODAL);
}

bool Render::ShowAskMessage(const char *str, const char *caption) {
	return (MessageBox(GetDC()->System_GetState(HGE_HWND), "Delete selected object?", "Are you sure?", MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES);
}

bool Render::IsRightMouseButton() {
	return GetDC()->Input_KeyDown(HGEK_RBUTTON);
}

bool Render::IsLeftMouseButton() {
	return GetDC()->Input_KeyDown(HGEK_LBUTTON);
}

void Render::DrawBar(float x, float y, float width, float height, DWORD color) {
	hgeQuad quad;
	quad.blend = BLEND_ALPHABLEND;
	quad.tex = NULL;
	quad.v[0].x = x;
	quad.v[0].y = y;
	quad.v[1].x = x + width;
	quad.v[1].y = y;
	quad.v[2].x = x + width;
	quad.v[2].y = y + height;
	quad.v[3].x = x;
	quad.v[3].y = y + height;
	for (int i = 0; i < 4; i++) {
		quad.v[i].col = color;
		quad.v[i].tx = 0;
		quad.v[i].ty = 0;
		quad.v[i].z = 0;
	}
	GetDC()->Gfx_RenderQuad(&quad);
}

void Render::Line(float x1, float y1, float x2, float y2, DWORD color) {
	GetDC()->Gfx_RenderLine(x1, y1, x2, y2, color);
}

#else

#include "Iw2D.h"
#include "IwGxPrint.h"

Render::Texture::Texture(const char *fileName) {
	s_Texture = new CIwTexture();
	s_Texture->LoadFromFile(fileName);
	s_Texture->Upload();

    // Set up screenspace vertex coords
    int16 x1 = 0;
    int16 x2 = (int16)s_Texture->GetWidth();
    int16 y1 = 0;
    int16 y2 = (int16)s_Texture->GetHeight();
        
    xy3[0].x = x1, xy3[0].y = y1;
    xy3[1].x = x1, xy3[1].y = y2;
    xy3[2].x = x2, xy3[2].y = y2;
    xy3[3].x = x2, xy3[3].y = y1;

    uvs[0] = CIwSVec2(0 << 12, 0 << 12);
    uvs[1] = CIwSVec2(0 << 12, 1 << 12);
    uvs[2] = CIwSVec2(1 << 12, 1 << 12);
    uvs[3] = CIwSVec2(1 << 12, 0 << 12);
}

Render::Texture::~Texture() {
	delete s_Texture;
}

bool Render::Texture::IsNotTransparent(int x, int y) {
	assert(false);
	return false;
}

void Render::Texture::Render(float x, float y) 
{
	for (int i = 0; i < 4; i++) {
		xy[i].x = xy3[i].x + static_cast<int16>(x);
		xy[i].y = xy3[i].y + static_cast<int16>(y);
	}
	// Allocate a material from the IwGx global cache
    CIwMaterial *pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    // Use Texture on Material
    pMat->SetTexture(s_Texture);
	// Set this as the active material
    IwGxSetMaterial(pMat);
	IwGxSetVertStreamScreenSpace(xy, 4);
    IwGxSetUVStream(uvs);
    // Draw single triangle
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
 }

void Render::Texture::Render(const FPoint2D &pos)  {
	Render(pos.x, pos.y);
}

void Render::Texture::SetBlendMode(DWORD mode) {
	assert(false);
}

void Render::Texture::SetColor(DWORD color) {
	assert(false);
}

void Render::PrintString(int x, int y, std::string fontName, const std::string &text, DWORD color = 0xFFFFFFFF) {
	int l = - int(text.length() * 6 / 2);
	IwGxPrintString(x + l, y, text.c_str());
}

#define INT(a) static_cast<int>(a)

void Render::Line(float x1, float y1, float x2, float y2, DWORD color) {
	Iw2DSetColour(color);
	Iw2DDrawLine(CIwSVec2(INT(x1), INT(y1)), CIwSVec2(INT(x2), INT(y2)));
}

void Render::DrawBar(float x, float y, float width, float height, DWORD color) {
	Iw2DSetColour(color);
	Iw2DFillRect(CIwSVec2(INT(x), INT(y)), CIwSVec2(INT(width), INT(height)));
}

bool Render::IsRightMouseButton() {
	return false;
}

bool Render::IsLeftMouseButton() {
	return true;
}

int Render::GetFontHeight(const std::string &fontName) {
	assert(false);
	return 0;
}

int Render::GetStringWidth(const std::string &fontName, const char *s) {
	assert(false);
	return 0;
}

int Render::IniFileGetUnsignedInt(const char *section, const char *variable, unsigned int defaultValue) {
	int i = defaultValue;
	int res = s3eConfigGetInt(section, variable, &i);
	assert(res != S3E_RESULT_ERROR);
	return i;
}

std::string Render::IniFileGetString(const char *section, const char *variable, const char *defaultValue) {
	char str[0x100];
	strcpy(str, defaultValue);
	int res = s3eConfigGetString(section, variable, str);
	assert(res != S3E_RESULT_ERROR);
	return str;
}

void Render::ShowMessage(const char *str, const char *caption) {
	//IwTrace(str);//assert(false);
	//IwTrace(caption);
	IwAssertMsg(MYAPP, caption, ("Error", str));
}

bool Render::ShowAskMessage(const char *str, const char *caption) {
	assert(false);
	return false;
}

CIwMaterial *Render::StartVertexBuffer(PTexture texture) {
	// Allocate a material from the IwGx global cache
    CIwMaterial *pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    // Use Texture on Material
    pMat->SetTexture(texture->s_Texture);
	// Set this as the active material
    IwGxSetMaterial(pMat);
	return pMat;
}

void Render::FinishVertexBuffer(CIwSVec2 *xy, CIwSVec2 *uvs, unsigned int counter) {
	IwGxSetVertStreamScreenSpace(xy, counter * 4);
    IwGxSetUVStream(uvs);
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, counter * 4);
}

#endif // HGE_COMPILE_KEY
