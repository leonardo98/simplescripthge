#include "skybox.h"

namespace SkyBox
{

struct WaterVertex
{
	float pos[3];
	float normal[3];
	float texcoord0[2];
};

unsigned int			*texture = NULL;
unsigned int			textures = 0;
TexInfo					*texinfo = NULL;

unsigned int			fonttex = 0;

unsigned int			particletex = 0;


unsigned int			waternormaltex = 0;

unsigned int			waterfp1 = 0;
unsigned int			watervp1 = 0;

unsigned int			cubemap = 0;

unsigned int			reflection = 0;
unsigned int			refraction = 0;
int						reflectiontexsizex = 512;
int						reflectiontexsizey = 512;

int wirewater = 1;


double modelview[16];
double projection[16];
int viewport[4];

float clipplane[6][4];


void GetMatrixFrustum()
{
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	matrix mv, mp, mvp;
	for (int k = 0; k < 16; k++)
		mv.m[k] = modelview[k];
	for (int k = 0; k < 16; k++)
		mp.m[k] = projection[k];
	mvp = mp * mv;

	// left
	clipplane[0][0] = mvp.m[3] + mvp.m[0];
	clipplane[0][1] = mvp.m[7] + mvp.m[4];
	clipplane[0][2] = mvp.m[11] + mvp.m[8];
	clipplane[0][3] = mvp.m[15] + mvp.m[12];

	// right
	clipplane[1][0] = mvp.m[3] - mvp.m[0];
	clipplane[1][1] = mvp.m[7] - mvp.m[4];
	clipplane[1][2] = mvp.m[11] - mvp.m[8];
	clipplane[1][3] = mvp.m[15] - mvp.m[12];

	// bottom
	clipplane[2][0] = mvp.m[3] + mvp.m[1];
	clipplane[2][1] = mvp.m[7] + mvp.m[5];
	clipplane[2][2] = mvp.m[11] + mvp.m[9];
	clipplane[2][3] = mvp.m[15] + mvp.m[13];

	// top
	clipplane[3][0] = mvp.m[3] - mvp.m[1];
	clipplane[3][1] = mvp.m[7] - mvp.m[5];
	clipplane[3][2] = mvp.m[11] - mvp.m[9];
	clipplane[3][3] = mvp.m[15] - mvp.m[13];

	// near
	clipplane[4][0] = mvp.m[3] + mvp.m[2];
	clipplane[4][1] = mvp.m[7] + mvp.m[6];
	clipplane[4][2] = mvp.m[11] + mvp.m[10];
	clipplane[4][3] = mvp.m[15] + mvp.m[14];

	// far
	clipplane[5][0] = mvp.m[3] - mvp.m[2];
	clipplane[5][1] = mvp.m[7] - mvp.m[6];
	clipplane[5][2] = mvp.m[11] - mvp.m[10];
	clipplane[5][3] = mvp.m[15] - mvp.m[14];

	float invlen;
	for (int n = 0; n < 6; n++)
	{
		invlen = 1 / sqrtf(clipplane[n][0] * clipplane[n][0] + clipplane[n][1] * clipplane[n][1] + clipplane[n][2] * clipplane[n][2]);
		clipplane[n][0] *= invlen;
		clipplane[n][1] *= invlen;
		clipplane[n][2] *= invlen;
		clipplane[n][3] *= invlen;
	}
}

int SphereFrustum(vector center, float radius)
{
	float distance;
	for (int n = 0; n < 6; n++)
	{
		distance = center.v[0] * clipplane[n][0] + center.v[1] * clipplane[n][1] + center.v[2] * clipplane[n][2] + clipplane[n][3];

		if (distance < -radius) // outside
			return -1;
		if (fabsf(distance) < radius) // intersect
			return 0;
	}
	return 1; // inside
}

int noisedim = 64;
float *noise1, *noise2, *noise3, *noise4;

int heightdim = 256;
float heightextx = 100;
float heightexty = 100;
float *heightmap, *h0r, *h0i, *kv, *skg, *normalmap;
unsigned char *normaltexdata;
float waveamp = .0008;
float gravity = 9.8;
float wind[2] = {5, 10};

// height of water plane
float height = 0;
//float bounds = 2;
// normalmap scale
float scale = .25;

float *workarea1, *workarea3;
int *workarea2;

void UpdateHeight()
{
	float **rh;
	rh = new float * [heightdim];
	for (int n = 0; n < heightdim; n++)
		rh[n] = new float [heightdim * 2];
	float t = timer.getelapsed();
	for (int x = 0; x < heightdim; x++)
	{
		for (int y = 0; y < heightdim / 2; y++)
		{
			float wkt = skg[x * heightdim + y] * t;
			float e1 = cosf(wkt);
			float e2 = sinf(wkt);
			// produces jittering...
//			float e2 = sqrtf(1 - e1 * e1);
			// conjugate is (e1, -e2)
			float rr, ri;
			// h~0(k) * e^(-i*wkt) + h~*0(-k) * e^(i*wkt);
			rr = h0r[x * heightdim + y] * e1 + h0r[x * heightdim + (heightdim - y - 1)] * e1;
			ri = h0i[x * heightdim + y] * e2 + -h0i[x * heightdim + (heightdim - y - 1)] * -e2;
			rh[x][y * 2 + 0] = rr;
			rh[x][y * 2 + 1] = ri;
			rh[x][(heightdim - y - 1) * 2 + 0] = rr;
			rh[x][(heightdim - y - 1) * 2 + 1] = -ri;
		}
	}

	cdft2d(heightdim, heightdim * 2, -1, rh, workarea1, workarea2, workarea3);

	for (int x = 0; x < heightdim; x++)
	{
		for (int y = 0; y < heightdim; y++)
		{
			if ((x + y) % 2 == 0)
				heightmap[x * heightdim + y] = rh[x][y * 2 + 0];
			else
				heightmap[x * heightdim + y] = -rh[x][y * 2 + 0];
		}
	}

	for (int n = 0; n < heightdim; n++)
		delete [] rh[n];
	delete [] rh;


	float invlen;
	for (int x = 0; x < heightdim; x++)
	{
		for (int y = 0; y < heightdim; y++)
		{
			normalmap[(x * heightdim + y) * 3 + 0] = 
				(heightmap[x * heightdim + y] - heightmap[(x + 1) % heightdim * heightdim + y]) * (heightdim / heightextx) * scale;
			normalmap[(x * heightdim + y) * 3 + 1] = 1;
			normalmap[(x * heightdim + y) * 3 + 2] = 
				(heightmap[x * heightdim + y] - heightmap[x * heightdim + (y + 1) % heightdim]) * (heightdim / heightexty) * scale;
///////////////////////////////////////////////////////////////////////////////////////////
			// could skip renormalizing if it's going to happen on gpu anyway
/////////////////////////////////////////////////////////////
			invlen = 1 / sqrtf(normalmap[(x * heightdim + y) * 3 + 0] * normalmap[(x * heightdim + y) * 3 + 0] + 1 + normalmap[(x * heightdim + y) * 3 + 2] * normalmap[(x * heightdim + y) * 3 + 2]);
			normalmap[(x * heightdim + y) * 3 + 0] *= invlen;
			normalmap[(x * heightdim + y) * 3 + 1] *= invlen;
			normalmap[(x * heightdim + y) * 3 + 2] *= invlen;
		}
	}
}

float pow1max(float x, float y, float max)
{
	x = powf(x, y);
	if (x < 1)
		return 1;
	else if (x > max)
		return max;
	else
		return x;
}

void SetupGrid3D(WaterVertex *&point, unsigned int *&pindex, int &picount)
{
	float invheightdim = (float)1 / heightdim;
//	float gridscale = 10 * ((float)64 / heightdim);
	float gridscale = 10;
	float invgridscale = (float)1 / gridscale;

	int pdimx = 64; // power of 2
	int pdimy = 64;
	int divmin = 16; // maximum amount to divide by
	int blockext = 2;
	int bias = 1;
	int vertices = 0, indices = -2;
	for (int bx = -blockext; bx < blockext + 1; bx++)
	{
		for (int by = -blockext; by < blockext + 1; by++)
		{
			int d, dx, dy;
			dx = abs(bx);
			dy = abs(by);
			if (dx > dy)
				d = dx;
			else
				d = dy;

			int bdimx, bdimy;
			bdimx = pdimx / pow1max(2, d + bias, divmin) + 1;
			bdimy = pdimy / pow1max(2, d + bias, divmin) + 1;
			vertices += bdimx * bdimy;
			indices += 2 + (bdimx - 1) * (bdimy * 2 + 2) - 2;
		}
	}
	vertices += 4;
	indices += 6;
	point = new WaterVertex [vertices];
	pindex = new unsigned int [indices];
	picount = indices;
	int spoint = 0;
	int sindex = 0;
	int firstblock = 1;
	for (int bx = -blockext; bx < blockext + 1; bx++)
	{
		for (int by = -blockext; by < blockext + 1; by++)
		{
			int d, dx, dy;
			dx = abs(bx);
			dy = abs(by);
			if (dx > dy)
				d = dx;
			else
				d = dy;

			int bdimx, bdimy;
			bdimx = pdimx / pow1max(2, d + bias, divmin) + 1;
			bdimy = pdimy / pow1max(2, d + bias, divmin) + 1;


			int i = 0, p, pointcount = 0;
			int xmin, xmax, ymin, ymax;
			int xi, yi;
			xi = camera.pos.v[0] / gridscale;
			yi = camera.pos.v[2] / gridscale;
			xmin = camera.pos.v[0] / gridscale - pdimx / 2 + pdimx * bx;
			xmin = xmin / (int)pow1max(2, blockext + bias, divmin) * (int)pow1max(2, blockext + bias, divmin);
			xmax = xmin + pdimx + 1;
			ymin = camera.pos.v[2] / gridscale - pdimy / 2 + pdimy * by;
			ymin = ymin / (int)pow1max(2, blockext + bias, divmin) * (int)pow1max(2, blockext + bias, divmin);
			ymax = ymin + pdimy + 1;
			int xp, yp;
			int incr = pow1max(2, d + bias, divmin);
			for (int x = xmin; x < xmax; x+= incr)
			{
				for (int y = ymin; y < ymax; y+= incr)
				{
					xp = x % heightdim;
					yp = y % heightdim;
					if (xp < 0 && yp < 0)
						p = (heightdim + xp) * heightdim + (heightdim + yp);
					else if (xp < 0)
						p = (heightdim + xp) * heightdim + yp;
					else if (yp < 0)
						p = (xp) * heightdim + (heightdim + yp);
					else
						p = (xp) * heightdim + yp;
					point[i + spoint].pos[0] = x * gridscale;
					point[i + spoint].pos[2] = y * gridscale;
//					if (bx == 0 && by == 0)
					point[i + spoint].pos[1] = heightmap[p];
//					else
//						point[i + spoint].pos[1] = heightmap[p] * 0;
					point[i + spoint].normal[0] = normalmap[p * 3 + 0];
					point[i + spoint].normal[1] = normalmap[p * 3 + 1];
					point[i + spoint].normal[2] = normalmap[p * 3 + 2];
					point[i + spoint].texcoord0[0] = point[i + spoint].pos[2] * invheightdim * invgridscale;
					point[i + spoint].texcoord0[1] = point[i + spoint].pos[0] * invheightdim * invgridscale;

					i++;
				}
			}
			pointcount = i;

			i = 0;
			if (firstblock)
			{
				firstblock = 0;
			}
			else
			{
				pindex[i++ + sindex] = bdimy + spoint;
			}
			for (int x = 0; x < bdimx - 1; x++)
			{
				for (int y = 0; y < bdimy; y++)
				{
					pindex[i++ + sindex] = (x + 1) * bdimy + y + spoint;
					pindex[i++ + sindex] = x * bdimy + y + spoint;
				}
				if (x < bdimx - 2)
				{
					pindex[i + sindex] = pindex[i - 1 + sindex];
					i++;
					pindex[i++ + sindex] = (x + 2) * bdimy + spoint;
				}
			}
			if (!firstblock && !(bx == blockext && by == blockext))
			{
				pindex[i + sindex] = pindex[i - 1 + sindex];
				i++;
			}
			spoint += pointcount;
			sindex += i;
		}
	}

	pindex[sindex] = pindex[sindex - 1];
	sindex++;
	pindex[sindex++] = spoint;
	pindex[sindex++] = spoint;
	point[spoint].pos[0] = -50000;
	point[spoint].pos[1] = height - 30;
	point[spoint].pos[2] = -50000;
	point[spoint].normal[0] = 0;
	point[spoint].normal[1] = 1;
	point[spoint].normal[2] = 0;
	point[spoint].texcoord0[0] = point[spoint].pos[2] * invheightdim * invgridscale;
	point[spoint].texcoord0[1] = point[spoint].pos[0] * invheightdim * invgridscale;
	spoint++;
	pindex[sindex++] = spoint;
	point[spoint].pos[0] = -50000;
	point[spoint].pos[1] = height - 30;
	point[spoint].pos[2] = 50000;
	point[spoint].normal[0] = 0;
	point[spoint].normal[1] = 1;
	point[spoint].normal[2] = 0;
	point[spoint].texcoord0[0] = point[spoint].pos[2] * invheightdim * invgridscale;
	point[spoint].texcoord0[1] = point[spoint].pos[0] * invheightdim * invgridscale;
	spoint++;
	pindex[sindex++] = spoint;
	point[spoint].pos[0] = 50000;
	point[spoint].pos[1] = height - 30;
	point[spoint].pos[2] = -50000;
	point[spoint].normal[0] = 0;
	point[spoint].normal[1] = 1;
	point[spoint].normal[2] = 0;
	point[spoint].texcoord0[0] = point[spoint].pos[2] * invheightdim * invgridscale;
	point[spoint].texcoord0[1] = point[spoint].pos[0] * invheightdim * invgridscale;
	spoint++;
	pindex[sindex++] = spoint;
	point[spoint].pos[0] = 50000;
	point[spoint].pos[1] = height - 30;
	point[spoint].pos[2] = 50000;
	point[spoint].normal[0] = 0;
	point[spoint].normal[1] = 1;
	point[spoint].normal[2] = 0;
	point[spoint].texcoord0[0] = point[spoint].pos[2] * invheightdim * invgridscale;
	point[spoint].texcoord0[1] = point[spoint].pos[0] * invheightdim * invgridscale;
	spoint++;

/*	glVertex4f(-100000, height, 100000, 1);
	glVertex4f(100000, height, 100000, 1);
	glVertex4f(100000, height, -100000, 1);
	glVertex4f(-100000, height, -100000, 1);*/

	wirewater = 0;
}

void SetupGrid2D(WaterVertex *&point, unsigned int *&pindex, int &picount)
{
	matrix m, mv, pm, mi;
	for (int n = 0; n < 16; n++)
	{
		mv.m[n] = modelview[n];
		pm.m[n] = projection[n];
	}
	m = pm * mv;
	mi = m;
	mi.inverse();


	picount = 0;

	float xstp = .05, ystp = .01;
	int pdimx, pdimy;
	float borderx = .2;
	float bordery = .3;
	// need to add some extra adjustments for this so it doesn't slow things down too much
	// tighter fit, wave height, etc.
	if (camera.pos.v[1] - height > 100)
	{
		borderx = .05;
		bordery = .05;
	}
	else
	{
		borderx = (100 - (camera.pos.v[1] - height)) / 200;
		bordery = (100 - (camera.pos.v[1] - height)) / 200;
	}
	pdimx = ((1.0 + borderx * 1.01) / xstp) * 2 + 1;
	pdimy = ((1.0 + bordery * 1.01) / ystp) * 2 + 1;
	point = new WaterVertex [pdimx * pdimy];
	pindex = new unsigned int [(pdimx - 1) * (pdimy * 2 + 2) - 2];
	int *ycount;
	ycount = new int [pdimx];
	memset(ycount, 0, sizeof(int) * pdimx);
	int xpos = 0;

	float t = fmodf(timer.getelapsed() * 8, noisedim);

	float pos[3];
	float tvr;
	float invw;
	float xinc[4];
	int horizon;
	float horizondist = 300000;
	float invheightdim = (float)1 / heightdim;
	xinc[0] = xstp * mi.m[0];
	xinc[1] = xstp * mi.m[1];
	xinc[2] = xstp * mi.m[2];
	xinc[3] = xstp * mi.m[3];
	float xmi[4];
	xmi[0] = (-1 - borderx - xstp) * mi.m[0] + mi.m[12];
	xmi[1] = (-1 - borderx - xstp) * mi.m[1] + mi.m[13];
	xmi[2] = (-1 - borderx - xstp) * mi.m[2] + mi.m[14];
	xmi[3] = (-1 - borderx - xstp) * mi.m[3] + mi.m[15];
	int i = 0;
	for (float fx = -1 - borderx; fx <= 1.0001f + borderx; fx+=xstp)
	{
		xmi[0] += xinc[0];
		xmi[1] += xinc[1];
		xmi[2] += xinc[2];
		xmi[3] += xinc[3];
		horizon = 0;
		for (float y = -1 - bordery; y <= 1.0001f + bordery; y+=ystp)
		{
			// transform from [[-1, 1],[-1, 1]] by inverse projection*modelview
			// project from camera to point and stop at intersection with plane
			invw = 1.0f / (xmi[3] + y * mi.m[7]);
			tvr = (height - camera.pos.v[1]) / ((xmi[1] + y * mi.m[5]) * invw - camera.pos.v[1]);
			// above plane
			if (tvr < 0)
			{
				horizon = 1;
				break;
			}
			pos[0] = camera.pos.v[0] + ((xmi[0] + y * mi.m[4]) * invw - camera.pos.v[0]) * tvr;
			pos[1] = camera.pos.v[2] + ((xmi[2] + y * mi.m[6]) * invw - camera.pos.v[2]) * tvr;
			pos[2] = t;
			point[i].pos[0] = pos[0];
//			point[i * 3 + 1] = height + Perlin3D(pos, noise1, noise2, noise3, noise4, noisedim, noisedim, noisedim);
			// bilinear seems to look the same as the S-Curve
			// try pixel readback to get interpolated height?
			point[i].pos[1] = height + Sample2DS(pos, heightmap, heightdim, heightdim, scale);
			point[i].pos[2] = pos[1];
			point[i].texcoord0[0] = pos[1] * invheightdim * scale;
			point[i].texcoord0[1] = pos[0] * invheightdim * scale;
			i++;
			ycount[xpos]++;
		}
		if (horizon)
		{
			pos[0] = (pos[0] - camera.pos.v[0]) * horizondist;
			pos[1] = (pos[1] - camera.pos.v[2]) * horizondist;
			pos[2] = t;
			point[i].pos[0] = pos[0];
			point[i].pos[1] = height;
			point[i].pos[2] = pos[1];
			point[i].texcoord0[0] = pos[1] * invheightdim * scale;
			point[i].texcoord0[1] = pos[0] * invheightdim * scale;
			i++;
			ycount[xpos]++;
		}
		xpos++;
	}
	// not safe for uneven horizons yet (need the check from the index portion)
	float u[3], v[3], invlen; // move normalize to vp
	i = 0;
	int x;
	for (x = 0; x < pdimx - 1; x++)
	{
		for (int y = 0; y < ycount[x] - 1; y++)
		{
			u[0] = point[i + ycount[x]].pos[0] - point[i].pos[0];
			u[1] = point[i + ycount[x]].pos[1] - point[i].pos[1];
			u[2] = point[i + ycount[x]].pos[2] - point[i].pos[2];
			v[0] = point[i + 1].pos[0] - point[i].pos[0];
			v[1] = point[i + 1].pos[1] - point[i].pos[1];
			v[2] = point[i + 1].pos[2] - point[i].pos[2];

			point[i].normal[0] = u[1] * v[2] - u[2] * v[1];
			point[i].normal[1] = u[2] * v[0] - u[0] * v[2];
			point[i].normal[2] = u[0] * v[1] - u[1] * v[0];
			invlen = 1 / sqrtf(point[i].normal[0] * point[i].normal[0] + point[i].normal[1] * point[i].normal[1] + point[i].normal[2] * point[i].normal[2]);
			point[i].normal[0] *= invlen;
			point[i].normal[1] *= invlen;
			point[i].normal[2] *= invlen;

			i++;
		}
		point[i].normal[0] = 0;
		point[i].normal[1] = 1;
		point[i].normal[2] = 0;
		i++;
	}
	for (int y = 0; y < ycount[x] - 1; y++)
	{
		u[0] = point[y].pos[0] - point[i].pos[0];
		u[1] = point[y].pos[1] - point[i].pos[1];
		u[2] = point[y].pos[2] - point[i].pos[2];
		v[0] = point[i + 1].pos[0] - point[i].pos[0];
		v[1] = point[i + 1].pos[1] - point[i].pos[1];
		v[2] = point[i + 1].pos[2] - point[i].pos[2];

		point[i].normal[0] = u[1] * v[2] - u[2] * v[1];
		point[i].normal[1] = u[2] * v[0] - u[0] * v[2];
		point[i].normal[2] = u[0] * v[1] - u[1] * v[0];
		invlen = 1 / sqrtf(point[i].normal[0] * point[i].normal[0] + point[i].normal[1] * point[i].normal[1] + point[i].normal[2] * point[i].normal[2]);
		point[i].normal[0] *= invlen;
		point[i].normal[1] *= invlen;
		point[i].normal[2] *= invlen;

		i++;
	}
	point[i].normal[0] = 0;
	point[i].normal[1] = 1;
	point[i].normal[2] = 0;
	i++;

	int ipos = 0;
	// will work for uneven horizons, but have to reverse the build order for one side
	for (int x = 0; x < pdimx - 1; x++)
	{
		for (int y = 0; y < ycount[x]; y++)
		{
			pindex[picount++] = ipos + y;
			if (y < ycount[x + 1])
				pindex[picount++] = ipos + ycount[x] + y;
			else
				pindex[picount++] = ipos + ycount[x] + ycount[x + 1] - 1;
		}
		if (x < pdimx - 2)
		{
			pindex[picount] = pindex[picount - 1];
			picount++;
			pindex[picount++] = ipos + ycount[x];
		}
		ipos += ycount[x];
	}
	delete [] ycount;
}

void RenderSkybox(unsigned int skytex)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, skytex);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);

	glDisable(GL_CULL_FACE);
	

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
//	glLoadIdentity();
//	glRotatef(-camera.rot.v[0], 1, 0, 0);
//	glRotatef(-camera.rot.v[1], 0, 1, 0);

	glColor4f(1, 1, 1, 1);

	float offset[3];
	offset[0] = camera.pos.v[0];
	offset[1] = camera.pos.v[1];
	offset[2] = camera.pos.v[2];

	glBegin(GL_QUADS);

	// +Z
	glTexCoord3f( 5,  5,  5);
	glVertex3f  (-5 + offset[0], -5 + offset[1],  5 + offset[2]);
	glTexCoord3f( 5, -5,  5);
	glVertex3f  (-5 + offset[0],  5 + offset[1],  5 + offset[2]);
	glTexCoord3f(-5, -5,  5);
	glVertex3f  ( 5 + offset[0],  5 + offset[1],  5 + offset[2]);
	glTexCoord3f(-5,  5,  5);
	glVertex3f  ( 5 + offset[0], -5 + offset[1],  5 + offset[2]);

	// -Z
	glTexCoord3f(-5, -5, -5);
	glVertex3f  ( 5 + offset[0],  5 + offset[1], -5 + offset[2]);
	glTexCoord3f(-5,  5, -5);
	glVertex3f  ( 5 + offset[0], -5 + offset[1], -5 + offset[2]);
	glTexCoord3f( 5,  5, -5);
	glVertex3f  (-5 + offset[0], -5 + offset[1], -5 + offset[2]);
	glTexCoord3f( 5, -5, -5);
	glVertex3f  (-5 + offset[0],  5 + offset[1], -5 + offset[2]);

	// +Y
	glTexCoord3f( 5,  5,  5);
	glVertex3f  (-5 + offset[0],  5 + offset[1], -5 + offset[2]);
	glTexCoord3f( 5,  5, -5);
	glVertex3f  (-5 + offset[0],  5 + offset[1],  5 + offset[2]);
	glTexCoord3f(-5,  5, -5);
	glVertex3f  ( 5 + offset[0],  5 + offset[1],  5 + offset[2]);
	glTexCoord3f(-5,  5,  5);
	glVertex3f  ( 5 + offset[0],  5 + offset[1], -5 + offset[2]);

	// -Y
	glTexCoord3f(-5, -5, -5);
	glVertex3f  ( 5 + offset[0], -5 + offset[1],  5 + offset[2]);
	glTexCoord3f(-5, -5,  5);
	glVertex3f  ( 5 + offset[0], -5 + offset[1], -5 + offset[2]);
	glTexCoord3f( 5, -5,  5);
	glVertex3f  (-5 + offset[0], -5 + offset[1], -5 + offset[2]);
	glTexCoord3f( 5, -5, -5);
	glVertex3f  (-5 + offset[0], -5 + offset[1],  5 + offset[2]);

	// +X
	glTexCoord3f( 5,  5,  5);
	glVertex3f  ( 5 + offset[0], -5 + offset[1], -5 + offset[2]);
	glTexCoord3f( 5, -5,  5);
	glVertex3f  ( 5 + offset[0],  5 + offset[1], -5 + offset[2]);
	glTexCoord3f( 5, -5, -5);
	glVertex3f  ( 5 + offset[0],  5 + offset[1],  5 + offset[2]);
	glTexCoord3f( 5,  5, -5);
	glVertex3f  ( 5 + offset[0], -5 + offset[1],  5 + offset[2]);

	// -X
	glTexCoord3f(-5, -5, -5);
	glVertex3f  (-5 + offset[0],  5 + offset[1],  5 + offset[2]);
	glTexCoord3f(-5,  5, -5);
	glVertex3f  (-5 + offset[0], -5 + offset[1],  5 + offset[2]);
	glTexCoord3f(-5,  5,  5);
	glVertex3f  (-5 + offset[0], -5 + offset[1], -5 + offset[2]);
	glTexCoord3f(-5, -5,  5);
	glVertex3f  (-5 + offset[0],  5 + offset[1], -5 + offset[2]);

	glEnd();

	glPopMatrix();


	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);
}

