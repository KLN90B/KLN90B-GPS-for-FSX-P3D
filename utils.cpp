#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include <stdio.h>
#include <math.h>

char *monthes[] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
extern symbol video_buffer[7][23];
symbol shadow_buffer[7][23];
extern CRITICAL_SECTION video_buffer_cs;

extern int SoundOk;												// SOUND
extern TGaugePlaySound GaugePlaySound;
extern TGaugeStopSound GaugeStopSound;
extern TTerminateSounds TerminateSounds;

static char KLN90_Main_Dir[MAX_PATH];
static char FS_Main_Dir[MAX_PATH];

void PlayASound(char *sound, char *stop, int type)				// SOUND
{
	char soundloc[256];
	double stoptype = 0;
	FILE *Testit;

	strcpy(soundloc, "KLN90B\\Sound\\");
	strcat(soundloc, sound);
	Testit = fopen(soundloc, "r");
	if (Testit == NULL) {			// If it doesn't exist
		return;
	}
	fclose(Testit);
	if (SoundOk) {
		(GaugePlaySound)(soundloc, stop, type);
	}
}

void set_kln90_dir(char *dir_name)
{
	dir_name ? _snprintf(KLN90_Main_Dir,sizeof(KLN90_Main_Dir)-1,"%s\\",dir_name) : sprintf(KLN90_Main_Dir,"");
}
int get_kln90_dir(char *buffer)
{
	if(!buffer)
		return(strlen(KLN90_Main_Dir));
	strncpy(buffer,KLN90_Main_Dir,strlen(KLN90_Main_Dir)+1);
	return (0);
}

void set_fs_dir(char *dir_name)
{
	dir_name ? _snprintf(FS_Main_Dir,sizeof(FS_Main_Dir)-1,"%s\\",dir_name) : sprintf(FS_Main_Dir,"");
}
int get_fs_dir(char *buffer)
{
	if(!buffer)
		return(strlen(FS_Main_Dir));
	strncpy(buffer,FS_Main_Dir,strlen(FS_Main_Dir)+1);
	return (0);
}

char *GetFsMainDir(void)
{	
	static char file_name[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL),file_name,sizeof(file_name)-1);
	int i=0;
	for(i = strlen(file_name)-1;file_name[i]!='\\';i--);
	file_name[i+1]='\0';
	return(file_name);
}

void K_GetDegMin(FLOAT64 whole_degree,K_deg *deg_ptr)
{
	deg_ptr->degrees = (DWORD)whole_degree;
    DWORD min_decmin = (DWORD)(float)((float)(fabs(whole_degree) - (DWORD)fabs(whole_degree))*60.0f*100.0f);
	deg_ptr->mins = min_decmin/100;
	deg_ptr->dec_mins = min_decmin % 100;
}

double DM2DEG(double deg,double min,double dec_min)
{
	double res = deg+(min+dec_min/100.0f) / 60.0f;
	long dec_mins = (long)((res - floor(res))*60.0f*100.0f)%100;
	if(dec_mins<dec_min) res+=1.0f/(60.0f*100.0f);
	return(res);
}


// -------------------------------------------------------------------------------------------------
// Name				get_date()
// Description		Get current date as a formatted string to display
// Parameters	
//		buff		Caller provided buffer for the formatted string or NULL
//		date_type	PC system, PC local or Flight Simulator date
// Returns			Pointer to string buffer. Either the user provided, of a static one
// Depends on		-
// Comment
//		SIM_TIME type is not yet supported
// -------------------------------------------------------------------------------------------------

char *get_date(char *buff, datetime_t time_type)
{
	static char local_buff[24] = {0};
	char *pb;
	SYSTEMTIME datetime;

	pb = (buff == NULL) ? local_buff : buff;
	switch(time_type)
	{
		case UTC_TIME:
	    	GetSystemTime( &datetime );
			sprintf(pb, "%.2d %s %.2d", datetime.wDay, monthes[datetime.wMonth - 1], datetime.wYear % 100);
			break;
		case LOCAL_TIME:
			GetLocalTime( &datetime );
			sprintf(pb, "%.2d %s %.2d", datetime.wDay, monthes[datetime.wMonth - 1], datetime.wYear % 100);
			break;
		case SIM_TIME:		// not yet implemented
		default:
			sprintf(pb,"31 JUL 94");
			break;
	}
	return(pb);
}

