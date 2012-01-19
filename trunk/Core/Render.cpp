// device specified function

#include "Render.h"

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

HGE *Render::_hge = NULL;
std::map<std::string, hgeFont*> Render::_fonts;
std::map<HTEXTURE, int> Render::_storageTextures;

DWORD Render::_currentColor;
Matrix Render::_matrixStack[MAX_MATRIX_AMOUNT];
unsigned int Render::_currentMatrix;
DWORD Render::_blendMode;

HGE *Render::GetDC() {
	return _hge;
}

int Texture::Width() const {
	return _frameWidth;
}

int Texture::Height() const {
	return _frameHeight;
}

Texture::Texture(HTEXTURE hTexture, int x, int y, int w, int h, int offsetX, int offsetY, int frameWidth, int frameHeight)
: _top(y)
, _left(x)
, _width(w)
, _height(h)
, _frameWidth(frameWidth)
, _frameHeight(frameHeight)
{
	_offsetX = offsetX;
	_offsetY = offsetY;
	_hTexture = hTexture;
	_texture = new hgeSprite(_hTexture, x, y, _width, _height);
	if (Render::_storageTextures.find(_hTexture) == Render::_storageTextures.end()) {
		Render::_storageTextures[_hTexture] = 1;
	} else {
		Render::_storageTextures[_hTexture]++;
	}

	{
		float w = Render::GetDC()->Texture_GetWidth(_hTexture);
		float h = Render::GetDC()->Texture_GetHeight(_hTexture);

		_originQuad.tex = _hTexture;
		_originQuad.blend = BLEND_DEFAULT;

		_originQuad.v[0].tx = x / w; _originQuad.v[0].ty = y / h;
		_originQuad.v[1].tx = (x + _width) / w; _originQuad.v[1].ty = y / h;
		_originQuad.v[2].tx = (x + _width) / w; _originQuad.v[2].ty = (y + _height) / h;
		_originQuad.v[3].tx = x / w; _originQuad.v[3].ty = (y + _height) / h;

		_screenVertex[0].x = _offsetX; _screenVertex[0].y = _offsetY;
		_screenVertex[1].x = _offsetX + _width; _screenVertex[1].y = _offsetY;
		_screenVertex[2].x = _offsetX + _width; _screenVertex[2].y = _offsetY + _height;
		_screenVertex[3].x = _offsetX; _screenVertex[3].y = _offsetY + _height;

		for (int i = 0; i < 4; ++i) {
			_originQuad.v[i].col = 0xFFFFFFFF;

			_originQuad.v[i].x = _screenVertex[i].x;
			_originQuad.v[i].y = _screenVertex[i].y;
			_originQuad.v[i].z = 1.f;
		}
	}
	_forCopyQuad = _originQuad;
}

Texture::Texture(const std::string &fileName) 
{
	_top = 0;
	_left = 0;
	_offsetX = 0.f;
	_offsetY = 0.f;
	_hTexture = Render::GetDC()->Texture_Load(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + fileName).c_str()));
	if (Render::_storageTextures.find(_hTexture) == Render::_storageTextures.end()) {
		Render::_storageTextures[_hTexture] = 1;
	}
	if (_hTexture == NULL) {
		LOG("Не могу открыть файл " + fileName);
		exit(-7);
	}

	float w = _frameWidth = _width = Render::GetDC()->Texture_GetWidth(_hTexture);
	float h = _frameHeight = _height = Render::GetDC()->Texture_GetHeight(_hTexture);
	
	_texture = new hgeSprite(_hTexture, 0, 0, w, h);

	_originQuad.tex = _hTexture;
	_originQuad.blend = BLEND_DEFAULT;

	_originQuad.v[0].tx = 0.f; _originQuad.v[0].ty = 0.f;
	_originQuad.v[1].tx = 1.f; _originQuad.v[1].ty = 0.f;
	_originQuad.v[2].tx = 1.f; _originQuad.v[2].ty = 1.f;
	_originQuad.v[3].tx = 0.f; _originQuad.v[3].ty = 1.f;

	_screenVertex[0].x = 0.f; _screenVertex[0].y = 0.f;
	_screenVertex[1].x = w; _screenVertex[1].y = 0.f;
	_screenVertex[2].x = w; _screenVertex[2].y = h;
	_screenVertex[3].x = 0.f; _screenVertex[3].y = h;

	for (int i = 0; i < 4; ++i) {
		_originQuad.v[i].col = 0xFFFFFFFF;

		_originQuad.v[i].x = _screenVertex[i].x;
		_originQuad.v[i].y = _screenVertex[i].y;
		_originQuad.v[i].z = 1.f;
	}
	_forCopyQuad = _originQuad;
}

Texture::~Texture() {
	delete _texture;
	Render::_storageTextures[_hTexture]--;
	if (Render::_storageTextures[_hTexture] == 0) {
		Render::GetDC()->Texture_Free(_hTexture);
	}
}

