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
			typedef unsigned short HPVER;

#define CBVERSTR 64

			class VersionInfo  
			{
			private:
				char strOut[MAX_BUFFER_LENGTH];

				HPVER verMaintenance;
				HPVER verAux;
				HPVER verMinor;
				HPVER verMajor;

				std::string productName;
				std::string companyName;
				std::string legalCopyright;

			protected:
				bool GetVersionOfFile(const char* szFile);
				void Reset();

			public:
				VersionInfo(const VersionInfo& ver);
				VersionInfo(VS_FIXEDFILEINFO*);
				VersionInfo(const char* strVer);
				VersionInfo(HMODULE);     
				VersionInfo();
				virtual ~VersionInfo();

				void SetFromString(const char*);
				void SetFromFFI(VS_FIXEDFILEINFO*);
				void SetFromVerInfo(const VersionInfo&);
				void SetMajor(HPVER verMajor);
				void SetMinor(HPVER verMinor);
				void SetAux(HPVER verAux);
				void SetMaintenance(HPVER verMaintenance);
				void SetFromInstance(HMODULE);
				void SetFromFilename(const char*);

				void AsResource(char*) const;  
				const char* AsString(std::string& str) const;
				const char* AsStringShort(std::string& str) const;
				char* AsStringShort(char*) const;
				char* AsString(char*) const;
				const char* AsString();

				const char* GetExtProductName(std::string*) const;
				const char* GetProductName() const;
				const char* GetCompanyName() const;
				const char* GetLegalInfo() const;

				unsigned int Compare(const VersionInfo& ver) const;

				VersionInfo& operator=(const VersionInfo& ver);
				bool operator==(const VersionInfo& ver) const;
				bool operator<(const VersionInfo& ver) const;
				bool operator>(const VersionInfo& ver) const;

				static char* GetVersionInfo(const char* szFile);

				unsigned short GetVerAux();
			};
		}
	}
}

