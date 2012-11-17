#include "LevelSet.h"
#include "../Core/Math.h"
#include "Beauty.h"
#include "ColoredPolygon.h"

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
	for (unsigned int i = 0; i < groundLines.size(); ++i) {
		delete groundLines[i];
	}
	groundLines.clear();
	//for (unsigned int i = 0; i < surpris.size(); ++i) {
	//	delete surpris[i];
	//}
	//surpris.clear();
	//for (unsigned int i = 0; i < movable.size(); ++i) {
	//	delete movable[i];
	//}
	//movable.clear();
	//for (unsigned int i = 0; i < images.size(); ++i) {
	//	delete images[i].sprite;
	//	Render::GetDC()->Texture_Free(images[i].texture);
	//}
	//images.clear();
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
		TiXmlElement *elem = groundXML->FirstChildElement("SolidGroundLine");
		while (elem != NULL) {
			SolidGroundLine *l = new SolidGroundLine(elem);
			groundLines.push_back(l);		
			elem = elem->NextSiblingElement("SolidGroundLine");
		}
	}
	{
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
				} else {
					assert(false);
				}
				beauties.push_back(beauty);
				elem = elem->NextSiblingElement();
			}
			elem = beautyList->FirstChildElement("ground");
			while (elem != NULL) {
				SolidGroundLine *gr = new SolidGroundLine(elem);
				groundLines.push_back(gr);
				elem = elem->NextSiblingElement("ground");
			}
		}
	}

//	TiXmlElement *word = xe->FirstChildElement("word");

}

