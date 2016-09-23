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
			static const char* const	PATH_SEPARATORS		= "/\\";
			static unsigned int			PATH_SEPARATORS_LEN	= 2;

			static const char			UNIX_PATH_SEPARATOR = '/';
			static const char			WINDOWS_PATH_SEPARATOR = '\\';

			inline void ToLowerString(std::string& instr)
			{
				std::transform(instr.begin(),instr.end(),instr.begin(),tolower);
			}

			inline std::string ToLowerString(const std::string& instr)
			{
				std::string ret;
				ToLowerString(ret);
				return ret;
			}

			inline void ToUpperString(std::string& instr)
			{
				std::transform(instr.begin(),instr.end(),instr.begin(),toupper);
			}

			inline std::string ToUpperString(const std::string& instr)
			{
				std::string ret;
				ToUpperString(ret);
				return ret;
			}

			inline void TrimString(std::string& str,bool left=true,bool right=true)
			{
				static const std::string delims=" \t\r";
				if(right)
					str.erase(str.find_last_not_of(delims)+1);
				if(left)
					str.erase(0,str.find_first_not_of(delims));
			}

			inline std::string TrimString(const std::string& str,bool left=true,bool right=true)
			{
				std::string ret=str;
				TrimString(ret,left,right);
				return ret;
			}

			template<typename T>
			inline static T FromString(const std::string& str) 
			{
				std::istringstream is(str);
				T t;
				is >> t;
				return t;
			}

			inline static std::string ToString(bool val)
			{
				return val?"True":"False";
			}

			template<typename T>
			inline static std::string ToString(const T& t) 
			{
				std::ostringstream os;
				os << t;
				return os.str();
			}

			inline static bool MultibyteToUnicode(const char* strSource,wchar_t* wstrDestination)
			{
				size_t cchDestChar=strlen(strSource)+1;

				if(wstrDestination==NULL||strSource==NULL||cchDestChar<1)
					return false;

				int nResult=MultiByteToWideChar(CP_ACP,0,strSource,-1,wstrDestination,(int)cchDestChar);
				wstrDestination[cchDestChar-1]=0;

				if(nResult==0)
					return false;

				return true;
			}

			inline static bool UnicodeToMultibyte(const wchar_t* wstrSource,char* strDestination)
			{
				size_t cchDestChar=wcslen(wstrSource)+1;

				if(strDestination==NULL||wstrSource==NULL||cchDestChar<1)
					return false;

				int nResult=WideCharToMultiByte(CP_ACP,0,wstrSource,-1,strDestination,int(cchDestChar*sizeof(char)),NULL,NULL);
				strDestination[cchDestChar-1]=0;

				if(nResult==0)
					return false;

				return true;
			}

			inline static bool MultibyteToUnicode(const std::string& strSource,std::wstring& wstrDestination)
			{
				wchar_t* retbuf=new wchar_t[strSource.size()+1];
				bool ret=MultibyteToUnicode(strSource.c_str(),retbuf);
				if(!ret) {
					SafeDeleteArray(retbuf);
					return false;
				}
				wstrDestination.assign(retbuf);
				SafeDeleteArray(retbuf);
				return true;
			}

			inline static bool UnicodeToMultibyte(const std::wstring& strSource,std::string& wstrDestination)
			{
				char* retbuf=new char[strSource.size()+1];
				bool ret=UnicodeToMultibyte(strSource.c_str(),retbuf);
				if(!ret) {
					SafeDeleteArray(retbuf);
					return false;
				}
				wstrDestination.assign(retbuf);
				SafeDeleteArray(retbuf);
				return true;
			}

			inline static std::wstring MultibyteToUnicode(const std::string& strSource)
			{
				std::wstring ret;
				MultibyteToUnicode(strSource,ret);
				return ret;
			}

			inline static std::string UnicodeToMultibyte(const std::wstring& strSource)
			{
				std::string ret;
				UnicodeToMultibyte(strSource,ret);
				return ret;
			}

			inline static std::string GetFilePath(const std::string& filename)
			{
				std::string::size_type slash=filename.find_last_of(PATH_SEPARATORS);
				if(slash==std::string::npos) 
					return std::string();
				else 
					return std::string(filename,0,slash);
			}

			inline static bool BeginsWith(const std::string& test,const std::string& pattern) 
			{
				if(test.size()>=pattern.size()) {
					for(int i=0;i<(int)pattern.size();++i) {
						if(pattern[i]!=test[i]) {
							return false;
						}
					}
					return true;
				} else {
					return false;
				}
			}

			inline static bool EndsWith(const std::string& test,const std::string& pattern) 
			{
				if(test.size()>=pattern.size()) {
					size_t te=test.size()-1;
					size_t pe=pattern.size()-1;
					for(int i=(int)pattern.size()-1;i>=0;--i) {
						if(pattern[pe-i]!=test[te-i]) {
							return false;
						}
					}
					return true;
				} else {
					return false;
				}
			}

			inline static std::string RemoveTrailingSlash(const std::string& path) 
			{
				bool removeSlash=((EndsWith(path,"/")||EndsWith(path,"\\")))/*&&!isRoot(f)*/;
				return removeSlash?path.substr(0,path.length()-1):path;
			}

			inline static std::string AddTrailingSlash(const std::string& path) 
			{
				bool slash=((EndsWith(path,"/")||EndsWith(path,"\\")));
				return slash?path:path+WINDOWS_PATH_SEPARATOR;
			}

			inline bool CompareStringsNoCase(const std::string& rStr1,const std::string& rStr2)
			{
				return 0==_strcmpi(rStr1.c_str(),rStr2.c_str());
			}

			inline bool ParseStringForBool(const std::string& str)
			{
				if(CompareStringsNoCase(str,"true")||CompareStringsNoCase(str,"1")||CompareStringsNoCase(str,"yes")||CompareStringsNoCase(str,"on"))
					return true;
				return false;
			}

			inline std::string GetEnvironmentVar(const std::wstring& var)
			{
				static char buf[MAX_BUFFER_LENGTH]={""};
				wchar_t* libvar;
				size_t requiredSize;

				_wgetenv_s(&requiredSize,NULL,0,var.c_str());
				if(requiredSize!=0) {
					libvar=(wchar_t*)malloc(requiredSize*sizeof(wchar_t));
					if(!libvar) {
						EXCEPT("Failed to allocate memory!");
					}

					_wgetenv_s(&requiredSize,libvar,requiredSize,var.c_str());
					UnicodeToMultibyte(libvar,buf);
					free(libvar);
				}
				return std::string(buf);
			}

			inline std::string GetEnvironmentVar(const std::string& var)
			{
				return GetEnvironmentVar(MultibyteToUnicode(var));
			}

		}
	}
}

