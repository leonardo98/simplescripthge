#ifndef LEVELSET_H
#define LEVELSET_H

#include "../Core/Render.h"
#include "../Core/SplinePath.h"
#include "Byker.h"

#define SCALE_BOX2D 60

class SetItem {
public:
	enum Types{
		beauty_item = 0,
		destroyable_item = 1,
		funbush_item = 2,
		collectable_item = 3,
	};
	virtual ~SetItem();
	SetItem(Types type);
	SetItem(Types type, const char *fileName, const FPoint2D &pos, float angle);
	bool HasPixel(float x, float y);
	FPoint2D &Pos();
	float &Angle();
	virtual void SaveToXml(TiXmlElement *xe) const;
	virtual void LoadFromXml(TiXmlElement *xe);
	virtual void Draw() const;
	int GetDrawOrder() {return _drawOrder;}
	void SetDrawOrder(int order) {_drawOrder = order; }
	Types Type();
protected:
	const Types _type;
	HTEXTURE _texture;
	Sprite *_sprite;
	int _drawOrder;
	FPoint2D _pos;
	float _angle;
	std::string _filePath;
};

class Beauty : public SetItem{
public:
	Beauty();
	Beauty(const char *fileName, const FPoint2D &pos, float angle, float scale, bool mirror);
	virtual void SaveToXml(TiXmlElement *xe) const;
	virtual void LoadFromXml(TiXmlElement *xe);
	virtual void Draw() const;
private:
	bool _mirror;
	float _scale;
};

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