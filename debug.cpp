#include "PCH.h"
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include "fslib.h"
#include "kln90b.h"

// ----------------------------------------------------------------------------------
// Name:			PRINTLOG
// Description:		Sends formatted log info into file based on debug level defined in the 
//					config file. Each call starts a new line in the log file.
//					Truncates the log if it is too long.
// Parameters:
//		Severity	0 = Fatal, 1 = Critical, ... greater numbers less important
//		Format		Standard sprintf() format string
//		...			Variable list used by the format string
// Return:		-
// Parameters definned in the [DEBUG] section of the config file:
//		[LOGFNAME]	Name of log file. No input if this parameter is not defined.
//		[MAXSIZE]	Maximum log file size. If reached, the log is truncated.
//		[VERBOSITY]	Logging level between 0 and 10. 0 means no logging, 10 means
//					most thorough logging.
// ----------------------------------------------------------------------------------

#define LOGDEFSIZE	(10 * 1024 * 1024)
#define LOGMAXSIZE	(100 * 1024 * 1024)

void PRINTLOG(DWORD Severity, char *Format, ...)
{
	static char LogFileName[MAX_PATH] = "";
	static DWORD  LogMaxSize = 0;
	static DWORD Verbosity = -1;

	// checks if log level allows printing the message 
	if ( Verbosity < Severity )
		return;
	// checks if parameters are initialized
	if (Verbosity == -1)
	{
		struct stat st;

		// reads log parameters from config
		K_load_string_param("DEBUG", "LOGFNAME", LogFileName);		
		K_load_dword_param("DEBUG", "MAXSIZE", &LogMaxSize);
		K_load_dword_param("DEBUG", "VERBOSITY", &Verbosity);
		// check parameter limits
		LogMaxSize *= (1024 * 1024);
		if (LogMaxSize > LOGMAXSIZE)								
			LogMaxSize = LOGDEFSIZE;
		if (Verbosity > 10)
			Verbosity = 10;
		// truncates log file if too large
		if (stat(LogFileName, &st) != 0)							
			if (st.st_size > LogMaxSize)
				remove(LogFileName);
	}

	FILE *f = fopen(LogFileName, "a+");
	if (f)
	{
		va_list arg_list;
		va_start(arg_list, Format);
		time_t t;

		fprintf(f, "\n%s: ", asctime(localtime(&t)));
		vfprintf(f, Format, arg_list);
		fclose(f);

		va_end(arg_list);
	}
}

// ----------------------------------------------------------------------------------
// Name:		K_DEBUG
// Description:	Sends formatted debug info into file. The file name is defined in the
//				[KLN90B] section DEBUGFILE variable in the config file. If the parameter
//				does not exist, no debuginfo is written into file.
// ----------------------------------------------------------------------------------

void K_DEBUG(char *format,...)
{
	#ifdef KLN_DEBUG
		va_list arg_list;
		va_start(arg_list, format);
		FILE *f;
		static char fname[MAX_PATH] = "";

		if ( !fname[0] )
			if (!K_load_string_param("KLN90B", "DEBUGFILE", fname))
				return;



		f = fopen(fname,"a+");
		if (f)
		{
			vfprintf( f, format, arg_list );
			fclose(f);
		}

		va_end(arg_list);
	#endif
}

// ----------------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------------

static char debug_buffer[MAX_PATH];
static CRITICAL_SECTION debug_cs;
static DWORD buffer_refresh;

void K_DEBUG2(char *format,...)
{
#ifdef KLN_DEBUG
	va_list arg_list;
	va_start(arg_list,format);
	EnterCriticalSection(&debug_cs);
	   vsprintf(debug_buffer,format,arg_list);
       buffer_refresh=GetTickCount();
	LeaveCriticalSection(&debug_cs);
	va_end(arg_list);
#endif
}
void K_print_debug_buffer(void)
{
	if(GetTickCount()-buffer_refresh > 10000 )
		return;
	EnterCriticalSection(&debug_cs);
       //FSClearMessages(4);
       //FSPrintString(debug_buffer,4,100,1);
    LeaveCriticalSection(&debug_cs);
}
void init_debug(void)
{
   InitializeCriticalSection(&debug_cs);
   sprintf(debug_buffer,"%s","");
   buffer_refresh=0;
}
void deinit_debug(void)
{
   DeleteCriticalSection(&debug_cs);
}