// Variables.cpp: implementation of the Variables class.

#include "Variables.h"


void Variables ::Set(std::string variableName, std::string value)
{
	_map[variableName].SetValue(value);
}

std::string Variables ::Get(std::string variableName)
{
	return _map[variableName].GetValue();
}

Variable::Ptr Variables ::GetLink(std::string variableName)
{
	return _map[variableName].GetAdress();
}

void Variables ::Remove(std::string variableName)
{
	_map.erase(variableName);
}

std::map<std::string, Variable> Variables ::_map;
