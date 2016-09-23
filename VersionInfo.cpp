/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;

VersionInfo::VersionInfo(VS_FIXEDFILEINFO* pver)
{
	SetFromFFI(pver);
}

VersionInfo::VersionInfo(const VersionInfo& ver)
{
	SetFromVerInfo(ver);
}

VersionInfo::VersionInfo(const char* strVer)
{
	Reset();
	SetFromString(strVer);
}

VersionInfo::VersionInfo(HMODULE hmod)
{
	SetFromInstance(hmod);
}

VersionInfo::VersionInfo()
{
	Reset();
}

VersionInfo::~VersionInfo()
{

}

void VersionInfo::Reset()
{
	verMajor=0;
	verMinor=0;
	verAux=0;
	verMaintenance=0;
	productName.assign("");
	companyName.assign("");
	legalCopyright.assign("");
}

void VersionInfo::SetFromVerInfo(const VersionInfo& ver)
{
	verMajor=ver.verMajor;
	verMinor=ver.verMinor;
	verAux=ver.verAux;
	verMaintenance=ver.verMaintenance;
	productName=ver.productName;
	companyName=ver.companyName;
	legalCopyright=ver.legalCopyright;
}

void VersionInfo::SetFromFFI(VS_FIXEDFILEINFO* pver)
{
	verMajor=HIWORD(pver->dwFileVersionMS);
	verMinor=LOWORD(pver->dwFileVersionMS);
	verAux=HIWORD(pver->dwFileVersionLS);
	verMaintenance=LOWORD(pver->dwFileVersionLS);
}

void VersionInfo::SetFromString(const char* strVer)
{
	const char chDelim='.';
	const char* pch=strVer;
	
	HPVER *ppVer[]={&verMajor, 
					&verMinor, 
					&verAux, 
					&verMaintenance, 
					NULL};

	HPVER** ppVerNext=ppVer;

	while(pch&&*pch) {
#ifdef _UNICODE
		**ppVerNext=(HPVER)_wtoi(pch);
		pch=wcschr(pch,chDelim);
#else
		**ppVerNext=atoi(pch);
		pch=strchr(pch,chDelim);
#endif
		if(pch) 
			pch++;
		ppVerNext++;
		if(!*ppVerNext)
			break;
	}
}

void VersionInfo::SetFromInstance(HMODULE hmod)
{
	char szFile[MAX_BUFFER_LENGTH];

	if(hmod==NULL) {
		Reset();
		//return;
	}
	GetModuleFileName(hmod,szFile,sizeof(szFile));
	GetVersionOfFile(szFile);
}

void VersionInfo::SetFromFilename(const char* tszFile)
{
	GetVersionOfFile(tszFile);
}

void VersionInfo::SetMajor(HPVER verMajor)
{
	verMajor=verMajor;
}

void VersionInfo::SetMinor(HPVER verMinor)
{
	verMinor=verMinor;
}

void VersionInfo::SetAux(HPVER verAux)
{
	verAux=verAux;
}

void VersionInfo::SetMaintenance(HPVER verMaintenance)
{
	verMaintenance=verMaintenance;
}


void VersionInfo::AsResource(char* str) const
{
	sprintf_s(str,MAX_BUFFER_LENGTH,"%d,%d,%d,%d",verMajor,verMinor,verAux,verMaintenance);
}

char* VersionInfo::AsString(char* tszStrOut) const
{
	sprintf_s(tszStrOut,MAX_BUFFER_LENGTH,"%d.%d.%d.%d",verMajor,verMinor,verAux,verMaintenance);
	return tszStrOut;
}

char* VersionInfo::AsStringShort(char* tszStrOut) const
{
	sprintf_s(tszStrOut,MAX_BUFFER_LENGTH,"%d.%d",verMajor,verMinor);
	return tszStrOut;
}

const char* VersionInfo::AsString()
{
	return AsString(strOut);
}

const char* VersionInfo::AsString(std::string& str) const
{
	char tszVer[CBVERSTR];
	str=AsString(tszVer);
	return str.data();
}

