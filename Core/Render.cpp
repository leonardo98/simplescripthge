// device specified function

#include "Render.h"

HGE *Render::_hge = NULL;
std::map<std::string, hgeFont*> Render::_fonts;

HGE *Render::GetDC() {
	return _hge;
}

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

int Render::IniFileGetUnsignedInt(const std::string &section, const std::string &variable, unsigned int defaultValue) {
	char buff[10];
	sprintf(buff, "%d", defaultValue);
	return Parse(_hge->Ini_GetString(section.c_str(), variable.c_str(), buff));
}

std::string Render::IniFileGetString(const std::string &section, const std::string &variable, const std::string &defaultValue) {
	return _hge->Ini_GetString(section.c_str(), variable.c_str(), defaultValue.c_str());
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

void Render::InitApplication(hgeCallback frameFunc, hgeCallback renderFunc) {
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
