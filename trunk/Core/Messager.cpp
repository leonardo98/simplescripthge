// Messager.cpp: implementation of the Messager class.

#include "Messager.h"
#include "Render.h"

Messager::Messager(std::string receiverName)
{
	_name = receiverName;
	List::iterator i;
	for (i = _receiver.begin(); i != _receiver.end() && (*i)->_name != receiverName; i++);
	if (i != _receiver.end()) {// уже есть с таким именем в списке
		LOG("есть уже Messager с таким именем");
		exit(-2);
	}
	_receiver.push_back(this);
}

Messager::Messager(TiXmlElement *xe)
{
	std::string receiverName = xe->Attribute("name");

	_name = receiverName;
	List::iterator i;
	for (i = _receiver.begin(); i != _receiver.end() && (*i)->_name != receiverName; i++);
	if (i != _receiver.end()) {// уже есть с таким именем в списке
		LOG("есть уже Messager с таким именем");
		exit(-2);
	}
	_receiver.push_back(this);
}

Messager::~Messager()
{
	List::iterator i;
	for (i = _receiver.begin(); i != _receiver.end() && *i != this; i++);
	_receiver.erase(i);
}

Messager::AllMessages Messager::_messages;

void Messager::SendMessage(const std::string &receiverName, const std::string &message)
{
	_messages.push_back(std::make_pair<std::string, std::string>(receiverName, message));
}

void Messager::CoreSendMessages() 
{
	AllMessages tmp = _messages;
	_messages.clear();
	for (AllMessages::iterator q = tmp.begin(), w = tmp.end(); q != w; q++) {
		List::iterator i, e;
		for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != q->first; i++);
		if (i != e) {
			(*i)->OnMessage(q->second);
		} else {
			// нет получателя в списке
		}		
	}
}

int Messager::SetValueF(const std::string &receiverName, const std::string &variableName, const float &value) {
	List::iterator i, e;
	for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != receiverName; i++);
	if (i == e) {
		return -1;// нет получателя в списке
	}
	(*i)->SetValue(variableName, value);
	return 0;
}

int Messager::SetValueS(const std::string &receiverName, const std::string &variableName, const std::string &value) {
	List::iterator i, e;
	for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != receiverName; i++);
	if (i == e) {
		return -1;// нет получателя в списке
	}
	(*i)->SetValue(variableName, value);
	return 0;
}

int Messager::SetValueB(const std::string &receiverName, const std::string &variableName, const bool &value) {
	List::iterator i, e;
	for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != receiverName; i++);
	if (i == e) {
		return -1;// нет получателя в списке
	}
	(*i)->SetValue(variableName, value);
	return 0;
}

float Messager::GetNumberValue(const std::string &receiverName, const std::string &variableName) {
	List::iterator i, e;
	for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != receiverName; i++);
	if (i == e) {
		return -1;// нет получателя в списке
	}
	return (*i)->GetNumberValue(variableName);
}

bool Messager::GetBoolValue(const std::string &receiverName, const std::string &variableName) {
	List::iterator i, e;
	for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != receiverName; i++);
	if (i == e) {
		return false;// нет получателя в списке
	}
	return (*i)->GetBoolValue(variableName);
}

std::string Messager::GetValue(const std::string &receiverName, const std::string &variableName) {
	List::iterator i, e;
	for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != receiverName; i++);
	if (i == e) {
		return "-1";// нет получателя в списке
	}
	return (*i)->GetValue(variableName);
}

bool Messager::CanCut(const std::string &message, const std::string &substr, std::string &result)
{
	unsigned int rr = message.find(substr);
	if (rr != 0) {
		return false;
	} else {
		result = message.substr(substr.size());
		return true;
	}
}

Messager::List Messager::_receiver;
