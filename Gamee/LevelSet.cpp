#include "LevelSet.h"
#include "../Core/Math.h"

SetItem::~SetItem() {
	delete _sprite;
	Render::GetDC()->Texture_Free(_texture);
}

SetItem::SetItem(Types type) 
: _type(type)
{
}

SetItem::SetItem(Types type, const char *fileName, const FPoint2D &pos, float angle) 
: _type(type)
{
	_filePath = fileName;
	_angle = angle;
	_pos = pos;

	_texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + _filePath).c_str());
	_sprite = new Sprite(_texture, 0, 0, Render::GetDC()->Texture_GetWidth(_texture), Render::GetDC()->Texture_GetHeight(_texture));
}

bool SetItem::HasPixel(float x, float y) {
	return _sprite->HasPixel(x, y);
}

FPoint2D &SetItem::Pos() {
	return _pos;
}

float &SetItem::Angle() {
	return _angle;
}

void SetItem::SaveToXml(TiXmlElement *xe) const {
	xe->SetAttribute("filePath", _filePath.c_str());
	char s[16];
	sprintf(s, "%f", _angle);
	xe->SetAttribute("angle", s);
	sprintf(s, "%f", _pos.x);
	xe->SetAttribute("x", s);
	sprintf(s, "%f", _pos.y);
	xe->SetAttribute("y", s);
}

void SetItem::Draw() const {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixRotate(_angle / 180 * M_PI);
	_sprite->Render(- _sprite->Width() / 2, - _sprite->Height());
	Render::PopMatrix();
}

void SetItem::LoadFromXml(TiXmlElement *xe) {
	_filePath = xe->Attribute("filePath");
	_angle = atof(xe->Attribute("angle"));
	_pos.x = atof(xe->Attribute("x"));
	_pos.y = atof(xe->Attribute("y"));

	_texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + _filePath).c_str());
	_sprite = new Sprite(_texture, 0, 0, Render::GetDC()->Texture_GetWidth(_texture), Render::GetDC()->Texture_GetHeight(_texture));
}

Beauty::Beauty() 
: SetItem(beauty_item)
{}

Beauty::Beauty(const char *fileName, const FPoint2D &pos, float angle, float scale, bool mirror) 
: SetItem(beauty_item, fileName, pos, angle)
, _scale(scale)
, _mirror(mirror)
{	
}

void Beauty::SaveToXml(TiXmlElement *xe) const {
	SetItem::SaveToXml(xe);
	xe->SetAttribute("mirror", _mirror ? "1" : "0");
	char s[16];
	sprintf(s, "%f", _scale);
	xe->SetAttribute("scale", s);
}

void Beauty::Draw() const {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixMove(_scale, _scale);
	Render::MatrixRotate(_angle / 180 * M_PI);
	if (_mirror) {
		Render::MatrixScale(-1.f, 1.f);
	}
	_sprite->Render(- _sprite->Width() / 2, - _sprite->Height());
	Render::PopMatrix();
}

void Beauty::LoadFromXml(TiXmlElement *xe) {
	SetItem::LoadFromXml(xe);
	_mirror = atoi(xe->Attribute("mirror")) != 0;
	_scale = atof(xe->Attribute("scale"));
}

void LevelBlock::DrawLines() {
	float t = 0.f;
	float x1 = xPoses.getGlobalFrame(0.f);
	float y1 = yPoses.getGlobalFrame(0.f);
	float x2, y2;
	int subLine = xPoses.keys.size() * 6;//���������� ������ �������� �� ������� �������� ������ �������
	while (t + 1.f / subLine < 1.f) {
		t += 1.f / subLine;//���������� ������ �������� �� ������� �������� ������ �������
		x2 = xPoses.getGlobalFrame(t);
		y2 = yPoses.getGlobalFrame(t);
		Render::Line(x1, y1, x2, y2, 0xFFFF0000);
		x1 = x2;
		y1 = y2;
	}
	x2 = xPoses.getGlobalFrame(1.f);
	y2 = yPoses.getGlobalFrame(1.f);
	Render::Line(x1, y1, x2, y2, 0xFFFF0000);			

	static const float SIZEX = 3;
	for (unsigned int i = 0; i < xPoses.keys.size() - 1; ++i) {
		float x = xPoses.getFrame(i, 0.f);
		float y = yPoses.getFrame(i, 0.f);
		Render::Line(x - SIZEX, y - SIZEX, x + SIZEX, y + SIZEX, 0xFFFF0000);
		Render::Line(x - SIZEX, y + SIZEX, x + SIZEX, y - SIZEX, 0xFFFF0000);
	}
}

