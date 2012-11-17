#include "TileEditor.h"
#include "../Helpers/MyMessageBox.h"
#include "../Core/Core.h"
#include "../Core/Render.h"
#include "../Core/Math.h"
#include <cstdio>
#include "ColoredPolygon.h"

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
	TiXmlElement *beautyList = doc.RootElement()->FirstChildElement("Beauties");
	if (beautyList) {
		TiXmlElement *elem = beautyList->FirstChildElement();
		std::string typeName;
		BeautyBase *beauty;
		while (elem != NULL) {
			typeName = elem->Value();// Attribute("type");
			if (typeName == "ColoredPolygon") {
				beauty = new ColoredPolygon(elem);
			} else if (typeName == "Beauty") {
				beauty = new Beauty(elem);
			} else {
				assert(false);
			}
			_collection.push_back(beauty);
			elem = elem->NextSiblingElement();
		}
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
	_currents.beauty = NULL;
	_currents.line = NULL;
	My::AnimationManager::Load(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + "data\\byker\\ride.xml").c_str()));
	_byker = new Byker();
	_flags = Render::GetDC()->Texture_Load(Render::GetDC()->Resource_MakePath((Render::GetDataDir() + "data\\flagspoint.png").c_str()));
	_startFlag = new Sprite(_flags, 0, 0, 64, 64);
	_endFlag = new Sprite(_flags, 63, 0, 64, 64);

	_currentElement.selected = SelectedElement::none;
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

void TileEditor::EraseSelected() {
}

void TileEditor::EraseAllBodyes() {
	while (_byker->physic.GetAllLines().size()) {
		_byker->physic.RemoveLinesSet(NULL);
	}
}

// добавляем новый элемент в "случайное" место на экране
void TileEditor::AddElement(const std::string &typeId) {
}

void TileEditor::OnMouseDown(const FPoint2D &mousePos)
{	
	if (!_editor) {
		FPoint2D imp(0.f, -4.5f);
		_byker->physic.SetImpulse(imp);
		float speedChange = imp.Length() / _byker->physic.GetMass() + max(0.f, -_byker->physic.GetSpeedVector().y);
		_scipScaleChanging = speedChange / (_byker->physic.GetGravity().Length());
		return;
	}
	_mouseDown = true;
	_lastMousePos = mousePos;
	InitParams(NULL);
	FPoint2D fp = ScreenToWorld(mousePos);

	{// ищем по кому кликнули
		_currents.beauty = NULL;
		_currents.line = NULL;
		for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
			if (_level.groundLines[i]->DoesContainPoint(mousePos)) {
				_currents.line = _level.groundLines[i];
				break;
			}
		}
		if (!_currents.line) {
			for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
				if (_level.beauties[i]->PixelCheck(mousePos)) {
					_currents.beauty = _level.beauties[i];
					break;
				}
			}
		}
	}

	if (_currents.line) {
		if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT)) {
			if (Render::GetDC()->Input_GetKeyState(HGEK_D)) {
				for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
					if (_level.groundLines[i]->DoesContainPoint(mousePos)) {
						delete _level.groundLines[i];
						_level.groundLines.erase(_level.groundLines.begin() + i);
						_currents.line = NULL;
						break;
					}
				}
			}
		} else {
			_currents.line->MouseDown(mousePos);
		}
	} else if (_currents.beauty) {
		_currents.beauty->MouseDown(mousePos);
	}
	
}