bool Texture::IsNotTransparent(int x, int y) const {
	HTEXTURE h;
	h = _texture->GetTexture();
	DWORD *dw;
	dw = Render::GetDC()->Texture_Lock(h, true, x, y, 1, 1);
	bool result = ((*dw) >> 24) > 0x7F;
	Render::GetDC()->Texture_Unlock(h);
	return result;
}

void Texture::Render(float x, float y) {    
	_originQuad.v[0].col = Render::_currentColor;
	_originQuad.v[1].col = Render::_currentColor;
	_originQuad.v[2].col = Render::_currentColor;
	_originQuad.v[3].col = Render::_currentColor;

	_originQuad.blend = Render::_blendMode;

	Matrix m(Render::_matrixStack[Render::_currentMatrix]);
	m.Move(x, y);

	m.Mul(_screenVertex[0].x, _screenVertex[0].y, _originQuad.v[0].x, _originQuad.v[0].y);
	m.Mul(_screenVertex[1].x, _screenVertex[1].y, _originQuad.v[1].x, _originQuad.v[1].y);
	m.Mul(_screenVertex[2].x, _screenVertex[2].y, _originQuad.v[2].x, _originQuad.v[2].y);
	m.Mul(_screenVertex[3].x, _screenVertex[3].y, _originQuad.v[3].x, _originQuad.v[3].y);
	
	Render::GetDC()->Gfx_RenderQuad(&_originQuad);
}

void Texture::Render(const FPoint2D &pos) {
	Render(pos.x, pos.y);
}

void Texture::Render(const Matrix &transform) {

	_originQuad.v[0].col = Render::_currentColor;
	_originQuad.v[1].col = Render::_currentColor;
	_originQuad.v[2].col = Render::_currentColor;
	_originQuad.v[3].col = Render::_currentColor;

	_originQuad.blend = Render::_blendMode;

	Matrix m(Render::_matrixStack[Render::_currentMatrix]);
	m.Mul(transform);

	m.Mul(_screenVertex[0].x, _screenVertex[0].y, _originQuad.v[0].x, _originQuad.v[0].y);
	m.Mul(_screenVertex[1].x, _screenVertex[1].y, _originQuad.v[1].x, _originQuad.v[1].y);
	m.Mul(_screenVertex[2].x, _screenVertex[2].y, _originQuad.v[2].x, _originQuad.v[2].y);
	m.Mul(_screenVertex[3].x, _screenVertex[3].y, _originQuad.v[3].x, _originQuad.v[3].y);
	
	Render::GetDC()->Gfx_RenderQuad(&_originQuad);

}

HTEXTURE Texture::GetTexture() const {
	return _texture->GetTexture();
}

void Texture::SetColor(DWORD color) {
	_originQuad.v[0].col = color;
	_originQuad.v[1].col = color;
	_originQuad.v[2].col = color;
	_originQuad.v[3].col = color;
}

void StaticSprite::Set(const Texture *texture, float x, float y) {
	_origin = texture;
	if (_origin == NULL) {
		return;
	}
	_originWidth = Render::GetDC()->Texture_GetWidth(_origin->_originQuad.tex);
	_originHeight = Render::GetDC()->Texture_GetHeight(_origin->_originQuad.tex);
	_matrix.Unit();
	_matrix.Move(x, y);
}

void StaticSprite::Render() {
	_quad = _origin->_forCopyQuad;

	_quad.v[0].col = Render::_currentColor;
	_quad.v[1].col = Render::_currentColor;
	_quad.v[2].col = Render::_currentColor;
	_quad.v[3].col = Render::_currentColor;
	_quad.blend = Render::_blendMode;

	Matrix m(Render::_matrixStack[Render::_currentMatrix]);
	m.Mul(_matrix);

	m.Mul(_quad.v[0].x, _quad.v[0].y);
	m.Mul(_quad.v[1].x, _quad.v[1].y);
	m.Mul(_quad.v[2].x, _quad.v[2].y);
	m.Mul(_quad.v[3].x, _quad.v[3].y);

	Render::GetDC()->Gfx_RenderQuad(&_quad);
}

void StaticSprite::SetTransform(const Matrix &transform) {
	_matrix = transform;
}

void StaticSprite::PushTransform(const Matrix &transform) {
	Matrix m(transform);
	m.Mul(_matrix);
	_matrix = m;
}

bool StaticSprite::PixelCheck(const FPoint2D &pos) {
	FPoint2D m(pos.x - _quad.v[0].x, pos.y - _quad.v[0].y);
	FPoint2D a(_quad.v[1].x - _quad.v[0].x, _quad.v[1].y - _quad.v[0].y);
	FPoint2D b(_quad.v[3].x - _quad.v[0].x, _quad.v[3].y - _quad.v[0].y);

	float k1 = (m.x * b.y - m.y * b.x) / (a.x * b.y - a.y * b.x);
	float k2 = (b.y > 1e-5) ? (m.y - k1 * a.y) / b.y : (m.x - k1 * a.x) / b.x;

	if (k1 < 0 || k1 > 1 || k2 < 0 || k2 > 1) {
		return false;
	}
	int i = (k1 * (_quad.v[1].tx - _quad.v[0].tx) + _quad.v[0].tx) * _originWidth;
	int j = (k2 * (_quad.v[3].ty - _quad.v[0].ty) + _quad.v[0].ty) * _originHeight;
	return _origin->IsNotTransparent(i, j);
}

