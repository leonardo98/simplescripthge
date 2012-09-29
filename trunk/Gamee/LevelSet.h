#ifndef LEVELSET_H
#define LEVELSET_H

#include "../Core/Render.h"
#include "../Core/SplinePath.h"
#include "Byker.h"
#include "SetItem.h"

#define SCALE_BOX2D 60

struct LevelBlock {
	SplinePath yPoses;// это сплайн флоатов - поле высот
	std::vector<float> xPoses;// это положение на экране контрольных точек 
							  // и постфактум распределение флоатов в сплайне
	std::vector<FPoint2D> lineDots;
	std::vector<hgeTriple> triangles;
	void ExportToLines(std::vector<FPoint2D> &lineDots);
	static void FillTriangle(const FPoint2D &a, const FPoint2D &b, const FPoint2D &c, hgeTriple &tri);
	void GenerateTriangles();
	void DrawLines();
	void DrawTriangles();
	void AddPoint(float x, float y);
	int SearchNearest(float x, float y);
	bool CreateDot(float x, float y);
	void RemoveDot(int index);
	bool SearchProection(FPoint2D &pos);
	Lines * CreateBody(Byker *byker, int splineIndex);
	bool DotNearLine(const FPoint2D &one, const FPoint2D &two, const FPoint2D &p);
};

struct OneImage {
	HTEXTURE texture;
	Sprite *sprite;
	FPoint2D pos;
	std::string filePath;
};

struct CurrentBlock {
	LevelBlock *block;
	int dotIndex;
	float downX;
	float downY;
	std::vector<float> splineX;
	SplinePath splineY;//fake
	bool moveAllDots;
};

typedef std::vector<LevelBlock *> LevelBlocks;

struct LevelSet {
	LevelBlocks ground;
	LevelBlocks surpris;
	LevelBlocks movable;
	std::vector<SetItem *> beauties;
	std::vector<OneImage> images;
	FPoint2D Startpoint();
	FPoint2D Endpoint();
	void Clear();
	void LoadFromXml(TiXmlElement *xe, bool gameMode);
private:
	std::vector<FPoint2D> _start;
	std::vector<FPoint2D> _end;
};

#endif//LEVELSET_H