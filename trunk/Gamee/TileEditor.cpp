#include "TileEditor.h"
#include "../Helpers/MyMessageBox.h"
#include "../Core/Core.h"
#include "../Core/Render.h"
#include "../Core/Math.h"
#include <cstdio>
#define LEVELS_FILE "levels.xml"
#define DEC 0.01f
#define HALFBORDER 0.0025f

#define SCALE_BOX2D 60

Texture *_allElements;


int TileEditor::round(float a) {
	int b = static_cast<int>(a);
	return (a - b) >= 0.5f ? b + 1 : b;
} 

void TileEditorDestructionListener::SayGoodbye(b2Joint* joint)
{
	if (test->m_mouseJoint == joint) {
		test->m_mouseJoint = NULL;
	} else {
		test->JointDestroyed(joint);
	}
}

void TileEditor::LoadTemplates(const std::string &filename) {
	TiXmlDocument doc(Render::GetDC()->Resource_MakePath(filename.c_str()));
	if (!doc.LoadFile()) {
		LOG("File not found : " + Render::GetDC()->Resource_MakePath(filename.c_str()));
		return;
	}
	TiXmlElement *bodyDef = doc.RootElement()->FirstChildElement("b2object");
	while (bodyDef) {
		_collection.push_back(new BodyTemplate(bodyDef));
		bodyDef = bodyDef->NextSiblingElement("b2object");
	}
	_collection.push_back(_fish = new BodyTemplate(doc.RootElement()->FirstChildElement("fish")));
	_collection.push_back(_cat = new BodyTemplate(doc.RootElement()->FirstChildElement("cat")));
}

TileEditor::TileEditor(TiXmlElement *xe)
	: _viewScale(1.f)
	, _userLevelWin(false)
	, _worldCenter((SCREEN_WIDTH = Render::GetDC()->System_GetState(HGE_SCREENWIDTH)) / 2
	, (SCREEN_HEIGHT = Render::GetDC()->System_GetState(HGE_SCREENHEIGHT)) / 2)
//	, _angleMultiplier(BodyTemplate::MAX / (M_PI * 2))
	, Messager("tile_editor")
	, _editor(true)
	, _selectedBody(NULL)
	, _signal(0.f)
	, _currentLevel("")
	, _waitState(WaitNone)
	, _mouseDown(false)
	, _waitYesNoNewLevel(false)
	, _waitYesNoDelSelected(false)
	, _waitYesNoOverwrite(false)
	, _waitAddNewElem(false)
	, SLIDER_SCALE(1.2f)
	, SLIDER_MIN(0.2f)
	, _netVisible(true)
	, _waitForImage(false)
{
	My::AnimationManager::Load(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + "data\\byker\\ride.xml").c_str()));
	_byker = new Byker();
	_flags = Render::GetDC()->Texture_Load(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + "data\\flagspoint.png").c_str()));
	_startFlag = new Sprite(_flags, 0, 0, 64, 64);
	_endFlag = new Sprite(_flags, 63, 0, 64, 64);

	_currentElement.selected = SelectedElement::none;
	_currents.dotIndex = -1;
	_currents.moveAllDots = false;
	if (!_doc.LoadFile(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + LEVELS_FILE).c_str()))) {
		//OkMessageShow();
		LOG("Error! Levels file not found! " + Render::GetDC()->Resource_MakePath((Render::GetDataDir() + LEVELS_FILE).c_str()));
	} 
	_selectedUV[0].u = 0.f; _selectedUV[0].v = 0.f;
	_selectedUV[1].u = 0.f; _selectedUV[1].v = 0.f;
	_selectedUV[2].u = 0.f; _selectedUV[2].v = 0.f;
	_selectedUV[3].u = 0.f; _selectedUV[3].v = 0.f;

	_allElements = Core::getTexture("allElements");
	LoadTemplates(Render::GetDataDir() + "bodyes.xml");

	b2Vec2 gravity;
	gravity.Set(0.0f, 10.0f);
	bool doSleep = true;
	m_world = new b2World(gravity, doSleep);
	m_bomb = NULL;
	m_textLine = 30;
	m_mouseJoint = NULL;
	m_pointCount = 0;

	m_destructionListener.test = this;
	m_world->SetDestructionListener(&m_destructionListener);
	m_world->SetContactListener(this);
	
	m_bombSpawning = false;
	m_stepCount = 0;
}

TileEditor::~TileEditor()
{
	delete _byker;
	My::AnimationManager::UnloadAll();
	delete _startFlag;
	delete _endFlag;
	Render::GetDC()->Texture_Free(_flags);
	ClearLevel();
	EraseAllBodyes();
	// By deleting the world, we delete the bomb, mouse joint, etc.
	delete m_world;
	m_world = NULL;
	for (Collection::iterator i = _collection.begin(), e = _collection.end(); i != e; i++) {
		delete (*i);
	}
}

void TileEditor::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	const b2Manifold* manifold = contact->GetManifold();

	if (manifold->pointCount == 0) {
		return;
	}

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
	b2GetPointStates(state1, state2, oldManifold, manifold);

	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
	{
		ContactPoint* cp = m_points + m_pointCount;
		cp->fixtureA = fixtureA;
		cp->fixtureB = fixtureB;
		cp->position = worldManifold.points[i];
		cp->normal = worldManifold.normal;
		cp->state = state2[i];
		++m_pointCount;
	}
}

void TileEditor::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	return;

	if (_startLevel.Action()) {
		return;
	}
	BodyTemplate *btA = static_cast<MyBody *>(contact->GetFixtureA()->GetBody()->GetUserData())->base;
	BodyTemplate *btB = static_cast<MyBody *>(contact->GetFixtureB()->GetBody()->GetUserData())->base;
	if (!(btA->_breakable) && !(btB->_breakable))
	{
		if ((btA->_hero && btB->_hero)) {
			_userLevelWin = true;
		}
		return;
	}

	// Should the body break?
	int32 count = contact->GetManifold()->pointCount;

	float32 maxImpulse = 0.0f;
	for (int32 i = 0; i < count; ++i)
	{
		maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
	}

	if (maxImpulse > 0.1f) {
		// Flag the body for breaking.
		if (btA->_breakable) {
			static_cast<MyBody *>(contact->GetFixtureA()->GetBody()->GetUserData())->broken = true;
		}
		if (btB->_breakable) {
			static_cast<MyBody *>(contact->GetFixtureB()->GetBody()->GetUserData())->broken = true;
		}
	}
}

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside) {
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}
		// Continue the query.
		return true;
	}
	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

void TileEditor::EraseBody(b2Body *body) {
	if (body->GetUserData() != NULL && body->GetUserData() != _byker) {
		MyBody *t = static_cast<MyBody *>(body->GetUserData());
		delete t;
		body->SetUserData(NULL);
	}
	m_world->DestroyBody(body);
}

void TileEditor::EraseAllBodyes() {
	for (b2Body *body = m_world->GetBodyList(); body; ) {
		b2Body *remove = body;
		body = body->GetNext();
		if (remove) {
			EraseBody(remove);
		}
	}
	_prevLandBody.clear();
	_currentLandBody.clear();
}

