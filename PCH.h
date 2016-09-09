/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#pragma once

//////////////////////////////////////////////////////////////////////////
// System
#include <windows.h>
#include <exception>
//#include <filesystem>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <list>
#include <map>
#include <vector>
#include <cassert>
#include <process.h>
//#include <thread>
//#include <wininet.h>
#include <math.h>

#pragma comment(lib,"version.lib")
//#pragma comment(lib,"wininet.lib")
//#pragma comment(lib,"ws2_32.lib")

//////////////////////////////////////////////////////////////////////////
// FSX
#include "fslib.h"
#define SIMCONNECT_H_NOMANIFEST
#include "simconnect.h"

extern HMODULE gHModule;

//////////////////////////////////////////////////////////////////////////
// Lib

//#define LOGGING

const size_t MAX_BUFFER_LENGTH=4096;

// Core
#include "Lib/Core/Exception.h"
#include "Lib/Core/Helpers.h"
#include "Lib/Core/String.h"
//#include "Lib/Core/FileSystem.h"
#include "Lib/Core/IniFile.h"
#include "Lib/Core/VersionInfo.h"
//#include "Lib/Core/HTTPDownloader.h"
//#include "Lib/Core/DownloadManager.h"
#include "Lib/Core/DateTime.h"
#include "Lib/Core/WriterASCII.h"
#include "Lib/Core/Logger.h"

// FSX
#include "Lib/FSX/Measure.h"
#include "Lib/FSX/FSXKeyHandler.h"
#include "Lib/FSX/SimVars.h"
#include "Lib/FSX/SimConnectWrapper.h"

//////////////////////////////////////////////////////////////////////////
// Gauge
static enum GROUP_ID {
	GROUP_MENU
};

static enum EVENT_ID {
	EVENT_MENU_KLN,
	EVENT_MENU_HIDE_SHOW,
	EVENT_MENU_AFP_TO_FPL0,
	EVENT_MENU_AP_TOGGLE,
};

extern BOOL UseDefAP;