void LevelBlock::AddPoint(float x, float y) {
	xPoses.addKey(x);
	xPoses.CalculateGradient(true);
	yPoses.addKey(y);
	yPoses.CalculateGradient(true);
}


int LevelBlock::SearchNearest(float x, float y) {
	int result = -1;
	static const float SIZEX = 6;
	for (unsigned int i = 0; i < xPoses.keys.size() - 1 && result < 0; ++i) {
		float px = xPoses.getFrame(i, 0.f);
		float py = yPoses.getFrame(i, 0.f);
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

	float sign = 0.f;
	{
		// ���� ����� ������ ���� ��� �������� �����
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
				if (!intersection && Math::VMul(*b - *a, *c - *b) * sign > 0.f) {// �������� ������ �� ��� � ����� ������
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
	// ���� �������� � ������ HGE ����� ��� ��������� - ����� ������ � ����,
	// � ��������� �� �����������
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
	float t = 0.f;
	float x1 = xPoses.getGlobalFrame(0.f);
	float y1 = yPoses.getGlobalFrame(0.f);
	float x2, y2;
	int subLine = xPoses.keys.size() * 6;//���������� ������ �������� �� ������� �������� ������ �������
	FPoint2D gravity(0.f, 10.f);
	float speed = 50.f;
	//FPoint2D motor();
	while (t + 1.f / subLine < 1.f) {
		t += 1.f / subLine;//���������� ������ �������� �� ������� �������� ������ �������
		x2 = xPoses.getGlobalFrame(t);
		y2 = yPoses.getGlobalFrame(t);
		if (x1 <= pos.x && pos.x < x2) {
			// calc pos
		//Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);
			return true;
		}
		x1 = x2;
		y1 = y2;
	}
	x2 = xPoses.getGlobalFrame(1.f);
	y2 = yPoses.getGlobalFrame(1.f);
	if (x1 <= pos.x && pos.x < x2) {
	//	Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);			
		return true;
	}
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
	if (xPoses.keys.size() >= 50) {
		return false;
	}
	bool result = false;
	static const float SIZEX = 6;
	FPoint2D p(x, y);

	float t = 0.f;
	FPoint2D one(xPoses.getGlobalFrame(0.f), yPoses.getGlobalFrame(0.f));
	FPoint2D two;
	int subLine = xPoses.keys.size() * 6;//���������� ������ �������� �� ������� �������� ������ �������
	while (t + 1.f / subLine < 1.f && !result) {
		t += 1.f / subLine;
		two.x = xPoses.getGlobalFrame(t);
		two.y = yPoses.getGlobalFrame(t);
		if (result = DotNearLine(one, two, p)) {
			int index = t * (xPoses.keys.size() - 1) + 1;

			if (index < xPoses.keys.size() - 1) {
				SplinePath splineX = xPoses;
				SplinePath splineY = yPoses;
				xPoses.Clear();
				yPoses.Clear();
				for (int i = 0; i < index; ++i) {
					xPoses.addKey(splineX.getFrame(i, 0.f));
					yPoses.addKey(splineY.getFrame(i, 0.f));
				}
				xPoses.addKey(x);
				yPoses.addKey(y);
				for (int i = index; i < splineX.keys.size() - 1; ++i) {
					xPoses.addKey(splineX.getFrame(i, 0.f));
					yPoses.addKey(splineY.getFrame(i, 0.f));
				}
				xPoses.CalculateGradient(true);
				yPoses.CalculateGradient(true);
			} else {
				xPoses.addKey(x);
				yPoses.addKey(y);
				xPoses.CalculateGradient(true);
				yPoses.CalculateGradient(true);
			}
		}
		one = two;
	}
	if (!result) {
		two.x = xPoses.getGlobalFrame(1.f);
		two.y = yPoses.getGlobalFrame(1.f);
		if (result = DotNearLine(one, two, p)) {
			int index = xPoses.keys.size() - 1;
			SplinePath splineX = xPoses;
			SplinePath splineY = yPoses;
			xPoses.Clear();
			yPoses.Clear();
			for (int i = 0; i < index; ++i) {
				xPoses.addKey(splineX.getFrame(i, 0.f));
				yPoses.addKey(splineY.getFrame(i, 0.f));
			}
			xPoses.addKey(x);
			yPoses.addKey(y);
			for (int i = index; i < splineX.keys.size() - 1; ++i) {
				xPoses.addKey(splineX.getFrame(i, 0.f));
				yPoses.addKey(splineY.getFrame(i, 0.f));
			}
			xPoses.CalculateGradient(true);
			yPoses.CalculateGradient(true);
		}
	}

	return result;
}

void LevelBlock::RemoveDot(int index) {
	if (xPoses.keys.size() <= 4) {
		return;
	}
	SplinePath splineX = xPoses;
	SplinePath splineY = yPoses;
	xPoses.Clear();
	yPoses.Clear();
	for (int i = 0; i < index; ++i) {
		xPoses.addKey(splineX.getFrame(i, 0.f));
		yPoses.addKey(splineY.getFrame(i, 0.f));
	}
	for (int i = index + 1; i < splineX.keys.size() - 1; ++i) {
		xPoses.addKey(splineX.getFrame(i, 0.f));
		yPoses.addKey(splineY.getFrame(i, 0.f));
	}
	xPoses.CalculateGradient(true);
	yPoses.CalculateGradient(true);
}

void LevelBlock::ExportToLines(std::vector<FPoint2D> &lineDots) {
	lineDots.clear();
	float t = 0.f;
	int subLine = xPoses.keys.size() * 6;//���������� ������ �������� �� ������� �������� ������ �������
	while (t < 1.f) {
		float x = xPoses.getGlobalFrame(t);
		float y = yPoses.getGlobalFrame(t);
		lineDots.push_back(FPoint2D(x, y));
		t += 1.f / subLine;//���������� ������ �������� �� ������� �������� ������ �������
	}
}

Lines * LevelBlock::CreateBody(Byker *byker, int splineIndex) {
	DotsList dots;
	int subLine = xPoses.keys.size() * 6;//���������� ������ �������� �� ������� �������� ������ �������
	dots.resize(subLine);
	for (unsigned int i = 0; i < subLine; ++i) {
		FPoint2D &a = dots[i];
		float t = static_cast<float>(i) / subLine;
		a.x = xPoses.getGlobalFrame(t) / SCALE_BOX2D;
		a.y = yPoses.getGlobalFrame(t) / SCALE_BOX2D;
	}
	return byker->physic.AddLinesSet(dots, splineIndex);
	//// ����������� �������������
	//int n = triangles.size();
	//std::vector<Triangle> massiv;
	//for (unsigned int j = 0; j < n; ++j) {
	//	Triangle vec(3);
	//	for (unsigned int i = 0; i < 3; ++i) {
	//		vec[i].x = (triangles[j].v[2 - i].x) / SCALE_BOX2D;
	//		vec[i].y = (triangles[j].v[2 - i].y) / SCALE_BOX2D;
	//	}
	//	massiv.push_back(vec);
	//}
	//// ����������� - ����������� ������������� � �������� ��������������
	//for (unsigned int i = 0; i < massiv.size(); ) {
	//	Triangle t;
	//	for (unsigned int j = i + 1; j < (massiv.size() - 1) && t.size() == 0; ++j) {
	//		for (unsigned int k = 0; k < massiv[i].size() && t.size() == 0; ++k) {
	//			b2Vec2 a = massiv[i][k];
	//			b2Vec2 b = massiv[i][(k + 1) % massiv[i].size()];
	//			for (unsigned int l = 0; l < massiv[j].size() && t.size() == 0; ++l) {
	//				b2Vec2 d = massiv[j][l];
	//				b2Vec2 e = massiv[j][(l + 1) % massiv[j].size()];
	//				if ((a - d).Length() < 1e-3 && (b - e).Length() < 1e-3) {
	//					// ����� ����� �����
	//					for (unsigned int q = 0; q < massiv[i].size(); ++q)  {
	//						t.push_back(massiv[i][(q + k + 1) % massiv[i].size()]);
	//					}
	//					for (unsigned int q = 0; q < massiv[j].size(); ++q)  {
	//						unsigned int index = massiv[j].size() - 1 - q;
	//						if (index != l && index != (l + 1)) {
	//							t.push_back(massiv[j][index]);
	//						}
	//					}
	//					if (!Convex(t)) {
	//						t.clear();
	//					}
	//				} else if ((a - e).Length() < 1e-3 && (b - d).Length() < 1e-3) {
	//					// ����� ����� �����
	//					for (unsigned int q = 0; q < massiv[i].size(); ++q)  {
	//						t.push_back(massiv[i][(q + k + 1) % massiv[i].size()]);
	//					}
	//					for (unsigned int q = 0; q < massiv[j].size(); ++q)  {
	//						unsigned int index = massiv[j].size() - 1 - q;
	//						if (index != l && index != (l + 1)) {
	//							t.push_back(massiv[j][index]);
	//						}
	//					}
	//					if (!Convex(t)) {
	//						t.clear();
	//					}
	//				}
	//			}
	//		}
	//		if (t.size()) {
	//			massiv.erase(massiv.begin() + j);
	//		}
	//	}
	//	if (t.size()) {
	//		massiv[i] = t;
	//		t.clear();
	//	} else {
	//		++i;
	//	}
	//}
	//for (unsigned int j = 0; j < massiv.size(); ++j) {
	//	b2FixtureDef fd;
	//	fd.restitution = 0.f;
	//	fd.friction = 1.0f;
	//	fd.density = 1.0f;
	//	b2PolygonShape shape;
	//	//b2Vec2 vec[3];
	//	//for (unsigned int i = 0; i < 3; ++i) {
	//	//	vec[i].x = (triangles[j].v[2 - i].x) / SCALE_BOX2D;
	//	//	vec[i].y = (triangles[j].v[2 - i].y) / SCALE_BOX2D;
	//	//}
	//	shape.Set(&(*massiv[j].begin()), massiv[j].size());
	//	fd.shape = &shape;
	//	body->CreateFixture(&fd);
	//	body->ResetMassData();
	//}
}

void LevelSet::Clear() {
	startpoint.clear();
	endpoint.clear();
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
		
			SplinePath x;
			SplinePath y;
			TiXmlElement *dot = elem->FirstChildElement("dot");
			while (dot != NULL) {
				x.addKey(atof(dot->Attribute("x")));
				y.addKey(atof(dot->Attribute("y")));
				dot = dot->NextSiblingElement();
			}
			x.CalculateGradient(true);
			y.CalculateGradient(true);
			l->xPoses = x;
			l->yPoses = y;

			elem = elem->NextSiblingElement("elem");
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
		}
	}

	TiXmlElement *word = xe->FirstChildElement("word");

	if (word->Attribute("startX")) {
		startpoint.push_back(
			FPoint2D(atof(word->Attribute("startX")), 
						atof(word->Attribute("startY"))));
	}
	if (word->Attribute("endX")) {
		endpoint.push_back(
			FPoint2D(atof(word->Attribute("endX")), 
						atof(word->Attribute("endY"))));
	}
	for (unsigned int i = 0; i < ground.size(); ++i) {
		ground[i]->GenerateTriangles();
	}
}

