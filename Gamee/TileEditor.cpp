#include "TileEditor.h"
#include "../Helpers/MyMessageBox.h"
#include "../Core/Core.h"
#include "../Core/Render.h"
#include "../Core/Math.h"
#include <cstdio>
#define LEVELS_FILE "levels.xml"
#define DEC 0.01f
#define HALFBORDER 0.0025f

Texture *_allElements;


int TileEditor::round(float a) {
	int b = static_cast<int>(a);
	return (a - b) >= 0.5f ? b + 1 : b;
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
}

void TileEditor::LoadRandomLevelSet(const std::string &filename) {
}

TileEditor::TileEditor(TiXmlElement *xe)
	: _viewScale(1.f)
	, _useRandom(false)
	, _scipScaleChanging(0.f)
	, _userLevelWin(false)
	, _screenOffset((SCREEN_WIDTH = Render::GetDC()->System_GetState(HGE_SCREENWIDTH)) / 2
					, (SCREEN_HEIGHT = Render::GetDC()->System_GetState(HGE_SCREENHEIGHT)) / 2  )
	, _worldOffset(0.f, 0.f)
//	, _angleMultiplier(BodyTemplate::MAX / (M_PI * 2))
	, Messager("tile_editor")
	, _editor(true)
	, _selectedBody(NULL)
	, _signal(0.f)
	, _currentLevel("")
	, _mouseDown(false)
	, SLIDER_SCALE(1.2f)
	, SLIDER_MIN(0.2f)
	, _netVisible(true)
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
	//LoadTemplates(Render::GetDataDir() + "bodyes.xml");
	LoadRandomLevelsSet(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + "random.xml").c_str()));

	//b2Vec2 gravity;
	//gravity.Set(0.0f, 10.0f);
	//bool doSleep = true;
	//m_world = new b2World(gravity, doSleep);
	_byker->physic.SetGravity(FPoint2D(0.f, 9.f));

	//m_bomb = NULL;
	//m_textLine = 30;
	//m_mouseJoint = NULL;
	//m_pointCount = 0;

	//m_destructionListener.test = this;
	//m_world->SetDestructionListener(&m_destructionListener);
	//m_world->SetContactListener(this);
	
	//m_bombSpawning = false;
	//m_stepCount = 0;
}

TileEditor::~TileEditor()
{
	EraseAllBodyes();
	delete _byker;
	My::AnimationManager::UnloadAll();
	delete _startFlag;
	delete _endFlag;
	Render::GetDC()->Texture_Free(_flags);
	ClearLevel();
	for (unsigned int i = 0, e = _randomLevelsSet.size(); i < e; ++i) {
		delete _randomLevelsSet[i];
	}
	// By deleting the world, we delete the bomb, mouse joint, etc.
	//delete m_world;
	//m_world = NULL;
	for (Collection::iterator i = _collection.begin(), e = _collection.end(); i != e; i++) {
		delete (*i);
	}
}

//void TileEditor::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
//{
//	const b2Manifold* manifold = contact->GetManifold();
//
//	if (manifold->pointCount == 0) {
//		return;
//	}
//
//	b2Fixture* fixtureA = contact->GetFixtureA();
//	b2Fixture* fixtureB = contact->GetFixtureB();
//
//	b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
//	b2GetPointStates(state1, state2, oldManifold, manifold);
//
//	b2WorldManifold worldManifold;
//	contact->GetWorldManifold(&worldManifold);
//
//	for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
//	{
//		ContactPoint* cp = m_points + m_pointCount;
//		cp->fixtureA = fixtureA;
//		cp->fixtureB = fixtureB;
//		cp->position = worldManifold.points[i];
//		cp->normal = worldManifold.normal;
//		cp->state = state2[i];
//		++m_pointCount;
//	}
//}
//
//void TileEditor::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
//{
//	return;
//
//	if (_startLevel.Action()) {
//		return;
//	}
//	BodyTemplate *btA = static_cast<MyBody *>(contact->GetFixtureA()->GetBody()->GetUserData())->base;
//	BodyTemplate *btB = static_cast<MyBody *>(contact->GetFixtureB()->GetBody()->GetUserData())->base;
//	if (!(btA->_breakable) && !(btB->_breakable))
//	{
//		if ((btA->_hero && btB->_hero)) {
//			_userLevelWin = true;
//		}
//		return;
//	}
//
//	// Should the body break?
//	int32 count = contact->GetManifold()->pointCount;
//
//	float32 maxImpulse = 0.0f;
//	for (int32 i = 0; i < count; ++i)
//	{
//		maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
//	}
//
//	if (maxImpulse > 0.1f) {
//		// Flag the body for breaking.
//		if (btA->_breakable) {
//			static_cast<MyBody *>(contact->GetFixtureA()->GetBody()->GetUserData())->broken = true;
//		}
//		if (btB->_breakable) {
//			static_cast<MyBody *>(contact->GetFixtureB()->GetBody()->GetUserData())->broken = true;
//		}
//	}
//}
//
//class QueryCallback : public b2QueryCallback
//{
//public:
//	QueryCallback(const b2Vec2& point)
//	{
//		m_point = point;
//		m_fixture = NULL;
//	}
//
//	bool ReportFixture(b2Fixture* fixture)
//	{
//		b2Body* body = fixture->GetBody();
//		if (body->GetType() == b2_dynamicBody)
//		{
//			bool inside = fixture->TestPoint(m_point);
//			if (inside) {
//				m_fixture = fixture;
//
//				// We are done, terminate the query.
//				return false;
//			}
//		}
//		// Continue the query.
//		return true;
//	}
//	b2Vec2 m_point;
//	b2Fixture* m_fixture;
//};