// добавляем новый элемент в "случайное" место на экране
b2Body * TileEditor::AddElement(const std::string &typeId) {
	Collection::iterator index = _collection.begin();
	while (index != _collection.end() && (*index)->_id != typeId ) {index++;}
	assert(index != _collection.end() && (*index)->_id == typeId );

	BodyState bs;
	bs.base = (*index);	
	bs.angle = 0;
	{// найдём центр экрана 

	}
	bs.pos = b2Vec2((SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale, (_worldCenter.y - SCREEN_HEIGHT / 2) / _viewScale);
	bs.pos = b2Vec2(_level.startpoint[0].x / SCALE_BOX2D, _level.startpoint[0].y / SCALE_BOX2D);
	bs.width = bs.base->_width;
	bs.height = bs.base->_height;
	bs.radius = bs.base->_radius;
	
	return AddElement(bs);
}

// ДЛЯ ИГРЫ тут можно соптимизировать - все элементы "земли" вынести в одно body
// создаем физическое тело по описанию
b2Body * TileEditor::AddElement(const BodyState &bodyState){ 

	b2BodyDef bd;
	bd.type = b2_dynamicBody;  
	//if (bodyState.base->_fixed) {
		//bd.type = b2_staticBody;);// ХАК - QueryAABB не работает со статичными объктами, 
	//}								// т.е. иначе их нельзя будет двигать в редакторе
									// поэтому в статик они переключаются только в режиме игры,
									// а в редакторе псевдо-динамик
	bd.position.Set(bodyState.pos.x, bodyState.pos.y);
	bd.angle = bodyState.angle;
	
	b2Body* body = m_world->CreateBody(&bd);
	
	b2FixtureDef fd;
	fd.restitution = bodyState.base->_restitution;
	fd.friction = bodyState.base->_friction;
	fd.density = 1.0f;

	if (bodyState.base->_shape == "circle") {
		b2CircleShape shape;
		shape.m_radius = bodyState.radius + HALFBORDER;
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else if (bodyState.base->_shape == "box") {
		b2PolygonShape shape;
		shape.SetAsBox(bodyState.width / 2.f - DEC + HALFBORDER, bodyState.height / 2.f - DEC + HALFBORDER);
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else if (bodyState.base->_shape == "triangle") {
		b2PolygonShape shape;
		b2Vec2 vec[3];
		vec[0] = b2Vec2( bodyState.width / 2.f - DEC + HALFBORDER, bodyState.height / 2.f - DEC + HALFBORDER);
		vec[1] = b2Vec2(-bodyState.width / 2.f + DEC - HALFBORDER, bodyState.height / 2.f - DEC + HALFBORDER);
		vec[2] = b2Vec2(-bodyState.width / 2.f + DEC - HALFBORDER, -bodyState.height / 2.f + DEC - HALFBORDER);
		shape.Set(vec, 3);
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else {
		EraseBody(body);
		return NULL;
	}
	MyBody *myBody = new MyBody();
	myBody->base = bodyState.base;
	myBody->broken = false;

	myBody->width = bodyState.width;		
	myBody->height = bodyState.height;
	myBody->radius = bodyState.radius;		

	body->SetUserData(myBody);
	return body;
}

void TileEditor::OnMouseDown(const FPoint2D &mousePos)
{	_mouseDown = true;
	_lastMousePos = mousePos;
	InitParams(NULL);
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	_currentElement.selected = SelectedElement::none;
	
	if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT)) {
		bool found = false;
		for (unsigned int i = 0; i < _level.ground.size() && !found; ++i) {
			_currents.dotIndex = _level.ground[i]->SearchNearest(fp.x, fp.y);
			_currents.moveAllDots = Render::GetDC()->Input_GetKeyState(HGEK_ALT);
			_currents.block = _level.ground[i];
			_currents.splineX = _level.ground[i]->xPoses;
			_currents.splineY = _level.ground[i]->yPoses;
			_currents.downX = fp.x;
			_currents.downY = fp.y;
			found = _currents.dotIndex >= 0;
		}
	} else if (Render::GetDC()->Input_GetKeyState(HGEK_CTRL)) {
		bool found = false;
		for (unsigned int i = 0; i < _level.ground.size() && !found; ++i) {
			found = _level.ground[i]->CreateDot(fp.x, fp.y);
		}
	} else if (Render::GetDC()->Input_GetKeyState(HGEK_D)) {
		bool found = false;
		for (unsigned int i = 0; i < _level.ground.size() && !found; ++i) {
			int index = _level.ground[i]->SearchNearest(fp.x, fp.y);
			if (index >= 0) {
				found = true;
				_level.ground[i]->RemoveDot(index);
			}
		}
	} else {
		for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
			if (_level.beauties[i].sprite->HasPixel(mousePos.x, mousePos.y)) {
				_currentElement.selected = SelectedElement::beauty_element;
				_currentElement.index = i;
				break;
			}
		}
		if (_currentElement.selected == SelectedElement::none) {
			if (_level.startpoint.size() && _startFlag->HasPixel(mousePos.x, mousePos.y)) {
				_currentElement.selected = SelectedElement::start_flag;
			} else if (_level.endpoint.size() && _endFlag->HasPixel(mousePos.x, mousePos.y)) {
				_currentElement.selected = SelectedElement::end_flag;
			}
		} else if (_currentElement.selected = SelectedElement::beauty_element) {
		}
	}

	//b2Vec2 p(fp.x, - fp.y);// нужен для выбора объекта по которому кликнули
	//
	//if (m_mouseJoint != NULL)
	//{
	//	return;
	//}

	//// Make a small box.
	//b2AABB aabb;
	//b2Vec2 d;
	//d.Set(0.001f, 0.001f);
	//aabb.lowerBound = p - d;
	//aabb.upperBound = p + d;

	//// Query the world for overlapping shapes.
	//QueryCallback callback(p);
	//m_world->QueryAABB(&callback, aabb);

	//if (callback.m_fixture)
	//{
	//	b2Body* body = callback.m_fixture->GetBody();
	//	//BodyTemplate *bt = static_cast<MyBody *>(body->GetUserData())->base;
	//	if (_editor) {
	//		InitParams(body);
	//		return;
	//	}
	//}
}

void TileEditor::OnMouseUp()
{
	_mouseDown = false;
	_currents.dotIndex = -1;
	_currents.moveAllDots = false;
	if (m_mouseJoint) {
		m_mouseJoint->GetBodyB()->ResetMassData();
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}
	if (_editor){
		return;
	}
	FPoint2D fp = 1.f / _viewScale * (_lastMousePos - _worldCenter);
	b2Vec2 p(fp.x, - fp.y);// нужен для выбора объекта по которому кликнули
	
	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	m_world->QueryAABB(&callback, aabb);

	if (callback.m_fixture)	{
		b2Body* body = callback.m_fixture->GetBody();
		BodyTemplate *bt = static_cast<MyBody *>(body->GetUserData())->base;
		if (bt->_explosion) {
			b2Vec2 pos = body->GetPosition();
			EraseBody(body);
			Explosion(pos, bt->_explosionRadius, bt->_maxForce);
		} else if (bt->_destroyOnTap) {
			static_cast<MyBody *>(body->GetUserData())->broken = true;
		}
	}
}

void TileEditor::InitParams(b2Body *body) 
{
	if (body == NULL) {
		_selectedBody = NULL;
		SendMessage("radio", "clear");
		SetValueB("sliderpanel", "visible", false);
		return;
	} 
	if (body != NULL && body != _selectedBody) {
		_selectedBody = body;
		const BodyTemplate *bt = static_cast<MyBody *>(body->GetUserData())->base;

		SendMessage("radio", "clear");
		SendMessage("radio", "add del");
		SendMessage("radio", "add angle");
		if (bt->_shape == "circle") {
			SendMessage("radio", "add radius");
		} else {
			SendMessage("radio", "add width");
			SendMessage("radio", "add height");
		}
		SetValueB("sliderpanel", "visible", true);
		// set up angle to normal
		float angle = body->GetAngle();
		while (angle > 0.f) {angle -= (2 * M_PI);} 
		while (angle < 0.f) {angle += (2 * M_PI);} 
		SetValueF("slider", "", angle / (2 * M_PI) );

	} else if (body != NULL && body == _selectedBody) {
		const MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
		int radio = static_cast<int>(GetNumberValue("radio", ""));
		if (radio == 0) {
			AskMessageShow("Are you sure?\nDelete selected object?");
			_waitYesNoDelSelected = true;
		} else if (radio == 1) {
			// set up angle to normal
			float angle = body->GetAngle();
			while (angle > 0.f) {angle -= (2 * M_PI);} 
			while (angle < 0.f) {angle += (2 * M_PI);} 
			SetValueF("slider", "", angle / (2 * M_PI) );
		} else if (radio == 2) {
			// set up width
			// ... or set up radius - HACK
			float width = (myBody->width - SLIDER_MIN) / SLIDER_SCALE;
			SetValueF("slider", "", width );
		} else if (radio == 3) {
			assert(myBody->base->_shape != "circle");
			// set up height
			float height = (myBody->height - SLIDER_MIN) / SLIDER_SCALE;
			SetValueF("slider", "", height );
		}
	}
}

bool TileEditor::CanLevelStart() {
	return _level.endpoint.size() > 0 && _level.startpoint.size() > 0;
}

bool TileEditor::IsLevelFinish() {
	return _userLevelWin;//_finish != 0x3;
}


void TileEditor::OnMouseMove(const FPoint2D &mousePos)
{
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(fp.x, - fp.y);// нужен для выбора объекта по которому кликнули

	if (_currentElement.selected != SelectedElement::none && _mouseDown) {
		if (_currentElement.selected == SelectedElement::beauty_element) {
			_level.beauties[_currentElement.index].pos.x += (p.x - m_mouseWorld.x);
			_level.beauties[_currentElement.index].pos.y -= (p.y - m_mouseWorld.y);
		} else if (_currentElement.selected == SelectedElement::start_flag) {
			_level.startpoint[0].x += (p.x - m_mouseWorld.x);
			_level.startpoint[0].y -= (p.y - m_mouseWorld.y);
		} else if (_currentElement.selected == SelectedElement::end_flag) {
			_level.endpoint[0].x += (p.x - m_mouseWorld.x);
			_level.endpoint[0].y -= (p.y - m_mouseWorld.y);
		}
	} else if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT) && _currents.dotIndex >= 0) {
		SplinePath tmpx, tmpy;
		float dx = p.x - _currents.downX;
		float dy = - (p.y + _currents.downY);
		for (unsigned int i = 0; i < _currents.block->xPoses.keys.size() - 1; ++i) {
			if (i == _currents.dotIndex || _currents.moveAllDots) {
				tmpx.addKey(_currents.splineX.getFrame(i, 0.f) + dx);
				tmpy.addKey(_currents.splineY.getFrame(i, 0.f) + dy);
			} else {
				tmpx.addKey(_currents.splineX.getFrame(i, 0.f));
				tmpy.addKey(_currents.splineY.getFrame(i, 0.f));
			}
		}
		tmpx.CalculateGradient(true);
		tmpy.CalculateGradient(true);
		_currents.block->xPoses = tmpx;
		_currents.block->yPoses = tmpy;
	} else {
		if (m_mouseJoint) {
			m_mouseJoint->SetTarget(p);
		} else if (_editor && _mouseDown && _selectedBody) {
			b2Vec2 pos = _selectedBody->GetPosition();
			pos += 1.f / _viewScale * b2Vec2(mousePos.x - _lastMousePos.x, _lastMousePos.y - mousePos.y);
			if (_netVisible) {
				pos = p;
				pos.x = round(pos.x / 0.1f) * 0.1f;
				pos.y = round(pos.y / 0.1f) * 0.1f;
			}
			_selectedBody->SetTransform(pos, _selectedBody->GetAngle());
		} else if (_mouseDown) {
			_worldCenter += (mousePos - _lastMousePos);
		}
	}
	_lastMousePos = mousePos;
	m_mouseWorld = p;
}

