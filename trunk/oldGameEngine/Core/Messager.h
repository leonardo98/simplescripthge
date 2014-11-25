// Messager.h: interface for the Messager class.
#ifndef MYENGINE_MESSAGER_H
#define MYENGINE_MESSAGER_H

#include "types.h"

//
// наследуемся от этого класса, если хотим обрабатывать сообщения
//

class Messager
{
public:

	// переопределяем этот метод обработки сообщений
	virtual void OnMessage(const std::string &message){};
	// ... или метод для изменения значения переменных
	virtual void SetValue(const std::string &variableName, const float &value){};
	virtual void SetValue(const std::string &variableName, const bool &value){};
	virtual void SetValue(const std::string &variableName, const std::string &value){};

	virtual float GetNumberValue(const std::string &variableName){return -1;};
	virtual bool GetBoolValue(const std::string &variableName){return false;};
	virtual std::string GetValue(const std::string &variableName){return "-1";};

	virtual std::string GetMyName(){return _name;};

	// определяем свое имя получателя через конструктор ...
	Messager(std::string receiverName);
	// ... или имя должно быть задано в xml так <messager name="[ имя получателя ]"/>
	Messager(TiXmlElement *xe);

	~Messager();
	static void SendMessage(const std::string &receiverName, const std::string &message, float delay = 0.f);

	static int SetValueF(const std::string &receiverName, const std::string &variableName, const float &value);
	static int SetValueB(const std::string &receiverName, const std::string &variableName, const bool &value);
	static int SetValueS(const std::string &receiverName, const std::string &variableName, const std::string &value);

	static float GetNumberValue(const std::string &receiverName, const std::string &variableName);
	static bool GetBoolValue(const std::string &receiverName, const std::string &variableName);
	static std::string GetValue(const std::string &receiverName, const std::string &variableName);

	static bool CanCut(const std::string &message, const std::string &substr, std::string &result);

private:
	typedef std::list<Messager *> List;
	static List _receiver;
	std::string _name;
	struct Letter{
		std::string receiver;
		std::string message;
		float timer;
	};
	typedef std::list<Letter *> AllMessages;
	static AllMessages _incoming;
	static AllMessages _messages;
	static void CoreSendMessages(float dt); // только для вызова из ядра
	friend class Core;
};

#endif//MYENGINE_MESSAGER_H