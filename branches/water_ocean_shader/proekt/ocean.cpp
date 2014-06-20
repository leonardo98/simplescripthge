#include "ocean.h"
#include "vector3f.h"
#include "mat4.h"

// VBO Extension Function Pointers

#define IRRADIANCE_UNIT 0
#define INSCATTER_UNIT 1
#define TRANSMITTANCE_UNIT 2
#define SKY_UNIT 3
#define WAVE_UNIT 4
#define NOISE_UNIT 5
#ifndef M_PI
#define M_PI 3.14159265
#endif

void PushQuad(vector3f *buff, vector3f pos, float w, float h, UV *uv, float u1, float v1, float u2, float v2)
{
	*buff = pos;
	++buff;
	*buff = pos; buff->x += w;
	++buff;
	*buff = pos; buff->z += h;
	++buff;
	*buff = pos; buff->z += h; buff->x += w;
	++buff;
	*buff = pos; buff->z += h;
	++buff;
	*buff = pos; buff->x += w;

	uv->u = u1; uv->v = v1;
	++uv;
	uv->u = u2; uv->v = v1;
	++uv;
	uv->u = u1; uv->v = v2;
	++uv;
	uv->u = u2; uv->v = v2;
	++uv;
	uv->u = u1; uv->v = v2;
	++uv;
	uv->u = u2; uv->v = v1;
}


