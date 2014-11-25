#include "LevelSet.h"
#include "../Core/Math.h"
#include "Beauty.h"
#include "ColoredPolygon.h"
#include "AnimationArea.h"

FPoint2D LevelSet::Startpoint() {
	if (_start.size()) {
		assert(_start.size() == 1);
		return _start.front();
	}
	FPoint2D s = groundLines[0]->GetStart();
	for (unsigned int i = 0; i < groundLines.size(); ++i) {
		if (groundLines[i]->GetStart().x < s.x) {
			s = groundLines[i]->GetStart();
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
	FPoint2D e = groundLines[0]->GetFinish();
	for (unsigned int i = 0; i < groundLines.size(); ++i) {
		if (e.x < groundLines[i]->GetFinish().x) {
			e = groundLines[i]->GetFinish();
		}
	}
	_end.push_back(e);
	return e;
}

void LevelSet::Clear() {
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		delete beauties[i];
	}
	beauties.clear();
	for (unsigned int i = 0; i < groundLines.size(); ++i) {
		delete groundLines[i];
	}
	groundLines.clear();
	_start.clear();
	_end.clear();
}

void LevelSet::LoadFromXml(TiXmlElement *xe, bool gameMode) {
	Clear();
	assert(xe != NULL);
	// level loading

	TiXmlElement *beautyList = xe->FirstChildElement("Beauties");
	if (beautyList) {
		TiXmlElement *elem = beautyList->FirstChildElement();
		std::string typeName;
		BeautyBase *beauty;
		while (elem != NULL) {
			typeName = elem->Value();// Attribute("type");
			if (typeName == "ColoredPolygon") {
				beauty = new ColoredPolygon(elem);
			} else if (typeName == "Beauty") {
				beauty = new Beauty(elem);
			} else if (typeName == "Animation") {
				beauty = new AnimationArea(elem);
			} else if (typeName == "GroundSpline") {
				beauty = new SolidGroundLine(elem);
			} else if (typeName == "SolidGroundLine") {
				beauty = new SolidGroundLine(elem);
			} else {
				assert(false);
			}
			if (typeName == "GroundSpline" || typeName == "SolidGroundLine") {
				groundLines.push_back((SolidGroundLine*)beauty);
			} else {
				beauties.push_back(beauty);
			}
			elem = elem->NextSiblingElement();
		}
	}
}

LevelSet::LevelSet(const LevelSet &l) {
	std::string typeName;
	BeautyBase *beauty;
	for (unsigned int i = 0; i < l.beauties.size(); ++i) {
		typeName = l.beauties[i]->Type();// Attribute("type");
		if (typeName == "ColoredPolygon") {
			beauty = new ColoredPolygon(*(ColoredPolygon *)(l.beauties[i]));
		} else if (typeName == "Beauty") {
			beauty = new Beauty(*(Beauty *)(l.beauties[i]));
		} else if (typeName == "Animation") {
			beauty = new AnimationArea(*(AnimationArea *)(l.beauties[i]));
		} else if (typeName == "SolidGroundLine") {
			beauty = new SolidGroundLine(*(SolidGroundLine *)(l.beauties[i]));
		} else {
			assert(false);
		}
		beauties.push_back(beauty);
	}
}

LevelSet::~LevelSet() {
	Clear();
}

const LevelSet &LevelSet::operator=(const LevelSet &l) {
	Clear();
	std::string typeName;
	BeautyBase *beauty;
	for (unsigned int i = 0; i < l.beauties.size(); ++i) {
		typeName = l.beauties[i]->Type();// Attribute("type");
		if (typeName == "ColoredPolygon") {
			beauty = new ColoredPolygon(*(ColoredPolygon *)(l.beauties[i]));
		} else if (typeName == "Beauty") {
			beauty = new Beauty(*(Beauty *)(l.beauties[i]));
		} else if (typeName == "Animation") {
			beauty = new AnimationArea(*(AnimationArea *)(l.beauties[i]));
		} else if (typeName == "SolidGroundLine") {
			beauty = new SolidGroundLine(*(SolidGroundLine *)(l.beauties[i]));
		} else {
			assert(false);
		}
		beauties.push_back(beauty);
	}
	return *this;
}
