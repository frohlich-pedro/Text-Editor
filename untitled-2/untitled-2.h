#pragma once

#include "resource.h"
#include <commdlg.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <numeric>

#define MAX_LOADSTRING 100

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_SAVE 3
#define IDM_FILE_EXIT 4

#define EC_LEFTMARGIN      0x0005
#define EC_RIGHTMARGIN     0x0005
#define EC_USEFONTINFO     0xffff