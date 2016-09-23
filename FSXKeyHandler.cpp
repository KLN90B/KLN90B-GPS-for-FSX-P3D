/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

namespace swan2
{
	namespace lib
	{
		namespace fsx
		{
			extern bool FSAPI fnFSXKeyHandler(ID32 event,UINT32 evdata,PVOID userdata)
			{
				FSXKeyHandler* pMyHandle=(FSXKeyHandler*)userdata;
				if(!pMyHandle->IsStarted())
					return false;
				return pMyHandle->fnKeyHandler(event,evdata);
			}

			FSXKeyHandler::FSXKeyHandler()
			{
				isStarted=false;
			}

			FSXKeyHandler::~FSXKeyHandler()
			{
				Stop();
			}

			void FSXKeyHandler::Start()
			{
				Stop();
				if(isStarted)
					return;
				register_key_event_handler((GAUGE_KEY_EVENT_HANDLER)fnFSXKeyHandler,this);
				isStarted=true;
			}

			void FSXKeyHandler::Stop()
			{
				if(!isStarted)
					return;
				unregister_key_event_handler((GAUGE_KEY_EVENT_HANDLER)fnFSXKeyHandler,this);
				isStarted=false;
			}
		}
	}
}
