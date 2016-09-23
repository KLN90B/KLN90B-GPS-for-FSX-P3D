/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;

DownloadManager::DownloadManager()
{
}

DownloadManager::~DownloadManager()
{
}

void DownloadManager::Create()
{
}

HTTPDownloader* DownloadManager::CreateDownload()
{
	HTTPDownloader* pDL=new HTTPDownloader;

	downloadList.push_back(pDL);

	pDL->Create(this);

	return pDL;
}

void DownloadManager::RemoveDownload(HTTPDownloader* pDownload)
{
	auto it=std::find(downloadList.begin(),downloadList.end(),pDownload);

	if(it!=downloadList.end()) {
		downloadList.erase(it);
	}
}

void DownloadManager::Update()
{
	auto it=downloadList.begin();

	while(it!=downloadList.end()) {
		HTTPDownloader* pDL=*it;

		switch(pDL->GetState()) 	{
			case HTTP_STATE_NONE:
			case HTTP_STATE_WORKING:
				++it;
			continue;
			case HTTP_STATE_COMPLETE:
				pDL->OnComplete();
			break;
			case HTTP_STATE_ERROR:
				pDL->OnError();
			break;
			case HTTP_STATE_CANCELED:
				pDL->OnCancel();
			break;
		}

		it=downloadList.erase(it);

		pDL->Release();
	}
}

void DownloadManager::Release()
{
	for(std::list<HTTPDownloader*>::iterator it=downloadList.begin();it!=downloadList.end();) {
		HTTPDownloader* pDL=*it;

		it=downloadList.erase(it);

		pDL->Release();
	}

	delete this;
}

