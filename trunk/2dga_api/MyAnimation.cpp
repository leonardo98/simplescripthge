#include "Animation.h"
#include "../Core/Render.h"

namespace My {
//
// ���������� - ������� ��� ��������� �������
// (�������� ��������� ������ �������� ��������, 
// �.�. �� ����� �������� ����� ���� ��������� ������ ��������)
//
Animation::~Animation() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

//
// ����������� - ��������� �������� �������� �� XML 
// ����� ������������ ����� ������,
// ������� ����� ���������� ���� � ������ ��������,
// ����� ����������� TinyXml
//
Animation::Animation(TiXmlElement *xe, HTEXTURE hTexture)
{
	_time = fatof(xe->Attribute("time"));
	_pivotPos.x = fatof(xe->Attribute("pivotX"));
	_pivotPos.y = fatof(xe->Attribute("pivotY"));

	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		_bones.push_back(new MovingPart(element, hTexture));
		element = element->NextSiblingElement();
	}
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
}

//
// ���������� ��������� �������� �� ������, 
// "pos" - ��������� �������� �� ������, 
// ����� ���������� ��������� - ������ "mirror" � "true".
// ����� ����� ����� �������� ����� ������ ������� Draw()
// ���������� ����� ����, ���� ����� �������� �� ������ ����� ����
//
void Animation::SetPos(const hgeVector &pos, bool mirror) {
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
	if (mirror) {
		_subPosition.Scale(-1.f, 1.f);
	}
	_subPosition.Move(pos.x, pos.y);
}

//
// ���������� �������� �� ������ 
// "position" - �������� ������� ��� �������� �� ��������� [0, 1]
//
void Animation::Draw(float position) {
	_matrixsStack.clear(); 
	// ��� ������� ��������� ��� �������� ��������� ���� ���������� "����" ������
	_matrixsStack.push_back(_subPosition);
	for(int i = 0; i < (int)_bones.size();i++) {
		_bones[i]->Draw(position, _matrixsStack);
	}
}

//
// ����� ������� ����� �������� 
// ��� ������� ����� ������������ � Update ��� ������� 
// �������� "position" ������ Draw
// ��������, ��� ���������� �������� ����� ������� ���:
// progress += deltaTime / anim->Time();
// if(progress >= 1.f) progress = 0.f;
//
float Animation::Time() const {
	return _time;
}


/* ���������� ������� ��������� ��������� */

// ��������� ���� ��������
// � ������ ������ - ������ "true"
// ����� ��������� ��������� ������ - �������� "���������������" ��������
//
// ����� � ����� ������������ HGE ��� �������� � �������� �������
// ��� ����� ���� ����� �������� ��� ������������ �� ������ ������ 
//
bool AnimationManager::Load(const std::string &fileName) {
	
	// ��������� �������� - ���������� ��������� �� ��� � "tex", ������� ������������ �����
	//
	// ������ ����� - ���� ��� ��� ������������ ���� ����������
	HGE *hge = hgeCreate(HGE_VERSION);
	HTEXTURE tex = hge->Texture_Load((fileName.substr(0, fileName.length() - 3) + "png").c_str());
	if (tex == 0) { // ���� �������� �� ������� - ��������� ��������, ���������� "false"
		hge->Release();
		return false;
		LOG("assigned png file not found " + fileName);
	}
	// ����� �����

	TiXmlDocument doc;
	if (doc.LoadFile(hge->Resource_MakePath(fileName.c_str()))) { // ���� ���� ���������� - ������ XML � ��������� ��������
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *animation = root->FirstChildElement("Animation");
		while (animation) {
			const char *id = animation->Attribute("id");
			// �������� - �� ���� � ����� �������� � ����������� id - ����� ��������� ������ ������
			if (_animations.find(id) == _animations.end()) { 
				_animations[id] = new Animation(animation, tex);
			}
			animation = animation->NextSiblingElement("Animation");
		}
		_textures.push_back(tex);

		// ������ ����� - ���� ��� ��� ������������ ���� ����������
		hge->Release();
		// ����� �����

		return true;
	} else { 
		// ���� �������� �� ������ ��� ��� �� ������� ���������
		// ��������� ��������, ���������� "false"
		//
		// ������ ����� - ���� ��� ��� ������������ ���� ����������
		hge->Texture_Free(tex);
		hge->Release();
		// ����� �����

		char buffer[1024];
		GetCurrentDirectory(1024, buffer);
		LOG("file not found: %s" + fileName);
		return false;
	}
}

//
// ��������� ��� �������� � �������� �� ������
//
void AnimationManager::UnloadAll() {
	HGE *hge = hgeCreate(HGE_VERSION);
	for (AnimationMap::iterator it = _animations.begin(); it != _animations.end(); it++) {
		delete (*it).second;
	}
	for (unsigned int i = 0; i < _textures.size();i++) {
		hge->Texture_Free(_textures[i]);
	}
	_animations.clear();
	_textures.clear();
	hge->Release();
}

//
// �������� ��������� �� �������� �� animationId - ��� ��� �������� � ��������� 
// (����� ������ �������� delete ��� ���!)
//
Animation *AnimationManager::getAnimation(const std::string &animationId) {
	AnimationMap::iterator it_find = _animations.find(animationId);
	if (it_find != _animations.end()) {
		return (*it_find).second;
	}
	LOG("animation " + animationId + " not found.");
	return NULL;
}

AnimationManager::AnimationMap AnimationManager::_animations;
std::vector<HTEXTURE> AnimationManager::_textures;

};