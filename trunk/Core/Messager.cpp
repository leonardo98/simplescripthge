// Messager.cpp: implementation of the Messager class.

#include "Messager.h"
#include "Render.h"
#include "Core.h"

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

Messager::AllMessages Messager::_incoming;
Messager::AllMessages Messager::_messages;

void Messager::SendMessage(const std::string &receiverName, const std::string &message, float delay)
{
	Letter *l = new Letter();
	l->receiver = receiverName;
	l->message = message;
	l->timer = delay;
	_incoming.push_back(l);
}

void Messager::CoreSendMessages(float dt) 
{
	for (AllMessages::iterator i = _incoming.begin(), e = _incoming.end(); i != e; ++i) {
		_messages.push_back(*i);
	}
	_incoming.clear();
	for (AllMessages::iterator q = _messages.begin(), w = _messages.end(); q != w;) {
		(*q)->timer -= dt;
		if ((*q)->timer < 0.f) {
			if ((*q)->receiver.substr(0, 4) == "lua:") {
				Core::DoLua(((*q)->receiver.substr(4) + "(" + (*q)->message + ");").c_str());
			} else if ((*q)->receiver == "Core") {
				Core::OnMessage((*q)->message);
			} else {
				List::iterator i, e;
				for (i = _receiver.begin(), e = _receiver.end(); i != e && (*i)->_name != (*q)->receiver; i++);
				if (i != e) {
					(*i)->OnMessage((*q)->message);
				} else {
					LOG("Receiver - not found: " + (*q)->receiver);
				}		
			}
			delete (*q);
			q = _messages.erase(q);
		} else {
			++q;
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
