#ifndef MYENGINE_SIMULATOR_H
#define MYENGINE_SIMULATOR_H

#include "../Core/InputSystem.h"
#include "../Core/Messager.h"
#include "../Core/Render.h"
#include "Box2D/Box2D.h"
#include <cstdlib>
#include "BodyTemplate.h"
#include "../Helpers/Counter.h"

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
		hz(50.0f),
		velocityIterations(8),
		positionIterations(3),
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

// класс описывающий физическую сцену и ее отображение
class Simulator 
	: public b2ContactListener
	, public InputSystem
	, public Messager
{
public:
	
	Simulator(TiXmlElement *xe);
	virtual ~Simulator();

	virtual void Step(Settings* settings);

	struct MyBody { // структура которая прикрепляется к b2Body в UserData для идентификации 
					// отдельно храню тут ширину и высоту
		BodyTemplate *base;
		float width;
		float height;
		float radius;
		bool broken;
		MyBody() : width(0.f), height(0.f), radius(0.f), broken(false) {
		}
	};
	struct BodyState : public MyBody
	{ // структура для хранения начального положения элементов на уровне
	  // используется для чтения их из файла, для записи обратно
		b2Vec2 pos;
		float angle;
	};
	virtual b2Body * AddElement(const std::string &typeId);
	virtual b2Body * AddElement(const BodyState &bodyState);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	void OnMouseMove(const FPoint2D &mousePos);
	virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

	// Callbacks for derived classes.
	virtual void BeginContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void EndContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual bool OnMouseWheel(int direction);
	virtual void Draw();
	virtual void Update(float deltaTime);

	void LoadTemplates(const std::string &filename);

	typedef std::list<BodyTemplate *> Collection;
	BodyTemplate *_fish;
	BodyTemplate *_cat;
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
	bool _mouseDown;
	bool _waitYesNoNewLevel;
	bool _waitYesNoDelSelected;
	bool _waitYesNoOverwrite;
	bool _waitAddNewElem;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	const float SLIDER_SCALE;
	const float SLIDER_MIN;

	std::string _saveLevelName;
	TiXmlElement *_saveLevelXml;
	void SaveLevel(const std::string &levelName);

	FPoint2D _lastMousePos;
	Settings settings;


	Texture *_allElements;

	float _viewScale; // масштаб всей Сцены
	FPoint2D _worldCenter; // координаты центра Сцены(0,0) на экране
	bool _netVisible;

	float _angleMultiplier;
	bool _editor;

	int _finish;// ==0 - закончились и бомбы и синие; &1 - еще есть динамит; &2 - еще есть синие
	bool CanLevelStart();
	bool IsLevelFinish();
	void Explosion(b2Vec2 pos, float radius, float maxForce);
	inline void DrawLine(const b2Vec2 &a, const b2Vec2 &b, DWORD color = 0xFFFFF0F0);
	virtual void OnMessage(const std::string &message);

	typedef std::list<BodyState> BodyStates;
	BodyStates _state;
	TiXmlDocument _doc;
	std::string _currentLevel;
	void EraseBody(b2Body *body);
	void EraseAllBodyes();
	enum {
		WaitNone,
		WaitForLevelOpen,
		WaitForLevelSave,
	} _waitState;

	void SaveState();
	void ResetState();

	int round(float a);

	float _signal;
	bool _userLevelWin;
	Counter _lastTimer;
	Counter _startLevel; // ждем пока устаканиться уровень

	b2Body *_selectedBody;
	void InitParams(b2Body *body);
	UV _selectedUV[4];
	inline void DrawElement(Vertex *&buf, const UV *uv, const b2Vec2 &pos, const FPoint2D *angles);
};

#endif//MYENGINE_SIMULATOR_H
