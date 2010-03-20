// Messager.cpp: implementation of the Messager class.

#include "Messager.h"


Messager::Messager(std::string receiverName)
{
	_name = receiverName;
	List::iterator i;
	for (i = _receiver.begin(); i != _receiver.end() && (*i)->_name != receiverName; i++);
	if (i != _receiver.end()) {// ��� ���� � ����� ������ � ������
		Messager::SendMessage("log", "���� ��� Messager � ����� ������");
		exit(-2);
	}
	_receiver.push_back(this);
}

Messager::Messager(TiXmlElement *xe)
{
	std::string receiverName = xe->FirstChildElement("messager")->Attribute("name");

	_name = receiverName;
	List::iterator i;
	for (i = _receiver.begin(); i != _receiver.end() && (*i)->_name != receiverName; i++);
	if (i != _receiver.end()) {// ��� ���� � ����� ������ � ������
		Messager::SendMessage("log", "���� ��� Messager � ����� ������");
		exit(-2);
	}
	_receiver.push_back(this);
}

Messager::~Messager()
{
	List::iterator i;
	for (i = _receiver.begin(); i != _receiver.end() && *i != this; i++);
	if (i == _receiver.end()) {// ��� � ������ (��� ������)
		Messager::SendMessage("log", "������ ��� Messager � ����� ������");
		exit(-3);
	}
	_receiver.erase(i);
}

int Messager::SendMessage(std::string receiverName, std::string message)
{
	List::iterator i, e;
	for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != receiverName; i++);
	if (i == e) {
		return -1;// ��� ���������� � ������
	}
	(*i)->OnMessage(message);
	return 0;
}

bool Messager::CanCut(std::string &message, std::string substr)
{
	if (message.find(substr) == std::string::npos) {
		return false;
	} else {
		message.replace(0, substr.size(), "");
		return true;
	}
}

Messager::List Messager::_receiver;
