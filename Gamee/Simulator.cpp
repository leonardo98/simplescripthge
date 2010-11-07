#include "Simulator.h"
#include "..\Core\Core.h"
#include <cstdio>

void DestructionListener::SayGoodbye(b2Joint* joint)
{
	if (test->m_mouseJoint == joint)
	{
		test->m_mouseJoint = NULL;
	}
	else
	{
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
		_templates.push_back(new BodyTemplate(bodyDef));
		bodyDef = bodyDef->NextSiblingElement();
	}
}

Simulator::Simulator(TiXmlElement *xe)
	: _viewScale(100.f)
	, _worldCenter(400.f, 300.f)
{
	_allElements = Core::getTexture("allElements");
	
	// синий €щик
	_blue_box[0].u = 0.75f; _blue_box[0].v =   1.f;
	_blue_box[1].u =   1.f; _blue_box[1].v =   1.f;
	_blue_box[2].u =   1.f; _blue_box[2].v = 0.75f;
	_blue_box[3].u = 0.75f; _blue_box[3].v = 0.75f;

	// красный €щик
	_red_box[0].u =  0.5f; _red_box[0].v =   1.f;
	_red_box[1].u = 0.75f; _red_box[1].v =   1.f;
	_red_box[2].u = 0.75f; _red_box[2].v = 0.75f;
	_red_box[3].u =  0.5f; _red_box[3].v = 0.75f;

	// серый €щик
	_wall[0].u = 0.25f; _wall[0].v =   1.f;
	_wall[1].u =  0.5f; _wall[1].v =   1.f;
	_wall[2].u =  0.5f; _wall[2].v = 0.75f;
	_wall[3].u = 0.25f; _wall[3].v = 0.75f;

	// м€ч
	_ball[0].u =   0.f; _ball[0].v = 0.25f;
	_ball[1].u = 0.25f; _ball[1].v = 0.25f;
	_ball[2].u = 0.25f; _ball[2].v = 0.0f;
	_ball[3].u =   0.f; _ball[3].v = 0.0f;

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

	b2BodyDef bodyDef;
	m_groundBody = m_world->CreateBody(&bodyDef);

	{
		b2BodyDef bd;
		b2Body* ground = m_world->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsEdge(b2Vec2(-10.0f, 0.0f), b2Vec2(0.0f, -2.5f));
		ground->CreateFixture(&shape, 0.0f);

		shape.SetAsEdge(b2Vec2(0.0f, -2.5f), b2Vec2(10.0f, 0.f));
		ground->CreateFixture(&shape, 0.0f);
	}
}

Simulator::~Simulator()
{
	// By deleting the world, we delete the bomb, mouse joint, etc.
	delete m_world;
	m_world = NULL;
	for (unsigned int i = 0; i < _templates.size(); i++) {
		delete _templates[i];
	}
}

