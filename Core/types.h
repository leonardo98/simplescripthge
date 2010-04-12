#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>

extern "C" 
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#include "hge.h"
#include "hgesprite.h"
#include "hgevector.h"
#include "hgefont.h"
#include "hgeparticle.h"

#include "tinyxml.h"
#include "tinystr.h"

#include "Object.h"
#include "Texture.h"

#pragma 

//typedef hgeSprite Texture;

#define LOG_NOTE 1
#define LOG_STRING(note, msg) Messager::SendMessage("Core", msg)