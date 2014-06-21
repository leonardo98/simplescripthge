#include "skybox_vector.h"

namespace SkyBox
{

extern __forceinline void MxM(float *m1, float *m2, float *r)
{
	r[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
	r[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
	r[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
	r[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
	r[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
	r[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
	r[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
	r[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
	r[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
	r[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
	r[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
	r[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
	r[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
	r[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
	r[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
	r[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
}

extern __forceinline float Dot(const vector &a, const vector &b)
{
	return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
}

extern __forceinline vector Cross(const vector &a, const vector &b)
{
	float temp[3];
	temp[0] = a.v[1] * b.v[2] - a.v[2] * b.v[1];
	temp[1] = a.v[2] * b.v[0] - a.v[0] * b.v[2];
	temp[2] = a.v[0] * b.v[1] - a.v[1] * b.v[0];
	return *(vector *)temp;
}

extern __forceinline vector Average(const vector &a, const vector &b)
{
	float temp[3];
	temp[0] = (a.v[0] + b.v[0]) * .5f;
	temp[1] = (a.v[1] + b.v[1]) * .5f;
	temp[2] = (a.v[2] + b.v[2]) * .5f;
	return *(vector *)temp;
}

extern __forceinline vector VectorSet(const float &a, const float &b, const float &c)
{
	float temp[3];
	temp[0] = a; temp[1] = b; temp[2] = c;
	return *(vector *)temp;
}

extern __forceinline vector NormalVec(vector a)
{
	float invlen = 1 / sqrtf(a.v[0] * a.v[0] + a.v[1] * a.v[1] + a.v[2] * a.v[2]);
	a.v[0] *= invlen;
	a.v[1] *= invlen;
	a.v[2] *= invlen;
	return a;
/*	vector *b = &a;
	__asm
	{
		MOV EDX, [b] // EDX = v
		FEMMS // clear mmx

		MOVQ MM0, [EDX] // MM0 = v[0] | v[1]
		MOVD MM1, [EDX + 8] // MM1 = 0 | v[2]
		MOVQ MM2, MM0 // MM2 = v[0] | v[1]
		MOVQ MM3, MM1 // MM3 = 0 | v[2]
		PFMUL MM0, MM0 // MM0 = v[0] * v[0] | v[1] * v[1]
		PFMUL MM1, MM1 // MM1 = 0 | v[2] * v[2]
		PFADD MM1, MM0 // MM1 = v[0] * v[0] | v[1] * v[1] + v[2] * v[2]
		PUNPCKHDQ MM0, MM0 // MM0 = v[0] * v[0] | v[0] * v[0]
		PFADD MM0, MM1 // MM0 = v[0] * v[0] + v[0] * v[0] | v[0] * v[0] + v[1] * v[1] + v[2] * v[2]
		PUNPCKLDQ MM0, MM0
		PFRSQRT MM4, MM0 // MM0 = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])
		MOVQ MM5, MM4
		PFMUL MM4, MM4
		PFRSQIT1 MM4, MM0
		PFRCPIT2 MM4, MM5
		PFMUL MM2, MM4 // MM2 = normalized v[0] | v[1]
		PFMUL MM3, MM4 // MM3 = normalized 0 | v[2]
		MOVQ [EDX], MM2 // v[0], v[1] = normalized v[0] | v[1]
		MOVD [EDX + 8], MM3 // v[2] = normalized v[2]

		FEMMS // clear mmx
	}
	return a;*/
}

extern __forceinline vector ZeroNormalVec(vector a)
{
	float invlen = sqrtf(a.v[0] * a.v[0] + a.v[1] * a.v[1] + a.v[2] * a.v[2]);
	if (invlen == 0)
		return a;
	invlen = 1 / invlen;
	a.v[0] *= invlen;
	a.v[1] *= invlen;
	a.v[2] *= invlen;
	return a;
/*	vector *b = &a;
	__asm
	{
		MOV EDX, [b] // EDX = v
		FEMMS // clear mmx

		MOVQ MM0, [EDX] // MM0 = v[0] | v[1]
		MOVD MM1, [EDX + 8] // MM1 = 0 | v[2]
		MOVQ MM2, MM0 // MM2 = v[0] | v[1]
		MOVQ MM3, MM1 // MM3 = 0 | v[2]
		PFMUL MM0, MM0 // MM0 = v[0] * v[0] | v[1] * v[1]
		PFMUL MM1, MM1 // MM1 = 0 | v[2] * v[2]
		PFADD MM1, MM0 // MM1 = v[0] * v[0] | v[1] * v[1] + v[2] * v[2]
		PUNPCKHDQ MM0, MM0 // MM0 = v[0] * v[0] | v[0] * v[0]
		PFADD MM0, MM1 // MM0 = v[0] * v[0] + v[0] * v[0] | v[0] * v[0] + v[1] * v[1] + v[2] * v[2]
		PUNPCKLDQ MM0, MM0
		PFRSQRT MM4, MM0 // MM0 = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])
		MOVQ MM5, MM4
		PFMUL MM4, MM4
		PFRSQIT1 MM4, MM0
		PFRCPIT2 MM4, MM5
		PFMUL MM2, MM4 // MM2 = normalized v[0] | v[1]
		PFMUL MM3, MM4 // MM3 = normalized 0 | v[2]
		MOVQ [EDX], MM2 // v[0], v[1] = normalized v[0] | v[1]
		MOVD [EDX + 8], MM3 // v[2] = normalized v[2]

		FEMMS // clear mmx
	}
	return a;*/
}

extern __forceinline float PlaneDistance(vector Normal, vector Point)
{	
	float distance;
	distance = -Dot(Normal, Point);
	return distance;
}

extern __forceinline bool IntersectedPlane(vector vPoly[], vector vLine[], vector &vNormal, float &originDistance)
{
	// pre-computer normals to speed up
	float distance1, distance2;
	vector Angle1(vPoly[2], vPoly[0]);
	vector Angle2(vPoly[1], vPoly[0]);
	vNormal = Cross(Angle1, Angle2);
	vNormal.normalize();
	originDistance = PlaneDistance(vNormal, vPoly[0]);
	distance1 = Dot(vNormal, vLine[0]) + originDistance;
	distance2 = Dot(vNormal, vLine[1]) + originDistance;
	int c1,c2;
	if (distance1 > 0)
		c1 = 1;
	else
		if (distance1 < -0)
			c1 = -1;
		else
			c1 = 0;
		if (distance2 > 0)
			c2 = 1;
		else
			if (distance2 < -0)
				c2 = -1;
			else
				c2 = 0;
			if (c1 == c2)
				return 0;
			return 1;
}

extern __forceinline float AngleBetweenVectors(vector Vector1, vector Vector2)
{							
	float dotProduct = Dot(Vector1, Vector2);				
	float vectorsMagnitude = Vector1.length() * Vector2.length();
	return acosf(dotProduct / vectorsMagnitude);
}

extern __forceinline vector IntersectionPoint(vector vNormal, vector vLine[], float distance)
{
	vector vPoint;
	vector vLineDir(vLine[1], vLine[0]);
	float Numerator = 0, Denominator = 0, dist = 0;
	vLineDir.normalize();
	Numerator = -(Dot(vNormal, vLine[0]) + distance);
	Denominator = Dot(vNormal, vLineDir);				  
	if (Denominator == 0)
		return vLine[0];
	dist = Numerator / Denominator;	
	vPoint = vLine[0] + (vLineDir * dist);
	return vPoint;
}

extern __forceinline bool InsidePolygon(vector vIntersection, vector Poly[], int verticeCount)
{
	float Angle = 0;
	for (int i = 0; i < verticeCount; i++)
	{	
		vector vA(Poly[i], vIntersection);
		vector vB(Poly[(i+1) % verticeCount], vIntersection);
		Angle += AngleBetweenVectors(vA, vB);
	}
	if (Angle >= (.95 * (2.0 * PI)) )
		return 1;
	return 0;
}

extern __forceinline bool IntersectedPolygon(vector vPoly[], vector vLine[], int verticeCount, vector &ipoint)
{
	vector vNormal;
	float originDistance = 0;
	if (!IntersectedPlane(vPoly, vLine, vNormal, originDistance))
		return 0;
	vector vIntersection = IntersectionPoint(vNormal, vLine, originDistance);
	ipoint = vIntersection;
	if (InsidePolygon(vIntersection, vPoly, verticeCount))
		return 1;
	return 0;
}

extern __forceinline float IntersectRayPlane(vector rOrigin, vector rVector, vector pOrigin, vector pNormal)
{
	float d = - (Dot(pNormal, pOrigin));

	float numer = Dot(pNormal, rOrigin) + d;
	float denom = Dot(pNormal, rVector);

	if (denom == 0)
		return -1.0f;

	return -numer / denom;
}

extern __forceinline float IntersectRaySphere2(vector rO, vector rV, vector sO, float sR)
{
	vector Q = sO - rO;

	float c = Q.length();
	float v = Dot(Q, rV);
	float d = sR * sR - (c * c - v * v);

	if (d < 0.0)
		return -1.0f;
	return v - sqrtf(d);
}

extern __forceinline int IntersectRaySphere(vector ro, vector rv, vector so, float r, vector &iptnear, vector &iptfar)
{
	vector sr = ro - so;
	float a, b, c, rvlen;
	a = sr.v[0] * sr.v[0] + sr.v[1] * sr.v[1] + sr.v[2] * sr.v[2];
	b = r * r;
	// for a true/false only with no intersection points, add distance checks here
	if (a < b)
		return 2;
	c = a - b;
	a = rv.v[0] * rv.v[0] + rv.v[1] * rv.v[1] + rv.v[2] * rv.v[2];
	rvlen = sqrtf(a);
//	b = 2 * (rv.v[0] * (ro.v[0] - so.v[0]) + rv.v[1] * (ro.v[1] - so.v[1]) + rv.v[2] * (ro.v[2] - so.v[2]));
	b = 2 * (rv.v[0] * sr.v[0] + rv.v[1] * sr.v[1] + rv.v[2] * sr.v[2]);

	float t1, t2;
//	t1 = (-b + sqrtf(b * b - 4 * a * c)) / (2 * a);
	t1 = b * b - 4 * a * c;
	if (t1 < 0)
		return false;
	t1 = sqrtf(t1);
	t2 = (-b - t1) / (2 * a);
	t1 = (-b + t1) / (2 * a);
	if (t1 < 0)
	{
		if (fabsf(t2) > rvlen)
			return false;
		iptnear = ro + rv * t2;
		iptfar = iptnear;
	}
	else if (t2 < 0)
	{
		if (t1 > rvlen)
			return false;
		iptnear = ro + rv * t1;
		iptfar = iptnear;
	}
	else if (t1 < t2)
	{
		if (t1 > rvlen)
			return false;
		iptnear = ro + rv * t1;
		if (t2 > rvlen)
			iptfar = iptnear;
		else
			iptfar = ro + rv * t2;
	}
	else
	{
		if (t2 > rvlen)
			return false;
		iptnear = ro + rv * t2;
		if (t1 > rvlen)
			iptfar = iptnear;
		else
			iptfar = ro + rv * t1;
	}
	return true;
}

extern __forceinline bool CheckPointInTriangle3(float *point, float *normal, float *triangle)
{
	float u0, v0, u1, v1, u2, v2, beta, alpha;
	long i1, i2;

	if((normal[1] > normal[0]) && (normal[1] > normal[2]))
	{
		i1 = 0;
		i2 = 2;
	}
	else
	{
		i1 = 0;
		i2 = 1;
	}
	
	u0 = point[i1] - triangle[i1];
	v0 = point[i2] - triangle[i2];
	u1 = triangle[3 + i1] - triangle[i1];
	v1 = triangle[3 + i2] - triangle[i2];
	u2 = triangle[6 + i1] - triangle[i1];
	v2 = triangle[6 + i2] - triangle[i2];
	if (u1 == 0.0)
	{
		beta = u0 / u2;
		if ((0.0 <= beta) && (beta <= 1.0))
		{
			alpha = (v0 - beta * v2) / v1;
		}
	}
	else
	{
		beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
		if ((0.0 <= beta) && (beta <= 1.0))
			alpha = (u0 - beta * u2) / u1;
	}
	
	return (alpha >= 0.0) && (beta >= 0.0) && (alpha + beta <= 1.0);
}

extern __forceinline bool CheckPointInTriangle2(vector &point, vector &a, vector &b, vector &c)
{
	vector e1, e2, e3, e4, e5, e6, e7, e8, e9;
	e1 = (b - a).normalize();
	e2 = (c - b).normalize();
	e3 = (a - c).normalize();
	e4 = (c - a).normalize();
	e5 = (a - b).normalize();
	e6 = (b - c).normalize();
	e7 = (point - a).normalize();
	e8 = (point - b).normalize();
	e9 = (point - c).normalize();

	float dot1a = Dot(e1, e4);
	float dot2a = Dot(e2, e5);
	float dot3a = Dot(e3, e6);
	float dot1b = Dot(e1, e7);
	float dot2b = Dot(e2, e8);
	float dot3b = Dot(e3, e9);
	float dot1c = Dot(e4, e7);
	float dot2c = Dot(e5, e8);
	float dot3c = Dot(e6, e9);
	if (dot1c < dot1b)
		dot1b = dot1c;
	if (dot2c < dot2b)
		dot2b = dot2c;
	if (dot3c < dot3b)
		dot3b = dot3c;
	if (dot1b > dot1a && dot2b > dot2a && dot3b > dot3a)
		return true;
	return false;
}

extern __forceinline bool CheckPointInTriangle(vector &point, vector &a, vector &b, vector &c)
{
	float total_angles = 0.0f;

	vector v1 = point - a;
	vector v2 = point - b;
	vector v3 = point - c;

	v1.normalize();
	v2.normalize();
	v3.normalize();

	total_angles += acosf(Dot(v1, v2));
	total_angles += acosf(Dot(v2, v3));
	total_angles += acosf(Dot(v3, v1));
	
	if (fabsf(total_angles - 2.0f * PI) <= 0.005f)
		return true;

	return false;
}

extern __forceinline bool CheckPointInTriangle4(float *p, float *t)
{
	float v[9];
	v[0] = p[0] - t[0];
	v[1] = p[1] - t[1];
	v[2] = p[2] - t[2];
	v[3] = p[0] - t[3];
	v[4] = p[1] - t[4];
	v[5] = p[2] - t[5];
	v[6] = p[0] - t[6];
	v[7] = p[1] - t[7];
	v[8] = p[2] - t[8];

	float invlen = 1 / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= invlen;
	v[1] *= invlen;
	v[2] *= invlen;
	invlen = 1 / sqrtf(v[3] * v[3] + v[4] * v[4] + v[5] * v[5]);
	v[3] *= invlen;
	v[4] *= invlen;
	v[5] *= invlen;
	invlen = 1 / sqrtf(v[6] * v[6] + v[7] * v[7] + v[8] * v[8]);
	v[6] *= invlen;
	v[7] *= invlen;
	v[8] *= invlen;

	if (fabsf(acosf(v[0] * v[3] + v[1] * v[4] + v[2] * v[5])
		+ acosf(v[3] * v[6] + v[4] * v[7] + v[5] * v[8])
		+ acosf(v[6] * v[0] + v[7] * v[1] + v[8] * v[2]) - TWOPI) <= 0.005f)
		return true;

	return false;
}

extern __forceinline vector ClosestPointOnLine(vector &a, vector &b, vector &p)
{
	vector c = p - a;
	vector V = b - a;

	float d = V.length();

//	V.normalize();
	V *= 1 / d;

	float t = Dot(V, c);

	if (t < 0.0f)
		return a;
	if (t > d)
		return b;

	V.v[0] = V.v[0] * t;
	V.v[1] = V.v[1] * t;
	V.v[2] = V.v[2] * t;

	return a + V;
}

extern __forceinline vector ClosestPointOnTriangle(vector a, vector b, vector c, vector p)
{
	vector Rab = ClosestPointOnLine(a, b, p);
	vector Rbc = ClosestPointOnLine(b, c, p);
	vector Rca = ClosestPointOnLine(c, a, p);

	float dAB = (p - Rab).length();
	float dBC = (p - Rbc).length();
	float dCA = (p - Rca).length();

	float min = dAB;
	vector result = Rab;
	
	if (dBC < min)
	{
		min = dBC;
		result = Rbc;
	}

	if (dCA < min)
		result = Rca;

	return result;
}

extern __forceinline bool CheckPointInSphere(vector point, vector sO, float sR)
{
	float d = (point - sO).length();

	if(d <= sR)
		return true;
	return false;
}

extern __forceinline vector TangentPlaneNormalOfEllipsoid(vector point, vector eO, vector eR)
{
	vector p = point - eO;

	float a2 = eR.v[0] * eR.v[0];
	float b2 = eR.v[1] * eR.v[1];
	float c2 = eR.v[2] * eR.v[2];

	vector res;
	res.v[0] = p.v[0] / a2;
	res.v[1] = p.v[1] / b2;
	res.v[2] = p.v[2] / c2;

	res.normalize();
	return res;
}

extern __forceinline int ClassifyPoint(vector point, vector pO, vector pN)
{
	vector dir = pO - point;
	float d = Dot(dir, pN);

	if (d < -0.001f)
		return 1;
	else if (d > 0.001f)
		return -1;

	return 0;
}

extern __forceinline float PointTriangleDistance(vector a, vector b, vector c, vector p)
{
	vector kDiff = a - p;
	vector e0, e1;
	e0 = b - a;
	e1 = c - a;
	float fA00 = e0.length2();
	float fA01 = Dot(e0, e1);
	float fA11 = e1.length2();
	float fB0 = Dot(kDiff, e0);
	float fB1 = Dot(kDiff, e1);
	float fC = kDiff.length2();
	float fDet = fabsf(fA00 * fA11 - fA01 * fA01);
	float fS = fA01 * fB1 - fA11 * fB0;
    float fT = fA01*fB0-fA00*fB1;
    float fSqrDist;

    if ( fS + fT <= fDet )
    {
        if ( fS < (float)0.0 )
        {
            if ( fT < (float)0.0 )  // region 4
            {
                if ( fB0 < (float)0.0 )
                {
                    fT = (float)0.0;
                    if ( -fB0 >= fA00 )
                    {
                        fS = (float)1.0;
                        fSqrDist = fA00+((float)2.0)*fB0+fC;
                    }
                    else
                    {
                        fS = -fB0/fA00;
                        fSqrDist = fB0*fS+fC;
                    }
                }
                else
                {
                    fS = (float)0.0;
                    if ( fB1 >= (float)0.0 )
                    {
                        fT = (float)0.0;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= fA11 )
                    {
                        fT = (float)1.0;
                        fSqrDist = fA11+((float)2.0)*fB1+fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDist = fB1*fT+fC;
                    }
                }
            }
            else  // region 3
            {
                fS = (float)0.0;
                if ( fB1 >= (float)0.0 )
                {
                    fT = (float)0.0;
                    fSqrDist = fC;
                }
                else if ( -fB1 >= fA11 )
                {
                    fT = (float)1.0;
                    fSqrDist = fA11+((float)2.0)*fB1+fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDist = fB1*fT+fC;
                }
            }
        }
        else if ( fT < (float)0.0 )  // region 5
        {
            fT = (float)0.0;
            if ( fB0 >= (float)0.0 )
            {
                fS = (float)0.0;
                fSqrDist = fC;
            }
            else if ( -fB0 >= fA00 )
            {
                fS = (float)1.0;
                fSqrDist = fA00+((float)2.0)*fB0+fC;
            }
            else
            {
                fS = -fB0/fA00;
                fSqrDist = fB0*fS+fC;
            }
        }
        else  // region 0
        {
            // minimum at interior point
            float fInvDet = ((float)1.0)/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) +
                fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
        }
    }
    else
    {
        float fTmp0, fTmp1, fNumer, fDenom;

        if ( fS < (float)0.0 )  // region 2
        {
            fTmp0 = fA01 + fB0;
            fTmp1 = fA11 + fB1;
            if ( fTmp1 > fTmp0 )
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-2.0f*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fS = (float)1.0;
                    fT = (float)0.0;
                    fSqrDist = fA00+((float)2.0)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = (float)1.0 - fS;
                    fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
                }
            }
            else
            {
                fS = (float)0.0;
                if ( fTmp1 <= (float)0.0 )
                {
                    fT = (float)1.0;
                    fSqrDist = fA11+((float)2.0)*fB1+fC;
                }
                else if ( fB1 >= (float)0.0 )
                {
                    fT = (float)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDist = fB1*fT+fC;
                }
            }
        }
        else if ( fT < (float)0.0 )  // region 6
        {
            fTmp0 = fA01 + fB1;
            fTmp1 = fA00 + fB0;
            if ( fTmp1 > fTmp0 )
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-((float)2.0)*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fT = (float)1.0;
                    fS = (float)0.0;
                    fSqrDist = fA11+((float)2.0)*fB1+fC;
                }
                else
                {
                    fT = fNumer/fDenom;
                    fS = (float)1.0 - fT;
                    fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) +
                        fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
                }
            }
            else
            {
                fT = (float)0.0;
                if ( fTmp1 <= (float)0.0 )
                {
                    fS = (float)1.0;
                    fSqrDist = fA00+((float)2.0)*fB0+fC;
                }
                else if ( fB0 >= (float)0.0 )
                {
                    fS = (float)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fS = -fB0/fA00;
                    fSqrDist = fB0*fS+fC;
                }
            }
        }
        else  // region 1
        {
            fNumer = fA11 + fB1 - fA01 - fB0;
            if ( fNumer <= (float)0.0 )
            {
                fS = (float)0.0;
                fT = (float)1.0;
                fSqrDist = fA11+((float)2.0)*fB1+fC;
            }
            else
            {
                fDenom = fA00-2.0f*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fS = (float)1.0;
                    fT = (float)0.0;
                    fSqrDist = fA00+((float)2.0)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = (float)1.0 - fS;
                    fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) +
                        fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
                }
            }
        }
    }

//    if ( pfSParam )
//        *pfSParam = fS;

//    if ( pfTParam )
//        *pfTParam = fT;

//    return Math<Real>::FAbs(fSqrDist);
	return fabsf(fSqrDist);
}

}