#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

#define PI					3.1415926535f
#define TWOPI				6.283185307f
#define PIOVER180			0.0174532925f
#define INVSQRT2			0.70710678118f

class vector
{
private:
public:
	float v[3];
	__forceinline vector()
	{
//		v[0] = 0; v[1] = 0; v[2] = 0;
	}
	vector (float *p)
	{
		v[0] = p[0];
		v[1] = p[1];
		v[2] = p[2];
	}
	__forceinline vector(const float &a, const float &b, const float &c)
	{
		v[0] = a; v[1] = b; v[2] = c;
	}
	__forceinline vector(const vector &a, const vector &b)
	{
		v[0] = a.v[0] - b.v[0];
		v[1] = a.v[1] - b.v[1];
		v[2] = a.v[2] - b.v[2];
	}
//	__forceinline ~vector()
//	{
//	}
	__forceinline vector operator + (const vector &param)
	{
//		vector temp;
//		temp.v[0] = v[0] + param.v[0];
//		temp.v[1] = v[1] + param.v[1];
//		temp.v[2] = v[2] + param.v[2];
//		return temp;
		float temp[3];
		temp[0] = v[0] + param.v[0];
		temp[1] = v[1] + param.v[1];
		temp[2] = v[2] + param.v[2];
		return *(vector *)temp;
	}
	__forceinline void operator += (const vector &param)
	{
		v[0] += param.v[0];
		v[1] += param.v[1];
		v[2] += param.v[2];
	}
	__forceinline vector operator + (const float &param)
	{
//		vector temp;
//		temp.v[0] = v[0] + param;
//		temp.v[1] = v[1] + param;
//		temp.v[2] = v[2] + param;
//		return temp;
		float temp[3];
		temp[0] = v[0] + param;
		temp[1] = v[1] + param;
		temp[2] = v[2] + param;
		return *(vector *)temp;
	}
	__forceinline void operator += (const float &param)
	{
		v[0] += param;
		v[1] += param;
		v[2] += param;
	}
	__forceinline vector operator - (const vector &param)
	{
//		vector temp;
//		temp.v[0] = v[0] - param.v[0];
//		temp.v[1] = v[1] - param.v[1];
//		temp.v[2] = v[2] - param.v[2];
//		return temp;
		float temp[3];
		temp[0] = v[0] - param.v[0];
		temp[1] = v[1] - param.v[1];
		temp[2] = v[2] - param.v[2];
		return *(vector *)temp;
	}
	__forceinline void operator -= (const vector &param)
	{
		v[0] -= param.v[0];
		v[1] -= param.v[1];
		v[2] -= param.v[2];
	}
	__forceinline vector operator - (const float &param)
	{
//		vector temp;
//		temp.v[0] = v[0] - param;
//		temp.v[1] = v[1] - param;
//		temp.v[2] = v[2] - param;
//		return temp;
		float temp[3];
		temp[0] = v[0] - param;
		temp[1] = v[1] - param;
		temp[2] = v[2] - param;
		return *(vector *)temp;
	}
	__forceinline void operator -= (const float &param)
	{
		v[0] -= param;
		v[1] -= param;
		v[2] -= param;
	}
	__forceinline vector operator * (const vector &param)
	{
//		vector temp;
//		temp.v[0] = v[0] * param.v[0];
//		temp.v[1] = v[1] * param.v[1];
//		temp.v[2] = v[2] * param.v[2];
//		return temp;
		float temp[3];
		temp[0] = v[0] * param.v[0];
		temp[1] = v[1] * param.v[1];
		temp[2] = v[2] * param.v[2];
		return *(vector *)temp;
	}
	__forceinline void operator *= (const vector &param)
	{
		v[0] *= param.v[0];
		v[1] *= param.v[1];
		v[2] *= param.v[2];
	}
	__forceinline vector operator * (const float &param)
	{
//		vector temp;
//		temp.v[0] = v[0] * param;
//		temp.v[1] = v[1] * param;
//		temp.v[2] = v[2] * param;
//		return temp;
		float temp[3];
		temp[0] = v[0] * param;
		temp[1] = v[1] * param;
		temp[2] = v[2] * param;
		return *(vector *)temp;
	}
	__forceinline void operator *= (const float &param)
	{
		v[0] *= param;
		v[1] *= param;
		v[2] *= param;
	}
	__forceinline vector operator / (const vector &param)
	{
//		vector temp;
//		temp.v[0] = v[0] / param.v[0];
//		temp.v[1] = v[1] / param.v[1];
//		temp.v[2] = v[2] / param.v[2];
//		return temp;
		float temp[3];
		temp[0] = v[0] / param.v[0];
		temp[1] = v[1] / param.v[1];
		temp[2] = v[2] / param.v[2];
		return *(vector *)temp;
	}
	__forceinline void operator /= (const vector &param)
	{
		v[0] /= param.v[0];
		v[1] /= param.v[1];
		v[2] /= param.v[2];
	}
	__forceinline vector operator / (const float &param)
	{
//		vector temp;
//		float ftemp = 1 / param;
//		temp.v[0] = v[0] * ftemp;
//		temp.v[1] = v[1] * ftemp;
//		temp.v[2] = v[2] * ftemp;
//		return temp;
		float temp[3];
		float ftemp = 1 / param;
		temp[0] = v[0] * param;
		temp[1] = v[1] * param;
		temp[2] = v[2] * param;
		return *(vector *)temp;
	}
	__forceinline void operator /= (const float &param)
	{
		float ftemp = 1 / param;
		v[0] *= ftemp;
		v[1] *= ftemp;
		v[2] *= ftemp;
/*		__asm
		{
			MOV EDX, [this.v]
			MOV EAX, [param]
			FEMMS

			MOVD MM2, [EAX]
			MOVQ MM0, QWORD PTR [EDX]
			MOVD MM1, QWORD PTR [EDX + 8]
			PFRCP MM3, MM2
			PUNPCKLDQ MM2, MM2
			PFRCPIT1 MM2, MM3
			PFRCPIT2 MM2, MM3
			PFMUL MM0, MM2
			PFMUL MM1, MM2
			MOVQ [EDX], MM0
			MOVD [EDX + 8], MM1

			FEMMS
		}*/
	}
	__forceinline vector& normalize()
	{
		float invlen = 1 / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		v[0] *= invlen;
		v[1] *= invlen;
		v[2] *= invlen;
		return *this;
/*		__asm
		{
			MOV EDX, [this.v] // EDX = v
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
		}*/
	}
	__forceinline vector& zeronormalize()
	{
		float invlen = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		if (invlen == 0)
			return *this;
		invlen = 1 / invlen;
		v[0] *= invlen;
		v[1] *= invlen;
		v[2] *= invlen;
		return *this;
	}
	__forceinline void clamp(const float &lower, const float &upper)
	{
		if (v[0] > upper) v[0] = upper; else if (v[0] < lower) v[0] = lower;
		if (v[1] > upper) v[1] = upper; else if (v[1] < lower) v[1] = lower;
		if (v[2] > upper) v[2] = upper; else if (v[2] < lower) v[2] = lower;
	}
	__forceinline float length()
	{
		return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}
	__forceinline float length2()
	{
		return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	}
	__forceinline bool operator == (vector param)
	{
		if (v[0] == param.v[0] && v[1] == param.v[1] && v[2] == param.v[2])
			return 1;
		return 0;
	}
	__forceinline bool operator != (vector param)
	{
		if (v[0] != param.v[0] || v[1] != param.v[1] || v[2] != param.v[2])
			return 1;
		return 0;
	}
	__forceinline void set(const float &p1, const float &p2, const float &p3)
	{
		v[0] = p1; v[1] = p2; v[2] = p3;
	}

/*	void operator *= (const matrix &m)
	{
		float w;
		w = v[0] * m.m[3] + v[1] * m.m[7] + v[2] * m.m[11] + m.m[15];

		float r[3];
		r[0] = (v[0] * m.m[0] + v[1] * m.m[4] + v[2] * m.m[8] + m.m[12]) / w;
		r[1] = (v[0] * m.m[1] + v[1] * m.m[5] + v[2] * m.m[9] + m.m[13]) / w;
		r[2] = (v[0] * m.m[2] + v[1] * m.m[6] + v[2] * m.m[10] + m.m[14]) / w;

		v[0] = r[0];
		v[1] = r[1];
		v[2] = r[2];
	}*/
};