bool TileEditor::OnMouseWheel(int direction) {
	float old = _viewScale;
	FPoint2D fp = 1.f / _viewScale * (_lastMousePos - _worldCenter);
	if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT) && _currentElement.selected != SelectedElement::none) {
		if (_currentElement.selected == SelectedElement::beauty_element) {
			_level.beauties[_currentElement.index].angle += direction * 3;
		}
	} else if (direction > 0 && _viewScale < 4.f) {
		_viewScale *= 1.09f * direction;
	} else if (direction < 0 && _viewScale > 1.f / 8.f) {
		_viewScale *= 0.9f * abs(direction);
	}
	_viewScale = min(4.f, max(1.f / 8.f, _viewScale));
	_worldCenter = - (_viewScale / old * (_lastMousePos - _worldCenter) - _lastMousePos);
	return true;
} 

void TileEditor::Step(Settings* settings)
{
	float32 timeStep = settings->hz > 0.0f ? 1.0f / settings->hz : float32(0.0f);

	if (settings->pause) {
		if (settings->singleStep) {
			settings->singleStep = 0;
		} else {
			timeStep = 0.0f;
		}
		m_textLine += 15;
	}

	uint32 flags = 0;
	flags += settings->drawShapes			* b2DebugDraw::e_shapeBit;
	flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	////m_debugDraw.SetFlags(flags);

	m_world->SetWarmStarting(settings->enableWarmStarting > 0);
	m_world->SetContinuousPhysics(settings->enableContinuous > 0);

	m_pointCount = 0;

	m_world->Step(timeStep, settings->velocityIterations, settings->positionIterations);

	if (timeStep > 0.0f) {
		++m_stepCount;
	}

	if (m_mouseJoint) {
	}
}

bool TileEditor::IsMouseOver(const FPoint2D &mousePos) {
	return true;
}

inline void TileEditor::DrawElement(Vertex *&buf, const UV *uv, const b2Vec2 &pos, const FPoint2D *angles) {
	float x =   _viewScale * pos.x + _worldCenter.x;
	float y = - _viewScale * pos.y + _worldCenter.y;
	buf[0].x = x + _viewScale * angles[0].x; buf[0].y = y + _viewScale * angles[0].y; 
	buf[1].x = x + _viewScale * angles[1].x; buf[1].y = y + _viewScale * angles[1].y; 
	buf[2].x = x + _viewScale * angles[2].x; buf[2].y = y + _viewScale * angles[2].y; 
	buf[3].x = x + _viewScale * angles[3].x; buf[3].y = y + _viewScale * angles[3].y; 

	buf[0].tx = uv[0].u; buf[0].ty = uv[0].v; buf[0].col = 0xFFFFFFFF;
	buf[1].tx = uv[1].u; buf[1].ty = uv[1].v; buf[1].col = 0xFFFFFFFF;
	buf[2].tx = uv[2].u; buf[2].ty = uv[2].v; buf[2].col = 0xFFFFFFFF;
	buf[3].tx = uv[3].u; buf[3].ty = uv[3].v; buf[3].col = 0xFFFFFFFF;
	buf += 4;
}

/// DrawLine
inline void TileEditor::DrawLine(const b2Vec2 &a, const b2Vec2 &b, DWORD color)
{
	Render::Line(_viewScale * a.x + _worldCenter.x, - _viewScale * a.y + _worldCenter.y, _viewScale * b.x + _worldCenter.x, - _viewScale * b.y + _worldCenter.y, color);
}

void TileEditor::Draw() {
	bool oldTnt = (_finish & 0x1) != 0;
	bool oldBlue = (_finish & 0x2) != 0;
	bool tnt = false;
	bool blue = false;
	_finish = 0x0;
	int max = 1;
	Vertex *buffer;
	if (_netVisible) {
		for (unsigned int i = 0; i < _level.images.size(); ++i) {
			_level.images[i].sprite->RenderEx(_level.images[i].pos.x * _viewScale + _worldCenter.x, _level.images[i].pos.y * _viewScale + _worldCenter.y, 0.f, _viewScale);// DrawLines(_worldCenter, _viewScale);
		}
		float STEP = 64.f;
		int n = SCREEN_WIDTH / (_viewScale * STEP);
		float t = _worldCenter.x / (STEP * _viewScale);
		t = (t - static_cast<int>(t)) * (STEP * _viewScale);
		for (int i = 0; i <= n; i++) {
			float x = i * STEP * _viewScale + t;
			Render::GetDC()->Gfx_RenderLine(x, 0, x, SCREEN_HEIGHT, 0x4FFFFFFF);
		}
		n = SCREEN_HEIGHT / (_viewScale * STEP) + 1;
		t = _worldCenter.y / (STEP * _viewScale);
		t = (t - static_cast<int>(t)) * (STEP * _viewScale);
		for (int i = 0; i <= n; i++) {
			float y = i * STEP * _viewScale + t;
			Render::GetDC()->Gfx_RenderLine(0, y, SCREEN_WIDTH, y, 0x4FFFFFFF);
		}
		for (unsigned int i = 0; i < _level.ground.size(); ++i) {
			_level.ground[i]->DrawLines(_worldCenter, _viewScale);
		}
	} else {
		for (unsigned int i = 0; i < _level.ground.size(); ++i) {
			_level.ground[i]->DrawTriangles(_worldCenter, _viewScale);
		}
	}
	Render::PushMatrix();
	Render::MatrixMove(_worldCenter.x, _worldCenter.y);
	Render::MatrixScale(_viewScale, _viewScale);
	for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
		bool changeColor = false;
		if (_currentElement.selected == SelectedElement::beauty_element && _currentElement.index == i) {
			DWORD f = 0x7F + 0x7F * sin(M_PI * _signal);
			Render::SetColor(0xFF000000 | f << 16 | f << 8 | f);
			changeColor = true;
		}
		_level.beauties[i].Draw();
		if (changeColor) {
			Render::SetColor(0xFFFFFFFF);
		}
	}
	if (_level.startpoint.size()) {
		bool changeColor = false;
		if (_currentElement.selected == SelectedElement::start_flag) {
			DWORD f = 0x7F + 0x7F * sin(M_PI * _signal);
			Render::SetColor(0xFF000000 | f << 16 | f << 8 | f);
			changeColor = true;
		}
		_startFlag->Render(_level.startpoint[0].x, _level.startpoint[0].y - _startFlag->Height());
		if (changeColor) {
			Render::SetColor(0xFFFFFFFF);
		}
	}
	if (_level.endpoint.size()) {
		bool changeColor = false;
		if (_currentElement.selected == SelectedElement::end_flag) {
			DWORD f = 0x7F + 0x7F * sin(M_PI * _signal);
			Render::SetColor(0xFF000000 | f << 16 | f << 8 | f);
			changeColor = true;
		}
		_endFlag->Render(_level.endpoint[0].x, _level.endpoint[0].y - _endFlag->Height());
		if (changeColor) {
			Render::SetColor(0xFFFFFFFF);
		}
	}

	FPoint2D pselect[4];
	typedef std::list<b2Body *> Remove;
	Remove remove;

	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		if (body->GetUserData() == _byker) {
			if (!_editor) {
				FPoint2D p;
				const b2Transform & xf = body->GetTransform();
				p.x = xf.position.x * SCALE_BOX2D;
				p.y = xf.position.y * SCALE_BOX2D;
				_byker->SetPos(p);
				_byker->Draw();
				if (Render::GetDC()->Input_GetKeyState(HGEK_SPACE)) {
					body->SetAngularVelocity(40);
				} else {
					body->SetAngularVelocity(10);
				}
				char buff[20];
				sprintf(buff, "ANGLE %f, %i",  xf.GetAngle(), (body->IsFixedRotation() ? 1 : 0));
				Render::PrintString(100, 50, "", buff);
				b2Fixture *fixture = body->GetFixtureList();
				while (fixture) {
					b2Shape *shape = fixture->GetShape();
					fixture = fixture->GetNext();
				}
			}
		} else if (body->GetUserData() == NULL) {
			assert(body->GetType() == b2_staticBody);
			b2Fixture *fixture = body->GetFixtureList();
			while (fixture) {
				b2Shape *shape = fixture->GetShape();
				assert(shape->GetType() == b2Shape::e_polygon);
				b2PolygonShape *polygon = (b2PolygonShape *)shape;

				Render::Line(polygon->GetVertex(0).x * SCALE_BOX2D, polygon->GetVertex(0).y * SCALE_BOX2D, polygon->GetVertex(1).x * SCALE_BOX2D, polygon->GetVertex(1).y * SCALE_BOX2D, 0xFFFFFFFF);
				Render::Line(polygon->GetVertex(2).x * SCALE_BOX2D, polygon->GetVertex(2).y * SCALE_BOX2D, polygon->GetVertex(1).x * SCALE_BOX2D, polygon->GetVertex(1).y * SCALE_BOX2D, 0xFFFFFFFF);
				Render::Line(polygon->GetVertex(0).x * SCALE_BOX2D, polygon->GetVertex(0).y * SCALE_BOX2D, polygon->GetVertex(2).x * SCALE_BOX2D, polygon->GetVertex(2).y * SCALE_BOX2D, 0xFFFFFFFF);

				fixture = fixture->GetNext();
			}
		}
	}

	//buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
	unsigned int counter = 0;
	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		if (body->GetUserData() != NULL && body->GetUserData() != _byker) {
			const MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
			if (myBody->broken) {
				remove.push_back(body);
			}
			const BodyTemplate *bt = myBody->base;
			const b2Transform & xf = body->GetTransform();

			//FPoint2D p[4];
			//float width2 = myBody->width / 2;
			//float height2 = myBody->height / 2;
			//p[0].x = -width2; p[0].y =  height2;
			//p[1].x =  width2; p[1].y =  height2;
			//p[2].x =  width2; p[2].y = -height2;
			//p[3].x = -width2; p[3].y = -height2;

			//float angle(-xf.GetAngle());
			//p[0] = *p[0].Rotate(angle);
			//p[1] = *p[1].Rotate(angle);
			//p[2] = *p[2].Rotate(angle);
			//p[3] = *p[3].Rotate(angle);

			FPoint2D p;
			p.x = xf.position.x * SCALE_BOX2D;
			p.y = xf.position.y * SCALE_BOX2D;
			_byker->SetPos(p);
			_byker->Draw();
			//DrawElement(buffer, bt->_uv, xf.position, p);
			//if (_selectedBody == body) {
			//	for (unsigned int i = 0; i < 4; i++) { pselect[i] = p[i]; }
			//}
			++counter;
			if (static_cast<int>(counter) > max) {
				assert(false);
			}
		}
	}
	//Render::GetDC()->Gfx_FinishBatch(counter);
	if (!_editor) {// смотрим не пора ли подставить новый кусок
		float x = _endPoint.x;
		float y = _endPoint.y;
		Render::GetCurrentMatrix().Mul(x, y);
		
		//if (x >= 0 && x <= 960 && y >=0 && y <= 640) {
		if (abs(_byker->_attachedBody->GetTransform().position.x * SCALE_BOX2D - _endPoint.x) < 100) {
			
			for (unsigned int i = 0; i < _prevLandBody.size(); ++i) {
				EraseBody(_prevLandBody[i]);
			}
			_prevLandBody = _currentLandBody;
			_currentLandBody.clear();
			
			FPoint2D shift = (_level.endpoint[0] - _level.startpoint[0]) + _oldShift;
			if (_needMoveToOrigin) {
				b2Vec2 b2shift;
				b2shift.x = -shift.x / SCALE_BOX2D;
				b2shift.y = -shift.y / SCALE_BOX2D;
				for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
					b2Vec2 p = body->GetTransform().position;
					body->SetTransform(p + b2shift, 0.f);
				}
				shift = FPoint2D(0.f, 0.f);
			}

			_endPoint = _level.endpoint[0] + shift;

			b2BodyDef bd;
			bd.type = b2_staticBody;  
			bd.position.Set(0.f, 0.f);
			bd.angle = 0.f;
			b2Body* body = m_world->CreateBody(&bd);		
			for (unsigned int i = 0; i < _level.ground.size(); ++i) {
				_level.ground[i]->CreateBody(body, shift);
			}
			body->ResetMassData();
			_currentLandBody.push_back(body);

			_oldShift = shift;
			_needMoveToOrigin = !_needMoveToOrigin;
		}
	}
	Render::PopMatrix();
	for (;remove.begin() != remove.end();) {
		EraseBody(*remove.begin());
		remove.erase(remove.begin());
	}
	if (_selectedBody && _signal > 0.5f) {
		max = 1;
		const BodyTemplate *bt = static_cast<MyBody *>(_selectedBody->GetUserData())->base;
		const b2Transform & xf = _selectedBody->GetTransform();

		Vertex *buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_ALPHAADD | BLEND_COLORADD, &max);

		DrawElement(buffer, bt->_uv, xf.position, pselect);
		Render::GetDC()->Gfx_FinishBatch(1);
	}
	char buff[10];
	Math::FloatToChar(_viewScale, buff);
	Render::PrintString(940, 0, "", buff);
}

