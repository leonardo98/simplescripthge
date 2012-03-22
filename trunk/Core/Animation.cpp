#include "Animation.h"
#include "Core.h"
#include "Math.h"

BoneList::iterator Find(BoneList::iterator start, BoneList::iterator end, Bone *item) {
	while (start != end && (*start) != item) {
		++start; 
	} 
	return start;
}

Bone::Bone(const char *name, const BoneType bt) 
: boneType(bt)
{
	if (name) {
		strcpy_s(boneName, name);
	}
}

// for editor
#ifdef ANIMATION_EDITOR

void ReOrder(BoneList::iterator start, BoneList::iterator end) {
	int order = 1; 
	while (start != end) { 
		(*start)->order = order++; 
		++start;
	}
}

void Bone::Rename(const char *newName) {
	strcpy_s(boneName, newName);
}

bool Bone::hasBone(const std::string &boneName) {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		if (boneName == _bones[i]->boneName || _bones[i]->hasBone(boneName)) {
			return true;
		}
	}
	return false;
}
#endif

Bone::Bone(Bone &bone) 
: boneType(bone.boneType)
, _offparent(bone._offparent)
{
	strcpy_s(boneName, bone.boneName);
}

bool Bone::ReplaceTexture(const std::string &boneName, const char *texture) {
	return false;
}

void Bone::ResortBones() {
	_topBone.clear();
	_bottomBone.clear();
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		if (_bones[i]->_offparent == OffParentOrder_top) {
			_topBone.push_back(_bones[i]);
		} else if (_bones[i]->_offparent == OffParentOrder_bottom) {
			_bottomBone.push_back(_bones[i]);
		} else {
			assert(false);
		}
	}
}

void Bone::CalcGradient() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->CalcGradient();
	}
}

#ifdef ANIMATION_EDITOR

void Bone::CollectFileNames(NameList &list) {
	for (unsigned int i = 0; i < _parts.size(); ++i) {
		list[_parts[i]->fileName] = 1;
	}
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->CollectFileNames(list);
	}
}

void * MovingPart::EditorCall(CallBones myCall, void *parent) {
	this->parent = parent;
	void *item = myCall(parent, boneName, this);
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->EditorCall(myCall, item);
	}
	return item;
}
#endif

void MovingPart::LoadTextures() {
	for (unsigned int i = 0; i < _parts.size(); ++i) {
		_parts[i]->Set(Core::getTexture(_parts[i]->fileName));
	}
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->LoadTextures();
	}
}

void MovingPart::UnloadTextures() {
	for (unsigned int i = 0; i < _parts.size(); ++i) {
		_parts[i]->Set(NULL);
	}
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->UnloadTextures();
	}
}

void MovingPart::SetKey(int index, const MovingPartkey &key) {
	_x.keys[index].value = key.x;
	_y.keys[index].value = key.y;
	_angle.keys[index].value = key.angle;
	_scaleX.keys[index].value = key.sx;
	_scaleY.keys[index].value = key.sy;

	_x.CalculateGradient();
	_y.CalculateGradient();
	_angle.CalculateGradient();
	_scaleX.CalculateGradient();
	_scaleY.CalculateGradient();
}