void Simulator::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	const b2Manifold* manifold = contact->GetManifold();

	if (manifold->pointCount == 0)
	{
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
			if (inside)
			{
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

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position.Set(RandomFloat(-2.f, 2.f), 3);

	b2Body* body = m_world->CreateBody(&bd);

	int index;

	if (key == HGEK_1) {
		index = 0;
	} else if (key == HGEK_2) {
		index = 1;
	} else if (key == HGEK_3) {
		index = 2;
	} else if (key == HGEK_4) {
		index = 3;
	} else {
		m_world->DestroyBody(body);
		return;
	}

	b2FixtureDef fd;
	fd.restitution = _templates[index]->_restitution;
	fd.friction = _templates[index]->_friction;
	fd.density = 1.0f;

	if (_templates[index]->_shape == "circle") {
		b2CircleShape shape;
		shape.m_radius = _templates[index]->_radius;
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else if (_templates[index]->_shape == "box") {
		b2PolygonShape shape;
		shape.SetAsBox(_templates[index]->_width, _templates[index]->_height);
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else {
		m_world->DestroyBody(body);
		return;
	}

	body->SetUserData((void *)&(_templates[index]->_type));
	bodyes.push_back(body);
}

void Simulator::OnMouseDown(hgeVector mousePos)
{
/*	b2Vec2 p(mousePos.x, mousePos.y); // нужен дл€ выбора объекта по которому кликнули
	m_mouseWorld = p;
	
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
		b2MouseJointDef md;
		md.bodyA = m_groundBody;
		md.bodyB = body;
		md.target = p;
		md.maxForce = 1000.0f * body->GetMass();
		m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
		body->SetAwake(true);
	}*/
}

void Simulator::OnMouseUp()
{
}

void Simulator::OnMouseMove(hgeVector mousePos)
{
	b2Vec2 p(mousePos.x, mousePos.y);
	lastMousePos = mousePos;
	m_mouseWorld = p;
	
	if (m_mouseJoint)
	{
	//	m_mouseJoint->SetTarget(p);
	}
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

inline void Simulator::DrawElement(hgeVertex *&buf, const UV *uv, const FPoint2D &pos, float angle, float size) {
	float x =   _viewScale * pos.x + _worldCenter.x;
	float y = - _viewScale * pos.y + _worldCenter.y;
	size = size * _viewScale / sqrt(2.f);
	// пока предполагаем, что все элементы имеют квадратные текстуры
	float vs = size * sin(angle + M_PI / 4);
	float vc = size * cos(angle + M_PI / 4);
	buf[0].x = x - vs; buf[0].y = y + vc; buf[0].tx = uv[0].u; buf[0].ty = uv[0].v; buf[0].col = 0xFFFFFFFF;
	buf[1].x = x + vc; buf[1].y = y + vs; buf[1].tx = uv[1].u; buf[1].ty = uv[1].v; buf[1].col = 0xFFFFFFFF;
	buf[2].x = x + vs; buf[2].y = y - vc; buf[2].tx = uv[2].u; buf[2].ty = uv[2].v; buf[2].col = 0xFFFFFFFF;
	buf[3].x = x - vc; buf[3].y = y - vs; buf[3].tx = uv[3].u; buf[3].ty = uv[3].v; buf[3].col = 0xFFFFFFFF;
	buf += 4;
}

void Simulator::Draw() {
	int max;
	hgeVertex *buffer = Core::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
	unsigned int counter = 0;
	for (unsigned int i = 0; i < bodyes.size(); i++) {
		const b2Transform& xf = bodyes[i]->GetTransform();
		for (b2Fixture* f = bodyes[i]->GetFixtureList(); f; f = f->GetNext()){
			if (f->GetType() == b2Shape::e_circle) {
				b2CircleShape* circle = (b2CircleShape*)f->GetShape();

				b2Vec2 center = b2Mul(xf, circle->m_p);
				float32 radius = circle->m_radius;
				b2Vec2 axis = xf.R.col1;

				//_ball->RenderEx(center.x, center.y, -atan2(axis.y, axis.x), 20.f / 64);
				DrawElement(buffer, _ball, FPoint2D(center.x, center.y), -atan2(axis.y, axis.x), 2 * radius);
			} else if (f->GetType() == b2Shape::e_polygon) {
				/*b2PolygonShape* box = (b2PolygonShape*)f->GetShape();

				b2Vec2 center = b2Mul(xf, box->m_p);
				float32 radius = circle->m_radius;
				b2Vec2 axis = xf.R.col1;
				*/
				//_ball->RenderEx(10*center.x + 400.f, 400.f - 10*center.y, -atan2(axis.y, axis.x), 20.f / 64);
			}
		}
		b2Vec2 position = 10 * bodyes[i]->GetPosition();
		position.x += 400.f;
		position.y = 400 - position.y;
		float32 angle = - bodyes[i]->GetAngle();
		BodyTemplate::Types type = *(BodyTemplate::Types *)(bodyes[i]->GetUserData());
		switch (type) {
			//case BodyTemplate::TYPE_UNBREAKABLE: {_wall->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
			//case BodyTemplate::TYPE_BLUE: {_blue_box->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
			//case BodyTemplate::TYPE_EXPLOSION: {_red_box->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
			//case BodyTemplate::TYPE_BALL: {_ball->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
		};
		++counter;
		if (counter > max) {
			assert(false);
		}
	}
	Core::GetDC()->Gfx_FinishBatch(counter);
}

void Simulator::Update(float deltaTime) {	
	settings.hz = 60;
	Step(&settings);
}