void TileEditor::Update(float deltaTime) {	
	_signal += 2 * deltaTime;
	while (_signal > 1.f) {
		_signal -= 1.f;
	}
	if (_editor) {
		if (Render::GetDC()->Input_GetKeyState(HGEK_HOME)) {
			_viewScale = 1.f;
		}
		if (_selectedBody != NULL) {
			MyBody *myBody = static_cast<MyBody *>(_selectedBody->GetUserData());
			int radio = static_cast<int>(GetNumberValue("radio", ""));
			if (radio == 0) { // удаляем - ничего не делаем в Update
			} else if (radio == 1) { // angle
				float angle = GetNumberValue("slider", "");
				_selectedBody->SetTransform(_selectedBody->GetPosition(), angle * (2 * M_PI));
			} else if (radio == 2) { // width or radius
				float width = GetNumberValue("slider", "") * SLIDER_SCALE + SLIDER_MIN;
				if (myBody->base->_shape == "circle") {
					myBody->radius = width / 2;
					myBody->width = width;
					myBody->height = width;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_circle);
					static_cast<b2CircleShape *>(_selectedBody->GetFixtureList()->GetShape())->m_radius = myBody->radius + HALFBORDER;
				} else if (myBody->base->_shape == "box") {
					myBody->width = width;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_polygon);
					static_cast<b2PolygonShape *>(_selectedBody->GetFixtureList()->GetShape())->SetAsBox(myBody->width / 2.f - DEC + HALFBORDER, myBody->height / 2.f - DEC + HALFBORDER);
				} else if (myBody->base->_shape == "triangle") {
					myBody->width = width;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_polygon);
					b2Vec2 vec[3];
					vec[0] = b2Vec2( myBody->width / 2.f - DEC +  HALFBORDER, myBody->height / 2.f - DEC + HALFBORDER);
					vec[1] = b2Vec2(-myBody->width / 2.f + DEC - HALFBORDER, myBody->height / 2.f - DEC + HALFBORDER);
					vec[2] = b2Vec2(-myBody->width / 2.f + DEC - HALFBORDER, -myBody->height / 2.f + DEC - HALFBORDER);
					static_cast<b2PolygonShape *>(_selectedBody->GetFixtureList()->GetShape())->Set(vec, 3);
				} else {
					assert(false);
				}
				// конкретно мы в редакторе точно знаем - один Fixture и один Shape
				_selectedBody->ResetMassData();
			} else if (radio == 3) { // height
				float height = GetNumberValue("slider", "") * SLIDER_SCALE + SLIDER_MIN;
				if (myBody->base->_shape == "box") {
					myBody->height = height;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_polygon);
					static_cast<b2PolygonShape *>(_selectedBody->GetFixtureList()->GetShape())->SetAsBox(myBody->width / 2.f - DEC + HALFBORDER, myBody->height / 2.f - DEC + HALFBORDER);
				} else if (myBody->base->_shape == "triangle") {
					myBody->height = height;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_polygon);
					b2Vec2 vec[3];
					vec[0] = b2Vec2( myBody->width / 2.f - DEC + HALFBORDER, myBody->height / 2.f - DEC + HALFBORDER);
					vec[1] = b2Vec2(-myBody->width / 2.f - DEC + HALFBORDER, myBody->height / 2.f - DEC + HALFBORDER);
					vec[2] = b2Vec2(-myBody->width / 2.f + DEC - HALFBORDER, -myBody->height / 2.f + DEC - HALFBORDER);
					static_cast<b2PolygonShape *>(_selectedBody->GetFixtureList()->GetShape())->Set(vec, 3);
				} else {
					assert(false);
				}
			} else {
				assert(false);
			}
		}
		return;
	} else {
		_byker->Update(deltaTime);
		Step(&settings);
		{// двигаем камеру
			const b2Transform &xf = _byker->_attachedBody->GetTransform();
			_worldCenter.x = (-xf.position.x * SCALE_BOX2D * _viewScale + SCREEN_WIDTH / 2);
			_worldCenter.y = (-xf.position.y * SCALE_BOX2D * _viewScale + SCREEN_HEIGHT / 2);			
			//_worldCenter.x = (-xf.position.x * SCALE_BOX2D * _viewScale + SCREEN_WIDTH / 2 - SCREEN_WIDTH / 3);
			//_worldCenter.y = (-xf.position.y * SCALE_BOX2D * _viewScale + SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 6);			
		}
		if (_startLevel.Action()) {
			_startLevel.Update(deltaTime);
		}
		if (IsLevelFinish()) {
			if (_lastTimer.Action()) {
				_lastTimer.Update(deltaTime);
				if (!_lastTimer.Action()) {
					if (_finish & 0x2) { // остались синие
						OkMessageShow("Game over!\nYou lose!");
					} else {
						OkMessageShow("Congratulation!\nYou win!");
					}
					OnMessage("play");
				}
			} else {
				_lastTimer.Init(0.6f);
			}
		}
	}
}

