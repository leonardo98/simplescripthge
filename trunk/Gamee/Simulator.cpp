#include "Simulator.h"
#include "..\Core\Core.h"
#include <cstdio>

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
	TiXmlDocument doc(filename);
	if (!doc.LoadFile()) {
		return;
	}
	TiXmlElement *bodyDef = doc.RootElement()->FirstChildElement();
	while (bodyDef) {
		BodyTemplate *bt = new BodyTemplate(bodyDef);
		_collection.push_back(bt);
		bodyDef = bodyDef->NextSiblingElement();
	}
}

Simulator::Simulator(TiXmlElement *xe)
	: _viewScale(200.f)
	, _worldCenter(400.f, 400.f)
	, _groundType(BODY_TYPE_GROUND)
	, _angleMultiplier(BodyTemplate::MAX / (M_PI * 2))
	//, _selectedBody(NULL)
{
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

	{
		b2BodyDef bd;
		m_groundBody = m_world->CreateBody(&bd);
		m_groundBody->SetUserData(&_groundType);

		b2PolygonShape shape;
		shape.SetAsEdge(b2Vec2(-5.0f, 0.0f), b2Vec2(5.0f, 0.f));
		m_groundBody->CreateFixture(&shape, 0.0f);

		shape.SetAsEdge(b2Vec2(-5.0f, 5.f), b2Vec2(-5.0f, 0.f));
		m_groundBody->CreateFixture(&shape, 0.0f);

		shape.SetAsEdge(b2Vec2(5.0f, 5.f), b2Vec2(5.0f, 0.f));
		m_groundBody->CreateFixture(&shape, 0.0f);
	}
}