void TileEditor::OnMouseUp()
{
	_mouseDown = false;
	if (_currents.beauty) {
		_currents.beauty->MouseUp(_lastMousePos);
		assert(_currents.line == NULL);
	} else if (_currents.line) {
		_currents.line->MouseUp(_lastMousePos);
		assert(_currents.beauty == NULL);//?
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
	return true;
}

bool TileEditor::IsLevelFinish() {
	return _userLevelWin;//_finish != 0x3;
}


void TileEditor::OnMouseMove(const FPoint2D &mousePos)
{
	if (!_editor) {
		return;
	}
	FPoint2D newMmouseWorld = ScreenToWorld(mousePos);

	if (!Render::GetDC()->Input_GetKeyState(HGEK_ALT)) {
		for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
			_level.beauties[i]->MouseMove(mousePos);
		}

		for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
			_level.groundLines[i]->MouseMove(mousePos);
		}
	}

	//if (_currentElement.selected != SelectedElement::none && _mouseDown) {
	//	if (_currentElement.selected == SelectedElement::beauty_element) {
	//		_level.beauties[_currentElement.index]->Pos().x += (newMmouseWorld.x - _mouseWorld.x);
	//		_level.beauties[_currentElement.index]->Pos().y += (newMmouseWorld.y - _mouseWorld.y);
	//	}
	//} else if (Render::GetDC()->Input_GetKeyState(HGEK_SHIFT) && _currents.dotIndex >= 0) {
	//	float dx = newMmouseWorld.x - _mouseWorld.x;
	//	float dy = newMmouseWorld.y - _mouseWorld.y;

	//	_currents.block->ShiftDot(_currents.dotIndex, dx, dy, _currents.moveAllDots);

	//} else 
	if ((_currents.beauty == NULL && _currents.line == NULL) || Render::GetDC()->Input_GetKeyState(HGEK_ALT)) {
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
	if (direction > 0 && _viewScale < 4.f) {
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
		// кусты
		for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
			_level.beauties[i]->Draw();
		}
		for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
			_level.groundLines[i]->DebugDrawLines();
		}
		if (_netVisible) {// сетка
			Matrix m;
			m.MakeRevers(Render::GetCurrentMatrix());
			float startX = 0.f;
			float startY = 0.f;
			m.Mul(startX, startY);
			float endX = 960.f;
			float endY = 640.f;
			m.Mul(endX, endY);
			float STEP = 64.f;
			float x = static_cast<int>(startX / STEP) * STEP;
			while (x < endX) {
				Render::Line(x, startY, x, endY, 0x4FFFFFFF);
				x += STEP;
			}
			float y = static_cast<int>(startY / STEP) * STEP;
			while (y < endY) {
				Render::Line(startX, y, endX, y, 0x4FFFFFFF);
				y += STEP;
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
				(*j)._set->beauties[i]->Draw();
			}
			Render::Line((*j)._set->Startpoint().x - 5, (*j)._set->Startpoint().y - 5,
				(*j)._set->Startpoint().x + 5, (*j)._set->Startpoint().y + 5, 0xFF44FF44);
			Render::Line((*j)._set->Startpoint().x + 5, (*j)._set->Startpoint().y - 5,
				(*j)._set->Startpoint().x - 5, (*j)._set->Startpoint().y + 5, 0xFF44FF44);
			Render::Line((*j)._set->Endpoint().x - 5, (*j)._set->Endpoint().y - 5,
				(*j)._set->Endpoint().x + 5, (*j)._set->Endpoint().y + 5, 0xFF44FF44);
			Render::Line((*j)._set->Endpoint().x + 5, (*j)._set->Endpoint().y - 5,
				(*j)._set->Endpoint().x - 5, (*j)._set->Endpoint().y + 5, 0xFF44FF44);
			Render::PopMatrix();
		}

		// мотоциклист
		FPoint2D p(_byker->physic.GetPosition() * SCALE_BOX2D);

		Render::Line(LittleHero::a.x * SCALE_BOX2D, LittleHero::a.y * SCALE_BOX2D, LittleHero::b.x * SCALE_BOX2D, LittleHero::b.y * SCALE_BOX2D, 0xFFFFFFFF);

		Render::PushMatrix();
		Render::MatrixMove(p.x, p.y);
		Render::MatrixRotate(_bykerAngle);
		_byker->SetPos(FPoint2D(0, 0));
		if (Render::GetDC()->Input_GetKeyState(HGEK_CTRL)) {
			Render::Line(0, 0, _byker->physic.GetSpeedVector().x * SCALE_BOX2D, _byker->physic.GetSpeedVector().y * SCALE_BOX2D, 0xFFFFFFFF);
		}
		_byker->Draw(_bykerAngle);
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

			FPoint2D shift = (_islands.back()._set->Endpoint() - newSet->Startpoint());
			FPoint2D b2shift;
			b2shift.x = -shift.x / SCALE_BOX2D;
			b2shift.y = -shift.y / SCALE_BOX2D;
			for (LittleHero::AllLines::iterator i = _byker->physic.GetAllLines().begin(), 
												e = _byker->physic.GetAllLines().end(); i != e; ++i) {
				(*i)->Move(b2shift);
			}
			_byker->physic.SetPosition(_byker->physic.GetPosition() + b2shift);
			_worldOffset.y += shift.y;

			for (unsigned int i = 0; i < newSet->groundLines.size(); ++i) {
				island._lines.push_back(newSet->groundLines[i]->CreateBody(_byker, i));
			}

			_startPoint = newSet->Startpoint();
			_endPoint = newSet->Endpoint();

			_islands.push_back(island);

		}
		Render::PopMatrix();
	}
	// фпс
	char buff[10];
	Math::FloatToChar(_viewScale, buff);
	Render::PrintString(940, 0, "", buff);
}

