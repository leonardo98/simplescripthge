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
	DWORD _color;
	void CalcWidthAndHeight();

	std::vector<FPoint2D> _dots; // точки для редактирования
	std::vector<FPoint2D> _screenDots; 
	std::vector<hgeTriple> _triangles;

	bool GenerateTriangles();
	void FillTriangle(const FPoint2D &a, const FPoint2D &b, const FPoint2D &c, hgeTriple &tri);
	void DrawTriangles();

public:

	ColoredPolygon(TiXmlElement *xe);

	virtual void Draw();
	virtual void Update(float dt) {}
	virtual bool PixelCheck(const FPoint2D &point);

	virtual std::string Type();

	virtual int Width();
	virtual int Height();

};


#endif//COLRED_POLYGON_H