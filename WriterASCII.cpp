/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;

WriterASCII::WriterASCII(EOutputType eType,const std::string& strName) : fileOut(NULL),hWndOut(NULL),nIndentation(0)
{
#ifdef LOGGING
	switch(eType) {
		case OUTPUT_STDOUT:
			InitStdout();
		break;
		case OUTPUT_FILE:
			InitFile(strName);
		break;
		case OUTPUT_WINDOW:
			InitWindow(strName);
		break;
		case OUTPUT_NONE:
		default:
		break;
	}
#endif
}

bool WriterASCII::InitStdout()
{
#ifdef LOGGING
	if(Ready()) {
		Close();
	}
	fileOut=stdout;
	if(fileOut==NULL) {
		return false;
	}
	eOutputType=OUTPUT_STDOUT;
#endif
	return true;
}

bool WriterASCII::InitFile(const std::string& strFile)
{
#ifdef LOGGING
	if(strFile=="") {
		eOutputType=OUTPUT_NONE;
		return false;
	}

	if(Ready()) {
		Close();
	}

	SafeFopen(fileOut,strFile,"wt");

	if(!fileOut) {
		EXCEPT("Couldn't open "+strFile+" for output logging.");
		return false;
	}
	eOutputType=OUTPUT_FILE;
#endif
	return true;
}

bool WriterASCII::InitWindow(const std::string& strTitle)
{
#ifdef LOGGING
	if(Ready()) {
		Close();
	}

	hWndOut=CreateWindow("EDIT",strTitle.data(),WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE|ES_NOHIDESEL|ES_WANTRETURN|ES_READONLY,20,20,640,480,NULL,NULL,NULL,NULL);

	if(hWndOut==NULL) {
		EXCEPT("Error creating a log window.");
		return false;
	}

	UpdateWindow(hWndOut);
	eOutputType=OUTPUT_WINDOW;
#endif
	return true;
}

bool WriterASCII::Close()
{
#ifdef LOGGING
	switch(eOutputType) {
		case OUTPUT_NONE:
		break;
		case OUTPUT_STDOUT:
		break;
		case OUTPUT_FILE:
			SafeFclose(fileOut);
		break;
		case OUTPUT_WINDOW:
			if(hWndOut) {
				DestroyWindow(hWndOut);
				hWndOut=NULL;
			}
		break;
		default:
		break;
	}

	eOutputType=OUTPUT_NONE;
	nIndentation=0;
#endif
	return true;
}

bool WriterASCII::Write(const char* szMessage,...) const
{
#ifdef LOGGING
	if(eOutputType==OUTPUT_NONE) {
		return true;
	}

	if(Ready()==false) {
		return false;
	}

	static char szFormattedMessage[MAX_BUFFER_LENGTH];

	va_list arguments;
	va_start(arguments,szMessage);
	vsprintf_s(szFormattedMessage,szMessage,arguments);
	va_end(arguments);

#ifdef _DEBUG
	OutputDebugString(szFormattedMessage);
#endif

	size_t nCount,nLength=strlen(szFormattedMessage);

	for(nCount=0;nCount<nLength;nCount++) {
		if(szFormattedMessage[nCount]=='\n') {
			WriteChar('\n');
			if(eOutputType==OUTPUT_WINDOW) {
				WriteChar('\r');
			}
			WriteIndentation();
		} else {
			WriteChar(szFormattedMessage[nCount]);
		}
	}
	Flush();
#endif
	return true;
}

bool WriterASCII::Write(const char* file,const char* func,const int ln,const char* szMessage,...)	const
{
#ifdef LOGGING
	if(eOutputType==OUTPUT_NONE) {
		return true;
	}

	if(Ready()==false) {
		return false;
	}

	static char szFormattedMessage[MAX_BUFFER_LENGTH];

	va_list arguments;
	va_start(arguments,szMessage);
	vsprintf_s(szFormattedMessage,szMessage,arguments);
	va_end(arguments);
	return Write("%s, file: %s, function: %s, line: %d",szFormattedMessage,file,func,ln);
#else
	return true;
#endif
}

bool WriterASCII::WriteChar(char chCar) const
{
#ifdef LOGGING
	int nWindowTextLength;

	switch(eOutputType) {
		case OUTPUT_NONE:
			return true;
		case OUTPUT_STDOUT:
		case OUTPUT_FILE:
			fputwc(chCar,fileOut);
		break;
		case OUTPUT_WINDOW:
			nWindowTextLength=GetWindowTextLength(hWndOut)+1;
			std::string newText;
			newText.reserve(nWindowTextLength+1);
			GetWindowText(hWndOut,(char *)newText.data(),nWindowTextLength);
			newText[nWindowTextLength]=chCar;
			SetWindowText(hWndOut,newText.data());
		break;
	}
#endif
	return true;
}

bool WriterASCII::WriteIndentation() const
{
#ifdef LOGGING
	if(eOutputType==OUTPUT_NONE) {
		return true;
	}

	for(int nCount=0;nCount<nIndentation;nCount++) {
		WriteChar(' ');
	}
#endif
	return true;
}

bool WriterASCII::Flush() const
{
#ifdef LOGGING
	if(Ready()==false) {
		return false;
	}

	switch(eOutputType) {
		case OUTPUT_NONE:
			return true;
		case OUTPUT_STDOUT:
		case OUTPUT_FILE:
			if(fileOut) {
				fflush(fileOut);
			}
		break;
		case OUTPUT_WINDOW:
			if(hWndOut)
				UpdateWindow(hWndOut);
		break;
	}
#endif
	return true;
}

bool WriterASCII::Write(const std::string& szMessage) const
{
	return Write(szMessage.c_str());
}

bool WriterASCII::Write(const char* file,const char* func,const int ln,const std::string& szMessage) const
{
	return Write(file,func,ln,szMessage.c_str());
}