// -------------------------------------------------------------------------------------------------
// Name				get_time()
// Description		Get current time as a formatted string to display
// Parameters	
//		buff		Caller provided buffer for the formatted string or NULL
//		date_type	PC system, PC local or Flight Simulator time
// Returns			Pointer to string buffer. Either the user provided, of a static one
// Depends on		-
// Comment
//		SIM_TIME type is not yet supported
// -------------------------------------------------------------------------------------------------

char *get_time(char *buff, datetime_t time_type)
{
	static char local_buff[24] = { 0 };
	char *pb;
	SYSTEMTIME datetime;

	pb = (buff == NULL) ? local_buff : buff;
	switch (time_type)
	{
	case UTC_TIME:
		GetSystemTime( &datetime );
		sprintf(pb, "%.2d:%.2d:%.2d", datetime.wHour, datetime.wMinute, datetime.wSecond);
		break;
	case LOCAL_TIME:
		GetLocalTime( &datetime );
		sprintf(pb, "%.2d:%.2d:%.2d", datetime.wHour, datetime.wMinute, datetime.wSecond);
		break;
	case SIM_TIME:		// not yet implemented
	default:
		sprintf(pb, "08:10:00");
		break;
	}
	return(pb);
}

// -----------------------------------------------------------------------------------------------

void print_str(int row, int col, ULONG attribute, char *text, ...)
{  
	char _text[24];
	char *__text=_text;

	va_list arg_list;
	va_start(arg_list,text);
	_vsnprintf(_text,23,text,arg_list);
	va_end(arg_list);

	_text[23]='\0';
    if(col < 0 )
	{
	   //K_DEBUG("print_str: col < 0 COL=[%d]\n",col);
	   col=0;
	}
    if(row < 0 )
	{
	   //K_DEBUG("print_str: row < 0 ROW=[%d]\n",row);
	   row=0;
	}
    if(row > 6 )
	{
	   //K_DEBUG("print_str: row > 6 ROW=[%d]\n",row);
	   row=6;
	}
	for(int current_col=col;(*__text) && (current_col < 24);current_col++)
	{
	      shadow_buffer[row][current_col].ch = *__text++;
		  shadow_buffer[row][current_col].attribute = attribute;
	}
}
void K_change_attribute(int row,int col,int len,ULONG attribute)
{
	int i;
	for( i=0; i<len; i++ )
	{
		shadow_buffer[row][col+i].attribute = attribute;
	}
}

ULONG K_get_attribute(int row,int col)
{
		return(shadow_buffer[row][col].attribute);
}

char K_get_char(int row,int col)
{
   return(shadow_buffer[row][col].ch);
}
void K_set_char(int row,int col,char ch)
{
   shadow_buffer[row][col].ch=ch;
}

void update_screen(void)
{
    EnterCriticalSection(&video_buffer_cs);
	   memcpy(video_buffer,shadow_buffer,sizeof(video_buffer));
	LeaveCriticalSection(&video_buffer_cs);
}

void init_console(void)
{
   memcpy(shadow_buffer,video_buffer,sizeof(shadow_buffer));
}
//=============================================================================
void __strrev(char *buffer)
{
    char *tmp = strrev(buffer);
	strcpy(buffer,tmp);
}
//==============================================================================
void convert_id(unsigned long id_code,char *buffer)
{
    unsigned long id = (id_code & 0xFFFFFFF0) >> 5;
	char *temp_buff = buffer;
	int symbols=0;
	while(id)
	{
	   *temp_buff++ = (char)(id%0x26>0xB) ? (id%0x26)+0x35 : (id%0x26)+0x2E ;
	   symbols++;
       id/=0x26;
	}
	if(*buffer && symbols)
	{
	   *temp_buff=0;
	   __strrev(buffer);
	}
	else
	{
	  *buffer++=0x30;
	  *buffer = '\0';
	}
}

// -------------------------------------------------------------------------------
// Name:		iceo2id
// Description:	Converts an airport ICAO ID to a numerical ID
// Parameters:
//		buffer	ICAO ID
// Returns:		Numerical ID
// Comment:		Used for building up fast access lists. It is possibly
//				a similar (tough reverse) conversion than the one described here:
//				http://www.kloth.net/radio/icao-id.php
// -------------------------------------------------------------------------------