FPoint2D TileEditor::ScreenToWorld(const FPoint2D &screenPos) {
	return (screenPos - _screenOffset) / _viewScale + _worldOffset;
}

FPoint2D TileEditor::WorldToScreen(const FPoint2D &worldPos) {
	return (worldPos - _worldOffset) * _viewScale + _screenOffset;
}

bool TileEditor::OnKey(int key) {
	if (_currentElement.selected == SelectedElement::beauty_element) {
		if (key == HGEK_DELETE) {
			if (_currents.beauty) {
				for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
					if (_level.beauties[i] == _currents.beauty) {
						delete _level.beauties[i];
						_level.beauties.erase(_level.beauties.begin() + i);
						_currents.beauty = NULL;
						break;
					}
				}
				for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
					_level.groundLines[i]->DrawLines();
				}
			} else if (_currents.line) {
				for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
					if (_level.groundLines[i] == _currents.line) {
						delete _level.groundLines[i];
						_level.groundLines.erase(_level.groundLines.begin() + i);
						_currents.line = NULL;
						break;
					}
				}
			}
		}
	}
	return true;
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
		if (Render::GetDC()->Input_GetKeyState(HGEK_SPACE)) {
		}
		_byker->physic.Update(deltaTime);
		float angle = atan2(_byker->physic.GetSpeedVector().y, _byker->physic.GetSpeedVector().x);
		if (fabs(_bykerAngle - angle) > 3e-1) {
			_bykerAngle = angle;
		} else {
			_bykerAngle = _bykerAngle * 0.7f + angle * 0.3f;
		}
		{// двигаем камеру
			FPoint2D bykerScreenPos = WorldToScreen(_byker->physic.GetPosition() * SCALE_BOX2D);
			FPoint2D speedVScreenPos = WorldToScreen((_byker->physic.GetPosition() + _byker->physic.GetSpeedVector()) * SCALE_BOX2D);
			_worldOffset = _byker->physic.GetPosition() * SCALE_BOX2D;
			_screenOffset.x = SCREEN_WIDTH / 6;
			if (_scipScaleChanging < 1e-3) {
				float newScreenOffsetY = _screenOffset.y;
				if (fabs(_byker->physic.GetSpeedVector().y * SCALE_BOX2D) > 20) {
					float delta = -(_byker->physic.GetSpeedVector().y) / fabs(_byker->physic.GetSpeedVector().y) + 0.4f;
					_screenOffset.y = min(SCREEN_HEIGHT / 3 * 2, max(SCREEN_HEIGHT / 4, _screenOffset.y + (delta) * 100.f * deltaTime));
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
	_screenOffset = FPoint2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	_worldOffset = _level.Startpoint();
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

	std::string message(msg);

	if (msg == "ground") {
		SolidGroundLine *b = new SolidGroundLine();
		for (int i = 0; i < 5; ++i) {
			b->AddPoint(_worldOffset.x - 256.f + 256 * i / 2, _worldOffset.y);
		}
		_level.groundLines.push_back(b);
	} else if (CanCut(msg, "beauty", message)) {
		BeautyBase *origin = _collection[atoi(message.c_str())];
		BeautyBase *b;
		if (origin->Type() == "Beauty") {
			b = new Beauty(*(Beauty *)origin);
		} else {
			assert(false);
		}
		b->MoveTo(_screenOffset.x, _screenOffset.y);
		 _level.beauties.push_back(b);
		_currents.beauty = b;
	} else if (msg == "box") {
		Messager::SendMessage("SmallList", "prefix AddBoxElement");
		Messager::SendMessage("SmallList", "add small");
		Messager::SendMessage("SmallList", "add big");
		Messager::SendMessage("SmallList", "special add cancel");
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
	} else if (message == "play" || message == "random") {
		if (_editor) { // переходим в режим игры
			if (CanLevelStart()) {
				SetValueS("play", "", "stop");
				_useRandom = (message == "random");
				_editor = false;
				SaveState();
				_netVisible = false;
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
		Messager::SendMessage("SmallList", "add ground");
		//Messager::SendMessage("SmallList", "add curv");
		Messager::SendMessage("SmallList", "add beauty");
		//Messager::SendMessage("SmallList", "add bonus");
		//Messager::SendMessage("SmallList", "add box");
		//Messager::SendMessage("SmallList", "add transport");
		//Messager::SendMessage("SmallList", "add animal");
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
	for (LevelGroundLines::iterator i = _level.groundLines.begin(), e = _level.groundLines.end(); i != e; i++) {
		TiXmlElement *elem = new TiXmlElement((*i)->Type());
		(*i)->SaveToXml(elem);
		elemGround->LinkEndChild(elem);
	}

	TiXmlElement *beautyList = new TiXmlElement("Beauties");
	for (int i = 0; i < _level.beauties.size(); i++) {
		TiXmlElement *beauty = new TiXmlElement(_level.beauties[i]->Type());
		_level.beauties[i]->SaveToXml(beauty);
		beautyList->LinkEndChild(beauty);
	}
	_saveLevelXml->LinkEndChild(beautyList);

	TiXmlElement *word = new TiXmlElement("word");
	word->SetAttribute("x", _worldOffset.x);
	word->SetAttribute("y", _worldOffset.y);
	char s[16];
	sprintf(s, "%f", _viewScale);
	word->SetAttribute("scale", s);
	_saveLevelXml->LinkEndChild(word);

	_doc.SaveFile();
	_currentLevel = levelName;
}

void TileEditor::ClearLevel() {
	_currentElement.selected = SelectedElement::none;
	_level.Clear();

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
	_level.LoadFromXml(xe, false);

	TiXmlElement *word = xe->FirstChildElement("word");
	_worldOffset.x = atof(word->Attribute("x"));
	_worldOffset.y = atof(word->Attribute("y"));

	_viewScale = atof(word->Attribute("scale"));
	SetValueS("play", "", "play");
	//ResetState();
	_currentLevel = msg;
}

void TileEditor::CalcNextBykePos(float dt) {
	assert(false);
	FPoint2D proection;
	for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
		if (_level.groundLines[i]->SearchProection(proection)) {
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
	for (unsigned int i = 0; i < _level.groundLines.size(); ++i) {
		island._lines.push_back(_level.groundLines[i]->CreateBody(_byker, i));
	}
	_islands.push_back(island);

	_byker->physic.SetPosition(FPoint2D(_level.Startpoint().x / SCALE_BOX2D, _level.Startpoint().y / SCALE_BOX2D - _byker->physic.GetSize()));
	_byker->physic.SetMinSpeed(7.f);
	_byker->physic.SetSpeedVector(FPoint2D(0.f, 0.f));
	_startPoint = _level.Startpoint();
	_endPoint = _level.Endpoint();
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