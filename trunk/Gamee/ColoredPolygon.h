#ifndef COLRED_POLYGON_H
#define COLRED_POLYGON_H

#include "BeautyBase.h"

//
// Украшение
// Закрашеный многоугольник
//

class ColoredPolygon : public BeautyBase
{
private:

	int _width;
	int _height;
	bool _needUpdate;
	void CalcWidthAndHeight();

	std::vector<FPoint2D> _dots; // точки для редактирования
	std::vector<hgeTriple> _triangles;

	bool GenerateTriangles();

	int SearchNearest(float x, float y);

	bool CreateDot(float x, float y);
	void RemoveDot(int index);

public:

	ColoredPolygon(TiXmlElement *xe);

	virtual void Draw();
	virtual void DebugDraw();
	virtual void Update(float dt) {}
	virtual void MouseDown(FPoint2D mouse);
	virtual void MouseMove(FPoint2D mouse);
	virtual void MouseUp(FPoint2D mouse);
	virtual bool PixelCheck(FPoint2D point);

	virtual void SaveToXml(TiXmlElement *xe);
	virtual std::string Type();

	virtual int Width();
	virtual int Height();

};


#endif//COLRED_POLYGON_H