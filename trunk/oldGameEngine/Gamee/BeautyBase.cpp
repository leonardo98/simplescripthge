#include "BeautyBase.h"
#include "../Core/Render.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

BeautyBase::~BeautyBase() {
	delete center;
	center = NULL;
	for (int i = 0; i < 4; ++i) {
		delete scale[i];
		scale[i] = NULL;
		delete scaleSide[i];
		scaleSide[i] = NULL;
	}
	delete rotate;
	rotate = NULL;
}

BeautyBase::BeautyBase(const BeautyBase &b) {
	_pos = b._pos;
	
	_angle = b._angle;

	_sx = b._sx;
	_sy = b._sy;

	_inGameType = b._inGameType;

	_canBeRotated = b._canBeRotated;
	_canBeScaled = b._canBeScaled;

	_debugDraw = false;

	HTEXTURE h_base = Core::getTexture("data\\red.png")->GetTexture();
	for (int i = 0; i < 4; ++i) {
		scale[i] = new Sprite(h_base, 4, 13, 12, 12);
		scaleSide[i] = new Sprite(h_base, 4, 1, 10, 10);
	}
	rotate = new Sprite(h_base, 1, 44, 20, 20);
	center = new Sprite(h_base, 4, 29, 12, 12);
	_mouseDown = false;
	UpdateScreenRect();
}


BeautyBase::BeautyBase(TiXmlElement *xe) {
	_pos.x = atof(xe->Attribute("x"));
	_pos.y = atof(xe->Attribute("y"));
		
	_angle = Math::Read(xe, "angle", 0.f);

	_sx = Math::Read(xe, "sx", 1.f);
	_sy = Math::Read(xe, "sy", 1.f);

	_canBeRotated = Math::Read(xe, "canBeRotated", false);
	_canBeScaled = Math::Read(xe, "canBeScaled", false);

	_debugDraw = false;

	HTEXTURE h_base = Core::getTexture("data\\red.png")->GetTexture();
	for (int i = 0; i < 4; ++i) {
		scale[i] = new Sprite(h_base, 4, 13, 12, 12);
		scaleSide[i] = new Sprite(h_base, 4, 1, 10, 10);
	}
	rotate = new Sprite(h_base, 1, 44, 20, 20);
	center = new Sprite(h_base, 4, 29, 16, 16);

	const char *inGameType = xe->Attribute("inGameType");
	if (inGameType) {
		_inGameType = inGameType;
	}
	_mouseDown = false;
	UpdateScreenRect();
}

void BeautyBase::MoveTo(float x, float y) { 
	_pos.x = x; 
	_pos.y = y; 
}

void BeautyBase::ShiftTo(float dx, float dy) { 
	_pos.x += dx; 
	_pos.y += dy; 
}

void BeautyBase::SaveToXml(TiXmlElement *xe) {
	char s[16];

	if (_inGameType != "") {
		xe->SetAttribute("inGameType", _inGameType.c_str());
	}

	sprintf(s, "%f", _pos.x); 
	xe->SetAttribute("x", s);
	sprintf(s, "%f", _pos.y); 
	xe->SetAttribute("y", s);

	if (fabs(_angle) > 1e-5) {
		sprintf(s, "%f", _angle); 
		xe->SetAttribute("angle", s);
	}

	if (fabs(_sx - 1.f) > 1e-5) {
		sprintf(s, "%f", _sx); 
		xe->SetAttribute("sx", s);
	}
	if (fabs(_sy - 1.f) > 1e-5) {
		sprintf(s, "%f", _sy); 
		xe->SetAttribute("sy", s);
	}

	if (_canBeRotated) {
		xe->SetAttribute("canBeRotated", "1");
	}
	if (_canBeScaled) {
		xe->SetAttribute("canBeScaled", "1");
	}	
}

void BeautyBase::Draw() {
	_debugDraw = false;
}