void TileEditor::Explosion(b2Vec2 pos, float maxDistance, float maxForce)
{
	b2Vec2 b2TouchPosition = b2Vec2(pos.x, pos.y);
	b2Vec2 b2BodyPosition;
	float distance; 
	float strength;
	float force;
	float angle;
	for (b2Body* b = m_world->GetBodyList(); b; b = b->GetNext()) {
		b2BodyPosition = b2Vec2(b->GetPosition().x, b->GetPosition().y);
		distance = b2Distance(b2BodyPosition, b2TouchPosition);
		if(distance < maxDistance - 0.01) {
			strength = (maxDistance - distance) / maxDistance; 
			force = sqrtf(strength) * maxForce;
			angle = atan2f(b2BodyPosition.y - b2TouchPosition.y, b2BodyPosition.x - b2TouchPosition.x);
			// Apply an impulse to the body, using the angle
			b->ApplyLinearImpulse(b2Vec2(cosf(angle) * force, sinf(angle) * force), b->GetPosition());
		}
	}
}

void TileEditor::SaveState() {
	//_state.clear();
	//for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
	//	const b2Transform & xf = body->GetTransform();
	//	MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
	//	if (myBody->base->_fixed) {
	//		body->SetType(b2_staticBody);// ХАК - QueryAABB не работает со статичными объктами, 
	//									 // т.е. иначе их нельзя будет двигать в редакторе
	//	}
	//	BodyState s;
	//	s.base = myBody->base;
	//	s.angle = xf.GetAngle();
	//	s.pos = xf.position;
	//	s.width = myBody->width;
	//	s.height = myBody->height;
	//	s.radius = myBody->radius;
	//	_state.push_back(s);
	//}
}

void TileEditor::ResetState() {
	InitParams(NULL);
	_editor = true;
	if (_level.startpoint.size()) {
//		_worldCenter = _level.startpoint[0];
		_worldCenter.x = (_level.startpoint[0].x * _viewScale + SCREEN_WIDTH / 2);
		_worldCenter.y = (_level.startpoint[0].y * _viewScale + SCREEN_HEIGHT / 2);			
	}
	EraseAllBodyes();
	for (BodyStates::iterator i = _state.begin(), e = _state.end(); i != e; i++) {

		/*b2Body *b = */AddElement(*i);
		//b->SetTransform(_state[i].pos, _state[i].angle);
	}
}

void TileEditor::OnMessage(const std::string &message) {
	if (_waitYesNoNewLevel) {
		if (message == "yes") {
			InitParams(NULL);
			_currentLevel = "";
			_state.clear();
			EraseAllBodyes();
			_worldCenter = FPoint2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			_viewScale = 1.f;
			ClearLevel();
		}
		_waitYesNoNewLevel = false;
	} else if (_waitYesNoDelSelected) {
		EraseBody(_selectedBody);
		InitParams(NULL);
		_waitYesNoDelSelected = false;
	} else if (_waitYesNoOverwrite) {
		if (message == "yes") {
			SaveLevel(_saveLevelName);
		}
		_waitYesNoOverwrite = false;
	}
	std::string msg;
	if (message == "changes") {
		InitParams(_selectedBody);
	} else if (message == "net") {
		_netVisible = !_netVisible;
		if (!_netVisible) {
			for (unsigned int i = 0; i < _level.ground.size(); ++i) {
				_level.ground[i]->GenerateTriangles();
			}
		}
	} else if (message == "play") {
		if (_editor) { // переходим в режим игры
			if (CanLevelStart()) {
				//SetValueB("big", "visible", false);
				//SetValueB("small", "visible", true);
				SetValueS("play", "", "stop");
				_editor = false;
				//InitParams(NULL);
				SaveState();
				//_finish = 0x3;
				//_startLevel.Init(2.f);
				//_userLevelWin = false;
				_netVisible = false;
				for (unsigned int i = 0; i < _level.ground.size(); ++i) {
					_level.ground[i]->GenerateTriangles();
				}
				SetupBox2D();
			} else {
				OkMessageShow("Error!\nLevel must have START and END poses!");
			}
		} else { // в редактор
			SetValueS("play", "", "play");
			_netVisible = true;
			ResetState();
		}
	} else if (message == "add new elem") {
		//for (Collection::iterator i = _collection.begin(); i != _collection.end(); i++) {
		//	Messager::SendMessage("SmallList", "add " + (*i)->_id);
		//}
		Messager::SendMessage("SmallList", "add curv");
		Messager::SendMessage("SmallList", "add image");
		Messager::SendMessage("SmallList", "add beauty");
		Messager::SendMessage("SmallList", "add bonus");
		Messager::SendMessage("SmallList", "add transport");
		Messager::SendMessage("SmallList", "add animal");
		if (_level.startpoint.size() == 0) {
			Messager::SendMessage("SmallList", "add start pos");
		}
		if (_level.endpoint.size() == 0) {
			Messager::SendMessage("SmallList", "add end pos");
		}
		Messager::SendMessage("SmallList", "special add cancel");
		_waitAddNewElem = true;
	} else if (message == "left") {
	} else if (message == "right") {
	} else if (message == "up") {
	} else if (message == "down") {
	} else if (message == "new") {
		AskMessageShow("Are you sure?\nDelete all objects?");
		_waitYesNoNewLevel = true;
	} else if (message == "open") {
		TiXmlElement *xe = _doc.RootElement()->FirstChildElement();
		std::string s;
		while (xe) {			
			s = xe->Attribute("id");
			if (_currentLevel == s) {
				Messager::SendMessage("BigList", "special add " + s);
			} else {
				Messager::SendMessage("BigList", "add " + s);
			}
			xe = xe->NextSiblingElement();
		}
		Messager::SendMessage("BigList", "special add cancel");
		_waitState = WaitForLevelOpen;
	} else if (message == "save") {
		if (!_editor) {
			OnMessage("play");
		}
		TiXmlElement *xe = _doc.RootElement()->FirstChildElement();
		std::string s;
		while (xe) {
			s = xe->Attribute("id");
			if (_currentLevel == s) {
				Messager::SendMessage("BigList", "special add " + s);
			} else {
				Messager::SendMessage("BigList", "add " + s);
			}
			xe = xe->NextSiblingElement();
		}
		Messager::SendMessage("BigList", "special add cancel");
		Messager::SendMessage("BigList", "special add as new");
		_waitState = WaitForLevelSave;
	} else if (CanCut(message, "button pressed ", msg)) {
		if (_waitForImage) {
			_waitForImage = false;
			Messager::SendMessage("MiddleList", "clear");
			if (msg == "cancel") {
				return;
			}
			OneImage image;
			image.texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + "data\\images\\" + msg).c_str());
			image.sprite = new hgeSprite(image.texture, 0, 0, Render::GetDC()->Texture_GetWidth(image.texture), Render::GetDC()->Texture_GetHeight(image.texture));
			image.pos = FPoint2D(0.f, 0.f);
			image.filePath = msg;
			if (_level.images.size() > 0) {
				for (unsigned int i = 0; i < _level.images.size(); ++i) {
					delete _level.images[i].sprite;
					Render::GetDC()->Texture_Free(_level.images[i].texture);
				}
				_level.images.clear();
			}
			_level.images.push_back(image);
		} else if (_waitAddNewElem) {
			_waitAddNewElem = false;
			Messager::SendMessage("SmallList", "clear");
			if (msg == "cancel") {
				return;
			}
			//if (!_editor) {
			//	SetValueS("play", "", "play");
			//	ResetState();
			//}
			//InitParams(AddElement(msg));
			if (msg == "curv") {
				LevelBlock *b = new LevelBlock();
				float cx = (SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale;
				float cy = (SCREEN_HEIGHT / 2 - _worldCenter.y) / _viewScale;
				for (int i = 0; i < 4; ++i) {
					b->AddPoint(cx + 32.f * sin(M_PI / 2 * i), cy + 32.f * cos(M_PI / 2 * i));
				}
				_level.ground.push_back(b);
			} else if (msg == "image") {
				_waitForImage = true;
				{
					WIN32_FIND_DATA FindFileData;
					HANDLE hf;
					std::string s(Render::GetDC()->Resource_MakePath(Render::GetDataDir().c_str()));
					s += "data\\images\\*.jpg";
					hf = FindFirstFile(s.c_str(), &FindFileData);
					if (hf != INVALID_HANDLE_VALUE){
						do {
							//std::cout << FindFileData.cFileName << "\n";
							Messager::SendMessage("MiddleList", std::string("add ") + FindFileData.cFileName);
						}
						while (FindNextFile(hf,&FindFileData) != 0);
						FindClose(hf);
					}
				}
				{
					WIN32_FIND_DATA FindFileData;
					HANDLE hf;
					std::string s(Render::GetDC()->Resource_MakePath(Render::GetDataDir().c_str()));
					s += "data\\images\\*.png";
					hf = FindFirstFile(s.c_str(), &FindFileData);
					if (hf != INVALID_HANDLE_VALUE){
						do {
							//std::cout << FindFileData.cFileName << "\n";
							Messager::SendMessage("MiddleList", std::string("add ") + FindFileData.cFileName);
						}
						while (FindNextFile(hf,&FindFileData) != 0);
						FindClose(hf);
					}
				}
				Messager::SendMessage("MiddleList", "special add cancel");
			} else if (msg == "beauty") {
				Beauty b;
				b.filePath = "data\\beauty\\bush.png";
				b.texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + b.filePath).c_str());
				b.sprite = new Sprite(b.texture, 0, 0, Render::GetDC()->Texture_GetWidth(b.texture), Render::GetDC()->Texture_GetHeight(b.texture));
				b.mirror = false;
				b.scale = 1.f;
				b.angle = 0.f;
				float cx = (SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale;
				float cy = (SCREEN_HEIGHT / 2 - _worldCenter.y) / _viewScale;
				b.pos = FPoint2D(cx, cy);
				_level.beauties.push_back(b);
			} else if (msg == "start pos") {
				float cx = (SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale;
				float cy = (SCREEN_HEIGHT / 2 - _worldCenter.y) / _viewScale;				
				_level.startpoint.push_back(FPoint2D(cx, cy));
			} else if (msg == "end pos") {
				float cx = (SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale;
				float cy = (SCREEN_HEIGHT / 2 - _worldCenter.y) / _viewScale;				
				_level.endpoint.push_back(FPoint2D(cx, cy));
			} else if (msg == "bonus") {
				AddElement("rubber");
			}
		} else if (_waitState == WaitForLevelOpen) {
			_waitState = WaitNone;
			Messager::SendMessage("BigList", "clear");
			if (msg == "cancel") {
				return;
			}
			LoadLevel(msg);
		} else if (_waitState == WaitForLevelSave) {
			_waitState = WaitNone;
			Messager::SendMessage("BigList", "clear");
			if (msg == "cancel") {
				return;
			}
			TiXmlElement *xe = _doc.RootElement()->FirstChildElement();
			std::string s;
			while (xe != NULL && xe->Attribute("id") != msg) {			
				s = xe->Attribute("id");
				xe = xe->NextSiblingElement();
			}
			_saveLevelXml = xe;
			if (xe == NULL) {
				int save_as_new = atoi(s.c_str()) + 1;
				char buff[10];
				sprintf(buff, "%i", save_as_new);
				TiXmlElement *level = new TiXmlElement("level");
				level->SetAttribute("id", buff);
				_doc.RootElement()->LinkEndChild(level);
				xe = level; 
				_saveLevelXml = xe;
				msg = buff;
			} else if (_currentLevel != msg) {
				AskMessageShow("Are you sure?\nDo you want overwrite " + msg + "?");
				_waitYesNoOverwrite = true;
				_saveLevelName = msg;
				_saveLevelXml = xe;
				return;
			}
			SaveLevel(msg);
		} else {
			assert(false);
		}
	}
}

