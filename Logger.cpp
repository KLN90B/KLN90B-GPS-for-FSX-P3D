/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;

std::string DEFAULT_LOG_FILE="AN2.txt";

Logger::Logger()
{
#ifdef LOGGING
	VersionInfo ver(GetModuleHandle(NULL));
	char ret[MAX_BUFFER_LENGTH];
	ver.AsString(ret);
	std::string v=ver.GetProductName()+std::string(", version ")+std::string(ret); 
	Start(DEFAULT_LOG_FILE,v);
#endif
}

Logger::~Logger()
{
#ifdef LOGGING
	this->WriteEndHeader();
	this->Close();
#endif
}

bool Logger::WriteStartHeader(const std::string& prjStr)
{
#ifdef LOGGING
	DateTime dt;
	Write("============================================================================\n");
	Write("  %s\n",prjStr.data());
	Write("  Log created on %s, at %s\n",dt.GetCurDate().c_str(),dt.GetCurTime().c_str());
	Write("============================================================================\n\n");
#endif
	return true;
}

bool Logger::WriteEndHeader()
{
#ifdef LOGGING
	DateTime dt;
	Write("\n\n============================================================================\n");
	Write("  Log closed on %s, at %s\n",dt.GetCurDate().c_str(),dt.GetCurTime().c_str());
	Write("============================================================================\n\n");
#endif
	return true;
}

bool Logger::Start(const std::string& strFile,const std::string& prjStr)
{
#ifdef LOGGING
	this->InitFile(strFile);
	this->WriteStartHeader(prjStr);
#endif
	return true;
}

