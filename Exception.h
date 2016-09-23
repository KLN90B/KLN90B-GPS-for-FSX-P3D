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
			class Exception : public std::exception
			{
			private:
				std::string message;
				std::string fileName;
				std::string functionName;
				long lineNumber;

			public:
				Exception(std::string msg,std::string filename,std::string func,long line) :
					exception(),
					message(msg),
					fileName(filename),
					functionName(func),
					lineNumber(line)
				{
#ifdef _DEBUG
					if(IsDebuggerPresent())
						_CrtDbgBreak();
#endif
				}

				const char* what() const
				{
					std::stringstream msg;
					msg << "Message : " << message << std::endl;
					msg << "File    : " << fileName << std::endl;
					msg << "Function: " << functionName << std::endl;
					msg << "Line    : " << lineNumber << std::endl;
					return msg.str().c_str();
				}

			};
		}
	}
}

#define EXCEPT(str)			throw swan2::lib::core::Exception(str,__FILE__,__FUNCTION__,__LINE__)