// perhaps disable mipmapping on the normal map so that it will produce a glittering effect in
// the distance
void Render()
{
	UpdateHeight();

	glBindTexture(GL_TEXTURE_2D, waternormaltex);
	int dim = heightdim;
	unsigned char *level;
	unsigned char *prevlevel;
	for (int n = 0; n < dim * dim * 3; n++)
	{
		normaltexdata[n] = (normalmap[n] + 1) * .5 * 255;
	}
	// opengl defaults to 4 bytes
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dim, dim, GL_RGB, GL_UNSIGNED_BYTE, normaltexdata);
	prevlevel = new unsigned char [dim * dim * 4];
//	memcpy(prevlevel, normaltexdata, dim * dim * 3);
	for (int x = 0; x < dim; x++)
	{
		for (int y = 0; y < dim; y++)
		{
			prevlevel[(x * dim + y) * 4 + 0] = normaltexdata[(x * dim + y) * 3 + 0];
			prevlevel[(x * dim + y) * 4 + 1] = normaltexdata[(x * dim + y) * 3 + 1];
			prevlevel[(x * dim + y) * 4 + 2] = normaltexdata[(x * dim + y) * 3 + 2];
		}
	}
	for (int i = 1; i < 7; i++)
	{
		dim /= 2;
		level = new unsigned char [dim * dim * 4];
		memset(level, 255, dim * dim * 4);
//		if (i > 4)
		if (0)
		{
			for (int x = 0; x < dim; x++)
			{
				for (int y = 0; y < dim; y++)
				{
					level[(x * dim + y) * 4 + 0] = 127;
					level[(x * dim + y) * 4 + 1] = 255;
					level[(x * dim + y) * 4 + 2] = 127;
				}
			}
		}
		else
		{
			int x;
			for (x = 0; x < dim - 1; x++)
			{
				int y;
				for (y = 0; y < dim - 1; y++)
				{
					level[(x * dim + y) * 4 + 0] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 0] / 4
						+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2) * 4 + 0] / 4
						+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2 + 1) * 4 + 0] / 4
						+ prevlevel[(x * 2 * dim * 2 + y * 2 + 1) * 4 + 0] / 4;
					level[(x * dim + y) * 4 + 1] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 1] / 4
						+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2) * 4 + 1] / 4
						+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2 + 1) * 4 + 1] / 4
						+ prevlevel[(x * 2 * dim * 2 + y * 2 + 1) * 4 + 1] / 4;
					level[(x * dim + y) * 4 + 2] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 2] / 4
						+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2) * 4 + 2] / 4
						+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2 + 1) * 4 + 2] / 4
						+ prevlevel[(x * 2 * dim * 2 + y * 2 + 1) * 4 + 2] / 4;
				}
				level[(x * dim + y) * 4 + 0] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 0] / 4
					+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2) * 4 + 0] / 4
					+ prevlevel[((x * 2 + 1) * dim * 2 + 0) * 4 + 0] / 4
					+ prevlevel[(x * 2 * dim * 2 + 0) * 4 + 0] / 4;
				level[(x * dim + y) * 4 + 1] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 1] / 4
					+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2) * 4 + 1] / 4
					+ prevlevel[((x * 2 + 1) * dim * 2 + 0) * 4 + 1] / 4
					+ prevlevel[(x * 2 * dim * 2 + 0) * 4 + 1] / 4;
				level[(x * dim + y) * 4 + 2] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 2] / 4
					+ prevlevel[((x * 2 + 1) * dim * 2 + y * 2) * 4 + 2] / 4
					+ prevlevel[((x * 2 + 1) * dim * 2 + 0) * 4 + 2] / 4
					+ prevlevel[(x * 2 * dim * 2 + 0) * 4 + 2] / 4;
			}
			int y;
			for (y = 0; y < dim - 1; y++)
			{
				level[(x * dim + y) * 4 + 0] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 0] / 4
					+ prevlevel[(0 + y * 2) * 4 + 0] / 4
					+ prevlevel[(0 + y * 2 + 1) * 4 + 0] / 4
					+ prevlevel[(x * 2 * dim * 2 + y * 2 + 1) * 4 + 0] / 4;
				level[(x * dim + y) * 4 + 1] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 1] / 4
					+ prevlevel[(0 + y * 2) * 4 + 1] / 4
					+ prevlevel[(0 + y * 2 + 1) * 4 + 1] / 4
					+ prevlevel[(x * 2 * dim * 2 + y * 2 + 1) * 4 + 1] / 4;
				level[(x * dim + y) * 4 + 2] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 2] / 4
					+ prevlevel[(0 + y * 2) * 4 + 2] / 4
					+ prevlevel[(0 + y * 2 + 1) * 4 + 2] / 4
					+ prevlevel[(x * 2 * dim * 2 + y * 2 + 1) * 4 + 2] / 4;
			}
			level[(x * dim + y) * 4 + 0] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 0] / 4
				+ prevlevel[(0 + y * 2) * 4 + 0] / 4
				+ prevlevel[(0) * 4 + 0] / 4
				+ prevlevel[(x * 2 * dim * 2 + 0) * 4 + 0] / 4;
			level[(x * dim + y) * 4 + 1] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 1] / 4
				+ prevlevel[(0 + y * 2) * 4 + 1] / 4
				+ prevlevel[1] / 4
				+ prevlevel[(x * 2 * dim * 2 + 0) * 4 + 1] / 4;
			level[(x * dim + y) * 4 + 2] = prevlevel[(x * 2 * dim * 2 + y * 2) * 4 + 2] / 4
				+ prevlevel[(0 + y * 2) * 4 + 2] / 4
				+ prevlevel[2] / 4
				+ prevlevel[(x * 2 * dim * 2 + 0) * 4 + 2] / 4;
		}
		// why am I having to send this as RGBA? Because it's reading 4 bytes even though I
		// specify RGB...but only on the 5th level mipmap. ATI & NVidia
		// BGRA would probably be a bit faster
		glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, dim, dim, GL_RGBA, GL_UNSIGNED_BYTE, level);
		delete [] prevlevel;
		prevlevel = level;
	}
	delete [] level;


	// clear to water color
	glClearColor(.169, .412, .396, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// infinite clip plane
	float p[4][4];
	p[1][0] = p[2][0] = p[3][0] = p[0][1] = p[2][1] = p[3][1] = p[0][2] = p[1][2] = p[0][3] = p[1][3] = p[3][3] = 0;
	p[0][0] = (1 / tan((float)60 * .5 * PIOVER180)) / ((float)wsizex / (float)wsizey);
	p[1][1] = (1 / tan((float)60 * .5 * PIOVER180));
	p[3][2] = (float)-2 * 1.0; // near clip
	p[2][2] = p[2][3] = -1;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float *)p);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-camera.rot.v[0], 1, 0, 0);
	glRotatef(-camera.rot.v[1], 0, 1, 0);

	glTranslatef(-camera.pos.v[0], -camera.pos.v[1], -camera.pos.v[2]);

	GetMatrixFrustum();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


