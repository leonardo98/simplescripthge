#ifndef MYENGINE_ANIMATION_H
#define MYENGINE_ANIMATION_H

#include "Render.h"
#include "SplinePath.h"
#include "Matrix.h"

enum BoneType {
	BT_MovingPart,
	BT_IKTwoBone
};

class Bone;

typedef std::vector<Bone *> BoneList;

class Bone
{
public:
	Bone(const char *id, const Matrix *main, const BoneType bt);
	Bone(Bone &bone);
	virtual void Draw(const Matrix &transform, float p) = 0; // [ 0<= p <= 1 ]
	virtual bool PixelCheck(const FPoint2D &pos) = 0;
	virtual bool ReplaceTexture(const std::string &boneName, const char *texture);
	virtual ~Bone() {}
	const std::string boneName;
	const BoneType boneType;
protected:
	void ResortBones();
	const Matrix *_mainTransform;
	BoneList _topBone;// links
	BoneList _bottomBone;// links
	BoneList _bones;
	enum OffParentOrder {
		top,
		bottom
	};
	OffParentOrder _offparent;

	friend class AnimationEditor;
};


class MovingPart : public Bone
{
public:
	~MovingPart();
	MovingPart(TiXmlElement *xe, bool loop, const Matrix *main);
	MovingPart(MovingPart &movinPart);
	virtual void Draw(const Matrix &transform, float p); // [ 0<= p <= 1 ]
	virtual bool PixelCheck(const FPoint2D &pos);
	virtual bool ReplaceTexture(const std::string &boneName, const char *texture);
private:
	Matrix _transform;
	SplinePath _x;
	SplinePath _y;
	SplinePath _angle;
	FPoint2D _center;
	SplinePath _scaleX;
	SplinePath _scaleY;
	std::vector<StaticSprite *> _parts;
	StaticSprite *_last;
	bool _discontinuous;
	bool _loop;
};

class IKTwoBone : public Bone {
public:
	~IKTwoBone();
	IKTwoBone(TiXmlElement *xe, bool loop, const Matrix *main);
	IKTwoBone(IKTwoBone &twoBone);
	virtual void Draw(const Matrix &transform, float p); // [ 0<= p <= 1 ]
	virtual bool PixelCheck(const FPoint2D &pos);
private:
	void SetPos(float x, float y);
	StaticSprite _first;
	StaticSprite _second;
	bool _invert;
	FPoint2D _anchorPos;
	FPoint2D _connectPos;
	FPoint2D _targetPos;
	float _firstBoneLength;
	float _secondBoneLength;
	float _baseAngleFirst;
	float _baseAngleSecond;
	FPoint2D _lastScreenConnectPos;
	int _angleSign;
	Matrix _firstTransform;
	Matrix _secondTransform;
	SplinePath _x;
	SplinePath _y;

	bool _freeBones;//unsafe to rotate|scale animation;
	const Matrix *_local;
};

class Animation
{
public:
	Animation(TiXmlElement *xe);
	Animation(Animation &animation);
	virtual ~Animation();
	void Draw();
	void Draw(float position);
	void Update(float dt);
	float Time();
	void SetPos(const FPoint2D &pos, bool mirror);
	bool PixelCheck(const FPoint2D &pos);
	bool ReplaceTexture(const std::string &boneName, const char *texture);
	//bool RemoveBone(const std::string &boneName, Bone *bone); 
private:
	FPoint2D _pivotPos;
	Matrix _mainMatrix;
	float _time;
	float _timeCounter;
	BoneList _bones;

	friend class AnimationEditor;
};

#endif//MYENGINE_ANIMATION_H
