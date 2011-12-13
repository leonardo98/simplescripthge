#include "PersPaths.h"
#include "../Core/Core.h"

AllDots PersPaths::_dots;

OneDot::OneDot(float x, float y) 
: pos(x, y)
{}

void PersPaths::SetPair(int one, int two) {
	_dots[one]->connections.push_back(two);
	_dots[two]->connections.push_back(one);
}

int PersPaths::IndexOf(const OneDot *dot) {
	int counter = 0;
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		if ((*i) == dot) {
			return counter;
		}
		++counter;
	}
	return -1;
}

PersPaths::PersPaths(void)
{
	_mouseDown = false;

	_changes = false;
	_visible = false;
	_selected = -1;
	_texture = Core::getTexture("data\\textures\\red.png");
	TiXmlDocument doc(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + "PathDots.xml").c_str()));
	if (!doc.LoadFile()) {
		assert(false);
		return;
	}
	TiXmlElement *root = doc.RootElement();
	TiXmlElement *dot = root->FirstChildElement("dot");
	while (dot) {
		_dots.push_back(new OneDot(atof(dot->Attribute("x")), atof(dot->Attribute("y"))));
		dot = dot->NextSiblingElement("dot");
	}
	TiXmlElement *pair = root->FirstChildElement("pair");
	std::vector<std::vector<bool> > loaded = std::vector<std::vector<bool> >(_dots.size(), std::vector<bool>(_dots.size(), false));
	while (pair) {
		int li = atoi(pair->Attribute("one"));
		int lj = atoi(pair->Attribute("two"));
		if (!loaded[li][lj]) {
			SetPair(li, lj);
			loaded[li][lj] = true;
			loaded[lj][li] = true;
		}
		pair = pair->NextSiblingElement("pair");
	}
	Generate();
}

void PersPaths::Save() {
	TiXmlElement *root = new TiXmlElement("root");

	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		TiXmlElement *dot = new TiXmlElement("dot");
		dot->SetAttribute("x", (*i)->pos.x);
		dot->SetAttribute("y", (*i)->pos.y);
		root->LinkEndChild(dot);
	}
	std::vector<std::vector<bool> > saved = std::vector<std::vector<bool> >(_dots.size(), std::vector<bool>(_dots.size(), false));
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		for (int k = 0; k < (*i)->connections.size(); k++) {
			int si = IndexOf((*i));
			int sj = (*i)->connections[k];
			if (!saved[si][sj]) {
				TiXmlElement *pair = new TiXmlElement("pair");
				pair->SetAttribute("one", si);
				pair->SetAttribute("two", sj);
				root->LinkEndChild(pair);
				saved[si][sj] = true;
				saved[sj][si] = true;
			}
		}
	}
	TiXmlDocument doc("PathDots.xml");
	doc.LinkEndChild(root);
	if (!doc.SaveFile()) {
		LOG("Cant save file");
	}
}

void PersPaths::OnMouseDown(const FPoint2D &mousePos) {
	if (!_visible) {
		return;
	}
	_mouseDown = true;

	if (Render::GetDC()->Input_GetKeyState(HGEK_1)) {
		Add(mousePos);
		_changes = true;
	} else if (Render::GetDC()->Input_GetKeyState(HGEK_0)) {
		Remove(mousePos);
		_changes = true;
	} else if (Render::GetDC()->Input_GetKeyState(HGEK_2)) {
		if (_selected == -1) {
			_selected = Find(mousePos);
		} else {
			int index = Find(mousePos);
			if (index >= 0) {
				SetPair(_selected, index);
				_changes = true;
			}
			_selected = -1;
		}
	} else {
		_selected = -1;
	}
}

void PersPaths::OnMouseMove(const FPoint2D &mousePos) {
	if (!_visible) {
		return;
	}
	int index = Find(_lastMousePos);
	if (_mouseDown && Render::GetDC()->Input_GetKeyState(HGEK_SHIFT) && index >= 0) {
		_dots[index]->pos += (mousePos - _lastMousePos);
		_changes = true;
	}
	_lastMousePos = mousePos;
}

void PersPaths::OnMouseUp() {
	_mouseDown = false;
}

PersPaths::~PersPaths(void)
{
	if (_changes) {
		Save();
	}
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		delete (*i);
	}
}

void PersPaths::SetVisible_debug(bool active) {
	_visible = active;
}

bool PersPaths::IsMouseOver(const FPoint2D &mousePos) {
	return _visible;//_visible;
}

void PersPaths::Draw() {
	if (!_visible) {
		return;
	}
	if (_changes) {
		Render::PrintString(512, 0, "", "net change");
	}
	if (_selected >= 0) {
		Render::Line(_dots[_selected]->pos.x, _dots[_selected]->pos.y, _lastMousePos.x, _lastMousePos.y, 0xFFFF0000);
	}
	int counter = 0;
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		for (int k = 0; k < (*i)->connections.size(); k++) {
			if ((*i)->pos.x >= _dots[(*i)->connections[k]]->pos.x) {
				Render::Line((*i)->pos.x, (*i)->pos.y, _dots[(*i)->connections[k]]->pos.x, _dots[(*i)->connections[k]]->pos.y, 0xFFFF0000);
				++counter;
			}	
		}
	}
	counter = counter;
	for (int i = 0; i < _dots.size(); ++i) {
		_texture->Render(_dots[i]->pos.x - _texture->Width() / 2, _dots[i]->pos.y - _texture->Height() / 2);
		char buff[10];
		sprintf(buff, "%i", i);
		Render::PrintString(_dots[i]->pos.x - _texture->Width() / 2, _dots[i]->pos.y - _texture->Height() / 2, "", buff);
	}
}

