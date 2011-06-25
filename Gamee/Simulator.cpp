#include "Simulator.h"
#include "..\Helpers\MyMessageBox.h"
#include "..\Core\Core.h"
#include "..\Core\Render.h"
#include <cstdio>
#define LEVELS_FILE "levels.xml"

int Simulator::round(float a) {
	int b = static_cast<int>(a);
	return (a - b) > 0.5f ? b + 1 : b;
} 

void DestructionListener::SayGoodbye(b2Joint* joint)
{
	if (test->m_mouseJoint == joint) {
		test->m_mouseJoint = NULL;
	} else {
		test->JointDestroyed(joint);
	}
}

void Simulator::LoadTemplates(const std::string &filename) {
	TiXmlDocument doc(filename.c_str());
	if (!doc.LoadFile()) {
		return;
	}
	TiXmlElement *bodyDef = doc.RootElement()->FirstChildElement("b2object");
	while (bodyDef) {
		_collection.push_back(new BodyTemplate(bodyDef));
		bodyDef = bodyDef->NextSiblingElement("b2object");
	}
}

Simulator::Simulator(TiXmlElement *xe)
	: _viewScale(100.f)
	, _worldCenter((SCREEN_WIDTH = 480) / 2, (SCREEN_HEIGHT = 320) / 2)
