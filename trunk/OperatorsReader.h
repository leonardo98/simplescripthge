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
		WORD_EQ,  // сравниваем 2 строки на вершине стека на равенство и ложим вместо них на стек "true" или "false" (-2 / +1)
		WORD_NEQ, // сравниваем 2 строки на вершине стека на НЕравенство и ложим вместо них на стек "false" или "true" (-2 / +1)
		WORD_JMP, // безусловный вариант
		WORD_JZ,  // переходим если на вершине стека "false" (его убираем), параметр - номер строки в массиве куда переход (-1)
		WORD_MOV, // записываем значение со стека в переменную, параметр - индекс переменной в ХЕШ-таблице (_hash) (-1)
		WORD_SND, // отправляем с вершины стека строку получателю, который под ней (2-ой) сверху стека (-2)
		WORD_AND, // логическое И - проверяем если на вершине стека два "true" - заменяем на "true" иначе "false" (-2 / +1)
		WORD_CST, // копируем значение строки на стек, параметр - строка в таблице данных (_data) (+1)
		WORD_CVR, // копируем значение переменной на стек, параметр - индекс переменной в ХЕШ-таблице (_hash) (+1)
		WORD_EXIT // завершаем выполнение байт-кода
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