Simulator::~Simulator()
{
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

void Simulator::OnKeyDown(int key){ 

	if (key == HGEK_MINUS) {
		_viewScale *= 0.9;
	} else if (key == HGEK_EQUALS) {
		_viewScale *= 1.09;
	}
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position.Set(RandomFloat(-2.f, 2.f), 3);
	b2Body* body = m_world->CreateBody(&bd);

	int index = 0;
	if (key == HGEK_1) {
		while (_collection[index]->_type != BODY_TYPE_UNBREAKABLE && index < _collection.size()) {++index;};
	} else if (key == HGEK_2) {
		while (_collection[index]->_type != BODY_TYPE_BLUE && index < _collection.size()) {++index;};
	} else if (key == HGEK_3) {
		while (_collection[index]->_type != BODY_TYPE_EXPLOSION && index < _collection.size()) {++index;};
	} else if (key == HGEK_4) {
		while (_collection[index]->_type != BODY_TYPE_BALL && index < _collection.size()) {++index;};
	} else {
		index = _collection.size();// чтобы далее нас выкинули
	}
	if (index >= _collection.size()) {
		m_world->DestroyBody(body);
		return;
	}
	b2FixtureDef fd;
	fd.restitution = _collection[index]->_restitution;
	fd.friction = _collection[index]->_friction;
	fd.density = 1.0f;

	if (_collection[index]->_shape == "circle") {
		b2CircleShape shape;
		shape.m_radius = _collection[index]->_radius;
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else if (_collection[index]->_shape == "box") {
		b2PolygonShape shape;
		shape.SetAsBox(_collection[index]->_width / 2.f, _collection[index]->_height / 2.f);
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else {
		m_world->DestroyBody(body);
		return;
	}

	body->SetUserData(_collection[index]);
}

void Simulator::OnMouseDown(hgeVector mousePos)
{	
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
		BodyTemplate *bt = (BodyTemplate *)(body->GetUserData());
		if (Core::GetDC()->Input_GetKeyState(HGEK_ALT)) {
			m_world->DestroyBody(body);
			return;
		} else if (bt->_type == BODY_TYPE_EXPLOSION && Core::GetDC()->Input_GetKeyState(HGEK_CTRL)) {
			m_world->DestroyBody(body);
			Explosion(body->GetPosition(), bt->_radius, bt->_maxForce);
		} else {
			b2MassData m;
			m.center = b2Vec2(0,0);
			m.mass = 0.00001f;
			m.I = 0.00001f;
			body->SetMassData(&m);

			b2MouseJointDef md;
			md.bodyA = m_groundBody;
			md.bodyB = body;
			md.target = p;
			md.maxForce = 1000.0f;
			m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
			body->SetAwake(true);
		}
	}
}

void Simulator::OnMouseUp()
{
	if (m_mouseJoint) {
		m_mouseJoint->GetBodyB()->ResetMassData();
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}
}

void Simulator::OnMouseMove(hgeVector mousePos)
{
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(1.f / _viewScale * (mousePos.x - _worldCenter.x), 1.f / _viewScale * (_worldCenter.y - mousePos.y)); // нужен для выбора объекта по которому кликнули

	if (Core::GetDC()->Input_GetKeyState(HGEK_RBUTTON)) {
		_worldCenter += (mousePos - _lastMousePos);
	}
	if (m_mouseJoint)
	{
		m_mouseJoint->SetTarget(p);
	} else {		
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
			BodyTemplate *bt = (BodyTemplate *)(body->GetUserData());
			if (Core::GetDC()->Input_GetKeyState(HGEK_ALT)) {
				m_world->DestroyBody(body);
			}
		}
	}
	_lastMousePos = mousePos;
	m_mouseWorld = p;
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

bool Simulator::IsMouseOver(hgeVector mousePos) {
	return true;
}

void Simulator::PostSolve(const b2Contact* contact, const b2ContactImpulse* impulse)
{
	/*if (m_broke)
	{
		// The body already broke.
		return;
	}

	// Should the body break?
	int32 count = contact->GetManifold()->pointCount;

	float32 maxImpulse = 0.0f;
	for (int32 i = 0; i < count; ++i)
	{
		maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
	}

	if (maxImpulse > 40.0f)
	{
		// Flag the body for breaking.
		m_break = true;
	}*/
}

inline void Simulator::DrawElement(hgeVertex *&buf, const BodyTemplate::UV *uv, const b2Vec2 &pos, const FPoint2D *angles) {
	float x =   _viewScale * pos.x + _worldCenter.x;
	float y = - _viewScale * pos.y + _worldCenter.y;
	//size = size * _viewScale / sqrt(2.f);
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
inline void Simulator::DrawLine(const b2Vec2 &a, const b2Vec2 &b, DWORD color)
{
	Core::GetDC()->Gfx_RenderLine(_viewScale * a.x + _worldCenter.x, - _viewScale * a.y + _worldCenter.y, _viewScale * b.x + _worldCenter.x, - _viewScale * b.y + _worldCenter.y, color);
}

void Simulator::Draw() {

	for(b2Fixture *f = m_groundBody->GetFixtureList(); f; f = f->GetNext()) {
		b2PolygonShape *s = (b2PolygonShape *)f->GetShape();
		DrawLine(s->GetVertex(0), s->GetVertex(1));
	}

	int max;
	hgeVertex *buffer = Core::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
	unsigned int counter = 0;

	if (Core::GetDC()->Input_GetKeyState(HGEK_SHIFT)) { // рисуем интерфейс
		FPoint2D oldWC = _worldCenter;
		_worldCenter = FPoint2D(25, 25);
		float oldScale = _viewScale;
		_viewScale = 100.f;
		for (unsigned int i = 0; i < _collection.size(); i++) {
			const BodyTemplate *bt = _collection[i];
			DrawElement(buffer, bt->_uv, b2Vec2(i * 0.5f, 0), bt->_positions[0]);
			++counter;
		}
		_worldCenter = oldWC;
		_viewScale = oldScale;
	}

	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		const b2Transform & xf = body->GetTransform();
		int type = *(int *)(body->GetUserData());
		if (type == BODY_TYPE_GROUND) {
			continue;
		}
		const BodyTemplate *bt = (BodyTemplate *)(body->GetUserData());
		int angle = round(xf.GetAngle() * _angleMultiplier + BodyTemplate::MAX) % BodyTemplate::MAX;
		assert(0 <= angle && angle <= BodyTemplate::MAX);
		switch (bt->_type) {
			case BODY_TYPE_UNBREAKABLE: {
				DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
				break; }
			case BODY_TYPE_BLUE: {
				DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
				break; }
			case BODY_TYPE_EXPLOSION: {
				DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
				break; }
			case BODY_TYPE_BALL: {
				DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
				break; }
			default : continue;
		};
		
		++counter;
		if (counter > max) {
			assert(false);
		}
	}
	Core::GetDC()->Gfx_FinishBatch(counter);
}

void Simulator::Update(float deltaTime) {	
	if (Core::GetDC()->Input_GetKeyState(HGEK_SHIFT) || Core::GetDC()->Input_GetKeyState(HGEK_ALT)) {return;}
	Step(&settings);
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
			force = sqrt(strength) * maxForce;
			angle = atan2f(b2BodyPosition.y - b2TouchPosition.y, b2BodyPosition.x - b2TouchPosition.x);
			// Apply an impulse to the body, using the angle
			b->ApplyLinearImpulse(b2Vec2(cosf(angle) * force, sinf(angle) * force), b->GetPosition());
		}
	}
}