#ifndef MATRIX_H
#define MATRIX_H

#pragma warning (disable: 4700)

#include <memory.h>
#include "vector.h"

class matrix
{
public:
	float m[16];

	matrix()
	{
	}
	~matrix()
	{
	}

	void identity()
	{
		m[0] = 1;
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;
		m[4] = 0;
		m[5] = 1;
		m[6] = 0;
		m[7] = 0;
		m[8] = 0;
		m[9] = 0;
		m[10] = 1;
		m[11] = 0;
		m[12] = 0;
		m[13] = 0;
		m[14] = 0;
		m[15] = 1;
	}

	void inverse()
	{
		matrix minv;
		
		float r1[8], r2[8], r3[8], r4[8];
		float *s[4], *tmprow;
		
		s[0] = &r1[0];
		s[1] = &r2[0];
		s[2] = &r3[0];
		s[3] = &r4[0];
		
		register int i,j,p,jj;
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				s[i][j] = m[i*4+j];
				if(i==j) s[i][j+4] = 1.0;
				else     s[i][j+4] = 0.0;
			}
		}
		float scp[4];
		for(i=0;i<4;i++)
		{
			scp[i] = float(fabs(s[i][0]));
			for(j=1;j<4;j++)
				if(float(fabs(s[i][j])) > scp[i]) scp[i] = float(fabs(s[i][j]));
				if(scp[i] == 0.0)
					memcpy(m, minv.m, sizeof(float) * 16); // singular matrix!
		}
		
		int pivot_to;
		float scp_max;
		for(i=0;i<4;i++)
		{
			// select pivot row
			pivot_to = i;
			scp_max = float(fabs(s[i][i]/scp[i]));
			// find out which row should be on top
			for(p=i+1;p<4;p++)
				if(float(fabs(s[p][i]/scp[p])) > scp_max)
				{ scp_max = float(fabs(s[p][i]/scp[p])); pivot_to = p; }
				// Pivot if necessary
				if(pivot_to != i)
				{
					tmprow = s[i];
					s[i] = s[pivot_to];
					s[pivot_to] = tmprow;
					float tmpscp;
					tmpscp = scp[i];
					scp[i] = scp[pivot_to];
					scp[pivot_to] = tmpscp;
				}
				
				float mji;
				// perform gaussian elimination
				for(j=i+1;j<4;j++)
				{
					mji = s[j][i]/s[i][i];
					s[j][i] = 0.0;
					for(jj=i+1;jj<8;jj++)
						s[j][jj] -= mji*s[i][jj];
				}
		}
		if(s[3][3] == 0.0)
			memcpy(m, minv.m, sizeof(float) * 16); // singular matrix!
		
		//
		// Now we have an upper triangular matrix.
		//
		//  x x x x | y y y y
		//  0 x x x | y y y y 
		//  0 0 x x | y y y y
		//  0 0 0 x | y y y y
		//
		//  we'll back substitute to get the inverse
		//
		//  1 0 0 0 | z z z z
		//  0 1 0 0 | z z z z
		//  0 0 1 0 | z z z z
		//  0 0 0 1 | z z z z 
		//
		
		float mij;
		for(i=3;i>0;i--)
		{
			for(j=i-1;j > -1; j--)
			{
				mij = s[j][i]/s[i][i];
				for(jj=j+1;jj<8;jj++)
					s[j][jj] -= mij*s[i][jj];
			}
		}
		
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				minv.m[i*4+j] = s[i][j+4] / s[i][i];
			
		memcpy(m, minv.m, sizeof(float) * 16);
	}

	vector operator * (const vector &p)
	{
		float invw;
		invw = 1 / (p.v[0] * m[3] + p.v[1] * m[7] + p.v[2] * m[11] + m[15]);

		vector r;
		r.v[0] = (p.v[0] * m[0] + p.v[1] * m[4] + p.v[2] * m[8] + m[12]) * invw;
		r.v[1] = (p.v[0] * m[1] + p.v[1] * m[5] + p.v[2] * m[9] + m[13]) * invw;
		r.v[2] = (p.v[0] * m[2] + p.v[1] * m[6] + p.v[2] * m[10] + m[14]) * invw;

		return r;
	}

	matrix operator * (const matrix &p)
	{
		matrix r;
		r.m[0] = m[0] * p.m[0] + m[4] * p.m[1] + m[8] * p.m[2] + m[12] * p.m[3];
		r.m[1] = m[1] * p.m[0] + m[5] * p.m[1] + m[9] * p.m[2] + m[13] * p.m[3];
		r.m[2] = m[2] * p.m[0] + m[6] * p.m[1] + m[10] * p.m[2] + m[14] * p.m[3];
		r.m[3] = m[3] * p.m[0] + m[7] * p.m[1] + m[11] * p.m[2] + m[15] * p.m[3];
		r.m[4] = m[0] * p.m[4] + m[4] * p.m[5] + m[8] * p.m[6] + m[12] * p.m[7];
		r.m[5] = m[1] * p.m[4] + m[5] * p.m[5] + m[9] * p.m[6] + m[13] * p.m[7];
		r.m[6] = m[2] * p.m[4] + m[6] * p.m[5] + m[10] * p.m[6] + m[14] * p.m[7];
		r.m[7] = m[3] * p.m[4] + m[7] * p.m[5] + m[11] * p.m[6] + m[15] * p.m[7];
		r.m[8] = m[0] * p.m[8] + m[4] * p.m[9] + m[8] * p.m[10] + m[12] * p.m[11];
		r.m[9] = m[1] * p.m[8] + m[5] * p.m[9] + m[9] * p.m[10] + m[13] * p.m[11];
		r.m[10] = m[2] * p.m[8] + m[6] * p.m[9] + m[10] * p.m[10] + m[14] * p.m[11];
		r.m[11] = m[3] * p.m[8] + m[7] * p.m[9] + m[11] * p.m[10] + m[15] * p.m[11];
		r.m[12] = m[0] * p.m[12] + m[4] * p.m[13] + m[8] * p.m[14] + m[12] * p.m[15];
		r.m[13] = m[1] * p.m[12] + m[5] * p.m[13] + m[9] * p.m[14] + m[13] * p.m[15];
		r.m[14] = m[2] * p.m[12] + m[6] * p.m[13] + m[10] * p.m[14] + m[14] * p.m[15];
		r.m[15] = m[3] * p.m[12] + m[7] * p.m[13] + m[11] * p.m[14] + m[15] * p.m[15];
		return r;
	}
	matrix operator * (const float *p)
	{
		matrix r;
		r.m[0] = m[0] * p[0] + m[4] * p[1] + m[8] * p[2] + m[12] * p[3];
		r.m[1] = m[1] * p[0] + m[5] * p[1] + m[9] * p[2] + m[13] * p[3];
		r.m[2] = m[2] * p[0] + m[6] * p[1] + m[10] * p[2] + m[14] * p[3];
		r.m[3] = m[3] * p[0] + m[7] * p[1] + m[11] * p[2] + m[15] * p[3];
		r.m[4] = m[0] * p[4] + m[4] * p[5] + m[8] * p[6] + m[12] * p[7];
		r.m[5] = m[1] * p[4] + m[5] * p[5] + m[9] * p[6] + m[13] * p[7];
		r.m[6] = m[2] * p[4] + m[6] * p[5] + m[10] * p[6] + m[14] * p[7];
		r.m[7] = m[3] * p[4] + m[7] * p[5] + m[11] * p[6] + m[15] * p[7];
		r.m[8] = m[0] * p[8] + m[4] * p[9] + m[8] * p[10] + m[12] * p[11];
		r.m[9] = m[1] * p[8] + m[5] * p[9] + m[9] * p[10] + m[13] * p[11];
		r.m[10] = m[2] * p[8] + m[6] * p[9] + m[10] * p[10] + m[14] * p[11];
		r.m[11] = m[3] * p[8] + m[7] * p[9] + m[11] * p[10] + m[15] * p[11];
		r.m[12] = m[0] * p[12] + m[4] * p[13] + m[8] * p[14] + m[12] * p[15];
		r.m[13] = m[1] * p[12] + m[5] * p[13] + m[9] * p[14] + m[13] * p[15];
		r.m[14] = m[2] * p[12] + m[6] * p[13] + m[10] * p[14] + m[14] * p[15];
		r.m[15] = m[3] * p[12] + m[7] * p[13] + m[11] * p[14] + m[15] * p[15];
		return r;
	}
	matrix& operator *= (const matrix &p)
	{
		matrix r;
		r.m[0] = m[0] * p.m[0] + m[4] * p.m[1] + m[8] * p.m[2] + m[12] * p.m[3];
		r.m[1] = m[1] * p.m[0] + m[5] * p.m[1] + m[9] * p.m[2] + m[13] * p.m[3];
		r.m[2] = m[2] * p.m[0] + m[6] * p.m[1] + m[10] * p.m[2] + m[14] * p.m[3];
		r.m[3] = m[3] * p.m[0] + m[7] * p.m[1] + m[11] * p.m[2] + m[15] * p.m[3];
		r.m[4] = m[0] * p.m[4] + m[4] * p.m[5] + m[8] * p.m[6] + m[12] * p.m[7];
		r.m[5] = m[1] * p.m[4] + m[5] * p.m[5] + m[9] * p.m[6] + m[13] * p.m[7];
		r.m[6] = m[2] * p.m[4] + m[6] * p.m[5] + m[10] * p.m[6] + m[14] * p.m[7];
		r.m[7] = m[3] * p.m[4] + m[7] * p.m[5] + m[11] * p.m[6] + m[15] * p.m[7];
		r.m[8] = m[0] * p.m[8] + m[4] * p.m[9] + m[8] * p.m[10] + m[12] * p.m[11];
		r.m[9] = m[1] * p.m[8] + m[5] * p.m[9] + m[9] * p.m[10] + m[13] * p.m[11];
		r.m[10] = m[2] * p.m[8] + m[6] * p.m[9] + m[10] * p.m[10] + m[14] * p.m[11];
		r.m[11] = m[3] * p.m[8] + m[7] * p.m[9] + m[11] * p.m[10] + m[15] * p.m[11];
		r.m[12] = m[0] * p.m[12] + m[4] * p.m[13] + m[8] * p.m[14] + m[12] * p.m[15];
		r.m[13] = m[1] * p.m[12] + m[5] * p.m[13] + m[9] * p.m[14] + m[13] * p.m[15];
		r.m[14] = m[2] * p.m[12] + m[6] * p.m[13] + m[10] * p.m[14] + m[14] * p.m[15];
		r.m[15] = m[3] * p.m[12] + m[7] * p.m[13] + m[11] * p.m[14] + m[15] * p.m[15];
		memcpy(m, r.m, sizeof(float) * 16);
		return *this;
	}
	vector dirmult(const vector &p)
	{
		vector r;
		r.v[0] = p.v[0] * m[0] + p.v[1] * m[4] + p.v[2] * m[8];
		r.v[1] = p.v[0] * m[1] + p.v[1] * m[5] + p.v[2] * m[9];
		r.v[2] = p.v[0] * m[2] + p.v[1] * m[6] + p.v[2] * m[10];
		return r;
	}

private:
};

#endif