unsigned long icao2id(char *buffer)
{
	unsigned long id = 0;
	while(*buffer)
	{
	   id *= 0x26;	
	   if( *buffer >= 'A') 
		   id += ( *buffer - 0x35 ); // ASCII code - 53
	   else 
		   id += ( *buffer - 0x2E);		// ASCII code - 46
       buffer++;
	}
	return(id<<5);
}

//================================================================================
char *K_FormName(char *src,char *dst,int count)
{
	static char int_dst[MAX_PATH]={0};
	char *return_buffer = dst == NULL ? int_dst : dst;
	int i=0;
	for(i=0;i<count;i++)
		return_buffer[i]=src[i];
	return_buffer[i]='\0';
	return(return_buffer);
}

void clear_screen(void)
{
   for(int __row=0;__row<7;__row++)
	   print_str(__row,0,ATTRIBUTE_NORMAL,"                       ");
}

void sr_screen(int ACTION)
{
	static symbol __temp_buffer[7][23];
	EnterCriticalSection(&video_buffer_cs);
	   if(ACTION == SCREEN_SAVE)
           memcpy(__temp_buffer,video_buffer,sizeof(video_buffer));
	   if(ACTION == SCREEN_RESTORE)
	   {
           memcpy(video_buffer,__temp_buffer,sizeof(video_buffer));
		   memcpy(shadow_buffer,__temp_buffer,sizeof(video_buffer));
	   }
	LeaveCriticalSection(&video_buffer_cs);
}
void sr_screen_to_buf(int ACTION,void *__temp_buffer)
{
	EnterCriticalSection(&video_buffer_cs);
	   if(ACTION == SCREEN_SAVE)
           memcpy(__temp_buffer,video_buffer,sizeof(video_buffer));
	   if(ACTION == SCREEN_RESTORE)
	   {
           memcpy(video_buffer,__temp_buffer,sizeof(video_buffer));
		   memcpy(shadow_buffer,__temp_buffer,sizeof(video_buffer));
	   }
	LeaveCriticalSection(&video_buffer_cs);
}
//===================================================================================
char K_next_char(char ch)
{
		if(ch == BLANK_CHAR || ch=='-')
			ch='A';
		else if(ch == '9')
            ch='A';
		else if(ch == 'Z')
            ch='0';
		else
			ch++;
		return(ch);
}
char K_prev_char(char ch)
{
		if(ch == BLANK_CHAR || ch=='-')
			ch='9';
		else if(ch == 'A')
            ch='9';
		else if(ch == '0')
            ch='Z';
		else
			ch--;
		return(ch);
}
char *K_get_string(int start_row,int start_col,int end_col,char *_string)
{
    int i;
	for(i=0;i<=end_col-start_col;i++)
	{
	    _string[i] = K_get_char(start_row,start_col+i);
	}
    _string[i]='\0';
	return(_string);
}
char *K_trim_string(char *_string)
{
	int i;
	for(i=0;i<strlen(_string);i++)
	{
		if(_string[i]==' ' || _string[i]=='-')
			_string[i]='\0';
	}
	return(_string);
}

// ----------------------------------------------------------------------------------------------
// Name:		K_GetParam
// Description:	Returns a parameter value from a string containing "ParamName=[ParamValue]"
// Parameters:
//		main_str		Source string
//		sub_str			Parameter name 
//		TYPE			Variable type: STRING_T, INT_T, DOUBLE_T
//		target_buffer	Storage for parameter value
//		beg				Value left delimiter. Default is '['
//		end				Value right delimiter. Default is ']'
//	Returns:	-
// ----------------------------------------------------------------------------------------------

void K_GetParam(char *main_str,char *sub_str,int TYPE,void *target_buffer,char beg,char end)
{
    char temp_str[4*MAX_PATH];
	char *temp_ptr = temp_str;
	char *param = strstr(main_str,sub_str);
	if(!param) return;
	param+=strlen(sub_str);
	if(*param++ != beg) return;
	while(*param!=end && *param )
		*temp_ptr++=*param++;
    *temp_ptr='\0';
	switch(TYPE)
	{
	case STRING_T:
		strcpy((char *)target_buffer,temp_str);
		break;
	case INT_T:
		*(long unsigned *)target_buffer = (DWORD)atof(temp_str);
		break;
	case DOUBLE_T:
		*(double *)       target_buffer = atof(temp_str);
		break;
	}
}
//==============================================================
HANDLE k_heap;


