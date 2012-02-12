#include "Animation.h"
#include "Core.h"
#include "Math.h"

Bone::Bone(const char *name, const BoneType bt) 
: boneType(bt)
{
	if (name) {
		strcpy_s(boneName, name);
	}
}

	// for editor
#ifdef ANIMATION_EDITOR
void Bone::Rename(const char *newName) {
	strcpy_s(boneName, newName);
}

void Bone::EditorCall(CallBones myCall, void *parent) {
	this->parent = parent;
	void *item = myCall(parent, boneName, this);
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->EditorCall(myCall, item);
	}
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

void Bone::SetLoop(bool loop) {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->SetLoop(loop);
	}
}

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
	_x.pushedKeys[index] = SplinePath::KeyFrame(key.x, key.x);
	_y.pushedKeys[index] = SplinePath::KeyFrame(key.y, key.y);
	_angle.pushedKeys[index] = SplinePath::KeyFrame(key.angle, key.angle);
	_scaleX.pushedKeys[index] = SplinePath::KeyFrame(key.sx, key.sx);
	_scaleY.pushedKeys[index] = SplinePath::KeyFrame(key.sy, key.sy);

	_x.CalculateGradient(_loop);
	_y.CalculateGradient(_loop);
	_angle.CalculateGradient(_loop);
	_scaleX.CalculateGradient(_loop);
	_scaleY.CalculateGradient(_loop);
}