/*	glColor4f(1, .7, .7, 1);
	glBegin(GL_QUADS);
	glVertex4f(-100000, height, 100000, 1);
	glVertex4f(100000, height, 100000, 1);
	glVertex4f(100000, height, -100000, 1);
	glVertex4f(-100000, height, -100000, 1);
	glEnd();*/


	WaterVertex *point;
	unsigned int *pindex;
	int picount = 0;

//	SetupGrid2D(point, pindex, picount);
	SetupGrid3D(point, pindex, picount);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// infinite clip plane
	p[1][0] = p[2][0] = p[3][0] = p[0][1] = p[2][1] = p[3][1] = p[0][2] = p[1][2] = p[0][3] = p[1][3] = p[3][3] = 0;
	p[0][0] = (1 / tan((float)70 * .5 * PIOVER180)) / ((float)wsizex / (float)wsizey);
	p[1][1] = (1 / tan((float)70 * .5 * PIOVER180));
	p[3][2] = (float)-2 * 1.0; // near clip
	p[2][2] = p[2][3] = -1;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float *)p);

	glMatrixMode(GL_MODELVIEW);

	glViewport(0, 0, reflectiontexsizex, reflectiontexsizey);
	double cp[4];
	cp[0] = 0;
	cp[1] = -1;
	cp[2] = 0;
	cp[3] = height + 0; // since the surface is raised
	glPushMatrix();

	glLoadIdentity();
	glRotatef(-camera.rot.v[0], 1, 0, 0);
	glRotatef(-camera.rot.v[1], 0, 1, 0);
	glClipPlane(GL_CLIP_PLANE0, cp);

	glLoadIdentity();

	glRotatef(-camera.rot.v[0], 1, 0, 0);
	glRotatef(-camera.rot.v[1], 0, 1, 0);

