#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "navdata.h"
#include <algorithm>
#include <vector>
#include <string>


vector<nv_hdr_t> sid_points;
vector<nv_hdr_t> star_points;

using namespace std;

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

static BOOL apt_rnw_nvhdr(nv_hdr_t *rnw_coords,char *rnw_str,nvdb_apt *__apt)
{
	char temp_str[MAX_PATH];
	for(int cur_rnw=0;cur_rnw<__apt->rnws_count;cur_rnw++)
	{
	   //_snprintf(,"%02d%s",(int)apt->rws[i].pNum,get_RNW_des(apt->rws[i].pDes));
	   sprintf(temp_str,"%d%s",(int)__apt->rws[cur_rnw].pNum,get_RNW_des(__apt->rws[cur_rnw].pDes));
	   if(!strcmp(temp_str,rnw_str))
	   {
	      sprintf(rnw_coords->ICAO_ID,"RW%02d%s",(int)__apt->rws[cur_rnw].pNum,get_RNW_des(__apt->rws[cur_rnw].pDes));
		  rnw_coords->Lat = __apt->rws[cur_rnw].bLat;
		  rnw_coords->Lon = __apt->rws[cur_rnw].bLon;
		  return(TRUE);
	   }
       sprintf(temp_str,"%d%s",(int)__apt->rws[cur_rnw].sNum,get_RNW_des(__apt->rws[cur_rnw].sDes));
	   if(!strcmp(temp_str,rnw_str))
	   {
	      sprintf(rnw_coords->ICAO_ID,"RW%02d%s",(int)__apt->rws[cur_rnw].sNum,get_RNW_des(__apt->rws[cur_rnw].sDes));
		  rnw_coords->Lat = __apt->rws[cur_rnw].eLat;
		  rnw_coords->Lon = __apt->rws[cur_rnw].eLon;
		  return(TRUE);
	   }
	   //_snprintf(,"%02d%s",(int)apt->rws[i].sNum,get_RNW_des(apt->rws[i].sDes));
	}
	return(FALSE);
}

// ------------------------------------------------------------------------------------------
// ss_load_on_demand()
//
// Loads the SID and STAR name lists of the airport defined 
// in the ICAO_ID member into the provided nvdb_appt structure.
// Does nothing if the descriptions are already loaded.
// 
// Parameter
// 		__apt	pointer to airport data structure
//
// SID/STAR storage in the files
//	[LHBP_STARS]
//	Count=24
//	[LHBP_STAR_000]
//	[LHBP_STAR_001]
//	...
//	[LHBP_STAR_023]
//
// ------------------------------------------------------------------------------------------