void TileEditor::SaveLevel(const std::string &levelName) {
	// level saving
	SaveState();
	TiXmlElement *elem = _saveLevelXml->FirstChildElement();
	while (elem != NULL) {
		TiXmlElement *remove = elem;
		elem = elem->NextSiblingElement();
		_saveLevelXml->RemoveChild(remove);
	}
	TiXmlElement *elemGround = new TiXmlElement("Ground");
	_saveLevelXml->LinkEndChild(elemGround);
	for (LevelBlocks::iterator i = _level.ground.begin(), e = _level.ground.end(); i != e; i++) {
		TiXmlElement *elem = new TiXmlElement("elem");
		for (int j = 0; j < (*i)->xPoses.keys.size() - 1; ++j) {
			TiXmlElement *dot = new TiXmlElement("dot");
			char s[16];
			sprintf(s, "%f", (*i)->xPoses.getFrame(j, 0.f)); dot->SetAttribute("x", s);
			sprintf(s, "%f", (*i)->yPoses.getFrame(j, 0.f)); dot->SetAttribute("y", s);
			elem->LinkEndChild(dot);
		}
		elemGround->LinkEndChild(elem);
	}

	TiXmlElement *imagesList = new TiXmlElement("Images");
	for (int i = 0; i < _level.images.size(); i++) {
		TiXmlElement *image = new TiXmlElement("image");
		image->SetAttribute("filePath", _level.images[i].filePath.c_str());
		imagesList->LinkEndChild(image);
	}
	_saveLevelXml->LinkEndChild(imagesList);

	TiXmlElement *beautyList = new TiXmlElement("Beauties");
	for (int i = 0; i < _level.beauties.size(); i++) {
		TiXmlElement *beauty = new TiXmlElement("beauty");
		_level.beauties[i].SaveToXml(beauty);
		beautyList->LinkEndChild(beauty);
	}
	_saveLevelXml->LinkEndChild(beautyList);

	TiXmlElement *word = new TiXmlElement("word");
	word->SetAttribute("x", _worldCenter.x);
	word->SetAttribute("y", _worldCenter.y);
	if (_level.startpoint.size()) {
		word->SetAttribute("startX", _level.startpoint[0].x);
		word->SetAttribute("startY", _level.startpoint[0].y);
	}
	if (_level.endpoint.size()) {
		word->SetAttribute("endX", _level.endpoint[0].x);
		word->SetAttribute("endY", _level.endpoint[0].y);
	}
	char s[16];
	sprintf(s, "%f", _viewScale);
	word->SetAttribute("scale", s);
	_saveLevelXml->LinkEndChild(word);

	_doc.SaveFile();
	_currentLevel = levelName;
}

void Beauty::SaveToXml(TiXmlElement *xe) {
	xe->SetAttribute("filePath", filePath.c_str());
	xe->SetAttribute("mirror", mirror ? "1" : "0");
	char s[16];
	sprintf(s, "%f", scale);
	xe->SetAttribute("scale", s);
	sprintf(s, "%f", angle);
	xe->SetAttribute("angle", s);
	sprintf(s, "%f", pos.x);
	xe->SetAttribute("x", s);
	sprintf(s, "%f", pos.y);
	xe->SetAttribute("y", s);
}

void Beauty::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(pos.x, pos.y);
	Render::MatrixRotate(angle / 180 * M_PI);
	if (mirror) {
		Render::MatrixScale(-1.f, 1.f);
	}
	sprite->Render(- sprite->Width() / 2, - sprite->Height());
	Render::PopMatrix();
}

void Beauty::LoadFromXml(TiXmlElement *xe) {
	filePath = xe->Attribute("filePath");
	mirror = atoi(xe->Attribute("mirror")) != 0;
	scale = atof(xe->Attribute("scale"));
	angle = atof(xe->Attribute("angle"));
	pos.x = atof(xe->Attribute("x"));
	pos.y = atof(xe->Attribute("y"));
	texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + filePath).c_str());
	sprite = new Sprite(texture, 0, 0, Render::GetDC()->Texture_GetWidth(texture), Render::GetDC()->Texture_GetHeight(texture));
}

void LevelBlock::DrawLines(const FPoint2D &worldPos, float scale) {
	float t = 0.f;
	float x1 = xPoses.getGlobalFrame(0.f) * scale + worldPos.x;
	float y1 = yPoses.getGlobalFrame(0.f) * scale + worldPos.y;
	float x2, y2;
	int subLine = xPoses.keys.size() * 6;//количество прямых кусочков из которых рисуется кривая сплайна
	while (t + 1.f / subLine < 1.f) {
		t += 1.f / subLine;//количество прямых кусочков из которых рисуется кривая сплайна
		x2 = xPoses.getGlobalFrame(t) * scale + worldPos.x;
		y2 = yPoses.getGlobalFrame(t) * scale + worldPos.y;
		Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);
		x1 = x2;
		y1 = y2;
	}
	x2 = xPoses.getGlobalFrame(1.f) * scale + worldPos.x;
	y2 = yPoses.getGlobalFrame(1.f) * scale + worldPos.y;
	Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);			

	static const float SIZEX = 3;
	for (unsigned int i = 0; i < xPoses.keys.size() - 1; ++i) {
		float x = xPoses.getFrame(i, 0.f) * scale + worldPos.x;
		float y = yPoses.getFrame(i, 0.f) * scale + worldPos.y;
		Render::GetDC()->Gfx_RenderLine(x - SIZEX, y - SIZEX, x + SIZEX, y + SIZEX, 0xFFFF0000);
		Render::GetDC()->Gfx_RenderLine(x - SIZEX, y + SIZEX, x + SIZEX, y - SIZEX, 0xFFFF0000);
	}
}

void LevelBlock::AddPoint(float x, float y) {
	xPoses.addKey(x);
	xPoses.CalculateGradient(true);
	yPoses.addKey(y);
	yPoses.CalculateGradient(true);
}


int LevelBlock::SearchNearest(float x, float y) {
	int result = -1;
	static const float SIZEX = 6;
	for (unsigned int i = 0; i < xPoses.keys.size() - 1 && result < 0; ++i) {
		float px = xPoses.getFrame(i, 0.f);
		float py = yPoses.getFrame(i, 0.f);
		if ((FPoint2D(x, y) - FPoint2D(px, py)).Length() < SIZEX) {
			result = i;
		}
	}
	return result;
}

bool DotNearLine(const FPoint2D &one, const FPoint2D &two, const FPoint2D &p) {
	float a = (one - p).Length();
	float b = (p - two).Length();
	float c = (one - two).Length();
	if (c > a && c > b) {
		float s;
		Math::STrinagle(a, b, c, s);
		return (s / c < 4);
	}
	return false;
}

