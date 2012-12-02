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
#include "../2dga_api/Animation.h"
#include "Byker.h"
#include "LevelSet.h"
#include "Beauty.h"

class BykerGame;
struct Settings;
struct SelectedElement {
	enum {
		none,
		beauty_element,
		start_flag,
		end_flag
	} selected;
	int index;
};



// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
//class TileEditorDestructionListener : public b2DestructionListener
//{
//public:
//	void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
//	void SayGoodbye(b2Joint* joint);
//
//	BykerGame* test;
//};

// класс описывающий физическую сцену и ее отображение
class BykerGame 
	: public b2ContactListener
	, public InputSystem
	, public Messager
{
public:
	
	BykerGame(TiXmlElement *xe);
	virtual ~BykerGame();

	//virtual void Step(Settings* settings);

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
	void AddElement(const std::string &typeId);
	//virtual b2Body * AddElement(const BodyState &bodyState);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	virtual bool OnKey(int key);
	void OnMouseMove(const FPoint2D &mousePos);
	//virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

	// Callbacks for derived classes.
	//virtual void BeginContact(b2Contact* contact) { B2_NOT_USED(contact); }
	//virtual void EndContact(b2Contact* contact) { B2_NOT_USED(contact); }
	//virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	//virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual bool OnMouseWheel(int direction);
	virtual void Draw();
	virtual void Update(float deltaTime);

	void LoadTemplates(const std::string &filename);

	typedef std::vector<BeautyBase *> Collection;
	//BodyTemplate *_fish;
	//BodyTemplate *_cat;
	Collection _collection;

protected:
	
	/*typedef std::list<b2Body *> Bodyes;
	Bodyes _bodyes;*/

	//friend class TileEditorDestructionListener;
	//friend class BoundaryListener;
	//friend class ContactListener;

	//b2Body* m_groundBody;
	//b2AABB m_worldAABB;
	//ContactPoint m_points[k_maxContactPoints];
	//int32 m_pointCount;
	//TileEditorDestructionListener m_destructionListener;
	//int32 m_textLine;
	//b2World* m_world;
	//b2Body* m_bomb;
	//b2MouseJoint* m_mouseJoint;
	//b2Vec2 m_bombSpawnPoint;
	//bool m_bombSpawning;
	//int32 m_stepCount;

	FPoint2D _mouseWorld;
	bool _mouseDown;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	const float SLIDER_SCALE;
	const float SLIDER_MIN;

	std::string _saveLevelName;
	TiXmlElement *_saveLevelXml;

	FPoint2D _lastMousePos;
	Settings settings;

	float _viewScale; // масштаб всей Сцены
	
	FPoint2D _worldOffset; // смещение всех точек мира для отображения их на экране
	FPoint2D _screenOffset; // точка на экране в которую проецируется точка (0, 0) мира(в режиме редактирования она фактически равна центру экрана)

	float _angleMultiplier;
	bool _editor;

	int _finish;// ==0 - закончились и бомбы и синие; &1 - еще есть динамит; &2 - еще есть синие
	bool CanLevelStart();
	bool IsLevelFinish();
	void Explosion(b2Vec2 pos, float radius, float maxForce);
	virtual void OnMessage(const std::string &message);

	typedef std::list<BodyState> BodyStates;
	BodyStates _state;
	TiXmlDocument _doc;
	std::string _currentLevel;
	void EraseSelected();
	void EraseAllBodyes();
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


	LevelSet _level;
	std::vector<LevelSet *> _randomLevelsSet;
	void LoadRandomLevelSet(const std::string &filename);
	CurrentBlock _currents;
	SelectedElement _currentElement;
	void ClearLevel();
	void LoadLevel(const std::string &msg);
	HTEXTURE _flags;
	Sprite *_startFlag;
	Sprite *_endFlag;
	Byker *_byker;
	typedef std::list<b2Body *> LandBodies;
	LandBodies _landBodies;
	FPoint2D _endPoint;
	FPoint2D _startPoint;
	struct Island {
		std::vector<Lines *> _lines;// для физики
		LevelSet *_set;// для отрисовки только
	};
	typedef std::list<Island> Islands;
	Islands _islands;
	float _bykerAngle;
	void SetupBox2D();
	FPoint2D ScreenToWorld(const FPoint2D &screenPos);
	FPoint2D WorldToScreen(const FPoint2D &worldPos);
	float _scipScaleChanging;
	void LoadRandomLevelsSet(const std::string &fileName);
	bool _useRandom;
};

#endif//MYENGINE_TILEEDITOR_H
