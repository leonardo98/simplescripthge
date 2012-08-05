#ifndef LEVELSET_H
#define LEVELSET_H

#include "../Core/Render.h"
#include "../Core/SplinePath.h"
#include "Byker.h"

#define SCALE_BOX2D 60

struct Beauty {
	HTEXTURE texture;
	Sprite *sprite;
	bool mirror;
	FPoint2D pos;
	float scale;
	float angle;
	std::string filePath;
	void SaveToXml(TiXmlElement *xe);
	void LoadFromXml(TiXmlElement *xe);
	void Draw();
};

struct LevelBlock {
	SplinePath xPoses;
	SplinePath yPoses;
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
	SplinePath splineX;
	SplinePath splineY;
	bool moveAllDots;
};

typedef std::vector<LevelBlock *> LevelBlocks;

struct LevelSet {
	LevelBlocks ground;
	LevelBlocks surpris;
	LevelBlocks movable;
	std::vector<Beauty> beauties;
	std::vector<OneImage> images;
	std::vector<FPoint2D> startpoint;
	std::vector<FPoint2D> endpoint;
	void Clear();
	void LoadFromXml(TiXmlElement *xe, bool gameMode);
};

#endif//LEVELSET_H