bool LevelBlock::CreateDot(float x, float y) {
	if (xPoses.keys.size() >= 50) {
		return false;
	}
	bool result = false;
	static const float SIZEX = 6;
	FPoint2D p(x, y);

	float t = 0.f;
	FPoint2D one(xPoses.getGlobalFrame(0.f), yPoses.getGlobalFrame(0.f));
	FPoint2D two;
	int subLine = xPoses.keys.size() * 6;//количество прямых кусочков из которых рисуется кривая сплайна
	while (t + 1.f / subLine < 1.f && !result) {
		t += 1.f / subLine;
		two.x = xPoses.getGlobalFrame(t);
		two.y = yPoses.getGlobalFrame(t);
		if (result = DotNearLine(one, two, p)) {
			int index = t * (xPoses.keys.size() - 1) + 1;

			if (index < xPoses.keys.size() - 1) {
				SplinePath splineX = xPoses;
				SplinePath splineY = yPoses;
				xPoses.Clear();
				yPoses.Clear();
				for (int i = 0; i < index; ++i) {
					xPoses.addKey(splineX.getFrame(i, 0.f));
					yPoses.addKey(splineY.getFrame(i, 0.f));
				}
				xPoses.addKey(x);
				yPoses.addKey(y);
				for (int i = index; i < splineX.keys.size() - 1; ++i) {
					xPoses.addKey(splineX.getFrame(i, 0.f));
					yPoses.addKey(splineY.getFrame(i, 0.f));
				}
				xPoses.CalculateGradient(true);
				yPoses.CalculateGradient(true);
			} else {
				xPoses.addKey(x);
				yPoses.addKey(y);
				xPoses.CalculateGradient(true);
				yPoses.CalculateGradient(true);
			}
		}
		one = two;
	}
	if (!result) {
		two.x = xPoses.getGlobalFrame(1.f);
		two.y = yPoses.getGlobalFrame(1.f);
		if (result = DotNearLine(one, two, p)) {
			int index = xPoses.keys.size() - 1;
			SplinePath splineX = xPoses;
			SplinePath splineY = yPoses;
			xPoses.Clear();
			yPoses.Clear();
			for (int i = 0; i < index; ++i) {
				xPoses.addKey(splineX.getFrame(i, 0.f));
				yPoses.addKey(splineY.getFrame(i, 0.f));
			}
			xPoses.addKey(x);
			yPoses.addKey(y);
			for (int i = index; i < splineX.keys.size() - 1; ++i) {
				xPoses.addKey(splineX.getFrame(i, 0.f));
				yPoses.addKey(splineY.getFrame(i, 0.f));
			}
			xPoses.CalculateGradient(true);
			yPoses.CalculateGradient(true);
		}
	}

	return result;
}

void LevelBlock::RemoveDot(int index) {
	if (xPoses.keys.size() <= 4) {
		return;
	}
	SplinePath splineX = xPoses;
	SplinePath splineY = yPoses;
	xPoses.Clear();
	yPoses.Clear();
	for (int i = 0; i < index; ++i) {
		xPoses.addKey(splineX.getFrame(i, 0.f));
		yPoses.addKey(splineY.getFrame(i, 0.f));
	}
	for (int i = index + 1; i < splineX.keys.size() - 1; ++i) {
		xPoses.addKey(splineX.getFrame(i, 0.f));
		yPoses.addKey(splineY.getFrame(i, 0.f));
	}
	xPoses.CalculateGradient(true);
	yPoses.CalculateGradient(true);
}

void TileEditor::ClearLevel() {
	_level.startpoint.clear();
	_level.endpoint.clear();
	for (unsigned int i = 0; i < _level.ground.size(); ++i) {
		delete _level.ground[i];
	}
	_level.ground.clear();
	for (unsigned int i = 0; i < _level.surpris.size(); ++i) {
		delete _level.surpris[i];
	}
	_level.surpris.clear();
	for (unsigned int i = 0; i < _level.movable.size(); ++i) {
		delete _level.movable[i];
	}
	_level.movable.clear();
	for (unsigned int i = 0; i < _level.images.size(); ++i) {
		delete _level.images[i].sprite;
		Render::GetDC()->Texture_Free(_level.images[i].texture);
	}
	_level.images.clear();
	for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
		delete _level.beauties[i].sprite;
		Render::GetDC()->Texture_Free(_level.beauties[i].texture);
	}
	_level.beauties.clear();
}

void TileEditor::LoadLevel(std::string &msg) {
	ClearLevel();
	TiXmlElement *xe = _doc.RootElement()->FirstChildElement();
	while (xe != NULL && xe->Attribute("id") != msg) {			
		xe = xe->NextSiblingElement();
	}
	assert(xe != NULL);
	// level loading
	_state.clear();
	TiXmlElement *ground = xe->FirstChildElement("Ground");
	if (ground) {
		TiXmlElement *elem = ground->FirstChildElement("elem");
		while (elem != NULL) {
			LevelBlock *l = new LevelBlock();
			_level.ground.push_back(l);
		
			SplinePath x;
			SplinePath y;
			TiXmlElement *dot = elem->FirstChildElement("dot");
			while (dot != NULL) {
				x.addKey(atof(dot->Attribute("x")));
				y.addKey(atof(dot->Attribute("y")));
				dot = dot->NextSiblingElement();
			}
			x.CalculateGradient(true);
			y.CalculateGradient(true);
			l->xPoses = x;
			l->yPoses = y;

			elem = elem->NextSiblingElement("elem");
		}
	}
	{
		TiXmlElement *imagesList = xe->FirstChildElement("Images");
		if (imagesList) {
			TiXmlElement *elem = imagesList->FirstChildElement("image");
			while (elem != NULL) {
				OneImage image;
				std::string msg = elem->Attribute("filePath");
				image.texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + "data\\images\\" + msg).c_str());
				image.sprite = new hgeSprite(image.texture, 0, 0, Render::GetDC()->Texture_GetWidth(image.texture), Render::GetDC()->Texture_GetHeight(image.texture));
				image.pos = FPoint2D(0.f, 0.f);
				image.filePath = msg;
				_level.images.push_back(image);
				elem = elem->NextSiblingElement("image");
			}
		}
	}
	{
		TiXmlElement *beautyList = xe->FirstChildElement("Beauties");
		if (beautyList) {
			TiXmlElement *elem = beautyList->FirstChildElement("beauty");
			while (elem != NULL) {
				Beauty beauty;
				beauty.LoadFromXml(elem);
				_level.beauties.push_back(beauty);
				elem = elem->NextSiblingElement("beauty");
			}
		}
	}

	TiXmlElement *word = xe->FirstChildElement("word");
	_worldCenter.x = atof(word->Attribute("x"));
	_worldCenter.y = atof(word->Attribute("y"));

	if (word->Attribute("startX")) {
		_level.startpoint.push_back(
			FPoint2D(atof(word->Attribute("startX")), 
						atof(word->Attribute("startY"))));
	}
	if (word->Attribute("endX")) {
		_level.endpoint.push_back(
			FPoint2D(atof(word->Attribute("endX")), 
						atof(word->Attribute("endY"))));
	}


	_viewScale = atof(word->Attribute("scale"));
	SetValueS("play", "", "play");
	//ResetState();
	_currentLevel = msg;
	if (!_netVisible) {
		for (unsigned int i = 0; i < _level.ground.size(); ++i) {
			_level.ground[i]->GenerateTriangles();
		}
	}
}

void LevelBlock::ExportToLines(std::vector<FPoint2D> &lineDots) {
	lineDots.clear();
	float t = 0.f;
	int subLine = xPoses.keys.size() * 6;//количество прямых кусочков из которых рисуется кривая сплайна
	while (t < 1.f) {
		float x = xPoses.getGlobalFrame(t);
		float y = yPoses.getGlobalFrame(t);
		lineDots.push_back(FPoint2D(x, y));
		t += 1.f / subLine;//количество прямых кусочков из которых рисуется кривая сплайна
	}
}

float sign;
float minAngle;

