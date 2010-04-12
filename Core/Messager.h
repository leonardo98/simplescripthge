// Messager.h: interface for the Messager class.

#if !defined(_Messager_INCLUDED_)
#define _Messager_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

//
// наследуемся от этого класса, если хотим обрабатывать сообщения
//

class Messager
{
public:

	// переопределяем этот метод обработки сообщений
	virtual void OnMessage(std::string message) = 0;
	// определяем свое имя получателя через конструктор ...
	Messager(std::string receiverName);
	// ... или имя должно быть задано в xml так <messager name="[ имя получателя ]"/>
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
