#include "Simulator.h"
#include "..\Core\Core.h"
#include <cstdio>
#define LEVELS_FILE "data.xml"

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
		_collection.push_back(new BodyTemplate(bodyDef));
		bodyDef = bodyDef->NextSiblingElement();
	}
}

Simulator::Simulator(TiXmlElement *xe)
	: _viewScale(45.f)
	, _worldCenter(240.f, 290.f)
	, _angleMultiplier(BodyTemplate::MAX / (M_PI * 2))
	, Messager("simulator")
	, _editor(false)
	, _selectedBody(NULL)
	, _signal(0.f)
	, _doc(LEVELS_FILE)
	, _currentLevel("")
	, _waitState(WaitNone)
{
	if (!_doc.LoadFile()) {
		MessageBox(0, "Levels file not found!", "Error!", MB_OK | MB_ICONERROR);
	} 

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

	/*{
		b2BodyDef bd;
		m_groundBody = m_world->CreateBody(&bd);
		m_groundBody->SetUserData(&_groundType);

		b2PolygonShape shape;
		//shape.SetAsEdge(b2Vec2(-5.0f, 0.0f), b2Vec2(5.0f, 0.f));
		shape.SetAsBox(10.f, 1.f, b2Vec2(0.f, 0.f), 0.f);
		m_groundBody->CreateFixture(&shape, 0.0f);

		//shape.SetAsEdge(b2Vec2(-5.0f, 5.f), b2Vec2(-5.0f, 0.f));
		shape.SetAsBox(1.f, 5.f, b2Vec2(-5.f, 2.5f), 0.f);
		m_groundBody->CreateFixture(&shape, 0.0f);

		//shape.SetAsEdge(b2Vec2(5.0f, 5.f), b2Vec2(5.0f, 0.f));
		shape.SetAsBox(1.f, 5.f, b2Vec2(5.f, 2.5f), 0.f);
		m_groundBody->CreateFixture(&shape, 0.0f);
	}*/
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

	/*b2BodyDef bd; но ДЛЯ ИГРЫ тут можно соптимизировать - все неподвижные элементы вынести в одно body
	m_groundBody = m_world->CreateBody(&bd);
	m_groundBody->SetUserData(&_groundType);

	b2PolygonShape shape;
	//shape.SetAsEdge(b2Vec2(-5.0f, 0.0f), b2Vec2(5.0f, 0.f));
	shape.SetAsBox(10.f, 1.f, b2Vec2(0.f, 0.f), 0.f);
	m_groundBody->CreateFixture(&shape, 0.0f);*/ 


b2Body * Simulator::AddElement(BodyTypes type){ 

	b2BodyDef bd;
	if (type == BODY_TYPE_GROUND) {
		//bd.type = b2_staticBody;);// ХАК - QueryAABB не работает со статичными объктами, 
		bd.type = b2_dynamicBody;   // т.е. иначе их нельзя будет двигать в редакторе
	} else {
		bd.type = b2_dynamicBody;
	}
	bd.position.Set(RandomFloat(-2.f, 2.f), 3);
	b2Body* body = m_world->CreateBody(&bd);

	int index = 0;
	while (_collection[index]->_type != type && index < _collection.size()) {++index;};
	if (index >= _collection.size()) {
		m_world->DestroyBody(body);
		return NULL;
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
		return NULL;
	}

	body->SetUserData(_collection[index]);
	return body;
}

void Simulator::OnDoubleClick(hgeVector mousePos) {
	if (_editor){
		return;
	}
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(1.f / _viewScale * (mousePos.x - _worldCenter.x), 1.f / _viewScale * (_worldCenter.y - mousePos.y)); // нужен для выбора объекта по которому кликнули
	
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
		if (bt->_type == BODY_TYPE_EXPLOSION) {
			m_world->DestroyBody(body);
			Explosion(body->GetPosition(), bt->_radius, bt->_maxForce);
		}
	}
}