MovingPart::~MovingPart() {
	for (unsigned int i = 0; i < _parts.size(); ++i) {
		delete _parts[i];
	}
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

void MovingPart::CalcGradient() {
	_x.CalculateGradient();
	_y.CalculateGradient();
	_angle.CalculateGradient();
	_scaleX.CalculateGradient();
	_scaleY.CalculateGradient();
	Bone::CalcGradient();
}

hgeSprite * MovingPart::GetSprite() {
	if (_parts.size()) {
		return _parts.front()->GetHGESprite();
	}
	return NULL;
}

void MovingPart::SaveToXml(TiXmlElement *xe) {
	xe->SetValue("movingPart");
	xe->SetAttribute("name", boneName);
	if (_movingType == MovingType_spline) {
//		xe->SetAttribute("moving_type", "spline");
	} else if (_movingType == MovingType_line) {
		xe->SetAttribute("moving_type", "line");
	} else {
		xe->SetAttribute("moving_type", "discontinuous");
	}
	if (_offparent == OffParentOrder_bottom) {
		xe->SetAttribute("offparent", "bottom");
	}
	if (fabs(_center.x) > 1e-3) {
		Math::Write(xe, "centerX", _center.x);
	}
	if (fabs(_center.y) > 1e-3) {
		Math::Write(xe, "centerY", _center.y);
	}
	if (_parts.size()) {
		xe->SetAttribute("texture", _parts[0]->fileName.c_str());
	}

	for (unsigned int i = 0; i < _x.keys.size(); ++i) {
		TiXmlElement *pos = new TiXmlElement("pos");
	
		if (fabs(_x.keys[i].time) > 1e-3) {
			Math::Write(pos, "time", _x.keys[i].time);
		}
		if (fabs(_x.keys[i].value) > 1e-3) {
			Math::Write(pos, "x", _x.keys[i].value);
		}
		if (fabs(_y.keys[i].value) > 1e-3) {
			Math::Write(pos, "y", _y.keys[i].value);
		}
		if (fabs(_scaleX.keys[i].value - 1.f) > 1e-3) {
			Math::Write(pos, "scaleX", _scaleX.keys[i].value);
		}
		if (fabs(_scaleY.keys[i].value - 1.f) > 1e-3) {
			Math::Write(pos, "scaleY", _scaleY.keys[i].value);
		}
		if (fabs(_angle.keys[i].value) > 1e-5) {
			Math::Write(pos, "angle", _angle.keys[i].value);
		}

		xe->LinkEndChild(pos);
	}

	for (unsigned int i = 0; i < _bones.size(); ++i) {
		TiXmlElement *mp = new TiXmlElement("movingPart");
		_bones[i]->SaveToXml(mp);
		xe->LinkEndChild(mp);
	}
}

MovingPart::MovingPart(TiXmlElement *xe)
: Bone(xe->Attribute("name"), BT_MovingPart)
{
	const char *tmp = xe->Attribute("moving_type");
	if (tmp == NULL || strcmp(tmp, "spline") == 0) {
		_movingType = MovingType_spline;
	} else if (strcmp(tmp, "line") == 0) {
		_movingType = MovingType_line;
	} else {
		_movingType = MovingType_discontinuous;
	}
	_offparent = ((xe->Attribute("offparent") && std::string(xe->Attribute("offparent")) == "bottom") ? OffParentOrder_bottom : OffParentOrder_top);
	_center.x = Math::Read(xe, "centerX", 0.f);
	_center.y = Math::Read(xe, "centerY", 0.f);
	const char *name = xe->Attribute("texture"); 
	if (name) {
		_parts.push_back(new AnimationPart(name));
	}
	TiXmlElement *pos = xe->FirstChildElement("pos");
	if (pos == NULL) {
		_x.AddKey(0.f, 0.f);
		_y.AddKey(0.f, 0.f);
		_scaleX.AddKey(0.f, 1.f);
		_scaleY.AddKey(0.f, 1.f);
		_angle.AddKey(0.f, 0.f);
	}
	while (pos) {
		float time = Math::Read(pos, "time", 0.f);
		_x.AddKey(time, Math::Read(pos, "x", 0.f));
		_y.AddKey(time, Math::Read(pos, "y", 0.f));	
		_scaleX.AddKey(time, Math::Read(pos, "scaleX", 1.f));
		_scaleY.AddKey(time, Math::Read(pos, "scaleY", 1.f));
		_angle.AddKey(time, Math::Read(pos, "angle", 0.f));

		pos = pos->NextSiblingElement("pos");
	}

	TiXmlElement *part = xe->FirstChildElement("part");
	assert(!(part != NULL && xe->Attribute("texture") != NULL));// двойное заполнение 
	while (part) {
		_parts.push_back(new AnimationPart(part->Attribute("texture")));
		part = part->NextSiblingElement("part");
	}
	_last = NULL;
	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back( new MovingPart(element) );
		}
		element = element->NextSiblingElement();
	}
	ReOrder(_bones.begin(), _bones.end());
	ResortBones();
}

