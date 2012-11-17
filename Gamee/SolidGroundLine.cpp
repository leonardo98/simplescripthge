#include "SolidGroundLine.h"
#include "../Core/Math.h"
#include "../Core/Core.h"

void SolidGroundLine::DrawLines() {	
	assert(xPoses.size() != 1);
	for (unsigned int i = 0; i < xPoses.size() - 1; ++i) {
		float minx = xPoses[i];
		float maxx = xPoses[i + 1];
		for (unsigned int k = 0; k < 6; ++k) {
			float x1 = minx + (maxx - minx) * k / 6.f;
			float x2 = minx + (maxx - minx) * (k + 1) / 6.f;
			float y1 = yPoses.getFrame(i, k / 6.f);
			float y2 = yPoses.getFrame(i, (k + 1) / 6.f);
			Render::Line(x1, y1, x2, y2, 0xFFFF0000);
		}
	}
}

void SolidGroundLine::UpdateScreenRect() {	
	_screenRect.Clear();
	for (unsigned int i = 0; i < xPoses.size() - 1; ++i) {
		float minx = xPoses[i];
		float maxx = xPoses[i + 1];
		for (unsigned int k = 0; k < 6; ++k) {
			float x1 = minx + (maxx - minx) * k / 6.f;
			float y1 = yPoses.getFrame(i, k / 6.f);
			_screenRect.Encapsulate(x1, y1);
		}
	}
	_screenRect.Encapsulate(GetFinish().x, GetFinish().y);
}

void SolidGroundLine::DebugDrawLines() {
	
	parent = Render::GetCurrentMatrix();

	DrawLines();

	float sx, sy;
	parent.GetScale(sx, sy);

	Render::GetDC()->System_SetState(HGE_TEXTUREFILTER, false);
	for (unsigned int i = 0; i < xPoses.size(); ++i) {
		float x = xPoses[i];
		float y = yPoses.keys[i].first;
		Render::PushMatrix();
		Render::MatrixMove(x, y);
		Render::MatrixScale(1.f / sx, 1.f / sy);
		if (_underMouse == i) {
			_sprite->Render(- _sprite->Width() / 2,  - _sprite->Height() / 2);
		} else {
			_small->Render( - _small->Width() / 2, - _small->Height() / 2);
		}
		Render::PopMatrix();
	}
	Render::GetDC()->System_SetState(HGE_TEXTUREFILTER, true);

}

void SolidGroundLine::AddPoint(float x, float y) {
	yPoses.addKey(y);
	yPoses.CalculateGradient(false);
	xPoses.push_back(x);
}

int SolidGroundLine::SearchNearest(float x, float y) {
	FPoint2D fp(x, y);
	int result = -1;
	static const float SIZEX = 6;
	for (unsigned int i = 0; i < xPoses.size() && result < 0; ++i) {
		float px = xPoses[i];
		float py = yPoses.keys[i].first;
		if ((FPoint2D(fp.x, fp.y) - FPoint2D(px, py)).Length() < SIZEX) {
			result = i;
		}
	}
	return result;
}

bool SolidGroundLine::SearchProection(FPoint2D &pos) {
	assert(false);
	//float t = 0.f;
	//float x1 = xPoses.front();
	//float y1 = yPoses.getGlobalFrame(0.f);
	//float x2, y2;
	//int subLine = xPoses.size() * 6;//количество прямых кусочков из которых рисуется кривая сплайна
	//FPoint2D gravity(0.f, 10.f);
	//float speed = 50.f;
	////FPoint2D motor();
	//while (t + 1.f / subLine < 1.f) {
	//	t += 1.f / subLine;//количество прямых кусочков из которых рисуется кривая сплайна
	//	x2 = xPoses.getGlobalFrame(t);
	//	int index = t * (yPoses.size() - 1);
	//	float f = (t - index * subLine) / subLine;
	//	y2 = (yPoses[index + 1] - yPoses[index]) * f + yPoses[index];
	//	if (x1 <= pos.x && pos.x < x2) {
	//		// calc pos
	//	//Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);
	//		return true;
	//	}
	//	x1 = x2;
	//	y1 = y2;
	//}
	//x2 = xPoses.getGlobalFrame(1.f);
	//y2 = yPoses.back();
	//if (x1 <= pos.x && pos.x < x2) {
	////	Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);			
	//	return true;
	//}
	return false;
}