void Simulator::OnMouseDown(hgeVector mousePos)
{	
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
		BodyTemplate *bt = (BodyTemplate *)(body->GetUserData());
		if (_editor) {
			InitParams(body);

			//if (Core::GetDC()->Input_GetKeyState(HGEK_ALT)) {
			//	m_world->DestroyBody(body);
			//}
			return;
		}
		if (bt->_type == BODY_TYPE_EXPLOSION) {
//			m_world->DestroyBody(body);
//			Explosion(body->GetPosition(), bt->_radius, bt->_maxForce);
//		} else {
			/*b2MassData m;
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
			body->SetAwake(true);*/
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

void Simulator::InitParams(b2Body *body) 
{
	_selectedBody = body;
	if (body == NULL) {
		SendMessage("radio", "clear");
		SetValue("sliderpanel", "visible", false);
		return;
	} if (body != NULL) {
		const BodyTemplate *bt = (BodyTemplate *)(body->GetUserData());
		if (bt->_type == BODY_TYPE_GROUND) {
			SendMessage("radio", "clear");
			SendMessage("radio", "add angle");
			SendMessage("radio", "add size");
			SetValue("sliderpanel", "visible", true);
		} else {
			SendMessage("radio", "clear");
			SendMessage("radio", "add angle");
			SetValue("sliderpanel", "visible", true);
		}
	}
}

void Simulator::OnMouseMove(hgeVector mousePos)
{
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(1.f / _viewScale * (mousePos.x - _worldCenter.x), 1.f / _viewScale * (_worldCenter.y - mousePos.y)); // нужен для выбора объекта по которому кликнули

	if (m_mouseJoint) {
		m_mouseJoint->SetTarget(p);
	} else if (_editor && Core::GetDC()->Input_GetKeyState(HGEK_LBUTTON) && _selectedBody) {
		b2Vec2 pos = _selectedBody->GetPosition();
		pos += 1.f / _viewScale * b2Vec2(mousePos.x - _lastMousePos.x, _lastMousePos.y - mousePos.y);
		_selectedBody->SetTransform(pos, _selectedBody->GetAngle());
	} else if (false) {// если в режиме удаления - пока не реализован
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
	} else if (Core::GetDC()->Input_GetKeyState(HGEK_LBUTTON)) {
		_worldCenter += (mousePos - _lastMousePos);
	}
	_lastMousePos = mousePos;
	m_mouseWorld = p;
}

bool Simulator::OnMouseWheel(int direction) {
	if (direction > 0) {
		_viewScale *= 1.09 * direction;
	} else if (direction < 0) {
		_viewScale *= 0.9 * abs(direction);
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
	/*for(b2Fixture *f = m_groundBody->GetFixtureList(); f; f = f->GetNext()) {
		b2PolygonShape *s = (b2PolygonShape *)f->GetShape();
		DrawLine(s->GetVertex(0), s->GetVertex(1));
	}*/

	int max;
	hgeVertex *buffer = Core::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
	unsigned int counter = 0;
	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		const b2Transform & xf = body->GetTransform();
		/*int type = *(int *)(body->GetUserData());
		if (type == BODY_TYPE_GROUND) {
			continue;
		}*/
		const BodyTemplate *bt = (BodyTemplate *)(body->GetUserData());
		int angle = round(xf.GetAngle() * _angleMultiplier + BodyTemplate::MAX) % BodyTemplate::MAX;
		assert(0 <= angle && angle <= BodyTemplate::MAX);
		DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
		/*switch (bt->_type) {
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
		};*/
		
		++counter;
		if (counter > max) {
			assert(false);
		}
	}
	Core::GetDC()->Gfx_FinishBatch(counter);
	if (_selectedBody && _signal > 0.5f) {
		max = 1;
		hgeVertex *buffer = Core::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_ALPHAADD | BLEND_COLORADD, &max);
		const BodyTemplate *bt = (BodyTemplate *)(_selectedBody->GetUserData());
		const b2Transform & xf = _selectedBody->GetTransform();
		int angle = round(xf.GetAngle() * _angleMultiplier + BodyTemplate::MAX) % BodyTemplate::MAX;
		assert(0 <= angle && angle <= BodyTemplate::MAX);
		DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
		Core::GetDC()->Gfx_FinishBatch(1);
	}
}

void Simulator::Update(float deltaTime) {	
	if (_editor) {
		_signal += 2 * deltaTime;
		while (_signal > 1.f) {
			_signal -= 1.f;
		}
		return;
	}
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

void Simulator::SaveState() {
	_state.clear();
	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		const b2Transform & xf = body->GetTransform();
		const BodyTemplate *bt = (BodyTemplate *)(body->GetUserData());
		if (bt->_type == BODY_TYPE_GROUND) {
			body->SetType(b2_staticBody);// ХАК - QueryAABB не работает со статичными объктами, 
										 // т.е. иначе их нельзя будет двигать в редакторе
		}
		BodyState s;
		s.type = bt->_type;
		s.angle = xf.GetAngle();
		s.pos = xf.position;
		_state.push_back(s);
	}
}

void Simulator::ResetState() {
	InitParams(NULL);
	_editor = true;
	for (b2Body *body = m_world->GetBodyList(); body; ) {
		b2Body *remove = body;
		/*b2Body *remove = NULL;
		BodyTypes type = *(BodyTypes *)(body->GetUserData());
		if (type != BODY_TYPE_GROUND) {
			remove = body;
		} else {
			body->SetType(b2_dynamicBody);// ХАК - QueryAABB не работает со статичными объктами, 
										  // т.е. иначе их нельзя будет двигать в редакторе
		}*/
		body = body->GetNext();
		if (remove) {
			m_world->DestroyBody(remove);
		}
	}
	for (unsigned int i = 0; i < _state.size(); i++) {
		b2Body *b = AddElement(_state[i].type);
		b->SetTransform(_state[i].pos, _state[i].angle);
	}
}

void Simulator::OnMessage(const std::string &message) {
	std::string msg;
	if (message == "play") {
		if (_editor) {
			SetValue("big", "visible", false);
			SetValue("small", "visible", true);
			_editor = false;
			InitParams(NULL);
			SaveState();
		} else {
			ResetState();
		}
	} else if (CanCut(message, "add ", msg)) {		
		if (!_editor) {
			ResetState();
		}
		if (msg == "wall") {
			InitParams(AddElement(BODY_TYPE_UNBREAKABLE));
		} else if (msg == "explosion") {
			InitParams(AddElement(BODY_TYPE_EXPLOSION));
		} else if (msg == "box") {
			InitParams(AddElement(BODY_TYPE_BLUE));
		} else if (msg == "ball") {
			InitParams(AddElement(BODY_TYPE_BALL));
		} else if (msg == "ground") {
			InitParams(AddElement(BODY_TYPE_GROUND));
		}
	} else if (message == "left") {
	} else if (message == "right") {
	} else if (message == "up") {
	} else if (message == "down") {
	} else if (message == "new") {
		if (MessageBox(0, "Delete all objects?", "Are you sure?", MB_YESNO | MB_ICONQUESTION) == IDYES) {
			InitParams(NULL);
			_currentLevel = "";
			_state.clear();
			for (b2Body *body = m_world->GetBodyList(); body; ) {
				b2Body *remove = body;
				body = body->GetNext();
				m_world->DestroyBody(remove);
			}
		}
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
		if (_waitState == WaitForLevelOpen) {
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
			TiXmlElement *elem = xe->FirstChildElement();
			while (elem != NULL) {
				Simulator::BodyState s;
				s.type = static_cast<BodyTypes>(atoi(elem->Attribute("type")));			
				s.pos.x = atof(elem->Attribute("x"));
				s.pos.y = atof(elem->Attribute("y"));
				s.angle = atof(elem->Attribute("angle"));
				_state.push_back(s);
				elem = elem->NextSiblingElement();
			}
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
			if (xe == NULL) {
				int save_as_new = atoi(s.c_str()) + 1;
				char buff[10];
				sprintf(buff, "%i", save_as_new);
				TiXmlElement *level = new TiXmlElement("level");
				level->SetAttribute("id", buff);
				_doc.RootElement()->LinkEndChild(level);
				xe = level; 
				msg = buff;
			} else if (_currentLevel != msg) {
				std::string s = "Do you want overwrite " + msg + "?";
				if (MessageBox(0, s.c_str(), "Are you sure?", MB_YESNO | MB_ICONQUESTION) != IDYES) {
					return;
				}
			}
			// level saving
			SaveState();
			TiXmlElement *elem = xe->FirstChildElement();
			while (elem != NULL) {
				TiXmlElement *remove = elem;
				elem = elem->NextSiblingElement();
				xe->RemoveChild(remove);
			}
			for (unsigned int i = 0; i < _state.size(); i++) {
				TiXmlElement *elem = new TiXmlElement("element");
				elem->SetAttribute("type", static_cast<int>(_state[i].type));
				char s[16];
				sprintf(s, "%f", _state[i].pos.x); elem->SetAttribute("x", s);
				sprintf(s, "%f", _state[i].pos.y); elem->SetAttribute("y", s);
				sprintf(s, "%f", _state[i].angle); elem->SetAttribute("angle", s);
				xe->LinkEndChild(elem);
			}
			_doc.SaveFile();
			_currentLevel = msg;
		} else {
			assert(false);
		}
	}
}