long lrandom(long *seed)
{
    *seed = (*seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return *seed;
}

float frandom(long *seed)
{
    long r = lrandom(seed) >> (31 - 24);
    return r / (float)(1 << 24);
}

inline float grandom(float mean, float stdDeviation, long *seed)
{
    float x1, x2, w, y1;
    static float y2;
    static int use_last = 0;

    if (use_last) {
        y1 = y2;
        use_last = 0;
    } else {
        do {
            x1 = 2.0f * frandom(seed) - 1.0f;
            x2 = 2.0f * frandom(seed) - 1.0f;
            w  = x1 * x1 + x2 * x2;
        } while (w >= 1.0f);
        w  = sqrt((-2.0f * log(w)) / w);
        y1 = x1 * w;
        y2 = x2 * w;
        use_last = 1;
    }
	return mean + y1 * stdDeviation;
}

#define srnd() (2*frandom(&seed) - 1)


Ocean::~Ocean()
{
}

Ocean::Ocean(float w, float h)
{
	time = 0.f;

	width = w;
	height = h;

	render = NULL;
	sky = NULL;
	skymap = NULL;
	clouds = NULL;
	skyTexSize = 512;
	skyTex;
	noiseTex;
	cloudLayer = false;
	octaves = 10.0;
	lacunarity = 2.2;
	gain = 0.7;
	norm = 0.5;
	clamp1 = -0.15;
	clamp2 = 0.2;
	cloudColor[0] = cloudColor[1] = cloudColor[2] = cloudColor[3] = 1.0; 
	fbo;
	vbo;
	vboIndices;
	vboParams;
	vboSize = 0;
	sunTheta = M_PI / 2.0 - 0.05;
	sunPhi = 0.0;
	cameraHeight = 1.8f;
	cameraTheta = 0.0;

	// RENDERING OPTIONS
	gridSize = 8.0;
	nyquistMin = 1.0;
	nyquistMax = 1.5;

	seaColor[0] = 10.0 / 255.0;
	seaColor[1] = 40.0 / 255.0;
	seaColor[2] = 120.0 / 255.0;
	seaColor[3] = 0.1;

	hdrExposure = 0.4;
	grid = false;
	animate = true;
	seaContrib = true;
	sunContrib = true;
	skyContrib = true;
	manualFilter = false;

	// WAVES PARAMETERS (INPUT)
	waveTex;
	nbWaves = 60;
	waves = NULL;
	lambdaMin = 0.02;
	lambdaMax = 30.0;
	heightMax = 0.32;
	waveDirection = 2.4;
	U0 = 10.0;
	waveDispersion = 1.25f;

	// WAVE STATISTICS (OUTPUT)
	sigmaXsq = 0.0;
	sigmaYsq = 0.0;
	meanHeight = 0.0;
	heightVariance = 0.0;
	amplitudeMax = 0.0;
	
	
	float *data;
	FILE *f;

    int res = 64;
    int nr = res / 2;
    int nv = res * 2;
    int nb = res / 2;
    int na = 8;
    f = fopen("data/inscatter.raw", "rb");
    data = new float[nr*nv*nb*na*4];
    fread(data, 1, nr*nv*nb*na*4*sizeof(float), f);
    fclose(f);
    glActiveTexture(GL_TEXTURE0 + INSCATTER_UNIT);
    glGenTextures(1, &inscatterTex);
    glBindTexture(GL_TEXTURE_3D, inscatterTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F_ARB, na*nb, nv, nr, 0, GL_RGBA, GL_FLOAT, data);
    delete[] data;

    data = new float[256*64*3];
    f = fopen("data/transmittance.raw", "rb");
    fread(data, 1, 256*64*3*sizeof(float), f);
    fclose(f);
    glActiveTexture(GL_TEXTURE0 + TRANSMITTANCE_UNIT);
    glGenTextures(1, &transmittanceTex);
    glBindTexture(GL_TEXTURE_2D, transmittanceTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, 256, 64, 0, GL_RGB, GL_FLOAT, data);
    delete[] data;

    float maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

    unsigned char* img = new unsigned char[512 * 512 + 38];
    f = fopen("data/noise.pgm", "rb");
    fread(img, 1, 512 * 512 + 38, f);
    fclose(f);
    glActiveTexture(GL_TEXTURE0 + NOISE_UNIT);
    glGenTextures(1, &noiseTex);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 512, 512, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img + 38);
    glGenerateMipmapEXT(GL_TEXTURE_2D);
    delete[] img;

    glActiveTexture(GL_TEXTURE0 + SKY_UNIT);
    glGenTextures(1, &skyTex);
    glBindTexture(GL_TEXTURE_2D, skyTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, skyTexSize, skyTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmapEXT(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0 + WAVE_UNIT);
    glGenTextures(1, &waveTex);
    glBindTexture(GL_TEXTURE_1D, waveTex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    generateWaves();

    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    generateMesh();

	loadPrograms(true);
}

void Ocean::generateWaves()
{
    long seed = 1234567;
    float min = log(lambdaMin) / log(2.0f);
    float max = log(lambdaMax) / log(2.0f);

    sigmaXsq = 0.0;
    sigmaYsq = 0.0;
    meanHeight = 0.0;
    heightVariance = 0.0;
    amplitudeMax = 0.0;

    if (waves != NULL) {
        delete[] waves;
    }
    waves = new vec4f[nbWaves];

	#define nbAngles 5 // even
	#define angle(i) (1.5*(((i)%nbAngles)/(float)(nbAngles/2)-1))
	#define dangle() (1.5/(float)(nbAngles/2))

	float Wa[nbAngles]; // normalised gaussian samples
    int index[nbAngles]; // to hash angle order
    float s=0;
    for (int i = 0; i < nbAngles; i++) {
        index[i] = i;
        float a = angle(i);
        s += Wa[i] = exp(-0.5*a*a);
    }
    for (int i = 0; i < nbAngles; i++) {
        Wa[i] /= s;
    }

    for (int i = 0; i < nbWaves; ++i) {
        float x = i / (nbWaves - 1.0f);

        float lambda = pow(2.0f, (1.0f - x) * min + x * max);
        float ktheta = grandom(0.0f, 1.0f, &seed) * waveDispersion;
        float knorm = 2.0f * M_PI / lambda;
        float omega = sqrt(9.81f * knorm);
        float amplitude;

        float step = (max-min) / (nbWaves-1); // dlambda/di
        float omega0 = 9.81 / U0;
        if ((i%(nbAngles)) == 0) { // scramble angle ordre
            for (int k = 0; k < nbAngles; k++) {   // do N swap in indices
                int n1 = lrandom(&seed)%nbAngles, n2 = lrandom(&seed)%nbAngles, n;
                n = index[n1];
				index[n1] = index[n2];
				index[n2] = n;
            }
        }
        ktheta = waveDispersion * (angle(index[(i)%nbAngles]) + 0.4*srnd()*dangle());
        ktheta *= 1.0 / (1.0 + 40.0*pow(omega0/omega,4));
        amplitude = (8.1e-3*9.81*9.81) / pow(omega,5) * exp(-0.74*pow(omega0/omega,4));
        amplitude *= 0.5*sqrt(2*M_PI*9.81/lambda)*nbAngles*step;
        amplitude = 3*heightMax*sqrt(amplitude);

        if (amplitude > 1.0f / knorm) {
            amplitude = 1.0f / knorm;
        } else if (amplitude < -1.0f / knorm) {
            amplitude = -1.0f / knorm;
        }

        waves[i].x = amplitude;
        waves[i].y = omega;
        waves[i].z = knorm * cos(ktheta);
        waves[i].w = knorm * sin(ktheta);
        sigmaXsq += pow(cos(ktheta), 2.0f) * (1.0 - sqrt(1.0 - knorm * knorm * amplitude * amplitude));
        sigmaYsq += pow(sin(ktheta), 2.0f) * (1.0 - sqrt(1.0 - knorm * knorm * amplitude * amplitude));
        meanHeight -= knorm * amplitude * amplitude * 0.5f;
        heightVariance += amplitude * amplitude * (2.0f - knorm * knorm * amplitude * amplitude) * 0.25f;
        amplitudeMax += fabs(amplitude);
    }

    float var = 4.0f;
    amplitudeMax = 2.0f * var * sqrt(heightVariance);

    glActiveTexture(GL_TEXTURE0 + WAVE_UNIT);
    glBindTexture(GL_TEXTURE_1D, waveTex);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F_ARB, nbWaves, 0, GL_RGBA, GL_FLOAT, waves);
}

void Ocean::generateMesh()
{
    if (vboSize != 0) {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &vboIndices);
    }
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float horizon = tan(cameraTheta);
    float s = min(1.1f, 0.5f + horizon * 0.5f);

    float vmargin = 0.1;
    float hmargin = 0.1;

    vboParams = vec4f(width, height, gridSize, cameraTheta);
    vec4f *data = new vec4f[int(ceil(height * (s + vmargin) / gridSize) + 5) * int(ceil(width * (1.0 + 2.0 * hmargin) / gridSize) + 5)];

    int n = 0;
    int nx = 0;
    for (float j = height * s - 0.1; j > -height * vmargin - gridSize; j -= gridSize) {
        nx = 0;
        for (float i = -width * hmargin; i < width * (1.0 + hmargin) + gridSize; i += gridSize) {
            data[n++] = vec4f(-1.0 + 2.0 * i / width, -1.0 + 2.0 * j / height, 0.0, 1.0);
            nx++;
        }
    }

    glBufferData(GL_ARRAY_BUFFER, n * 16, data, GL_STATIC_DRAW);
    delete[] data;

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ARRAY_BUFFER, vboIndices);

    vboSize = 0;
    GLuint *indices = new GLuint[6 * int(ceil(height * (s + vmargin) / gridSize) + 4) * int(ceil(width * (1.0 + 2.0 * hmargin) / gridSize) + 4)];

    int nj = 0;
    for (float j = height * s - 0.1; j > -height * vmargin; j -= gridSize) {
        int ni = 0;
        for (float i = -width * hmargin; i < width * (1.0 + hmargin); i += gridSize) {
            indices[vboSize++] = ni + (nj + 1) * nx;
            indices[vboSize++] = (ni + 1) + (nj + 1) * nx;
            indices[vboSize++] = (ni + 1) + nj * nx;
            indices[vboSize++] = (ni + 1) + nj * nx;
            indices[vboSize++] = ni + (nj + 1) * nx;
            indices[vboSize++] = ni + nj * nx;
            ni++;
        }
        nj++;
    }

    glBufferData(GL_ARRAY_BUFFER, vboSize * 4, indices, GL_STATIC_DRAW);
    delete[] indices;
}

