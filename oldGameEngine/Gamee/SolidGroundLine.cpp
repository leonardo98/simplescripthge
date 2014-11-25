#include "SolidGroundLine.h"
#include "../Core/Math.h"
#include "../Core/Core.h"

void SolidGroundLine::ExportToLines(std::vector<FPoint2D> &lineDots) {
	lineDots.clear();
	for (unsigned int i = 0; i < xPoses.size() - 1; ++i) {
		float minx = xPoses[i];
		float maxx = xPoses[i + 1];
		for (unsigned int k = 0; k < 6; ++k) {
			float x = minx + (maxx - minx) * k / 6.f;
			float y = yPoses.getFrame(i, k / 6.f);
			lineDots.push_back(FPoint2D(x, y));
		}
	}
	float x = xPoses.back();
	float y = yPoses.getGlobalFrame(1.f);
	lineDots.push_back(FPoint2D(x, y));
}

Lines * SolidGroundLine::CreateBody(Byker *byker, int splineIndex) {
	DotsList dots;
	ExportToLines(dots);
	for (unsigned int i = 0; i < dots.size(); ++i) {
		dots[i] = (dots[i] + _pos) * (1.f / SCALE_BOX2D);
		//dots[i] = (dots[i]  * (1.f / SCALE_BOX2D) + _pos);
		//dots[i] *= (1.f / SCALE_BOX2D);
	}
	return byker->physic.AddLinesSet(dots, splineIndex);
}

std::string SolidGroundLine::Type() {
	return "SolidGroundLine";
}

SolidGroundLine::SolidGroundLine(TiXmlElement* xe) 
: BeautyBase(xe)
{
	_color = Math::ReadColor(xe->Attribute("color"));
	std::vector<float> x;
	SplinePath y;
	TiXmlElement *dot = xe->FirstChildElement("dot");
	while (dot != NULL) {
		x.push_back(atof(dot->Attribute("x")));
		y.addKey(atof(dot->Attribute("y")));
		dot = dot->NextSiblingElement();
	}
	y.CalculateGradient(false);
	
	xPoses = x;
	yPoses = y;

}

FPoint2D SolidGroundLine::GetStart() {
	return (FPoint2D(xPoses.front(), yPoses.getGlobalFrame(0.f)) + _pos);// * (1.f / SCALE_BOX2D);
	//return (FPoint2D(xPoses.front(), yPoses.getGlobalFrame(0.f)));
	//return (FPoint2D(xPoses.front(), yPoses.getGlobalFrame(0.f)) * (1.f / SCALE_BOX2D) + _pos);
}

FPoint2D SolidGroundLine::GetFinish() {
	//return (FPoint2D(xPoses.back(), yPoses.getGlobalFrame(1.f)) * (1.f / SCALE_BOX2D) + _pos);
	//return (FPoint2D(xPoses.back(), yPoses.getGlobalFrame(1.f)));
	return (FPoint2D(xPoses.back(), yPoses.getGlobalFrame(1.f)) + _pos);// * (1.f / SCALE_BOX2D);
}

