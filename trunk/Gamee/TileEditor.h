#ifndef MYENGINE_TILEEDITOR_H
#define MYENGINE_TILEEDITOR_H

#include "../Core/InputSystem.h"
#include "../Core/Messager.h"
#include "../Core/Render.h"
#include "Box2D/Box2D.h"
#include <cstdlib>
#include "BodyTemplate.h"
#include "../Helpers/Counter.h"
#include "CommonBox2DTypes.h"
#include "../Core/SplinePath.h"

class TileEditor;
struct Settings;

struct LevelBlock {
	SplinePath xPoses;
	SplinePath yPoses;
	void DrawLines(const FPoint2D &worldPos, float scale);
	void AddPoint(float x, float y);
	int SearchNearest(float x, float y);
	bool CreateDot(float x, float y);
};

struct CurrentBlock {
	LevelBlock *block;
	int dotIndex;
	float downX;
	float downY;
	SplinePath splineX;
	SplinePath splineY;
	bool moveAllDots;
};

typedef std::vector<LevelBlock *> LevelBlocks;

struct LevelSet {
	LevelBlocks ground;
	LevelBlocks background;
	LevelBlocks surpris;
	LevelBlocks movable;
};

// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class TileEditorDestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint);

	TileEditor* test;
};

// класс описывающий физическую сцену и ее отображение
class TileEditor 
	: public b2ContactListener
	, public InputSystem
	, public Messager
{
public:
	
	TileEditor(TiXmlElement *xe);
	virtual ~TileEditor();

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

	friend class TileEditorDestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	b2Body* m_groundBody;
	b2AABB m_worldAABB;
	ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;
	TileEditorDestructionListener m_destructionListener;
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


	LevelSet _level;
	CurrentBlock _currents;


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

#endif//MYENGINE_TILEEDITOR_H