void Ocean::Draw(float cameraD, float cameraT)
{
    if (vboParams.x != width || vboParams.y != height || vboParams.z != gridSize || vboParams.w != cameraTheta) {
        generateMesh();
    }
    vec4f sun = vec4f(sin(sunTheta) * cos(sunPhi), sin(sunTheta) * sin(sunPhi), cos(sunTheta), 0.0);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, skyTex, 0);
    glViewport(0, 0, skyTexSize, skyTexSize);
    glUseProgram(skymap->program);
    glUniform3f(glGetUniformLocation(skymap->program, "sunDir"), sun.x, sun.y, sun.z);
    glUniform1f(glGetUniformLocation(skymap->program, "octaves"), octaves);
    glUniform1f(glGetUniformLocation(skymap->program, "lacunarity"), lacunarity);
    glUniform1f(glGetUniformLocation(skymap->program, "gain"), gain);
    glUniform1f(glGetUniformLocation(skymap->program, "norm"), norm);
    glUniform1f(glGetUniformLocation(skymap->program, "clamp1"), clamp1);
    glUniform1f(glGetUniformLocation(skymap->program, "clamp2"), clamp2);
    glUniform4f(glGetUniformLocation(skymap->program, "cloudsColor"), cloudColor[0], cloudColor[1], cloudColor[2], cloudColor[3]);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(-1, 1);
    glVertex2f(1, 1);
    glEnd();
    glActiveTexture(GL_TEXTURE0 + SKY_UNIT);
    glGenerateMipmapEXT(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0, 0, width, height);

    float ch = cameraHeight - meanHeight;
	//ch += 3 * sin(time * 1.0);

	float m[4][4];

	//mat4f view = mat4f(
	//	0.0, -1.0, 0.0, 0.0,
	//	0.0, 0.0, 1.0, -ch,
	//	-1.0, 0.0, 0.0, 0.0,
	//	0.0, 0.0, 0.0, 1.0
	//);
	//cameraTheta = -cameraT;
	//view = mat4f::rotatey(cameraD / M_PI * 180.0) * view;
	//view = mat4f::rotatex(cameraTheta / M_PI * 180.0) * view;
	//view = mat4f::rotatey(cameraT / M_PI * 180.0) * view;
	glGetFloatv(GL_MODELVIEW_MATRIX, (float *)m);
	//mat4f view = mat4f(
 //       m[0][0], m[0][1], m[0][2], m[0][3],
 //       m[1][0], m[1][1], m[1][2], m[1][3],
 //       m[2][0], m[2][1], m[2][2], m[2][3],
 //       m[3][0], m[3][1], m[3][2], m[3][3]
 //   );
	//view = mat4f::rotatex(cameraTheta / M_PI * 90.0) * view;
	mat4f view = mat4f(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
	view = view * mat4f::rotatex(90);

	glGetFloatv(GL_PROJECTION_MATRIX, (float *)m);
	mat4f proj = mat4f(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
 //   mat4f proj = mat4f::perspectiveProjection(90.0, float(width) / float(height), 0.1 * ch, 1000000.0 * ch);

    float worldToWind[4];
    worldToWind[0] = cos(waveDirection);
    worldToWind[1] = sin(waveDirection);
    worldToWind[2] = -sin(waveDirection);
    worldToWind[3] = cos(waveDirection);

    float windToWorld[4];
    windToWorld[0] = cos(waveDirection);
    windToWorld[1] = -sin(waveDirection);
    windToWorld[2] = sin(waveDirection);
    windToWorld[3] = cos(waveDirection);

	glUseProgram(render->program);
    glUniformMatrix4fv(glGetUniformLocation(render->program, "screenToCamera"), 1, true, proj.inverse().coefficients());
    glUniformMatrix4fv(glGetUniformLocation(render->program, "cameraToWorld"), 1, true, view.inverse().coefficients());
    glUniformMatrix4fv(glGetUniformLocation(render->program, "worldToScreen"), 1, true, (proj * view).coefficients());
    glUniformMatrix2fv(glGetUniformLocation(render->program, "worldToWind"), 1, true, worldToWind);
    glUniformMatrix2fv(glGetUniformLocation(render->program, "windToWorld"), 1, true, windToWorld);
    glUniform3f(glGetUniformLocation(render->program, "worldCamera"), 0.0, 0.0, ch);
    glUniform3f(glGetUniformLocation(render->program, "worldSunDir"), sun.x, sun.y, sun.z);
    glUniform1f(glGetUniformLocation(render->program, "hdrExposure"), hdrExposure);

    glUniform1f(glGetUniformLocation(render->program, "nbWaves"), nbWaves);
    glUniform1f(glGetUniformLocation(render->program, "heightOffset"), -meanHeight);
    glUniform2f(glGetUniformLocation(render->program, "sigmaSqTotal"), sigmaXsq, sigmaYsq);
    if (animate) {
        glUniform1f(glGetUniformLocation(render->program, "time"), time * 0.5f);
    }

    glUniform4f(glGetUniformLocation(render->program, "lods"),
        gridSize,
        atan(2.0 / height) * gridSize, // angle under which a screen pixel is viewed from the camera * gridSize
        log(lambdaMin) / log(2.0f),
        (nbWaves - 1.0f) / (log(lambdaMax) / log(2.0f) -  log(lambdaMin) / log(2.0f)));
    glUniform1f(glGetUniformLocation(render->program, "nyquistMin"), nyquistMin);
    glUniform1f(glGetUniformLocation(render->program, "nyquistMax"), nyquistMax);

    glUniform3f(glGetUniformLocation(render->program, "seaColor"), seaColor[0] * seaColor[3], seaColor[1] * seaColor[3], seaColor[2] * seaColor[3]);

	// cетка океана
	//{
	//    glPolygonMode(GL_FRONT, GL_LINE);
	//    glPolygonMode(GL_BACK, GL_LINE);
	//}
	{
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glVertexPointer(4, GL_FLOAT, 16, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawElements(GL_TRIANGLES, vboSize, GL_UNSIGNED_INT, 0);
    glDisableClientState(GL_VERTEX_ARRAY);

}

void Ocean::loadPrograms(bool all)
{
    char* files[2];
    char options[512];
    files[0] = "atmosphere.glsl";
    files[1] = "ocean.glsl";
    sprintf(options, "#define %sSEA_CONTRIB\n#define %sSUN_CONTRIB\n#define %sSKY_CONTRIB\n#define %sCLOUDS\n#define %sHARDWARE_ANISTROPIC_FILTERING\n",
        seaContrib ? "" : "NO_", sunContrib ? "" : "NO_", skyContrib ? "" : "NO_", cloudLayer ? "" : "NO_", manualFilter ? "NO_" : "");

    if (render != NULL) {
        delete render;
    }
	render = new Program(2, files, options);
    glUseProgram(render->program);
    glUniform1i(glGetUniformLocation(render->program, "skyIrradianceSampler"), IRRADIANCE_UNIT);
    glUniform1i(glGetUniformLocation(render->program, "inscatterSampler"), INSCATTER_UNIT);
    glUniform1i(glGetUniformLocation(render->program, "transmittanceSampler"), TRANSMITTANCE_UNIT);
    glUniform1i(glGetUniformLocation(render->program, "skySampler"), SKY_UNIT);
    glUniform1i(glGetUniformLocation(render->program, "wavesSampler"), WAVE_UNIT);

    if (!all) {
        return;
    }

    files[0] = "atmosphere.glsl";
    files[1] = "sky.glsl";
    if (sky != NULL) {
        delete sky;
    }
	sky = new Program(2, files, options);
    glUseProgram(sky->program);
    glUniform1i(glGetUniformLocation(sky->program, "skyIrradianceSampler"), IRRADIANCE_UNIT);
    glUniform1i(glGetUniformLocation(sky->program, "inscatterSampler"), INSCATTER_UNIT);
    glUniform1i(glGetUniformLocation(sky->program, "transmittanceSampler"), TRANSMITTANCE_UNIT);
    glUniform1i(glGetUniformLocation(sky->program, "skySampler"), SKY_UNIT);

    files[0] = "atmosphere.glsl";
    files[1] = "skymap.glsl";
    if (skymap != NULL) {
        delete skymap;
    }
	skymap = new Program(2, files, options);
    glUseProgram(skymap->program);
    glUniform1i(glGetUniformLocation(skymap->program, "skyIrradianceSampler"), IRRADIANCE_UNIT);
    glUniform1i(glGetUniformLocation(skymap->program, "inscatterSampler"), INSCATTER_UNIT);
    glUniform1i(glGetUniformLocation(skymap->program, "transmittanceSampler"), TRANSMITTANCE_UNIT);
    glUniform1i(glGetUniformLocation(skymap->program, "noiseSampler"), NOISE_UNIT);

    if (clouds == NULL) {
        files[0] = "atmosphere.glsl";
        files[1] = "clouds.glsl";
        clouds = new Program(2, files);
        glUseProgram(clouds->program);
        glUniform1i(glGetUniformLocation(clouds->program, "skyIrradianceSampler"), IRRADIANCE_UNIT);
        glUniform1i(glGetUniformLocation(clouds->program, "inscatterSampler"), INSCATTER_UNIT);
        glUniform1i(glGetUniformLocation(clouds->program, "transmittanceSampler"), TRANSMITTANCE_UNIT);
        glUniform1i(glGetUniformLocation(clouds->program, "noiseSampler"), NOISE_UNIT);
    }
}
