#include "TileEditor.h"
#include "../Helpers/MyMessageBox.h"
#include "../Core/Core.h"
#include "../Core/Render.h"
#include "../Core/Math.h"
#include <cstdio>
#define LEVELS_FILE "levels.xml"
#define DEC 0.01f
#define HALFBORDER 0.0025f


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
	//TiXmlDocument doc(Render::GetDC()->Resource_MakePath(filename.c_str()));
	//if (!doc.LoadFile()) {
	//	LOG("File not found : " + Render::GetDC()->Resource_MakePath(filename.c_str()));
	//	return;
	//}
	//TiXmlElement *bodyDef = doc.RootElement()->FirstChildElement("b2object");
	//while (bodyDef) {
	//	_collection.push_back(new BodyTemplate(bodyDef));
	//	bodyDef = bodyDef->NextSiblingElement("b2object");
	//}
	//_collection.push_back(_fish = new BodyTemplate(doc.RootElement()->FirstChildElement("fish")));
	//_collection.push_back(_cat = new BodyTemplate(doc.RootElement()->FirstChildElement("cat")));
}

TileEditor::TileEditor(TiXmlElement *xe)
	: _viewScale(1.f)
	, _userLevelWin(false)
	, _worldCenter((SCREEN_WIDTH = Render::GetDC()->System_GetState(HGE_SCREENWIDTH)) / 2
	, (SCREEN_HEIGHT = Render::GetDC()->System_GetState(HGE_SCREENHEIGHT)) / 2)
