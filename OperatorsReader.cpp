// OperatorsReader.cpp: implementation of the OperatorsReader class.

#include "OperatorsReader.h"
#include "Variables.h"

void OperatorsReader::ReadNextOperator()
{
	NextLexem();
	if (_currentLexem.Type() == ltDevider && _currentLexem.Text() == ";") { // пустой оператор
		return;
	} else if (_currentLexem.Type() == ltVariable) { // присваивание
		Assigning();
	} else if (_currentLexem.Type() == ltResWord && _currentLexem.Text() == "if") { // условный оператор
		IfElse();
	} else if (_currentLexem.Type() == ltResWord && _currentLexem.Text() == "send") { // send - to встроеный оператор
		SendTo();
	} else if (_currentLexem.Type() == ltResWord && _currentLexem.Text() == "{") { // составной оператор
		while (true) {
			ReadNextOperator();
			NextLexem();
			if (_currentLexem.Type() == ltResWord && _currentLexem.Text() == "}") {
				return;
			} else { _skipReadingNextLexem = true; }
			if (_currentLexem.Type() == ltUnknown) {
				Error("unexpected end of file.");
				return; 
			}
		}
		if (!(_currentLexem.Type() == ltResWord && _currentLexem.Text() == "}")) {
			Error("expected: '}'");
		}
	} else {
		Error("unexpected word");
	}
}

void OperatorsReader::Assigning()
{
	std::string variableName = _currentLexem.Text();
	NextLexem();
	if (_currentLexem.Type() == ltOperator && _currentLexem.Text() == "=") {
		NextLexem();
		if (_currentLexem.Type() == ltStrConstant) {
			std::string value = _currentLexem.Text();
			NextLexem();
			if (_currentLexem.Type() == ltDevider && _currentLexem.Text() == ";") {
				// генерация кода
				_data.push_back(value);
				int code = _data.size() - 1;
				_hash.push_back(Variables::GetLink(variableName));
				int variable = _hash.size() - 1;
				_code.push_back(Word(WORD_CST, code));
				_code.push_back(Word(WORD_MOV, variable));
			} else {
				Error("expected: ;");
			}
		} else {
			Error("expected: string expression");
		}
	} else {
		Error("expected: =");
	}
}

void OperatorsReader::ReadLogicExpression()
{
	NextLexem();
	if (_currentLexem.Type() == ltVariable) {
		std::string variableName = _currentLexem.Text();
		NextLexem();
		if (_currentLexem.Type() == ltOperator && (_currentLexem.Text() == "!=" || _currentLexem.Text() == "==")) {
			Word op;
			if (_currentLexem.Text() == "!=") {
				op = WORD_NEQ;
			} else if (_currentLexem.Text() == "==") {
				op = WORD_EQ;
			}
			NextLexem();
			if (_currentLexem.Type() == ltStrConstant) {
				std::string expression = _currentLexem.Text();
				// генерация кода
				_data.push_back(expression);
				int code = _data.size() - 1;
				_hash.push_back(Variables::GetLink(variableName));
				int variable = _hash.size() - 1;
				_code.push_back(Word(WORD_CVR, variable));
				_code.push_back(Word(WORD_CST, code));
				_code.push_back(op);
				NextLexem();
				if (_currentLexem.Type() == ltOperator && _currentLexem.Text() == "&&") {
					// генерация кода
					ReadLogicExpression();
					_code.push_back(Word(WORD_AND));
				}
			} else {
				Error("expected: string");
			}
		} else {
			Error("expected: == | !=");
		}
	} else {
		Error("expected: variable's name");
	}
}

void OperatorsReader::IfElse()
{
	NextLexem();
	if (_currentLexem.Type() == ltOperator && _currentLexem.Text() == "(") {
		ReadLogicExpression();
		if (_currentLexem.Type() == ltOperator && _currentLexem.Text() == ")") {
			// генерируем код
			_code.push_back(Word(WORD_JZ));
			int jump = _code.size() - 1;
			ReadNextOperator();
			_code.push_back(Word(WORD_JMP));
			_code[jump].index = _code.size();
			int to_else_jump = _code.size() - 1;
			NextLexem();
			if (_currentLexem.Type() == ltResWord && _currentLexem.Text() == "else") {
				ReadNextOperator();
			} else {
				_skipReadingNextLexem = true;
			}
			_code[to_else_jump].index = _code.size();
		} else {
			Error("expected: )");
		}
	} else {
		Error("expected: (");
	}
}

void OperatorsReader::SendTo()
{
	NextLexem();
	if (_currentLexem.Type() == ltStrConstant) {
		std::string message = _currentLexem.Text();
		NextLexem();
		if (_currentLexem.Type() == ltResWord && _currentLexem.Text() == "to") {
			NextLexem();
			if (_currentLexem.Type() == ltStrConstant) {
				std::string receiver = _currentLexem.Text();
				NextLexem();
				if (_currentLexem.Type() == ltDevider && _currentLexem.Text() == ";") {
					// генерация кода
					_data.push_back(message);
					int msg_code = _data.size() - 1;
					_data.push_back(receiver);
					int rsvr_code = _data.size() - 1;
					_code.push_back(Word(WORD_CST, rsvr_code));
					_code.push_back(Word(WORD_CST, msg_code));
					_code.push_back(WORD_SND);
				} else {
					Error("expected: ;");
				}
			} else {
				Error("expected: string expression");
			}
		} else {
			Error("expected: to");
		}
	} else {
		Error("expected: string expression");
	}
}

std::vector<std::string> OperatorsReader::_stek;