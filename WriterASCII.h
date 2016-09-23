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
			class WriterASCII
			{
			protected:
				enum EOutputType {
					OUTPUT_NONE=0,
					OUTPUT_STDOUT,
					OUTPUT_FILE,
					OUTPUT_WINDOW
				};

			private:
				EOutputType				eOutputType;
				FILE*					fileOut;
				HWND					hWndOut;
				int						nIndentation;

				bool WriteIndentation	() const;
				bool Flush				() const;
				bool WriteChar			(char chCar)	const;

			public:
				WriterASCII				() : eOutputType(OUTPUT_NONE),fileOut(NULL),hWndOut(NULL),nIndentation(0) {};
				WriterASCII				(EOutputType eType,const std::string& strName);
				~WriterASCII			() { Close(); };

				bool InitStdout			();
				bool Close				();
				bool InitFile			(const std::string& strFile);
				bool InitWindow			(const std::string& strTitle);
				bool Write				(const char* szMessage,...) const;
				bool Write				(const char* file,const char* func,const int ln,const char* szMessage,...) const;
				bool Write				(const std::string& szMessage) const;
				bool Write				(const char* file,const char* func,const int ln,const std::string& szMessage) const;
				bool Ready				()					const	{ return eOutputType!=OUTPUT_NONE; };
				int  Indentation		()					const	{ return nIndentation; };
				void SetIndentation		(int nIndentChars)			{ nIndentation=nIndentChars; };
				void IncIndentation		(int nInc)					{ nIndentation+=nInc; };
				void DecIndentation		(int nDec)					{ nIndentation-=nDec; if(nIndentation<0){nIndentation=0;} };
			};
		}
	}
}



