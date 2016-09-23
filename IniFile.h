/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#pragma once

namespace swan2
{
	namespace lib
	{
		namespace core 
		{
			class IniFile
			{
			public:
				static std::string				GetValue(const std::string& KeyName,const std::string& SectionName,const std::string& FileName);
				static bool						GetBoolean(const std::string& KeyName,const std::string& SectionName,bool defvalue,const std::string& FileName);
				static bool						SetValue(const std::string& KeyName,const std::string& Value,const std::string& SectionName,const std::string& FileName);
			};
		}
	}
}



