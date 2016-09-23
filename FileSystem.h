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
			namespace fs=std::tr2::sys;

			inline bool IsFileExists(const std::string& filename)
			{
				return fs::exists(fs::path(filename));
			}

			inline std::string GetFileExtension(const std::string& filename)
			{
				return fs::extension(fs::path(filename));
			}

			inline long long GetFileSize(const std::string& filename)
			{
				return fs::file_size(fs::path(filename));
			}

			inline bool IsPathDirectory(const std::string& filename)
			{
				return fs::is_directory(fs::path(filename));
			}

			inline bool IsFileOrDirectoryEmpty(const std::string& filename)
			{
				return fs::is_empty(fs::path(filename));
			}

			inline static bool RenameFile(const std::string& from,const std::string& to)
			{
				return fs::rename(fs::path(from),fs::path(to));
			}

			inline static void CopyFile(const std::string& from,const std::string& to,bool overwrite)
			{
				fs::copy_file(fs::path(from),fs::path(to),overwrite?fs::copy_option::overwrite_if_exists:fs::copy_option::fail_if_exists);
			}

			inline static bool DeleteFile(const std::string& filename)
			{
				return fs::remove_filename(fs::path(filename));
			}

			inline static LONGLONG FileTimeToPOSIX(FILETIME ft)
			{
				// takes the last modified date
				LARGE_INTEGER date,adjust;
				date.HighPart=ft.dwHighDateTime;
				date.LowPart=ft.dwLowDateTime;

				// 100-nanoseconds = milliseconds * 10000
				adjust.QuadPart=11644473600000*10000;

				// removes the diff between 1970 and 1601
				date.QuadPart-=adjust.QuadPart;

				// converts back from 100-nanoseconds to seconds
				return date.QuadPart/10000000;
			}
		}
	}
}


