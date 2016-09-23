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
		namespace fsx
		{
			class FSXKeyHandler
			{
			private:
				friend bool FSAPI fnFSXKeyHandler(ID32 event,UINT32 evdata,PVOID userdata);
				bool isStarted;

			protected:
				virtual bool fnKeyHandler(ID32 event,UINT32 evdata)=0;

			public:
				FSXKeyHandler();
				virtual ~FSXKeyHandler();

				void Start();
				void Stop();
				bool IsStarted()
				{
					return isStarted;
				}
			};
		}
	}
}

