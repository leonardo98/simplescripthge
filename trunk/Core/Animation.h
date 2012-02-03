#ifndef MYENGINE_ANIMATION_H
#define MYENGINE_ANIMATION_H

#include "Render.h"
#include "SplinePath.h"
#include "Matrix.h"

enum BoneType {
	BT_MovingPart,
	BT_IKTwoBone
};

enum OffParentOrder {
	top,
	bottom
};

class Bone;

typedef std::vector<Bone *> BoneList;
typedef void *(* const CallBones)(void *, char *, Bone *); 

class AnimationPart : public StaticSprite {
public:
	const std::string fileName;
	AnimationPart(const char *FileName) : fileName(FileName) {}
};

struct MovingPartInfo {
	SplinePath x;
	SplinePath y;
	SplinePath angle;
	FPoint2D center;
	SplinePath scaleX;
	SplinePath scaleY;
	std::vector<std::string> partsNames;
	bool discontinuous;
	OffParentOrder offparent;
};

struct AnimationInfo {
	FPoint2D pivotPos;
	float time;
	bool loop;
};

class Bone
{
public:
	Bone(const char *id, const BoneType bt);
	Bone(Bone &bone);
	virtual void Draw(float p) = 0; // [ 0<= p <= 1 ]
	virtual bool PixelCheck(const FPoint2D &pos) = 0;
	virtual bool ReplaceTexture(const std::string &boneName, const char *texture);
	virtual ~Bone() {}
#define MAX_LENGTH 256
	char boneName[MAX_LENGTH];
	const BoneType boneType;
	virtual void LoadTextures() {}
	virtual void UnloadTextures() {}
	virtual void Get(MovingPartInfo &info) const {}
	virtual void Set(const MovingPartInfo &info) {}
	virtual void SetLoop(bool loop);

	// for editor
	virtual hgeSprite * GetSprite() { return NULL; }
	void EditorCall(CallBones myCall, void *parent);
	bool hasBone(const std::string &boneName); 
	void Rename(const char *newName);
protected:
	void ResortBones();
	BoneList _topBone;// links
	BoneList _bottomBone;// links
	BoneList _bones;
	OffParentOrder _offparent;

	friend class AnimationEditor;
};

class MovingPart : public Bone
{
public:
	~MovingPart();
	MovingPart(TiXmlElement *xe);
	MovingPart(MovingPart &movinPart);
	virtual void Draw(float p); // [ 0<= p <= 1 ]
	virtual bool PixelCheck(const FPoint2D &pos);
	virtual bool ReplaceTexture(const std::string &boneName, const char *texture);
	virtual void LoadTextures();
	virtual void UnloadTextures();
	virtual void Get(MovingPartInfo &info) const;
	virtual void Set(const MovingPartInfo &info);
	virtual void SetLoop(bool loop);
	virtual hgeSprite * GetSprite();

	// for editor
	MovingPart(const std::string &boneName);
	MovingPart * addBone(const std::string &boneName);
	bool removeBone(MovingPart *movingPart); 

private:
	FPoint2D _center;
	SplinePath _x;
	SplinePath _y;
	SplinePath _angle;
	SplinePath _scaleX;
	SplinePath _scaleY;
	std::vector<AnimationPart *> _parts;
	StaticSprite *_last;
	bool _discontinuous;
	bool _loop;
};

class IKTwoBone : public Bone {
public:
	~IKTwoBone();
	IKTwoBone(TiXmlElement *xe, bool loop);
	IKTwoBone(IKTwoBone &twoBone);
	virtual void Draw(float p); // [ 0<= p <= 1 ]
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
	SplinePath _x;
	SplinePath _y;

	bool _freeBones;//unsafe to rotate|scale animation;
};

class Animation
{
public:
	Animation(TiXmlElement *xe);
	Animation(Animation &animation);
	virtual ~Animation();
	void EditorCall(CallBones myCall, void *parent);
	void Draw();
	void Draw(float position);
	void Update(float dt);
	float Time();
	void SetPos(const FPoint2D &pos, bool mirror);
	bool PixelCheck(const FPoint2D &pos);
	bool ReplaceTexture(const std::string &boneName, const char *texture);
	void LoadTextures();
	void UnloadTextures();
	bool TextureLoaded();
	void Get(AnimationInfo &info) const;
	void Set(const AnimationInfo &info);
	void SetLoop(bool loop);
	
	// for editor
	Animation();
	MovingPart * addBone(const std::string &boneName);
	bool removeBone(MovingPart *movingPart); 
	bool hasBone(const std::string &boneName); 

private:
	bool _texturesLoaded;
	FPoint2D _pivotPos;
	float _time;
	bool _loop;
	float _timeCounter;
	BoneList _bones;
	Matrix _subPosition;

	friend class AnimationEditor;
};

#endif//MYENGINE_ANIMATION_H
