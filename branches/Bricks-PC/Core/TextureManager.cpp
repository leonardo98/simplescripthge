// TextureManager.cpp: implementation of the TextureManager class.

#include "TextureManager.h"
#include "Messager.h"

void TextureManager::ReadDescriptions(std::string fileName)
{
	TiXmlDocument doc( fileName.c_str() );
	bool loadOkay = doc.LoadFile();
	if (!loadOkay) {
		LOG("Не могу открыть файл " + fileName);
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
				ts.path = item->Attribute( "path" );
				ts.texture = NULL;
				_texturesMap[item->Attribute( "id" )] = ts;
				item = item->NextSiblingElement();
			}
		} else if (name == "texture") {
			TextureState ts;
			ts.group = "";
			ts.path = element->Attribute( "path" );
			ts.texture = new Render::Texture(ts.path.c_str());
			_texturesMap[element->Attribute( "id" )] = ts;
		}
		element = element->NextSiblingElement();
	}
}

void TextureManager::LoadGroup(std::string groupId)
{
	for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
		if (i->second.group == groupId && i->second.texture == NULL) {
			i->second.texture = new Render::Texture(i->second.path.c_str());
		}
	}
}

void TextureManager::UnloadGroup(std::string groupId)
{
	for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
		if (i->first == groupId && i->second.texture != NULL) {
			delete i->second.texture;
			i->second.texture = NULL;
		}
	}
}

PTexture TextureManager::GetTexture(std::string textureId)
{
	if (_texturesMap.find(textureId) == _texturesMap.end()) {
		return NULL;
	} else {
		return _texturesMap[textureId].texture;
	}
}

void TextureManager::Release() 
{
	for (;_texturesMap.begin() != _texturesMap.end();) {
		if (_texturesMap.begin()->second.texture != NULL) {
			delete _texturesMap.begin()->second.texture;
		}
		_texturesMap.erase(_texturesMap.begin());
	}
}

TextureManager::TextureMap TextureManager::_texturesMap;