MovingPart * MovingPart::addBone(const char *boneName, MovingPart *newChildBone, MovingPart *afterBone) {
	if (newChildBone == NULL) {
		newChildBone = new MovingPart(boneName);
	} else {
		strcpy_s(newChildBone->boneName, boneName);
	}
	BoneList::iterator i = _bones.begin();

	if (afterBone) {
		i = Find(_bones.begin(), _bones.end(), afterBone);
		++i;
	}
	_bones.insert(i, newChildBone);

	ReOrder(_bones.begin(), _bones.end());
	newChildBone->CalcGradient();
	ResortBones();
	return newChildBone;
}

bool MovingPart::removeBone(Bone *bone) {
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		if (*i == bone) {
			_bones.erase(i);
			ResortBones();
			return true;
		} else if ((*i)->removeBone(bone)) {
			return true;
		}
	}
	return false;
}

MovingPart::MovingPart(const std::string &boneName)
: Bone(boneName.c_str(), BT_MovingPart){
	_movingType = MovingType_line;
	_x.AddKey(0.f, 0.f);
	_y.AddKey(0.f, 0.f);
	_angle.AddKey(0.f, 0.f);
	_center = FPoint2D(0.f, 0.f);
	_scaleX.AddKey(0.f, 1.f);
	_scaleY.AddKey(0.f, 1.f);
	_last = NULL;
	_offparent = OffParentOrder_top;
}

MovingPart::MovingPart(MovingPart &movinPart) 
: Bone(movinPart)
{
	_movingType = movinPart._movingType;
	_x = movinPart._x;
	_y = movinPart._y;
	_angle = movinPart._angle;
	_center = movinPart._center;
	_scaleX = movinPart._scaleX;
	_scaleY = movinPart._scaleY;
	_last = movinPart._last;
	_offparent = movinPart._offparent;

	for (std::vector<AnimationPart *>::iterator i = movinPart._parts.begin(), e = movinPart._parts.end(); i != e; ++i) {
		_parts.push_back(new AnimationPart(*(*i)));
	}

	for (BoneList::iterator i = movinPart._bones.begin(), e = movinPart._bones.end(); i != e; ++i) {
		MovingPart *b;
		if ((*i)->boneType == BT_MovingPart) {
			b = new MovingPart(*static_cast<MovingPart *>(*i));
		} else {
			assert(false);
		}
		_bones.push_back(b);
	}
	ReOrder(_bones.begin(), _bones.end());
	ResortBones();
}


void MovingPart::Draw(float p) {
	assert(0.f <= p && p < 1);
	Render::PushMatrix();
	float dp = p;
	if (_movingType == MovingType_discontinuous) {
		dp = static_cast<int>(p * _x.keys.size()) / static_cast<float>(_x.keys.size());
		assert(0.f <= dp && dp < 1);
	}
	Render::MatrixMove(_x.Value(dp), _y.Value(dp));
#ifdef ANIMATION_EDITOR
	matrix = Render::GetCurrentMatrix();
#endif

	Render::MatrixRotate(_angle.Value(dp));
	Render::MatrixScale(_scaleX.Value(dp), _scaleY.Value(dp));

	Render::MatrixMove(-_center.x, -_center.y);

	for (unsigned int i = 0; i < _bottomBone.size(); ++i) {
		_bottomBone[i]->Draw(p);
	}
	if (_parts.size()) {
		int i = static_cast<int>(p * _parts.size());
		assert(0 <= i && i < static_cast<int>(_parts.size()));
		_last = _parts[i];
		_last->Render();
	}
	for (unsigned int i = 0; i < _topBone.size(); ++i) {
		_topBone[i]->Draw(p);
	}

	Render::PopMatrix();

}

