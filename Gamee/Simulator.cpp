#include "Simulator.h"
#include "..\Core\Core.h"
#include "..\Core\Render.h"
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
		Render::ShowMessage("Levels file not found!", "Error!");
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
	if (btA->_type != BODY_TYPE_BLUE && btB->_type != BODY_TYPE_BLUE)
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
		if (btA->_type == BODY_TYPE_BLUE)
		{
			static_cast<MyBody *>(contact->GetFixtureA()->GetBody()->GetUserData())->broken = true;
		}
		if (btB->_type == BODY_TYPE_BLUE)
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
	delete static_cast<MyBody *>(body->GetUserData());
	m_world->DestroyBody(body);
}

void Simulator::EraseAllBodyes() {
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
			EraseBody(remove);
		}
	}
}

// добавляем новый элемент в "случайное" место на эфране
b2Body * Simulator::AddElement(BodyTypes type) {
	int index = 0;
	while (_collection[index]->_type != type && index < _collection.size()) {index++;}
	assert(index < _collection.size());

	BodyState bs;
	bs.base = _collection[index];	
	bs.angle = 0;
	bs.pos = b2Vec2(RandomFloat(-2.f, 2.f), 3);
	bs.width = bs.base->_width;
	
	return AddElement(bs);
}

// ДЛЯ ИГРЫ тут можно соптимизировать - все элементы "земли" вынести в одно body
// создаем физическое тело по описанию
b2Body * Simulator::AddElement(const BodyState &bodyState){ 

	b2BodyDef bd;
	float width;
	if (bodyState.base->_type == BODY_TYPE_GROUND) {
		//bd.type = b2_staticBody;);// ХАК - QueryAABB не работает со статичными объктами, 
									// т.е. иначе их нельзя будет двигать в редакторе
		bd.type = b2_dynamicBody;  
		width = bodyState.width;
	} else {
		bd.type = b2_dynamicBody;
		width = bodyState.base->_width;
	}
	bd.position.Set(bodyState.pos.x, bodyState.pos.y);
	bd.angle = bodyState.angle;
	
	b2Body* body = m_world->CreateBody(&bd);
	
	b2FixtureDef fd;
	fd.restitution = bodyState.base->_restitution;
	fd.friction = bodyState.base->_friction;
	fd.density = 1.0f;

	if (bodyState.base->_shape == "circle") {
		b2CircleShape shape;
		shape.m_radius = bodyState.base->_radius;
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else if (bodyState.base->_shape == "box") {
		b2PolygonShape shape;
		shape.SetAsBox(width / 2.f, bodyState.base->_height / 2.f);
		fd.shape = &shape;
		body->CreateFixture(&fd);
	} else {
		EraseBody(body);
		return NULL;
	}
	MyBody *myBody = new MyBody();
	myBody->base = bodyState.base;
	myBody->broken = false;
	if (bodyState.base->_type == BODY_TYPE_GROUND) {
		myBody->width = bodyState.width;		
	}
	body->SetUserData(myBody);
	return body;
}

void Simulator::OnMouseDown(FPoint2D mousePos)
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
		BodyTemplate *bt = static_cast<MyBody *>(body->GetUserData())->base;
		if (_editor) {
			InitParams(body);
			return;
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

	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		BodyTemplate *bt = static_cast<MyBody *>(body->GetUserData())->base;
		if (bt->_type == BODY_TYPE_EXPLOSION) {
			EraseBody(body);
			Explosion(body->GetPosition(), bt->_radius, bt->_maxForce);
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
		if (bt->_type == BODY_TYPE_GROUND) {
			SendMessage("radio", "clear");
			SendMessage("radio", "add del");
			SendMessage("radio", "add angle");
			SendMessage("radio", "add size");
			SetValueB("sliderpanel", "visible", true);
			// set up angle to normal
			float angle = body->GetAngle();
			while (angle > 0.f) {angle -= (2 * M_PI);} 
			while (angle < 0.f) {angle += (2 * M_PI);} 
			SetValueF("slider", "", angle / (2 * M_PI) );
		} else {
			SendMessage("radio", "clear");
			SendMessage("radio", "add del");
			SendMessage("radio", "add angle");
			SetValueB("sliderpanel", "visible", true);
			float angle = body->GetAngle();
			while (angle > 0.f) {angle -= (2 * M_PI);} 
			while (angle < 0.f) {angle += (2 * M_PI);} 
			SetValueF("slider", "", angle / (2 * M_PI) );
		}
	} else if (body != NULL && body == _selectedBody) {
		const MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
		int radio = GetNumberValue("radio", "");
		if (radio == 0) {
			if (Render::ShowAskMessage("Delete selected object?", "Are you sure?")) {
				EraseBody(_selectedBody);
				InitParams(NULL);
			}
		} else if (radio == 1) {
			// set up angle to normal
			float angle = body->GetAngle();
			while (angle > 0.f) {angle -= (2 * M_PI);} 
			while (angle < 0.f) {angle += (2 * M_PI);} 
			SetValueF("slider", "", angle / (2 * M_PI) );
		} else if (radio == 2) {
			assert(myBody->base->_type == BODY_TYPE_GROUND);
			// set up size
			float width = (myBody->width - 0.2f) / 10.f;
			SetValueF("slider", "", width );
		}
	}
}

bool Simulator::CanLevelStart() { // если в уровне есть "динамит" и синие коробки - в него можно играть
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
}

bool Simulator::IsLevelFinish() {
	return _finish != 0x3;
}


void Simulator::OnMouseMove(FPoint2D mousePos)
{
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(1.f / _viewScale * (mousePos.x - _worldCenter.x), 1.f / _viewScale * (_worldCenter.y - mousePos.y)); // нужен для выбора объекта по которому кликнули

	if (m_mouseJoint) {
		m_mouseJoint->SetTarget(p);
	} else if (_editor && Render::IsLeftMouseButton() && _selectedBody) {
		b2Vec2 pos = _selectedBody->GetPosition();
		pos += 1.f / _viewScale * b2Vec2(mousePos.x - _lastMousePos.x, _lastMousePos.y - mousePos.y);
		_selectedBody->SetTransform(pos, _selectedBody->GetAngle());
	} else if (Render::IsLeftMouseButton()) {
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

bool Simulator::IsMouseOver(FPoint2D mousePos) {
	return true;
}

inline void Simulator::DrawElement(Vertex *&buf, const BodyTemplate::UV *uv, const b2Vec2 &pos, const FPoint2D *angles) {
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
	Render::Line(_viewScale * a.x + _worldCenter.x, - _viewScale * a.y + _worldCenter.y, _viewScale * b.x + _worldCenter.x, - _viewScale * b.y + _worldCenter.y, color);
}

void Simulator::Draw() {
	bool oldTnt = (_finish & 0x1) != 0;
	bool oldBlue = (_finish & 0x2) != 0;
	bool tnt = false;
	bool blue = false;
	_finish = 0x0;
	int max;
	Vertex *buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
	unsigned int counter = 0;
	bool exception = false;
	FPoint2D pselect[4];
	std::vector<b2Body *> remove;
	for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		const MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
		if (myBody->broken) {
			remove.push_back(body);
		}
		const BodyTemplate *bt = myBody->base;
		const b2Transform & xf = body->GetTransform();
		int angle = round(xf.GetAngle() * _angleMultiplier + BodyTemplate::MAX) % BodyTemplate::MAX;
		assert(0 <= angle && angle <= BodyTemplate::MAX);

		if (bt->_type != BODY_TYPE_GROUND) {
			if (!tnt && bt->_type == BODY_TYPE_EXPLOSION) {
				tnt = true;
				_finish |= 0x1;
			}
			if (!blue && bt->_type == BODY_TYPE_BLUE) {
				blue = true;
				_finish |= 0x2;
			}
			DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
		} else {
			FPoint2D p[4];
			float width2 = myBody->width / 2;
			float height2 = bt->_height / 2;
			p[0].x = -width2; p[0].y =  height2;
			p[1].x =  width2; p[1].y =  height2;
			p[2].x =  width2; p[2].y = -height2;
			p[3].x = -width2; p[3].y = -height2;

			float angle(-xf.GetAngle());
			p[0] = *p[0].Rotate(angle);
			p[1] = *p[1].Rotate(angle);
			p[2] = *p[2].Rotate(angle);
			p[3] = *p[3].Rotate(angle);
			DrawElement(buffer, bt->_uv, xf.position, p);
			if (_selectedBody == body) {
				exception = true;
				for (unsigned int i = 0; i < 4; i++) { pselect[i] = p[i]; }
			}
		}
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
	Render::GetDC()->Gfx_FinishBatch(counter);
	for (unsigned int i = 0; i < remove.size(); i++) {
		EraseBody(remove[i]);
	}
	if (oldTnt && !tnt) {
		_lastTimer.Init(4.f);
	} else if (oldBlue && !blue) {
		_lastTimer.Init(2.f);
	}
	if (_selectedBody && _signal > 0.5f) {
		max = 1;
		Vertex *buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_ALPHAADD | BLEND_COLORADD, &max);
		const BodyTemplate *bt = static_cast<MyBody *>(_selectedBody->GetUserData())->base;
		const b2Transform & xf = _selectedBody->GetTransform();
		if (exception) {
			DrawElement(buffer, bt->_uv, xf.position, pselect);
		} else {
			int angle = round(xf.GetAngle() * _angleMultiplier + BodyTemplate::MAX) % BodyTemplate::MAX;
			assert(0 <= angle && angle <= BodyTemplate::MAX);
			DrawElement(buffer, bt->_uv, xf.position, bt->_positions[angle]);
		}

		Render::GetDC()->Gfx_FinishBatch(1);
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
			int radio = GetNumberValue("radio", "");
			if (radio == 0) { // удаляем - ничего не делаем в Update
			} else if (radio == 1) { // angle
				float angle = GetNumberValue("slider", "");
				_selectedBody->SetTransform(_selectedBody->GetPosition(), angle * (2 * M_PI));
			} else if (radio == 2 && myBody->base->_type == BODY_TYPE_GROUND) { // size
				float width = GetNumberValue("slider", "") * 10 + 0.2f;
				myBody->width = width;
				// конкретно для земли мы в редакторе точно знаем - один Fixture и один Shape
				static_cast<b2PolygonShape *>(_selectedBody->GetFixtureList()->GetShape())->SetAsBox(myBody->width / 2.f, myBody->base->_height / 2.f);
				_selectedBody->ResetMassData();
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
				Render::ShowMessage("You lose!", "Game over!");
			} else {
				Render::ShowMessage("You win!", "Congratulation!");				
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
		MyBody *myBody = static_cast<MyBody *>(body->GetUserData());
		if (myBody->base->_type == BODY_TYPE_GROUND) {
			body->SetType(b2_staticBody);// ХАК - QueryAABB не работает со статичными объктами, 
										 // т.е. иначе их нельзя будет двигать в редакторе
		}
		BodyState s;
		s.base = myBody->base;
		s.angle = xf.GetAngle();
		s.pos = xf.position;
		s.width = myBody->width;
		_state.push_back(s);
	}
}

void Simulator::ResetState() {
	InitParams(NULL);
	_editor = true;
	EraseAllBodyes();
	for (unsigned int i = 0; i < _state.size(); i++) {
		b2Body *b = AddElement(_state[i]);
		//b->SetTransform(_state[i].pos, _state[i].angle);
	}
}

void Simulator::OnMessage(const std::string &message) {
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
				Render::ShowMessage("Level must have TNT and BLUE boxes!", "Error!");
			}
		} else { // в редактор
			SetValueS("play", "", ">>");
			ResetState();
		}
	} else if (CanCut(message, "add ", msg)) {		
		if (!_editor) {
			SetValueS("play", "", ">>");
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
		if (Render::ShowAskMessage("Delete all objects?", "Are you sure?")) {
			InitParams(NULL);
			_currentLevel = "";
			_state.clear();
			EraseAllBodyes();
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
				BodyTypes type = static_cast<BodyTypes>(atoi(elem->Attribute("type")));

				int index = 0;
				while (_collection[index]->_type != type && index < _collection.size()) {index++;}
				assert(index < _collection.size());
				
				s.base = _collection[index];
				s.pos.x = atof(elem->Attribute("x"));
				s.pos.y = atof(elem->Attribute("y"));
				s.angle = atof(elem->Attribute("angle"));
				if (type == BODY_TYPE_GROUND) {
					//assert(elem->Attribute("width"));
					if (elem->Attribute("width")) {
						s.width = atof(elem->Attribute("width"));
					} else {
						s.width = 1.5f;
					}
				}
				_state.push_back(s);
				elem = elem->NextSiblingElement();
			}
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
				if (Render::ShowAskMessage(s.c_str(), "Are you sure?")) {
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
				elem->SetAttribute("type", static_cast<int>(_state[i].base->_type));
				char s[16];
				sprintf(s, "%f", _state[i].pos.x); elem->SetAttribute("x", s);
				sprintf(s, "%f", _state[i].pos.y); elem->SetAttribute("y", s);
				sprintf(s, "%f", _state[i].angle); elem->SetAttribute("angle", s);
				if (_state[i].base->_type == BODY_TYPE_GROUND) {
					sprintf(s, "%f", _state[i].width); elem->SetAttribute("width", s);
				}
				xe->LinkEndChild(elem);
			}
			_doc.SaveFile();
			_currentLevel = msg;
		} else {
			assert(false);
		}
	}
}