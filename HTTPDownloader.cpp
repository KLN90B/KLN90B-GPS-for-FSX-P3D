/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;

HTTPDownloader::HTTPDownloader() : 
	dlThread(0),
	inetHandle(0),
	urlHandle(0),
	fileSize(0),
	dlBuffer(0),
	curState(HTTP_STATE_NONE),
	parentManager(0)
{
}

HTTPDownloader::~HTTPDownloader()
{
}

int HTTPDownloader::Create(DownloadManager* pParent)
{
	parentManager=pParent;
	return 1;
}

int HTTPDownloader::Download(const char* szURL,const char* szDestination)
{
	urlPath=szURL;
	dstFile=szDestination;
	isContinue=1;
	CreateThread();
	return 1;
}

void HTTPDownloader::Cancel()
{
	isContinue=0;
}

DWORD HTTPDownloader::DownloadProc(HTTPDownloader* _this)
{
	_this->DoDownload();
	return 0;
}

void HTTPDownloader::CreateThread()
{
	DWORD dwThreadId=0;
	dlThread=::CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadProc,this,0,&dwThreadId);
}

DWORD HTTPDownloader::DoDownload()
{
	curState=HTTP_STATE_WORKING;
	std::string useragent = "swan2.dll/" + (std::string)VERSION_FILESTR;
	inetHandle=InternetOpen((LPCSTR)useragent.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,0,0,0);

	if(!inetHandle) {
		curState=HTTP_STATE_ERROR;
		return 1;
	}

	if(!isContinue) {
		curState=HTTP_STATE_CANCELED;
		return 1;
	}

	DWORD dwFlags=INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_NO_UI|INTERNET_FLAG_RELOAD;
	urlHandle=InternetOpenUrl(inetHandle,urlPath.c_str(),0,0,dwFlags,0);

	if(!urlHandle) {
		curState=HTTP_STATE_ERROR;
		return 1;
	}

	if(!isContinue) {
		curState=HTTP_STATE_CANCELED;
		return 1;
	}

	char szBuffer[64]={0};
	DWORD dwSize=64;
	int bQuery=HttpQueryInfo(urlHandle,HTTP_QUERY_CONTENT_LENGTH,szBuffer,&dwSize,0);

	if(bQuery) {
		fileSize=atoi(szBuffer);
	} else {
		fileSize=-1;
	}

	if(!isContinue) {
		curState=HTTP_STATE_CANCELED;
		return 1;
	}

	PrepareBuffer();

	FILE* hFile=fopen(dstFile.c_str(),"wb");

	if(!hFile) {
		curState=HTTP_STATE_ERROR;
		return 1;
	}

	DWORD dwRead=0;

	while(InternetReadFile(urlHandle,dlBuffer,HTTP_BUFFER_SIZE,&dwRead)) {
		if(dwRead) {
			fwrite(dlBuffer,1,dwRead,hFile);
		} else {
			fclose(hFile);
			curState=HTTP_STATE_COMPLETE;
			return 1;
		}

		if(!isContinue) {
			fclose(hFile);
			curState=HTTP_STATE_CANCELED;
			return 1;
		}

		Sleep(5);
	}

	fclose(hFile);

	curState=HTTP_STATE_ERROR;
	
	return 1;
}

void HTTPDownloader::PrepareBuffer()
{
	if(!dlBuffer) {
		dlBuffer=new unsigned char[HTTP_BUFFER_SIZE];
	}
}

void HTTPDownloader::Release()
{
	isContinue=0;

	if(urlHandle) {
		InternetCloseHandle(urlHandle);
	}

	if(inetHandle) {
		InternetSetStatusCallback(inetHandle,0);
		InternetCloseHandle(inetHandle);
	}

	if(dlBuffer) {
		delete[] dlBuffer;
		dlBuffer=0;
	}

	inetHandle=0;
	urlHandle=0;
	fileSize=0;
	urlPath.clear();
	dstFile.clear();

	WaitForSingleObject(dlThread,5); 

	CloseHandle(dlThread);
	dlThread=0;
	parentManager->RemoveDownload(this);

	delete this;
}

void HTTPDownloader::OnError()
{
}

void HTTPDownloader::OnComplete()
{
}

void HTTPDownloader::OnCancel()
{
}

