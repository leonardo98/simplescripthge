#include "LevelSet.h"
#include "../Core/Math.h"

void LevelBlock::DrawLines() {
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

	static const float SIZEX = 3;
	for (unsigned int i = 0; i < xPoses.size(); ++i) {
		float x = xPoses[i];
		float y = yPoses.keys[i].first;
		Render::Line(x - SIZEX, y - SIZEX, x + SIZEX, y + SIZEX, 0xFFFF0000);
		Render::Line(x - SIZEX, y + SIZEX, x + SIZEX, y - SIZEX, 0xFFFF0000);
	}
}

void LevelBlock::AddPoint(float x, float y) {
	yPoses.addKey(y);
	yPoses.CalculateGradient(false);
	xPoses.push_back(x);
}


int LevelBlock::SearchNearest(float x, float y) {
	int result = -1;
	static const float SIZEX = 6;
	for (unsigned int i = 0; i < xPoses.size() && result < 0; ++i) {
		float px = xPoses[i];
		float py = yPoses.keys[i].first;
		if ((FPoint2D(x, y) - FPoint2D(px, py)).Length() < SIZEX) {
			result = i;
		}
	}
	return result;
}

void LevelBlock::GenerateTriangles() {
	triangles.clear();
	std::vector<FPoint2D> &dots = lineDots;
	ExportToLines(dots);

	float minx = dots.front().x;
	float maxx = dots.back().x;
	float maxy = dots[0].y;
	for (unsigned int i = 1; i < dots.size(); ++i) {
		if (maxy > dots[i].y) {
			maxy = dots[i].y;
		}
	}
	maxy += 1000.f;
	dots.push_back(FPoint2D(maxx, maxy));
	dots.push_back(FPoint2D(minx, maxy));

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
			if (i < dots.size() - 1) {
				b = &dots[i + 1];
			} else {
				b = &dots[i + 1 - dots.size()];
			}
			if (i < dots.size() - 2) {
				c = &dots[i + 2];
			} else {
				c = &dots[i + 2 - dots.size()];
			}
			float angle = (*a - *b).Angle(&(*c - *b));
			if (angle <= 0.f) {
				assert(false);
			}
			if (angle < minAngle) {
				minAngle = angle;
				index = i;
			}
		}
		a = &dots[index];
		if (index < dots.size() - 1) {
			b = &dots[index + 1];
		} else {
			b = &dots[index + 1 - dots.size()];
		}
		if (index < dots.size() - 2) {
			c = &dots[index + 2];
		} else {
			c = &dots[index + 2 - dots.size()];
		}
		sign = Math::VMul(*b - *a, *c - *b);
	}
	while (dots.size() > 0) {
		assert(dots.size() > 2);
		if (dots.size() == 3) {
			hgeTriple tri;
			FillTriangle(dots[0], dots[1], dots[2], tri);
			triangles.push_back(tri);
			dots.clear();
		} else {
			FPoint2D *a;
			FPoint2D *b;
			FPoint2D *c;
			for (int i = 0; i < dots.size(); ++i) {
				a = &dots[i];
				if (i < dots.size() - 1) {
					b = &dots[i + 1];
				} else {
					b = &dots[i + 1 - dots.size()];
				}
				if (i < dots.size() - 2) {
					c = &dots[i + 2];
				} else {
					c = &dots[i + 2 - dots.size()];
				}
				
				bool intersection = false;
				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					FPoint2D *b2;
					if (j < dots.size() - 1) {
						b2 = &dots[j + 1];
					} else {
						b2 = &dots[j + 1 - dots.size()];
					}
					intersection = (a != a2 && a != b2 && b != a2 && b != b2 && Math::Intersection(*a, *c, *a2, *b2, NULL));
				}
				std::vector<FPoint2D> triangle(3);
				triangle[0] = *a;
				triangle[1] = *b;
				triangle[2] = *c;
				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					intersection = (a2 != a && a2 != b && a2 != c && Math::Inside(*a2, triangle));
				}
				if (!intersection && Math::VMul(*b - *a, *c - *b) * sign > 0.f) {// выбираем только те что с нашим знаком
					hgeTriple tri;
					FillTriangle(*a, *b, *c, tri);
					triangles.push_back(tri);
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
}

void LevelBlock::FillTriangle(const FPoint2D &a, const FPoint2D &b, const FPoint2D &c, hgeTriple &tri) {
	tri.v[0].x = a.x;
	tri.v[0].y = a.y;
	tri.v[1].x = b.x;
	tri.v[1].y = b.y;
	tri.v[2].x = c.x;
	tri.v[2].y = c.y;

	tri.blend = BLEND_ALPHABLEND | BLEND_COLORMUL;
	// надо добавить в движке HGE режим без блендинга - можно только в игре,
	// в редакторе не обязательно
	tri.tex = 0;//_allElements->GetTexture();
	for (unsigned int i = 0; i < 3; ++i) {
		tri.v[i].col = 0xFF4c812d;
		tri.v[i].z = 0.f;
		tri.v[i].tx = tri.v[i].x / 512.f;
		tri.v[i].ty = tri.v[i].y / 512.f;
	}
}

void LevelBlock::DrawTriangles() {
	int n = triangles.size();// * f;
	const Matrix &m = Render::GetCurrentMatrix();
	for (unsigned int j = 0; j < n; ++j) {
		hgeTriple tri = triangles[j];
		for (unsigned int i = 0; i < 3; ++i) {
			m.Mul(tri.v[i].x, tri.v[i].y);
		}
		Render::GetDC()->Gfx_RenderTriple(&tri);
	}

	if (lineDots.size() >= 2) {
		// test debug
		float x1 = lineDots[0].x;
		float y1 = lineDots[0].y;
		float x2, y2;
		for (unsigned int i = 0; i < lineDots.size(); ++i) {
			x2 = lineDots[i].x;
			y2 = lineDots[i].y;
			Render::Line(x1, y1, x2, y2, 0x4FFFFFFF);
			x1 = x2;
			y1 = y2;
		}
		x2 = lineDots[0].x;
		y2 = lineDots[0].y;
		Render::Line(x1, y1, x2, y2, 0x4FFFFFFF);			
	}
}

bool LevelBlock::SearchProection(FPoint2D &pos) {
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

bool LevelBlock::DotNearLine(const FPoint2D &one, const FPoint2D &two, const FPoint2D &p) {
	float a = (one - p).Length();
	float b = (p - two).Length();
	float c = (one - two).Length();
	if (c > a && c > b) {
		float s;
		Math::STrinagle(a, b, c, s);
		return (s / c < 4);
	}
	return false;
}

bool LevelBlock::CreateDot(float x, float y) {
	if (xPoses.size() >= 20) {
		return false;
	}
	bool result = false;
	static const float SIZEX = 6;
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
			if (result = DotNearLine(one, two, p)) {
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

void LevelBlock::RemoveDot(int index) {
	if (xPoses.size() <= 4) {
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
}

void LevelBlock::ExportToLines(std::vector<FPoint2D> &lineDots) {
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

Lines * LevelBlock::CreateBody(Byker *byker, int splineIndex) {
	DotsList dots;
	ExportToLines(dots);
	for (unsigned int i = 0; i < dots.size(); ++i) {
		dots[i] *= (1.f / SCALE_BOX2D);
	}
	return byker->physic.AddLinesSet(dots, splineIndex);
}

std::string LevelBlock::Type() {
	return "elem";
}

void LevelBlock::SaveToXml(TiXmlElement *xe) {
	for (int j = 0; j < xPoses.size(); ++j) {
		TiXmlElement *dot = new TiXmlElement("dot");
		char s[16];
		sprintf(s, "%f", xPoses[j]); 
		dot->SetAttribute("x", s);
		sprintf(s, "%f", yPoses.keys[j].first); 
		dot->SetAttribute("y", s);
		xe->LinkEndChild(dot);
	}
}

void LevelBlock::LoadFromXml(TiXmlElement* xe) {
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




void LevelIsland::AddBottomPoint(float x, float y) {
	yBottomPoses.addKey(y);
	yBottomPoses.CalculateGradient(false);
	xBottomPoses.push_back(x);
}

void LevelIsland::ExportToLines(std::vector<FPoint2D> &lineDots) {
	LevelBlock::ExportToLines(lineDots);
	for (unsigned int i = 0; i < xBottomPoses.size() - 1; ++i) {
		float minx = xBottomPoses[i];
		float maxx = xBottomPoses[i + 1];
		for (unsigned int k = 0; k < 6; ++k) {
			float x = minx + (maxx - minx) * k / 6.f;
			float y = yBottomPoses.getFrame(i, k / 6.f);
			lineDots.push_back(FPoint2D(x, y));
		}
	}
	float x = xBottomPoses.back();
	float y = yBottomPoses.getGlobalFrame(1.f);
	lineDots.push_back(FPoint2D(x, y));
	lineDots.push_back(lineDots.front());
}

void LevelIsland::GenerateTriangles() {
	triangles.clear();
	std::vector<FPoint2D> &dots = lineDots;
	ExportToLines(dots);

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
			if (i < dots.size() - 1) {
				b = &dots[i + 1];
			} else {
				b = &dots[i + 1 - dots.size()];
			}
			if (i < dots.size() - 2) {
				c = &dots[i + 2];
			} else {
				c = &dots[i + 2 - dots.size()];
			}
			float angle = (*a - *b).Angle(&(*c - *b));
			if (angle <= 0.f) {
				assert(false);
			}
			if (angle < minAngle) {
				minAngle = angle;
				index = i;
			}
		}
		a = &dots[index];
		if (index < dots.size() - 1) {
			b = &dots[index + 1];
		} else {
			b = &dots[index + 1 - dots.size()];
		}
		if (index < dots.size() - 2) {
			c = &dots[index + 2];
		} else {
			c = &dots[index + 2 - dots.size()];
		}
		sign = Math::VMul(*b - *a, *c - *b);
	}
	while (dots.size() > 0) {
		assert(dots.size() > 2);
		if (dots.size() == 3) {
			hgeTriple tri;
			FillTriangle(dots[0], dots[1], dots[2], tri);
			triangles.push_back(tri);
			dots.clear();
		} else {
			FPoint2D *a;
			FPoint2D *b;
			FPoint2D *c;
			for (int i = 0; i < dots.size(); ++i) {
				a = &dots[i];
				if (i < dots.size() - 1) {
					b = &dots[i + 1];
				} else {
					b = &dots[i + 1 - dots.size()];
				}
				if (i < dots.size() - 2) {
					c = &dots[i + 2];
				} else {
					c = &dots[i + 2 - dots.size()];
				}
				
				bool intersection = false;
				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					FPoint2D *b2;
					if (j < dots.size() - 1) {
						b2 = &dots[j + 1];
					} else {
						b2 = &dots[j + 1 - dots.size()];
					}
					intersection = (a != a2 && a != b2 && b != a2 && b != b2 && Math::Intersection(*a, *c, *a2, *b2, NULL));
				}
				std::vector<FPoint2D> triangle(3);
				triangle[0] = *a;
				triangle[1] = *b;
				triangle[2] = *c;
				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					intersection = (a2 != a && a2 != b && a2 != c && Math::Inside(*a2, triangle));
				}
				if (!intersection && Math::VMul(*b - *a, *c - *b) * sign > 0.f) {// выбираем только те что с нашим знаком
					hgeTriple tri;
					FillTriangle(*a, *b, *c, tri);
					triangles.push_back(tri);
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
}

void LevelIsland::DrawLines() {
	LevelBlock::DrawLines();

	assert(xBottomPoses.size() != 1);

	for (unsigned int i = 0; i < xBottomPoses.size() - 1; ++i) {
		float minx = xBottomPoses[i];
		float maxx = xBottomPoses[i + 1];
		for (unsigned int k = 0; k < 6; ++k) {
			float x1 = minx + (maxx - minx) * k / 6.f;
			float x2 = minx + (maxx - minx) * (k + 1) / 6.f;
			float y1 = yBottomPoses.getFrame(i, k / 6.f);
			float y2 = yBottomPoses.getFrame(i, (k + 1) / 6.f);
			Render::Line(x1, y1, x2, y2, 0xFFFF0000);
		}
	}

	static const float SIZEX = 3;
	for (unsigned int i = 0; i < xBottomPoses.size(); ++i) {
		float x = xBottomPoses[i];
		float y = yBottomPoses.keys[i].first;
		Render::Line(x - SIZEX, y - SIZEX, x + SIZEX, y + SIZEX, 0xFFFF0000);
		Render::Line(x - SIZEX, y + SIZEX, x + SIZEX, y - SIZEX, 0xFFFF0000);
	}

	Render::Line(xBottomPoses.front(), yBottomPoses.getGlobalFrame(0.f), xPoses.back(), yPoses.getGlobalFrame(1.f), 0xFFFF0000);
	Render::Line(xBottomPoses.back(), yBottomPoses.getGlobalFrame(1.f), xPoses.front(), yPoses.getGlobalFrame(0.f), 0xFFFF0000);

}

int LevelIsland::SearchNearest(float x, float y) {
	int result = -1;
	static const float SIZEX = 6;
	for (unsigned int i = 0; i < xBottomPoses.size() && result < 0; ++i) {
		float px = xBottomPoses[i];
		float py = yBottomPoses.keys[i].first;
		if ((FPoint2D(x, y) - FPoint2D(px, py)).Length() < SIZEX) {
			result = i;
		}
	}
	return (result != -1 ? result + xPoses.size(): LevelBlock::SearchNearest(x, y));
}

bool LevelIsland::CreateDot(float x, float y) {
	if (xBottomPoses.size() >= 20) {
		return false;
	}
	bool result = false;
	static const float SIZEX = 6;
	FPoint2D p(x, y);
	FPoint2D one;
	FPoint2D two;
	for (unsigned int i = 0; i < (xBottomPoses.size() - 1) && !result; ++i) {
		float minx = xBottomPoses[i];
		float maxx = xBottomPoses[i + 1];
		for (unsigned int k = 0; k < 6 && !result; ++k) {
			one.x = minx + (maxx - minx) * k / 6.f;
			two.x = minx + (maxx - minx) * (k + 1) / 6.f;
			one.y = yBottomPoses.getFrame(i, k / 6.f);
			two.y = yBottomPoses.getFrame(i, (k + 1) / 6.f);
			if (result = DotNearLine(one, two, p)) {
				int index = i + 1;
				if (index < xBottomPoses.size()) {
					std::vector<float> splineX = xBottomPoses;
					SplinePath splineY = yBottomPoses;
					xBottomPoses.clear();
					yBottomPoses.Clear();
					for (int i = 0; i < index; ++i) {
						xBottomPoses.push_back(splineX[i]);
						yBottomPoses.addKey(splineY.keys[i].first);
					}
					xBottomPoses.push_back(x);
					yBottomPoses.addKey(y);
					for (int i = index; i < splineX.size(); ++i) {
						xBottomPoses.push_back(splineX[i]);
						yBottomPoses.addKey(splineY.keys[i].first);
					}
					yBottomPoses.CalculateGradient(false);
				} else {
					xBottomPoses.push_back(x);
					yBottomPoses.addKey(y);
					yBottomPoses.CalculateGradient(false);
				}
			}
		}
	}
	return result || LevelBlock::CreateDot(x, y);
}

void LevelIsland::RemoveDot(int index) {
	if (index < xPoses.size()) {
		LevelBlock::RemoveDot(index);
		return;
	}
	index -= xPoses.size();
	if (xBottomPoses.size() <= 4) {
		return;
	}
	std::vector<float> splineX = xBottomPoses;
	SplinePath splineY = yBottomPoses;
	xBottomPoses.clear();
	yBottomPoses.Clear();
	for (int i = 0; i < index; ++i) {
		xBottomPoses.push_back(splineX[i]);
		yBottomPoses.addKey(splineY.keys[i].first);
	}
	for (int i = index + 1; i < splineX.size(); ++i) {
		xBottomPoses.push_back(splineX[i]);
		yBottomPoses.addKey(splineY.keys[i].first);
	}
	yBottomPoses.CalculateGradient(false);
}

std::string LevelIsland::Type() {
	return "island";
}

void LevelIsland::SaveToXml(TiXmlElement *xe) {
	TiXmlElement *top = new TiXmlElement("top");
	LevelBlock::SaveToXml(top);
	xe->LinkEndChild(top);

	TiXmlElement *bottom = new TiXmlElement("bottom");
	for (int j = 0; j < xBottomPoses.size(); ++j) {
		TiXmlElement *dot = new TiXmlElement("dot");
		char s[16];
		sprintf(s, "%f", xBottomPoses[j]); 
		dot->SetAttribute("x", s);
		sprintf(s, "%f", yBottomPoses.keys[j].first); 
		dot->SetAttribute("y", s);
		bottom->LinkEndChild(dot);
	}
	xe->LinkEndChild(bottom);
}

void LevelIsland::LoadFromXml(TiXmlElement* xe) {
	LevelBlock::LoadFromXml(xe->FirstChildElement("top"));

	xe = xe->FirstChildElement("bottom");

	std::vector<float> x;
	SplinePath y;
	TiXmlElement *dot = xe->FirstChildElement("dot");
	while (dot != NULL) {
		x.push_back(atof(dot->Attribute("x")));
		y.addKey(atof(dot->Attribute("y")));
		dot = dot->NextSiblingElement();
	}
	y.CalculateGradient(false);
	
	xBottomPoses = x;
	yBottomPoses = y;
}




void LevelSet::Clear() {
	for (unsigned int i = 0; i < ground.size(); ++i) {
		delete ground[i];
	}
	ground.clear();
	for (unsigned int i = 0; i < surpris.size(); ++i) {
		delete surpris[i];
	}
	surpris.clear();
	for (unsigned int i = 0; i < movable.size(); ++i) {
		delete movable[i];
	}
	movable.clear();
	for (unsigned int i = 0; i < images.size(); ++i) {
		delete images[i].sprite;
		Render::GetDC()->Texture_Free(images[i].texture);
	}
	images.clear();
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		delete beauties[i];
	}
	beauties.clear();
	_start.clear();
	_end.clear();
}

void LevelSet::LoadFromXml(TiXmlElement *xe, bool gameMode) {
	Clear();
	assert(xe != NULL);
	// level loading
	TiXmlElement *groundXML = xe->FirstChildElement("Ground");
	if (groundXML) {
		TiXmlElement *elem = groundXML->FirstChildElement("elem");
		while (elem != NULL) {
			LevelBlock *l = new LevelBlock();
			ground.push_back(l);
		
			l->LoadFromXml(elem);
			elem = elem->NextSiblingElement("elem");
		}
		elem = groundXML->FirstChildElement("island");
		while (elem != NULL) {
			LevelIsland *l = new LevelIsland();
			ground.push_back(l);
		
			l->LoadFromXml(elem);
			elem = elem->NextSiblingElement("island");
		}
	}
	if (!gameMode) {
		TiXmlElement *imagesList = xe->FirstChildElement("Images");
		if (imagesList) {
			TiXmlElement *elem = imagesList->FirstChildElement("image");
			while (elem != NULL) {
				OneImage image;
				std::string msg = elem->Attribute("filePath");
				image.texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + "data\\images\\" + msg).c_str());
				image.sprite = new Sprite(image.texture, 0, 0, Render::GetDC()->Texture_GetWidth(image.texture), Render::GetDC()->Texture_GetHeight(image.texture));
				image.pos = FPoint2D(0.f, 0.f);
				image.filePath = msg;
				images.push_back(image);
				elem = elem->NextSiblingElement("image");
			}
		}
	}
	{
		TiXmlElement *beautyList = xe->FirstChildElement("Beauties");
		if (beautyList) {
			TiXmlElement *elem = beautyList->FirstChildElement("beauty");
			while (elem != NULL) {
				Beauty *beauty = new Beauty();
				beauty->LoadFromXml(elem);
				beauties.push_back(beauty);
				elem = elem->NextSiblingElement("beauty");
			}
			elem = beautyList->FirstChildElement("ground");
			while (elem != NULL) {
				GroundLine *beauty = new GroundLine();
				beauty->LoadFromXml(elem);
				beauties.push_back(beauty);
				elem = elem->NextSiblingElement("ground");
			}
		}
	}

	TiXmlElement *word = xe->FirstChildElement("word");

	for (unsigned int i = 0; i < ground.size(); ++i) {
		ground[i]->GenerateTriangles();
	}
}

FPoint2D LevelSet::Startpoint() {
	if (_start.size()) {
		assert(_start.size() == 1);
		return _start.front();
	}
	FPoint2D s;
	s.x = ground[0]->xPoses.front();
	s.y = ground[0]->yPoses.keys.front().first;
	for (unsigned int i = 0; i < ground.size(); ++i) {
		if (ground[i]->xPoses.front() < s.x) {
			s.x = ground[i]->xPoses.front();
			s.y = ground[i]->yPoses.keys.front().first;
		}
	}
	_start.push_back(s);
	return s;
}

FPoint2D LevelSet::Endpoint() {
	if (_end.size()) {
		assert(_end.size() == 1);
		return _end.front();
	}
	FPoint2D e;
	e.x = ground[0]->xPoses.back();
	e.y = ground[0]->yPoses.keys.back().first;
	for (unsigned int i = 0; i < ground.size(); ++i) {
		if (e.x < ground[i]->xPoses.back()) {
			e.x = ground[i]->xPoses.back();
			e.y = ground[i]->yPoses.keys.back().first;
		}
	}
	_end.push_back(e);
	return e;
}