void mem_init(void)
{
    k_heap = HeapCreate(0,4096,0);
}
void mem_deinit(void)
{
    HeapDestroy(k_heap);
}
void *K_malloc(DWORD bytes)
{
    return(HeapAlloc(k_heap,HEAP_ZERO_MEMORY,bytes));
}
void K_free(void *ptr)
{
   if(ptr)
      HeapFree(k_heap,0,ptr);
}
//===============================================================
LONG round_deg(double deg)
{
    if((DWORD)(deg*10.0f) % 10 > 5)
		deg++;
	deg = (LONG)deg;
	if(deg < 0 ) return(360+deg);
	if(deg > 360) return(deg-360);
	return(deg);
}
//===============================================================
int char_type(char ch)
{
  if(
     (ch>='0' && ch<='9')
     ||
     (ch>='A' && ch<='Z')
    )
	return(CHAR_ALPHA_NUM);
  if(ch == BLANK_CHAR)
	  return(BLANK_CHAR);
  return(CHAR_UNKNOWN);
}
//================================================================
LONG K_ftol(double arg)
{
   LONG ARG = (LONG)(arg*100.0f);
   ARG = ((abs(ARG) % 100) >= 50 ? (ARG/100)+1 : ARG/100);
   if(ARG < 0 ) return(360+ARG);
   if(ARG > 360 ) ARG = ARG - 360;
   return(ARG);
}

void get_LL(FLOAT64 Lat_src,FLOAT64 Lon_src,FLOAT64 S,FLOAT64 OD,FLOAT64 *dLat,FLOAT64 *dLon)
{
	double s_from_beg = S;

	if(s_from_beg<0) s_from_beg=0;

	double _cos_S = cos(s_from_beg/(57.29578 * 60 * 1.853)); 
	double _sin_S = sin(s_from_beg/(57.29578 * 60 * 1.853)); 
	
	double Lat = 57.29578 * asin( sin(Lat_src/57.29578) * 
		                          _cos_S + cos(Lat_src/57.29578) * _sin_S * 
								  cos(OD/57.29578)
								 );

	double cos_Lon_S_Lon = (_cos_S - sin(Lat_src/57.29578)*sin(Lat/57.29578))/
		                   (cos(Lat_src/57.29578)*cos(Lat/57.29578));
    if(fabs(cos_Lon_S_Lon)>1.0f)
	{
	   cos_Lon_S_Lon=1.0f;
	   if(cos_Lon_S_Lon<0) cos_Lon_S_Lon=-cos_Lon_S_Lon;
	}
	double Lon;

	double s_Lon = Lon_src<0?Lon_src+360.0f:Lon_src;

	if(OD<=180) Lon = (acos(cos_Lon_S_Lon) + s_Lon/57.29578)*57.29578;
	else Lon = (-acos(fabs(cos_Lon_S_Lon)>1.0f?cos_Lon_S_Lon>0?1.0f:-1.0f:cos_Lon_S_Lon) + s_Lon/57.29578)*57.29578;
    if(Lon>180) Lon = Lon-360.0;
	*dLat=Lat;
	*dLon=Lon;
}

BOOL K_get_rp_point(nv_point_t *pt)
{
	switch(get_rp_type())
	{
	case NDB_PAGE:
	   {
	      pt->type = NAVAID_NDB;
		  pt->buffer_spec = get_current_ndb();
	   }
	   break;
	case VOR_PAGE:
	   {
	      pt->type = NAVAID_VOR;
		  pt->buffer_spec = get_current_vor();
	   }
	   break;
	case APT_PAGE:
	   {
	      pt->type = NAVAID_APT;
		  pt->buffer_spec = get_current_apt();
	   }
	   break;
	case WPT_PAGE:
	   {
	      pt->type = NAVAID_WPT;
		  pt->buffer_spec = get_current_wpt();
	   }
	   break;
	case SUP_PAGE:
		{
			pt->type = NAVAID_SUP;
			pt->buffer_spec = get_current_sup();
		}
		break;
	case ACT_PAGE:
		{
			act_get_act_point(pt);
		}
		break;
	default:
		pt->buffer_spec = NULL;
		break;
	}
	return(pt->buffer_spec ? TRUE : FALSE);
}

