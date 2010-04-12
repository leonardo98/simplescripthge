// TextureManager.h: interface for the TextureManager class.

#if !defined(_TEXTUREMANAGER_INCLUDED_)
#define _TEXTUREMANAGER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"


class TextureManager  
{
public:
	struct TextureState
	{
		std::string path;
		std::string group;
		HTEXTURE hTexture;
		Texture *texture;
	};
	typedef std::map<std::string, TextureState> TextureMap;
protected:
	void ReadDescriptions(std::string fileName);
	void LoadGroup(std::string groupId);
	void UnloadGroup(std::string groupId);
	static Texture * GetTexture(std::string textureId);
	TextureManager();
	virtual ~TextureManager();
private:
	static TextureMap _texturesMap;
	HGE *_hge;
};

#endif // !defined(_TEXTUREMANAGER_INCLUDED_)