//	glScalef(1, -1, 1);

	glTranslatef(-camera.pos.v[0], (camera.pos.v[1] - height), -camera.pos.v[2]);

	glScalef(1, -1, 1);

	glEnable(GL_CLIP_PLANE0);

	RenderSkybox(cubemap);

	glDisable(GL_CLIP_PLANE0);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, reflection);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, reflectiontexsizex, reflectiontexsizey);

//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	RenderSkybox(cubemap);
//	glBindTexture(GL_TEXTURE_2D, refraction);
//	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, reflectiontexsizex, reflectiontexsizey);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, wsizex, wsizey);
	RenderSkybox(cubemap);


	if (wirewater)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


//	glActiveTextureARB(GL_TEXTURE2_ARB);
//	glBindTexture(GL_TEXTURE_2D, refraction);

	glActiveTextureARB(GL_TEXTURE1_ARB);
//	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
//	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, reflection);

	glActiveTextureARB(GL_TEXTURE0_ARB);
//	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
//	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, waternormaltex);
//	glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0);



	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, watervp1);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, waterfp1);
	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);

	glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, camera.pos.v[0], camera.pos.v[1], camera.pos.v[2], 1);

	// texscale because reflection texture has a wider fov
	float texscale = tanf(60 * .5 * PIOVER180) / tanf(70 * .5 * PIOVER180);
	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, (float)1 / wsizex * texscale, (float)1 / wsizey * texscale, 10, 1); // z is inverse scale
	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, (1 - texscale) * .5, (1 - texscale) * .5, 0, 0);
	// water color. Clear to this when drawing reflection texture
	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, .169, .412, .396, 1);
	// direction to light
