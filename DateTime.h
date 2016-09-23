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
			class DateTime
			{
			public:
				DateTime();
				~DateTime();

				std::string GetCurDate();
				std::string GetCurTime();
				std::string GetCurDateTime();
			};
		}
	}
}