//	, _angleMultiplier(BodyTemplate::MAX / (M_PI * 2))
	, Messager("simulator")
	, _editor(false)
	, _selectedBody(NULL)
	, _signal(0.f)
	, _doc(LEVELS_FILE)
	, _currentLevel("")
	, _waitState(WaitNone)
	, _mouseDown(false)
	, _waitYesNoNewLevel(false)
	, _waitYesNoDelSelected(false)
	, _waitYesNoOverwrite(false)
	, _waitAddNewElem(false)
	, SLIDER_SCALE(5.f)
	, SLIDER_MIN(0.2f)
{
	if (!_doc.LoadFile()) {
		OkMessageShow("Error!\nLevels file not found!");
	} 
	_selectedUV[0].u = 0.f; _selectedUV[0].v = 0.f;
	_selectedUV[1].u = 0.f; _selectedUV[1].v = 0.f;
	_selectedUV[2].u = 0.f; _selectedUV[2].v = 0.f;
	_selectedUV[3].u = 0.f; _selectedUV[3].v = 0.f;

	_allElements = Core::getTexture("allElements");
	LoadTemplates("bodyes.xml");

	b2Vec2 gravity;
	gravity.Set(0.0f, -10.0f);
	bool doSleep = false;
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

Simulator::~Simulator()
{
	EraseAllBodyes();
	// By deleting the world, we delete the bomb, mouse joint, etc.
	delete m_world;
	m_world = NULL;
	for (Collection::iterator i = _collection.begin(), e = _collection.end(); i != e; i++) {
		delete (*i);
	}
}

void Simulator::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
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

void Simulator::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	if (_startLevel.Action()) {
		return;
	}
	BodyTemplate *btA = static_cast<MyBody *>(contact->GetFixtureA()->GetBody()->GetUserData())->base;
	BodyTemplate *btB = static_cast<MyBody *>(contact->GetFixtureB()->GetBody()->GetUserData())->base;
	if (!(btA->_breakable) && !(btB->_breakable))
	{
		return;
	}

	// Should the body break?
	int32 count = contact->GetManifold()->pointCount;

	float32 maxImpulse = 0.0f;
	for (int32 i = 0; i < count; ++i)
	{
		maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
	}

	if (maxImpulse > 0.1f)
	{
		// Flag the body for breaking.
		if (btA->_breakable)
		{
			static_cast<MyBody *>(contact->GetFixtureA()->GetBody()->GetUserData())->broken = true;
		}
		if (btB->_breakable)
		{
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

void Simulator::EraseBody(b2Body *body) {
	if (body->GetUserData() != NULL) {
		MyBody *t = static_cast<MyBody *>(body->GetUserData());
		delete t;
		body->SetUserData(NULL);
		m_world->DestroyBody(body);
	}
}

void Simulator::EraseAllBodyes() {
	for (b2Body *body = m_world->GetBodyList(); body; ) {
		b2Body *remove = body;
		body = body->GetNext();
		if (remove) {
			EraseBody(remove);
		}
	}
}

// добавляем новый элемент в "случайное" место на экране
b2Body * Simulator::AddElement(const std::string &typeId) {
	Collection::iterator index = _collection.begin();
	while (index != _collection.end() && (*index)->_id != typeId ) {index++;}
	assert(index != _collection.end() && (*index)->_id == typeId );

	BodyState bs;
	bs.base = (*index);	
	bs.angle = 0;
	{// найдём центр экрана 

	}
	bs.pos = b2Vec2((SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale, (_worldCenter.y - SCREEN_HEIGHT / 2) / _viewScale);
	bs.width = bs.base->_width;
	bs.height = bs.base->_height;
	bs.radius = bs.base->_radius;
	
	return AddElement(bs);
}

// ДЛЯ ИГРЫ тут можно соптимизировать - все элементы "земли" вынести в одно body
// создаем физическое тело по описанию
b2Body * Simulator::AddElement(const BodyState &bodyState){ 

	b2BodyDef bd;
	bd.type = b2_dynamicBody;  
	if (bodyState.base->_fixed) {
		//bd.type = b2_staticBody;);// ХАК - QueryAABB не работает со статичными объктами, 
	}								// т.е. иначе их нельзя будет двигать в редакторе
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
		shape.m_radius = bodyState.radius;
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else if (bodyState.base->_shape == "box") {
		b2PolygonShape shape;
		shape.SetAsBox(bodyState.width / 2.f, bodyState.height / 2.f);
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else if (bodyState.base->_shape == "triangle") {
		b2PolygonShape shape;
		b2Vec2 vec[3];
		vec[0] = b2Vec2( bodyState.width / 2.f, bodyState.height / 2.f);
		vec[1] = b2Vec2(-bodyState.width / 2.f, bodyState.height / 2.f);
		vec[2] = b2Vec2(-bodyState.width / 2.f, -bodyState.height / 2.f);
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

void Simulator::OnMouseDown(FPoint2D mousePos)
{	_mouseDown = true;
	_lastMousePos = mousePos;
	InitParams(NULL);
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(1.f / _viewScale * (mousePos.x - _worldCenter.x), 1.f / _viewScale * (_worldCenter.y - mousePos.y)); // нужен для выбора объекта по которому кликнули
	
	if (m_mouseJoint != NULL)
	{
		return;
	}

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	m_world->QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		//BodyTemplate *bt = static_cast<MyBody *>(body->GetUserData())->base;
		if (_editor) {
			InitParams(body);
			return;
		}
	}
}

void Simulator::OnMouseUp()
{
	_mouseDown = false;
	if (m_mouseJoint) {
		m_mouseJoint->GetBodyB()->ResetMassData();
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}
	if (_editor){
		return;
	}
	FPoint2D fp = 1.f / _viewScale * (_lastMousePos - _worldCenter);
	b2Vec2 p(1.f / _viewScale * (_lastMousePos.x - _worldCenter.x), 1.f / _viewScale * (_worldCenter.y - _lastMousePos.y)); // нужен для выбора объекта по которому кликнули
	
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
			Explosion(pos, bt->_radius, bt->_maxForce);
		} else if (bt->_destroyOnTap) {
			static_cast<MyBody *>(body->GetUserData())->broken = true;
		}
	}
}

void Simulator::InitParams(b2Body *body) 
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

bool Simulator::CanLevelStart() {
	return true;
	/* // если в уровне есть "динамит" и синие коробки - в него можно играть
	bool tnt = false;
	bool blue = false;
	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		const BodyTemplate *bt = static_cast<MyBody *>(body->GetUserData())->base;		
		if (!tnt && bt->_type == BODY_TYPE_EXPLOSION) {
			tnt = true;
		}
		if (!blue && bt->_type == BODY_TYPE_BLUE) {
			blue = true;
		}
	}
	return (tnt && blue);
	*/
}

bool Simulator::IsLevelFinish() {
	return false;//_finish != 0x3;
}


void Simulator::OnMouseMove(FPoint2D mousePos)
{
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(1.f / _viewScale * (mousePos.x - _worldCenter.x), 1.f / _viewScale * (_worldCenter.y - mousePos.y)); // нужен для выбора объекта по которому кликнули

	if (m_mouseJoint) {
		m_mouseJoint->SetTarget(p);
	} else if (_editor && _mouseDown && _selectedBody) {
		b2Vec2 pos = _selectedBody->GetPosition();
		pos += 1.f / _viewScale * b2Vec2(mousePos.x - _lastMousePos.x, _lastMousePos.y - mousePos.y);
		_selectedBody->SetTransform(pos, _selectedBody->GetAngle());
	} else if (_mouseDown) {
		_worldCenter += (mousePos - _lastMousePos);
	}
	_lastMousePos = mousePos;
	m_mouseWorld = p;
}

bool Simulator::OnMouseWheel(int direction) {
	if (direction > 0) {
		_viewScale *= 1.09f * direction;
	} else if (direction < 0) {
		_viewScale *= 0.9f * abs(direction);
	}
	return true;
}

void Simulator::Step(Settings* settings)
{
	float32 timeStep = settings->hz > 0.0f ? 1.0f / settings->hz : float32(0.0f);

	if (settings->pause)
	{
		if (settings->singleStep)
		{
			settings->singleStep = 0;
		}
		else
		{
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

	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}

	if (m_mouseJoint)
	{
	}
}

bool Simulator::IsMouseOver(FPoint2D mousePos) {
	return true;
}

#ifndef HGE_COMPILE_KEY
inline void Simulator::DrawElement(CIwSVec2 *&bufVert, CIwSVec2 *&bufUV, const BodyTemplate::UV *uv, const b2Vec2 &pos, const FPoint2D *angles) {
	float x =   _viewScale * pos.x + _worldCenter.x;
	float y = - _viewScale * pos.y + _worldCenter.y;
#define INT(a) static_cast<int16>(a)
	bufVert[0].x = INT(x + _viewScale * angles[0].x); bufVert[0].y = INT(y + _viewScale * angles[0].y); 
	bufVert[1].x = INT(x + _viewScale * angles[1].x); bufVert[1].y = INT(y + _viewScale * angles[1].y); 
	bufVert[2].x = INT(x + _viewScale * angles[2].x); bufVert[2].y = INT(y + _viewScale * angles[2].y); 
	bufVert[3].x = INT(x + _viewScale * angles[3].x); bufVert[3].y = INT(y + _viewScale * angles[3].y); 
	bufVert += 4;

	bufUV[0].x = uv[0].u; bufUV[0].y = uv[0].v;// buf[0].col = 0xFFFFFFFF;
	bufUV[1].x = uv[1].u; bufUV[1].y = uv[1].v;// buf[1].col = 0xFFFFFFFF;
	bufUV[2].x = uv[2].u; bufUV[2].y = uv[2].v;// buf[2].col = 0xFFFFFFFF;
	bufUV[3].x = uv[3].u; bufUV[3].y = uv[3].v;// buf[3].col = 0xFFFFFFFF;
	bufUV += 4;
}
#else
inline void Simulator::DrawElement(Vertex *&buf, const UV *uv, const b2Vec2 &pos, const FPoint2D *angles) {
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
#endif

/// DrawLine
inline void Simulator::DrawLine(const b2Vec2 &a, const b2Vec2 &b, DWORD color)
{
	Render::Line(_viewScale * a.x + _worldCenter.x, - _viewScale * a.y + _worldCenter.y, _viewScale * b.x + _worldCenter.x, - _viewScale * b.y + _worldCenter.y, color);
}

void Simulator::Draw() {
	bool oldTnt = (_finish & 0x1) != 0;
	bool oldBlue = (_finish & 0x2) != 0;
	bool tnt = false;
	bool blue = false;
	_finish = 0x0;
	int max = 1;
#ifndef HGE_COMPILE_KEY
	CIwSVec2 *bufferVert = _xy;
	CIwSVec2 *bufferUV = _uvs;
	Render::StartVertexBuffer(_allElements);
#else
	Vertex *buffer;
	buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
#endif
	unsigned int counter = 0;
	bool exception = false;
	FPoint2D pselect[4];
	typedef std::list<b2Body *> Remove;
	Remove remove;
	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		const MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
		if (myBody->broken) {
			remove.push_back(body);
		}
		const BodyTemplate *bt = myBody->base;
		const b2Transform & xf = body->GetTransform();

		FPoint2D p[4];
		float width2 = myBody->width / 2;
		float height2 = myBody->height / 2;
		p[0].x = -width2; p[0].y =  height2;
		p[1].x =  width2; p[1].y =  height2;
		p[2].x =  width2; p[2].y = -height2;
		p[3].x = -width2; p[3].y = -height2;

		float angle(-xf.GetAngle());
		p[0] = *p[0].Rotate(angle);
		p[1] = *p[1].Rotate(angle);
		p[2] = *p[2].Rotate(angle);
		p[3] = *p[3].Rotate(angle);
#ifndef HGE_COMPILE_KEY
		DrawElement(bufferVert, bufferUV, bt->_uv, xf.position, p);
#else
		DrawElement(buffer, bt->_uv, xf.position, p);
#endif
		if (_selectedBody == body) {
			for (unsigned int i = 0; i < 4; i++) { pselect[i] = p[i]; }
		}
		++counter;
#ifndef HGE_COMPILE_KEY
	}
	Render::FinishVertexBuffer(_xy, _uvs, counter);
#else
		if (static_cast<int>(counter) > max) {
			assert(false);
		}
	}
	Render::GetDC()->Gfx_FinishBatch(counter);
#endif
	for (;remove.begin() != remove.end();) {
		EraseBody(*remove.begin());
		remove.erase(remove.begin());
	}
	if (oldTnt && !tnt) {
		_lastTimer.Init(4.f);
	} else if (oldBlue && !blue) {
		_lastTimer.Init(2.f);
	}
	if (_selectedBody && _signal > 0.5f) {
		max = 1;
		const BodyTemplate *bt = static_cast<MyBody *>(_selectedBody->GetUserData())->base;
		const b2Transform & xf = _selectedBody->GetTransform();

#ifdef HGE_COMPILE_KEY
		Vertex *buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_ALPHAADD | BLEND_COLORADD, &max);
#else
		CIwSVec2 *bufferVert = _xy;
		CIwSVec2 *bufferUV = _uvs;
		Render::StartVertexBuffer(_allElements);
#endif


#ifndef HGE_COMPILE_KEY
		DrawElement(bufferVert, bufferUV, bt->_uv, xf.position, pselect);
#else
		DrawElement(buffer, bt->_uv, xf.position, pselect);
#endif

#ifdef HGE_COMPILE_KEY
		Render::GetDC()->Gfx_FinishBatch(1);
#else
		Render::FinishVertexBuffer(_xy, _uvs, 1);
#endif
	}
}

void Simulator::Update(float deltaTime) {	
	if (_editor) {
		_signal += 2 * deltaTime;
		while (_signal > 1.f) {
			_signal -= 1.f;
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
					static_cast<b2CircleShape *>(_selectedBody->GetFixtureList()->GetShape())->m_radius = myBody->radius;
				} else if (myBody->base->_shape == "box") {
					myBody->width = width;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_polygon);
					static_cast<b2PolygonShape *>(_selectedBody->GetFixtureList()->GetShape())->SetAsBox(myBody->width / 2.f, myBody->height / 2.f);
				} else if (myBody->base->_shape == "triangle") {
					myBody->width = width;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_polygon);
					b2Vec2 vec[3];
					vec[0] = b2Vec2( myBody->width / 2.f, myBody->height / 2.f);
					vec[1] = b2Vec2(-myBody->width / 2.f, myBody->height / 2.f);
					vec[2] = b2Vec2(-myBody->width / 2.f, -myBody->height / 2.f);
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
					static_cast<b2PolygonShape *>(_selectedBody->GetFixtureList()->GetShape())->SetAsBox(myBody->width / 2.f, myBody->height / 2.f);
				} else if (myBody->base->_shape == "triangle") {
					myBody->height = height;
					assert(_selectedBody->GetFixtureList()->GetShape()->GetType() == b2Shape::e_polygon);
					b2Vec2 vec[3];
					vec[0] = b2Vec2( myBody->width / 2.f, myBody->height / 2.f);
					vec[1] = b2Vec2(-myBody->width / 2.f, myBody->height / 2.f);
					vec[2] = b2Vec2(-myBody->width / 2.f, -myBody->height / 2.f);
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
		Step(&settings);
		if (_startLevel.Action()) {
			_startLevel.Update(deltaTime);
		}
		if (_lastTimer.Action()) {
			_lastTimer.Update(deltaTime);
			return;
		}
		if (IsLevelFinish()) {
			if (_finish & 0x2) { // остались синие
				OkMessageShow("Game over!\nYou lose!");
			} else {
				OkMessageShow("Congratulation!\nYou win!");
			}
			OnMessage("play");
		}
	}
}

void Simulator::Explosion(b2Vec2 pos, float maxDistance, float maxForce)
{
	b2Vec2 b2TouchPosition = b2Vec2(pos.x, pos.y);
	b2Vec2 b2BodyPosition;
	float distance; 
	float strength;
	float force;
	float angle;
	for (b2Body* b = m_world->GetBodyList(); b; b = b->GetNext())
	{
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

void Simulator::SaveState() {
	_state.clear();
	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		const b2Transform & xf = body->GetTransform();
		MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
		if (myBody->base->_fixed) {
			body->SetType(b2_staticBody);// ХАК - QueryAABB не работает со статичными объктами, 
										 // т.е. иначе их нельзя будет двигать в редакторе
		}
		BodyState s;
		s.base = myBody->base;
		s.angle = xf.GetAngle();
		s.pos = xf.position;
		s.width = myBody->width;
		s.height = myBody->height;
		s.radius = myBody->radius;
		_state.push_back(s);
	}
}

void Simulator::ResetState() {
	InitParams(NULL);
	_editor = true;
	EraseAllBodyes();
	for (BodyStates::iterator i = _state.begin(), e = _state.end(); i != e; i++) {

		/*b2Body *b = */AddElement(*i);
		//b->SetTransform(_state[i].pos, _state[i].angle);
	}
}

void Simulator::OnMessage(const std::string &message) {
	if (_waitYesNoNewLevel) {
		if (message == "yes") {
			InitParams(NULL);
			_currentLevel = "";
			_state.clear();
			EraseAllBodyes();
		}
		_waitYesNoNewLevel = false;
	} else if (_waitYesNoDelSelected) {
		EraseBody(_selectedBody);
		InitParams(NULL);
		_waitYesNoDelSelected = false;
	} else if (_waitYesNoOverwrite) {
		SaveLevel(_saveLevelName);
		_waitYesNoOverwrite = false;
	}
	std::string msg;
	if (message == "changes") {
		InitParams(_selectedBody);
	} else if (message == "play") {
		if (_editor) { // переходим в режим игры
			if (CanLevelStart()) {
				SetValueB("big", "visible", false);
				SetValueB("small", "visible", true);
				SetValueS("play", "", "stop");
				_editor = false;
				InitParams(NULL);
				SaveState();
				_finish = 0x3;
				_startLevel.Init(2.f);
			} else {
				OkMessageShow("Error!\nLevel must have TNT and BLUE boxes!");
			}
		} else { // в редактор
			SetValueS("play", "", ">>");
			ResetState();
		}
	} else if (message == "add new elem") {
		for (Collection::iterator i = _collection.begin(); i != _collection.end(); i++) {
			Messager::SendMessage("SmallList", "add " + (*i)->_id);
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
		if (_waitAddNewElem) {
			_waitAddNewElem = false;
			Messager::SendMessage("SmallList", "clear");
			if (msg == "cancel") {
				return;
			}
			if (!_editor) {
				SetValueS("play", "", ">>");
				ResetState();
			}
			InitParams(AddElement(msg));
		} else if (_waitState == WaitForLevelOpen) {
			_waitState = WaitNone;
			Messager::SendMessage("BigList", "clear");
			if (msg == "cancel") {
				return;
			}
			TiXmlElement *xe = _doc.RootElement()->FirstChildElement();
			while (xe != NULL && xe->Attribute("id") != msg) {			
				xe = xe->NextSiblingElement();
			}
			assert(xe != NULL);
			// level loading
			_state.clear();
			TiXmlElement *elem = xe->FirstChildElement("element");
			while (elem != NULL) {
				Simulator::BodyState s;
				std::string typeId = elem->Attribute("type");

				Collection::iterator index = _collection.begin();
				while ((*index)->_id != typeId && index != _collection.end()) {index++;}
				assert(index != _collection.end());
				
				s.base = (*index);
				s.pos.x = static_cast<float>(atof(elem->Attribute("x")));
				s.pos.y = static_cast<float>(atof(elem->Attribute("y")));
				s.angle = static_cast<float>(atof(elem->Attribute("angle")));
				s.width = static_cast<float>(atof(elem->Attribute("width")));
				s.height = static_cast<float>(atof(elem->Attribute("height")));
				_state.push_back(s);
				elem = elem->NextSiblingElement("element");
			}
			TiXmlElement *word = xe->FirstChildElement("word");
			_worldCenter.x = atof(word->Attribute("x"));
			_worldCenter.y = atof(word->Attribute("y"));
			_viewScale = atof(word->Attribute("scale"));
			SetValueS("play", "", ">>");
			ResetState();
			_currentLevel = msg;
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

void Simulator::SaveLevel(const std::string &levelName) {
	// level saving
	SaveState();
	TiXmlElement *elem = _saveLevelXml->FirstChildElement();
	while (elem != NULL) {
		TiXmlElement *remove = elem;
		elem = elem->NextSiblingElement();
		_saveLevelXml->RemoveChild(remove);
	}
	for (BodyStates::iterator i = _state.begin(), e = _state.end(); i != e; i++) {
		TiXmlElement *elem = new TiXmlElement("element");
		elem->SetAttribute("type", i->base->_id.c_str());
		char s[16];
		sprintf(s, "%f", i->pos.x); elem->SetAttribute("x", s);
		sprintf(s, "%f", i->pos.y); elem->SetAttribute("y", s);
		sprintf(s, "%f", i->angle); elem->SetAttribute("angle", s);
		sprintf(s, "%f", i->width); elem->SetAttribute("width", s);
		sprintf(s, "%f", i->height); elem->SetAttribute("height", s);
		_saveLevelXml->LinkEndChild(elem);
	}
	TiXmlElement *word = new TiXmlElement("word");
	word->SetAttribute("x", _worldCenter.x);
	word->SetAttribute("y", _worldCenter.y);
	word->SetAttribute("scale", _viewScale);
	_saveLevelXml->LinkEndChild(word);

	_doc.SaveFile();
	_currentLevel = levelName;
}