extern __forceinline void MxM(float *m1, float *m2, float *r);
extern __forceinline float Dot(const vector&, const vector&);
extern __forceinline vector Cross(const vector&, const vector&);
extern __forceinline vector Average(const vector&, const vector&);
extern __forceinline vector VectorSet(const float &a, const float &b, const float &c);
extern __forceinline vector NormalVec(vector a);
extern __forceinline vector ZeroNormalVec(vector a);
extern __forceinline float PlaneDistance(vector, vector);
extern __forceinline bool IntersectedPlane(vector [], vector [], vector&, float&);
extern __forceinline float AngleBetweenVectors(vector, vector);
extern __forceinline vector IntersectionPoint(vector, vector [], float);
extern __forceinline bool InsidePolygon(vector, vector [], int);
extern __forceinline bool IntersectedPolygon(vector [], vector [], int, vector&);
extern __forceinline float IntersectRayPlane(vector rOrigin, vector rVector, vector pOrigin, vector pNormal);
extern __forceinline float IntersectRaySphere2(vector rO, vector rV, vector sO, float sR);
extern __forceinline int IntersectRaySphere(vector ro, vector rv, vector so, float r, vector &iptnear, vector &iptfar);
extern __forceinline bool CheckPointInTriangle(vector &point, vector &a, vector &b, vector &c);
extern __forceinline vector ClosestPointOnLine(vector &a, vector &b, vector &p);
extern __forceinline vector ClosestPointOnTriangle(vector a, vector b, vector c, vector p);
extern __forceinline bool CheckPointInSphere(vector point, vector sO, float sR);
extern __forceinline vector TangentPlaneNormalOfEllipsoid(vector point, vector eO, vector eR);
extern __forceinline int ClassifyPoint(vector point, vector pO, vector pN);
extern __forceinline bool CheckPointInTriangle2(vector &point, vector &a, vector &b, vector &c);
extern __forceinline bool CheckPointInTriangle3(float *point, float *normal, float *triangle);
extern __forceinline bool CheckPointInTriangle4(float *p, float *t);
extern __forceinline float PointTriangleDistance(vector a, vector b, vector c, vector p);