MovingPart::~MovingPart() {
	for (unsigned int i = 0; i < _parts.size(); ++i) {
		delete _parts[i];
	}
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

void MovingPart::SetLoop(bool loop) {
	_x.CalculateGradient(loop);
	_y.CalculateGradient(loop);
	_angle.CalculateGradient(loop);
	_scaleX.CalculateGradient(loop);
	_scaleY.CalculateGradient(loop);
	Bone::SetLoop(loop);
	_loop = loop;
}

hgeSprite * MovingPart::GetSprite() {
	if (_parts.size()) {
		return _parts.front()->GetHGESprite();
	}
	return NULL;
}

MovingPart::MovingPart(TiXmlElement *xe)
: Bone(xe->Attribute("name"), BT_MovingPart)
{
	_discontinuous = Math::Read(xe, "discontinuous", false);
	_offparent = ((xe->Attribute("offparent") && std::string(xe->Attribute("offparent")) == "bottom") ? OffParentOrder_bottom : OffParentOrder_top);
	_center.x = Math::Read(xe, "centerX", 0.f);
	_center.y = Math::Read(xe, "centerY", 0.f);
	const char *name = xe->Attribute("texture"); 
	if (name) {
		_parts.push_back(new AnimationPart(name));
	}
	TiXmlElement *pos = xe->FirstChildElement("pos");
	if (pos == NULL) {
		_x.addKey(0.f);
		_y.addKey(0.f);
		_scaleX.addKey(1.f);
		_scaleY.addKey(1.f);
		_angle.addKey(0.f);
	}
	while (pos) {
		_x.addKey(Math::Read(pos, "x", 0.f));
		_y.addKey(Math::Read(pos, "y", 0.f));	
		_scaleX.addKey(Math::Read(pos, "scaleX", 1.f));
		_scaleY.addKey(Math::Read(pos, "scaleY", 1.f));
		_angle.addKey(Math::Read(pos, "angle", 0.f));

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
	ResortBones();
}

MovingPart * MovingPart::addBone(const char *boneName, MovingPart *newChildBone) {
	if (newChildBone == NULL) {
		newChildBone = new MovingPart(boneName);
	} else {
		strcpy_s(newChildBone->boneName, boneName);
	}
	_bones.push_back(newChildBone);
	newChildBone->SetLoop(_loop);
	ResortBones();
	return newChildBone;
}

bool MovingPart::removeBone(Bone *bone) {
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		if (*i == bone) {
			delete (*i);
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
	_discontinuous = false;
	_x.addKey(0.f);
	_y.addKey(0.f);
	_angle.addKey(0.f);
	_center = FPoint2D(0.f, 0.f);
	_scaleX.addKey(1.f);
	_scaleY.addKey(1.f);
	_last = NULL;
	_offparent = OffParentOrder_top;
}

MovingPart::MovingPart(MovingPart &movinPart) 
: Bone(movinPart)
{
	_loop = movinPart._loop;
	_discontinuous = movinPart._discontinuous;
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
		Bone *b;
		if ((*i)->boneType == BT_MovingPart) {
			b = new MovingPart(*static_cast<MovingPart *>(*i));
		} else if ((*i)->boneType == BT_IKTwoBone) {
			b = new IKTwoBone(*static_cast<IKTwoBone *>(*i));
		} else {
			assert(false);
		}
		_bones.push_back(b);
	}
	ResortBones();
}


void MovingPart::Draw(float p) {
	assert(0.f <= p && p < 1);
	Render::PushMatrix();
	float dp = p;
	if (_discontinuous) {
		if (_loop) {
			dp = static_cast<int>(p * (_x.keys.size() - 1)) / static_cast<float>(_x.keys.size() - 1);
		} else {
			dp = static_cast<int>(p * _x.keys.size()) / static_cast<float>(_x.keys.size());
		}
		assert(0.f <= dp && dp < 1);
	}
	Render::MatrixMove(_x.getGlobalFrame(dp), _y.getGlobalFrame(dp));
	Render::MatrixRotate(_angle.getGlobalFrame(dp));
	Render::MatrixScale(_scaleX.getGlobalFrame(dp), _scaleY.getGlobalFrame(dp));

#ifdef ANIMATION_EDITOR
	matrix = Render::GetCurrentMatrix();
#endif

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

IKTwoBone::~IKTwoBone() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

IKTwoBone::IKTwoBone(TiXmlElement *xe, bool loop)
: Bone(xe->Attribute("name"), BT_IKTwoBone)
{
	_offparent = ((xe->Attribute("offparent") && std::string(xe->Attribute("offparent")) == "bottom") ? OffParentOrder_bottom : OffParentOrder_top);
	const char *tmp = xe->Attribute("order");
	_invert = tmp && std::string(tmp) == "invert";
	_anchorPos.x = fatof(xe->Attribute("anchorX"));
	_anchorPos.y = fatof(xe->Attribute("anchorY"));
	_connectPos.x = fatof(xe->Attribute("connectX"));
	_connectPos.y = fatof(xe->Attribute("connectY"));
	_targetPos.x = fatof(xe->Attribute("targetX"));
	_targetPos.y = fatof(xe->Attribute("targetY"));
	tmp = xe->Attribute("freeBones");
	_freeBones = tmp && std::string(tmp) == "true";
	_firstBoneLength = (_anchorPos - _connectPos).Length();
	_secondBoneLength = (_connectPos - _targetPos).Length();
	tmp = xe->Attribute("firstTexture"); 
	_first.Set(Core::getTexture(tmp));
	tmp = xe->Attribute("secondTexture"); 
	_second.Set(Core::getTexture(tmp));
	TiXmlElement *pos = xe->FirstChildElement("pos");
	while (pos) {
		_x.addKey(Math::Read(pos, "x", 0.f));
		_y.addKey(Math::Read(pos, "y", 0.f));
		pos = pos->NextSiblingElement("pos");
	}
	_x.CalculateGradient(loop);
	_y.CalculateGradient(loop);

	_angleSign = static_cast<int>(Math::Read(xe, "angleSign", 0.f));

	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back( new MovingPart(element) );
		} else if (name == "IKTwoBone") {
			_bones.push_back( new IKTwoBone(element, loop) );
		}
		element = element->NextSiblingElement();
	}
	ResortBones();
	_lastScreenConnectPos = _connectPos;

	_baseAngleFirst = atan2(_connectPos.y - _anchorPos.y, _connectPos.x - _anchorPos.x);
	_baseAngleSecond = atan2(_targetPos.y -  _connectPos.y, _targetPos.x -  _connectPos.x);
}

IKTwoBone::IKTwoBone(IKTwoBone &twoBone) 
: Bone(twoBone)
{
	_first = twoBone._first;
	_second = twoBone._second;
	_invert = twoBone._invert;
	_anchorPos = twoBone._anchorPos;
	_connectPos = twoBone._connectPos;
	_targetPos = twoBone._targetPos;
	_firstBoneLength = twoBone._firstBoneLength;
	_secondBoneLength = twoBone._secondBoneLength;
	_baseAngleFirst = twoBone._baseAngleFirst;
	_baseAngleSecond = twoBone._baseAngleSecond;
	_lastScreenConnectPos = twoBone._lastScreenConnectPos;
	_angleSign = twoBone._angleSign;
	_x = twoBone._x;
	_y = twoBone._y;

	_freeBones = twoBone._freeBones;

	for (BoneList::iterator i = twoBone._bones.begin(), e = twoBone._bones.end(); i != e; ++i) {
		Bone *b;
		if ((*i)->boneType == BT_MovingPart) {
			b = new MovingPart(*static_cast<MovingPart *>(*i));
		} else if ((*i)->boneType == BT_IKTwoBone) {
			b = new IKTwoBone(*static_cast<IKTwoBone *>(*i));
		} else {
			assert(false);
		}
		_bones.push_back(b);
	}
	ResortBones();
}

void IKTwoBone::Draw(float p) { // [ 0<= p <= 1 ]
	assert(0 <= p && p < 1);
	SetPos(_x.getGlobalFrame(p), _y.getGlobalFrame(p));

	for (unsigned int i = 0; i < _bottomBone.size(); ++i) {
		_bottomBone[i]->Draw(p);
	}
	//_first.SetTransform(_firstTransform);
	//_second.SetTransform(_secondTransform);
	if (_invert) {
		_second.Render();
		_first.Render();
	} else {
		_first.Render();
		_second.Render();
	}
	for (unsigned int i = 0; i < _topBone.size(); ++i) {
		_topBone[i]->Draw(p);
	}
}

bool IKTwoBone::PixelCheck(const FPoint2D &pos) {
	if (_second.PixelCheck(pos) || _first.PixelCheck(pos)) {
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

void IKTwoBone::SetPos(float x, float y) 
{
	float angleFirst, angleSecond;
	FPoint2D q1, q2;

	if (FPoint2D(_anchorPos.x - x, _anchorPos.y - y).Length() < 1.f) {
		angleFirst = - _baseAngleFirst;
		angleSecond = M_PI - _baseAngleSecond;
	} else if (Math::GetCirclesIntersect2(_anchorPos.x, _anchorPos.y, _firstBoneLength, 
				x, y, _secondBoneLength, q1, q2)) {
		if ((q1 - _lastScreenConnectPos).Length() > (q2 - _lastScreenConnectPos).Length() && _angleSign == 0) {
			q1 = q2;
		} else if (_angleSign == 1 && Math::VMul(FPoint2D(x, y) - q2, (q2 - _anchorPos)) > 0) {
			q1 = q2;
		} else if (_angleSign == -1 && Math::VMul(FPoint2D(x, y) - q2, (q2 - _anchorPos)) < 0) {
			q1 = q2;
		}
		angleFirst = atan2(q1.y - _anchorPos.y, q1.x - _anchorPos.x) - _baseAngleFirst;
		angleSecond = atan2(y - q1.y, x - q1.x) - _baseAngleSecond;
	} else {
		angleFirst = angleSecond = atan2(y - _anchorPos.y, x - _anchorPos.x);
		angleFirst -= _baseAngleFirst;
		angleSecond -= _baseAngleSecond;
		q1 = q2 = _anchorPos + *(_connectPos - _anchorPos).Rotate(angleFirst);
	}

	/*
	_firstTransform.Unit();
	_firstTransform.Move(-_anchorPos.x, -_anchorPos.y);
	_firstTransform.Rotate(angleFirst);
	_firstTransform.Move(_anchorPos.x, _anchorPos.y);

	_secondTransform.Unit();
	_secondTransform.Move(-_connectPos.x, -_connectPos.y);
	_secondTransform.Rotate(angleSecond);
	_secondTransform.Move(q1.x, q1.y);
	*/
	_lastScreenConnectPos = q1;
}


Animation::~Animation() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

#ifdef ANIMATION_EDITOR
void Animation::EditorCall(CallBones myCall, void *parent) {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->EditorCall(myCall, parent);
	}
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

Animation::Animation(TiXmlElement *xe)
: _timeCounter(0.f)
, _texturesLoaded(false)
{
	_loop = (std::string(xe->Attribute("loop")) == "true");
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

	FPoint2D pos(Math::Read(xe, "x", 0.f), Math::Read(xe, "y", 0.f));
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
	_subPosition.Move(pos.x, pos.y);
	SetLoop(_loop);
}

Animation::Animation() 
{
	_pivotPos = FPoint2D(0.f, 0.f);
	_subPosition.Unit();
	_time = 1.f;
	_timeCounter = 0.f;
	_texturesLoaded = false;
	_loop = true;
}

MovingPart * Animation::addBone(const char *boneName, MovingPart *newChildBone) {
	if (newChildBone == NULL) {
		newChildBone = new MovingPart(boneName);
	} else {
		strcpy_s(newChildBone->boneName, boneName);
	}
	_bones.push_back(newChildBone);
	newChildBone->SetLoop(_loop);
	return newChildBone;
}

bool Animation::removeBone(MovingPart *movingPart) {
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		if (*i == movingPart) {
			delete (*i);
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
	_loop = animation._loop;
	
	_bones;
	for (BoneList::iterator i = animation._bones.begin(), e = animation._bones.end(); i != e; ++i) {
		Bone *b;
		if ((*i)->boneType == BT_MovingPart) {
			b = new MovingPart(*static_cast<MovingPart *>(*i));
		} else if ((*i)->boneType == BT_IKTwoBone) {
			b = new IKTwoBone(*static_cast<IKTwoBone *>(*i));
		} else {
			assert(false);
		}
		_bones.push_back(b);
	}
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
	info.discontinuous = _discontinuous;
	info.offparent = _offparent;
}

void MovingPart::Set(const MovingPartInfo &info) {
	_x = info.x;
	_y = info.y;
	_angle = info.angle;
	_center = info.center;
	_scaleX = info.scaleX;
	_scaleY = info.scaleY;
	_discontinuous = info.discontinuous;
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
	info.loop = _loop;
}

void Animation::Set(const AnimationInfo &info) {
	_pivotPos = info.pivotPos;
	_time = info.time;
	if (_loop != info.loop) {
		SetLoop(info.loop);
		_loop = info.loop;
	}
}

void Animation::SetLoop(bool loop) {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->SetLoop(loop);
	}
}