//	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, 0, .9396926, .3420201, 1);
	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, 0, sinf(32 * PIOVER180), cosf(32 * PIOVER180), 1);
	// specular color
	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 4, 1, .9, .8, 1);


	glColor4f(1, 1, 1, 1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(WaterVertex), point[0].pos);
	glNormalPointer(GL_FLOAT, sizeof(WaterVertex), point[0].normal);
	glTexCoordPointer(2, GL_FLOAT, sizeof(WaterVertex), point[0].texcoord0);
	glDrawRangeElementsEXT(GL_TRIANGLE_STRIP, 0, picount, picount, GL_UNSIGNED_INT, pindex);
//	glDrawElements(GL_TRIANGLE_STRIP, picount, GL_UNSIGNED_INT, pindex);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);


	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0);


	glColor4f(1, 1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	delete [] point;
	delete [] pindex;


// heightmap and normalmap
/*	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, wsizex, 0, wsizey, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glRasterPos2d(0, 0);
	glDrawPixels(heightdim, heightdim, GL_LUMINANCE, GL_FLOAT, heightmap);
	glRasterPos2d(heightdim, 0);
	glDrawPixels(heightdim, heightdim, GL_RGB, GL_FLOAT, normalmap);*/





	glPushMatrix();
	glLoadIdentity();
	RECT window;
	//GetClientRect(hWnd, &window);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, wsizex, 0, wsizey, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_CULL_FACE);
	glAlphaFunc(GL_GREATER, .5);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, texture[fonttex]);


	Print(wsizex - 36, wsizey - 16, "%.0f", averagefps);
