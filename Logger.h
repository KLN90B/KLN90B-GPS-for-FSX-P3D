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
			class Logger : public WriterASCII,public SingletonStatic<Logger>
			{
			private:
				bool WriteStartHeader(const std::string& prjStr="");
				bool WriteEndHeader();

			public:
				Logger();
				~Logger();

				bool Start(const std::string& strFile,const std::string& prjStr);
				void FakeInit() {};
			};
		}
	}
}

//#define LOG	swan2::lib::core::Logger::Instance()

