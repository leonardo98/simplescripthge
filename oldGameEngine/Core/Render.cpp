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

DWORD Render::_currentColor;
Matrix Render::_matrixStack[MAX_MATRIX_AMOUNT];
unsigned int Render::_currentMatrix;
DWORD Render::_blendMode;

HGE *Render::GetDC() {
	return _hge;
}

Sprite::Sprite(HTEXTURE hTexture, int x, int y, int w, int h) {
	if (w == 0 && h == 0) {
		w = Render::GetDC()->Texture_GetWidth(hTexture, true);
		h = Render::GetDC()->Texture_GetHeight(hTexture, true);
	}
	_lastRender.blend = 0;
	_lastRender.tex = hTexture;
	float width = 1.f;
	float height = 1.f;
	if (hTexture) {
		width = static_cast<float>(Render::GetDC()->Texture_GetWidth(hTexture));
		height = static_cast<float>(Render::GetDC()->Texture_GetHeight(hTexture));
	}
	_lastRender.v[0].tx = x / width; _lastRender.v[0].ty = y / height;
	_lastRender.v[1].tx = (x + w) / width; _lastRender.v[1].ty = y / height;
	_lastRender.v[2].tx = (x + w) / width; _lastRender.v[2].ty = (y + h) / height;
	_lastRender.v[3].tx = x / width; _lastRender.v[3].ty = (y + h) / height;

	_lastRender.v[0].z = 0.f;
	_lastRender.v[1].z = 0.f;
	_lastRender.v[2].z = 0.f;
	_lastRender.v[3].z = 0.f;

	_width = w;
	_height = h;
}

bool Sprite::HasPixel(int x, int y) const// valid for last render only! 
{
	FPoint2D m(x - _lastRender.v[0].x, y - _lastRender.v[0].y);
	FPoint2D a(_lastRender.v[1].x - _lastRender.v[0].x, _lastRender.v[1].y - _lastRender.v[0].y);
	FPoint2D b(_lastRender.v[3].x - _lastRender.v[0].x, _lastRender.v[3].y - _lastRender.v[0].y);

	float k1 = (m.x * b.y - m.y * b.x) / (a.x * b.y - a.y * b.x);
	float k2 = (b.y > 1e-5) ? (m.y - k1 * a.y) / b.y : (m.x - k1 * a.x) / b.x;

	if (k1 < 0 || k1 > 1 || k2 < 0 || k2 > 1) {
		return false;
	}

	float originWidth = _width / (_lastRender.v[1].tx - _lastRender.v[0].tx);
	float originHeight = _height / (_lastRender.v[3].ty - _lastRender.v[0].ty);

	int i = (k1 * (_lastRender.v[1].tx - _lastRender.v[0].tx) + _lastRender.v[0].tx) * originWidth;
	int j = (k2 * (_lastRender.v[3].ty - _lastRender.v[0].ty) + _lastRender.v[0].ty) * originHeight;

	DWORD *dw;
	dw = Render::GetDC()->Texture_Lock(_lastRender.tex, true, i, j, 1, 1);
	bool result = ((*dw) >> 24) > 0x7F;
	Render::GetDC()->Texture_Unlock(_lastRender.tex);
	return result;
}

void Sprite::Render() {
	_lastRender.v[0].col = Render::GetColor();
	_lastRender.v[1].col = Render::GetColor();
	_lastRender.v[2].col = Render::GetColor();
	_lastRender.v[3].col = Render::GetColor();

	_lastRender.blend = Render::GetBlendMode();

	const Matrix &m = Render::GetCurrentMatrix();

	m.Mul(   0.f,     0.f, _lastRender.v[0].x, _lastRender.v[0].y);
	m.Mul(_width,     0.f, _lastRender.v[1].x, _lastRender.v[1].y);
	m.Mul(_width, _height, _lastRender.v[2].x, _lastRender.v[2].y);
	m.Mul(   0.f, _height, _lastRender.v[3].x, _lastRender.v[3].y);
	
	assert(_lastRender.blend >= 0 && _lastRender.blend < 8);
	Render::GetDC()->Gfx_RenderQuad(&_lastRender);
}

