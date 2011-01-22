#pragma once

#include "..\Core\InputSystem.h"
#include "..\Core\Messager.h"
#include "Box2D\Box2D.h"
#include <cstdlib>
#include "BodyTemplate.h"

#ifndef SIMULATOR_H
#define SIMULATOR_H

class Simulator;
struct Settings;

#define	RAND_LIMIT	32767

/// Random number in range [-1,1]
inline float32 RandomFloat()
{
	float32 r = (float32)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float32 RandomFloat(float32 lo, float32 hi)
{
	float32 r = (float32)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}

/// Simulator settings. Some can be controlled in the GUI.
struct Settings
{
	Settings() :
		hz(30.0f),
		velocityIterations(9),
		positionIterations(5),
		drawStats(0),
		drawShapes(1),
		drawJoints(1),
		drawAABBs(0),
		drawPairs(0),
		drawContactPoints(0),
		drawContactNormals(0),
		drawContactForces(0),
		drawFrictionForces(0),
		drawCOMs(0),
		enableWarmStarting(0),
		enableContinuous(1),
		pause(0),
		singleStep(0)
		{}

	float32 hz;
	int32 velocityIterations;
	int32 positionIterations;
	int32 drawShapes;
	int32 drawJoints;
	int32 drawAABBs;
	int32 drawPairs;
	int32 drawContactPoints;
	int32 drawContactNormals;
	int32 drawContactForces;
	int32 drawFrictionForces;
	int32 drawCOMs;
	int32 drawStats;
	int32 enableWarmStarting;
	int32 enableContinuous;
	int32 pause;
	int32 singleStep;
};

// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class DestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint);

	Simulator* test;
};

const int32 k_maxContactPoints = 2048;

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
};

// ����� ����������� ���������� ����� � �� �����������
class Simulator 
	: public b2ContactListener
	, public InputSystem
	, public Messager
{
public:
	
	Simulator(TiXmlElement *xe);
	virtual ~Simulator();

	virtual void Step(Settings* settings);
	virtual b2Body * AddElement(BodyTypes type);
	virtual void OnMouseDown(hgeVector mousePos);
	virtual void OnMouseUp();
	void OnMouseMove(hgeVector mousePos);
	virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

	// Callbacks for derived classes.
	virtual void BeginContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void EndContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(const b2Contact* contact, const b2ContactImpulse* impulse);
	virtual bool IsMouseOver(hgeVector mousePos);
	virtual bool OnMouseWheel(int direction);
	virtual void Draw();
	virtual void Update(float deltaTime);

	void LoadTemplates(const std::string &filename);

	typedef std::vector<BodyTemplate *> Collection;
	Collection _collection;

protected:
	
	/*typedef std::list<b2Body *> Bodyes;
	Bodyes _bodyes;*/

	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	b2Body* m_groundBody;
	b2AABB m_worldAABB;
	ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;
	DestructionListener m_destructionListener;
	int32 m_textLine;
	b2World* m_world;
	b2Body* m_bomb;
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_bombSpawnPoint;
	bool m_bombSpawning;
	b2Vec2 m_mouseWorld;
	int32 m_stepCount;

	hgeVector _lastMousePos;
	Settings settings;


	Texture *_allElements;
	inline void DrawElement(hgeVertex *&buf, const BodyTemplate::UV *uv, const b2Vec2 &pos, const FPoint2D *angles);

	float _viewScale; // ������� ���� �����
	FPoint2D _worldCenter; // ���������� ������ �����(0,0) �� ������

	float _angleMultiplier;
	b2Body *_selectedBody;
	bool _editor;

	void Explosion(b2Vec2 pos, float radius, float maxForce);
	inline void DrawLine(const b2Vec2 &a, const b2Vec2 &b, DWORD color = 0xFFFFF0F0);
	virtual void OnMessage(const std::string &message);
	virtual void OnDoubleClick(hgeVector mousePos);

	struct BodyState {
		BodyTypes type;
		b2Vec2 pos;
		float angle;
	};
	std::vector<BodyState> _state;
	TiXmlDocument _doc;
	std::string _currentLevel;
	enum {
		WaitNone,
		WaitForLevelOpen,
		WaitForLevelSave
	} _waitState;

	void SaveState();
	void ResetState();

	int round(float a);

	float _signal;
};

#endif //SIMULATOR_H
