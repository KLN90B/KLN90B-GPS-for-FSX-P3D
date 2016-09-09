#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "sid_star.h"
#include "navdata.h"
#include "Shlwapi.h"	// for PathFileExists()
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

nav_db_t nav_db={0};

extern DWORD load_ils;

#define RETURN_DBCHEK(n) switch(n){ case DBPART_NOTEXIST: return(DB_NOTEXIST); case DBPART_EXPIRE: return(DB_EXPIRE);   }


char ndbs_file[MAX_PATH];
char vors_file[MAX_PATH];
char wpts_file[MAX_PATH];
char sups_file[MAX_PATH];
char apts_file[MAX_PATH];
char cust_file[MAX_PATH];

enum {DBPART_NOTEXIST,DBPART_EXPIRE,DBPART_OK};

void MakeUpper(char *check1)
{
	unsigned int i;

	for (i = 0; i < strlen(check1); i++) {
		check1[i] = toupper(check1[i]);
	}
}


int check_dbpart_expiration(char *db_part_name)
{
   HANDLE file_hand = CreateFile(db_part_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
   if(INVALID_HANDLE_VALUE == file_hand)
	   return(DBPART_NOTEXIST);
   FILETIME ft_ct,ft_lat,ft_lwt;
   GetFileTime(file_hand,&ft_ct,&ft_lat,&ft_lwt);
   CloseHandle(file_hand);

   SYSTEMTIME stUTC, stLocal;
   FileTimeToSystemTime(&ft_lwt, &stUTC);
   SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
   WORD file_month,file_year;
   file_month = stLocal.wMonth;
   file_year  = stLocal.wYear;
   GetSystemTime(&stUTC);
   SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
   return DBPART_OK; //(file_month == stLocal.wMonth && file_year == stLocal.wYear?DBPART_OK:DBPART_EXPIRE);
}
int check_db(char *fs_base_dir)
{
   char dir_and_name[MAX_PATH];
   WIN32_FIND_DATA FileData;
   sprintf(dir_and_name,"%sKLN90B",fs_base_dir);
   set_kln90_dir(dir_and_name);
   HANDLE h_ourdir = FindFirstFile(dir_and_name,&FileData);
   if(h_ourdir == INVALID_HANDLE_VALUE)
   {
	   if(!CreateDirectory(dir_and_name,NULL))
		   return(DB_ERRCRITICAL);
	   FileData.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
   }   
   else
       FindClose(h_ourdir);

   if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
   {
	   sprintf(ndbs_file, "%sKLN90B\\PTT_NDB.DAT", fs_base_dir);
	   sprintf(vors_file, "%sKLN90B\\PTT_VOR.DAT", fs_base_dir);
	   sprintf(wpts_file, "%sKLN90B\\PTT_WPT.DAT", fs_base_dir);
	   sprintf(sups_file, "%sKLN90B\\PTT_SUP.DAT", fs_base_dir);
	   sprintf(apts_file, "%sKLN90B\\PTT_APT.DAT", fs_base_dir);
	   sprintf(cust_file, "%sKLN90B\\users.dat", fs_base_dir);

       RETURN_DBCHEK(check_dbpart_expiration(ndbs_file));
	   RETURN_DBCHEK(check_dbpart_expiration(vors_file));
	   RETURN_DBCHEK(check_dbpart_expiration(apts_file));
	   RETURN_DBCHEK(check_dbpart_expiration(wpts_file));
	   RETURN_DBCHEK(check_dbpart_expiration(sups_file));
	   return(DB_OK);
   } 
   return(DB_ERRCRITICAL);
}

map<unsigned long,vector<int> >icao_index;

int Create_NAVDB(char *FS_MainDir)
{
	mav_db_main(FS_MainDir);
	return(0);
}

int navaid_type(int element_number,long *local_index)
{
	if(is_ndb_id(element_number,local_index))
		return(NAVAID_NDB);
	if(is_vor_id(element_number,local_index))
		return(NAVAID_VOR);
	if(is_apt_id(element_number,local_index))
		return(NAVAID_APT);
	if(is_wpt_id(element_number,local_index))
		return(NAVAID_WPT);
	return (0);
}
inline BOOL is_ndb_equal(int ind1,int ind2)
{
	return(!ndb_compare(&nav_db.ndbs[ind1],&nav_db.ndbs[ind2]));
}
inline BOOL is_vor_equal(int ind1,int ind2)
{
   return(!vor_compare(&nav_db.vors[ind1],&nav_db.vors[ind2]));
}
inline int is_apt_equal(int ind1,int ind2)
{
	return(apt_compare(&nav_db.apts[ind1],&nav_db.apts[ind2]));
}
BOOL is_wpt_equal(int local_index,int curr_local_ind)
{
   return(FALSE);
}

inline BOOL is_in_navaid(double Lat,double Lon,naviad_info *nv_info)
{
	if( (nv_info->type==NAVAID_WPT) || (nv_info->type==NAVAID_SUP) )
		return(FALSE);
	int row = Lon<0?(DWORD)(360+Lon):(DWORD)(Lon);
	int col = (DWORD)(90+Lat);

	for(vector<int>::iterator it = nav_db.navaids[row][col].navaids.begin();
		it!=nav_db.navaids[row][col].navaids.end();
		it++
		) {

/*
	const std::vector<int> myCopy = nav_db.navaids[row][col].navaids;
	std::vector<int>::const_iterator it = myCopy.begin();
	while(it != myCopy.end())
	{*/
	  int ind = *it;
	  int curr_local_ind;

	  if(  nv_info->type 
		   == 
		   navaid_type(ind,(long *)&curr_local_ind) 
		 )
	  {
		  switch(nv_info->type)
		   {
		   case NAVAID_NDB:
			   {
			      if(is_ndb_equal(nv_info->local_index,curr_local_ind))
				     return(TRUE);
			   }
			   break;
		   case NAVAID_VOR:
			   {
			      if(is_vor_equal(nv_info->local_index,curr_local_ind))
				     return(TRUE);
			   }
			   break;
		   case NAVAID_APT:
			   {
			      int ret = is_apt_equal(nv_info->local_index,curr_local_ind);
				  if(ret!=1)
				  {
				     if(ret==2 || ret==4)
					 {
					    nvdb_apt temp_apt;
						memcpy(&temp_apt,&nav_db.apts[curr_local_ind],sizeof(nvdb_apt));
						memcpy(&nav_db.apts[curr_local_ind],&nav_db.apts[nv_info->local_index],sizeof(nvdb_apt));
						memcpy(&nav_db.apts[nv_info->local_index],&temp_apt,sizeof(nvdb_apt));
					 }
					 return(TRUE);
				  }
			   }
				  //if(is_apt_equal(nv_info->local_index,curr_local_ind))
				  //   return(TRUE);
			   break;
//		   case NAVAID_WPT:
//			   {
//			      if(is_wpt_equal(nv_info->local_index,curr_local_ind))
//				     return(TRUE);
//			   }
			   break;
		   }
	  }
	}
	return(FALSE);
}
//================= Fill elements to coord ===========================================
void insert_to_navaids(double Lon,double Lat,naviad_info *nv,char *icao_id)
{
	int row = Lon<0?(DWORD)(360+Lon):(DWORD)(Lon);
	int col = (DWORD)(90+Lat);
    if( (nv->type==-1) || (is_in_navaid(Lat,Lon,nv)==FALSE) )
	{
       nav_db.navaids[row][col].navaids.push_back(nv->global_index);
	   icao_index[icao2id(icao_id)].push_back(nv->global_index);
	}
}

void drop_from_navaids(double Lon,double Lat,naviad_info *nv,char *icao_id)
{
	int row = Lon<0?(DWORD)(360+Lon):(DWORD)(Lon);
	int col = (DWORD)(90+Lat);

	for(vector<int>::iterator it = nav_db.navaids[row][col].navaids.begin();it!=nav_db.navaids[row][col].navaids.end();it++)
	{
		if(nv->global_index == *it)
		{
			nav_db.navaids[row][col].navaids.erase(it);
			break;
		}
	}
	for(vector<int>::iterator it = icao_index[icao2id(icao_id)].begin();it!=icao_index[icao2id(icao_id)].end();it++)
	{
		if(nv->global_index == *it)
		{
			icao_index[icao2id(icao_id)].erase(it);
			break;
		}
	}
	if(icao_index[icao2id(icao_id)].empty())
	{
		map<unsigned long,vector<int> >::iterator it;
        it = icao_index.find(icao2id(icao_id));
		icao_index.erase(it);
		//icao_index.erase(icao2id(icao_id));
	}
	//nav_db.navaids[row][col].navaids.push_back(nv->global_index);
	//icao_index[icao2id(icao_id)].push_back(nv->global_index);
}

vector<int> *get_from_navaids(double Lon,double Lat)
{
	int row = Lon<0?(DWORD)(360+Lon):(DWORD)(Lon);
	int col = (DWORD)(90+Lat);
    return(&(nav_db.navaids[row][col].navaids));
}
//================= Sorting ===========================================================
int sort_ndbs(const void *arg1,const void *arg2)
{
    return(_stricmp(((nvdb_ndb *)arg1)->ICAO_ID,((nvdb_ndb *)arg2)->ICAO_ID));
}
int sort_vors(const void *arg1,const void *arg2)
{
    return(_stricmp(((nvdb_vor *)arg1)->ICAO_ID,((nvdb_vor *)arg2)->ICAO_ID));
}
int sort_apts(const void *arg1,const void *arg2)
{
	return(_stricmp(((nvdb_apt *)arg1)->ICAO_ID,((nvdb_apt *)arg2)->ICAO_ID));
}
int sort_wpts(const void *arg1,const void *arg2)
{
    return(_stricmp(((nvdb_wpt *)arg1)->ICAO_ID,((nvdb_wpt *)arg2)->ICAO_ID));
}
int sort_rnws(const void *arg1,const void *arg2)
{
	double res = ((rnw_t *)arg2)->len - ((rnw_t *)arg1)->len;
	if(res<0)
		return(-1);
	if(res>0)
		return(1);
	return(0);
}
//================= Nav DB Loading ====================================================
LONG rec_count(FILE *fp)
{
	char number[9]={'0'};
	fseek(fp,-9,SEEK_END);
	fread(number,sizeof(number)-sizeof(char),1,fp);
    number[8]='\0';
	LONG ret;
	sscanf(number,"%X",&ret);
	return(ret);
}

// ----------------------------------------------------------------------------
// Name:		Load_NDBS
// Description:	Loading NDB data from file PTT_NDB.DAT
// Parameters:	
//		fname - NDB file name
// Return:		Success status
// ----------------------------------------------------------------------------

bool Load_NDBS( const char* fname )
{
    char linebuff[MAX_PATH];
	char name[MAX_PATH];
	FILE *f;

	f = fopen(fname, "rb");
	if (!f)
		return false;

	nav_db.ndbs_count = rec_count(f);
	nav_db.ndbs = (nvdb_ndb *) K_malloc(sizeof(nvdb_ndb) * nav_db.ndbs_count );

	fseek(f, 0, SEEK_SET);
	for (int i = 0; i < nav_db.ndbs_count; i++)
	{
		
		fgets(linebuff, sizeof(linebuff) - 1, f);
		/*
		sscanf(linebuff,"Lon=[%f] Lat=[%f] ICAO_ID=[%s] REGION_ID=[%s] APT_ID=[%s] FREQ=[%lu] NAME=[%s]",
			          &__ndbs[i].Lon,&__ndbs[i].Lat,__ndbs[i].ICAO_ID,__ndbs[i].REGION_ID,
			          __ndbs[i].APT_ID,&__ndbs[i].FREQ,name);
        */
		K_GetParam(linebuff, "Lon=", DOUBLE_T, &nav_db.ndbs[i].Lon);
		K_GetParam(linebuff, "Lat=", DOUBLE_T, &nav_db.ndbs[i].Lat);
		K_GetParam(linebuff, "ICAO_ID=", STRING_T, nav_db.ndbs[i].ICAO_ID);
		K_GetParam(linebuff, "REGION_ID=", STRING_T, nav_db.ndbs[i].REGION_ID);
		K_GetParam(linebuff, "APT_ID=", STRING_T, nav_db.ndbs[i].APT_ID);
		K_GetParam(linebuff, "FREQ=", INT_T, &nav_db.ndbs[i].FREQ);
		K_GetParam(linebuff, "NAME=", STRING_T, name);
		name[22]='\0';
		MakeUpper(name);
		strcpy(nav_db.ndbs[i].NAME, name);
	}

	fclose(f);
	qsort(nav_db.ndbs, nav_db.ndbs_count, sizeof(nvdb_ndb), sort_ndbs);
	return true;
}

// ----------------------------------------------------------------------------
// Name:		Load_VORS
// Description:	Loading VOR data from file PTT_VOR.DAT
// Parameters:	
//		fname - VOR file name
// Return:		Success status
// Comment:
//		
// ----------------------------------------------------------------------------

bool Load_VORS(const char* fname)
{
    char linebuff[MAX_PATH];
	char name[MAX_PATH];
	char S_DME[4];
	char check_ils[16];
	char voricao[16];
	int vorfreq, ix;
	static long written = 0;
	static long *apt_id = NULL;
	nvdb_apt *apt = NULL;
	FILE *f;

	f = fopen(fname, "rb");
	if (!f)
		return false;

	nav_db.vors_count = rec_count(f);
	nav_db.vors = (nvdb_vor *)K_malloc(sizeof(nvdb_vor) * nav_db.vors_count);

	K_load_dword_param("KLN90B", "ILSFREQ", &load_ils);
	fseek(f, 0, SEEK_SET);
	for (int i = 0; i < nav_db.vors_count; i++)
	{
		fgets(linebuff, sizeof(linebuff) - 1, f);

		K_GetParam(linebuff, "ICAO_ID=", STRING_T, voricao);
		K_GetParam(linebuff, "APT_ID=", STRING_T, check_ils);
		K_GetParam(linebuff, "FREQ=", INT_T, &vorfreq);
		if (strlen(check_ils) > 1) 
		{
			if (load_ils) 
			{
				for (int j = 0; j < nav_db.apts_count; j++)
				{
					apt = &nav_db.apts[j];
					if (strcmp(apt->ICAO_ID, check_ils ) == 0)
					{
						for (ix = 0; ix < apt->rnws_count; ix++)
						{
							if (strcmp(voricao, apt->rws[ix].pILSicao) == 0)
								apt->rws[ix].pILS = vorfreq;
							if (strcmp(voricao, apt->rws[ix].sILSicao) == 0)
								apt->rws[ix].sILS = vorfreq;
						} 
					}
				}

				/* apt_id = find_apts_by_icao(check_ils, &written);

				   --- this code is not handling the situation with repeated ICAO. This might happen
				   --- if one is installing a custom scenery over the standard one.
				if (apt_id && written == 1) {
					apt = &nav_db.apts[apt_id[0]];
					for (ix = 0; ix < apt->rnws_count; ix++) {
						if (strcmp(voricao, apt->rws[ix].pILSicao) == 0) {
							apt->rws[ix].pILS = vorfreq;
						}
						if (strcmp(voricao, apt->rws[ix].sILSicao) == 0) {
							apt->rws[ix].sILS = vorfreq;
						}
					}
				}
				

				if (apt_id)		// if at least one airport record had been found with the ICAO id
				{
					for (int j = 0; j < written; j++)	// go through all AP records (possibly only one)
					{
						apt = &nav_db.apts[apt_id[j]];	
						for (ix = 0; ix < apt->rnws_count; ix++)
						{
							if (strcmp(voricao, apt->rws[ix].pILSicao) == 0)
								apt->rws[ix].pILS = vorfreq;
							if (strcmp(voricao, apt->rws[ix].sILSicao) == 0)
								apt->rws[ix].sILS = vorfreq;
						}
					}
				}
				*/
			}
			//continue;
		}

		strcpy(nav_db.vors[i].ICAO_ID, voricao);
		nav_db.vors[i].FREQ = vorfreq;
		K_GetParam(linebuff, "Lon=", DOUBLE_T, &nav_db.vors[i].Lon);
		K_GetParam(linebuff, "Lat=", DOUBLE_T, &nav_db.vors[i].Lat);
		K_GetParam(linebuff, "MV=", DOUBLE_T, &nav_db.vors[i].mag_var);
		K_GetParam(linebuff, "TYPE=", INT_T, &nav_db.vors[i].TYPE);
		K_GetParam(linebuff, "REG_ID=", STRING_T, nav_db.vors[i].REG_ID);
		strcpy(nav_db.vors[i].APT_ID, check_ils);
		K_GetParam(linebuff, "NAME=", STRING_T, name);
		K_GetParam(linebuff, "DME=", STRING_T, S_DME);

		name[22]='\0';
		MakeUpper(name);
		strcpy(nav_db.vors[i].NAME, name);
		nav_db.vors[i].DME = _stricmp(S_DME, "YES") ? 0 : 1;
	}

	fclose(f);
	qsort(nav_db.vors, nav_db.vors_count, sizeof(nvdb_vor), sort_vors);
	return true;
}

// ----------------------------------------------------------------------------
// Name:		Load_APTS
// Description:	Loading Airports data from file PTT_APT.DAT
// Parameters:	
//		fname - Airports file name
// Return:		Success status
// ----------------------------------------------------------------------------

bool Load_APTS( const char* fname )
{ 
	char linebuff[MAX_PATH];
//	nvdb_apt *__apts;
	char ils[6];
	FILE *f;

	f = fopen(fname, "rb");
	if (!f)
		return false;

	nav_db.apts_count = rec_count(f);
	nav_db.apts = (nvdb_apt *) K_malloc(sizeof(nvdb_apt) * nav_db.apts_count);

	fseek(f,0,SEEK_SET);
	for (int i = 0; i < nav_db.apts_count; i++)
	{
		fgets(linebuff, sizeof(linebuff) - 1, f);

		K_GetParam(linebuff, "Lon=", DOUBLE_T, &nav_db.apts[i].Lon);
		K_GetParam(linebuff, "Lat=", DOUBLE_T, &nav_db.apts[i].Lat);
		K_GetParam(linebuff, "Alt=", DOUBLE_T, &nav_db.apts[i].Alt);
		K_GetParam(linebuff, "ICAO_ID=", STRING_T, nav_db.apts[i].ICAO_ID);
		if (strcmp(nav_db.apts[i].ICAO_ID, "KAVL") == 0) {						// GH ????? What is this ???
			strcpy(ils, "");
		}
		K_GetParam(linebuff, "NAME=", STRING_T, nav_db.apts[i].NAME);
		MakeUpper(nav_db.apts[i].NAME);
		K_GetParam(linebuff, "CITY=", STRING_T, nav_db.apts[i].CITY);
		MakeUpper(nav_db.apts[i].CITY);
		K_GetParam(linebuff, "COUNTRY=", STRING_T, nav_db.apts[i].COUNTRY);
		MakeUpper(nav_db.apts[i].COUNTRY);
		strcpy(nav_db.apts[i].REG_ID, nav_db.apts[i].ICAO_ID);
		nav_db.apts[i].REG_ID[2] = '\0';
		nav_db.apts[i].NAME[22] = '\0';
		K_GetParam(linebuff, "RNWS=", INT_T, &nav_db.apts[i].rnws_count);
		nav_db.apts[i].rws = (rnw_t *)K_malloc(sizeof(rnw_t) * nav_db.apts[i].rnws_count);
		K_GetParam(linebuff, "COMS=", INT_T, &nav_db.apts[i].freqs_count);
		nav_db.apts[i].freqs = (freq_t *) K_malloc(sizeof(freq_t) * nav_db.apts[i].freqs_count);


		double max_res = 0;
		int rnw_index = 0;
		int com_index = 0;
		for (int subrec = 0; subrec < nav_db.apts[i].rnws_count + nav_db.apts[i].freqs_count; subrec++)
		{
			char *p = linebuff + 4;

			fgets(linebuff, sizeof(linebuff) - 1, f);		// Reads subrecord line

			if (strncmp(linebuff, "RNW", 3) == 0)			// If runway description line...
			{
				double _temp_d;
				INT   _temp_i;
				double __bLat, __bLon;
				K_line_t *ln;

				K_GetParam(p, "SRF=", STRING_T, nav_db.apts[i].rws[rnw_index].surf, '{', '}');
				K_GetParam(p, "rLat=", DOUBLE_T, &nav_db.apts[i].rws[rnw_index].bLat, '{', '}');
				K_GetParam(p, "rLon=", DOUBLE_T, &nav_db.apts[i].rws[rnw_index].bLon, '{', '}');
				K_GetParam(p, "HDG=", DOUBLE_T, &_temp_d, '{', '}');
				nav_db.apts[i].rws[rnw_index].HDG = _temp_d;
				K_GetParam(p, "LEN=", DOUBLE_T, &_temp_d, '{', '}');
				nav_db.apts[i].rws[rnw_index].len = _temp_d;

				K_GetParam(p, "LF=", INT_T, &_temp_i, '{', '}');
				nav_db.apts[i].rws[rnw_index].light_flag = (BYTE)(_temp_i & 31);
				K_GetParam(p, "PF=", INT_T, &_temp_i, '{', '}');
				nav_db.apts[i].rws[rnw_index].pattern_flag = (BYTE)_temp_i;

				K_GetParam(p, "PN=", INT_T, &_temp_i, '{', '}');
				nav_db.apts[i].rws[rnw_index].pNum = _temp_i;
				K_GetParam(p, "PD=", INT_T, &_temp_i, '{', '}');
				nav_db.apts[i].rws[rnw_index].pDes = _temp_i;
				K_GetParam(p, "SN=", INT_T, &_temp_i, '{', '}');
				nav_db.apts[i].rws[rnw_index].sNum = _temp_i;
				K_GetParam(p, "SD=", INT_T, &_temp_i, '{', '}');
				nav_db.apts[i].rws[rnw_index].sDes = _temp_i;

				strcpy(ils, "");
				K_GetParam(p, "PI=", STRING_T, ils, '{', '}');
				strcpy(nav_db.apts[i].rws[rnw_index].pILSicao, ils);
				strcpy(ils, "");
				K_GetParam(p, "SI=", STRING_T, ils, '{', '}');
				strcpy(nav_db.apts[i].rws[rnw_index].sILSicao, ils);
				nav_db.apts[i].rws[rnw_index].pILS = 0;
				nav_db.apts[i].rws[rnw_index].sILS = 0;

				get_LL(nav_db.apts[i].rws[rnw_index].bLat, nav_db.apts[i].rws[rnw_index].bLon, nav_db.apts[i].rws[rnw_index].len / 1000.0f / 2,
					nav_db.apts[i].rws[rnw_index].HDG, &nav_db.apts[i].rws[rnw_index].eLat, &nav_db.apts[i].rws[rnw_index].eLon);
				get_LL(nav_db.apts[i].rws[rnw_index].bLat, nav_db.apts[i].rws[rnw_index].bLon, nav_db.apts[i].rws[rnw_index].len / 1000.0f / 2,
					__add_deg(nav_db.apts[i].rws[rnw_index].HDG, 180.0f), &__bLat, &__bLon);
				nav_db.apts[i].rws[rnw_index].bLat = __bLat; nav_db.apts[i].rws[rnw_index].bLon = __bLon;

				calc_log_line(nav_db.apts[i].rws[rnw_index].bLat, nav_db.apts[i].rws[rnw_index].bLon, nav_db.apts[i].rws[rnw_index].eLat,
					nav_db.apts[i].rws[rnw_index].eLon, nav_db.apts[i].Lat, nav_db.apts[i].Lon, &nav_db.apts[i].rws[rnw_index].apt3_line);

				_snprintf(nav_db.apts[i].rws[rnw_index].apt3_line.pDes, sizeof(nav_db.apts[i].rws[rnw_index].apt3_line.pDes) - 1,
					"%02d%s", (int)nav_db.apts[i].rws[rnw_index].pNum, get_RNW_des(nav_db.apts[i].rws[rnw_index].pDes));

				_snprintf(nav_db.apts[i].rws[rnw_index].apt3_line.sDes, sizeof(nav_db.apts[i].rws[rnw_index].apt3_line.sDes) - 1,
					"%02d%s", (int)nav_db.apts[i].rws[rnw_index].sNum, get_RNW_des(nav_db.apts[i].rws[rnw_index].sDes));

				ln = &nav_db.apts[i].rws[rnw_index].apt3_line;

				if (fabs(ln->x1) > max_res)
					max_res = fabs(ln->x1);
				if (fabs(ln->x2) > max_res)
					max_res = fabs(ln->x2);
				if (fabs(ln->y1) > max_res)
					max_res = fabs(ln->y1);
				if (fabs(ln->y2) > max_res)
					max_res = fabs(ln->y2);

				rnw_index++;
			}

			else if (strncmp(linebuff, "COM", 3) == 0)		// If COM description line
			{
				K_GetParam( p, "TYPE=", INT_T, &nav_db.apts[i].freqs[com_index].type, '{', '}');
				K_GetParam( p, "FREQ=", INT_T, &nav_db.apts[i].freqs[com_index].freq, '{', '}');
				com_index++;
			}
		}

		nav_db.apts[i].sids = NULL;
		nav_db.apts[i].stars = NULL;
		nav_db.apts[i].apt3_resolution = 2.0f*max_res;
		qsort(nav_db.apts[i].rws, nav_db.apts[i].rnws_count, sizeof(rnw_t), sort_rnws);      
	}

	fclose(f);
	qsort(nav_db.apts, nav_db.apts_count, sizeof(nvdb_apt), sort_apts);
	return true;
}

// ----------------------------------------------------------------------------
// Name:		Load_SUPS
// Description:	Loading supplies data from file PTT_SUP.DAT
// Parameters:	
//		fname - Supplies file name
// Return:		Success status
// ----------------------------------------------------------------------------

bool Load_SUPS( const char* fname)
{
	char linebuff[MAX_PATH];
	char sub_type[4];
	FILE *f;

	f = fopen(fname, "rb");
	if (!f)
		return false;

	nav_db.sups_count = rec_count(f);
	nav_db.sups = (nvdb_wpt *) K_malloc(sizeof(nvdb_wpt) * nav_db.sups_count);

	fseek(f, 0, SEEK_SET);
	for (int i = 0; i < nav_db.sups_count; i++)
	{
		fgets(linebuff, sizeof(linebuff) - 1, f);
		K_GetParam(linebuff, "Lon=", DOUBLE_T, &nav_db.sups[i].Lon);
		K_GetParam(linebuff, "Lat=", DOUBLE_T, &nav_db.sups[i].Lat);
		K_GetParam(linebuff, "ICAO_ID=", STRING_T, nav_db.sups[i].ICAO_ID);
		K_GetParam(linebuff, "REG_ID=", STRING_T, nav_db.sups[i].REG_ID);
		K_GetParam(linebuff, "APT_ID=", STRING_T, nav_db.sups[i].APT_ID);
		K_GetParam(linebuff, "TYPE=", INT_T, &nav_db.sups[i].TYPE);
		K_GetParam(linebuff, "ROUTES=", INT_T, &nav_db.sups[i].ROUTES);
		nav_db.sups[i].SUB_TYPE = 0;
	}

	fclose(f);
	qsort(nav_db.sups, nav_db.sups_count, sizeof(nvdb_wpt), sort_wpts);
	return true;
}

// ----------------------------------------------------------------------------
// Name:		USRS
// Description:	Loading custom (user) waypoint data from file users.dat
// Parameters:	
//		fname - Custom waypoint file name
// Return:		Success status
// ----------------------------------------------------------------------------

bool Load_USRS(const char* fname)
{
	char buffer[MAX_PATH];
	char name[MAX_PATH];

	nav_db.usrs_count = USR_MAX_POINTS;
	nav_db.usrs = (nvdb_usr *) K_malloc(sizeof(nvdb_wpt) * nav_db.usrs_count);

	for (int i = 0;  i < nav_db.usrs_count; i++)
	{
		sprintf(name, "Point.%d", i);
		DWORD bytes = GetPrivateProfileString("User Points", name, "NULL", buffer, sizeof(buffer) - 1, fname);
		if (!bytes || !strcmp(buffer, "NULL"))
		{
			nav_db.usrs[i].ICAO_ID[0] = '\0';
			nav_db.usrs[i].usr_type = -1;
			continue;
		}
		K_GetParam(buffer, "Lat=", DOUBLE_T, &nav_db.usrs[i].Lat);
		K_GetParam(buffer, "Lon=", DOUBLE_T, &nav_db.usrs[i].Lon);
		K_GetParam(buffer, "ICAO_ID=", STRING_T, &nav_db.usrs[i].ICAO_ID);
		K_GetParam(buffer, "USRTYPE=", INT_T, &nav_db.usrs[i].usr_type);
	}

	//qsort(nav_db.usrs,nav_db.usrs_count,sizeof(nvdb_usr),sort_usrs);
	init_usr_points();
	return true;
}


// ----------------------------------------------------------------------------
// Name:		WPTS
// Description:	Loading waypoint data from file PTT_WPT.DAT
// Parameters:	
//		fname - Waypoint file name
// Return:		Success status
// ----------------------------------------------------------------------------

bool Load_WPTS( const char* fname )
{
	K_DEBUG("[HG] Entering Load_WPTS()\n");

	nav_db.wpts_count = 0;

	if (GetNavDatabaseCode() > 0)		// If external navigation database pack had been defined
	{
		vector<nvdb_wpt > ints;
		bool success = false;

		// Loads waypoints from external database
		success = LoadWaypoints(ints);

		// Add a fake point to the database if we had failed to load any data
		if (!success)
		{
			ints.push_back({ " ", 0, 0, "", "", 1, 0, 0 });
		}

		nav_db.wpts_count = ints.size();
		K_DEBUG("[HG] Number of waypoints read: %d\n", nav_db.wpts_count);
		nav_db.wpts = (nvdb_wpt *) K_malloc(sizeof(nvdb_wpt) * nav_db.wpts_count);
		int wpt_number = 0;
		for (vector<nvdb_wpt>::iterator it = ints.begin(); it != ints.end(); it++)
		{
			nav_db.wpts[wpt_number++] = *it;
		}
		ints.clear();
	}
	else			// Only BLG files are used, no external waypoint and procedure pack is defined
	{
		char linebuff[MAX_PATH];
		char sub_type[4];
		FILE *f;

		f = fopen(fname, "rb");
		if (!f)
			return false;

		nav_db.wpts_count = rec_count(f);
		nav_db.wpts = (nvdb_wpt *) K_malloc(sizeof(nvdb_wpt) * nav_db.wpts_count);

		fseek(f, 0, SEEK_SET);
		for (int i = 0; i < nav_db.wpts_count; i++)
		{
			fgets(linebuff, sizeof(linebuff) - 1, f);
			K_GetParam(linebuff, "Lon=", DOUBLE_T, &nav_db.wpts[i].Lon);
			K_GetParam(linebuff, "Lat=", DOUBLE_T, &nav_db.wpts[i].Lat);
			K_GetParam(linebuff, "ICAO_ID=", STRING_T, nav_db.wpts[i].ICAO_ID);
			K_GetParam(linebuff, "REG_ID=", STRING_T, nav_db.wpts[i].REG_ID);
			K_GetParam(linebuff, "APT_ID=", STRING_T, nav_db.wpts[i].APT_ID);
			K_GetParam(linebuff, "TYPE=", INT_T, &nav_db.wpts[i].TYPE);
			K_GetParam(linebuff, "ROUTES=", INT_T, &nav_db.wpts[i].ROUTES);
			K_GetParam(linebuff, "SUB_TYPE=", STRING_T, sub_type);
			if (_stricmp(sub_type, "WPT"))
				nav_db.wpts[i].SUB_TYPE = 1;
			else if (_stricmp(sub_type, "MRK"))
				nav_db.wpts[i].SUB_TYPE = 2;
			else
				nav_db.wpts[i].SUB_TYPE = 0;
		}

		fclose(f);
	}

	qsort(nav_db.wpts, nav_db.wpts_count, sizeof(nvdb_wpt), sort_wpts);
	K_DEBUG("[HG] Leaving Load_WPTS()\n");
	return true;
}

//  ------------------------------------------------------------------

BOOL is_ndb_id(long ind,long *local_index)
{
	if(ind < nav_db.ndbs_count)
	{
	    if(local_index) *local_index = ind;
		return(TRUE);
	}
	return(FALSE);
}
BOOL is_vor_id(long ind,long *local_index)
{
	if(ind < (nav_db.ndbs_count+nav_db.vors_count) && (ind>=nav_db.ndbs_count))
	{
	    if(local_index) 
			*local_index = ind - nav_db.ndbs_count;
		return(TRUE);
	}
	return(FALSE);
}
BOOL is_apt_id(long ind,long *local_index)
{
	if(ind < (nav_db.ndbs_count+nav_db.vors_count+nav_db.apts_count) && (ind>=nav_db.ndbs_count+nav_db.vors_count))
	{
	    if(local_index) 
			*local_index = ind - nav_db.ndbs_count - nav_db.vors_count;
		return(TRUE);
	}
	return(FALSE);
}

BOOL is_wpt_id(long ind,long *local_index)
{
	if( ind < (nav_db.ndbs_count+nav_db.vors_count+nav_db.apts_count+nav_db.wpts_count) 
		&&
	   ( ind>=nav_db.ndbs_count+nav_db.vors_count+nav_db.apts_count )	
	   )
	{
	    if(local_index) 
			*local_index = ind - nav_db.ndbs_count - nav_db.vors_count - nav_db.apts_count;
		return(TRUE);
	}
	return(FALSE);
}

BOOL is_sup_id(long ind,long *local_index)
{
	long sup_beg_ind = nav_db.ndbs_count+nav_db.vors_count+nav_db.apts_count+nav_db.wpts_count;
	if(ind >= sup_beg_ind)
	{
	    if(ind < (sup_beg_ind+nav_db.sups_count))
		{
		   if(local_index) 
		      *local_index = ind - sup_beg_ind;
		   return(TRUE);
		}
		else
		{
		   long usr_index = ind - (sup_beg_ind+nav_db.sups_count);
		   if(local_index) 
			   *local_index = -(usr_index+1);
		   return(TRUE);
		}
	}

	return(FALSE);
}
nv_hdr_t *get_navaid_buffer(long local_index,int type)
{
    if(type==NAVAID_NDB)
	{
		if(local_index < nav_db.ndbs_count && local_index>=0)
			return((nv_hdr_t *)&nav_db.ndbs[local_index]);
	}
    if(type==NAVAID_VOR)
	{
		if(local_index < nav_db.vors_count && local_index>=0)
			return((nv_hdr_t *)&nav_db.vors[local_index]);
	}
    if(type==NAVAID_APT)
	{
		if(local_index < nav_db.apts_count && local_index>=0)
			return((nv_hdr_t *)&nav_db.apts[local_index]);
	}
    if(type==NAVAID_WPT)
	{
		if(local_index < nav_db.wpts_count && local_index>=0)
			return((nv_hdr_t *)&nav_db.wpts[local_index]);
	}
    if(type==NAVAID_SUP)
	{
		if(local_index < nav_db.sups_count && local_index>=0)
			return((nv_hdr_t *)&nav_db.sups[local_index]);
	}
	return(NULL);
}
void Build_Navaids(void)
{
   int i;
   naviad_info nv;
   for(i=0;i<nav_db.ndbs_count;i++)
   {
	   nv.global_index=i;
	   nv.local_index=i;
	   nv.type = NAVAID_NDB;
	   if(i==65) {
		   int iii=0;
		   iii++;
	   }
	   insert_to_navaids(nav_db.ndbs[i].Lon,nav_db.ndbs[i].Lat,&nv,nav_db.ndbs[i].ICAO_ID);
   }
   for(i=0;i<nav_db.vors_count;i++)
   {
	   nv.global_index=i+nav_db.ndbs_count;
	   nv.local_index=i;
	   nv.type = NAVAID_VOR;
	   insert_to_navaids(nav_db.vors[i].Lon,nav_db.vors[i].Lat,&nv,nav_db.vors[i].ICAO_ID);
   }
   for(i=0;i<nav_db.apts_count;i++)
   {
	   nv.global_index=i+nav_db.ndbs_count+nav_db.vors_count;
	   nv.local_index=i;
	   nv.type = NAVAID_APT;
	   insert_to_navaids(nav_db.apts[i].Lon,nav_db.apts[i].Lat,
	                     &nv,
						 nav_db.apts[i].ICAO_ID);
   }
   for(i=0;i<nav_db.wpts_count;i++)
   {
	   nv.global_index=i+nav_db.ndbs_count+nav_db.vors_count+nav_db.apts_count;
	   nv.local_index=i;
	   nv.type = NAVAID_WPT;
	   insert_to_navaids(nav_db.wpts[i].Lon,nav_db.wpts[i].Lat,
	                     &nv,
						 nav_db.wpts[i].ICAO_ID);
   }
   for(i=0;i<nav_db.sups_count;i++)
   {
	   nv.global_index=i+nav_db.ndbs_count+nav_db.vors_count+nav_db.apts_count+nav_db.wpts_count;
	   nv.local_index=i;
	   nv.type = NAVAID_SUP;
	   insert_to_navaids(nav_db.sups[i].Lon,nav_db.sups[i].Lat,
		   &nv,
		   nav_db.sups[i].ICAO_ID);
   }
   for(i=0;i<nav_db.usrs_count;i++)
   {
	   long usr_ind_beg = nav_db.ndbs_count+nav_db.vors_count+nav_db.apts_count+nav_db.wpts_count+nav_db.sups_count;
	   nv.global_index=i+usr_ind_beg;
	   nv.local_index=-1;
	   nv.type = nav_db.usrs[i].usr_type;
	   if(nav_db.usrs[i].ICAO_ID[0])
	      insert_to_navaids(nav_db.usrs[i].Lon,nav_db.usrs[i].Lat,&nv,nav_db.usrs[i].ICAO_ID);
   }
}

// -----------------------------------------------------------------------------------
// Name:		Load_NAVDB
// -----------------------------------------------------------------------------------

void Load_NAVDB(char *FS_MainDir)
{
	char FS_MagVar_File[MAX_PATH];

	memset(&nav_db, 0, sizeof(nav_db));		// initializes navigation DB
	
	// Loading navigation points. 
	// APTs should be loaded prior to VORs as VOR data refers to airports
	Load_NDBS(ndbs_file);
	Load_APTS(apts_file);
	Load_VORS(vors_file);
	Load_WPTS(wpts_file);
	Load_SUPS(sups_file);
	Load_USRS(cust_file);

	Build_Navaids();

	sprintf(FS_MagVar_File,"%sScenery\\Base\\Scenery\\magdec.bgl",FS_MainDir);
	read_magdec(FS_MagVar_File);

	do_fpl_pages(ACTION_INIT);
	do_apt_page(ACTION_INIT);
}

// -------------------------------------------------------------------------------------

#define FREE_NVDB(p,c) if(p){K_free(p);p=NULL;c=0;}
void UnLoad_NAVDB(void)
{
	for(int i=0;i<nav_db.apts_count;i++)
	{
		if(nav_db.apts[i].rws) K_free(nav_db.apts[i].rws);
		if(nav_db.apts[i].freqs) K_free(nav_db.apts[i].freqs);
		if(nav_db.apts[i].sids)
		{
		   (nav_db.apts[i].sids)->clear();
		   delete nav_db.apts[i].sids;

		}
		if(nav_db.apts[i].stars)
		{
		   (nav_db.apts[i].stars)->clear();
		   delete nav_db.apts[i].stars;
		}
	}
	FREE_NVDB(nav_db.apts,nav_db.apts_count);
	FREE_NVDB(nav_db.wpts,nav_db.wpts_count);
	FREE_NVDB(nav_db.vors,nav_db.vors_count);
	FREE_NVDB(nav_db.ndbs,nav_db.ndbs_count);
	FREE_NVDB(nav_db.sups,nav_db.sups_count);
	FREE_NVDB(nav_db.usrs,nav_db.usrs_count);

	for(map<unsigned long,vector<int> >::iterator it = icao_index.begin();it!=icao_index.end();++it)
	{
		long aaa = it->first;
		vector<int> v = it->second;
		v.clear();
	}
	icao_index.clear();

    for(int i=0;i<360;i++)
       for(int j=0;j<181;j++)
           nav_db.navaids[i][j].navaids.clear();
	do_ndb_page(ACTION_FREE_RESOURCES);
	do_vor_page(ACTION_FREE_RESOURCES);
	do_apt_page(ACTION_FREE_RESOURCES);
	do_wpt_page(ACTION_FREE_RESOURCES);
	do_sup_page(ACTION_FREE_RESOURCES);
	do_fpl_pages(ACTION_FREE_RESOURCES);
	do_nav_page(ACTION_FREE_RESOURCES,0);
}
#define BGLID           0x0001
#define ENDOFDATA       "End of Data "

typedef unsigned char   _byte;
typedef unsigned short  _word;
typedef signed short    _int;

typedef _int             pseudodegree_t;
typedef	pseudodegree_t  magdectable_t[360][181];

typedef	struct _tag_magdechdr_t
{
  _int   nBglID;         // BGL file ID: 0x0001	
  _byte  unknown1[108];  // all zero
  _int   nHdrSize;       // Header size? 0x0080
  _byte  unknown2[16];   // all zero
  _word  nRowNum;        // number of rows (longitudes) in table: 360
  _word  nColNum;        // number of columns (latitudes) intable: 181 (incl Equator)
  _byte  bDay;           // date day? (01)
  _byte  bMonth;         // date month? (01)
  _byte  bYear;          // date year? (93)
  _byte  bCentury;       // date century? (19)
} magdechdr_t;

#pragma pack(1)
typedef struct _tag_magdeclbgl_t
{
  magdechdr_t   hdr;
  magdectable_t mdt;
  char          eofdata[12];
}magdecbgl_t;
#pragma pack()
magdecbgl_t mag_dec_file;

float f_magdectable[360][181];

void read_magdec(char *FS_MagDec_File)
{
   FILE *fp = fopen(FS_MagDec_File,"rb");
   fread(&mag_dec_file,sizeof(mag_dec_file),1,fp);
   for(int meredian=0;meredian<360;meredian++)
   {
	   for(int parallel=0;parallel<=180;parallel++)
	   {
		   f_magdectable[meredian][parallel]
		   = 
		   (float)mag_dec_file.mdt[meredian][parallel] / 65536.0f * 360.0f;
	   }
   }
   fclose(fp);
}

double get_magdec(double Lon,double Lat)
{
	if(Lat>74.0f || Lat<-60.0f)
		return(0.0f);
	
	int row2 = Lon < 0 ? ceil(360+Lon):ceil(Lon);
	int row1 = Lon < 0 ? floor(360+Lon):floor(Lon);
	int col2 = ceil(90+Lat);
	int col1 = floor(90+Lat);

	if(row1 < 0 ) row1 = 359; if(row1 > 359 ) row1 = 0;
	if(row2 < 0 ) row2 = 359; if(row2 > 359 ) row2 = 0;

	if(col1 < 0 ) col1 = 180; if(col1 > 180 ) col1 = 0; 
	if(col2 < 0 ) col2 = 180; if(col2 > 180 ) col2 = 0; 

	double K4 = f_magdectable[row1][col2];
	double K1 = f_magdectable[row1][col1];
	double K3 = f_magdectable[row2][col2];
	double K2 = f_magdectable[row2][col1];
	int m=(Lat-(int)(Lat))*10;
	int n=(Lon-(int)(Lon))*10;
    double K = m*((K3-K2)/10.0f) + K1 + n*((K3-K1)/10.0f);
	return(K);
}