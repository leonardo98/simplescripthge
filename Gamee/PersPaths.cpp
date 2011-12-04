#include "PersPaths.h"
#include "../Core/Core.h"

AllDots PersPaths::_dots;

OneDot::OneDot(float x, float y) 
: pos(x, y)
{}

void PersPaths::SetPair(int one, int two) {
	_dots[one]->connections.push_back(_dots[two]);
	_dots[two]->connections.push_back(_dots[one]);
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
	TiXmlDocument doc((Render::GetDataDir() + "PathDots.xml").c_str());
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
			int sj = IndexOf((*i)->connections[k]);
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
			if ((*i)->pos.x >= (*i)->connections[k]->pos.x) {
				Render::Line((*i)->pos.x, (*i)->pos.y, (*i)->connections[k]->pos.x, (*i)->connections[k]->pos.y, 0xFFFF0000);
				++counter;
			}	
		}
	}
	counter = counter;
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		_texture->Render((*i)->pos.x - _texture->Width() / 2, (*i)->pos.y - _texture->Height() / 2);
	}
}

int PersPaths::Find(const FPoint2D &pos) {
	int counter = 0;
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		if ((pos - (*i)->pos).Length() < 16) {//_texture->Width()) {
			return counter;
		}
		++counter;
	}
	return -1;
}

void PersPaths::Remove(const FPoint2D &pos) {
	int index = Find(pos);
	for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		for (AllDots::iterator k = (*i)->connections.begin(); k != (*i)->connections.end(); ) {
			if ((*k) == _dots[index]) {
				k = (*i)->connections.erase(k);
			} else {
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
	int index = Find(from);
	if (index < 0) {
		FPoint2D t(99999.f, 999999.f);
		for (AllDots::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
			if ((from - (*i)->pos).Length() < (from - t).Length()) {
				t = (*i)->pos;
			}
		}
		return t;
	}
	FPoint2D t((*_dots[index]->connections.begin())->pos);
	for (AllDots::iterator i = _dots[index]->connections.begin(), e = _dots[index]->connections.end(); i != e; ++i) {
		//assert((_dots[index]->pos - (*i)->pos).Length() > 1);
		if ((from - (*i)->pos).Length() > 1.f && ((to - (*i)->pos).Length() + (from - (*i)->pos).Length()) < ((to - t).Length() + (from - t).Length())) { 
			t = (*i)->pos;
		}
	}
	if ((from - to).Length() < (t - to).Length()) {
//		return to;
	}
	return t;
}