//	, _angleMultiplier(BodyTemplate::MAX / (M_PI * 2))
	, Messager("tile_editor")
	, _editor(false)
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
{
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

TileEditor::~TileEditor()
{
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
	if (body->GetUserData() != NULL) {
		MyBody *t = static_cast<MyBody *>(body->GetUserData());
		delete t;
		body->SetUserData(NULL);
		m_world->DestroyBody(body);
	}
}

void TileEditor::EraseAllBodyes() {
	for (b2Body *body = m_world->GetBodyList(); body; ) {
		b2Body *remove = body;
		body = body->GetNext();
		if (remove) {
			EraseBody(remove);
		}
	}
}

// ��������� ����� ������� � "���������" ����� �� ������
b2Body * TileEditor::AddElement(const std::string &typeId) {
	Collection::iterator index = _collection.begin();
	while (index != _collection.end() && (*index)->_id != typeId ) {index++;}
	assert(index != _collection.end() && (*index)->_id == typeId );

	BodyState bs;
	bs.base = (*index);	
	bs.angle = 0;
	{// ����� ����� ������ 

	}
	bs.pos = b2Vec2((SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale, (_worldCenter.y - SCREEN_HEIGHT / 2) / _viewScale);
	bs.width = bs.base->_width;
	bs.height = bs.base->_height;
	bs.radius = bs.base->_radius;
	
	return AddElement(bs);
}

// ��� ���� ��� ����� ��������������� - ��� �������� "�����" ������� � ���� body
// ������� ���������� ���� �� ��������
b2Body * TileEditor::AddElement(const BodyState &bodyState){ 

	b2BodyDef bd;
	bd.type = b2_dynamicBody;  
	//if (bodyState.base->_fixed) {
		//bd.type = b2_staticBody;);// ��� - QueryAABB �� �������� �� ���������� ��������, 
	//}								// �.�. ����� �� ������ ����� ������� � ���������
									// ������� � ������ ��� ������������� ������ � ������ ����,
									// � � ��������� ������-�������
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
	}

	//b2Vec2 p(fp.x, - fp.y);// ����� ��� ������ ������� �� �������� ��������
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
	b2Vec2 p(fp.x, - fp.y);// ����� ��� ������ ������� �� �������� ��������
	
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
	return true;
	/* // ���� � ������ ���� "�������" � ����� ������� - � ���� ����� ������
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

bool TileEditor::IsLevelFinish() {
	return _userLevelWin;//_finish != 0x3;
}


void TileEditor::OnMouseMove(const FPoint2D &mousePos)
{
	FPoint2D fp = 1.f / _viewScale * (mousePos - _worldCenter);
	b2Vec2 p(fp.x, - fp.y);// ����� ��� ������ ������� �� �������� ��������

	if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT) && _currents.dotIndex >= 0) {
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
	if (direction > 0 && _viewScale < 4.f) {
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
	}

	for (unsigned int i = 0; i < _level.ground.size(); ++i) {
		_level.ground[i]->DrawLines(_worldCenter, _viewScale);
	}

	buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
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
		DrawElement(buffer, bt->_uv, xf.position, p);
		if (_selectedBody == body) {
			for (unsigned int i = 0; i < 4; i++) { pselect[i] = p[i]; }
		}
		++counter;
		if (static_cast<int>(counter) > max) {
			assert(false);
		}
	}
	Render::GetDC()->Gfx_FinishBatch(counter);
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
}

void TileEditor::Update(float deltaTime) {	
	if (_editor) {
		_signal += 2 * deltaTime;
		while (_signal > 1.f) {
			_signal -= 1.f;
		}
		if (_selectedBody != NULL) {
			MyBody *myBody = static_cast<MyBody *>(_selectedBody->GetUserData());
			int radio = static_cast<int>(GetNumberValue("radio", ""));
			if (radio == 0) { // ������� - ������ �� ������ � Update
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
				// ��������� �� � ��������� ����� ����� - ���� Fixture � ���� Shape
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
		Step(&settings);
		if (_startLevel.Action()) {
			_startLevel.Update(deltaTime);
		}
		if (IsLevelFinish()) {
			if (_lastTimer.Action()) {
				_lastTimer.Update(deltaTime);
				if (!_lastTimer.Action()) {
					if (_finish & 0x2) { // �������� �����
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
	//		body->SetType(b2_staticBody);// ��� - QueryAABB �� �������� �� ���������� ��������, 
	//									 // �.�. ����� �� ������ ����� ������� � ���������
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
	} else if (message == "net") {
		_netVisible = !_netVisible;
	} else if (message == "play") {
		if (_editor) { // ��������� � ����� ����
			if (CanLevelStart()) {
				SetValueB("big", "visible", false);
				SetValueB("small", "visible", true);
				SetValueS("play", "", "stop");
				_editor = false;
				InitParams(NULL);
				SaveState();
				_finish = 0x3;
				_startLevel.Init(2.f);
				_userLevelWin = false;
			} else {
				OkMessageShow("Error!\nLevel must have TNT and BLUE boxes!");
			}
		} else { // � ��������
			SetValueS("play", "", ">>");
			ResetState();
		}
	} else if (message == "add new elem") {
		LevelBlock *b = new LevelBlock();
		float cx = (SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale;
		float cy = (SCREEN_HEIGHT / 2 - _worldCenter.y) / _viewScale;
		for (int i = 0; i < 4; ++i) {
			b->AddPoint(cx + 32.f * sin(M_PI / 2 * i), cy + 32.f * cos(M_PI / 2 * i));
		}
		_level.ground.push_back(b);
		//for (Collection::iterator i = _collection.begin(); i != _collection.end(); i++) {
		//	Messager::SendMessage("SmallList", "add " + (*i)->_id);
		//}
		//Messager::SendMessage("SmallList", "special add cancel");
		//_waitAddNewElem = true;
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
	TiXmlElement *word = new TiXmlElement("word");
	word->SetAttribute("x", _worldCenter.x);
	word->SetAttribute("y", _worldCenter.y);
	char s[16];
	sprintf(s, "%f", _viewScale);
	word->SetAttribute("scale", s);
	_saveLevelXml->LinkEndChild(word);

	_doc.SaveFile();
	_currentLevel = levelName;
}


void LevelBlock::DrawLines(const FPoint2D &worldPos, float scale) {
	float t = 0.f;
	float x1 = xPoses.getGlobalFrame(0.f) * scale + worldPos.x;
	float y1 = yPoses.getGlobalFrame(0.f) * scale + worldPos.y;
	float x2, y2;
	int subLine = xPoses.keys.size() * 6;//���������� ������ �������� �� ������� �������� ������ �������
	while (t + 1.f / subLine < 1.f) {
		t += 1.f / subLine;//���������� ������ �������� �� ������� �������� ������ �������
		x2 = xPoses.getGlobalFrame(t) * scale + worldPos.x;
		y2 = yPoses.getGlobalFrame(t) * scale + worldPos.y;
		Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0x4FFFFFFF);
		x1 = x2;
		y1 = y2;
	}
	x2 = xPoses.getGlobalFrame(1.f) * scale + worldPos.x;
	y2 = yPoses.getGlobalFrame(1.f) * scale + worldPos.y;
	Render::GetDC()->Gfx_RenderLine(x1, y1, x2, y2, 0x4FFFFFFF);			

	static const float SIZEX = 3;
	for (unsigned int i = 0; i < xPoses.keys.size() - 1; ++i) {
		float x = xPoses.getFrame(i, 0.f) * scale + worldPos.x;
		float y = yPoses.getFrame(i, 0.f) * scale + worldPos.y;
		Render::GetDC()->Gfx_RenderLine(x - SIZEX, y - SIZEX, x + SIZEX, y + SIZEX, 0xFFFFFFFF);
		Render::GetDC()->Gfx_RenderLine(x - SIZEX, y + SIZEX, x + SIZEX, y - SIZEX, 0xFFFFFFFF);
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
	bool result = false;
	static const float SIZEX = 6;
	FPoint2D p(x, y);

	float t = 0.f;
	FPoint2D one(xPoses.getGlobalFrame(0.f), yPoses.getGlobalFrame(0.f));
	FPoint2D two;
	int subLine = xPoses.keys.size() * 6;//���������� ������ �������� �� ������� �������� ������ �������
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
	TiXmlElement *word = xe->FirstChildElement("word");
	_worldCenter.x = atof(word->Attribute("x"));
	_worldCenter.y = atof(word->Attribute("y"));
	_viewScale = atof(word->Attribute("scale"));
	SetValueS("play", "", ">>");
	//ResetState();
	_currentLevel = msg;
}