void BeautyBase::DebugDraw(bool onlyControl) {
	_debugDraw = true;
	{
		Render::GetDC()->System_SetState(HGE_TEXTUREFILTER, false);
		Matrix matrix = parent = Render::GetCurrentMatrix();
		Render::PushMatrix();
		Render::SetMatrixUnit();
		matrix.Move(_pos.x, _pos.y);
		matrix.Rotate(_angle);
		rotateMatrix = matrix;
		matrix.Scale(_sx, _sy);
		scaleMatrix = matrix;

		float x, y;
		matrix.Mul(0.f, 0.f, x, y);
		{
			x = Math::round(x);
			y = Math::round(y);
		}
		hgeRect r = hgeRect(- Width() / 2, - Height() / 2, Width() / 2, Height() / 2);
		{
			if (r.x1 > r.x2) {
				float t = r.x1;
				r.x1 = r.x2;
				r.x2 = t;
			}
			if (r.y1 > r.y2) {
				float t = r.y1;
				r.y1 = r.y2;
				r.y2 = t;
			}
			float alpha = (Render::GetColor() >> 24) / 255.f;
			Render::SetAlpha(Math::round(0xAF * alpha));
			FPoint2D corners[4];
			{
//				matrix.Move(- Width() / 2, - Height() / 2);
				
				matrix.Mul(r.x1, r.y1, corners[0].x, corners[0].y);
				corners[0].x = Math::round(corners[0].x);
				corners[0].y = Math::round(corners[0].y);

				matrix.Mul(r.x2, r.y1, corners[1].x, corners[1].y);
				corners[1].x = Math::round(corners[1].x);
				corners[1].y = Math::round(corners[1].y);

				matrix.Mul(r.x2, r.y2, corners[2].x, corners[2].y);
				corners[2].x = Math::round(corners[2].x);
				corners[2].y = Math::round(corners[2].y);

				matrix.Mul(r.x1, r.y2, corners[3].x, corners[3].y);
				corners[3].x = Math::round(corners[3].x);
				corners[3].y = Math::round(corners[3].y);

				for (int i = 0; i < 4; ++i) {
					if (_canBeScaled) {
						scale[i]->Render(corners[i].x - scale[i]->Width() / 2.f, corners[i].y - scale[i]->Height() / 2.f);
					} else {
						scaleSide[i]->Render(corners[i].x - scaleSide[i]->Width() / 2.f, corners[i].y - scaleSide[i]->Height() / 2.f);
					}
				}
			}
			if (_canBeScaled) {
				for (int i = 0; i < 4; ++i) {
					float x = (corners[i].x + corners[(i + 1) % 4].x) / 2;
					float y = (corners[i].y + corners[(i + 1) % 4].y) / 2;
					scaleSide[i]->Render(x - scaleSide[i]->Width() / 2.f, y - scaleSide[i]->Height() / 2.f);
				}
			}
			if (_canBeRotated) {
				Render::PushMatrix();
				Render::MatrixMove(x, y);
				
				float distance = Math::Distance(corners[0], corners[1], FPoint2D(x, y));
				FPoint2D pos(0.f, - (distance + 40));

				Matrix m;
				m.MakeRevers(rotateMatrix);
				FPoint2D r(pos);
				m.Mul(r);
				FPoint2D r2(0, 0);
				m.Mul(r2);

				FPoint2D rpos = *pos.Rotate(- M_PI_2 -(r - r2).Angle());
				Render::Line(0, 0, rpos.x, rpos.y, 0x7F000000);
				rotate->Render( Math::round(rpos.x - rotate->Width() / 2.f), Math::round(rpos.y - rotate->Height() / 2.f));
				Render::PopMatrix();
			}
			if (_canBeRotated) {
				center->Render(x - center->Width() / 2.f, y - center->Height() / 2.f);
			}
			Render::SetAlpha(Math::round(0xFF * alpha));
		}
		Render::PopMatrix();
		Render::GetDC()->System_SetState(HGE_TEXTUREFILTER, true);
	}
}