extern MODULE_VAR   zulu_hour_sim;
extern MODULE_VAR   zulu_minute_sim;
extern MODULE_VAR   zulu_day_sim;
extern MODULE_VAR   zulu_year_sim;
extern MODULE_VAR   clock_second_sim;

static int days_in_month[12]={31,28,31,30,31,30,31,31,30,31,30,31};

void get_DT(K_dite_time_t *__dt)
{
   int __day,__year,__month=0;
   GET_SIM_VAL(zulu_day_sim,__day);
   GET_SIM_VAL(zulu_year_sim,__year);
   GET_SIM_VAL(zulu_hour_sim,__dt->hour);
   GET_SIM_VAL(zulu_minute_sim,__dt->min);
   GET_SIM_VAL(clock_second_sim,__dt->sec);

   for(int i=0;i<12;i++)
   {
      if(__day <= days_in_month[i])
	  {
		  __month=i;
		  break;
	  }
	  __day-=days_in_month[i];
   }
   __dt->day = __day;
   __dt->year = __year;
   __dt->month = __month;

 
}
void get_NOW_plus_this(K_dite_time_t *add)
{
   K_dite_time_t __dt;
   get_DT(&__dt);
   int res_min = __dt.min+add->min;
   int res_hour = __dt.hour+add->hour;
   if(res_min>59) 
   { res_min=res_min-60; res_hour+=1;}
   while(res_hour>23) res_hour=res_hour-24;
   add->min=res_min;
   add->hour=res_hour;
}

K_dite_time_t *sub_DT(K_dite_time_t *newer,K_dite_time_t *later)
{
    static K_dite_time_t __private_time;
	int min_bef=0;
	int hour_bef=0;
	if(newer->sec<later->sec)
	{
	   min_bef=1;  __private_time.sec = (60+newer->sec)-later->sec;
	}
	else 
	   __private_time.sec = newer->sec-later->sec;
	int mins = newer->min-min_bef;
	if(mins<later->min)
	{
	   hour_bef=1; __private_time.min = (60+mins)-later->min;
	}
	else
	   __private_time.min = mins-later->min;
    int hours = newer->hour-hour_bef;
	if(hours<later->hour)
	{
	  __private_time.hour = (24+hours)-later->hour;
	}
    return(&__private_time);
}

char *get_RNW_des(int des)
{
	switch(des)
	{
	case 1:
		return("L");
	case 2:
		return("R");
	default:
		return("");
	}
}


void show_nv_point(nv_point_t *__pt,BOOL save_screen)
{
	if(save_screen)
	   sr_screen(SCREEN_SAVE);
	switch(__pt->type)
	{
	case NAVAID_NDB:
		print_ndb((nvdb_ndb *)__pt->buffer_spec,"NDB  ");
        break;
	case NAVAID_VOR:
		print_vor((nvdb_vor *)__pt->buffer_spec,"VOR  ");
		break;
	case NAVAID_APT:
		apt_print_apt((nvdb_apt *)__pt->buffer_spec,"APT 1");
		break;
	case NAVAID_WPT:
		print_wpt((nvdb_wpt *)__pt->buffer_spec,"INT  ");
		break;
	case NAVAID_SUP:
		print_wpt((nvdb_wpt *)__pt->buffer_spec,"SUP  ");
		break;
	}
}


int get_arrow_attr(void)
{
    int attr = ATTRIBUTE_NORMAL;
	rt_leg_t *__rt = get_rt_leg();
	if(!__rt) return(attr);
	if(get_GS(1.852)<30) return(attr);
	if(on_the_ground()) return(attr);

    double rem_secs = fabs((__rt->ETE)*60.0f*60.0f);

	if(nav_mode() != FPL_LEG)
	{
	  if(rem_secs<=36.0)
		  attr=attr=ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH;
	}
	else
	{
		if(rem_secs<=20.0)
			attr=attr=ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH;
	}
	//else if(nav_mode()==FPL_LEG)
	//{
	//    double Lat,Lon;
	//	get_PPOS(&Lat,&Lon);
	//	if(36.0f>=fabs(60.0f*60.0f*(get_S(__rt->Lat_src,__rt->Lon_src,Lat,Lon) / get_GS(1))))
	//	   attr=ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH;
	//}
	return(attr);
}

