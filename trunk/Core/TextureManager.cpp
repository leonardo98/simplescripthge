// TextureManager.cpp: implementation of the TextureManager class.

#include "TextureManager.h"
#include "Messager.h"

void TextureManager::ReadDescriptions(std::string fileName)
{
	TiXmlDocument doc( Render::GetDC()->Resource_MakePath((Render::GetDataDir() + fileName).c_str()));
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
				const char *name = item->Attribute( "id" );
				if (_texturesMap.find(name) != _texturesMap.end()) {
					LOG("resource already loaded :" + name);
					assert(false);
				}
				_texturesMap[name] = ts;
				item = item->NextSiblingElement();
			}
		} else if (name == "texture") {
			TextureState ts;
			ts.group = "";
			ts.path = element->Attribute( "path" );
			ts.texture = new Texture(ts.path.c_str());
			const char *name = element->Attribute( "id" );
			if (_texturesMap.find(name) != _texturesMap.end()) {
				LOG("resource already loaded :" + name);
				assert(false);
			}
			_texturesMap[name] = ts;
		} else if (name == "atlas") {
			std::string path = element->Attribute("path");
			TiXmlDocument doc(std::string(Render::GetDataDir() + path).c_str());
			if (!doc.LoadFile()) {
				LOG("file not found " + path);
				assert(false);
			}
			path.replace(path.find("desc.xml"), std::string("desc.xml").size(), "png");
			HTEXTURE hTexture = Render::GetDC()->Texture_Load((Render::GetDataDir() + path).c_str());
			if (hTexture == 0) {
				LOG("file not found " + path);
				assert(false);
			}
			TiXmlElement *root = doc.RootElement();
			TiXmlElement *frame = root->FirstChildElement("frame");
			while (frame) {
				int x = atoi(frame->Attribute("x"));
				int y = atoi(frame->Attribute("y"));
				int width = atoi(frame->Attribute("width"));
				int height = atoi(frame->Attribute("height"));
				int innerX = atoi(frame->Attribute("innerX"));
				int innerY = atoi(frame->Attribute("innerY"));
				int frameWidth = atoi(frame->Attribute("frameWidth"));
				int frameHeight = atoi(frame->Attribute("frameHeight"));
				const char *name = frame->Attribute("path");
				
				TextureState ts;
				ts.group = "";
				ts.path = name;
				ts.texture = new Texture(hTexture, x, y, width, height, innerX, innerY);
				if (_texturesMap.find(name) != _texturesMap.end()) {
					LOG("resource already loaded :" + name);
					assert(false);
				}
				_texturesMap[name] = ts;

				frame = frame->NextSiblingElement("frame");
			}
		}
		element = element->NextSiblingElement();
	}
}

void TextureManager::LoadGroup(std::string groupId)
{
	for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
		if (i->second.group == groupId && i->second.texture == NULL) {
			i->second.texture = new Texture(i->second.path.c_str());
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

Texture *TextureManager::getTexture(const std::string &textureId)
{
	if (_texturesMap.find(textureId) == _texturesMap.end()) {
		// если такой текстуры нет - может нам дают ее путь?
		TextureState ts;
		ts.group = "";
		ts.path = textureId.c_str();
		ts.texture = new Texture(ts.path.c_str());
		_texturesMap[textureId] = ts;
		return ts.texture;
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