int StaticSprite::SpriteWidth() {
	return _origin->Width();
}

int StaticSprite::SpriteHeight() {
	return _origin->Height();
}

void Render::IniFile(const std::string &fileName) {
	_hge->System_SetState(HGE_INIFILE, fileName.c_str());
}

int Render::IniFileGetUnsignedInt(const char *section, const char *variable, unsigned int defaultValue) {
	std::string value = _hge->Ini_GetString(section, variable, "none");
	if (value != "none") {
		return Parse(value);
	}
	return defaultValue;
}

std::string Render::IniFileGetString(const char *section, const char *variable, const char *defaultValue) {
	return _hge->Ini_GetString(section, variable, defaultValue);
}

void Render::PrintString(int x, int y, const std::string &text, int align) {
	std::string fontName("data\\font2.fnt");
	fontName = GetDataDir() + fontName;
	hgeFont *font;
	if (_fonts.find(fontName) == _fonts.end()) {
		_fonts[fontName] = new hgeFont(fontName.c_str());
	}
	font = _fonts[fontName];
	font->Render(x, y, align, text.c_str());
}

void Render::PrintString(int x, int y, std::string fontName, const std::string &text, DWORD color) {
	if (fontName == "") {
		fontName = "data\\font2.fnt";
	}
	fontName = GetDataDir() + fontName;
	hgeFont *font;
	if (_fonts.find(fontName) == _fonts.end()) {
		_fonts[fontName] = new hgeFont(fontName.c_str());
	}
	font = _fonts[fontName];
	font->SetColor(color);
	font->Render(x, y, HGETEXT_CENTER, text.c_str());
}

int Render::GetFontHeight(const std::string &fontName) {
	std::string fn = GetDataDir() + fontName;
	if (_fonts.find(fn) == _fonts.end()) {
		_fonts[fn] = new hgeFont(fn.c_str());
	}
	hgeFont *font = _fonts[fn];
	return font->GetHeight();
}

int Render::GetStringWidth(const std::string &fontName, const char *s) {
	if (_fonts.find(fontName) == _fonts.end()) {
		_fonts[fontName] = new hgeFont(fontName.c_str());
	}
	hgeFont *font = _fonts[fontName];
	return font->GetStringWidth(s);
}

bool Render::InitApplication(hgeCallback frameFunc, hgeCallback renderFunc, const std::string &dataDir) {
	_dataDir = dataDir;
	_hge = hgeCreate(HGE_VERSION);
	_hge->System_SetState(HGE_INIFILE, (GetDataDir() + "settings.ini").c_str());
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
	_currentColor = 0xFFFFFFFF;
	_blendMode = BLEND_DEFAULT;
	_currentMatrix = 0;
	SetMatrixUnit();
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
//	MessageBox(GetDC()->System_GetState(HGE_HWND), str, caption, MB_OK | MB_APPLMODAL);
}

bool Render::ShowAskMessage(const char *str, const char *caption) {
//	return (MessageBox(GetDC()->System_GetState(HGE_HWND), "Delete selected object?", "Are you sure?", MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES);
    return false;
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

void Render::SetColor(DWORD color) {
	_currentColor = color;
}

void Render::SetAlpha(DWORD alpha) {
	_currentColor = (alpha << 24) | (_currentColor & 0xFFFFFF);
}

std::string Render::_dataDir;

std::string Render::GetDataDir() {
	if (_dataDir.size()) {
		return _dataDir + "\\";
	}
	return "";
}

void Render::PushMatrix() {
	assert((_currentMatrix + 1)< MAX_MATRIX_AMOUNT);
	++_currentMatrix;
	_matrixStack[_currentMatrix] = _matrixStack[_currentMatrix - 1];
}

void Render::PopMatrix() {
	assert(_currentMatrix > 0);
	--_currentMatrix;
}

void Render::MatrixMove(float x, float y) {
	_matrixStack[_currentMatrix].Move(x, y);
}

void Render::MatrixRotate(float angle) {
	_matrixStack[_currentMatrix].Rotate(angle);
}

void Render::MatrixScale(float sx, float sy) {
	_matrixStack[_currentMatrix].Scale(sx, sy);
}

void Render::SetMatrixUnit() {
	_matrixStack[_currentMatrix].Unit();
}

void Render::SetMatrix(const Matrix &matrix) {
	_matrixStack[_currentMatrix] = matrix;
}

void Render::SetBlendMode(DWORD mode) {
	_blendMode = mode;
}

void Render::Draw(hgeTriple &triple) {
	Matrix &m = Render::_matrixStack[Render::_currentMatrix];
	m.Mul(triple.v[0].x, triple.v[0].y);
	m.Mul(triple.v[1].x, triple.v[1].y);
	m.Mul(triple.v[2].x, triple.v[2].y);
	Render::GetDC()->Gfx_RenderTriple(&triple);
}
