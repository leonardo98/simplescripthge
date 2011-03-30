// StarsRect.cpp: implementation of the StarsRect class.
//
//////////////////////////////////////////////////////////////////////

#include "StarsRect.h"
#include "..\Core\Core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StarsRect::StarsRect(TiXmlElement *xe)
{
	_pos.x = atoi(xe->Attribute("x"));
	_pos.y = atoi(xe->Attribute("y"));
	_width = atoi(xe->Attribute("width"));
	_height = atoi(xe->Attribute("height"));
	_number = atoi(xe->Attribute("number"));
	std::string texture = xe->Attribute("texture"); 
	_texture = Core::getTexture(texture);
	_texture->SetBlendMode(BLEND_NOZWRITE);
	for (unsigned int i = 0; i < _number; i++ ) {
		_stars.push_back(Star(_pos, _width, _height));
		_stars.back().pos += (rand() / 32768.f);
	}
}

StarsRect::~StarsRect()
{
}

void StarsRect::Draw()
{
	for (std::list<Star>::iterator i = _stars.begin(), e = _stars.end(); i != e; i++) {
		_texture->Render(i->getPos());
	}
}

void StarsRect::Update(float deltaTime)
{
	for (std::list<Star>::iterator i = _stars.begin(), e = _stars.end(); i != e; i++) {
		if (i->pos < 1.f) {
			i->Update(deltaTime);
		} else {
			*i = Star(_pos, _width, _height);
		}
	}
}

StarsRect::Star::Star(FPoint2D pos, int width, int height) {
	start = pos + FPoint2D(width * (rand() / 32768.f), height * (rand() / 32768.f));
	end = pos + FPoint2D(width * (rand() / 32768.f), height * (rand() / 32768.f));
	if (rand() % 2 == 1) {
		start.x = pos.x + width * (rand() % 2);
		end.x = pos.x + width * (rand() % 2);
	} else {
		start.y = pos.y + height * (rand() % 2);
		end.y = pos.y + height * (rand() % 2);
	}
	this->pos = 0;
	speed = 5.f + (rand() / 32768.f) * 15.f;
}

void StarsRect::Star::Update(float deltaTime) {
	pos += speed * deltaTime / (end - start).Length();
}

FPoint2D StarsRect::Star::getPos() {
	return start + (end - start) * pos;
}