//	Print(wsizex - 120, wsizey - 16 * 2, "%.3f %.3f %.3f", ortho.up.v[0], ortho.up.v[1], ortho.up.v[2]);
//	Print(wsizex - 120, wsizey - 16 * 3, "%.3f %.3f %.3f", ortho.right.v[0], ortho.right.v[1], ortho.right.v[2]);



	glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void ShutdownRender()
{
	delete [] noise1;
	delete [] noise2;
	delete [] noise3;
	delete [] noise4;

	delete [] heightmap;
	delete [] h0r;
	delete [] h0i;
	delete [] kv;
	delete [] skg;
	delete [] normalmap;

	delete [] normaltexdata;

	delete [] workarea1;
	delete [] workarea2;
	delete [] workarea3;
}

float Phillips(float a, float k[2], float wind[2])
{
	float k2 = k[0] * k[0] + k[1] * k[1];
	if (k2 == 0)
		return 0;
	float v2 = wind[0] * wind[0] + wind[1] * wind[1];
	float l = v2 / gravity;
	return a * expf(-1 / (k2 * l * l)) / (k2 * k2) * 
		(k[0] * wind[0] + k[1] * wind[1]) * (k[0] * wind[0] + k[1] * wind[1]) / (k2 * v2);
}

void SetupRender()
{
//	InitExtensions();

	camera.pos.set(0, 50, 0);
	camera.rot.v[1] = 0;

	sgenrand(116);


	heightmap = new float [heightdim * heightdim];
	h0r = new float [heightdim * heightdim];
	h0i = new float [heightdim * heightdim];
	kv = new float [heightdim * heightdim * 2];
	skg = new float [heightdim * heightdim];
	normalmap = new float [heightdim * heightdim * 3];

	normaltexdata = new unsigned char [heightdim * heightdim * 3];

	for (int x = 0; x < heightdim; x++)
	{
		for (int y = 0; y < heightdim; y++)
		{
			kv[(x * heightdim + y) * 2 + 0] = ((float)x - heightdim / 2) * TWOPI / heightextx;
			kv[(x * heightdim + y) * 2 + 1] = ((float)y - heightdim / 2) * TWOPI / heightexty;
//			kv[(x * heightdim + y) * 2 + 0] = (x) * TWOPI / heightextx;
//			kv[(x * heightdim + y) * 2 + 1] = (y) * TWOPI / heightexty;
			skg[x * heightdim + y] = sqrtf(sqrtf(kv[(x * heightdim + y) * 2 + 0] * kv[(x * heightdim + y) * 2 + 0] + kv[(x * heightdim + y) * 2 + 1] * kv[(x * heightdim + y) * 2 + 1]) * gravity);
			float ph = sqrtf(Phillips(waveamp, &kv[(x * heightdim + y) * 2], wind));
			h0r[x * heightdim + y] = INVSQRT2 * gaussian() * ph;
			h0i[x * heightdim + y] = INVSQRT2 * gaussian() * ph;
/*			float gr = gaussian();
			float ur = genrand() * TWOPI;
			h0r[x * heightdim + y] = gr * cosf(ur) * ph;
			h0i[x * heightdim + y] = gr * sinf(ur) * ph;*/
		}
	}

	workarea1 = new float [heightdim * 8];
	workarea2 = new int [2 + (int)sqrt(heightdim + .5)];
	workarea2[0] = 0;
	workarea3 = new float [heightdim / 2];




	glGenTextures(1, &waternormaltex);
	glBindTexture(GL_TEXTURE_2D, waternormaltex);
	int dim = heightdim;
	for (int i = 0; dim > 0; i++)
	{
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGB8, dim, dim, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		dim /= 2;
	}
/*	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, heightdim, heightdim, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB8, heightdim / 2, heightdim / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 2, GL_RGB8, heightdim / 4, heightdim / 4, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 3, GL_RGB8, heightdim / 8, heightdim / 8, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 4, GL_RGB8, heightdim / 16, heightdim / 16, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 5, GL_RGB8, heightdim / 32, heightdim / 32, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 6, GL_RGB8, heightdim / 64, heightdim / 64, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 7, GL_RGB8, heightdim / 128, heightdim / 128, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);*/
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 1000);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &reflection);
	glBindTexture(GL_TEXTURE_2D, reflection);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, reflectiontexsizex, reflectiontexsizey, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &refraction);
	glBindTexture(GL_TEXTURE_2D, refraction);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, reflectiontexsizex, reflectiontexsizey, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



	noise4 = new float [noisedim * noisedim * noisedim];
	noise3 = new float [noisedim / 2 * noisedim / 2 * noisedim / 2];
	noise2 = new float [noisedim / 4 * noisedim / 4 * noisedim / 4];
	noise1 = new float [noisedim / 8 * noisedim / 8 * noisedim / 8];

	Noise3DInit(noise4, noisedim, noisedim, noisedim);
	Noise3DInit(noise3, noisedim / 2, noisedim / 2, noisedim / 2);
	Noise3DInit(noise2, noisedim / 4, noisedim / 4, noisedim / 4);
	Noise3DInit(noise1, noisedim / 8, noisedim / 8, noisedim / 8);


	fonttex = CreateTexture("font.tga", false, false);
	BuildFont("font", fonttex, fontbase);


	LoadCubemap("cube", cubemap);



	char program[10000];
	FILE *file;
	FILE *shaderlog = fopen("shaderlog.txt", "wt");

	int maxinstructions, maxtemps, maxparams, maxattribs, maxalu, maxtex, maxindirect;
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &maxinstructions);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, &maxtemps);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, &maxparams);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, &maxattribs);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, &maxalu);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, &maxtex);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, &maxindirect);

	fprintf(shaderlog, "Fragment Program Native Maximums:\nInstructions: %d\nTemps: %d\nParams: %d\nAttribs: %d\nALU Instructions: %d\nTEX Instructions: %d\nTex Indirections: %d\n\n",
		maxinstructions, maxtemps, maxparams, maxattribs, maxalu, maxtex, maxindirect);

	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &maxinstructions);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, &maxtemps);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, &maxparams);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, &maxattribs);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, &maxalu);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, &maxtex);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, &maxindirect);

	fprintf(shaderlog, "Vertex Program Native Maximums:\nInstructions: %d\nTemps: %d\nParams: %d\nAttribs: %d\nALU Instructions: %d\nTEX Instructions: %d\nTex Indirections: %d\n\n",
		maxinstructions, maxtemps, maxparams, maxattribs, maxalu, maxtex, maxindirect);



	memset(program, 0, 10000);
	file = fopen("shader\\waterfp1.txt", "rt");
	fread(program, filelength(file->_file), 1, file);
	fclose(file);
	glGenProgramsARB(1, &waterfp1);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, waterfp1);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);

	unsigned char *error;
	error = (unsigned char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);

	int instructions, temps, params, attribs, alu, tex, indirect;
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &instructions);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEMPORARIES_ARB, &temps);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_PARAMETERS_ARB, &params);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_ATTRIBS_ARB, &attribs);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, &alu);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, &tex);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, &indirect);

	int errorposition = 0;
