#include "Animation.h"
#include "Core.h"
#include "Math.h"

Bone::Bone(const char *name, const Matrix *main, const BoneType bt) 
: boneName( name ? name : "" )
, _mainTransform(main)
, boneType(bt)
{}

Bone::Bone(Bone &bone) 
: boneName(bone.boneName)
, _mainTransform(bone._mainTransform)
, boneType(bone.boneType)
, _offparent(bone._offparent)
{}

bool Bone::ReplaceTexture(const std::string &boneName, const char *texture) {
	return false;
}

void Bone::ResortBones() {
	_topBone.clear();
	_bottomBone.clear();
	for (int i = 0; i < _bones.size(); ++i) {
		if (_bones[i]->_offparent == top) {
			_topBone.push_back(_bones[i]);
		} else if (_bones[i]->_offparent == bottom) {
			_bottomBone.push_back(_bones[i]);
		} else {
			assert(false);
		}
	}
}

MovingPart::~MovingPart() {
	for (int i = 0; i < _parts.size(); ++i) {
		delete _parts[i];
	}
	for (int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

MovingPart::MovingPart(TiXmlElement *xe, bool loop, const Matrix *main)
: Bone(xe->Attribute("name"), main, BT_MovingPart)
{
	_offparent = ((xe->Attribute("offparent") && std::string(xe->Attribute("offparent")) == "bottom") ? bottom : top);
	_center.x = Math::Read(xe, "centerX", 0.f);
	_center.y = Math::Read(xe, "centerY", 0.f);
	const char *name = xe->Attribute("texture"); 
	if (name) {
		StaticSprite *s = new StaticSprite();
		s->Set(Core::getTexture(name));
		_parts.push_back(s);
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
	_x.CalculateGradient(loop);
	_y.CalculateGradient(loop);
	_scaleX.CalculateGradient(loop);
	_scaleY.CalculateGradient(loop);
	_angle.CalculateGradient(loop);

	TiXmlElement *part = xe->FirstChildElement("part");
	assert((part == NULL && _parts.size() == 1) || (part != NULL && _parts.size() == 0));
	while (part) {
		const char *name = part->Attribute("texture"); 
		StaticSprite *s = new StaticSprite();
		s->Set(Core::getTexture(name));
		_parts.push_back(s);
		part = part->NextSiblingElement("part");
	}
	_last = NULL;
	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back( new MovingPart(element, loop, main) );
		} else if (name == "IKTwoBone") {
			_bones.push_back( new IKTwoBone(element, loop, main) );
		}
		element = element->NextSiblingElement();
	}
	ResortBones();
}

MovingPart::MovingPart(MovingPart &movinPart) 
: Bone(movinPart)
{
	_transform = movinPart._transform;
	_x = movinPart._x;
	_y = movinPart._y;
	_angle = movinPart._angle;
	_center = movinPart._center;
	_scaleX = movinPart._scaleX;
	_scaleY = movinPart._scaleY;
	_last = movinPart._last;

	for (std::vector<StaticSprite *>::iterator i = movinPart._parts.begin(), e = movinPart._parts.end(); i != e; ++i) {
		_parts.push_back(new StaticSprite(*(*i)));
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


void MovingPart::Draw(const Matrix &transform, float p) {
	_transform.Unit();
	_transform.Move(-_center.x, -_center.y);
	_transform.Scale(_scaleX.getGlobalFrame(p), _scaleY.getGlobalFrame(p));
	_transform.Rotate(_angle.getGlobalFrame(p));
	_transform.Move(_x.getGlobalFrame(p), _y.getGlobalFrame(p));
	
	_transform.Mul(transform);

	for (int i = 0; i < _bottomBone.size(); ++i) {
		_bottomBone[i]->Draw(_transform, p);
	}
	if (_parts.size()) {
		_last = _parts[static_cast<int>(p * _parts.size())];
		_last->SetTransform(_transform);
		_last->Render();
	}
	for (int i = 0; i < _topBone.size(); ++i) {
		_topBone[i]->Draw(_transform, p);
	}
}

bool MovingPart::PixelCheck(const FPoint2D &pos) {
	if (_last && _last->PixelCheck(pos)) {
		return true;
	}
	for (int i = 0; i < _bottomBone.size(); ++i) {
		if (_bottomBone[i]->PixelCheck(pos)) {
			return true;
		}
	}
	for (int i = 0; i < _topBone.size(); ++i) {
		if (_topBone[i]->PixelCheck(pos)) {
			return true;
		}
	}
	return false;
}

bool MovingPart::ReplaceTexture(const std::string &boneName, const char *texture) {
	bool success = (Bone::boneName == boneName);
	if (success) {
		for (int i = 0; i < _parts.size(); ++i) {
			delete _parts[i];
		}
		_parts.clear();
		StaticSprite *s = new StaticSprite();
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
	for (int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

IKTwoBone::IKTwoBone(TiXmlElement *xe, bool loop, const Matrix *main)
: Bone(xe->Attribute("name"), main, BT_IKTwoBone)
{
	_offparent = ((xe->Attribute("offparent") && std::string(xe->Attribute("offparent")) == "bottom") ? bottom : top);
	const char *tmp = xe->Attribute("order");
	_invert = tmp && std::string(tmp) == "invert";
	_anchorPos.x = atof(xe->Attribute("anchorX"));
	_anchorPos.y = atof(xe->Attribute("anchorY"));
	_connectPos.x = atof(xe->Attribute("connectX"));
	_connectPos.y = atof(xe->Attribute("connectY"));
	_targetPos.x = atof(xe->Attribute("targetX"));
	_targetPos.y = atof(xe->Attribute("targetY"));
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

	_angleSign = Math::Read(xe, "angleSign", 0.f);

	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back( new MovingPart(element, loop, main) );
		} else if (name == "IKTwoBone") {
			_bones.push_back( new IKTwoBone(element, loop, main) );
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
	_firstTransform = twoBone._firstTransform;
	_secondTransform = twoBone._secondTransform;
	_x = twoBone._x;
	_y = twoBone._y;

	_freeBones = twoBone._freeBones;
	_local = twoBone._local;

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

void IKTwoBone::Draw(const Matrix &transform, float p) { // [ 0<= p <= 1 ]
	_local = &transform;
	SetPos(_x.getGlobalFrame(p), _y.getGlobalFrame(p));
	_firstTransform.Mul(transform);
	_secondTransform.Mul(transform);

	for (int i = 0; i < _bottomBone.size(); ++i) {
		_bottomBone[i]->Draw(_secondTransform, p);
	}
	_first.SetTransform(_firstTransform);
	_second.SetTransform(_secondTransform);
	if (_invert) {
		_second.Render();
		_first.Render();
	} else {
		_first.Render();
		_second.Render();
	}
	for (int i = 0; i < _topBone.size(); ++i) {
		_topBone[i]->Draw(_secondTransform, p);
	}
}

bool IKTwoBone::PixelCheck(const FPoint2D &pos) {
	if (_second.PixelCheck(pos) || _first.PixelCheck(pos)) {
		return true;
	}
	for (int i = 0; i < _bottomBone.size(); ++i) {
		if (_bottomBone[i]->PixelCheck(pos)) {
			return true;
		}
	}
	for (int i = 0; i < _topBone.size(); ++i) {
		if (_topBone[i]->PixelCheck(pos)) {
			return true;
		}
	}
	return false;
}

void IKTwoBone::SetPos(float x, float y) 
{
	if (_freeBones) {
		Matrix t0, t;
		t0.Mul(*_local);
		t.MakeRevers(*_mainTransform);
		t0.Mul(t);
		t.MakeRevers(t0);
		t.Mul(x, y);
	}
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

	_firstTransform.Unit();
	_firstTransform.Move(-_anchorPos.x, -_anchorPos.y);
	_firstTransform.Rotate(angleFirst);
	_firstTransform.Move(_anchorPos.x, _anchorPos.y);

	_secondTransform.Unit();
	_secondTransform.Move(-_connectPos.x, -_connectPos.y);
	_secondTransform.Rotate(angleSecond);
	_secondTransform.Move(q1.x, q1.y);

	_lastScreenConnectPos = q1;
}


Animation::~Animation() {
	for (int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

Animation::Animation(TiXmlElement *xe)
: _timeCounter(0.f)
{
	bool loop = (std::string(xe->Attribute("loop")) == "true");
	_time = atof(xe->Attribute("time"));
	_pivotPos.x = atoi(xe->Attribute("pivotX"));
	_pivotPos.y = atoi(xe->Attribute("pivotY"));

	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back(new MovingPart(element, loop, &_mainMatrix));
		} else if (name == "IKTwoBone") {
			_bones.push_back( new IKTwoBone(element, loop, &_mainMatrix) );
		}
		element = element->NextSiblingElement();
	}

	FPoint2D pos(Math::Read(xe, "x", 0.f), Math::Read(xe, "y", 0.f));
	_mainMatrix.Unit();
	_mainMatrix.Move(-_pivotPos.x, -_pivotPos.y);
	_mainMatrix.Move(pos.x, pos.y);
}

Animation::Animation(Animation &animation) {
	_pivotPos = animation._pivotPos;
	_mainMatrix = animation._mainMatrix;
	_time = animation._time;
	_timeCounter = animation._timeCounter;
	
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
	_mainMatrix.Unit();
	_mainMatrix.Move(-_pivotPos.x, -_pivotPos.y);
	if (mirror) {
		_mainMatrix.Scale(-1.f, 1.f);
	}
	_mainMatrix.Move(pos.x, pos.y);
}

void Animation::SetPos(const Matrix &transform) {
	_mainMatrix.Unit();
	_mainMatrix.Move(-_pivotPos.x, -_pivotPos.y);
	_mainMatrix.Mul(transform);
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
	float p = _timeCounter / _time;
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->Draw(_mainMatrix, p);
	}
}

void Animation::Draw(float position) {
	assert(0.f <= position && position <= 1.f);
	for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i) {
		(*i)->Draw(_mainMatrix, position);
	}
}

void Animation::Update(float dt) {
	while ((_timeCounter += dt) >= _time) {
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
