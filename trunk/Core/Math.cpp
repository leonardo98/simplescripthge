#include "Math.h"

int Math::round(float a) {
	int b = static_cast<int>(a);
	return (a - b) >= 0.5f ? b + 1 : b;
} 
float Math::random(float a, float b) {
	return a + (b - a) * rand() / RAND_MAX;
} 
float Math::Read(TiXmlElement *xe, const char *name, const float defaultValue) {
	const char *tmp = xe->Attribute(name);
	return (tmp ? atof(tmp) : defaultValue);
}

bool Math::Read(TiXmlElement *xe, const char *name, const bool defaultValue) {
	const char *tmp = xe->Attribute(name);
	return (tmp ? std::string(tmp) == "true" : defaultValue);
}

bool Math::GetCirclesIntersect(float X1, float Y1, float R1, float X2, float Y2, float R2, FPoint2D &P1, FPoint2D &P2)
{

	if (FPoint2D(X1 - X2, Y1 - Y2).Length() >= R1 + R2) {
		return false;
	}
	// ���� ����������� ��������� ������ � Near1.
	float  C1 = R1 * R1 - R2 * R2 + Y2 * Y2 - (X2 - X1) * (X2 - X1) - Y1 * Y1,  // ���������� ���������.
		C2 = X2 - X1,                                                        // ���������� ���������.
		C3 = Y2 - Y1,                                                        // ���������� ���������.
		a  = -4 * C2 * C2 - 4 * C3 * C3,
		b  = (8 * C2 * C2 * Y2 + 4 * C1 * C3) / a,
		c  = (4 * C2 * C2 * R2 * R2 - 4 * C2 * C2 * Y2 * Y2 - C1 * C1) / a,
		X_1_1 = 0, X_1_2 = 0, Y_1 = 0, X_2_1 = 0, X_2_2 = 0, Y_2 = 0,
		Leng1, Leng2;
	a = 1;
	float  sqrtVal = b * b - 4 * a * c;
	if (fabs(sqrtVal) < 0.5f) {
		sqrtVal = 0.f;
	}
	if ( sqrtVal < 0  ||  a == 0 )
	{
		assert(false);
		if ( X1 == X2  &&  Y1 == Y2  &&  R1 == R2 )  // ���� ���������� ���������, �� ����� �������, ��� ����� ���:
		{
			P1.x = X1 - R1;
			P1.y = Y1;
			P2.x = X1 + R1;
			P2.y = Y1;
			return true;
		}
		return false;
	}

	Y_1 = (-b + sqrt(sqrtVal)) / (2 * a);
	Y_2 = (-b - sqrt(sqrtVal)) / (2 * a);
	float sqrtVal2 = R1 * R1 - (Y_1 - Y1) * (Y_1 - Y1);
	if (fabs(sqrtVal2) < 0.5f) {
		sqrtVal2 = 0.f;
	}
	X_1_1 = X1 + sqrt(sqrtVal2);
	X_1_2 = X1 - sqrt(sqrtVal2);
	float sqrtVal3 = R2 * R2 - (Y_2 - Y2) * (Y_2 - Y2);
	if (fabs(sqrtVal3) < 0.5f) {
		sqrtVal3 = 0.f;
	}
	X_2_1 = X2 + sqrt(sqrtVal3);
	X_2_2 = X2 - sqrt(sqrtVal3);

	//P1.y = Y_1;
	//P2.y = Y_2;
	int counter = 0;
	if (fabs(FPoint2D(X2 - X_1_1, Y2 - Y_1).Length() - R2) < 1) {
		P1.x = X_1_1;
		P1.y = Y_1;
		counter = 1;
	}
	if (fabs(FPoint2D(X2 - X_1_2, Y2 - Y_1).Length() - R2) < 1) {
		if (counter == 0) {
			P1.x = X_1_2;
			P1.y = Y_1;
			counter = 1;
		} else {
			P2.x = X_1_2;
			P2.y = Y_1;
			return true;
		}
	}
	if (fabs(FPoint2D(X1 - X_2_1, Y1 - Y_2).Length() - R1) < 1) {
		if (counter == 0) {
			P1.x = X_2_1;
			P1.y = Y_2;
			counter = 1;
		} else {
			P2.x = X_2_1;
			P2.y = Y_2;
			return true;
		}
	}
	if (fabs(FPoint2D(X1 - X_2_2, Y1 - Y_2).Length() - R1) < 1) {
		if (counter == 0) {
			P1.x = X_2_2;
			P1.y = Y_2;
			counter = 1;
		} else {
			P2.x = X_2_2;
			P2.y = Y_2;
			return true;
		}
	}
	assert(false);
	return true;
}

float Math::VMul(const FPoint2D &one, const FPoint2D &two) {
	return (one.x * two.y - one.y * two.x);
}

float Math::SMul(const FPoint2D &one, const FPoint2D &two) {
	return (one.x * two.x + one.y * two.y) / (one.Length() * two.Length());
}

bool Math::STrinagle(float a, float b, float c, float &s) {
	float p = (a + b + c) / 2;
	float underRoot = p * (p - a) * (p - b) * (p - c);
	if (underRoot > 0.f) {
		s = sqrt(underRoot);
		return true;
	}
	s = -1.f;
	return false;
}

bool Math::GetCirclesIntersect2(float x1, float y1, float r1, float x2, float y2, float r2, FPoint2D &q1, FPoint2D &q2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	float d = sqrt(dx * dx + dy * dy);
	float sTriangle;
	if (!STrinagle(d, r1, r2, sTriangle)) {
		return false;
	}
	float h = 2 * sTriangle / d;
	float t = sqrt(r1 * r1 - h * h);
	FPoint2D o(dx / d * t, dy / d * t);
	FPoint2D po(o.y, -o.x);
	o.x += x1;
	o.y += y1;
	po = po / po.Length() * h;
	q1 = o + po;
	q2 = o - po;
	return true;
}

std::string Math::IntToStr(int i) {
	char buff[10];
	sprintf(buff, "%i", i);
	return buff;
}

FPoint2D Math::lerp(const FPoint2D &one, const FPoint2D &two, float p) {
	return FPoint2D((two.x - one.x) * p + one.x, (two.y - one.y) * p + one.y);
}