bool MovingPart::PixelCheck(const FPoint2D &pos) {
	if (_last && _last->PixelCheck(pos)) {
		return true;
	}
	for (unsigned int i = 0; i < _bottomBone.size(); ++i) {
		if (_bottomBone[i]->PixelCheck(pos)) {
			return true;
		}
	}
	for (unsigned int i = 0; i < _topBone.size(); ++i) {
		if (_topBone[i]->PixelCheck(pos)) {
			return true;
		}
	}
	return false;
}

bool MovingPart::ReplaceTexture(const std::string &boneName, const char *texture) {
	bool success = (Bone::boneName == boneName);
	if (success) {
		for (unsigned int i = 0; i < _parts.size(); ++i) {
			delete _parts[i];
		}
		_parts.clear();
		AnimationPart *s = new AnimationPart(texture);
		s->Set(Core::getTexture(texture));
		_parts.push_back(s);
	}
	for (BoneList::iterator i = _topBone.begin(), e = _topBone.end(); i != e; ++i) {
		success |= (*i)->ReplaceTexture(boneName, texture);
	}
	for (BoneList::iterator i = _bottomBone.begin(), e = _bottomBone.end(); i != e; ++i) {
		success |= (*i)->ReplaceTexture(boneName, texture);
	}
	return success;
}


Animation::~Animation() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

#ifdef ANIMATION_EDITOR
void * Animation::EditorCall(CallBones myCall, void *parent) {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->EditorCall(myCall, parent);
	}
	return NULL;
}

bool Animation::hasBone(const std::string &boneName) {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		if (boneName == _bones[i]->boneName || _bones[i]->hasBone(boneName)) {
			return true;
		}
	}
	return false;
}
#endif

void Animation::SaveToXml(TiXmlElement *xe) {

	Math::Write(xe, "time", _time);
	Math::Write(xe, "pivotX", _pivotPos.x);
	Math::Write(xe, "pivotY", _pivotPos.y);

	for (unsigned int i = 0; i < _bones.size(); ++i) {
		TiXmlElement *mp = new TiXmlElement("movingPart");
		_bones[i]->SaveToXml(mp);
		xe->LinkEndChild(mp);
	}
}

Animation::Animation(TiXmlElement *xe)
: _timeCounter(0.f)
, _texturesLoaded(false)
{
	_time = fatof(xe->Attribute("time"));
	_pivotPos.x = fatof(xe->Attribute("pivotX"));
	_pivotPos.y = fatof(xe->Attribute("pivotY"));

	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back(new MovingPart(element));
		}
		element = element->NextSiblingElement();
	}
	ReOrder(_bones.begin(), _bones.end());

	FPoint2D pos(Math::Read(xe, "x", 0.f), Math::Read(xe, "y", 0.f));
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
	_subPosition.Move(pos.x, pos.y);
	CalcGradient();
}

Animation::Animation() 
{
	_pivotPos = FPoint2D(0.f, 0.f);
	_subPosition.Unit();
	_time = 1.f;
	_timeCounter = 0.f;
	_texturesLoaded = false;
}

MovingPart * Animation::addBone(const char *boneName, MovingPart *newChildBone, MovingPart *afterBone) {
	if (newChildBone == NULL) {
		newChildBone = new MovingPart(boneName);
	} else {
		strcpy_s(newChildBone->boneName, boneName);
	}
	BoneList::iterator i = _bones.begin();

	if (afterBone) {
		i = Find(_bones.begin(), _bones.end(), afterBone);
		++i;
	}
	_bones.insert(i, newChildBone);
	ReOrder(_bones.begin(), _bones.end());

	newChildBone->CalcGradient();
	return newChildBone;
}

bool Animation::removeBone(MovingPart *movingPart) {
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		if (*i == movingPart) {
			_bones.erase(i);
			return true;
		} else if ((*i)->removeBone(movingPart)) {
			return true;
		}
	}
	return false;
}

