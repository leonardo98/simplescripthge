// Messager.h: interface for the Messager class.

#if !defined(_Messager_INCLUDED_)
#define _Messager_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

//
// ����������� �� ����� ������, ���� ����� ������������ ���������
//

class Messager
{
public:

	// �������������� ���� ����� ��������� ���������
	virtual void OnMessage(const std::string &message){};
	// ... ��� ����� ��� ��������� �������� ����������
	virtual void SetValue(const std::string &variableName, const float &value){};
	virtual void SetValue(const std::string &variableName, const bool &value){};
	virtual void SetValue(const std::string &variableName, const std::string &value){};

	virtual float GetNumberValue(const std::string &variableName){return -1;};
	virtual bool GetBoolValue(const std::string &variableName){return false;};
	virtual std::string GetValue(const std::string &variableName){return "-1";};

	virtual std::string GetMyName(){return _name;};

	// ���������� ���� ��� ���������� ����� ����������� ...
	Messager(std::string receiverName);
	// ... ��� ��� ������ ���� ������ � xml ��� <messager name="[ ��� ���������� ]"/>
	Messager(TiXmlElement *xe);

	~Messager();
	static void SendMessage(const std::string &receiverName, const std::string &message);
	static void CoreSendMessages(); // ������ ��� ������ �� ����

	static int SetValue(const std::string &receiverName, const std::string &variableName, const float &value);
	static int SetValue(const std::string &receiverName, const std::string &variableName, const bool &value);
	static int SetValue(const std::string &receiverName, const std::string &variableName, const std::string &value);

	static float GetNumberValue(const std::string &receiverName, const std::string &variableName);
	static bool GetBoolValue(const std::string &receiverName, const std::string &variableName);
	static std::string GetValue(const std::string &receiverName, const std::string &variableName);

	static bool CanCut(const std::string &message, const std::string &substr, std::string &result);

private:
	typedef std::list<Messager *> List;
	static List _receiver;
	std::string _name;
	typedef std::vector<std::pair<std::string, std::string> > AllMessages;
	static AllMessages _messages;
};

#endif // !defined(_Messager_INCLUDED_)
