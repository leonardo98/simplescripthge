// cursors.h: interface for the cursors class.
//
//////////////////////////////////////////////////////////////////////

#ifndef cursors_h
#define cursors_h

#include "../Core/types.h"
#include "../Core/Render.h"

class cursors  
{
public:
	static void Init(std::string fileName);
	static void Draw(FPoint2D pos);
	static void Set(std::string type);
private:
	static std::map<std::string, Texture *> _names;
	static std::string _currentType;
};

#endif//cursors_h