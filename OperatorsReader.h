// OperatorsReader.h: interface for the OperatorsReader class.

#if !defined(_OPERATORSREADER_INCLUDED_)
#define _OPERATORSREADER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LexemsReader.h"
#include "Variable.h"

class OperatorsReader 
	: public LexemsReader  
{
protected:
	void ReadNextOperator();
	void Assigning();
	void ReadLogicExpression();
	void IfElse();
	void SendTo();
	static enum WORD {
		WORD_EQ,  // ���������� 2 ������ �� ������� ����� �� ��������� � ����� ������ ��� �� ���� "true" ��� "false" (-2 / +1)
		WORD_NEQ, // ���������� 2 ������ �� ������� ����� �� ����������� � ����� ������ ��� �� ���� "false" ��� "true" (-2 / +1)
		WORD_JMP, // ����������� �������
		WORD_JZ,  // ��������� ���� �� ������� ����� "false" (��� �������), �������� - ����� ������ � ������� ���� ������� (-1)
		WORD_MOV, // ���������� �������� �� ����� � ����������, �������� - ������ ���������� � ���-������� (_hash) (-1)
		WORD_SND, // ���������� � ������� ����� ������ ����������, ������� ��� ��� (2-��) ������ ����� (-2)
		WORD_AND, // ���������� � - ��������� ���� �� ������� ����� ��� "true" - �������� �� "true" ����� "false" (-2 / +1)
		WORD_CST, // �������� �������� ������ �� ����, �������� - ������ � ������� ������ (_data) (+1)
		WORD_CVR, // �������� �������� ���������� �� ����, �������� - ������ ���������� � ���-������� (_hash) (+1)
		WORD_EXIT // ��������� ���������� ����-����
	};
	struct Word {
		WORD op;
		int index;
		Word(WORD op, int index = -1) {
			this->op = op;
			this->index = index;
		}
		Word() {}
	};
	static std::vector<std::string> _stek;
	std::vector<Word> _code;
	std::vector<Variable::Ptr> _hash;
	std::vector<std::string> _data;
};

#endif // !defined(_OPERATORSREADER_INCLUDED_)
