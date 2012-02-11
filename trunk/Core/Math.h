#ifndef MYENGINE_MATH_H
#define MYENGINE_MATH_H

#define fatof(a) static_cast<float>(atof(a))

#include "types.h"

class Math {
public:
	static int round(float a);
	static float random(float a, float b);
	static float Read(TiXmlElement *xe, const char *name, const float defaultValue);
	static bool Read(TiXmlElement *xe, const char *name, const bool defaultValue);
	static bool GetCirclesIntersect(float X1, float Y1, float R1, float X2, float Y2, float R2, FPoint2D &P1, FPoint2D &P2);
	static bool GetCirclesIntersect2(float x1, float y1, float r1, float x2, float y2, float r2, FPoint2D &q1, FPoint2D &q2);
	static float VMul(const FPoint2D &one, const FPoint2D &two);
	static float SMul(const FPoint2D &one, const FPoint2D &two);
	static FPoint2D lerp(const FPoint2D &one, const FPoint2D &two, float p);
	static float lerp(float one, float two, float p);
	static bool STrinagle(float a, float b, float c, float &s);
	static std::string IntToStr(int i);
};

#endif//MYENGINE_MATH_H