void ss_load_on_demand(nvdb_apt *__apt)
{
	// Returns if both lists are already loaded
	if(__apt->sids && __apt->stars) 
		return;

	// It's not really possible that one of the lists exist, but let's clean them anyhow
	delete __apt->sids;
	delete __apt->stars;

	// Allocate lists
	__apt->sids = new vector<string>;
	__apt->stars = new vector<string>;

	LoadSIDNames(__apt->ICAO_ID, *__apt->sids);
	LoadSTARNames(__apt->ICAO_ID, *__apt->stars);

	K_DEBUG("Procedures loaded for %s. SIDS = %d, STARS = %d\n", __apt->ICAO_ID, __apt->sids->size(), __apt->stars->size());

	// Sorting SID and STAR names
	sort((__apt->sids)->begin(),(__apt->sids)->end());
	sort((__apt->stars)->begin(),(__apt->stars)->end());
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

void ss_load_sid_rnws(nvdb_apt *__apt,char *sid_name)
{
	__apt->sids->clear();

	LoadSIDRunways(__apt->ICAO_ID, sid_name, *__apt->sids);

	K_DEBUG("Runways loaded for airport %s SID %s. Runways = %d\n", __apt->ICAO_ID, sid_name, __apt->sids->size() );

	// Sort runways
	sort((__apt->sids)->begin(),(__apt->sids)->end());
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

void ss_load_star_rnws(nvdb_apt *__apt,char *star_name)
{
	__apt->stars->clear();

	LoadSTARRunways(__apt->ICAO_ID, star_name, *__apt->stars);

	K_DEBUG("Runways loaded for airport %s STAR %s. Runways = %d\n", __apt->ICAO_ID, star_name, __apt->stars->size());

	sort((__apt->stars)->begin(),(__apt->stars)->end());
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

void ss_load_sid_trans(nvdb_apt *__apt, char *sid_name)
{
	__apt->sids->clear();

	LoadSIDTransitions(__apt->ICAO_ID, sid_name, *__apt->sids);
	K_DEBUG("Transitions loaded for airport %s SID %s. Transitions = %d\n", __apt->ICAO_ID, sid_name, __apt->sids->size());

	// Sort transitions
	sort((__apt->sids)->begin(), (__apt->sids)->end());
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

void ss_load_star_trans(nvdb_apt *__apt, char *star_name)
{
	__apt->stars->clear();

	LoadSTARTransitions(__apt->ICAO_ID, star_name, *__apt->stars);
	K_DEBUG("Transitions loaded for airport %s STAR %s. Transitions = %d\n", __apt->ICAO_ID, star_name, __apt->stars->size());

	// Sort transitions
	sort((__apt->stars)->begin(), (__apt->stars)->end());
}

// -------------------------------------------------------------------------------------------------
// Name				ss_load_sid_points()
// Description		Loads the navigation point of the defined SID procedure from procedure file
// Parameters	
//		__apt		Airport structure
//		sid_name	Name if SID procedure
//		rnw_name	Name of runway
//		tran_name	Name of transition
// Return			-		
// Depends on
//		sid_points
// -------------------------------------------------------------------------------------------------

void ss_load_sid_points(nvdb_apt *__apt, char *sid_name, char *rnw_name, char *tran_name)
{
	sid_points.clear();
	__apt->sids->clear();

	LoadSIDPoints(__apt->ICAO_ID, sid_name, rnw_name, tran_name, sid_points);

	for (std::vector<nv_hdr_t>::iterator it = sid_points.begin(); it != sid_points.end(); ++it) 
	{
		__apt->sids->push_back( it->ICAO_ID );
	}

	K_DEBUG("Navigation points loaded for airport %s SID %s runway %s. Points = %d\n", __apt->ICAO_ID, sid_name, rnw_name, __apt->sids->size());
}

// -------------------------------------------------------------------------------------------------
// Name				ss_load_star_points()
// Description		Loads the navigation point of the defined STAR procedure from procedure file
// Parameters	
//		__apt		Airport structure
//		star_name	Name if STAR procedure
//		rnw_name	Name of runway
//		tran_name	Name of transition
// Return			-		
// Depends on
//		star_points
// -------------------------------------------------------------------------------------------------

void ss_load_star_points(nvdb_apt *__apt, char *star_name, char *rnw_name, char *tran_name)
{
	nv_hdr_t rnw_pt = { "", 0L, 0L };

	star_points.clear();
	__apt->stars->clear();

	LoadSTARPoints(__apt->ICAO_ID, star_name, rnw_name, tran_name, star_points);

	for (std::vector<nv_hdr_t>::iterator it = star_points.begin(); it != star_points.end(); ++it)
	{
		__apt->stars->push_back(it->ICAO_ID);
	}

	// Adding runway coordinates as last point for the STAR procedure if available
	if (apt_rnw_nvhdr(&rnw_pt, rnw_name, __apt))
	{
		star_points[__apt->stars->size()].Lat = rnw_pt.Lat;
		star_points[__apt->stars->size()].Lat = rnw_pt.Lon;
		strcpy(star_points[__apt->stars->size()].ICAO_ID, rnw_pt.ICAO_ID);
		__apt->stars->push_back(rnw_pt.ICAO_ID);
	}

	K_DEBUG("Navigation points loaded for airport %s STAR %s runway %s. Points = %d\n", __apt->ICAO_ID, star_name, rnw_name, __apt->stars->size());
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

void ss_reload(nvdb_apt *__apt)
{
   __apt->sids->clear();
   __apt->stars->clear();
   delete __apt->sids;
   delete __apt->stars;
   __apt->sids=NULL;
   __apt->stars=NULL;
   ss_load_on_demand(__apt);
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

nv_hdr_t *get_sid_point(int index)
{
   return(&sid_points[index]);
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

nv_hdr_t *get_star_point(int index)
{
	return(&star_points[index]);
}