void print_navaid_arrow(void *buffer_spec)
{
    int attribute = get_arrow_attr();

	rt_leg_t *__rt  = get_rt_leg();
	if(__rt)
	{
		if(__rt->dstp->buffer_spec == buffer_spec)
		{
			print_str(0,12,attribute,">");
		}
	}

}

void project_point(double x,double y,double cx,double cy,K_point_t *out_pouint,double north,double mag_var)
{
	double _CRS,_BRG,_S,_alpha;
	_CRS=north;

	_BRG   = get_ZPU(cx,cy,x,y,mag_var);
	_S     = get_S(cx,cy,x,y)/1.852f;
	_alpha = _BRG-_CRS;

	out_pouint->y = _S*cos(_alpha/57.29578);
	out_pouint->x = _S*sin(_alpha/57.29578);
}

void calc_log_line(double sLat,double sLon,double dLat,double dLon,double cLat,double cLon,K_line_t *result_line,double north,double mag_var)
{
	K_point_t p;

    project_point(sLat,sLon,cLat,cLon,&p,north,mag_var);
	result_line->x1=p.x;result_line->y1=p.y;
	project_point(dLat,dLon,cLat,cLon,&p,north,mag_var);
	result_line->x2=p.x;result_line->y2=p.y;

	CALC_K((*result_line));
	CALC_B((*result_line));
}

int find_of(int *array,int value,int max_values)
{
   for(int i=0;i<max_values;i++)
	   if(array[i]==value) return(i);
   return(-1);
}


void K_save_dword_param(char *section,char *attribute,DWORD value)
{
   char *fsdir = GetFsMainDir();
   char kln_config[MAX_PATH];
   char str_value[MAX_PATH];
   sprintf(kln_config,"%s/KLN90B/%s",fsdir,KLN_CONFIG_NAME);
   sprintf(str_value,"%lu",value);
   WritePrivateProfileString(section,attribute,str_value,kln_config);
}
BOOL K_load_dword_param(char *section,char *attribute,DWORD *value)
{
	char *fsdir = GetFsMainDir();
	char kln_config[MAX_PATH];
	char str_value[MAX_PATH];
	sprintf(kln_config,"%s/KLN90B/%s",fsdir,KLN_CONFIG_NAME);
	sprintf(str_value,"%lu",value);
	GetPrivateProfileString(section,attribute,"NULL",str_value,sizeof(str_value),kln_config);
    if(!strcmp(str_value,"NULL")) return(FALSE);
	if(value) *value = atol(str_value);
	return(TRUE);
}

void K_save_string_param(char *section,char *attribute,const TCHAR *value)
{
	char *fsdir = GetFsMainDir();
	char kln_config[MAX_PATH];
	char str_value[MAX_PATH];
	sprintf(kln_config,"%s/KLN90B/%s",fsdir,KLN_CONFIG_NAME);
	sprintf(str_value,"%s",value);
	WritePrivateProfileString(section,attribute,str_value,kln_config);
}

// ----------------------------------------------------------------------------
// Name:		K_load_string_param
// Description:	Reads a string parameter from the config file
// Parameters:	
//		section		section name in the configuration file
//		attribute	attribute (parameter) name witin the section
//		value		returned attribute string
// Return:
//		true if parameter had been found, otherwise false
// Comment:
//		Removes potential comment and also right trims the attribute value
// ----------------------------------------------------------------------------

BOOL K_load_string_param(char *section,char *attribute,TCHAR *value)
{
	char *fsdir = GetFsMainDir();
	char kln_config[MAX_PATH];
	char str_value[MAX_PATH];
	char *p;

	sprintf(kln_config,"%s/KLN90B/%s",fsdir,KLN_CONFIG_NAME);
	GetPrivateProfileString(section,attribute,"NULL",str_value,sizeof(str_value),kln_config);

	// Manipulations to get a rtrimmed path with potential embedded spaces.
	// At first cut potential comment started with ;
	p = strpbrk(str_value, ";\t");
	if (p)
		*p = '\0';
	// Right trimming the value
	char *blanks = " \t";
	for (int i = strlen(str_value); i > 0; i--)
	{
		if (strchr(blanks, str_value[i]) == NULL)
			break;
		str_value[i] = '\0';
	}

	if (!strcmp(str_value, "NULL"))
		return(FALSE);

	strcpy(value,str_value);
	return(TRUE);
}

