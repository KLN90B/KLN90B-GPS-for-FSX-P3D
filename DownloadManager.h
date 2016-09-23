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
			class HTTPDownloader;

			class DownloadManager
			{
			public:
				DownloadManager();
				virtual ~DownloadManager();

				void			Create();
				HTTPDownloader*	CreateDownload();
				void			RemoveDownload(HTTPDownloader* pDownload);
				void			Update();
				void			Release();

			private:
				std::list<HTTPDownloader*>	downloadList;
			};		
		}
	}
}


