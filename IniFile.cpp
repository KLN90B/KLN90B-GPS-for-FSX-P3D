/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;

std::string IniFile::GetValue(const std::string& KeyName,const std::string& SectionName,const std::string& FileName)
{
	char ret[MAX_BUFFER_LENGTH];
	GetPrivateProfileString(SectionName.c_str(),KeyName.c_str(),"",ret,MAX_BUFFER_LENGTH,FileName.c_str());
	return std::string(ret);
}

bool IniFile::GetBoolean(const std::string& KeyName,const std::string& SectionName,bool defvalue,const std::string& FileName)
{
	bool ret=defvalue;
	std::string retstr=GetValue(KeyName,SectionName,FileName);
	if(retstr!="")
		ret=ParseStringForBool(retstr);			
	return ret;
}

bool IniFile::SetValue(const std::string& KeyName,const std::string& Value,const std::string& SectionName,const std::string& FileName)
{
	return WritePrivateProfileString(SectionName.c_str(),KeyName.c_str(),Value.c_str(),FileName.c_str());
}