//	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_ERROR_POSITION_ARB, &errorposition);
	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorposition);

	fprintf(shaderlog, "Shader waterfp1.txt\n\n");
	fprintf(shaderlog, "Errors:\n%s\nLocation: %d\n\n", error, errorposition);
	fprintf(shaderlog, "Native Resources:\nInstructions: %d\nTemps: %d\nParams: %d\nAttribs: %d\nALU Instructions: %d\nTEX Instructions: %d\nTex Indirections: %d\n\n",
		instructions, temps, params, attribs, alu, tex, indirect);



	memset(program, 0, 10000);
	file = fopen("shader\\watervp1.txt", "rt");
	fread(program, filelength(file->_file), 1, file);
	fclose(file);
	glGenProgramsARB(1, &watervp1);
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, watervp1);
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(program), program);

	error = (unsigned char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);

	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &instructions);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEMPORARIES_ARB, &temps);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_PARAMETERS_ARB, &params);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ATTRIBS_ARB, &attribs);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, &alu);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, &tex);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, &indirect);

	errorposition = 0;
//	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ERROR_POSITION_ARB, &errorposition);
	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorposition);

	fprintf(shaderlog, "Shader watervp1.txt\n\n");
	fprintf(shaderlog, "Errors:\n%s\nLocation: %d\n\n", error, errorposition);
	fprintf(shaderlog, "Native Resources:\nInstructions: %d\nTemps: %d\nParams: %d\nAttribs: %d\nALU Instructions: %d\nTEX Instructions: %d\nTex Indirections: %d\n\n",
		instructions, temps, params, attribs, alu, tex, indirect);



	
	fclose(shaderlog);
}

int CreateTexture(char filename[], bool clamp, bool duplicate)
{
	if (!filename)
		return -1;

	if (duplicate == false)
	{
		for (int n = 0; n < textures; n++)
		{
			if (stricmp(filename, texinfo[n].filename) == 0)
				return n;
		}
	}

	texture = (unsigned int*)realloc(texture, sizeof(unsigned int) * (textures + 1));
	texinfo = (TexInfo*)realloc(texinfo, sizeof(TexInfo) * (textures + 1));


	char fullname[256];
	sprintf(fullname, "textures\\%s", filename);
	FILE *file = 0;
	file = fopen(fullname, "rb");
	if (!file)
		return -1;

	byte tgadheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
	byte tgaheader[12];
	byte header[6];
	int bpp, bytespp, type, width, height, size;
	byte *data;

//	fread(tgaheader, 1, sizeof(tgadheader), file);
	if (!(fread(tgaheader, 1, sizeof(tgadheader), file) == sizeof(tgadheader) && (memcmp(tgaheader, tgadheader, sizeof(tgaheader)) == 0)))
		return -1;

	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		return -1;
	width = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];

	if (width < 1 || height < 1)
		return -1;
	bpp = header[4];
	if (bpp == 24)
		type = GL_BGR;
	else if (bpp == 32)
		type = GL_BGRA;
	else
		type = GL_LUMINANCE;
	bytespp = bpp / 8;
	size = width * height * bytespp;
	data = new byte [size];
	if (data == NULL || (fread(data, 1, size, file) != (unsigned)size))
		return -1;
/*	for (int n = 0; n < size; n+=bytespp)
	{
		int temp;
		temp = data[n];
		data[n] = data[n+2];
		data[n+2] = temp;
	}*/

	fclose(file);

	glGenTextures(1, &texture[textures]);
	glBindTexture(GL_TEXTURE_2D, texture[textures]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, bytespp, width, height, type, GL_UNSIGNED_BYTE, data);
//	glTexImage2D(GL_TEXTURE_2D, 0, bytespp, width, height, 0, type, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (clamp)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	delete [] data;

	strcpy(texinfo[textures].filename, filename);
	texinfo[textures].width = width;
	texinfo[textures].height = height;

	textures++;

	return textures - 1;
}

bool LoadCubemap(char name[], unsigned int &texid)
{
	char fullname[256];
	sprintf(fullname, "textures\\%s", name);
	char filename[256];

	FILE *file = 0;

	byte tgadheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
	byte tgaheader[12];
	byte header[6];
	int bpp, bytespp, type, width, height, size;
	byte *data;

	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, texid);
//	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	strcpy(filename, fullname);
	strcat(filename, "px.tga");
	file = fopen(filename, "rb");
	if (!file)
		return 0;

//	fread(tgaheader, 1, sizeof(tgadheader), file);
	if (!(fread(tgaheader, 1, sizeof(tgadheader), file) == sizeof(tgadheader) && (memcmp(tgaheader, tgadheader, sizeof(tgaheader)) == 0)))
		return 0;

	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		return 0;
	width = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];

	if (width < 1 || height < 1)
		return -1;
	bpp = header[4];
	if (bpp == 24)
		type = GL_BGR;
	else if (bpp == 32)
		type = GL_BGRA;
	else
		type = GL_LUMINANCE;
	bytespp = bpp / 8;
	size = width * height * bytespp;
	data = new byte [size];
	if (data == NULL || (fread(data, 1, size, file) != (unsigned)size))
		return -1;