Animation::Animation(Animation &animation) {
	_pivotPos = animation._pivotPos;
	_subPosition = animation._subPosition;
	_time = animation._time;
	_timeCounter = animation._timeCounter;
	_texturesLoaded = animation._texturesLoaded;
	
	_bones;
	for (BoneList::iterator i = animation._bones.begin(), e = animation._bones.end(); i != e; ++i) {
		MovingPart *b;
		if ((*i)->boneType == BT_MovingPart) {
			b = new MovingPart(*static_cast<MovingPart *>(*i));
		} else {
			assert(false);
		}
		_bones.push_back(b);
	}
	ReOrder(_bones.begin(), _bones.end());
}

void Animation::SetPos(const FPoint2D &pos, bool mirror) {
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
	if (mirror) {
		_subPosition.Scale(-1.f, 1.f);
	}
	_subPosition.Move(pos.x, pos.y);
}

bool Animation::PixelCheck(const FPoint2D &pos) {
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		if ((*i)->PixelCheck(pos)) {
			return true;
		}
	}
	return false;
}

void Animation::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(-_pivotPos.x, -_pivotPos.y);
	float p = _timeCounter / _time;
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->Draw(p);
	}
	Render::PopMatrix();
}

void Animation::Draw(float position) {
	assert(0.f <= position && position < 1.f);
	Render::PushMatrix();
	Render::MatrixMove(-_pivotPos.x, -_pivotPos.y);
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->Draw(position);
	}
	Render::PopMatrix();
}

void Animation::Update(float dt) {
	_timeCounter += dt;
	while (_timeCounter >= _time) {
		_timeCounter -= _time;
	}
}

float Animation::Time() {
	return _time;
}

bool Animation::ReplaceTexture(const std::string &boneName, const char *texture) {
	bool success = false;
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		success |= (*i)->ReplaceTexture(boneName, texture);
	}
	return success;
}

void Animation::LoadTextures() {
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->LoadTextures();
	}
	_texturesLoaded = true;
}

void Animation::UnloadTextures() {
	_texturesLoaded = false;
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->UnloadTextures();
	}
}

bool Animation::TextureLoaded() {
	return _texturesLoaded;
}

void MovingPart::Get(MovingPartInfo &info) const {
	info.x = _x;
	info.y = _y;
	info.angle = _angle;
	info.center = _center;
	info.scaleX = _scaleX;
	info.scaleY = _scaleY;
	info.partsNames.clear();
	for (unsigned int i = 0; i < _parts.size(); ++i) {
		info.partsNames.push_back(_parts[i]->fileName);
	}
	info.movingType = _movingType;
	info.offparent = _offparent;
}

void MovingPart::Set(const MovingPartInfo &info) {
	_x = info.x;
	_y = info.y;
	_angle = info.angle;
	_center = info.center;
	_scaleX = info.scaleX;
	_scaleY = info.scaleY;
	_movingType = info.movingType;
	_offparent = info.offparent;

	bool equal = _parts.size() == info.partsNames.size();
	for (unsigned int i = 0; i < _parts.size() && equal; ++i) {
		equal &= _parts[i]->fileName == info.partsNames[i];
	}
	if (equal) {
		return;
	}

	for (unsigned int i = 0; i < _parts.size(); ++i) {
		_parts[i]->Set(NULL);
		delete _parts[i];
	}
	_parts.clear();
	for (unsigned int i = 0; i < info.partsNames.size(); ++i) {
		_parts.push_back(new AnimationPart(info.partsNames[i].c_str()));
		_parts.back()->Set(Core::getTexture(_parts[i]->fileName));
	}
}

void Animation::Get(AnimationInfo &info) const {
	info.pivotPos = _pivotPos;
	info.time = _time;
}

void Animation::Set(const AnimationInfo &info) {
	_pivotPos = info.pivotPos;
	_time = info.time;
}

void Animation::CalcGradient() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->CalcGradient();
	}
}

void Animation::CollectFileNames(NameList &list) {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->CollectFileNames(list);
	}
}
