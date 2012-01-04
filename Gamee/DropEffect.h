#include "../Core/types.h"
#include "../Core/Render.h"
#include "BaseElement.h"

class DropEffect : public BaseElement
{
public:
	DropEffect(const FPoint2D &startPos, const FPoint2D &offsetStartPos, const FPoint2D &endPos, const FPoint2D &offsetEndPos, const std::string &product);
	virtual void Draw();
	virtual void DrawBottom();
	virtual void Update(float dt);
private:
	FPoint2D _startPos;
	float _startHeight;
	FPoint2D _endPos;
	float _endHeight;
	float _timeCounter;
	float _time;
	Texture *_shadow;
	Texture *_product;
};