bool SolidGroundLine::CreateDot(float x, float y) {
	if (xPoses.size() >= 20) {
		return false;
	}
	bool result = false;
	static const float SIZEX = 6;
	
	FPoint2D fp(x, y);

	FPoint2D p(x, y);
	FPoint2D one;
	FPoint2D two;
	for (unsigned int i = 0; i < (xPoses.size() - 1) && !result; ++i) {
		float minx = xPoses[i];
		float maxx = xPoses[i + 1];
		for (unsigned int k = 0; k < 6 && !result; ++k) {
			one.x = minx + (maxx - minx) * k / 6.f;
			two.x = minx + (maxx - minx) * (k + 1) / 6.f;
			one.y = yPoses.getFrame(i, k / 6.f);
			two.y = yPoses.getFrame(i, (k + 1) / 6.f);
			if (result = Math::DotNearLine(one, two, p)) {
				int index = i + 1;
				if (index < xPoses.size()) {
					std::vector<float> splineX = xPoses;
					SplinePath splineY = yPoses;
					xPoses.clear();
					yPoses.Clear();
					for (int i = 0; i < index; ++i) {
						xPoses.push_back(splineX[i]);
						yPoses.addKey(splineY.keys[i].first);
					}
					xPoses.push_back(x);
					yPoses.addKey(y);
					for (int i = index; i < splineX.size(); ++i) {
						xPoses.push_back(splineX[i]);
						yPoses.addKey(splineY.keys[i].first);
					}
					yPoses.CalculateGradient(false);
				} else {
					xPoses.push_back(x);
					yPoses.addKey(y);
					yPoses.CalculateGradient(false);
				}
			}
		}
	}
	return result;
}

void SolidGroundLine::RemoveDot(int index) {
	if (xPoses.size() <= 2) {
		return;
	}
	std::vector<float> splineX = xPoses;
	SplinePath splineY = yPoses;
	xPoses.clear();
	yPoses.Clear();
	for (int i = 0; i < index; ++i) {
		xPoses.push_back(splineX[i]);
		yPoses.addKey(splineY.keys[i].first);
	}
	for (int i = index + 1; i < splineX.size(); ++i) {
		xPoses.push_back(splineX[i]);
		yPoses.addKey(splineY.keys[i].first);
	}
	yPoses.CalculateGradient(false);
	_underMouse = -1;
}

void SolidGroundLine::ExportToLines(std::vector<FPoint2D> &lineDots) {
	lineDots.clear();
	for (unsigned int i = 0; i < xPoses.size() - 1; ++i) {
		float minx = xPoses[i];
		float maxx = xPoses[i + 1];
		for (unsigned int k = 0; k < 6; ++k) {
			float x = minx + (maxx - minx) * k / 6.f;
			float y = yPoses.getFrame(i, k / 6.f);
			lineDots.push_back(FPoint2D(x, y));
		}
	}
	float x = xPoses.back();
	float y = yPoses.getGlobalFrame(1.f);
	lineDots.push_back(FPoint2D(x, y));
}

Lines * SolidGroundLine::CreateBody(Byker *byker, int splineIndex) {
	DotsList dots;
	ExportToLines(dots);
	for (unsigned int i = 0; i < dots.size(); ++i) {
		dots[i] *= (1.f / SCALE_BOX2D);
	}
	return byker->physic.AddLinesSet(dots, splineIndex);
}

std::string SolidGroundLine::Type() {
	return "SolidGroundLine";
}

void SolidGroundLine::SaveToXml(TiXmlElement *xe) {
	for (int j = 0; j < xPoses.size(); ++j) {
		TiXmlElement *dot = new TiXmlElement("dot");
		char s[16];
		sprintf(s, "%f", xPoses[j]); 
		dot->SetAttribute("x", s);
		sprintf(s, "%f", yPoses.keys[j].first); 
		dot->SetAttribute("y", s);
		xe->LinkEndChild(dot);
	}
	HTEXTURE h_base = Core::getTexture("data\\red.png")->GetTexture();
	_sprite = new Sprite(h_base, 4, 13, 12, 12);
	_small = new Sprite(h_base, 4, 1, 10, 10);
}

