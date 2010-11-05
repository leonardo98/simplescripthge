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
{
	_blue_box = Core::getTexture("blue_box");
	_red_box = Core::getTexture("red_box");
	_ball = Core::getTexture("ball");
	_wall = Core::getTexture("wall");

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
	//m_world->SetDebugDraw(&//m_debugDraw);
	
	m_bombSpawning = false;

	m_stepCount = 0;

	b2BodyDef bodyDef;
	m_groundBody = m_world->CreateBody(&bodyDef);

	{
		b2BodyDef bd;
		b2Body* ground = m_world->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(20.0f, -10.0f));
		ground->CreateFixture(&shape, 0.0f);

		shape.SetAsEdge(b2Vec2(20.0f, -10.0f), b2Vec2(60.0f, 4.0f));
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

/*void Simulator::DrawTitle(int x, int y, const char *string)
{
    //m_debugDraw.DrawString(x, y, string);
}*/

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
	bd.position.Set(rand()%40 - 20, 30);

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

	//B2_NOT_USED(key); 
	/*b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.active = true;
	bd.allowSleep = false;
	bd.position = b2Vec2(rand()%40 - 20, 30);
	b2Body* body = m_world->CreateBody(&bd);
	b2Fixture 
	if (key == HGEK_1) {
		body->CreateFixture(&_templates[0]->_fixtureDef);
		body->SetUserData(&_templates[0]->_fixtureDef.userData);
	} else if (key == HGEK_2) {
		body->CreateFixture(&_templates[1]->_fixtureDef);
		body->SetUserData(&_templates[1]->_fixtureDef.userData);
	} else if (key == HGEK_3) {
		body->CreateFixture(&_templates[2]->_fixtureDef);
		body->SetUserData(&_templates[2]->_fixtureDef.userData);
	} else if (key == HGEK_4) {
		body->CreateFixture(&_templates[3]->_fixtureDef);
		body->SetUserData(&_templates[3]->_fixtureDef.userData);
	} else {
		m_world->DestroyBody(body);
		return;
	}
	bodyes.push_back(body);*/
}

void Simulator::OnMouseDown(hgeVector mousePos)
{
	b2Vec2 p(mousePos.x, mousePos.y);
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
	}
}

/*void Simulator::SpawnBomb(const b2Vec2& worldPt)
{
	m_bombSpawnPoint = worldPt;
	m_bombSpawning = true;
}
    
void Simulator::CompleteBombSpawn(const b2Vec2& p)
{
	if (m_bombSpawning == false)
	{
		return;
	}

	const float multiplier = 30.0f;
	b2Vec2 vel = m_bombSpawnPoint - p;
	vel *= multiplier;
	LaunchBomb(m_bombSpawnPoint,vel);
	m_bombSpawning = false;
}*/

void Simulator::ShiftMouseDown(const b2Vec2& p)
{
	m_mouseWorld = p;
	
	if (m_mouseJoint != NULL)
	{
		return;
	}

	//SpawnBomb(p); - нужная штука
}

void Simulator::OnMouseUp()
{
	if (m_mouseJoint)
	{
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}
	return;
	if (m_bombSpawning)
	{
		//CompleteBombSpawn(lastMousePos);
	}
	{ // тестируем с пирамидой
		for (unsigned int i = 0; i < bodyes.size(); i++) {
			m_world->DestroyBody(bodyes[i]);
		}
		bodyes.clear();
		/*{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);

			b2PolygonShape shape;
			shape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
			ground->CreateFixture(&shape, 0.0f);
		}*/

		{
			float32 a = 0.5f;
			b2PolygonShape shape;
			shape.SetAsBox(a, a);

			b2Vec2 x(-7.0f, 0.75f);
			b2Vec2 y;
			b2Vec2 deltaX(0.5625f, 1.25f);
			b2Vec2 deltaY(1.125f, 0.0f);
			int e_count = 10;
			for (int32 i = 0; i < e_count; ++i)
			{
				y = x;

				for (int32 j = i; j < e_count; ++j)
				{
					b2BodyDef bd;
					bd.type = b2_dynamicBody;
					bd.position = y;
					b2Body* body = m_world->CreateBody(&bd);
					body->CreateFixture(&shape, 5.0f);
					bodyes.push_back(body);
					y += deltaY;
				}

				x += deltaX;
			}
		}
		{
			b2CircleShape shape;
			shape.m_radius = 1.0f;

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 1.0f;

			float32 restitution[7] = {0.0f, 0.1f, 0.3f, 0.5f, 0.75f, 0.9f, 1.0f};

			for (int32 i = 0; i < 7; ++i)
			{
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-10.0f + 3.0f * i, 20.0f);

				b2Body* body = m_world->CreateBody(&bd);
				bodyes.push_back(body);

				fd.restitution = restitution[i];
				body->CreateFixture(&fd);
			}
		}
	}
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

