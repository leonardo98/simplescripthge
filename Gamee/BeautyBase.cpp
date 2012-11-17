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

BeautyBase::BeautyBase(TiXmlElement *xe) {
	_pos.x = atof(xe->Attribute("x"));
	_pos.y = atof(xe->Attribute("y"));
	
	_angle = atof(xe->Attribute("angle"));

	_sx = atof(xe->Attribute("sx"));
	_sy = atof(xe->Attribute("sy"));

	_canBeRotated = atoi(xe->Attribute("canBeRotated")) == 1;
	_canBeScaled = atoi(xe->Attribute("canBeScaled")) == 1;

	_debugDraw = false;

	HTEXTURE h_base = Core::getTexture("data\\red.png")->GetTexture();
	for (int i = 0; i < 4; ++i) {
		scale[i] = new Sprite(h_base, 4, 13, 12, 12);
		scaleSide[i] = new Sprite(h_base, 4, 1, 10, 10);
	}
	rotate = new Sprite(h_base, 1, 44, 20, 20);
	center = new Sprite(h_base, 4, 29, 16, 16);
}

void BeautyBase::MoveTo(float x, float y) { 
	_pos.x = x; 
	_pos.y = y; 
}

void BeautyBase::SaveToXml(TiXmlElement *xe) {
	char s[16];

	sprintf(s, "%f", _pos.x); 
	xe->SetAttribute("x", s);
	sprintf(s, "%f", _pos.y); 
	xe->SetAttribute("y", s);

	sprintf(s, "%f", _angle); 
	xe->SetAttribute("angle", s);

	sprintf(s, "%f", _sx); 
	xe->SetAttribute("sx", s);
	sprintf(s, "%f", _sy); 
	xe->SetAttribute("sy", s);

	xe->SetAttribute("canBeRotated", (_canBeRotated ? "1" : "0"));
	xe->SetAttribute("canBeScaled", (_canBeScaled ? "1" : "0"));
}

void BeautyBase::Draw() {
	_debugDraw = false;
}

void BeautyBase::DebugDraw() {
	_debugDraw = true;
	{
		Matrix matrix = parent = Render::GetCurrentMatrix();
		matrix.Move(_pos.x, _pos.y);
		matrix.Rotate(_angle);
		matrix.Scale(_sx, _sy);

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
			Render::SetAlpha(0xAF);
			FPoint2D corners[4];
			if (_canBeScaled) {
				matrix.Move(- Width() / 2, - Height() / 2);
				
				matrix.Mul(r.x1, r.y1, corners[0].x, corners[0].y);
				corners[0].x = Math::round(corners[0].x);
				corners[0].y = Math::round(corners[0].y);
				scale[0]->Render(corners[0].x - scale[0]->Width() / 2.f, corners[0].y - scale[0]->Height() / 2.f);

				matrix.Mul(r.x2, r.y1, corners[1].x, corners[1].y);
				corners[1].x = Math::round(corners[1].x);
				corners[1].y = Math::round(corners[1].y);
				scale[1]->Render(corners[1].x - scale[1]->Width() / 2.f, corners[1].y - scale[1]->Height() / 2.f);

				matrix.Mul(r.x2, r.y2, corners[2].x, corners[2].y);
				corners[2].x = Math::round(corners[2].x);
				corners[2].y = Math::round(corners[2].y);
				scale[2]->Render(corners[2].x - scale[2]->Width() / 2.f, corners[2].y - scale[2]->Height() / 2.f);

				matrix.Mul(r.x1, r.y2, corners[3].x, corners[3].y);
				corners[3].x = Math::round(corners[3].x);
				corners[3].y = Math::round(corners[3].y);
				scale[3]->Render(corners[3].x - scale[3]->Width() / 2.f, corners[3].y - scale[3]->Height() / 2.f);
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
				m.MakeRevers(matrix);
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
				move->Render(x - move->Width() / 2.f, y - move->Height() / 2.f);
			}
			Render::SetAlpha(0xFF);
		}
	}
}

void BeautyBase::MouseDown(const FPoint2D &mousePos) {
	if (!_debugDraw) {
		return;
	}
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
		_mouseDownPos = mousePos;
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
		_mouseDownPos = mousePos;
		_beforeDragScale = FPoint2D(_sx, _sy);
		return;
	} else if (PixelCheck(mousePos)) {
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
			reverse.MakeRevers(parent);

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
		if ((_mousePos - mousePos).Length() > 1e-3) {

			Matrix reverse;
			reverse.MakeRevers(parent);

			FPoint2D start(_mousePos);
			FPoint2D end(mousePos);
			reverse.Mul(start);
			reverse.Mul(end);

			_pos.x += (end.x - start.x);
			_pos.y += (end.y - start.y);
		}

	}
	else if (_state == edit_scale) 
	{
		if ((_mousePos - mousePos).Length() > 1e-3) {

			Matrix reverse;
			reverse.MakeRevers(parent);

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

		}

	} 
}

void BeautyBase::MouseUp(const FPoint2D &mouse) {
	_state = edit_none;
	_scaleX = false;
	_scaleY = false;
	_mouseDown = false;
}

bool BeautyBase::PixelCheck(const FPoint2D &point) {
	if (_debugDraw) {
		bool result = move->HasPixel(point.x, point.y);
		for (int i = 0; i < 4 && !result; ++i) {
			result |= scale[i]->HasPixel(point.x, point.y);
			result |= scaleSide[i]->HasPixel(point.x, point.y);
		}
		return result;
	}
	return false;
}