void BeautyBase::MouseDown(const FPoint2D &mousePos) {
	if (!_debugDraw) {
		_mousePos = mousePos;
		return;
	}
	_mouseDown = true;
	_mousePos = _mouseDownPos = mousePos;
	if (_canBeRotated
		&& rotate->HasPixel((int)mousePos.x, (int)mousePos.y)) {
		_state = edit_rotate;
		return;
	} else if (_canBeScaled
		&& (scale[0]->HasPixel((int)mousePos.x, (int)mousePos.y)
		|| scale[1]->HasPixel((int)mousePos.x, (int)mousePos.y)
		|| scale[2]->HasPixel((int)mousePos.x, (int)mousePos.y)
		|| scale[3]->HasPixel((int)mousePos.x, (int)mousePos.y)
		)) {
		_state = edit_scale;
		_scaleX = true;
		_scaleY = true;
		_beforeDragScale = FPoint2D(_sx, _sy);
		return;
	} else if (_canBeScaled
		&& ((_scaleY = scaleSide[0]->HasPixel((int)mousePos.x, (int)mousePos.y))
		|| (_scaleX = scaleSide[1]->HasPixel((int)mousePos.x, (int)mousePos.y))
		|| (_scaleY = scaleSide[2]->HasPixel((int)mousePos.x, (int)mousePos.y))
		|| (_scaleX = scaleSide[3]->HasPixel((int)mousePos.x, (int)mousePos.y))
		)) {
		_state = edit_scale;
		_beforeDragScale = FPoint2D(_sx, _sy);
		return;
	} else {
		_state = edit_move;
		return;
	} 
}

void BeautyBase::MouseMove(const FPoint2D &mousePos) {
	if (!_debugDraw) {
		return;
	}
	if (_state == edit_rotate)
	{
		if ((_mousePos - mousePos).Length() > 1e-3) {

			Matrix reverse;
			reverse.MakeRevers(rotateMatrix);

			FPoint2D start(_mousePos);
			FPoint2D end(mousePos);
			reverse.Mul(start);
			reverse.Mul(end);

			float angleStart = atan2(start.y, start.x);
			float angleEnd = atan2(end.y, end.x);

			float angle = _angle + (angleEnd - angleStart);

			_angle = angle;

		}			

	}
	else if (_state == edit_move)
	{
//		if ((_mousePos - mousePos).Length() > 1e-3 && (_mouseDownPos - mousePos).Length() > 4) {

			Matrix reverse;
			reverse.MakeRevers(parent);

			FPoint2D start(_mousePos);
			FPoint2D end(mousePos);
			reverse.Mul(start);
			reverse.Mul(end);

			_pos.x += (end.x - start.x);
			_pos.y += (end.y - start.y);
//		}

	}
	else if (_state == edit_scale) 
	{
		if ((_mousePos - mousePos).Length() > 1e-3) {

			Matrix reverse;
			reverse.MakeRevers(scaleMatrix);

			FPoint2D start(_mouseDownPos);
			FPoint2D end(mousePos);
			reverse.Mul(start);
			reverse.Mul(end);

			float sx = (end.x / start.x);
			float sy = (end.y / start.y);

			// маштабирование только по высоте - игнорируем изменение ширины
			if (!_scaleX) {
				sx = 1.f;
			}

			// маштабирование только по ширине - игнорируем изменение высоты
			if (!_scaleY) {
				sy = 1.f;
			}

			// маштабирование "синхронно" по обеим осям - вибираем меньший коэффициент из двух
			if (_scaleX && _scaleY && Render::GetDC()->Input_GetKeyState(HGEK_SHIFT)) {
				sx = sy = min(sx, sy);
			}

			_sx = sx * _beforeDragScale.x;
			_sy = sy * _beforeDragScale.y;
			if (Render::GetDC()->Input_GetKeyState(HGEK_CTRL)) {
				_sx = Math::round(_sx * 2) / 2.f;
				_sy = Math::round(_sy * 2) / 2.f;
			}

		}

	} 
	_mousePos = mousePos;
}

void BeautyBase::MouseUp(const FPoint2D &mouse) {
	if (_mouseDown) {
		_mouseDown = false;
		UpdateScreenRect();
	}
	_state = edit_none;
	_scaleX = false;
	_scaleY = false;
}

bool BeautyBase::PixelCheck(const FPoint2D &point) {
	if (_debugDraw) {
		bool result = rotate->HasPixel(point.x, point.y);
		for (int i = 0; i < 4 && !result; ++i) {
			result |= scale[i]->HasPixel(point.x, point.y);
			result |= scaleSide[i]->HasPixel(point.x, point.y);
		}
		return result;
	}
	return false;
}