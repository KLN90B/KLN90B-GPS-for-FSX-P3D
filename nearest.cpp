#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>
#include <vector>

extern nav_db_t nav_db;
vector<int> *get_from_navaids(double Lon,double Lat);

nr_ndb_list nr_ndb_l={0};
nr_vor_list nr_vor_l={0};
nr_apt_list nr_apt_l={0};

void add_to_lists(FLOAT64 Lon,FLOAT64 Lat,FLOAT64 ref_Lon,FLOAT64 ref_Lat)
{
    vector<int> *__nav;
 	__nav = get_from_navaids(Lon,Lat);
	for(vector<int>::iterator it = __nav->begin();it!=__nav->end();it++)
	{
	    long nav_id = *it;
		long local_index;
		if(is_ndb_id(nav_id,&local_index))
		{
			if(nr_ndb_l.nr_ndb_count<MAX_IN_LIST)
			{
			   nr_ndb_l.list[nr_ndb_l.nr_ndb_count].index = local_index;
			   nr_ndb_l.list[nr_ndb_l.nr_ndb_count].S 
			   = 
			   get_S(ref_Lat,ref_Lon,nav_db.ndbs[local_index].Lat,
				     nav_db.ndbs[local_index].Lon);
			   nr_ndb_l.list[nr_ndb_l.nr_ndb_count].radial
			   =
			   get_ZPU(ref_Lat,ref_Lon,nav_db.ndbs[local_index].Lat,
			           nav_db.ndbs[local_index].Lon,get_MAGVAR());
			   if( nr_ndb_l.list[nr_ndb_l.nr_ndb_count].S  < 373.178f )
			       nr_ndb_l.nr_ndb_count++;
			}
		}
		else if(is_vor_id(nav_id,&local_index))
		{
            if(nr_vor_l.nr_vor_count<MAX_IN_LIST)
			{
			   nr_vor_l.list[nr_vor_l.nr_vor_count].index = local_index;
			   //=========== S ============================================
			   nr_vor_l.list[nr_vor_l.nr_vor_count].S 
			   = 
			   get_S(ref_Lat,ref_Lon,nav_db.vors[local_index].Lat,
				     nav_db.vors[local_index].Lon);
			   //=========== RAD ==========================================
			   nr_vor_l.list[nr_vor_l.nr_vor_count].radial
			   =
			   get_ZPU(ref_Lat,ref_Lon,nav_db.vors[local_index].Lat,
			           nav_db.vors[local_index].Lon,get_MAGVAR());
			   if( nr_vor_l.list[nr_vor_l.nr_vor_count].S < 373.178f )
                   nr_vor_l.nr_vor_count++;
			}
		}
		else if(is_apt_id(nav_id,&local_index))
		{
			if(!nav_db.apts[local_index].rnws_count)
				continue;
			if(nav_db.apts[local_index].rws[0].len*3.281<set_get_rml())
				continue;

			if(nr_apt_l.nr_apt_count<MAX_IN_LIST)
			{
			   nr_apt_l.list[nr_apt_l.nr_apt_count].index = local_index;
			   //================= S ======================================
			   nr_apt_l.list[nr_apt_l.nr_apt_count].S 
			   = 
			   get_S(ref_Lat,ref_Lon,nav_db.apts[local_index].Lat,
				     nav_db.apts[local_index].Lon);
			   //============ RAD ==========================================
			   nr_apt_l.list[nr_apt_l.nr_apt_count].radial
			   =
			   get_ZPU(ref_Lat,ref_Lon,nav_db.apts[local_index].Lat,
			           nav_db.apts[local_index].Lon,get_MAGVAR());
			   if( nr_apt_l.list[nr_apt_l.nr_apt_count].S < 373.178f )
			       nr_apt_l.nr_apt_count++;
			}
		}
	}
}

int ndb_nr_sort(const void *a1,const void *a2)
{
	double res = ((nr_ndb *)a1)->S - ((nr_ndb *)a2)->S;
	if(res<0)
		return(-1);
	if(res>0)
		return(1);
	return(0);
}
int vor_nr_sort(const void *a1,const void *a2)
{
	double res = ((nr_vor *)a1)->S - ((nr_vor *)a2)->S;
	if(res<0)
		return(-1);
	if(res>0)
		return(1);
	return(0);
}
int apt_nr_sort(const void *a1,const void *a2)
{
    double res = ((nr_apt *)a1)->S - ((nr_apt *)a2)->S;
	if(res<0)
		return(-1);
	if(res>0)
		return(1);
	return(0);
}

double Lon_ADD(double Lon,double add)
{
     double ret = Lon + add;
	 if(ret>179)
	     ret = -180+(ret-179);
	 else if(ret<-179)
	     ret = 180+(ret+179);
	 return(ret);
}

int degs_in_km(double Lat,double kms)
{
     double KM_PER_DEG = get_S(Lat,0.0f,Lat,1.0f);
	 return((int)(kms/KM_PER_DEG)+1);
}
void build_nearest_list(void)
{
    static DWORD last_build = 0;
	FLOAT64 Lon,Lat,_lon,_lat;
	if((GetTickCount()-last_build)<3000)
		return;

    get_PPOS(&Lat,&Lon);
    nr_vor_l.nr_vor_count=nr_ndb_l.nr_ndb_count=nr_apt_l.nr_apt_count=0;
	int i,j;
	
 
	_lat=Lat-4;
	for(i=0;i<9;i++)
	{
		int lon_degs = degs_in_km(_lat,400.0f);
		_lon = Lon_ADD(Lon,-lon_degs);
		for(j=0;j<lon_degs*2+1;j++)
		{
			add_to_lists(_lon,_lat,Lon,Lat);
		    _lon=Lon_ADD(_lon,1);	
		}
		_lat++;
	}

	qsort(nr_ndb_l.list,nr_ndb_l.nr_ndb_count,sizeof(nr_ndb),ndb_nr_sort);
	qsort(nr_vor_l.list,nr_vor_l.nr_vor_count,sizeof(nr_vor),vor_nr_sort);
	qsort(nr_apt_l.list,nr_apt_l.nr_apt_count,sizeof(nr_apt),apt_nr_sort);
	
	if(nr_ndb_l.nr_ndb_count > 9)
       nr_ndb_l.nr_ndb_count = 9;
	if(nr_vor_l.nr_vor_count > 9)
       nr_vor_l.nr_vor_count = 9;
	if(nr_apt_l.nr_apt_count > 9)
       nr_apt_l.nr_apt_count = 9;

	last_build = GetTickCount();
}