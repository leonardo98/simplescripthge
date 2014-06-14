#include "particle.h"

bool CParticle::InitParticle(const CPos &p)
{
	pos = p;
	rotationx = rand()%180;
	rotationy = rand()%180;
	rotationz = rand()%180;

	return true;
}


void CParticle::RenderParticle()
{
	glDepthMask(false);

	glPushMatrix();
 
        // Переместим частицы в указанные координаты
	glTranslatef(pos.x, pos.y, pos.z);

	glRotatef(rotationx,1.0f,0.0f,0.0f);   
	glRotatef(rotationy,0.0f,1.0f,0.0f); 
	glRotatef(rotationz,0.0f,0.0f,1.0f); 
 
        float halfSize = 0.5f;
 
        // Отрисуем частицы
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-halfSize, halfSize, 0.0f); // Верхняя левая вершина
 
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-halfSize, -halfSize, 0.0f); // Нижняя левая вершина
 
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(halfSize, -halfSize, 0.0f); // Нижняя правая вершина
 
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(halfSize, halfSize, 0.0f); // Верхняя правая вершина
        glEnd();


 
    glPopMatrix();

	glDepthMask(true);
}

void CParticle::UpdateParticle(float &g_fElpasedTime)
{
	pos.y += -0.05  * (g_fElpasedTime*40);
	rotationx += rand()%15 * (g_fElpasedTime*80);
	rotationy += rand()%15 * (g_fElpasedTime*80);
	rotationz += rand()%15 * (g_fElpasedTime*80);
}




bool CParticle::InitParticleBum(const CPos &p)
{
	pos = p;
	rotationx = rand()%180;
	rotationy = rand()%180;
	rotationz = rand()%180;

	float smxz = rand()%8;
	float smzz = rand()%8;
	smx = ((rand()%5) - 5)/smxz;
	smz = ((rand()%6) - 8)/smzz;

	float py = rand()%2;
	pos.y+=rand()%2/py;

	gr=0;

	return true;
}


void CParticle::UpdateParticleBum(float &g_fElpasedTime)
{
	pos.x += smx * g_fElpasedTime;
	pos.z += smz * g_fElpasedTime;

	gr -= 0.0025;
	pos.y += 0.2 * (g_fElpasedTime*40) + gr;

	rotationx += rand()%15 * (g_fElpasedTime*80);
	rotationy += rand()%25 * (g_fElpasedTime*80);
	rotationz += rand()%35 * (g_fElpasedTime*80);
}


void CParticle::RenderParticleBum()
{
	glDepthMask(false);

	glPushMatrix();
 
        // Переместим частицы в указанные координаты
	glTranslatef(pos.x, pos.y, pos.z);

	glRotatef(rotationx,1.0f,0.0f,0.0f);   
	glRotatef(rotationy,0.0f,1.0f,0.0f); 
	glRotatef(rotationz,0.0f,0.0f,1.0f); 
 
        float halfSize = 0.5f;
 
        // Отрисуем частицы
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-halfSize, halfSize, 0.0f); // Верхняя левая вершина
 
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-halfSize, -halfSize, 0.0f); // Нижняя левая вершина
 
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(halfSize, -halfSize, 0.0f); // Нижняя правая вершина
 
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(halfSize, halfSize, 0.0f); // Верхняя правая вершина
        glEnd();


 
    glPopMatrix();

	glDepthMask(true);
}