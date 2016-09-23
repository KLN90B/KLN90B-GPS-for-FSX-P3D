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
			#define HTTP_BUFFER_SIZE		(16384)

			enum
			{
				HTTP_STATE_WORKING = 0,
				HTTP_STATE_COMPLETE,
				HTTP_STATE_CANCELED,
				HTTP_STATE_ERROR,
				HTTP_STATE_NONE,
			};

			class DownloadManager;

			class HTTPDownloader 
			{
			public:
				HTTPDownloader();
				virtual ~HTTPDownloader();

				int					Create(DownloadManager* pParent);
				int					Download(const char* szURL,const char* szDestination);
				void				Cancel();
				int					GetState()					{ return curState; };
				int					GetFileSize()		const	{ return fileSize; };
				const std::string&	GetURL()			const	{ return urlPath; };
				const std::string&	GetDstFileName()	const	{ return dstFile; };
				void				Release();

				void				OnError();
				void				OnComplete();
				void				OnCancel();

			private:

				static DWORD		DownloadProc(HTTPDownloader* _this);
				void				CreateThread();
				DWORD				DoDownload();
				void				PrepareBuffer();

				std::string			urlPath;
				std::string			dstFile;
				HANDLE				dlThread;
				HINTERNET			inetHandle;
				HINTERNET			urlHandle;

				unsigned char*		dlBuffer;
				int					fileSize;

				volatile int		curState;
				volatile bool		isContinue;

				DownloadManager*	parentManager;
			};
		}
	}
}