void TileEditor::EraseSelected() {
	//if (body->GetUserData() != NULL && body->GetUserData() != _byker) {
	//	MyBody *t = static_cast<MyBody *>(body->GetUserData());
	//	delete t;
	//	body->SetUserData(NULL);
	//}
	//m_world->DestroyBody(body);
}

void TileEditor::EraseAllBodyes() {
	while (_byker->physic.GetAllLines().size()) {
		_byker->physic.RemoveLinesSet(NULL);
	}
	//for (b2Body *body = m_world->GetBodyList(); body; ) {
	//	b2Body *remove = body;
	//	body = body->GetNext();
	//	if (remove) {
	//		EraseBody(remove);
	//	}
	//}
	//_landBodies.clear();
}

// добавляем новый элемент в "случайное" место на экране
void TileEditor::AddElement(const std::string &typeId) {
	//Collection::iterator index = _collection.begin();
	//while (index != _collection.end() && (*index)->_id != typeId ) {index++;}
	//assert(index != _collection.end() && (*index)->_id == typeId );

	//BodyState bs;
	//bs.base = (*index);	
	//bs.angle = 0;
	//{// найдём центр экрана 

	//}
	//bs.pos = b2Vec2((SCREEN_WIDTH / 2 - _worldCenter.x) / _viewScale, (_worldCenter.y - SCREEN_HEIGHT / 2) / _viewScale);
	//bs.pos = b2Vec2(_level.startpoint[0].x / SCALE_BOX2D, _level.startpoint[0].y / SCALE_BOX2D);
	//bs.width = bs.base->_width;
	//bs.height = bs.base->_height;
	//bs.radius = bs.base->_radius;
	//
	//return AddElement(bs);
}
//
//// ДЛЯ ИГРЫ тут можно соптимизировать - все элементы "земли" вынести в одно body
//// создаем физическое тело по описанию
//b2Body * TileEditor::AddElement(const BodyState &bodyState){ 
//
//	b2BodyDef bd;
//	bd.type = b2_dynamicBody;  
//	//if (bodyState.base->_fixed) {
//		//bd.type = b2_staticBody;);// ХАК - QueryAABB не работает со статичными объктами, 
//	//}								// т.е. иначе их нельзя будет двигать в редакторе
//									// поэтому в статик они переключаются только в режиме игры,
//									// а в редакторе псевдо-динамик
//	bd.position.Set(bodyState.pos.x, bodyState.pos.y);
//	bd.angle = bodyState.angle;
//	
//	b2Body* body = m_world->CreateBody(&bd);
//	
//	b2FixtureDef fd;
//	fd.restitution = bodyState.base->_restitution;
//	fd.friction = bodyState.base->_friction;
//	fd.density = 1.0f;
//
//	if (bodyState.base->_shape == "circle") {
//		b2CircleShape shape;
//		shape.m_radius = bodyState.radius + HALFBORDER;
//		fd.shape = &shape;
//		body->CreateFixture(&fd);
//	} else if (bodyState.base->_shape == "box") {
//		b2PolygonShape shape;
//		shape.SetAsBox(bodyState.width / 2.f - DEC + HALFBORDER, bodyState.height / 2.f - DEC + HALFBORDER);
//		fd.shape = &shape;
//		body->CreateFixture(&fd);
//	} else if (bodyState.base->_shape == "triangle") {
//		b2PolygonShape shape;
//		b2Vec2 vec[3];
//		vec[0] = b2Vec2( bodyState.width / 2.f - DEC + HALFBORDER, bodyState.height / 2.f - DEC + HALFBORDER);
//		vec[1] = b2Vec2(-bodyState.width / 2.f + DEC - HALFBORDER, bodyState.height / 2.f - DEC + HALFBORDER);
//		vec[2] = b2Vec2(-bodyState.width / 2.f + DEC - HALFBORDER, -bodyState.height / 2.f + DEC - HALFBORDER);
//		shape.Set(vec, 3);
//		fd.shape = &shape;
//		body->CreateFixture(&fd);
//	} else {
//		EraseBody(body);
//		return NULL;
//	}
//	MyBody *myBody = new MyBody();
//	myBody->base = bodyState.base;
//	myBody->broken = false;
//
//	myBody->width = bodyState.width;		
//	myBody->height = bodyState.height;
//	myBody->radius = bodyState.radius;		
//
//	body->SetUserData(myBody);
//	return body;
//}
//
void TileEditor::OnMouseDown(const FPoint2D &mousePos)
{	
	if (!_editor) {
		FPoint2D imp(0.f, -4.5f);
		_byker->physic.SetImpulse(imp);
		float speedChange = imp.Length() / _byker->physic.GetMass();
		_scipScaleChanging = speedChange / (2 * _byker->physic.GetGravity().Length());
		return;
	}
	_mouseDown = true;
	_lastMousePos = mousePos;
	InitParams(NULL);
	FPoint2D fp = ScreenToWorld(mousePos);
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
			PrefixedAskMessageShow("Are you sure?\nDelete selected object?", "DeleteSelectedYesNo");
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
	FPoint2D newMmouseWorld = ScreenToWorld(mousePos);

	if (_currentElement.selected != SelectedElement::none && _mouseDown) {
		if (_currentElement.selected == SelectedElement::beauty_element) {
			_level.beauties[_currentElement.index].pos.x += (newMmouseWorld.x - _mouseWorld.x);
			_level.beauties[_currentElement.index].pos.y += (newMmouseWorld.y - _mouseWorld.y);
		} else if (_currentElement.selected == SelectedElement::start_flag) {
			_level.startpoint[0].x += (newMmouseWorld.x - _mouseWorld.x);
			_level.startpoint[0].y += (newMmouseWorld.y - _mouseWorld.y);
		} else if (_currentElement.selected == SelectedElement::end_flag) {
			_level.endpoint[0].x += (newMmouseWorld.x - _mouseWorld.x);
			_level.endpoint[0].y += (newMmouseWorld.y - _mouseWorld.y);
		}
	} else if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT) && _currents.dotIndex >= 0) {
		SplinePath tmpx, tmpy;
		float dx = newMmouseWorld.x - _currents.downX;
		float dy = newMmouseWorld.y - _currents.downY;
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
		if (_mouseDown) {
			_worldOffset -= (mousePos - _lastMousePos) / _viewScale;
		}
	}
	_lastMousePos = mousePos;
	_mouseWorld = newMmouseWorld;
}

