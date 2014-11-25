#ifndef MYENGINE_POPUPMENU_H
#define MYENGINE_POPUPMENU_H

#include "../Core/Matrix.h"
#include "../Core/Render.h"
#include "CircleProgress.h"
#include "../Core/SplinePath.h"

class PopupMenu 
{
public:
	~PopupMenu();
	PopupMenu();
	void Draw();
	void SetPos(float x, float y);
	void SetIcons(const std::string &first, const std::string &second, const std::string &third);
	void Update(float dt);
	bool IsUnderMouse(const FPoint2D &mousePos);
	void OnMouseDown(const FPoint2D &mousePos);
	void OnMouseMove(const FPoint2D &mousePos);
	std::string GetActive(const FPoint2D &mousePos);
	bool IsVisible();
	void Hide();
	void Show();

private:
	StaticSprite _firstTab;
	StaticSprite _secondTab;
	StaticSprite _thirdTab;

	StaticSprite _firstIcon;
	StaticSprite _secondIcon;
	StaticSprite _thirdIcon;
	
	StaticSprite *_selected;

	float _timeCounter;
	bool _visible;
	enum {
		state_show,
		state_hide,
		state_visible,
		state_invisible
	} _state;
	
	std::vector<std::string> _iconsName;

	FPoint2D _position;

	SplinePath _riseUp;
};

#endif//MYENGINE_POPUPMENU_H