#ifndef MYENGINE_GAMEFIELD_H
#define MYENGINE_GAMEFIELD_H

#include "../Core/InputSystem.h"
#include "../Core/Core.h"
#include "../Core/Render.h"
#include "PersPaths.h"
#include "AnnaPers.h"
#include "BobPers.h"
#include "GrandpaPers.h"
#include "CloverField.h"
#include "GrainField.h"
#include "TreePlace.h"
#include "PlantPlace.h"
#include "EnvWell.h"
#include "ProductPlace.h"
#include "PopupMenu.h"
#include "Archaeopteryx.h"
#include "Animal.h"
#include "SwampPlace.h"
#include "Waterpan.h"
#include "Foodpan.h"
#include "ClientManager.h"
#include "Moneypod.h"
#include "DropEffect.h"
#include "VipClient.h"

class GameField 
	:public InputSystem
{
public:

	static const float SELECTION_BORDER;

	GameField(TiXmlElement *xe);
	virtual void Draw();
	virtual void Update(float dt);
	virtual ~GameField(void);
	virtual bool IsMouseOver(const FPoint2D &mousePos);
	virtual void OnMouseDown(const FPoint2D &mousePos);
	virtual void OnMouseMove(const FPoint2D &mousePos);
	virtual void OnMouseUp();
	static void HideAllPopupMenu();
	static void AddPopupMenu(PopupMenu *menu);
	static void RemovePopupMenu(PopupMenu *menu);
	static float AddDropEffect(const std::string &persName, const FPoint2D &endPos, float height, unsigned char endAlpha = 0xFF, float endScale = 1.f);
	static ProductPlace * GetBuckPlace();
	static EnvWell * GetWell();
	static void AddBird(const std::string &birdId);
	static void AddAnimal(const std::string &animalId);

private:

	void SortElements();
	StaticSprite _field_s1, _field_s1_m;
	StaticSprite _grass_set1_a, _grass_set1_a_m;
	StaticSprite _grass_set1_e, _grass_set1_e_m;
	StaticSprite _grass_set1_f;
	StaticSprite _groundup_s1;
	StaticSprite _ground_s1;
	StaticSprite _road_set1_a, _road_set1_a_m;
	StaticSprite _road_set1_b;
	StaticSprite _road_set1_c, _road_set1_c_m;
	StaticSprite _s01_bush;
	StaticSprite _stoneshelve_set1_a, _stoneshelve_set1_a_m;
	StaticSprite _stoneshelve_set1_a2, _stoneshelve_set1_a2_m;
	StaticSprite _stoneshelve_set1_b, _stoneshelve_set1_b_m;
	StaticSprite _stoneshelve_set1_d, _stoneshelve_set1_d_m;
	StaticSprite _stoneshelve_set1_e;

	typedef std::vector<BaseElement *> ElementList;
	static ElementList _elementList;
	ElementList _cover;

	PersPaths _persPaths;
	AnnaPers _anna;
	BobPers _bob;
	GrandpaPers _grandpa;
	static AnnaPers *_annaPtr;
	static BobPers *_bobPtr;
	static GrandpaPers *_grandpaPtr;
	CloverField _cloverField;
	GrainField _grainField;
	TreePlace _treeBed1;
	TreePlace _treeBed2;
	PlantPlace _plantBed1;
	PlantPlace _plantBed2;
	PlantPlace _plantBed3;
	PlantPlace _plantBed4;
	static EnvWell *_well;
	ProductPlace _storagePlace1;
	ProductPlace _storagePlace2;
	static ProductPlace *_buckPlace;
	ProductPlace _productPlace2;
	ProductPlace _productPlace3;
	ProductPlace _productPlace4;
	ProductPlace _productPlace5;
	ProductPlace _productPlace6;
	Waterpan _waterPan1;
	Waterpan _waterPan2;
	Foodpan _foodPan1;
	Foodpan _foodPan2;
	typedef std::vector<Archaeopteryx *> Birds;
	static Birds _archaeopteryx;
	typedef std::vector<Animal *> AnimalPtrs;
	static AnimalPtrs _animalPtrs;
	SwampPlace _swampPlace;
	VipClient _vipClients;

	StaticSprite _bush1;
	StaticSprite _bush2;
	StaticSprite _bush3;
	StaticSprite _bush4;
	StaticSprite _bush5;
	StaticSprite _bush6;
	StaticSprite _bush7;
	void DrawBushes();
	ClientManager _clientsManager;
	FPoint2D _lastMousePos;
	int _persIndexDebug;

	typedef std::list<PopupMenu *> PopupMenus;
	static PopupMenus _popupMenus;

	BaseElement *_lockMouseMove;

	Moneypod _moneypod;

	//StaticSprite _testBackground;
};

#endif//MYENGINE_GAMEFIELD_H