bool TileEditor::OnMouseWheel(int direction) {
	if (!_editor) {
		return true;
	}
	float old = _viewScale;
	FPoint2D fp = ScreenToWorld(_lastMousePos);
	_screenOffset = _lastMousePos;
	_worldOffset = fp;
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
	_worldOffset = ScreenToWorld(FPoint2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
	_screenOffset = FPoint2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	//_worldOffset += _viewScale / old * (fp - _worldOffset);
	return true;
} 

bool TileEditor::IsMouseOver(const FPoint2D &mousePos) {
	return true;
}

void DrawJoint(b2Joint* joint)
{
	b2Body* bodyA = joint->GetBodyA();
	b2Body* bodyB = joint->GetBodyB();
	const b2Transform& xf1 = bodyA->GetTransform();
	const b2Transform& xf2 = bodyB->GetTransform();
	b2Vec2 x1 = xf1.position;
	b2Vec2 x2 = xf2.position;
	b2Vec2 p1 = joint->GetAnchorA();
	b2Vec2 p2 = joint->GetAnchorB();

	switch (joint->GetType())
	{
	case e_distanceJoint:
		Render::Line(p1.x * SCALE_BOX2D, p1.y * SCALE_BOX2D, p2.x * SCALE_BOX2D, p2.y * SCALE_BOX2D, 0xFFFFFFFF);
		break;

	case e_pulleyJoint:
		{
			b2PulleyJoint* pulley = (b2PulleyJoint*)joint;
			b2Vec2 s1 = pulley->GetGroundAnchorA();
			b2Vec2 s2 = pulley->GetGroundAnchorB();
			Render::Line(s1.x * SCALE_BOX2D, s1.y * SCALE_BOX2D, p1.x * SCALE_BOX2D, p1.y * SCALE_BOX2D, 0xFFFFFFFF);
			Render::Line(s2.x * SCALE_BOX2D, s2.y * SCALE_BOX2D, p2.x * SCALE_BOX2D, p2.y * SCALE_BOX2D, 0xFFFFFFFF);
			Render::Line(s1.x * SCALE_BOX2D, s1.y * SCALE_BOX2D, s2.x * SCALE_BOX2D, s2.y * SCALE_BOX2D, 0xFFFFFFFF);
		}
		break;

	case e_mouseJoint:
		// don't draw this
		break;

	default:
		Render::Line(x1.x * SCALE_BOX2D, x1.y * SCALE_BOX2D, p1.x * SCALE_BOX2D, p1.y * SCALE_BOX2D, 0xFFFFFFFF);
		Render::Line(p1.x * SCALE_BOX2D, p1.y * SCALE_BOX2D, p2.x * SCALE_BOX2D, p2.y * SCALE_BOX2D, 0xFFFFFFFF);
		Render::Line(x2.x * SCALE_BOX2D, x2.y * SCALE_BOX2D, p2.x * SCALE_BOX2D, p2.y * SCALE_BOX2D, 0xFFFFFFFF);
	}
}


void TileEditor::Draw() {
	bool tnt = false;
	bool blue = false;
	_finish = 0x0;
	int max = 1;
	Vertex *buffer;
	if (_editor) { // режим редактора
		Render::PushMatrix();
		Render::MatrixMove(_screenOffset.x, _screenOffset.y);
		Render::MatrixScale(_viewScale, _viewScale);
		Render::MatrixMove(-_worldOffset.x, -_worldOffset.y);
		if (_netVisible) {// текстуры подложка
			for (unsigned int i = 0; i < _level.images.size(); ++i) {
				_level.images[i].sprite->Render(_level.images[i].pos.x, _level.images[i].pos.y);
			}
		}
		// кусты
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
		if (_netVisible) {// сетка
			Render::PushMatrix();
			Render::SetMatrixUnit();
			float STEP = 64.f;
			int n = SCREEN_WIDTH / (_viewScale * STEP);
			float t = -_worldOffset.x / (STEP * _viewScale);
			t = (t - static_cast<int>(t)) * (STEP * _viewScale);
			for (int i = 0; i <= n; i++) {
				float x = i * STEP * _viewScale + t;
				Render::GetDC()->Gfx_RenderLine(x, 0, x, SCREEN_HEIGHT, 0x4FFFFFFF);
			}
			n = SCREEN_HEIGHT / (_viewScale * STEP) + 1;
			t = -_worldOffset.y / (STEP * _viewScale);
			t = (t - static_cast<int>(t)) * (STEP * _viewScale);
			for (int i = 0; i <= n; i++) {
				float y = i * STEP * _viewScale + t;
				Render::GetDC()->Gfx_RenderLine(0, y, SCREEN_WIDTH, y, 0x4FFFFFFF);
			}
			Render::PopMatrix();
			for (unsigned int i = 0; i < _level.ground.size(); ++i) {
				_level.ground[i]->DrawLines();
			}
		} else {// залитая одним цветом земля
			for (unsigned int i = 0; i < _level.ground.size(); ++i) {
				_level.ground[i]->DrawTriangles();
			}
		}
		// флажек старта
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
		// флажек финиша
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
		Render::PopMatrix();
	} else { // режим игры
		// земля
		Render::PushMatrix();
		Render::MatrixMove(_screenOffset.x, _screenOffset.y);
		Render::MatrixScale(_viewScale, _viewScale);
		Render::MatrixMove(-_worldOffset.x, -_worldOffset.y);

		for (Islands::iterator j = _islands.begin(), e = _islands.end(); j != e; ++j) {
			Render::PushMatrix();
			Render::MatrixMove((*j)._lines[0]->GetOffset().x * SCALE_BOX2D, (*j)._lines[0]->GetOffset().y * SCALE_BOX2D);
			// кусты
			for (unsigned int i = 0; i < (*j)._set->beauties.size(); ++i) {
				(*j)._set->beauties[i].Draw();
			}
			for (unsigned int i = 0; i < (*j)._set->ground.size(); ++i) {
				(*j)._set->ground[i]->DrawTriangles();
			}
			Render::PopMatrix();
		}

		//for (LittleHero::AllLines::iterator i = _byker->physic.GetAllLines().begin(), 
		//									e = _byker->physic.GetAllLines().end(); i != e; ++i) {
		//	Render::PushMatrix();
		//	Render::MatrixMove((*i)->GetOffset().x * SCALE_BOX2D, (*i)->GetOffset().y * SCALE_BOX2D);
		//	// кусты
		//	for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
		//		_level.beauties[i].Draw();
		//	}
		//	for (unsigned int i = 0; i < _level.ground.size(); ++i) {
		//		_level.ground[i]->DrawTriangles();
		//	}
		//	Render::PopMatrix();
		//}

		// мотоциклист
		FPoint2D p(_byker->physic.GetPosition() * SCALE_BOX2D);

		Render::PushMatrix();
		Render::MatrixMove(p.x, p.y);
		float angle ;
//		if (_byker->physic.IsGround()) {
//			int spline;
//			float f = _byker->physic.GetSplinePos(spline);
//			float dx = _level.ground[spline]->xPoses.getGlobalGradient(f);
//			float dy = _level.ground[spline]->yPoses.getGlobalGradient(f);
//			angle = atan2(dy, dx);
//		} else {
//		}
		angle = atan2(_byker->physic.GetSpeedVector().y, _byker->physic.GetSpeedVector().x);
		Render::MatrixRotate(angle);
		_byker->SetPos(FPoint2D(0, 0));
		if (Render::GetDC()->Input_GetKeyState(HGEK_CTRL)) {
			Render::Line(0, 0, _byker->physic.GetSpeedVector().x * SCALE_BOX2D, _byker->physic.GetSpeedVector().y * SCALE_BOX2D, 0xFFFFFFFF);
		}
		_byker->Draw(angle);
		Render::PopMatrix();

		// смотрим не пора ли подставить новый кусок
		float x = _endPoint.x;
		float y = _endPoint.y;
		Render::GetCurrentMatrix().Mul(x, y);
		float sx = _startPoint.x;
		float sy = _startPoint.y;
		Render::GetCurrentMatrix().Mul(x, y);
		
		float xByker = _byker->physic.GetPosition().x * SCALE_BOX2D;
		if (sx < xByker && xByker < _endPoint.x) {
			if (_islands.size() >= 3) {
				for (int i = 0; i < _islands.front()._lines.size(); ++i) {
					_byker->physic.RemoveLinesSet(_islands.front()._lines[i]);
				}
				_islands.pop_front();
			}			

			Island island;
			LevelSet *newSet;
			if (!_useRandom) {
				newSet = island._set = &_level;
			} else {
				assert(_randomLevelsSet.size() > 0);
				int r = rand() % _randomLevelsSet.size();
				newSet = island._set = _randomLevelsSet[r];
			}

			FPoint2D shift = (_islands.back()._set->endpoint[0] - newSet->startpoint[0]);
			FPoint2D b2shift;
			b2shift.x = -shift.x / SCALE_BOX2D;
			b2shift.y = -shift.y / SCALE_BOX2D;
			for (LittleHero::AllLines::iterator i = _byker->physic.GetAllLines().begin(), 
												e = _byker->physic.GetAllLines().end(); i != e; ++i) {
				(*i)->Move(b2shift);
			}
			_byker->physic.SetPosition(_byker->physic.GetPosition() + b2shift);
			_worldOffset.y += shift.y;

			_startPoint = newSet->startpoint[0];
			_endPoint = newSet->endpoint[0];

			for (unsigned int i = 0; i < newSet->ground.size(); ++i) {
				island._lines.push_back(newSet->ground[i]->CreateBody(_byker, i));
			}
			_islands.push_back(island);

		}
		Render::PopMatrix();
	}
	// фпс
	char buff[10];
	Math::FloatToChar(_viewScale, buff);
	Render::PrintString(940, 0, "", buff);

	//buffer = Render::GetDC()->Gfx_StartBatch(HGEPRIM_QUADS, _allElements->GetTexture(), BLEND_DEFAULT, &max);
	if (Render::GetDC()->Input_GetKeyState(HGEK_CTRL)) {
		//for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
		//	Render::PushMatrix();
		//	const b2Transform & xf = body->GetTransform();
		//	Render::MatrixMove(xf.position.x * SCALE_BOX2D, xf.position.y * SCALE_BOX2D);
		//	Render::MatrixRotate(xf.GetAngle());
		//	for (b2Fixture *fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		//		b2Shape *shape = fixture->GetShape();
		//		if (shape->GetType() == b2Shape::e_polygon) {
		//			b2PolygonShape *polygon = (b2PolygonShape *)shape;				
		//			for (unsigned int i = 0; i < polygon->GetVertexCount(); ++i) {
		//				const b2Vec2 &start = polygon->GetVertex(i);
		//				const b2Vec2 &end = polygon->GetVertex((i + 1) % polygon->GetVertexCount());
		//				Render::Line(start.x * SCALE_BOX2D, start.y * SCALE_BOX2D
		//					, end.x * SCALE_BOX2D, end.y * SCALE_BOX2D, 0xFFFFFFFF);
		//			}
		//		} else if (shape->GetType() == b2Shape::e_circle) {
		//			b2CircleShape *circle = (b2CircleShape *)shape;
		//			Render::Circle(circle->m_p.y, circle->m_p.y, circle->m_radius * SCALE_BOX2D, 0xFFFFFFFF);
		//		}			
		//	}
		//	Render::PopMatrix();
		//	if (body->GetJointList()) {
		//		for (b2Joint *joint = body->GetJointList()->joint; joint; joint = joint->GetNext()) {
		//			DrawJoint(joint);
		//		}
		//	}
		//}
	}
	//Render::GetDC()->Gfx_FinishBatch(counter);
}

FPoint2D TileEditor::ScreenToWorld(const FPoint2D &screenPos) {
	return (screenPos - _screenOffset) / _viewScale + _worldOffset;
}

FPoint2D TileEditor::WorldToScreen(const FPoint2D &worldPos) {
	return (worldPos - _worldOffset) * _viewScale + _screenOffset;
}

void TileEditor::Update(float deltaTime) {	
//	deltaTime *= 0.1f;
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
		if (Render::GetDC()->Input_GetKeyState(HGEK_SPACE)) {
			//_byker->_rama->ApplyLinearImpulse(b2Vec2(0.f, -2.f), _byker->_rama->GetPosition());
		}
		//Step(&settings);
		_byker->physic.Update(deltaTime);
		{// двигаем камеру
			//const b2Transform &xf = _byker->_attachedBody->GetTransform();
			//_worldCenter.x = (-xf.position.x * SCALE_BOX2D * _viewScale + SCREEN_WIDTH / 2);
			//_worldCenter.y = (-xf.position.y * SCALE_BOX2D * _viewScale + SCREEN_HEIGHT / 2);			
			//_worldCenter.x = (-xf.position.x * SCALE_BOX2D * _viewScale + SCREEN_WIDTH / 2 - SCREEN_WIDTH / 3);
			FPoint2D bykerScreenPos = WorldToScreen(_byker->physic.GetPosition() * SCALE_BOX2D);
			FPoint2D speedVScreenPos = WorldToScreen((_byker->physic.GetPosition() + _byker->physic.GetSpeedVector()) * SCALE_BOX2D);

			_worldOffset = _byker->physic.GetPosition() * SCALE_BOX2D;

			_screenOffset.x = SCREEN_WIDTH / 6;

			if (_scipScaleChanging < 1e-3) {
				float newScreenOffsetY = _screenOffset.y;
				if (fabs(_byker->physic.GetSpeedVector().y * SCALE_BOX2D) > 10) {
					float delta = -(_byker->physic.GetSpeedVector().y) / fabs(_byker->physic.GetSpeedVector().y);
					_screenOffset.y = min(SCREEN_HEIGHT / 3 * 2, max(SCREEN_HEIGHT / 3, _screenOffset.y + delta * 100.f * deltaTime));
				}
				float newViewScale = min(1.f, 350.f / (_byker->physic.GetSpeedVector().Length() * SCALE_BOX2D));
				if (_viewScale > newViewScale) {
					_viewScale = newViewScale;
				} else {
					_viewScale = min(newViewScale, _viewScale + 0.1f * deltaTime);
				}
			} else {
				_scipScaleChanging -= deltaTime;
			}
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
	//b2Vec2 b2TouchPosition = b2Vec2(pos.x, pos.y);
	//b2Vec2 b2BodyPosition;
	float distance; 
	float strength;
	float force;
	float angle;
	//for (b2Body* b = m_world->GetBodyList(); b; b = b->GetNext()) {
	//	b2BodyPosition = b2Vec2(b->GetPosition().x, b->GetPosition().y);
	//	distance = b2Distance(b2BodyPosition, b2TouchPosition);
	//	if(distance < maxDistance - 0.01) {
	//		strength = (maxDistance - distance) / maxDistance; 
	//		force = sqrtf(strength) * maxForce;
	//		angle = atan2f(b2BodyPosition.y - b2TouchPosition.y, b2BodyPosition.x - b2TouchPosition.x);
	//		// Apply an impulse to the body, using the angle
	//		b->ApplyLinearImpulse(b2Vec2(cosf(angle) * force, sinf(angle) * force), b->GetPosition());
	//	}
	//}
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
		_screenOffset = FPoint2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		_worldOffset = _level.startpoint[0];
	}
	EraseAllBodyes();
	//for (BodyStates::iterator i = _state.begin(), e = _state.end(); i != e; i++) {

	//	/*b2Body *b = */AddElement(*i);
	//	//b->SetTransform(_state[i].pos, _state[i].angle);
	//}
}

void TileEditor::NewLevelYesNo(const std::string &message) {
	if (message == "yes") {
		InitParams(NULL);
		_currentLevel = "";
		_state.clear();
		EraseAllBodyes();
		_screenOffset = FPoint2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		_worldOffset = FPoint2D(0.f, 0.f);
		_viewScale = 1.f;
		ClearLevel();
	}
}

void TileEditor::DeleteSelectedYesNo(const std::string &message) {
	EraseSelected(/*_selectedBody*/);
	InitParams(NULL);
}

void TileEditor::OverwriteYesNo(const std::string &message) {
	if (message == "yes") {
		SaveLevel(_saveLevelName);
	}
}

void TileEditor::AddNewElement(const std::string &msg) {
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
		for (int i = 0; i < 4; ++i) {
			b->AddPoint(_worldOffset.x + 32.f * sin(M_PI / 2 * i), _worldOffset.y + 32.f * cos(M_PI / 2 * i));
		}
		_level.ground.push_back(b);
	} else if (msg == "image") {
		Messager::SendMessage("MiddleList", "prefix AddBackImage");
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
		b.pos = _worldOffset;
		_level.beauties.push_back(b);
	} else if (msg == "box") {
		Messager::SendMessage("SmallList", "prefix AddBoxElement");
		Messager::SendMessage("SmallList", "add small");
		Messager::SendMessage("SmallList", "add big");
		Messager::SendMessage("SmallList", "special add cancel");
	} else if (msg == "start pos") {
		_level.startpoint.push_back(_worldOffset);
	} else if (msg == "end pos") {
		_level.endpoint.push_back(_worldOffset);
	} else if (msg == "bonus") {
		AddElement("rubber");
	}
}

