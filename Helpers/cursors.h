// cursors.h: interface for the cursors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CURSORS_INCLUDED_)
#define _CURSORS_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Core\types.h"

class cursors  
{
public:
	static void Init(std::string fileName);
	static void Draw(hgeVector pos);
	static void Set(std::string type);
private:
	static std::map<std::string, Texture *> _names;
	static std::string _currentType;
};

#endif // !defined(_CURSORS_INCLUDED_)
