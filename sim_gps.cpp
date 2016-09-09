#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "gps_info.h"
#include "fs_gauge.h"
#include <vector>
#include <map>
#include <math.h>

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;
extern int Current_Active_Leg;		// The currently active flight plan leg. Set only in calc_active_leg() and when the FPL0 is loaded or reset.

typedef BOOL (NAVFUNC)(long ind,long *local_index);

NAVFUNC *pNavFuncs[NAVAID_MAX+1]={is_ndb_id,is_vor_id,is_apt_id,is_wpt_id,is_sup_id};

fpl_t sim_gps_fpl;
extern int external_request;

nv_point_t *find_in_navdata(double Lon,double Lat,char *ICAO_ID)
{
   //K_DEBUG("Lat=[%f] Lon=[%f] ICAO_ID=[%s]\n",Lon,Lat,ICAO_ID);
   long icao_id = icao2id(ICAO_ID);
   static nv_point_t nv_pt;
   nv_pt.buffer_spec = NULL;

   if( icao_index.find(icao_id) != icao_index.end() )
   {
     int i=0;
	 for(vector<int>::iterator it = icao_index[icao_id].begin();it!=icao_index[icao_id].end();it++)
	 {
	    int ind = *it;
		long local_ind;
		for(int ii=NAVAID_MIN;ii<(NAVAID_MAX+1);ii++)
		{
		    if(!pNavFuncs[ii](ind,&local_ind))
			   continue;
		    nv_hdr_t *nv_hdr = get_navaid_buffer(local_ind,ii);
            if(!nv_hdr)
			   continue;
			if (Lon == 0 && Lat == 0) {											// we have no lat & long from MSFS, assume our DB is correct
				nv_pt.buffer_spec = nv_hdr;
				nv_pt.type = ii;
				return(&nv_pt);
			}
			if( fabs(Lon-nv_hdr->Lon)<=0.5f && fabs(Lat-nv_hdr->Lat)<=0.5f )	// If we have lat & long it must match
		    {
			   nv_pt.buffer_spec = nv_hdr;
			   nv_pt.type        = ii;
			   return(&nv_pt);
		    }	
		}
	 }
   }
   return(NULL);
}

extern INT GLOBAL_STATE;

CRITICAL_SECTION cs_afp_fpl0;

void set_fpl0_from_af(void)
{
   if(GLOBAL_STATE != STATE_MAIN_LOOP)
	   return;
   static GPS_INFO* gpsinfo;
   MODULE_VAR var;
   initialize_var_by_name (&var, GPS_INFO_PANEL_VARIABLE_NAME);
   gpsinfo = (GPS_INFO*)var.var_ptr;
   int fpl_point = 0;
   nv_point_t *nv_point;

   if(!gpsinfo)
	   return;

   if(!((gpsinfo->bIsFlightPlanAvailable) && (!gpsinfo->bIsDirectTo)  && (gpsinfo->lWpCounts>1)))
	   return;
   int total_points = gpsinfo->lWpCounts;

   if (total_points > MAX_FP_POINTS)
	   total_points = MAX_FP_POINTS;
   EnterCriticalSection(&cs_afp_fpl0);

   for(int i=0;i<total_points;i++)
   {
	   int curr_index = i;
	   if ((i == MAX_FP_POINTS - 1) && (gpsinfo->lWpCounts > MAX_FP_POINTS))
		   curr_index=gpsinfo->lWpCounts-1; 
	   double lon      = ANGL48_TO_DEGREES(gpsinfo->pWpData[curr_index].vPosition.lon);//((double)gpsinfo->pWpData[i].vPosition.lon.i64) / 51240955760304312.000000f; 
       if(lon>180) lon=lon-360.0f;
	   double lat      = RADIANS_TO_DEGREES(LAT_METERS48_TO_RADIANS(gpsinfo->pWpData[curr_index].vPosition.lat));//(FLOAT64)(SINT64)(gpsinfo->pWpData[i].vPosition.lat.i64 & ~0xFFFF) * ((1.0 / (40007000.0/(3.14159265358*2))) * (1/4294967296.0)) * (180.0f/3.14159265358f);
 	   char   *icao_id = gpsinfo->pWpData[curr_index].szName;
	   nv_point = find_in_navdata(lon,lat,icao_id);
	   if(nv_point)
	   {
		   if ( fpl_point < MAX_FP_POINTS )
		   {
		      sim_gps_fpl.points[fpl_point].buffer_spec = nv_point->buffer_spec;
		      sim_gps_fpl.points[fpl_point].type        = nv_point->type;
		      fpl_point++;
		   }
       }
   }
   if(fpl_point>1)
   {
	   sim_gps_fpl.point_in_use = fpl_point;
	   Current_Active_Leg = 0;
	   external_request=REQUEST_LOADFPL0;
   }
   LeaveCriticalSection(&cs_afp_fpl0);
}