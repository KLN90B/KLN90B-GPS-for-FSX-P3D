#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <map>

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;

static long usr_navaids_counts[NAVAID_MAX+1];
static long usr_points_count;
long usr_ids[NAVAID_MAX+1][USR_MAX_POINTS];

int usr_ids_sort(const void *el1,const void *el2)
{
    return _stricmp(nav_db.usrs[*(long *)el1].ICAO_ID,nav_db.usrs[*(long *)el2].ICAO_ID);   
}

static long get_points_count(void)
{
   long total_points=0;
	for(long i=0;i<USR_MAX_POINTS;i++)
		if(nav_db.usrs[i].ICAO_ID[0]) total_points++;
   return(total_points);
}

long usr_spec_points(int nav_type)
{
   return(usr_navaids_counts[nav_type]);
}

nvdb_usr *usr_wpt_by_id(int nav_type,int index)
{
	return(&nav_db.usrs[usr_ids[nav_type][index]]);
}
nvdb_usr *usr_wpt_by_id2(int nav_type,int index)
{
	return(&nav_db.usrs[index]);
}
long usr_index_by_id(int nav_type,int index)
{
   return(-(usr_ids[nav_type][index]+1));
}

static long get_spec_points(int nav_type)
{
	long spec_count=0;

	for(long i=0;i<USR_MAX_POINTS;i++)
	{
		if(nav_db.usrs[i].ICAO_ID[0] && nav_db.usrs[i].usr_type == nav_type) 
		{
			usr_ids[nav_type][spec_count] = i;
			spec_count++;
		}
	}
	qsort(usr_ids[nav_type],spec_count,sizeof(long),usr_ids_sort);
	return(spec_count);
}

void calc_point_counts(void)
{
   usr_points_count=0;
	for(int i=NAVAID_MIN;i<NAVAID_MAX+1;i++)
	{
	   usr_navaids_counts[i]=get_spec_points(i);
	   usr_points_count+=usr_navaids_counts[i];
	}
}

long usr_get_pt_count(void)
{
   return(usr_points_count);
}
long find_free_slot(void)
{
   for(int i=0;i<USR_MAX_POINTS;i++)
   {
      if(!nav_db.usrs[i].ICAO_ID[0]) return(i);
   }
   return(-1);
}

nvdb_usr *usr_get_point(long index,int type)
{
   
   if(index<0)
   {
      return(&nav_db.usrs[(-index)-1]);
   }
   return(NULL);
}

void save_point_to_disk(int add_index,BOOL del_point=FALSE)
{
   char kln_dir[MAX_PATH];
   char usr_file[MAX_PATH];
   get_kln90_dir(kln_dir);
   sprintf(usr_file,"%s\\users.dat",kln_dir);
   char point_name[MAX_PATH];
   char point_str[MAX_PATH];
   nvdb_usr *__point = &nav_db.usrs[add_index];

   if(del_point)
   {
      sprintf(point_str,"NULL");
	  nav_db.usrs[add_index].ICAO_ID[0]='\0';
   }
   else
   {
      sprintf(point_str,"Lat=[%f] Lon=[%f] ICAO_ID=[%s] USRTYPE=[%d]",__point->Lat,__point->Lon,__point->ICAO_ID,__point->usr_type);
   }
   sprintf(point_name,"Point.%d",add_index);
   WritePrivateProfileString("User Points",point_name,point_str,usr_file);
}

long add_usr_point(nv_point_t *new_point)
{
   long add_index = find_free_slot();
   if(add_index<0) return(0);
   nav_db.usrs[add_index].usr_type = new_point->type;
   nv_hdr_t *new_hdr = (nv_hdr_t *)new_point->buffer_spec;
   strcpy(nav_db.usrs[add_index].ICAO_ID,new_hdr->ICAO_ID);
   nav_db.usrs[add_index].Lat = new_hdr->Lat;
   nav_db.usrs[add_index].Lon = new_hdr->Lon;
   nav_db.usrs[add_index].APT_ID[0]='\0';
   nav_db.usrs[add_index].REG_ID[0]='\0';
   nav_db.usrs[add_index].padding[0]=nav_db.usrs[add_index].padding[1]=0;
   long global_index = MAIN_POINTS_COUNT + add_index;
   naviad_info nv;
   nv.global_index=global_index;
   nv.local_index=add_index;
   nv.type = -1;
   insert_to_navaids(nav_db.usrs[add_index].Lon,nav_db.usrs[add_index].Lat,&nv,nav_db.usrs[add_index].ICAO_ID);
   calc_point_counts();
   save_point_to_disk(add_index);
   return(-(add_index+1));
}

void usr_del_point(long index)
{
	nvdb_usr *point_to_del = &nav_db.usrs[index];
	int       pt_type      = nav_db.usrs[index].usr_type;

	long global_index = MAIN_POINTS_COUNT + index;
	naviad_info nv;
	nv.global_index=global_index;
	nv.local_index=index;
	nv.type = -1;
	drop_from_navaids(nav_db.usrs[index].Lon,nav_db.usrs[index].Lat,&nv,nav_db.usrs[index].ICAO_ID);
	save_point_to_disk(index,TRUE);
	calc_point_counts();

	switch(pt_type)
	{
	case NAVAID_SUP:
		sup_nvpt_deleted((nvdb_wpt*)point_to_del);
		break;
	}

}

void init_usr_points(void)
{
   calc_point_counts();
}

