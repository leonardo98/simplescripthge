#ifndef SOLID_GROUND_LINE_H
#define SOLID_GROUND_LINE_H

#include "../Core/SplinePath.h"
#include "../Helpers/LittleHero.h"
#include "../Gamee/Byker.h"
#include "../Core/Render.h"

#define SCALE_BOX2D 60

class SolidGroundLine {

private:	
	
	SplinePath yPoses;// это сплайн флоатов - поле высот
	std::vector<float> xPoses;// это положение на экране контрольных точек 
							  // и постфактум распределение флоатов в сплайне

	std::vector<FPoint2D> lineDots;

	virtual void ExportToLines(std::vector<FPoint2D> &lineDots);
	virtual int SearchNearest(float x, float y);
	virtual bool CreateDot(float x, float y);
	virtual void RemoveDot(int index);
	virtual void ShiftDot(int index, float dx, float dy, bool moveAllDots);

	bool _mouseDown;
	FPoint2D _lastMousePos;
	Matrix parent;
	bool _moveAllDots;
	int _dotIndex;
	FPoint2D _downWorldPos;
	std::vector<float> _splineX;
	SplinePath _splineY;
	hgeRect _screenRect;
	void UpdateScreenRect();
	int _underMouse;
	Sprite *_sprite;
	Sprite *_small;

public:

	Lines * CreateBody(Byker *byker, int splineIndex);
	bool SearchProection(FPoint2D &pos);

	void DrawLines();
	void DebugDrawLines();

	void SaveToXml(TiXmlElement *xe);
	SolidGroundLine(TiXmlElement *xe);
	SolidGroundLine();
	~SolidGroundLine();
	void AddPoint(float x, float y);

	void MouseDown(const FPoint2D &mousePos);
	void MouseMove(const FPoint2D &mousePos);
	void MouseUp(const FPoint2D &mousePos);

	bool DoesContainPoint(const FPoint2D &mousePos);

	FPoint2D GetStart();
	FPoint2D GetFinish();
	std::string Type();

};

#endif//SOLID_GROUND_LINE_H