void Sprite::Render(float x, float y) {
	_lastRender.v[0].col = Render::GetColor();
	_lastRender.v[1].col = Render::GetColor();
	_lastRender.v[2].col = Render::GetColor();
	_lastRender.v[3].col = Render::GetColor();

	_lastRender.blend = Render::GetBlendMode();

	const Matrix &m = Render::GetCurrentMatrix();

	m.Mul(         x,           y, _lastRender.v[0].x, _lastRender.v[0].y);
	m.Mul(x + _width,           y, _lastRender.v[1].x, _lastRender.v[1].y);
	m.Mul(x + _width, y + _height, _lastRender.v[2].x, _lastRender.v[2].y);
	m.Mul(         x, y + _height, _lastRender.v[3].x, _lastRender.v[3].y);
	
	assert(_lastRender.blend >= 0 && _lastRender.blend < 8);
	Render::GetDC()->Gfx_RenderQuad(&_lastRender);
}

void Sprite::Render(const FPoint2D &pos) {
	Render(pos.x, pos.y);
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
, _textureHolder(false)
{
	_hTexture = hTexture;
	{
		float w = static_cast<float>(Render::GetDC()->Texture_GetWidth(hTexture));
		float h = static_cast<float>(Render::GetDC()->Texture_GetHeight(hTexture));

		_originQuad.tex = hTexture;
		_originQuad.blend = BLEND_DEFAULT;

		_originQuad.v[0].tx = x / w; _originQuad.v[0].ty = y / h;
		_originQuad.v[1].tx = (x + _width) / w; _originQuad.v[1].ty = y / h;
		_originQuad.v[2].tx = (x + _width) / w; _originQuad.v[2].ty = (y + _height) / h;
		_originQuad.v[3].tx = x / w; _originQuad.v[3].ty = (y + _height) / h;

		_screenVertex[0].x = offsetX; _screenVertex[0].y = offsetY;
		_screenVertex[1].x = offsetX + _width; _screenVertex[1].y = offsetY;
		_screenVertex[2].x = offsetX + _width; _screenVertex[2].y = offsetY + _height;
		_screenVertex[3].x = offsetX; _screenVertex[3].y = offsetY + _height;

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
: _textureHolder(true)
{
	_top = 0;
	_left = 0;
	_hTexture = Render::GetDC()->Texture_Load(Render::GetDC()->Resource_MakePath(fileName[1] == ':' ? fileName.c_str() : (Render::GetDataDir() + fileName).c_str()));
	if (_hTexture == NULL) {
		LOG("�� ���� ������� ���� " + fileName);
		exit(-7);
	}

	float w = _frameWidth = _width = static_cast<float>(Render::GetDC()->Texture_GetWidth(_hTexture));
	float h = _frameHeight = _height = static_cast<float>(Render::GetDC()->Texture_GetHeight(_hTexture));
	
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
	if (_textureHolder) {
		Render::GetDC()->Texture_Free(_hTexture);
	}
}

bool Texture::IsNotTransparent(int x, int y) const {
	if (x < 0 || y < 0 || x >= Width() || y >= Height()) {
		return false;
	}
	DWORD *dw;
	dw = Render::GetDC()->Texture_Lock(_hTexture, true, x, y, 1, 1);
	bool result = ((*dw) >> 24) > 0x7F;
	Render::GetDC()->Texture_Unlock(_hTexture);
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
	
	assert(_originQuad.blend >= 0 && _originQuad.blend < 8);
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
	
	assert(_originQuad.blend >= 0 && _originQuad.blend < 8);
	Render::GetDC()->Gfx_RenderQuad(&_originQuad);

}

HTEXTURE Texture::GetTexture() const {
	return _hTexture;
}

void Texture::SetColor(DWORD color) {
	_originQuad.v[0].col = color;
	_originQuad.v[1].col = color;
	_originQuad.v[2].col = color;
	_originQuad.v[3].col = color;
}

StaticSprite::StaticSprite() 
: _origin(NULL)
{
}

void StaticSprite::Set(const Texture *texture, float x, float y) {
	_origin = texture;
	if (_origin == NULL) {
		return;
	}
	_originWidth = static_cast<float>(Render::GetDC()->Texture_GetWidth(_origin->_originQuad.tex));
	_originHeight = static_cast<float>(Render::GetDC()->Texture_GetHeight(_origin->_originQuad.tex));
	_matrix.Unit();
	_matrix.Move(x, y);
}

void StaticSprite::Render() {
	assert(_origin != NULL);
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

	assert(_quad.blend >= 0 && _quad.blend < 8);
	Render::GetDC()->Gfx_RenderQuad(&_quad);
}

void StaticSprite::SetTransform(const Matrix &transform) {
	assert(_origin != NULL);
	_matrix = transform;
}

void StaticSprite::PushTransform(const Matrix &transform) {
	assert(_origin != NULL);
	Matrix m(transform);
	m.Mul(_matrix);
	_matrix = m;
}

bool StaticSprite::PixelCheck(const FPoint2D &pos) {
	assert(_origin != NULL);

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
	assert(_origin != NULL);
	return _origin->Width();
}

int StaticSprite::SpriteHeight() {
	assert(_origin != NULL);
	return _origin->Height();
}

hgeSprite * StaticSprite::GetHGESprite() {
	assert(_origin != NULL);
	return new hgeSprite(_origin->GetTexture(), _origin->_left, _origin->_top, _origin->_width, _origin->_height);
}


void Render::IniFile(const std::string &fileName) {
	_hge->System_SetState(HGE_INIFILE, fileName.c_str());
}

void Render::PrintString(float x, float y, const std::string &text, int align) {
	std::string fontName("data\\font2.fnt");
	fontName = GetDataDir() + fontName;
	hgeFont *font;
	if (_fonts.find(fontName) == _fonts.end()) {
		_fonts[fontName] = new hgeFont(fontName.c_str());
	}
	font = _fonts[fontName];
	font->Render(x, y, align, text.c_str());
}

void Render::PrintString(float x, float y, std::string fontName, const std::string &text, DWORD color) {
	if (fontName == "") {
		fontName = "data\\font2.fnt";
	}
	if (fontName[1] != ':') {
		fontName = GetDataDir() + fontName;
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

void Render::SetDataDir(const std::string &dataDir) {
	_dataDir = dataDir;
}

bool Render::InitApplication(hgeCallback frameFunc, hgeCallback renderFunc) {
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
	assert(quad.blend >= 0 && quad.blend < 8);
	GetDC()->Gfx_RenderQuad(&quad);
}

void Render::Line(float x1, float y1, float x2, float y2, DWORD color) {
	GetCurrentMatrix().Mul(x1, y1, x1, y1);
	GetCurrentMatrix().Mul(x2, y2, x2, y2);
	GetDC()->Gfx_RenderLine(x1, y1, x2, y2, color);
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

void Render::MatrixMul(const Matrix &matrix) {
	_matrixStack[_currentMatrix].Mul(matrix);
}

void Render::SetMatrixUnit() {
	_matrixStack[_currentMatrix].Unit();
}

void Render::SetMatrix(const Matrix &matrix) {
	_matrixStack[_currentMatrix] = matrix;
}

const Matrix & Render::GetCurrentMatrix() {
	return _matrixStack[_currentMatrix];
}

void Render::Draw(hgeTriple &triple) {
	Matrix &m = Render::_matrixStack[Render::_currentMatrix];
	m.Mul(triple.v[0].x, triple.v[0].y);
	m.Mul(triple.v[1].x, triple.v[1].y);
	m.Mul(triple.v[2].x, triple.v[2].y);
	Render::GetDC()->Gfx_RenderTriple(&triple);
}

void Render::Circle(float x, float y, float r, DWORD color) {
	Render::Line(x, y, x + r, y, color);
	float angle = 0.f;
	float xs = sin(angle) * r;
	float ys = cos(angle) * r;
	while (angle < M_PI * 2) {
		angle += M_PI / 10.f;
		float xe = sin(angle) * r;
		float ye = cos(angle) * r;
		Render::Line(x + xs, y + ys, x + xe, y + ye, color);
		xs = xe;
		ys = ye;
	}
}
