// Messager.h: interface for the Messager class.

#if !defined(_Messager_INCLUDED_)
#define _Messager_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

class Messager
{

public:

	// ќписываем его если хотим обрабатывать сообщени€
	virtual void OnMessage(std::string message) = 0;

	Messager(std::string receiverName);

	Messager(TiXmlElement *xe);

	~Messager();

	static int SendMessage(std::string receiverName, std::string message);

	static bool CanCut(std::string &message, std::string substr);

private:

	typedef std::list<Messager *> List;

	static List _receiver;

	std::string _name;

};

#endif // !defined(_Messager_INCLUDED_)