class quaternion
{
private:
public:
	float w, x, y, z;
	quaternion()
	{
	}
	// removed normalization of axis and xyzw from set
	quaternion(vector axis, float radians)
	{
		radians *= .5;
		w = cosf(radians);
		float sr = sinf(radians);
		x = axis.v[0] * sr;
		y = axis.v[1] * sr;
		z = axis.v[2] * sr;
	}
	void Set(vector axis, float radians)
	{
		radians *= .5;
		w = cosf(radians);
		float sr = sinf(radians);
		x = axis.v[0] * sr;
		y = axis.v[1] * sr;
		z = axis.v[2] * sr;
	}
	quaternion(const float *a, float radians)
	{
		radians *= .5;
		w = cosf(radians);
		float sr = sinf(radians);
		x = a[0] * sr;
		y = a[1] * sr;
		z = a[2] * sr;
	}
	void Set(const float *a, float radians)
	{
		radians *= .5;
		w = cosf(radians);
		float sr = sinf(radians);
		x = a[0] * sr;
		y = a[1] * sr;
		z = a[2] * sr;
	}

	quaternion operator * (quaternion param)
	{
		vector v1, v2, vp;
		quaternion qp;

		v1.v[0] = x; v1.v[1] = y; v1.v[2] = z;
		v2.v[0] = param.x; v2.v[1] = param.y; v2.v[2] = param.z;
		qp.w = w * param.w - Dot(v1, v2);
		vp = v2 * w + v1 * param.w - Cross(v1, v2);
		qp.x = vp.v[0];
		qp.y = vp.v[1];
		qp.z = vp.v[2];
		return qp;
	}
	void operator *= (const quaternion param)
	{
		float vp[3];

		vp[0] = param.x * w + x * param.w - (y * param.z - z * param.y);
		vp[1] = param.y * w + y * param.w - (z * param.x - x * param.z);
		vp[2] = param.z * w + z * param.w - (x * param.y - y * param.x);

		w = w * param.w - (x * param.x + y * param.y + z * param.z);

		x = vp[0];
		y = vp[1];
		z = vp[2];
	}
	void Matrix(float *m)
	{
		float x2, y2, z2, xx, yy, zz, xy, yz, xz, wx, wy, wz;
		x2 = x + x;
		y2 = y + y;
		z2 = z + z;
		xx = x * x2;
		yy = y * y2;
		zz = z * z2;
		xy = x * y2;
		yz = y * z2;
		xz = z * x2;
		wx = w * x2;
		wy = w * y2;
		wz = w * z2;
		m[0] = 1 - (yy + zz);
		m[1] = xy + wz;
		m[2] = xz - wy;
		m[3] = 0;
		m[4] = xy - wz;
		m[5] = 1 - (xx + zz);
		m[6] = yz + wx;
		m[7] = 0;
		m[8] = xz + wy;
		m[9] = yz - wx;
		m[10] = 1 - (xx + yy);
		m[11] = 0;
		m[12] = 0;
		m[13] = 0;
		m[14] = 0;
		m[15] = 1;
	}
	quaternion Slerp(quaternion q, float c)
	{
		float omega, cosomega, sinomega, k1, k2;
		quaternion q2, q3;
		cosomega = x * q.x + y * q.y + z * q.z + w * q.w;
		if (cosomega < 0)
		{
			cosomega = -cosomega;
			q2.x = -q.x;
			q2.y = -q.y;
			q2.z = -q.z;
			q2.w = -q.w;
		}
		else
			q2 = q;

		if (1 - cosomega > .000001)
		{
			omega = acosf(cosomega);
			sinomega = sinf(omega);
			k1 = sinf((1 - c) * omega) / sinomega;
			k2 = sinf(c * omega) / sinomega;
		}
		else
		{
			k1 = 1 - c;
			k2 = c;
		}

		q3.x = x * k1 + q2.x * k2;
		q3.y = y * k1 + q2.y * k2;
		q3.z = z * k1 + q2.z * k2;
		q3.w = w * k1 + q2.w * k2;

		return q3;
	}

	quaternion& conjugate()
	{
		x *= -1;
		y *= -1;
		z *= -1;
		return *this;
	}

	bool operator == (const quaternion &p)
	{
		return (x == p.x && y == p.y && z == p.z && w == p.w);
	}
	bool operator != (const quaternion &p)
	{
		return (x != p.x || y != p.y || z != p.z || w != p.w);
	}
};

#endif