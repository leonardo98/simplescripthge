#include "../Core/Render.h"

class SetItem {
public:
	enum Types{
		beauty_item = 0,
		beauty_cover = 1,
		destroyable_item = 2,
		funbush_item = 3,
		collectable_item = 4
	};
	virtual ~SetItem();
	SetItem(Types type);
	SetItem(Types type, const char *fileName, const FPoint2D &pos, float angle);
	bool HasPixel(float x, float y);
	FPoint2D &Pos();
	float &Angle();
	virtual void SaveToXml(TiXmlElement *xe) const;
	virtual void LoadFromXml(TiXmlElement *xe);
	virtual void Draw() const;
	virtual void Change(int p) = 0;
	int GetDrawOrder() {return _drawOrder;}
	void SetDrawOrder(int order) {_drawOrder = order; }
	Types Type();
protected:
	const Types _type;
	HTEXTURE _texture;
	Sprite *_sprite;
	int _drawOrder;
	FPoint2D _pos;
	float _angle;
	std::string _filePath;
};

class Beauty : public SetItem{
public:
	Beauty();
	Beauty(const char *fileName, const FPoint2D &pos, float angle, float scale, bool mirror);
	virtual void SaveToXml(TiXmlElement *xe) const;
	virtual void LoadFromXml(TiXmlElement *xe);
	virtual void Draw() const;
	virtual void Change(int p);
private:
	bool _mirror;
	float _scale;
};

class GroundLine : public SetItem{
public:
	GroundLine();
	GroundLine(const FPoint2D &pos, float angle, float scale, bool mirror);
	virtual void SaveToXml(TiXmlElement *xe) const;
	virtual void LoadFromXml(TiXmlElement *xe);
	virtual void Draw() const;
	virtual void Change(int p);
	void SetFileName(const std::string &fileName);
private:
	bool _mirror;
	float _scale;
	int _index;//[0..maxIndex)
	const int _maxIndex;
};

