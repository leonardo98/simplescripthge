// TextureManager.h: interface for the TextureManager class.

#ifndef MYENGINE_TEXTUREMANAGER_H
#define MYENGINE_TEXTUREMANAGER_H

#include "types.h"
#include "Render.h"

class TextureManager  
{
public:
	struct TextureState
	{
		std::string path;
		std::string group;
		Texture *texture;
	};
	typedef std::map<std::string, TextureState> TextureMap;
	static Texture *getTexture(const std::string &textureId);
	static bool isTexture(const std::string &textureId);
	static void LoadGroup(std::string groupId);
	static void UnloadGroup(std::string groupId);
	static void ReleaseAllTextures();
protected:
	static void ReadDescriptions(std::string fileName);
private:
	static TextureMap _texturesMap;
};

#endif//MYENGINE_TEXTUREMANAGER_H