#pragma once
#pragma warning(disable : 4800)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <map>

using namespace std;
#include "definitions.h"
#include "indexes.h"
#include "libraries\xorstr.h"

#include "libraries\vmt.h"
#include "../gpud3dhook/main.h"

extern std::ofstream errlog;
#define logerr(_x)		errlog<<XorStr(__FILE__)<<"("<<__LINE__<<")"<<XorStr(__FUNCTION__)<<": "<<XorStr(_x)<<"\r\n"

#include "structs\vector.h"
#include "structs\vmatrix.h"
#include "structs\usercmd.h"
#include "structs\playerinfo.h"
#include "structs\engine.h"
#include "structs\globals.h"
#include "structs\input.h"
#include "structs\panel.h"
#include "structs\surface.h"
#include "structs\client.h"
#include "structs\crc32.h"
#include "structs\quaternion.h"
#include "structs\modelinfo.h"
#include "structs\checksum_md5.h"
#include "libraries\interfaces.h"
#include "Utils.h"
#include "structs\baseentity.h"
#include "structs\cliententlist.h"
#include "structs\trace.h"
#include "structs\debugoverlay.h"
#include "structs\console.h"
#include "structs\event.h"
#include "structs\render.h"
#include "libraries\math.h"
