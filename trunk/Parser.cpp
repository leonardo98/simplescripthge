// Parser.cpp: implementation of the Parser class.

#include "Parser.h"
#include "Messager.h"

Parser::Parser()
{
	_code.push_back(Word(WORD_EXIT));
}

Parser::Parser(TiXmlElement *script)
{
	if (script != NULL) {
		TiXmlText *text = (TiXmlText *)script->FirstChild();
		if (text != NULL) {
			Read((char *)text->Value());
			return;
		}
	} 
	_code.push_back(Word(WORD_EXIT));
}

void Parser::Read(char *text)
{
	ReadText(text);
	_code.clear();
	ReadNextOperator();
	_code.push_back(Word(WORD_EXIT));
}

void Parser::Read(std::string fileName)
{
	ReadFile(fileName);
	ReadNextOperator();
	_code.push_back(Word(WORD_EXIT));
	printf("Done.\n");
}

void Parser::Execute()
{
	int cp = 0;
	do {
		Word w = _code[cp];
		if (w.op == WORD_EXIT) {
			return;
		} else if (w.op == WORD_JZ) {
			if (_stek.back() == "false") {
				cp = w.index;
			} else {
				cp++;
			}
			_stek.pop_back();
		} else if (w.op == WORD_JMP) {
			cp = w.index;
		} else if (w.op == WORD_CST) {
			_stek.push_back(_data[w.index]);
			cp++;
		} else if (w.op == WORD_CVR) {
			_stek.push_back(_hash[w.index]->GetValue());
			cp++;
		} else if (w.op == WORD_MOV) {
			_hash[w.index]->SetValue(_stek.back());
			_stek.pop_back();
			cp++;
		} else if (w.op == WORD_SND) {
			std::string msg = _stek.back();
			_stek.pop_back();
			std::string rsvr = _stek.back();
			_stek.pop_back();
			Messager::SendMessage(rsvr, msg);
			cp++;
		} else if (w.op == WORD_EQ) {
			std::string one = _stek.back();
			_stek.pop_back();
			std::string two = _stek.back();
			_stek.pop_back();
			if (one == two) {
				_stek.push_back("true");
			} else {
				_stek.push_back("false");
			}
			cp++;
		} else if (w.op == WORD_NEQ) {
			std::string one = _stek.back();
			_stek.pop_back();
			std::string two = _stek.back();
			_stek.pop_back();
			if (one != two) {
				_stek.push_back("true");
			} else {
				_stek.push_back("false");
			}
			cp++;
		} else if (w.op == WORD_AND) {
			std::string one = _stek.back();
			_stek.pop_back();
			std::string two = _stek.back();
			_stek.pop_back();
			if (one == "true" && two == "true") {
				_stek.push_back("true");
			} else {
				_stek.push_back("false");
			}
			cp++;
		} else {Error("unknown byte-code");}
	} while (true);
}