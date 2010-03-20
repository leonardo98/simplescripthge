// LexemsReader.h: interface for the LexemsReader class.

#if !defined(_LEXEMSREADER_INCLUDED_)
#define _LEXEMSREADER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Lexem.h"


static enum SCRIPT_CONSTANT {
	END_FILE = 0
};

class LexemsReader  
{
private:
	int _currentLine;
	int _currentPosition;
	int _errorLine;
	int _errorPosition;
	char _currentChar;
	typedef std::vector<std::string> FileLines;
	FileLines _fileLines;

protected:

	Lexem _currentLexem;
	bool _skipReadingNextLexem;
	LexemsReader();
	void ReadFile(std::string fileName);
	void ReadText(char *text);
	void Error(std::string);
	void NextLexem();
	void NextChar();
};

#endif // !defined(_LEXEMSREADER_INCLUDED_)
