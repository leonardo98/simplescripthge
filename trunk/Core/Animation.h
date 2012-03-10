#ifndef MYENGINE_ANIMATION_H
#define MYENGINE_ANIMATION_H

#include "Render.h"
#include "MotionValues.h"
#include "Matrix.h"

enum BoneType {
	BT_MovingPart,
	BT_IKTwoBone
};

enum OffParentOrder {
	OffParentOrder_top,
	OffParentOrder_bottom
};

enum MovingType {
	MovingType_line,
	MovingType_spline,
	MovingType_discontinuous
};

struct MovingPartkey {
	float time;
	float x;
	float y;
	float angle;
	float sx;
	float sy;
};

class MovingPart;

typedef std::vector<MovingPart *> BoneList;

typedef void *(* const CallBones)(void *, char *, MovingPart *); 

class AnimationPart : public StaticSprite {
public:
	const std::string fileName;
	AnimationPart(const char *FileName) : fileName(FileName) {}
};

struct MovingPartInfo {
	MotionValues x;
	MotionValues y;
	MotionValues angle;
	MotionValues scaleX;
	MotionValues scaleY;
	FPoint2D center;
	std::vector<std::string> partsNames;
	MovingType movingType;
	OffParentOrder offparent;
};

struct AnimationInfo {
	FPoint2D pivotPos;
	float time;
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
	virtual void CalcGradient();
	virtual bool removeBone(Bone *bone) {assert(false); return false;}
	void ResortBones();

	// for editor
#ifdef ANIMATION_EDITOR
	virtual hgeSprite * GetSprite() { return NULL; }
	bool hasBone(const std::string &boneName); 
	void Rename(const char *newName);
	void *parent;
	Matrix matrix;
	int order;
#endif

protected:
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
	virtual void CalcGradient();
	virtual hgeSprite * GetSprite();

	// for editor
	MovingPart(const std::string &boneName);
	MovingPart * addBone(const char *boneName, MovingPart *newChildBone = NULL, MovingPart *afterBone = NULL);
	bool removeBone(Bone *bone);
	void SetKey(int index, const MovingPartkey &key);
	void SaveToXml(TiXmlElement *xe);
	void * EditorCall(CallBones myCall, void *parent);

private:
	FPoint2D _center;
	MotionValues _x;
	MotionValues _y;
	MotionValues _angle;
	MotionValues _scaleX;
	MotionValues _scaleY;
	std::vector<AnimationPart *> _parts;
	StaticSprite *_last;
	MovingType _movingType;
};

class Animation
{
public:
	Animation(TiXmlElement *xe);
	Animation(Animation &animation);
	virtual ~Animation();
	void * EditorCall(CallBones myCall, void *parent);
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
	void CalcGradient();
	
	// for editor
	Animation();
	MovingPart * addBone(const char *boneName, MovingPart *newChildBone = NULL, MovingPart *afterBone = NULL);
	bool removeBone(MovingPart *movingPart); 
	bool hasBone(const std::string &boneName); 
	void SaveToXml(TiXmlElement *xe);

private:
	bool _texturesLoaded;
	FPoint2D _pivotPos;
	float _time;
	float _timeCounter;
	BoneList _bones;
	Matrix _subPosition;

	friend class AnimationEditor;
};

#endif//MYENGINE_ANIMATION_H