/*void Simulator::LaunchBomb()
{
	b2Vec2 p(RandomFloat(-15.0f, 15.0f), 30.0f);
	b2Vec2 v = -5.0f * p;
	LaunchBomb(p, v);
}

void Simulator::LaunchBomb(const b2Vec2& position, const b2Vec2& velocity)
{
	if (m_bomb)
	{
		m_world->DestroyBody(m_bomb);
		m_bomb = NULL;
	}

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = position;
	bd.bullet = true;
	m_bomb = m_world->CreateBody(&bd);
	m_bomb->SetLinearVelocity(velocity);
	
	b2CircleShape circle;
	circle.m_radius = 0.3f;

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = 20.0f;
	fd.restitution = 0.0f;
	
	b2Vec2 minV = position - b2Vec2(0.3f,0.3f);
	b2Vec2 maxV = position + b2Vec2(0.3f,0.3f);
	
	b2AABB aabb;
	aabb.lowerBound = minV;
	aabb.upperBound = maxV;

	m_bomb->CreateFixture(&fd);
}*/

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

		////m_debugDraw.DrawString(5, m_textLine, "****PAUSED****");
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

	//m_world->DrawDebugData();

	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}

	if (settings->drawStats)
	{
		//m_debugDraw.DrawString(5, m_textLine, "bodies/contacts/joints/proxies = %d/%d/%d",
		//	m_world->GetBodyCount(), m_world->GetContactCount(), m_world->GetJointCount(), m_world->GetProxyCount());
		m_textLine += 15;
	}

	if (m_mouseJoint)
	{
		/*b2Vec2 p1 = m_mouseJoint->GetAnchorB();
		b2Vec2 p2 = m_mouseJoint->GetTarget();

		glPointSize(4.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_POINTS);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glEnd();
		glPointSize(1.0f);

		glColor3f(0.8f, 0.8f, 0.8f);
		glBegin(GL_LINES);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glEnd();*/
	}
	
	if (m_bombSpawning)
	{
		/*glPointSize(4.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_POINTS);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(m_bombSpawnPoint.x, m_bombSpawnPoint.y);
		glEnd();
		
		glColor3f(0.8f, 0.8f, 0.8f);
		glBegin(GL_LINES);
		glVertex2f(m_mouseWorld.x, m_mouseWorld.y);
		glVertex2f(m_bombSpawnPoint.x, m_bombSpawnPoint.y);
		glEnd();*/
	}

	if (settings->drawContactPoints)
	{
		//const float32 k_impulseScale = 0.1f;
		const float32 k_axisScale = 0.3f;

		for (int32 i = 0; i < m_pointCount; ++i)
		{
			ContactPoint* point = m_points + i;

			if (point->state == b2_addState)
			{
				// Add
				//m_debugDraw.DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));
			}
			else if (point->state == b2_persistState)
			{
				// Persist
				//m_debugDraw.DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));
			}

			if (settings->drawContactNormals == 1)
			{
				b2Vec2 p1 = point->position;
				b2Vec2 p2 = p1 + k_axisScale * point->normal;
				//m_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.9f));
			}
			else if (settings->drawContactForces == 1)
			{
				//b2Vec2 p1 = point->position;
				//b2Vec2 p2 = p1 + k_forceScale * point->normalForce * point->normal;
				//DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
			}

			if (settings->drawFrictionForces == 1)
			{
				//b2Vec2 tangent = b2Cross(point->normal, 1.0f);
				//b2Vec2 p1 = point->position;
				//b2Vec2 p2 = p1 + k_forceScale * point->tangentForce * tangent;
				//DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
			}
		}
	}
}

bool Simulator::IsMouseOver(hgeVector mousePos) {
	return true;
}

void Simulator::Draw() {	
	for (unsigned int i = 0; i < bodyes.size(); i++) {
		const b2Transform& xf = bodyes[i]->GetTransform();
		for (b2Fixture* f = bodyes[i]->GetFixtureList(); f; f = f->GetNext()){
			if (f->GetType() == b2Shape::e_circle) {
				b2CircleShape* circle = (b2CircleShape*)f->GetShape();

				b2Vec2 center = b2Mul(xf, circle->m_p);
				float32 radius = circle->m_radius;
				b2Vec2 axis = xf.R.col1;

				_ball->RenderEx(10*center.x + 400.f, 400.f - 10*center.y, -atan2(axis.y, axis.x), 20.f / 64);
			}
		}
		b2Vec2 position = 10 * bodyes[i]->GetPosition();
		position.x += 400.f;
		position.y = 400 - position.y;
		float32 angle = - bodyes[i]->GetAngle();
		BodyTemplate::Types type = *(BodyTemplate::Types *)(bodyes[i]->GetUserData());
		switch (type) {
			case BodyTemplate::TYPE_UNBREAKABLE: {_wall->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
			case BodyTemplate::TYPE_BLUE: {_blue_box->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
			case BodyTemplate::TYPE_EXPLOSION: {_red_box->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
			//case BodyTemplate::TYPE_BALL: {_ball->RenderEx(position.x, position.y, angle, 20.f / 64); break;}
		};
	}
}

void Simulator::Update(float deltaTime) {	
	settings.hz = 60;
	Step(&settings);
}