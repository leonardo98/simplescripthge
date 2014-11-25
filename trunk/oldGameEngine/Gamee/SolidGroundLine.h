#ifndef SOLID_GROUND_LINE_H
#define SOLID_GROUND_LINE_H

#include "../Core/SplinePath.h"
#include "../Helpers/LittleHero.h"
#include "../Gamee/Byker.h"
#include "../Core/Render.h"
#include "BeautyBase.h"

#define SCALE_BOX2D 60

class SolidGroundLine : public BeautyBase
{

private:	
	
	SplinePath yPoses;// это сплайн флоатов - поле высот
	std::vector<float> xPoses;// это положение на экране контрольных точек 
							  // и постфактум распределение флоатов в сплайне

	void ExportToLines(std::vector<FPoint2D> &lineDots);

public:

	SolidGroundLine(TiXmlElement *xe);

	virtual std::string Type();

	Lines * CreateBody(Byker *byker, int splineIndex);
	bool SearchProection(FPoint2D &pos);

	FPoint2D GetStart();
	FPoint2D GetFinish();

};

#endif//SOLID_GROUND_LINE_H