void LevelBlock::GenerateTriangles() {
	triangles.clear();
	std::vector<FPoint2D> &dots = lineDots;
	ExportToLines(dots);

	//float sign = 0.f;
	sign = 0.f;
	{
		// ищем самый острый угол или наименее тупой
		FPoint2D *a;
		FPoint2D *b;
		FPoint2D *c;
		int index = 0;
		//float minAngle = 180.f;
		minAngle = 180.f;
		for (int i = 0; i < dots.size(); ++i) {
			a = &dots[i];
			if (i < dots.size() - 1) {
				b = &dots[i + 1];
			} else {
				b = &dots[i + 1 - dots.size()];
			}
			if (i < dots.size() - 2) {
				c = &dots[i + 2];
			} else {
				c = &dots[i + 2 - dots.size()];
			}
			float angle = (*a - *b).Angle(&(*c - *b));
			if (angle <= 0.f) {
				assert(false);
			}
			if (angle < minAngle) {
				minAngle = angle;
				index = i;
			}
		}
		a = &dots[index];
		if (index < dots.size() - 1) {
			b = &dots[index + 1];
		} else {
			b = &dots[index + 1 - dots.size()];
		}
		if (index < dots.size() - 2) {
			c = &dots[index + 2];
		} else {
			c = &dots[index + 2 - dots.size()];
		}

		//// посчитаем центр треугольника
		//FPoint2D m = FPoint2D((a->x + b->x + c->x) / 3.f, (a->y + b->y + c->y) / 3.f);
		//
		//// проверим находиться ли центр треугольника внтури области
		//int counter = 0;
		//for (int j = 0; j < dots.size(); ++j) {
		//	FPoint2D *a2 = &dots[j];
		//	FPoint2D *b2;
		//	if (j < dots.size() - 1) {
		//		b2 = &dots[j + 1];
		//	} else {
		//		b2 = &dots[j + 1 - dots.size()];
		//	} 
		//	if (a2->x < m.x && m.x <= b2->x) {// найти точку пересечения луча из М и отрезка a2b2
		//		float k = (a2->y - b2->y) / (a2->x - b2->x);
		//		float b = a2->y - a2->x * k;
		//		float y = k * m.x + b;
		//		if (y > m.y) {
		//			++counter;
		//		}
		//	}
		//}
		sign = Math::VMul(*b - *a, *c - *b);
		//if (counter % 2 != 1) {
		//	sign *= -1; // если точка m снаружи - меняем знак
		//}
	}
//	return;
	//int oldSize = dots.size() + 1;
	while (dots.size() > 0) {
		//if (oldSize == dots.size()) {
		//	dots.clear();
		//	break;
		//}
		//oldSize = dots.size();
		assert(dots.size() > 2);
		if (dots.size() == 3) {
			hgeTriple tri;
			FillTriangle(dots[0], dots[1], dots[2], tri);
			triangles.push_back(tri);
			dots.clear();
		} else {
			FPoint2D *a;
			FPoint2D *b;
			FPoint2D *c;
			for (int i = 0; i < dots.size(); ++i) {
				a = &dots[i];
				if (i < dots.size() - 1) {
					b = &dots[i + 1];
				} else {
					b = &dots[i + 1 - dots.size()];
				}
				if (i < dots.size() - 2) {
					c = &dots[i + 2];
				} else {
					c = &dots[i + 2 - dots.size()];
				}
				//// выкидываем точки находящиеся на одной линии
				//if (fabs((*a - *c).Length() - (*a - *b).Length() - (*b - *c).Length()) < 1e-3) {
				//	if (i < dots.size() - 1) {
				//		dots.erase(dots.begin() + i + 1);
				//	} else {
				//		dots.erase(dots.begin());
				//	}
				//	break;				
				//}
				
				bool intersection = false;
				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					FPoint2D *b2;
					if (j < dots.size() - 1) {
						b2 = &dots[j + 1];
					} else {
						b2 = &dots[j + 1 - dots.size()];
					}
					intersection = (a != a2 && a != b2 && b != a2 && b != b2 && Math::Intersection(*a, *c, *a2, *b2, NULL));
				}
				std::vector<FPoint2D> triangle(3);
				triangle[0] = *a;
				triangle[1] = *b;
				triangle[2] = *c;
				for (int j = 0; j < dots.size() && !intersection; ++j) {
					FPoint2D *a2 = &dots[j];
					intersection = (a2 != a && a2 != b && a2 != c && Math::Inside(*a2, triangle));
				}
				if (!intersection && Math::VMul(*b - *a, *c - *b) * sign > 0.f) {// выбираем только те что с нашим знаком
					hgeTriple tri;
					FillTriangle(*a, *b, *c, tri);
					triangles.push_back(tri);
					if (i < dots.size() - 1) {
						dots.erase(dots.begin() + i + 1);
					} else {
						dots.erase(dots.begin());
					}
					break;
				}
			}
		}		
	}
}

void LevelBlock::FillTriangle(const FPoint2D &a, const FPoint2D &b, const FPoint2D &c, hgeTriple &tri) {
	tri.v[0].x = a.x;
	tri.v[0].y = a.y;
	tri.v[1].x = b.x;
	tri.v[1].y = b.y;
	tri.v[2].x = c.x;
	tri.v[2].y = c.y;

	tri.blend = BLEND_ALPHABLEND | BLEND_COLORMUL;
	// надо добавить в движке HGE режим без блендинга - можно только в игре,
	// в редакторе не обязательно
	tri.tex = _allElements->GetTexture();
	for (unsigned int i = 0; i < 3; ++i) {
		tri.v[i].col = 0xFFFFFFFF;
		tri.v[i].z = 0.f;
		tri.v[i].tx = tri.v[i].x / 512.f;
		tri.v[i].ty = tri.v[i].y / 512.f;
	}
}

void LevelBlock::DrawTriangles(const FPoint2D &worldPos, float scale) {
	int n = triangles.size();// * f;
	for (unsigned int j = 0; j < n; ++j) {
		hgeTriple tri = triangles[j];
		for (unsigned int i = 0; i < 3; ++i) {
			tri.v[i].x = tri.v[i].x * scale + worldPos.x;
			tri.v[i].y = tri.v[i].y * scale + worldPos.y;
		}
		Render::GetDC()->Gfx_RenderTriple(&tri);
	}

	if (lineDots.size() >= 2) {
		// test debug
		float x1 = lineDots[0].x * scale + worldPos.x;
		float y1 = lineDots[0].y * scale + worldPos.y;
		float x2, y2;
		for (unsigned int i = 0; i < lineDots.size(); ++i) {
			x2 = lineDots[i].x * scale + worldPos.x;
			y2 = lineDots[i].y * scale + worldPos.y;
			Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0x4FFFFFFF);
			x1 = x2;
			y1 = y2;
		}
		x2 = lineDots[0].x * scale + worldPos.x;
		y2 = lineDots[0].y * scale + worldPos.y;
		Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0x4FFFFFFF);			
	}
}

bool LevelBlock::SearchProection(FPoint2D &pos) {
	float t = 0.f;
	float x1 = xPoses.getGlobalFrame(0.f);
	float y1 = yPoses.getGlobalFrame(0.f);
	float x2, y2;
	int subLine = xPoses.keys.size() * 6;//количество прямых кусочков из которых рисуется кривая сплайна
	FPoint2D gravity(0.f, 10.f);
	float speed = 50.f;
	//FPoint2D motor();
	while (t + 1.f / subLine < 1.f) {
		t += 1.f / subLine;//количество прямых кусочков из которых рисуется кривая сплайна
		x2 = xPoses.getGlobalFrame(t);
		y2 = yPoses.getGlobalFrame(t);
		if (x1 <= pos.x && pos.x < x2) {
			// calc pos
		//Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);
			return true;
		}
		x1 = x2;
		y1 = y2;
	}
	x2 = xPoses.getGlobalFrame(1.f);
	y2 = yPoses.getGlobalFrame(1.f);
	if (x1 <= pos.x && pos.x < x2) {
	//	Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0xFFFF0000);			
		return true;
	}
	return false;
}

void TileEditor::CalcNextBykePos(float dt) {
	FPoint2D proection;
	for (unsigned int i = 0; i < _level.ground.size(); ++i) {
		if (_level.ground[i]->SearchProection(proection)) {
		}
	}
}

void LevelBlock::CreateBody(b2Body *body, FPoint2D shift) {
	int n = triangles.size();
	for (unsigned int j = 0; j < n; ++j) {
		b2FixtureDef fd;
		fd.restitution = 0.f;
		fd.friction = 0.8f;
		fd.density = 1.0f;
		b2PolygonShape shape;
		b2Vec2 vec[3];
		for (unsigned int i = 0; i < 3; ++i) {
			vec[i].x = (triangles[j].v[2 - i].x + shift.x) / SCALE_BOX2D;
			vec[i].y = (triangles[j].v[2 - i].y + shift.y) / SCALE_BOX2D;
		}
		shape.Set(vec, 3);
		fd.shape = &shape;
		body->CreateFixture(&fd);
		body->ResetMassData();
	}
}

void TileEditor::SetupBox2D() {
	EraseAllBodyes();

	b2BodyDef bd;
	bd.type = b2_staticBody;  
	bd.position.Set(0.f, 0.f);
	_oldShift = FPoint2D(0.f, 0.f);
	_endPoint = _level.endpoint[0];
	bd.angle = 0.f;
	b2Body* body = m_world->CreateBody(&bd);		
	for (unsigned int i = 0; i < _level.ground.size(); ++i) {
		_level.ground[i]->CreateBody(body, _oldShift);
	}
	body->ResetMassData();
	_currentLandBody.push_back(body);
	_needMoveToOrigin = false;

	{
		b2BodyDef bd;
		bd.type = b2_dynamicBody;  
		bd.fixedRotation = false;

		bd.position.Set(_level.startpoint[0].x / SCALE_BOX2D, _level.startpoint[0].y / SCALE_BOX2D);
		bd.angle = 0.f;
		
		b2Body* body = m_world->CreateBody(&bd);
		
		b2FixtureDef fd;
		fd.restitution = 0.5f;
		fd.friction = 0.8f;
		fd.density = 1.f;

		b2CircleShape shape;
		shape.m_radius = 12.5f / SCALE_BOX2D;
		fd.shape = &shape;
		b2Fixture *fixt = body->CreateFixture(&fd);

		body->SetUserData(_byker);
		body->ResetMassData();
		_byker->_attachedBody = body;
	}
}