/*	for (int n = 0; n < size; n+=bytespp)
	{
		int temp;
		temp = data[n];
		data[n] = data[n+2];
		data[n+2] = temp;
	}*/

	fclose(file);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0, bytespp, 
			   width, height, 0, type,
			   GL_UNSIGNED_BYTE, data);
	delete [] data;

	strcpy(filename, fullname);
	strcat(filename, "py.tga");
	file = fopen(filename, "rb");
	if (!file)
		return 0;

//	fread(tgaheader, 1, sizeof(tgadheader), file);
	if (!(fread(tgaheader, 1, sizeof(tgadheader), file) == sizeof(tgadheader) && (memcmp(tgaheader, tgadheader, sizeof(tgaheader)) == 0)))
		return 0;

	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		return 0;
	width = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];

	if (width < 1 || height < 1)
		return -1;
	bpp = header[4];
	if (bpp == 24)
		type = GL_BGR;
	else if (bpp == 32)
		type = GL_BGRA;
	else
		type = GL_LUMINANCE;
	bytespp = bpp / 8;
	size = width * height * bytespp;
	data = new byte [size];
	if (data == NULL || (fread(data, 1, size, file) != (unsigned)size))
		return -1;
/*	for (int n = 0; n < size; n+=bytespp)
	{
		int temp;
		temp = data[n];
		data[n] = data[n+2];
		data[n+2] = temp;
	}*/

	fclose(file);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 0, bytespp, 
			   width, height, 0, type,
			   GL_UNSIGNED_BYTE, data);
	delete [] data;

	strcpy(filename, fullname);
	strcat(filename, "pz.tga");
	file = fopen(filename, "rb");
	if (!file)
		return 0;

//	fread(tgaheader, 1, sizeof(tgadheader), file);
	if (!(fread(tgaheader, 1, sizeof(tgadheader), file) == sizeof(tgadheader) && (memcmp(tgaheader, tgadheader, sizeof(tgaheader)) == 0)))
		return 0;

	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		return 0;
	width = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];

	if (width < 1 || height < 1)
		return -1;
	bpp = header[4];
	if (bpp == 24)
		type = GL_BGR;
	else if (bpp == 32)
		type = GL_BGRA;
	else
		type = GL_LUMINANCE;
	bytespp = bpp / 8;
	size = width * height * bytespp;
	data = new byte [size];
	if (data == NULL || (fread(data, 1, size, file) != (unsigned)size))
		return -1;
/*	for (int n = 0; n < size; n+=bytespp)
	{
		int temp;
		temp = data[n];
		data[n] = data[n+2];
		data[n+2] = temp;
	}*/

	fclose(file);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 0, bytespp, 
			   width, height, 0, type,
			   GL_UNSIGNED_BYTE, data);
	delete [] data;

	strcpy(filename, fullname);
	strcat(filename, "nx.tga");
	file = fopen(filename, "rb");
	if (!file)
		return 0;

//	fread(tgaheader, 1, sizeof(tgadheader), file);
	if (!(fread(tgaheader, 1, sizeof(tgadheader), file) == sizeof(tgadheader) && (memcmp(tgaheader, tgadheader, sizeof(tgaheader)) == 0)))
		return 0;

	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		return 0;
	width = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];

	if (width < 1 || height < 1)
		return -1;
	bpp = header[4];
	if (bpp == 24)
		type = GL_BGR;
	else if (bpp == 32)
		type = GL_BGRA;
	else
		type = GL_LUMINANCE;
	bytespp = bpp / 8;
	size = width * height * bytespp;
	data = new byte [size];
	if (data == NULL || (fread(data, 1, size, file) != (unsigned)size))
		return -1;
/*	for (int n = 0; n < size; n+=bytespp)
	{
		int temp;
		temp = data[n];
		data[n] = data[n+2];
		data[n+2] = temp;
	}*/

	fclose(file);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 0, bytespp, 
			   width, height, 0, type,
			   GL_UNSIGNED_BYTE, data);
	delete [] data;

	strcpy(filename, fullname);
	strcat(filename, "ny.tga");
	file = fopen(filename, "rb");
	if (!file)
		return 0;

//	fread(tgaheader, 1, sizeof(tgadheader), file);
	if (!(fread(tgaheader, 1, sizeof(tgadheader), file) == sizeof(tgadheader) && (memcmp(tgaheader, tgadheader, sizeof(tgaheader)) == 0)))
		return 0;

	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		return 0;
	width = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];

	if (width < 1 || height < 1)
		return -1;
	bpp = header[4];
	if (bpp == 24)
		type = GL_BGR;
	else if (bpp == 32)
		type = GL_BGRA;
	else
		type = GL_LUMINANCE;
	bytespp = bpp / 8;
	size = width * height * bytespp;
	data = new byte [size];
	if (data == NULL || (fread(data, 1, size, file) != (unsigned)size))
		return -1;
/*	for (int n = 0; n < size; n+=bytespp)
	{
		int temp;
		temp = data[n];
		data[n] = data[n+2];
		data[n+2] = temp;
	}*/

	fclose(file);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 0, bytespp, 
			   width, height, 0, type,
			   GL_UNSIGNED_BYTE, data);
	delete [] data;

	strcpy(filename, fullname);
	strcat(filename, "nz.tga");
	file = fopen(filename, "rb");
	if (!file)
		return 0;

//	fread(tgaheader, 1, sizeof(tgadheader), file);
	if (!(fread(tgaheader, 1, sizeof(tgadheader), file) == sizeof(tgadheader) && (memcmp(tgaheader, tgadheader, sizeof(tgaheader)) == 0)))
		return 0;

	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		return 0;
	width = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];

	if (width < 1 || height < 1)
		return -1;
	bpp = header[4];
	if (bpp == 24)
		type = GL_BGR;
	else if (bpp == 32)
		type = GL_BGRA;
	else
		type = GL_LUMINANCE;
	bytespp = bpp / 8;
	size = width * height * bytespp;
	data = new byte [size];
	if (data == NULL || (fread(data, 1, size, file) != (unsigned)size))
		return -1;
/*	for (int n = 0; n < size; n+=bytespp)
	{
		int temp;
		temp = data[n];
		data[n] = data[n+2];
		data[n+2] = temp;
	}*/

	fclose(file);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 0, bytespp, 
			   width, height, 0, type,
			   GL_UNSIGNED_BYTE, data);
	delete [] data;


	return true;
}

void Resize(int width, int height, int xoffset, int yoffset, float fov)
{
	if (height <= 0)
		height = 1;
	if (width <= 0)
		width = 1;

	glViewport(xoffset, yoffset, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

//	gluPerspective(fov, (float)width / height, 1, 5000);
	// infinite clip plane
	float p[4][4];
	p[1][0] = p[2][0] = p[3][0] = p[0][1] = p[2][1] = p[3][1] = p[0][2] = p[1][2] = p[0][3] = p[1][3] = p[3][3] = 0;
	p[0][0] = (1 / tan((float)fov * .5 * PIOVER180)) / ((float)wsizex / (float)wsizey);
	p[1][1] = (1 / tan((float)fov * .5 * PIOVER180));
	p[3][2] = (float)-2 * 1.0; // near clip
	p[2][2] = p[2][3] = -1;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float *)p);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

}