void TileEditor::AddBoxElement(const std::string &msg) {
	Messager::SendMessage("SmallList", "clear");
	if (msg == "cancel") {
		return;
	}
}

void TileEditor::AddBackImage(const std::string &msg) {
	Messager::SendMessage("MiddleList", "clear");
	if (msg == "cancel") {
		return;
	}
	OneImage image;
	image.texture = Render::GetDC()->Texture_Load((Render::GetDataDir() + "data\\images\\" + msg).c_str());
	image.sprite = new Sprite(image.texture, 0, 0, Render::GetDC()->Texture_GetWidth(image.texture), Render::GetDC()->Texture_GetHeight(image.texture));
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
}

void TileEditor::PreSaveLevel(const std::string &msg) {
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
		SaveLevel(buff);
		return;
	} else if (_currentLevel != msg) {
		PrefixedAskMessageShow("Are you sure?\nDo you want overwrite " + msg + "?", "OverwriteYesNo");
		_saveLevelName = msg;
		_saveLevelXml = xe;
		return;
	}
	SaveLevel(msg);
}

void TileEditor::OnMessage(const std::string &message) {
	std::string msg;
	if (CanCut(message, "NewLevelYesNo", msg)) {
		NewLevelYesNo(msg);
		return;
	}
	if (CanCut(message, "DeleteSelectedYesNo", msg)) {
		DeleteSelectedYesNo(msg);
		return;
	}
	if (CanCut(message, "OverwriteYesNo", msg)) {
		OverwriteYesNo(msg);
		return;
	}
	if (CanCut(message, "AddNewElement", msg)) {
		AddNewElement(msg);
		return;
	}
	if (CanCut(message, "AddBoxElement", msg)) {
		AddBoxElement(msg);
		return;
	}
	if (CanCut(message, "AddBackImage", msg)) {
		AddBackImage(msg);
		return;
	}
	if (CanCut(message, "LoadLevel", msg)) {
		LoadLevel(msg);
		return;
	}
	if (CanCut(message, "PreSaveLevel", msg)) {
		PreSaveLevel(msg);
		return;
	}
	if (message == "changes") {
		InitParams(_selectedBody);
	} else if (message == "net") {
		_netVisible = !_netVisible;
		if (!_netVisible) {
			for (unsigned int i = 0; i < _level.ground.size(); ++i) {
				_level.ground[i]->GenerateTriangles();
			}
		}
	} else if (message == "play" || message == "random") {
		if (_editor) { // переходим в режим игры
			if (CanLevelStart()) {
				//SetValueB("big", "visible", false);
				//SetValueB("small", "visible", true);
				SetValueS("play", "", "stop");
				_useRandom = (message == "random");
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
		Messager::SendMessage("SmallList", "prefix AddNewElement");
		Messager::SendMessage("SmallList", "add curv");
		Messager::SendMessage("SmallList", "add image");
		Messager::SendMessage("SmallList", "add beauty");
		Messager::SendMessage("SmallList", "add bonus");
		Messager::SendMessage("SmallList", "add box");
		Messager::SendMessage("SmallList", "add transport");
		Messager::SendMessage("SmallList", "add animal");
		if (_level.startpoint.size() == 0) {
			Messager::SendMessage("SmallList", "add start pos");
		}
		if (_level.endpoint.size() == 0) {
			Messager::SendMessage("SmallList", "add end pos");
		}
		Messager::SendMessage("SmallList", "special add cancel");
	} else if (message == "left") {
	} else if (message == "right") {
	} else if (message == "up") {
	} else if (message == "down") {
	} else if (message == "new") {
		PrefixedAskMessageShow("Are you sure?\nDelete all objects?", "NewLevelYesNo");
	} else if (message == "open") {
		Messager::SendMessage("BigList", "prefix LoadLevel");
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
	} else if (message == "save") {
		Messager::SendMessage("BigList", "prefix PreSaveLevel");
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
	} else {
		assert(message == "ok");
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
	word->SetAttribute("x", _worldOffset.x);
	word->SetAttribute("y", _worldOffset.y);
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

void TileEditor::ClearLevel() {
	_currentElement.selected = SelectedElement::none;
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

void TileEditor::LoadLevel(const std::string &msg) {
	Messager::SendMessage("BigList", "clear");
	if (msg == "cancel") {
		return;
	}
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
				image.sprite = new Sprite(image.texture, 0, 0, Render::GetDC()->Texture_GetWidth(image.texture), Render::GetDC()->Texture_GetHeight(image.texture));
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
	_worldOffset.x = atof(word->Attribute("x"));
	_worldOffset.y = atof(word->Attribute("y"));

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

void TileEditor::CalcNextBykePos(float dt) {
	FPoint2D proection;
	for (unsigned int i = 0; i < _level.ground.size(); ++i) {
		if (_level.ground[i]->SearchProection(proection)) {
		}
	}
}

typedef std::vector<b2Vec2> Triangle;

bool Convex(Triangle &m_vertices) {
	int32 m_vertexCount = m_vertices.size();
	for (int32 i = 0; i < m_vertexCount; ++i)
	{
		int32 i1 = i;
		int32 i2 = i + 1 < m_vertexCount ? i + 1 : 0;
		b2Vec2 edge = m_vertices[i2] - m_vertices[i1];

		for (int32 j = 0; j < m_vertexCount; ++j)
		{
			// Don't check vertices on the current edge.
			if (j == i1 || j == i2)
			{
				continue;
			}
			
			b2Vec2 r = m_vertices[j] - m_vertices[i1];

			// Your polygon is non-convex (it has an indentation) or
			// has colinear edges.
			float32 s = b2Cross(edge, r);
			return false;
		}
	}
	return true;
}

void TileEditor::SetupBox2D() {
	EraseAllBodyes();
	_viewScale = 1.f;
	_islands.clear();
	Island island;
	island._set = &_level;
	for (unsigned int i = 0; i < _level.ground.size(); ++i) {
		island._lines.push_back(_level.ground[i]->CreateBody(_byker, i));
	}
	_islands.push_back(island);

	_byker->physic.SetPosition(FPoint2D(_level.startpoint[0].x / SCALE_BOX2D, _level.startpoint[0].y / SCALE_BOX2D));
	_byker->physic.SetMinSpeed(7.f);
	_byker->physic.SetSpeedVector(FPoint2D(0.f, 0.f));
	//b2BodyDef bd;
	//bd.type = b2_staticBody;  
	//bd.position.Set(0.f, 0.f);
	_startPoint = _level.startpoint[0];
	_endPoint = _level.endpoint[0];
	//bd.angle = 0.f;
	//b2Body* body = m_world->CreateBody(&bd);		
	//for (unsigned int i = 0; i < _level.ground.size(); ++i) {
	//	_level.ground[i]->CreateBody(body);
	//}
	//body->ResetMassData();
	//_landBodies.push_back(body);

	//{
	//	// first wheel
	//	b2BodyDef bd;
	//	bd.type = b2_dynamicBody;  
	//	bd.fixedRotation = false;

	//	bd.position.Set(_level.startpoint[0].x / SCALE_BOX2D, _level.startpoint[0].y / SCALE_BOX2D);
	//	bd.angle = 0.f;
	//	
	//	b2Body* body = m_world->CreateBody(&bd);
	//	
	//	b2FixtureDef fd;
	//	fd.restitution = 0.0f;
	//	fd.friction = 1.f;
	//	fd.density = 1.f;

	//	b2CircleShape shape;//34px - 0.7m
	//	shape.m_radius = 17.f / SCALE_BOX2D;
	//	fd.shape = &shape;
	//	body->CreateFixture(&fd);

	//	body->SetUserData(_byker);
	//	body->ResetMassData();
	//	_byker->_attachedBody = body;

	//	// second wheel
	//	bd.position.Set((_level.startpoint[0].x + 78.f)/ SCALE_BOX2D, _level.startpoint[0].y / SCALE_BOX2D);
	//	bd.angle = 0.f;
	//	
	//	body = m_world->CreateBody(&bd);
	//	
	//	fd.restitution = 0.0f;
	//	fd.friction = 1.f;
	//	fd.density = 1.f;

	//	shape.m_radius = 17.f / SCALE_BOX2D;
	//	fd.shape = &shape;
	//	body->CreateFixture(&fd);

	//	body->ResetMassData();
	//	_byker->_attachedBody2 = body;

	//	//b2DistanceJointDef jointDef;
	//	//jointDef.Initialize(_byker->_attachedBody, _byker->_attachedBody2, _byker->_attachedBody->GetPosition(), _byker->_attachedBody2->GetPosition());
	//	//jointDef.collideConnected = true;

	//	//m_world->CreateJoint(&jointDef);

	//	// hidden part - byke
	//	bd.position.Set((_level.startpoint[0].x + 30.f)/ SCALE_BOX2D, (_level.startpoint[0].y) / SCALE_BOX2D);
	//	bd.angle = 0.f;
	//	bd.fixedRotation = false;
	//	
	//	body = m_world->CreateBody(&bd);
	//	_byker->_rama = body;
	//	
	//	fd.restitution = 0.0f;
	//	fd.friction = 0.f;
	//	fd.density = 1.f;

	//	b2PolygonShape box;
	//	box.SetAsBox(20.f / SCALE_BOX2D, 4.f / SCALE_BOX2D);
	//	//shape.m_radius = 36.f / SCALE_BOX2D;
	//	fd.shape = &box;
	//	body->CreateFixture(&fd);
	//	body->ResetMassData();

	//	b2RevoluteJointDef jointDef;
	//	jointDef.collideConnected = false;

	//	jointDef.Initialize(body, _byker->_attachedBody, _byker->_attachedBody->GetPosition());
	//	m_world->CreateJoint(&jointDef);

	//	jointDef.Initialize(body, _byker->_attachedBody2, _byker->_attachedBody2->GetPosition());
	//	m_world->CreateJoint(&jointDef);


	//	// hidden second part head
	//	bd.position.Set((_level.startpoint[0].x + 38.f)/ SCALE_BOX2D, (_level.startpoint[0].y - 63) / SCALE_BOX2D);
	//	bd.angle = 0.f;
	//	bd.fixedRotation = true;
	//	
	//	b2Body *head = m_world->CreateBody(&bd);
	//	
	//	fd.restitution = 0.0f;
	//	fd.friction = 0.f;
	//	fd.density = 1.f;

	//	shape.m_radius = 10.f / SCALE_BOX2D;
	//	fd.shape = &shape;
	//	head->CreateFixture(&fd);
	//	head->ResetMassData();
	//	_byker->_head = head;

	//	jointDef.Initialize(body, head, body->GetPosition());
	//	jointDef.enableLimit = true;
	//	jointDef.lowerAngle = - M_PI_4 / 2;// * 3;
	//	jointDef.upperAngle = M_PI_4 / 2;// * 3;
	//	m_world->CreateJoint(&jointDef);

	//}
}

void TileEditor::LoadRandomLevelsSet(const std::string &fileName) {
	if (_randomLevelsSet.size()) {
		LOG("_randomLevelsSet.size() != 0");
		assert(false);
	}
	TiXmlDocument doc(fileName.c_str());
	if (doc.LoadFile() == false) {
		LOG("file not found: " + fileName);
		assert(false);
	}
	TiXmlElement *xe = doc.RootElement()->FirstChildElement();
	while (xe != NULL) {
		LevelSet *l = new LevelSet();
		l->LoadFromXml(xe, true);
		_randomLevelsSet.push_back(l);
		xe = xe->NextSiblingElement();
	}
}