#include "TextureCut.h"

void TextureCut::CutImage(const char *fileName) {
	HTEXTURE h = Render::GetDC()->Texture_Load(fileName);
	int originWidth = Render::GetDC()->Texture_GetWidth(h, true);
	int originHeight = Render::GetDC()->Texture_GetHeight(h, true);
	DWORD *data = Render::GetDC()->Texture_Lock(h);
	int width = Render::GetDC()->Texture_GetWidth(h);
	int height = Render::GetDC()->Texture_GetHeight(h);

	int top;
	int bottom;
	int left;
	int right;
	{
		// top border search
		bool exit = false;
		for (unsigned int j = 0; j < originHeight && !exit; ++j) {
			top = j;
			for (unsigned int i = 0; i < originWidth; ++i) {
				if ((data[width * j + i] & 0xFF000000) != 0) {
					exit = true;
					break;
				}
			}
		}
	}
	{
		// bottom border search
		bool exit = false;
		for (int j = originHeight - 1; j >= 0 && !exit; --j) {
			bottom = j;
			for (unsigned int i = 0; i < originWidth; ++i) {
				if ((data[width * j + i] & 0xFF000000) != 0) {
					exit = true;
					break;
				}
			}
		}
	}
	{
		// left border search
		bool exit = false;
		for (unsigned int i = 0; i < originWidth && !exit; ++i) {
			left = i;
			for (unsigned int j = 0; j < originHeight; ++j) {
				if ((data[width * j + i] & 0xFF000000) != 0) {
					exit = true;
					break;
				}
			}
		}
	}
	{
		// right border search
		bool exit = false;
		for (int i = originWidth - 1; i >= 0 && !exit; --i) {
			right = i;
			for (unsigned int j = 0; j < originHeight; ++j) {
				if ((data[width * j + i] & 0xFF000000) != 0) {
					exit = true;
					break;
				}
			}
		}
	}
	HTEXTURE result = Render::GetDC()->Texture_Create(right - left + 1, bottom - top + 1);
	Render::GetDC()->Texture_Free(h);
}

void TextureCut::CutImages() {
	TiXmlDocument doc("temporary.xml");
	if (doc.LoadFile()) {
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *texture = root->FirstChildElement("texture");
		while (texture) {
			CutImage(texture->Attribute("path"));
			texture = texture->NextSiblingElement("texture");
		}
	}
}