const char* VersionInfo::AsStringShort(std::string& str) const
{
	char tszVer[CBVERSTR];
	str=AsStringShort(tszVer);
	return str.data();
}

const char* VersionInfo::GetCompanyName() const
{
	return companyName.data();
}

const char* VersionInfo::GetExtProductName(std::string* pstr) const
{
	std::string str;
	pstr->assign(productName);
	pstr->append(" ");
	pstr->append(AsStringShort(str));
	return pstr->data();
}

const char* VersionInfo::GetProductName() const
{
	return productName.data();
}

const char* VersionInfo::GetLegalInfo() const
{
	return legalCopyright.data();
}

unsigned int VersionInfo::Compare(const VersionInfo& ver) const
{
	HPVER rgVerThis[]={verMajor,verMinor,verAux,verMaintenance};
	HPVER rgVer[]={ver.verMajor,ver.verMinor,ver.verAux,ver.verMaintenance};
	int ixRg=0;

	assert(sizeof(rgVerThis)==sizeof(rgVer));

	while(ixRg<(sizeof(rgVerThis)/sizeof(HPVER))) {
		if(rgVerThis[ixRg]>rgVer[ixRg])
			return 1;
		if(rgVerThis[ixRg]<rgVer[ixRg])
			return -1;
		ixRg++;
	}
	return 0;
}

VersionInfo& VersionInfo::operator=(const VersionInfo& ver)
{
	SetFromVerInfo(ver);
	return *this;
}

bool VersionInfo::operator==(const VersionInfo& ver) const
{
	return (Compare(ver)==0);
}

bool VersionInfo::operator<(const VersionInfo& ver) const
{
	return (Compare(ver)==-1);
}

bool VersionInfo::operator>(const VersionInfo& ver) const
{
	return (Compare(ver)==1);
}

void GetVersionString(char* lpVI,char* tszStr,unsigned short* lng,std::string* pstr)
{
	DWORD dwBufSize; 
	TCHAR tszVerStrName[CBVERSTR];
	LPVOID lpt;

	sprintf_s(tszVerStrName,"\\StringFileInfo\\%04x%04x\\%s",lng[0],lng[1],tszStr);
	if(VerQueryValue(lpVI,tszVerStrName,&lpt,(unsigned int*)&dwBufSize))
		pstr->assign((char*)lpt);
}

bool VersionInfo::GetVersionOfFile(const char* szFile) 
{
	unsigned long dwBufSize; 
	VS_FIXEDFILEINFO* lpFFI; 
	char* lpVI=GetVersionInfo(szFile); 
	bool bRet=false;
	unsigned short* langInfo;
	unsigned int cbLang;

	if(!lpVI)
		return false;
	
	if(bRet=VerQueryValue(lpVI,"\\",(void**)&lpFFI,(unsigned int*)&dwBufSize)) {
		SetFromFFI(lpFFI);
	}
	
	VerQueryValue(lpVI,"\\VarFileInfo\\Translation",(void**)&langInfo,&cbLang);
	GetVersionString(lpVI,"ProductName",langInfo,&productName);
	GetVersionString(lpVI,"CompanyName",langInfo,&companyName);
	GetVersionString(lpVI,"LegalCopyright",langInfo,&legalCopyright);
	GlobalFree((HGLOBAL)lpVI); 
	return bRet; 
} 

char* VersionInfo::GetVersionInfo(const char* szFile) 
{
	unsigned long dwLen,dwUseless; 
	char* lpVI; 

	dwLen=GetFileVersionInfoSize((char*)szFile,&dwUseless); 
	if(dwLen==0) 
		return 0; 

	lpVI=(char*)GlobalAlloc(GPTR,dwLen); 
	if(lpVI) {
		GetFileVersionInfo((char*)szFile,0,dwLen,lpVI); 
		return lpVI;
	}   
	return NULL; 
}

unsigned short VersionInfo::GetVerAux()
{
	return verAux;
}

