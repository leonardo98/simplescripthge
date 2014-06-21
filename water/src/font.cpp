#include "main.h"

unsigned int fontbase;
int width[256];


/*void Print(int x, int y, int font, bool type, char format[], ...)
{
	if (!strlen(format))
		return;

	char text[255];
	va_list valist;

	va_start(valist, format);
	    vsprintf(text, format, valist);
	va_end(valist);

	glLoadIdentity();
	glTranslated(x, y, 0);
	glListBase(fontbase[font] - 32 + (128 * type));
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glListBase(0);
}
*/

void PrintULTRA(float scale, int x, int y, char format[], ...)
{
	if (!strlen(format))
		return;

	char text[512];
	va_list valist;

	va_start(valist, format);
	    vsprintf(text, format, valist);
	va_end(valist);

	RECT window;
	GetWindowRect(hWnd, &window);

	glLoadIdentity();
	glTranslatef(x - width[text[0] - 32] / 2, y, 0);
	glListBase(fontbase - 32);
	glPushMatrix();
	glScalef(scale, scale, scale);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopMatrix();
}

void Print(int x, int y, char format[], ...)
{
	if (!strlen(format))
		return;

	char text[512];
	va_list valist;

	va_start(valist, format);
	    vsprintf(text, format, valist);
	va_end(valist);

	RECT window;
	GetWindowRect(hWnd, &window);

	glLoadIdentity();
	glTranslatef(x - width[text[0] - 32] / 2, y, 0);
	glListBase(fontbase - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
}

void DeleteFont()
{
	glDeleteLists(fontbase, 256);
}

void BuildFont(char *widthfile, int tex, unsigned int &base)
{
	FILE *file;
	file = fopen(widthfile, "rt");
	for (int n = 0; n < 256; n++)
	{
		fscanf(file, "%d", &width[n]);
	}
	fclose(file);

	base = glGenLists(256);
	glBindTexture(GL_TEXTURE_2D, texture[tex]);
	for (int n = 0; n < 256; n++)
	{
		float cx = float(n % 16) / 16;
		float cy = float(n / 16) / 16;

		glNewList(base + n, GL_COMPILE);
			glTranslatef(width[n] / 2, 0, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(cx, cy + 0.0625);
				glVertex2f(0, 0);
				glTexCoord2f(cx + 0.0625, cy + 0.0625);
				glVertex2f(16, 0);
				glTexCoord2f(cx + 0.0625, cy + 0.001);
				glVertex2f(16, 16);
				glTexCoord2f(cx , cy + 0.001);
				glVertex2f(0, 16);
			glEnd();
			glTranslatef(width[n] / 2 + 1, 0, 0);
		glEndList();
	}
}