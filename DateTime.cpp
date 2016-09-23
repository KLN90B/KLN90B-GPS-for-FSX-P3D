/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;

DateTime::DateTime()
{

}

DateTime::~DateTime()
{

}

std::string DateTime::GetCurDate()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	char buf[MAX_BUFFER_LENGTH];
	sprintf_s(buf,MAX_BUFFER_LENGTH,"%02u.%02u.%4u",sysTime.wDay,sysTime.wMonth,sysTime.wYear);
	return std::string(buf);
}

std::string DateTime::GetCurTime()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	char buf[MAX_BUFFER_LENGTH];
	sprintf_s(buf,MAX_BUFFER_LENGTH,"%02u:%02u:%02u",sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
	return std::string(buf);
}

std::string DateTime::GetCurDateTime()
{
	return std::string(GetCurDate()+" "+GetCurTime());
}

