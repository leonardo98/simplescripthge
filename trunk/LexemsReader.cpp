// LexemsReader.cpp: implementation of the LexemsReader class.

#include "Lexem.h"
#include "LexemsReader.h"
#include "Messager.h"


LexemsReader::LexemsReader()
{
	_skipReadingNextLexem = false;
}

void LexemsReader::ReadFile(std::string fileName)
{
	_fileLines.clear();
	std::fstream file; 
    file.open( fileName.c_str(), std::ios::in );
    if (!file)
    {
		//printf(("file: " + fileName + " not found\n").c_str());
		return;
    }   
	while (!file.eof()) {
		char line[512];
		file.getline(line, 512);
		_fileLines.push_back(line);
	}
    file.close();
	//printf("\n");
	_currentLine = 0;
	_currentPosition = 0;
	_currentChar = 0;
	NextChar();
}

void LexemsReader::ReadText(char *text)
{
	_fileLines.clear();
	int i = 0;
	while (text[i] != 0) {
		char line[4096];
		int c = 0;
		while (text[i] != 10 && text[i] != 10 && text[i] != 0) {
			line[c++] = text[i++];
		}
		if (c > 0) {
			_fileLines.push_back(line);
		}
	}
	_currentLine = 0;
	_currentPosition = 0;
	_currentChar = 0;
	NextChar();
}

void LexemsReader::Error(std::string error)
{
	/*printf((_fileLines[_errorLine] + "\n").c_str());
	for (int i = 0; i < _errorPosition; i++) {
		printf(" ");
	}
	printf("^\n");
	printf(error.c_str());
	printf("\n");*/
	std::string s = _fileLines[_errorLine] + "\n";
	for (int i = 0; i < _errorPosition; i++) {
		s += " ";
	}
	s += "^\n";
	s += error;
	s += "\n";
	Messager::SendMessage("log", s);
	exit(-1);
}

void LexemsReader::NextChar()
{
	if (_currentLine < _fileLines.size() && _currentPosition < _fileLines[_currentLine].size()) {
		_currentChar =  _fileLines[_currentLine][_currentPosition];
		_currentPosition++;
		if (_currentPosition >= _fileLines[_currentLine].size()) {
			_currentPosition = 0;
			_currentLine++;
		}
	} else {
		_currentChar = END_FILE;
	}
}

void LexemsReader::NextLexem()
{
	if (_skipReadingNextLexem) {
		_skipReadingNextLexem = false;
		return;
	}
	_errorLine = _currentLine;
	_errorPosition = _currentPosition;
	while (_currentChar <= ' ' &&_currentChar != 0) {
		NextChar();
	}
	if (_currentChar == END_FILE) { 
		// кончился текст
		_currentLexem = Lexem();
		return;
	} else if (_currentChar == '"') {
		// строка в кавычах
		std::string text = "";
		NextChar();
		while (_currentChar != '"') {
			text += _currentChar;
			NextChar();
		}
		NextChar();
		_currentLexem = Lexem(ltStrConstant, text);
		return;
	} else if (_currentChar == '{' || _currentChar == '}') { 
		// скобки составного оператора
		std::string text(1, _currentChar);
		NextChar();
		_currentLexem = Lexem(ltResWord, text);
		return;
	} else if (_currentChar == '(' || _currentChar == ')') { 
		// скобки внутри условного оператора
		std::string text(1, _currentChar);
		NextChar();
		_currentLexem = Lexem(ltOperator, text);
		return;
	} else if (_currentChar == ';') { 
		// разделитель между операторами
		NextChar();
		_currentLexem = Lexem(ltDevider, ";");
		return;
	} else if (_currentChar == '$') {
		 // идентификатор переменной
		NextChar();
		if (!((_currentChar >= 'a' && _currentChar <= 'z') || (_currentChar >= 'A' && _currentChar <= 'Z'))) {
			Error("first simbol must be letter");
		}
		std::string text = "$";
		text += _currentChar;
		NextChar();
		while ((_currentChar >= 'a' && _currentChar <= 'z') 
			   || (_currentChar >= 'A' && _currentChar <= 'Z')
			   || (_currentChar >= '0' && _currentChar <= '9')
			   || _currentChar >= '_') {
			text += _currentChar;
			NextChar();
		}
		_currentLexem = Lexem(ltVariable, text);
		return;
	} else if (_currentChar == '=') {
		
		NextChar();
		if (_currentChar == '=') {
			 // логический оператор "равно"
			NextChar();
			_currentLexem = Lexem(ltOperator, "==");
			return;
		} else {
			 // символ оператора присваивания
			_currentLexem = Lexem(ltOperator, "=");
			return;
		}
	} else if (_currentChar == '!') {
		NextChar();
		if (_currentChar == '=') {
			// логический оператор "не равно"
			NextChar();
			_currentLexem = Lexem(ltOperator, "!=");
			return;
		} else {
			Error("не известный оператор");
			_currentLexem = Lexem();
			return;
		}
	} else if (_currentChar == 'a') {
		std::string text = "a";
		for (int i = 0; i < 2; i++) {
			NextChar();
			text += _currentChar;
		}
		if (text == "and") {
			// логический оператор "и"
			NextChar();
			_currentLexem = Lexem(ltOperator, "&&");
			return;
		} else {
			Error("не известный оператор");
			_currentLexem = Lexem();
			return;
		}
	} else if ((_currentChar >= 'a' && _currentChar <= 'z') 
			   || (_currentChar >= 'A' && _currentChar <= 'Z')
			   || (_currentChar >= '0' && _currentChar <= '9')
			   || _currentChar >= '_') {
		std::string text = "";
		while ((_currentChar >= 'a' && _currentChar <= 'z') 
			   || (_currentChar >= 'A' && _currentChar <= 'Z')
			   || (_currentChar >= '0' && _currentChar <= '9')
			   || _currentChar >= '_') {
			text += _currentChar;
			NextChar();
		}
		if (text == "if" || text == "else" || text == "send" || text == "to") {
			_currentLexem = Lexem(ltResWord, text);
			return;
		} else {
			Error("unknown operator");
			_currentLexem = Lexem();
			return;
		}
	} else {
		Error("unexpected simbol");
		_currentLexem = Lexem();
		return;
	}
	_currentLexem = Lexem();
	return;
}