SolidGroundLine::SolidGroundLine() {
	_mouseDown = false;
	HTEXTURE h_base = Core::getTexture("data\\red.png")->GetTexture();
	_sprite = new Sprite(h_base, 4, 13, 12, 12);
	_small = new Sprite(h_base, 4, 1, 10, 10);
}

SolidGroundLine::~SolidGroundLine() {
	delete _sprite;
	delete _small;
	_sprite = NULL;
	_small = NULL;
}

SolidGroundLine::SolidGroundLine(TiXmlElement* xe) {
	std::vector<float> x;
	SplinePath y;
	TiXmlElement *dot = xe->FirstChildElement("dot");
	while (dot != NULL) {
		x.push_back(atof(dot->Attribute("x")));
		y.addKey(atof(dot->Attribute("y")));
		dot = dot->NextSiblingElement();
	}
	y.CalculateGradient(false);
	
	xPoses = x;
	yPoses = y;
}

void SolidGroundLine::ShiftDot(int index, float dx, float dy, bool moveAllDots) {
	std::vector<float> splineX = xPoses;
	SplinePath splineY = yPoses;

	std::vector<float> tmpx;
	SplinePath tmpy;
	for (unsigned int i = 0; i < xPoses.size(); ++i) {
		if (i == index || moveAllDots) {
			tmpx.push_back(splineX[i] + dx);
			tmpy.addKey(splineY.keys[i].first + dy);
		} else {
			tmpx.push_back(splineX[i]);
			tmpy.addKey(splineY.keys[i].first);
		}
	}
	tmpy.CalculateGradient(false);

	xPoses = tmpx;
	yPoses = tmpy;
}


void SolidGroundLine::MouseDown(const FPoint2D &mousePos) {
	_mouseDown = true;
	_lastMousePos = mousePos;

	Matrix m;
	m.MakeRevers(parent);
	FPoint2D fp(mousePos);
	m.Mul(fp);

	if (Render::GetDC()->Input_GetKeyState(HGEK_CTRL)) {
		CreateDot(fp.x, fp.y);
	} else if (Render::GetDC()->Input_GetKeyState(HGEK_D)) {
		int index = SearchNearest(fp.x, fp.y);
		if (index >= 0) {
			RemoveDot(index);
		}
	} else {
		_dotIndex = SearchNearest(fp.x, fp.y);
		_moveAllDots = _dotIndex < 0;
		_splineX = xPoses;
		_splineY = yPoses;
		_downWorldPos = fp;
	}
}

void SolidGroundLine::MouseMove(const FPoint2D &mousePos) {

	if (_mouseDown) {
		Matrix m;
		m.MakeRevers(parent);
		FPoint2D newMmouseWorld(mousePos);
		m.Mul(newMmouseWorld);
		FPoint2D mouseWorld(_lastMousePos);
		m.Mul(mouseWorld);

		float dx = newMmouseWorld.x - mouseWorld.x;
		float dy = newMmouseWorld.y - mouseWorld.y;

		ShiftDot(_dotIndex, dx, dy, _moveAllDots);

	} else {
		// тут можно выделить точку если навели на нее
		Matrix m;
		m.MakeRevers(parent);
		FPoint2D fp(mousePos);
		m.Mul(fp);

		_underMouse = SearchNearest(fp.x, fp.y);
	}

	_lastMousePos = mousePos;
}

void SolidGroundLine::MouseUp(const FPoint2D &mousePos) {
	if (_mouseDown) {
		UpdateScreenRect();
		_mouseDown = false;
	}
}

bool SolidGroundLine::DoesContainPoint(const FPoint2D &mousePos) {

	Matrix m;
	m.MakeRevers(parent);
	FPoint2D fp(mousePos);
	m.Mul(fp);

	return SearchNearest(fp.x, fp.y) >= 0 || _screenRect.TestPoint(fp.x, fp.y);
}

FPoint2D SolidGroundLine::GetStart() {
	return FPoint2D(xPoses.front(), yPoses.keys.front().first);
}

FPoint2D SolidGroundLine::GetFinish() {
	return FPoint2D(xPoses.front(), yPoses.keys.front().first);
}