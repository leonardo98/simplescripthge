#include "ColoredPolygon.h"
#include "../Core/Math.h"

ColoredPolygon::ColoredPolygon(TiXmlElement *xe)
: BeautyBase(xe)
{
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
	Render::MatrixRotate(_angle);
	Render::MatrixMove(_sx, _sy);
	for (unsigned int i = 0; i < _dots.size(); ++i) {
		Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
	}
	Render::PopMatrix();
	BeautyBase::Draw();
}

void ColoredPolygon::DebugDraw() {
	Draw();
	BeautyBase::DebugDraw();
}

bool ColoredPolygon::PixelCheck(FPoint2D point) { 
	return Math::Inside(point, _dots) || BeautyBase::PixelCheck(point); 
}

void ColoredPolygon::SaveToXml(TiXmlElement *xe) {
	BeautyBase::SaveToXml(xe);
	for (int j = 0; j < _dots.size(); ++j) {
		TiXmlElement *dot = new TiXmlElement("dot");
		char s[16];
		sprintf(s, "%f", _dots[j].x); 
		dot->SetAttribute("x", s);
		sprintf(s, "%f", _dots[j].y); 
		dot->SetAttribute("y", s);
		xe->LinkEndChild(dot);
	}
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
	int minX = _dots[0].x;
	int maxX = _dots[0].x;
	int minY = _dots[0].y;
	int maxY = _dots[0].y;

	for (int i = 1; i < _dots.size(); ++i) {
		if (minX > _dots[i].x) {
			minX = _dots[i].x;
		} else if (maxX < _dots[i].x) {
			maxX = _dots[i].x;
		}
		if (minY > _dots[i].y) {
			minY = _dots[i].y;
		} else if (maxY < _dots[i].y) {
			maxY = _dots[i].y;
		}
	}

	_width = maxX - minX;
	_height = maxY - minY;
}

bool ColoredPolygon::GenerateTriangles() {
	return false;
}

int ColoredPolygon::SearchNearest(float x, float y) {
	int result = -1;
	static const float SIZEX = 6;
	FPoint2D p(x, y);
	for (unsigned int i = 0; i < _dots.size() && result < 0; ++i) {
		if ((_dots[i] - p).Length() < SIZEX) {
			result = i;
		}
	}
	return result;
}

bool ColoredPolygon::CreateDot(float x, float y) {
	if (_dots.size() >= 20) {
		return false;
	}
	bool result = false;
	static const float SIZEX = 6;
	FPoint2D p(x, y);
	for (unsigned int i = 0; i < _dots.size() && !result; ++i) {
		if (result = Math::DotNearLine(_dots[i], _dots[(i + 1) % _dots.size()], p)) {
			int index = (i + 1) % _dots.size();
			if (index < _dots.size()) {
				_dots.insert(_dots.begin() + index, p);
			} else {
				_dots.push_back(p);
			}
		}
	}
	if (result) {
		GenerateTriangles();
	}
	return result;
}

void ColoredPolygon::RemoveDot(int index) {
	if (_dots.size() <= 3) {
		return;
	}
	_dots.erase(_dots.begin() + index);
}

void ColoredPolygon::MouseDown(FPoint2D mouse) {
}

void ColoredPolygon::MouseMove(FPoint2D mouse) {
}

void ColoredPolygon::MouseUp(FPoint2D mouse) {
}
