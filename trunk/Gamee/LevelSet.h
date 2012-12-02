#ifndef LEVELSET_H
#define LEVELSET_H

#include "../Core/Render.h"
#include "../Core/SplinePath.h"
#include "Byker.h"
#include "SetItem.h"
#include "BeautyBase.h"
#include "SolidGroundLine.h"

struct OneImage {
	HTEXTURE texture;
	Sprite *sprite;
	FPoint2D pos;
	std::string filePath;
};

struct CurrentBlock {
	BeautyBase *beauty;
	SolidGroundLine *line;
};

typedef std::vector<SolidGroundLine *> LevelGroundLines;
typedef std::vector<BeautyBase *> BeautyList;

struct LevelSet {
	LevelGroundLines groundLines;
	//LevelBlocks surpris;
	//LevelBlocks movable;
	BeautyList beauties;
	//std::vector<OneImage> images;
	FPoint2D Startpoint();
	FPoint2D Endpoint();
	void Clear();
	void LoadFromXml(TiXmlElement *xe, bool gameMode);
	const LevelSet &operator=(const LevelSet &l);
	LevelSet(const LevelSet &l);
	~LevelSet();
	LevelSet() {}
private:
	std::vector<FPoint2D> _start;
	std::vector<FPoint2D> _end;
};

#endif//LEVELSET_H