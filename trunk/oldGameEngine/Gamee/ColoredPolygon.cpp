#include "ColoredPolygon.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "../Core/Messager.h"

ColoredPolygon::ColoredPolygon(TiXmlElement *xe)
: BeautyBase(xe)
{
	_color = Math::ReadColor(xe->Attribute("color"));
	TiXmlElement *dot = xe->FirstChildElement("dot");
	while (dot != NULL) {
		_dots.push_back(FPoint2D(atof(dot->Attribute("x")), atof(dot->Attribute("y"))));
		dot = dot->NextSiblingElement();
	}
	CalcWidthAndHeight();
	GenerateTriangles();
}

void ColoredPolygon::Draw() {

	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);

	DrawTriangles();

	// этот кусок нужен для определения клика по многоугольнику
	_screenDots = _dots;
	for (unsigned int i = 0; i < _dots.size(); ++i) {
		Render::GetCurrentMatrix().Mul(_screenDots[i]);	
	}

	Render::PopMatrix();
	BeautyBase::Draw();
}

bool ColoredPolygon::PixelCheck(const FPoint2D &point) { 
	if (Math::Inside(point, _screenDots)) {
		return true;
	}
	return false;
}

std::string ColoredPolygon::Type() { 
	return "ColoredPolygon"; 
}

int ColoredPolygon::Width() {
	return _width;
}

int ColoredPolygon::Height() {
	return _height;
}

void ColoredPolygon::CalcWidthAndHeight() {
	hgeRect rect;
	rect.Clear();
	for (int i = 0; i < _dots.size(); ++i) {
		rect.Encapsulate(_dots[i].x, _dots[i].y);
	}
	_width = fabs(rect.x2 - rect.x1);
	_height = fabs(rect.y2 - rect.y1);
}

bool ColoredPolygon::GenerateTriangles() {
	_triangles.clear();
	std::vector<FPoint2D> dots = _dots;

	float sign = 0.f;
	{
		// ищем самый острый угол или наименее тупой
		FPoint2D *a;
		FPoint2D *b;
		FPoint2D *c;
		int index = 0;
		float minAngle = 180.f;
		for (int i = 0; i < dots.size(); ++i) {
			a = &dots[i];
			b = &dots[(i + 1) % dots.size()];
			c = &dots[(i + 2) % dots.size()];
			FPoint2D m((*a + *b + *c) / 3);
			if (Math::Inside(m, dots)) {
				float angle = (*a - *b).Angle(&(*c - *b));
				if (angle <= 0.f) {
					assert(false);
				}
				if (angle < minAngle) {
					minAngle = angle;
					index = i;
				}
			}
		}
		a = &dots[index];
		b = &dots[(index + 1) % dots.size()];
		c = &dots[(index + 2) % dots.size()];
		sign = Math::VMul(*b - *a, *c - *b);
	}
	int counter = 0;
	int dsize = 0;
	while (dots.size() > 0) {
		assert(dots.size() > 2);
		if (dots.size() != dsize) {
			dsize = dots.size();
			counter = 0;
		} else {
			++counter;
			if (counter > 10) {
				_triangles.clear();
				return false;
			}
		}
		if (dots.size() == 3) {
			hgeTriple tri;
			FillTriangle(dots[0], dots[1], dots[2], tri);
			_triangles.push_back(tri);
			dots.clear();
		} else {
			FPoint2D *a;
			FPoint2D *b;
			FPoint2D *c;
			for (int i = 0; i < dots.size(); ++i) {
				a = &dots[i];
				b = &dots[(i + 1) % dots.size()];
				c = &dots[(i + 2) % dots.size()];
				
				bool intersection = false;
				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					FPoint2D *b2 = &dots[(j + 1) % dots.size()];
					intersection = (Math::Intersection(*a, *c, *a2, *b2, NULL) 
									|| Math::Intersection(*a, *b, *a2, *b2, NULL)
									|| Math::Intersection(*b, *c, *a2, *b2, NULL));
				}

				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					intersection = (a2 != a && a2 != b && a2 != c && Math::Inside(*a2, *a, *b, *c));
				}

				if (!intersection && Math::VMul(*b - *a, *c - *b) * sign > 0.f) {// выбираем только те что с нашим знаком
					hgeTriple tri;
					FillTriangle(*a, *b, *c, tri);
					_triangles.push_back(tri);
					if (i < dots.size() - 1) {
						dots.erase(dots.begin() + i + 1);
					} else {
						dots.erase(dots.begin());
					}
					break;
				}
			}
		}		
	}
	return true;
}

void ColoredPolygon::FillTriangle(const FPoint2D &a, const FPoint2D &b, const FPoint2D &c, hgeTriple &tri) {
	tri.v[0].x = a.x;
	tri.v[0].y = a.y;
	tri.v[1].x = b.x;
	tri.v[1].y = b.y;
	tri.v[2].x = c.x;
	tri.v[2].y = c.y;

	tri.blend = BLEND_ALPHABLEND | BLEND_COLORMUL;
	// надо добавить в движке HGE режим без блендинга - можно только в игре,
	// в редакторе не обязательно
	tri.tex = 0;
	for (unsigned int i = 0; i < 3; ++i) {
		tri.v[i].col = _color;
		tri.v[i].z = 0.f;
		tri.v[i].tx = tri.v[i].x / 512.f;
		tri.v[i].ty = tri.v[i].y / 512.f;
	}
}

void ColoredPolygon::DrawTriangles() {
	int n = _triangles.size();// * f;
	const Matrix &m = Render::GetCurrentMatrix();
	int counter = 500;
	hgeVertex *vertexs = Render::GetDC()->Gfx_StartBatch(HGEPRIM_TRIPLES, 0, BLEND_DEFAULT, &counter);
	n = min(n, counter);
	int num = 0;
	for (unsigned int j = 0; j < n; ++j) {
		hgeTriple tri = _triangles[j];
		for (unsigned int i = 0; i < 3; ++i) {
			vertexs[num] = tri.v[i];
			m.Mul(vertexs[num].x, vertexs[num].y);
			++num;
		}
	}
	Render::GetDC()->Gfx_FinishBatch(num / 3);
}