int PersPaths::Find(const FPoint2D &pos) {
	for (int i = 0, e = _dots.size(); i < e; ++i) {
		if ((pos - _dots[i]->pos).Length() < 16) {//_texture->Width()) {
			return i;
		}
	}
	return -1;
}

void PersPaths::Remove(const FPoint2D &pos) {
	int index = Find(pos);
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		for (DotsIndex::iterator k = (*i)->connections.begin(); k != (*i)->connections.end(); ) {
			if ((*k) == index) {
				k = (*i)->connections.erase(k);
			} else {
				if ((*k) > index) {
					--(*k);
				}
				++k;
			}
		}
	}
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		if ((*i) == _dots[index]) {
			delete _dots[index];
			_dots.erase(i);
			return;
		}
	}
}

void PersPaths::Add(const FPoint2D &pos) {
	_dots.push_back(new OneDot(pos.x, pos.y));
}

FPoint2D PersPaths::NextPointTo(const FPoint2D &from, const FPoint2D &to) {
	if (_dots.size() == 0) {// debug test only
		return to;
	}
	int index1 = Find(from);
	int index2 = Find(to);
	if (index1 == index2) {
		return to;
	}
	if (index2 < 0) {
		FPoint2D t(99999.f, 999999.f);
		int min = 0;
		for (int i = 0, e = _dots.size(); i < e; ++i) {
			if ((to - _dots[i]->pos).Length() < (to - _dots[min]->pos).Length()) {
				min = i;
			}
		}
		index2 = min;
	}
	unsigned int n = _dots[index1]->connections.size();
	int minIndex = 0;
	for (unsigned int i = 1; i < n; ++i) {
		if (_indexDistance[index2][_dots[index1]->connections[minIndex]] > _indexDistance[index2][_dots[index1]->connections[i]]) {
			minIndex = i;
		}
	}
	return _dots[_dots[index1]->connections[minIndex]]->pos;
/*
	if (index < 0) {
		FPoint2D t(99999.f, 999999.f);
		for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
			if ((from - (*i)->pos).Length() < (from - t).Length()) {
				t = (*i)->pos;
			}
		}
		return t;
	}
	FPoint2D toReal(99999.f, 999999.f);// test/debug only
	{
		for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
			if ((to - (*i)->pos).Length() < (to - toReal).Length()) {
				toReal = (*i)->pos;
			}
		}
	}
	if ((from - toReal).Length() < 10.f) {
		return from;
	}
	{
		if (_dots[index]->connections.size() == 0) { // debug test only
			return to;
		}
		FPoint2D to(toReal);
		FPoint2D t(_dots[*_dots[index]->connections.begin()]->pos);
		for (DotsIndex::iterator i = _dots[index]->connections.begin(), e = _dots[index]->connections.end(); i != e; ++i) {
			if ((to - _dots[(*i)]->pos).Length() < (to - t).Length()) { 
				t = _dots[(*i)]->pos;
			}
		}
		return t;
	}
	*/
}

FastIndex PersPaths::_fastIndex;
FastIndexHelp PersPaths::_indexDistance;

void PersPaths::Generate() {
	_fastIndex.clear();
	unsigned int n = _dots.size();
	_indexDistance = std::vector<std::vector<float> >(n, std::vector<float>(n, 999999.f));
	for (unsigned int i = 0; i < n; ++i) {
		_indexDistance[i][i] = 0.f;
		for (unsigned int j = i + 1; j < n; ++j) {
			_fastIndex = std::vector<std::vector<unsigned int> >(n, std::vector<unsigned int>(n, n + 1));
			_indexDistance[j][i] = _indexDistance[i][j] = Search(i, j, 0);
		}
	}
}

float PersPaths::Search(int i, int j, int n) {
	OneDot *a = _dots[i];
	OneDot *b = _dots[j];
	if (_fastIndex[i][j] < n) {
		return 999999.f;
	}
	_fastIndex[i][j] = n;
	_fastIndex[j][i] = n;
	unsigned int m = a->connections.size();
	for (unsigned int k = 0; k < m; ++k) {
		if (a->connections[k] == j) {
			return (a->pos - b->pos).Length();
		}
	}
	float min = 999999.f;
	for (unsigned int k = 0; k < m; ++k) {
		float d = (a->pos - _dots[a->connections[k]]->pos).Length() + Search(a->connections[k], j, n + 1);
		if (min > d) {
			min = d;
		}
	}
	return min;
}

FPoint2D PersPaths::RandomPos() {
	int r;
	do {
		r = rand() % _dots.size();
	} while (_dots[r]->connections.size() <= 2);
	return _dots[r]->pos;
}

FPoint2D PersPaths::SearchNearest(const FPoint2D &pos) {
	int min = 0;
	for (int i = 0, e = _dots.size(); i < e; ++i) {
		if ((pos - _dots[i]->pos).Length() < (pos - _dots[min]->pos).Length()) {
			min = i;
		}
	}
	return _dots[min]->pos;
}

