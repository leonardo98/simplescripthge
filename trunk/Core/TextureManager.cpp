// TextureManager.cpp: implementation of the TextureManager class.

#include "TextureManager.h"
#include "Messager.h"

void TextureManager::ReadDescriptions(std::string fileName)
{
	TiXmlDocument doc( fileName.c_str() );
	bool loadOkay = doc.LoadFile();
	if (!loadOkay) {
		Messager::SendMessage("log", "�� ���� ������� ���� " + fileName);
		exit(-6);
	}
	TiXmlNode* root = doc.FirstChildElement( "Resources" );
	assert( root );
	TiXmlElement* element = root->FirstChildElement();
	while ( element ) {
		std::string name = element->Value();
		if (name ==  "group") {
			std::string groupName = element->Attribute( "name" );
			TiXmlElement* item = element->FirstChildElement( "texture" );
			while ( item ) {
				TextureState ts;
				ts.group = groupName;
				ts.hTexture = 0;			
				ts.path = item->Attribute( "path" );
				ts.texture = NULL;
				_texturesMap[item->Attribute( "id" )] = ts;
				item = item->NextSiblingElement();
			}
		} else if (name == "texture") {
			TextureState ts;
			ts.group = "";
			ts.path = element->Attribute( "path" );
			ts.hTexture = _dc->Texture_Load(ts.path.c_str());
			if (ts.hTexture == NULL) {
				Messager::SendMessage("log", "�� ���� ������� ���� " + ts.path);
				exit(-7);
			}
			ts.texture = new Texture(ts.hTexture, 0, 0, _dc->Texture_GetWidth(ts.hTexture), _dc->Texture_GetHeight(ts.hTexture));
			_texturesMap[element->Attribute( "id" )] = ts;
		}
		element = element->NextSiblingElement();
	}
}

void TextureManager::LoadGroup(std::string groupId)
{
	for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
		if (i->second.group == groupId && i->second.hTexture == 0) {
			i->second.hTexture = _dc->Texture_Load(i->second.path.c_str());
			i->second.texture = new Texture(i->second.hTexture, 0, 0, _dc->Texture_GetWidth(i->second.hTexture), _dc->Texture_GetHeight(i->second.hTexture));
		}
	}
}

void TextureManager::UnloadGroup(std::string groupId)
{
	for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
		if (i->first == groupId && i->second.hTexture != 0) {
			delete i->second.texture;
			i->second.texture = NULL;
			_dc->Texture_Free(i->second.hTexture);
			i->second.hTexture = 0;
		}
	}
}

Texture * TextureManager::GetTexture(std::string textureId)
{
	if (_texturesMap.find(textureId) == _texturesMap.end()) {
		return NULL;
	} else {
		return _texturesMap[textureId].texture;
	}
}

TextureManager::TextureManager()
{
	_dc = hgeCreate(HGE_VERSION);
}

TextureManager::~TextureManager()
{
	_dc->Release();
}

TextureManager::TextureMap TextureManager::_texturesMap;