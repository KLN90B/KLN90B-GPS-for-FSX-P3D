#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "sid_star.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <map>

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;

extern int Current_Active_Leg;			// The currently active flight plan leg. Set only in calc_active_leg() and when the FPL0 is loaded or reset.
extern int Correct_Sequencing;			// 1 to use KLN90B correct.  0 to use the original "closest waypoint in the direction we're pointing is always active".
extern double Sequencing_Distance;		// how close we have to be to waypoint to sequence to the next one, in Get_S units
extern int Current_Mode = 0;			// 0 = none set yet 1=FPL is active 2=DTO is active 3=FPL DTO active
extern DWORD interface_obsout;			// whether we can set the OBS or they have to

static int fpl_number = 0;
static int index_in_fpl = 0;
static int FPL_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_ENTER|INPUT_CLR;
static int cursored = 0;
static int editing = 0;
static int row_e = 0;
static int col_e = 0;
static int pre_approve = 0;
static int on_load_0 = 0;
static int on_use = 0;
static int on_use_invert = 0;
static int on_delete=0;
static int on_delete_point=0;
static char Sequenced[16] = "";

const int  COL_START_EDIT = 4;
const int  COL_END_EDIT  = 8;

static long *ids_list;
static long ids_list_count;
static nv_point_t active_point;
int calc_active_leg(void);
BOOL is_null_before(fpl_t *__fpl,int index);
int CompareHeadings(int heading1, int heading2);

#define MAX_FLIGHTPLANS	26
fpl_t fpls[MAX_FLIGHTPLANS] = { 0 };

nv_hdr_t fp_sid_points[MAX_SS_POINTS];
nv_hdr_t fp_star_points[MAX_SS_POINTS];

BOOL fpl_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&FPL_INPUT_MASK);
}

//=======================================================================================
void fpl_set_navigation(void);
BOOL is_useable_fpl(fpl_t *__fpl);
//=======================================================================================

void CalcFPL(fpl_t *__fpl)
{
	FLOAT64 TotalLen = 0;
	__fpl->points[0].abs_dis=0;
	__fpl->points[0].dis_from_beg=0;

	for(int i=1;i<__fpl->point_in_use;i++)
	{
		if(!__fpl->points[i-1].buffer_spec || !__fpl->points[i].buffer_spec)
			continue;
		FLOAT64 sLat = ((nv_hdr_t *)__fpl->points[i-1].buffer_spec)->Lat;
		FLOAT64 sLon = ((nv_hdr_t *)__fpl->points[i-1].buffer_spec)->Lon;
		FLOAT64 dLat = ((nv_hdr_t *)__fpl->points[i].buffer_spec)->Lat;
		FLOAT64 dLon = ((nv_hdr_t *)__fpl->points[i].buffer_spec)->Lon;
		__fpl->points[i].abs_dis = get_S(sLat,sLon,dLat,dLon)/1.852f;
		__fpl->points[i].dis_from_beg = TotalLen+__fpl->points[i].abs_dis;
		TotalLen+=__fpl->points[i].abs_dis;
		__fpl->points[i].dtk = get_ZPU(sLat,sLon,dLat,dLon,get_magdec(sLon,sLat));
	}
}
//=======================================================================================
nv_hdr_t *find_nv_point(char *text_str,int *pt_type)
{
  int type;
  double Lon, Lat;
  char ICAO_ID[6];
  K_GetParam(text_str,"TYPE=",INT_T,&type);
  K_GetParam(text_str,"Lon=",DOUBLE_T,&Lon);
  K_GetParam(text_str,"Lat=",DOUBLE_T,&Lat);
  K_GetParam(text_str,"ICAO_ID=",STRING_T,ICAO_ID);
  long icao_id = icao2id(ICAO_ID);
  nv_hdr_t *temp_nv=NULL;

  if(icao_index.find(icao_id) != icao_index.end())
  {
     int i=0;
	 for(vector<int>::iterator it = icao_index[icao_id].begin();it!=icao_index[icao_id].end();it++)
	 {
	    int ind = *it;
		long local_ind;

        if(is_ndb_id(ind,&local_ind) && type == NVDB_NDB)
		{
			if((nav_db.ndbs[local_ind].Lat == Lat) && (nav_db.ndbs[local_ind].Lon == Lon))
			{
               temp_nv = (nv_hdr_t *)&nav_db.ndbs[local_ind];
			   break;
			}
		}
		else if(is_vor_id(ind,&local_ind) && type == NVDB_VOR)
		{
		    if((nav_db.vors[local_ind].Lat == Lat) && (nav_db.vors[local_ind].Lon == Lon))
			{
               temp_nv = (nv_hdr_t *)&nav_db.vors[local_ind];
			   break;
			}
		}
		else if(is_apt_id(ind,&local_ind) && type == NVDB_APT)
		{
		    if((nav_db.apts[local_ind].Lat == Lat) && (nav_db.apts[local_ind].Lon == Lon))
			{
               temp_nv = (nv_hdr_t *)&nav_db.apts[local_ind];
			   break;
			}
		}
		else if(is_wpt_id(ind,&local_ind) && type == NVDB_WPT)
		{
		    if((nav_db.wpts[local_ind].Lat == Lat) && (nav_db.wpts[local_ind].Lon == Lon))
			{
               temp_nv = (nv_hdr_t *)&nav_db.wpts[local_ind];
			   break;
			}
		}
		else if(is_sup_id(ind,&local_ind) && type == NVDB_SUP)
		{
		    if((get_sup_by_id(local_ind)->Lat == Lat) && (get_sup_by_id(local_ind)->Lon == Lon))
			{
               temp_nv = (nv_hdr_t *)get_sup_by_id(local_ind);
			   break;
			}
		}
	 }
  }
  if(temp_nv)
  {
     if(pt_type)
		 *pt_type=type;
	 return(temp_nv);
  }
  return(NULL);
}

// ------------------------------------------------------------------------
// Name:		set_fpl_empty
// ------------------------------------------------------------------------

void set_fpl_empty(fpl_t *__fpl)
{
   __fpl->point_in_use = 1;
   __fpl->points[0].buffer_spec = NULL;
}

// ------------------------------------------------------------------------
// Name:		Load_FPLS
// ------------------------------------------------------------------------

void Load_FPLS(void)
{
    char __file[MAX_PATH];
	char __sec[MAX_PATH];
	char __str[MAX_PATH];
	char __key[MAX_PATH];

	get_kln90_dir(__file);
    strcat(__file,"fpls.dat");
	for ( int i = 0; i < MAX_FLIGHTPLANS; i++ )
    {
	   _snprintf(__sec,sizeof(__sec)-1,"FPL.%d",i);
	   GetPrivateProfileString(__sec,"PointsCount","NULL",__str,sizeof(__str)-1,__file);

       if(!strcmp(__str,"NULL"))
	   {
	      set_fpl_empty(&fpls[i]);
	   }
	   else
	   {
	       int points_count = atoi(__str);
		   for(int ii=0;ii<points_count;ii++)
		   {
		       _snprintf(__key,sizeof(__key)-1,"Point.%d",ii);
               GetPrivateProfileString(__sec,__key,"NULL",__str,sizeof(__str)-1,__file);
			   if(!strcmp(__str,"NULL"))
			   {
				   set_fpl_empty(&fpls[i]);
				   break;
			   }
               int type;
			   nv_hdr_t *fpl_pt;
			   fpl_pt = find_nv_point(__str,&type);
			   if(!fpl_pt)
			   {
				   set_fpl_empty(&fpls[i]);
				   break;   
			   }
			   fpls[i].points[ii].buffer_spec = fpl_pt;
			   fpls[i].points[ii].type = type;
		   }
		   if(fpls[i].points[0].buffer_spec)
			   fpls[i].point_in_use = points_count;
		   if(!points_count)
		   {
		      fpls[i].point_in_use = 1;
			  fpls[i].points[0].buffer_spec = NULL;
		   }
	   }
	   CalcFPL(&fpls[i]);
    }
    //if(is_useable_fpl(&fpls[0]))
	//	fpl_set_navigation();
}

// ------------------------------------------------------------------------
// Name:		Save_FPL
// ------------------------------------------------------------------------

void Save_FPL(fpl_t *__fpl,int f_number)
{
	char __str[MAX_PATH];
	char __key[MAX_PATH];
	char __sec[MAX_PATH];
	char __file[MAX_PATH];
	int i;
    int real_points=0;

	// Assembles FP file name
	get_kln90_dir(__file);
	strcat(__file,"fpls.dat");

	// Assembles the actual FP section name and clears all entries in it in the file
	_snprintf(__sec, sizeof(__sec) - 1, "FPL.%d", f_number);
	WritePrivateProfileString(__sec, NULL, NULL, __file);

	// Saves all non navaid FP points
	// [FPL.0]
	// Point.0 = TYPE = [2] ICAO_ID = [LZKZ] Lat = [48.663056] Lon = [21.241111]
	// Point.1 = TYPE = [3] ICAO_ID = [KEKED] Lat = [48.523056] Lon = [21.291389]

	for(i = 0; i < __fpl->point_in_use; i++)
	{
		if ((!__fpl->points[i].buffer_spec) || (__fpl->points[i].type == NAVAID_SS_POINT))
			continue;

		_snprintf(__key, sizeof(__key) - 1, "Point.%d", real_points);
		_snprintf(__str, sizeof(__str) - 1, 
					"TYPE=[%d] ICAO_ID=[%s] Lat=[%f] Lon=[%f]", 
					__fpl->points[i].type,
					((nv_hdr_t *)__fpl->points[i].buffer_spec)->ICAO_ID,
					((nv_hdr_t *)__fpl->points[i].buffer_spec)->Lat,
					((nv_hdr_t *)__fpl->points[i].buffer_spec)->Lon);
		WritePrivateProfileString(__sec,__key,__str,__file);
		real_points++;
	}

	// Saves number of FP points into the file
	// PointsCount=5
    _snprintf(__str,sizeof(__str)-1,"%d",real_points);   
	WritePrivateProfileString(__sec,"PointsCount",__str,__file);
	CalcFPL(__fpl); 
}


//=======================================================================================
nv_point_t *show_active_point(long point_id)
{
      long local_index;
	  sr_screen(SCREEN_SAVE);
	  nv_point_t *_ret=NULL;
      if(is_ndb_id(point_id,&local_index))
	  {
		  active_point.buffer_spec = &nav_db.ndbs[local_index];
		  active_point.type = NAVAID_NDB;
		  _ret = &active_point;
		  print_ndb((nvdb_ndb *)active_point.buffer_spec,"NDB  ");
	  }
	  else if(is_vor_id(point_id,&local_index))
	  {
		  active_point.buffer_spec = &nav_db.vors[local_index];
		  active_point.type = NAVAID_VOR;
		  _ret = &active_point;
		  print_vor((nvdb_vor *)active_point.buffer_spec,"VOR  ");
	  }
	  else if(is_apt_id(point_id,&local_index))
	  {
		  active_point.buffer_spec = &nav_db.apts[local_index];
		  active_point.type = NAVAID_APT;
		  _ret = &active_point;
		  apt_print_apt((nvdb_apt *)active_point.buffer_spec,"APT 1");
	  }
	  else if(is_wpt_id(point_id,&local_index))
	  {
		  active_point.buffer_spec = &nav_db.wpts[local_index];
		  active_point.type = NAVAID_WPT;
		  _ret = &active_point;
		  print_wpt((nvdb_wpt *)active_point.buffer_spec,"INT  ");
	  }
	  else if(is_sup_id(point_id,&local_index))
	  {
		  active_point.buffer_spec = get_sup_by_id(local_index);
		  active_point.type = NAVAID_SUP;
		  _ret = &active_point;
		  print_wpt((nvdb_wpt *)active_point.buffer_spec,"SUP  ");
	  }
	  update_screen();
	  return(_ret);
}
//=======================================================================================
void get_icao_by_id(long point_id,char *__ICAO_ID,nv_point_t *point)
{
      long local_index;
	  nv_point_t __nv_pt;

	  if(is_ndb_id(point_id,&local_index))
	  {
	     if(__ICAO_ID) strcpy(__ICAO_ID,nav_db.ndbs[local_index].ICAO_ID);
		 __nv_pt.type=NAVAID_NDB;
		 __nv_pt.buffer_spec=&nav_db.ndbs[local_index];
	  }
	  else if(is_vor_id(point_id,&local_index))
	  {
	     if(__ICAO_ID) strcpy(__ICAO_ID,nav_db.vors[local_index].ICAO_ID);	  
	     __nv_pt.type=NAVAID_VOR;
	     __nv_pt.buffer_spec=&nav_db.vors[local_index];
	  }
	  else if(is_apt_id(point_id,&local_index))
	  {
         if(__ICAO_ID) strcpy(__ICAO_ID,nav_db.apts[local_index].ICAO_ID);
	     __nv_pt.type=NAVAID_APT;
	     __nv_pt.buffer_spec=&nav_db.apts[local_index];
	  }
	  else if(is_wpt_id(point_id,&local_index))
	  {
         if(__ICAO_ID) strcpy(__ICAO_ID,nav_db.wpts[local_index].ICAO_ID);
	     __nv_pt.type=NAVAID_WPT;
	     __nv_pt.buffer_spec=&nav_db.wpts[local_index];
	  }
	  else if(is_sup_id(point_id,&local_index))
	  {
          if(__ICAO_ID) strcpy(__ICAO_ID,get_sup_by_id(local_index)->ICAO_ID);
	     __nv_pt.type=NAVAID_SUP;
	     __nv_pt.buffer_spec=get_sup_by_id(local_index);
	  }
	  if(point) *point=__nv_pt;
}
extern int  count_of_elem;
extern char str_elems[100][12];
extern int  current_item;
//=======================================================================================
nv_hdr_t *get_pt_hdr(int global_index)
{
   long local_index;
   if(is_ndb_id(global_index,&local_index))
      return((nv_hdr_t*)&nav_db.ndbs[local_index]);
   if(is_vor_id(global_index,&local_index))
      return((nv_hdr_t*)&nav_db.vors[local_index]);
   if(is_apt_id(global_index,&local_index))
      return((nv_hdr_t*)&nav_db.apts[local_index]);
   if(is_wpt_id(global_index,&local_index))
      return((nv_hdr_t*)&nav_db.wpts[local_index]);
   if(is_sup_id(global_index,&local_index))
      return((nv_hdr_t*)get_sup_by_id(local_index));
   return(NULL);
}

int ids_sort(const void *a1,const void *a2)
{
	double Lat,Lon;
	get_PPOS(&Lat,&Lon);
	double res = get_S(Lat,Lon,((nv_hdr_t*)get_pt_hdr(*(int*)a1))->Lat,
		               ((nv_hdr_t*)get_pt_hdr(*(int*)a1))->Lon) 
					   -
			     get_S(Lat,Lon,((nv_hdr_t*)get_pt_hdr(*(int*)a2))->Lat,
				               ((nv_hdr_t*)get_pt_hdr(*(int*)a2))->Lon);
	if(res<0)
		return(-1);
	if(res>0)
		return(1);
	return(0);
}

void build_ml_list(void)
{
   char *__ICAO_ID=NULL;
   count_of_elem = ids_list_count;
   qsort(ids_list,ids_list_count,sizeof(int),ids_sort);

   for(int i=0;i<ids_list_count;i++)
   {
      long local_index;
	  if(is_ndb_id(ids_list[i],&local_index))
	  {
		  sprintf(str_elems[i],"%2d NDB  %s?",i+1,nav_db.ndbs[local_index].REGION_ID); 
		  if(!__ICAO_ID)
			  __ICAO_ID = nav_db.ndbs[local_index].ICAO_ID;
	  }
	  if(is_vor_id(ids_list[i],&local_index))
	  {
		  sprintf(str_elems[i],"%2d VOR  %s?",i+1,nav_db.vors[local_index].REG_ID); 
		  if(!__ICAO_ID)
			  __ICAO_ID = nav_db.vors[local_index].ICAO_ID;
	  }
	  if(is_apt_id(ids_list[i],&local_index))
	  {
		  sprintf(str_elems[i],"%2d APT  %s?",i+1,nav_db.apts[local_index].REG_ID); 
		  if(!__ICAO_ID)
			  __ICAO_ID = nav_db.apts[local_index].ICAO_ID;
	  }
	  if(is_wpt_id(ids_list[i],&local_index))
	  {
		  sprintf(str_elems[i],"%2d INT  %s?",i+1,nav_db.wpts[local_index].REG_ID); 
		  if(!__ICAO_ID)
			  __ICAO_ID = nav_db.wpts[local_index].ICAO_ID;
	  }
	  if(is_sup_id(ids_list[i],&local_index))
	  {
		  sprintf(str_elems[i],"%2d SUP  %s?",i+1,get_sup_by_id(local_index)->REG_ID); 
		  if(!__ICAO_ID)
			  __ICAO_ID = get_sup_by_id(local_index)->ICAO_ID;
	  }
   }
   sprintf(str_elems[99],"%s",__ICAO_ID);
}
void fpl_build_ml_list(int count,long *list)
{
   ids_list_count = count;
   ids_list = list;
   build_ml_list();
}
//=======================================================================================
void copy_fpl(fpl_t *dst,fpl_t *src)
{
	int real_points=0;
	for(int i=0;i<src->point_in_use;i++)
	{
		if(!src->points[i].buffer_spec) continue;
		if(src->points[i].type == NAVAID_SS_POINT) continue;
		dst->points[real_points].buffer_spec = src->points[i].buffer_spec;
		dst->points[real_points].type = src->points[i].type;
		real_points++;
	}
	dst->point_in_use = real_points;
}
long *find_ids_by_icao(char *icao_id_s,long *array_size)
{
    static DWORD ids_indexes[1024];
	int i;
	DWORD icao_id = icao2id(icao_id_s);
	i=0;
	if(icao_index.find(icao_id) != icao_index.end())
	{
		i=0;
		for(vector<int>::iterator it = icao_index[icao_id].begin();it!=icao_index[icao_id].end();it++)
		{
		  int ind = *it;
          ids_indexes[i++] = ind;
		}
	}
	if(i>0)
	{
	   if(array_size) *array_size=i;
	   return((long *)&ids_indexes);
	}
   //===============================================================================
   #define arrayof(a) (sizeof(a)/sizeof(a[0]))
   #define stringof(a) (sizeof(a)/sizeof(a[0])-1 )

   #define MAX_SYMBOL 5
   int start_index;
   char symbol[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char ar[MAX_SYMBOL+1],index[MAX_SYMBOL+1],*start_string = "";
   memset(ar,0,sizeof(ar));
   strcpy(ar,icao_id_s);   
   start_index = strlen(icao_id_s);
         
   for ( int i=start_index;i<stringof(ar);i++) 
   {
     memset(index+start_index,0,sizeof(index)-(start_index*sizeof(index[0])));
   
     while ( index[start_index] < stringof(symbol) ) {

		 int k=0;
       for ( k = start_index;k<=i;k++ ) {  ar[k] = symbol[index[k]]; }
       ar[k]=0;
	   //************************************
	   icao_id = icao2id(ar);
	   if(icao_index.find(icao_id) != icao_index.end())
	   {
	      i=0;
		  for(vector<int>::iterator it = icao_index[icao_id].begin();it!=icao_index[icao_id].end();it++)
		  {
		     int ind = *it;
             ids_indexes[i++] = ind;
		  }
	      if(array_size) *array_size=i;
	      return((long *)&ids_indexes);
	   }
	   //************************************
       for( int j=i;j>=start_index;j--) {
         if( ++index[j] < stringof(symbol) || j==start_index )
          break;
         index[j]=0;
       }
     }
   }
   //===============================================================================
   return(NULL);
}
//=======================================================================================
static int leg_number=0;
static int leg_fpldto_number = 0;

int get_prev(int ln)
{
	if(ln-1>=0)
	{
		if(fpls[0].points[ln-1].buffer_spec)
			return(ln-1);
		else if(ln-2>=0)
			return(ln-2);
	}
	return(-1);
}

int fpl_get_leg(void)
{
	return(leg_number);
}
int fpl_get_active_leg(void)
{
   if( !(nav_mode() == FPL_LEG || nav_mode() == FPLDTO_LEG) )
	   return(-1);
   int n_leg = nav_mode() == FPL_LEG ? calc_active_leg() : leg_fpldto_number;
   if(!fpls[0].points[n_leg].buffer_spec) n_leg++;
   return(n_leg);
}
fpl_t *fpl_GetCurrentFPL(int *fpl_number_ptr)
{
   if(fpl_number_ptr)
	   *fpl_number_ptr = fpl_number;
   return(&fpls[fpl_number]);
}
void fpl_set_fpldto(void)
{
   leg_fpldto_number=index_in_fpl;
}
void fpl_set_next_leg(void)
{
	//if(leg_number==(fpls[0].point_in_use-1))
	//	leg_number=0;
    //leg_number++;
//!!!!!!!!!!!!!!!!!! Fix stack overflow BUG !!!!!!!!!!!!!!!!!!!!!!!!
	//if((fpls[0].point_in_use == 2) && (nav_mode() == FPL_LEG) )
	//	return;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //fpl_set_curr_leg();
	//fpl_set_navigation();
}
//void fpl_set_curr_leg(void)
//{
//   leg_number = calc_active_leg();
//   if(leg_number)
//      set_route_leg(&fpls[0].points[leg_number-1],&fpls[0].points[leg_number],FPL_LEG);
//   else
//      set_route_leg(NULL,NULL,FPL_LEG);
//}

FLOAT64 get_dtk_leg(int leg_number)
{
	if(!(leg_number<fpls[0].point_in_use && leg_number>0)) 
	   return(INVALID_ANGLE);
    if(!fpls[0].points[leg_number-1].buffer_spec || !fpls[0].points[leg_number].buffer_spec)
       return(INVALID_ANGLE);

	FLOAT64 src_Lat = ((nv_hdr_t *)fpls[0].points[leg_number-1].buffer_spec)->Lat;
	FLOAT64 src_Lon = ((nv_hdr_t *)fpls[0].points[leg_number-1].buffer_spec)->Lon;
	FLOAT64 dst_Lat = ((nv_hdr_t *)fpls[0].points[leg_number].buffer_spec)->Lat;
	FLOAT64 dst_Lon = ((nv_hdr_t *)fpls[0].points[leg_number].buffer_spec)->Lon;

	return(get_ZPU(src_Lat,src_Lon,dst_Lat,dst_Lon,0/*get_magdec(src_Lon,src_Lat))*/));
}
FLOAT64 fpl_get_next_leg_dtk(FLOAT64 *__lbu,FLOAT64 *__dis,rt_leg_t *__rt)
{
	int si = fpl_get_active_leg();
	if(si<0) return(INVALID_ANGLE);
	int di=si+1;
	if(fpls[0].point_in_use <= di) return(INVALID_ANGLE);
	if(!fpls[0].points[di].buffer_spec) return(INVALID_ANGLE);
	nv_hdr_t *sp = (nv_hdr_t *)fpls[0].points[si].buffer_spec;
	nv_hdr_t *dp = (nv_hdr_t *)fpls[0].points[di].buffer_spec;
	FLOAT64 Lat,Lon,a,SS;
	get_PPOS(&Lat,&Lon);
	if(__lbu)
	   *__lbu = get_LBU(sp->Lat,sp->Lon,dp->Lat,dp->Lon,Lat,Lon,&a,&SS)/1.852;
	if(__dis)
	   *__dis = get_S(sp->Lat,sp->Lon,dp->Lat,dp->Lon)/1.852;
    
	if(__rt)
	{
	   __rt->Lat_src = sp->Lat;
	   __rt->Lat_dst = dp->Lat;
	   __rt->Lon_src = sp->Lon;
	   __rt->Lon_dst = dp->Lon;
	   __rt->OD = get_ZPU(__rt->Lat_src,__rt->Lon_src,__rt->Lat_dst,__rt->Lon_dst,0);
	   __rt->leg_crs = mod_get_mode();
	}

	return(get_ZPU(sp->Lat,sp->Lon,dp->Lat,dp->Lon,0));
}
BOOL fpl_get_actual_track(rt_leg_t *__rt)
{
   if(!is_useable_fpl(&fpls[0])) return(FALSE);
   leg_number=calc_active_leg();
   if(!leg_number) return(FALSE);
   int prev = get_prev(leg_number);
   if(prev>=0)
   {
		int next = leg_number;
		if(!fpls[0].points[next].buffer_spec)
			next++;
		if(__rt)
		{
			nv_hdr_t *sp = (nv_hdr_t *)fpls[0].points[prev].buffer_spec;
			nv_hdr_t *dp = (nv_hdr_t *)fpls[0].points[next].buffer_spec;

			__rt->Lat_src = sp->Lat;
			__rt->Lat_dst = dp->Lat;
			__rt->Lon_src = sp->Lon;
			__rt->Lon_dst = dp->Lon;
			__rt->OD = get_ZPU(__rt->Lat_src,__rt->Lon_src,__rt->Lat_dst,__rt->Lon_dst,0);
			__rt->leg_crs = mod_get_mode();
			return(TRUE);
		}
	}
   return(FALSE);
}

FLOAT64 fpl_get_active_leg_dtk(void)
{
    return(get_dtk_leg(leg_number));
}


void fpl_set_navigation(void)
{
	if(nav_mode() == DTO_LEG)
	{
	   nv_point_t *__dtopt = dto_get_cp();
	   for(int i=0;i<fpls[0].point_in_use;i++)
	   {
		   if(fpls[0].points[i].buffer_spec == __dtopt->buffer_spec)
		   {
		      leg_fpldto_number=i;
		      dto_change_mode(DTO_MODE_FPL);
			  return;
		   }
	   }
	}
	if(!is_useable_fpl(&fpls[0]))
	{
		set_route_leg(NULL,NULL,FPL_LEG);
		return;
	}
	leg_number=calc_active_leg();
	if(leg_number)
	{
	   int prev = get_prev(leg_number);
	   if(prev>=0)
	   {
		  int next = leg_number;
		  if(!fpls[0].points[next].buffer_spec)
			  next++;
		  
		  if((fpls[0].points[prev].type == NAVAID_STAR_MAIN) || 
	         (fpls[0].points[prev].type == NAVAID_SID_MAIN))
		    prev--;

	      set_route_leg(&fpls[0].points[prev],&fpls[0].points[next],FPL_LEG);
	   }
	   else
	      set_route_leg(NULL,NULL,FPL_LEG);
	}
	else
       set_route_leg(NULL,NULL,FPL_LEG);
}
void fpl_continue_navigation(void)
{
	//leg_number = leg_fpldto_number;
	//fpl_set_next_leg();
	//fpl_set_navigation();
}
//=======================================================================================
BOOL is_useable_fpl(fpl_t *__fpl)
{
	if(__fpl->point_in_use < 2 )
		return(FALSE);
	int nulls=0;
	for(int i=0;i<__fpl->point_in_use;i++)
		if(!__fpl->points[i].buffer_spec) nulls++;

	if((__fpl->point_in_use - nulls) > 1)
		return(TRUE);
	return(FALSE);
}
void print_fpl_header(fpl_t *__fpl)
{
   if( (row_e != index_in_fpl && index_in_fpl < 5)
	   ||
	   (!cursored)
	   ||
	   (__fpl->point_in_use<6))
   {
      if(fpl_number)
	  {
	     if(is_useable_fpl(__fpl))
	        print_str(0,0,ATTRIBUTE_NORMAL,"%-11s","USE? INVRT?");
	     else
	        print_str(0,0,ATTRIBUTE_NORMAL,"%-11s","LOAD FPL 0?");
	  }
	  else
	  {
		  if(cursored && fpls[0].point_in_use<6) print_str(0,0,ATTRIBUTE_NORMAL,"%-11s"," ");  
	  }
   }
}
void clear_fpl_arrows(void)
{
    for(int i=0;i<6;i++)
		print_str(i,0,ATTRIBUTE_NORMAL," ");
}

void print_fpl_arrows(int index,int screen_row)
{
    if(editing) return;
	BOOL to_symb_already=FALSE;
	BOOL from_symb_already=FALSE;
	BOOL vert_symb_already=FALSE;

	int up_line = index-screen_row;
	int start_row = 0;
	if(up_line<0)
	{
	   start_row=1;
	   up_line=0;
	}
	int actual_line = up_line;
	char ch=BLANK_CHAR;

	for(;start_row<6;start_row++)
	{
	   if( up_line>=fpls[0].point_in_use || !is_useable_fpl(&fpls[0]) )
		   break;
       if(start_row==5)
	   {
		  if(ch=='{' && actual_line == (fpls[0].point_in_use-1))
		  {
		     if(!to_symb_already)
			 {
		        ch='}';
				to_symb_already=TRUE;
			 }
		  }
		  else if(nav_mode() == FPLDTO_LEG)
		  {
			  if( (leg_fpldto_number == fpls[0].point_in_use-1)
				  &&  
				  fpls[0].points[fpls[0].point_in_use-1].buffer_spec )
			  {
				  if(!to_symb_already)
				  {
					  ch='>';
					  to_symb_already=TRUE;
				  }			  
			  }                 
			  else
			     ch=BLANK_CHAR;
		  }
		  else
              ch=BLANK_CHAR;
		  //if(!(on_delete_point && actual_line == index_in_fpl))
		  if(K_get_char(start_row,0)!='D')
		  {
             print_str(start_row,0,ATTRIBUTE_NORMAL,"%c",ch);
			 if(ch=='>' || ch=='}') K_change_attribute(start_row,0,1,get_arrow_attr());
		  }
		  break;
	   }
	   ch = BLANK_CHAR;
	   if(fpls[0].points[up_line].buffer_spec)
	   {
		   rt_leg_t *__rt = get_rt_leg();
		   nv_hdr_t *__nv = (nv_hdr_t *)fpls[0].points[up_line].buffer_spec;
		   if(nav_mode() == FPL_LEG)
		   {

		     //if( actual_line == (leg_number-1) )
			//	 ch = '{';//print_str(start_row,0,ATTRIBUTE_NORMAL,"<");
	         //else if( actual_line == leg_number )
			//	 ch = '}';//print_str(start_row,0,ATTRIBUTE_NORMAL,">");
			 //else
			 //{
				if((__rt->Lon_dst == __nv->Lon)&&(__rt->Lat_dst == __nv->Lat)&&!strcmp(__nv->ICAO_ID,__rt->dst_icao_id))
				{
				   if(!to_symb_already)
				   {
					   ch = '}';
					   to_symb_already=TRUE;
				   }			  
				}
				else if((__rt->Lon_src == __nv->Lon)&&(__rt->Lat_src == __nv->Lat)&&!strcmp(__nv->ICAO_ID,__rt->src_icao_id))
				{
					if(!from_symb_already)
					{
						ch = '{';
						from_symb_already=TRUE;
					}			  		 					
				}
		  }
		  else if(nav_mode() == FPLDTO_LEG)
		  {

			  rt_leg_t *__rt = get_rt_leg();
			  nv_hdr_t *__nv = (nv_hdr_t *)fpls[0].points[up_line].buffer_spec;
			  //if(actual_line == leg_fpldto_number)
			  //   ch = '>';//print_str(start_row,0,ATTRIBUTE_NORMAL,">");
			  //else
			  if((__rt->Lon_dst == __nv->Lon)&&(__rt->Lat_dst == __nv->Lat)&&!strcmp(__nv->ICAO_ID,__rt->dst_icao_id))
			  {
				  if(!to_symb_already)
				  {
					  ch = '>';
					  to_symb_already=TRUE;
				  }			  			  
			  }			     
		  }
		  actual_line++;
	   }
	   else
	   {
	      if(fpls[0].points[up_line].type==NAVAID_STAR_MAIN||fpls[0].points[up_line].type==NAVAID_SID_MAIN)
		  {
	         if(nav_mode() == FPL_LEG)
			 {
			    rt_leg_t *__rt = get_rt_leg();
				nv_hdr_t *__nv;
				if(fpls[0].points[up_line+1].buffer_spec)
				   __nv = (nv_hdr_t *)fpls[0].points[up_line+1].buffer_spec;
				else if(fpls[0].points[up_line+2].buffer_spec)
				   __nv = (nv_hdr_t *)fpls[0].points[up_line+1].buffer_spec;
				if((__nv->Lat==__rt->Lat_dst) && (__nv->Lon==__rt->Lon_dst))
				{			
					if(!vert_symb_already)
					{
						ch='|';
						vert_symb_already=TRUE;
					}			  			  
				}				   
			 }	     
		  }
	   }
	   up_line++;
	   //if(!(on_delete_point && (fpls[0].points[up_line].buffer_spec ? actual_line-1:actual_line) == index_in_fpl))
	   if(K_get_char(start_row,0)!='D')
	   {
	      print_str(start_row,0,ATTRIBUTE_NORMAL,"%c",ch);
		  if(ch=='>' || ch=='}') K_change_attribute(start_row,0,1,get_arrow_attr());
	   }
	}
	update_screen();
}
void calc_fpl0_indexes(int *index,int *row_in_screen)
{
   int start_row,start_number;
   fpl_t *__fpl = &fpls[0];

   if(nav_mode() == FPL_LEG || nav_mode() == FPLDTO_LEG)
      start_row=0;
   else
	  start_row=1;

   if(__fpl->point_in_use>5)
   {
//      start_row=0;
	  int cLeg = fpl_get_active_leg();
	  if(cLeg>0)
	  {
         if(cLeg+4<__fpl->point_in_use)
            start_number=cLeg;
		 else
            start_number = __fpl->point_in_use - 5;
	  }
	  else
	     start_number=1;
   }
   else
   {
//      start_row=1;
      start_number=1;
   }
   if(on_delete)
      start_row=1;
   else if( cursored && fpls[0].point_in_use < 6 )
      start_row=1;

   *index=start_number;  
   *row_in_screen=start_row;
}
int fpl_get_num(fpl_t *__fpl,int pt_number)
{
   int real_numb=0;
   for(int i=0;i<pt_number;i++)
   {
       if(!__fpl->points[i].buffer_spec)
	   {
	      if(__fpl->points[i].type==NAVAID_SID_MAIN || __fpl->points[i].type==NAVAID_STAR_MAIN)
		     continue;
	   }
	   real_numb++;
   }
   return(real_numb);
}
char fpl_get_typechar(int type)
{
   if(type==NAVAID_SS_POINT) return('.');
   return ':';
}
void fpl_print_pt(int _ind_in_fpl,int pt_number,fpl_t *__fpl,int i)
{
	if(__fpl->points[_ind_in_fpl].buffer_spec)
	{
		print_str(i,0,ATTRIBUTE_NORMAL,"%3d%c%-7s",fpl_get_num(__fpl,pt_number),fpl_get_typechar(__fpl->points[_ind_in_fpl].type),((nv_hdr_t*)__fpl->points[_ind_in_fpl].buffer_spec)->ICAO_ID);
	}
	else
	{
		if(__fpl->points[_ind_in_fpl].type==NAVAID_STAR_MAIN || __fpl->points[_ind_in_fpl].type==NAVAID_SID_MAIN)
			print_str(i,0,ATTRIBUTE_NORMAL," %-10s",__fpl->points[_ind_in_fpl].alt_name);
		else
			print_str(i,0,ATTRIBUTE_NORMAL,"%3d:%-7s",fpl_get_num(__fpl,pt_number)," ");
	}
}

void print_fpl(fpl_t *__fpl,int number,char *status,int stat_attr)
{
   int i;
   
   if(!number)
   {
	   int start_row;
	   int start_number;
	   char ch_before=BLANK_CHAR;
       int index,screen_row;

	   calc_fpl0_indexes(&start_number,&start_row);

	   if( start_row ==1 && start_number==1 && !on_delete)
	      print_str(0,0,ATTRIBUTE_NORMAL,"%-11s"," ");
       
	   index = start_number-1;
	   screen_row = start_row;

	   if(!__fpl->points[start_number-1].buffer_spec)
	   {
	      if(__fpl->points[start_number-1].type == NAVAID_SID_MAIN || __fpl->points[start_number-1].type == NAVAID_STAR_MAIN)
		  {
		     start_number--;		     
			 index = start_number-1;
		  }
	   }

       for(int i=start_row;i<6;i++)
	   {
	      if(i==5)
		  {
		     if( start_number<=__fpl->point_in_use )
			 {
		        fpl_print_pt(fpls[0].point_in_use-1,fpls[0].point_in_use,&fpls[0],i);
			 }
			 else
			    print_str(i,0,ATTRIBUTE_NORMAL,"%-11s"," ");
			 break;
		  }
		  if(start_number<=__fpl->point_in_use)
	         fpl_print_pt(start_number-1,start_number,__fpl,i);
		  else
		     print_str(i,0,ATTRIBUTE_NORMAL,"%-11s"," ");                      
	      start_number++;
	   }
	   print_fpl_arrows(index,screen_row);
   }
   else
   {
      print_fpl_header(&fpls[fpl_number]);
	  for(i=0;i<__fpl->point_in_use && i<4;i++)
		  print_str(1+i,0,ATTRIBUTE_NORMAL,"%3d:%-7s",i+1,
		                                   __fpl->points[i].buffer_spec
										   ?
	                                          ((nv_hdr_t*)__fpl->points[i].buffer_spec)->ICAO_ID
										   :
	                                          " "
	                );
	  for(;i<5;i++)
		  print_str(1+i,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	  if(__fpl->point_in_use>4)
		  print_str(5,0,ATTRIBUTE_NORMAL,"%3d:%-7s",__fpl->point_in_use,
		                                 __fpl->points[__fpl->point_in_use-1].buffer_spec
										 ?
		                                    ((nv_hdr_t*)__fpl->points[__fpl->point_in_use-1].buffer_spec)->ICAO_ID
										 :
	                                        " "
										 );
   }

   if(status)
   {
	  print_str(6,0,stat_attr,"%s",status);
	  if(stat_attr == ATTRIBUTE_INVERSE)
		  K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
   }
   else
      print_str(6,0,ATTRIBUTE_NORMAL,"FPL%2d",number);

}
//===================================================================================
void scrool_list(fpl_t *fpl,int start_index,int count)
{
   for(int i=0;i<fpl->point_in_use && (i<count);i++)
   {
      fpl_print_pt(start_index+i,start_index+1+i,fpl,i);
   }
}
//=====================================================================================
void scrool_list2(fpl_t *fpl,int start_index,int count)
{
   for(int i=0;i<fpl->point_in_use && (i<count);i++)
   {
	   if(i==5)
	   {
	      fpl_print_pt(fpl->point_in_use-1,fpl->point_in_use,fpl,i);
		  break;
	   }
       fpl_print_pt(start_index+i,start_index+1+i,fpl,i);
   }
}

// -------------------------------------------------------------------------------------------------
// Name				insert_into_fpl()
// Description		Insert one navigation point into the given position of the defined flight plan
// Parameters	
//		__fpl		Flight plan to use
//		nv_point	Navigation point to insert
//		index		Position to insert to. Starting from 0
// Returns			-		
// Depends on		
//		fpl_number
//		leg_fpldto_number
//		nav_mode()
// -------------------------------------------------------------------------------------------------

void insert_into_fpl( fpl_t *__fpl, nv_point_t *nv_point, int index)
{ 
	// If inserting a point into the active (0) flight plan before an active direct to
	// flight plan point, then we should adjust (increase) the direct to point index.
	if ( (nav_mode() == FPLDTO_LEG) && !fpl_number)
	{
		if ( index <= leg_fpldto_number )
		{
			leg_fpldto_number++;
		}
	}

	// Shift all points after the indertion point one location forward
	memmove( &__fpl->points[index+1], &__fpl->points[index], (__fpl->point_in_use-index) * sizeof(nv_point_t) );
	__fpl->point_in_use++;	

	// Mark in the flight plan if navigation aid point
	if( !nv_point )
	{
       __fpl->points[index].buffer_spec = NULL;
       __fpl->points[index].type        = NAVAID_NULL;
	}
	else
	{
	   __fpl->points[index].buffer_spec = nv_point->buffer_spec;
	   __fpl->points[index].type = nv_point->type;
	}
}

// -------------------------------------------------------------------------------------------------
// Name				delete_from_fpl()
// Description		Deletes one navigation point from the given position of the defined flight plan
// Parameters	
//		__fpl		Flight plan to use
//		index		Position of the nav point to delete. Starting from 0
// Return			-		
// Depends on
//		fpl_number
//		leg_fpldto_number
//		nav_mode()
//		dto_change_mode()
// -------------------------------------------------------------------------------------------------

void delete_from_fpl(fpl_t *__fpl, int index)
{
	K_DEBUG("[HG] ENTERING: delete_from_fpl() | index=%d | name=%s\n", index, (__fpl->points[index].buffer_spec) ? __fpl->points[index].buffer_spec : __fpl->points[index].alt_name);

	// If deleting the from the active flight plan with a direct to point active...
	if( (nav_mode() == FPLDTO_LEG) && !fpl_number )
	{
		// Switching back to normal flight plan mode if the active direct to node is deleted
		if (leg_fpldto_number == index)
		{
			dto_change_mode( DTO_MODE_NORMAL );
		}
		// If deleting a point from the active (0) flight plan before an active direct to
		// flight plan point, then we should adjust (decrease) the direct to point index.
		if ( index <= leg_fpldto_number )
		{
			leg_fpldto_number--;
		}
	}

	// Shift all points after the indertion point one location back
	memmove( &__fpl->points[index], &__fpl->points[index + 1], (__fpl->point_in_use - index) * sizeof(nv_point_t) );
	__fpl->point_in_use--;	

	// ??? Adjust if the only one flight plan point is deleted ???
	if( __fpl->point_in_use <= 0)
	{
	   __fpl->point_in_use = 1;
	   __fpl->points[0].buffer_spec = NULL;
	}
}

// -------------------------------------------------------------------------------------------------
// Name				is_fp_full()
// Description		Checks if FP0 is full
// Parameters		Flight plan number
// Returns			true if full, false otherwise		
// -------------------------------------------------------------------------------------------------

bool is_fp_full(int fp_num)
{
	return (fpls[fp_num].point_in_use >= MAX_FP_POINTS);
}

// -------------------------------------------------------------------------------------------------
// Name				is_fp0_full()
// Description		Checks if FP0 is full
// Parameters		-
// Returns			true if full, false otherwise		
// -------------------------------------------------------------------------------------------------

bool is_fp0_full()
{
	return is_fp_full(0);
}

// -------------------------------------------------------------------------------------------------
// Name				append_to_fpl0()
// Description		Append a navigation point to the active flight plan FP0
// Parameters	
//		nv_point	Navigation point to insert
// Returns			-		
// -------------------------------------------------------------------------------------------------

void append_to_fp0(nv_point_t *nv_point)
{
	if (!is_fp0_full())
		insert_into_fpl(&fpls[0], nv_point, fpls[0].point_in_use);
}

// ------------------------------------------------------------------------------------------------

int print_cursored_fpl(fpl_t *__fpl,int index_in_fpl,int row_e)
{
	int edit_row = row_e;
    int ret = row_e;
	if(edit_row == 5)
	{
	    edit_row = 0;
		index_in_fpl = index_in_fpl-4;
		ret = row_e-1;
	}

	for(int i=index_in_fpl;i<__fpl->point_in_use;i++)
	{
	   if(edit_row == 5)
	   {
          fpl_print_pt(__fpl->point_in_use-1,__fpl->point_in_use,__fpl,edit_row);
		  break;
	   }
	   fpl_print_pt(i,i+1,__fpl,edit_row);
	   edit_row++;
	}
	return(ret);
}
BOOL is_null_before(fpl_t *__fpl,int index)
{
   for(int i=0;i<=index;i++)
	   if(!__fpl->points[0].buffer_spec) return(TRUE);
   return(FALSE);
}
void check_fpl_for_null(fpl_t *__fpl)
{
	if(!__fpl->points[__fpl->point_in_use-1].buffer_spec && __fpl->point_in_use>1)
		__fpl->point_in_use--;
	for(int i=0;i<__fpl->point_in_use;i++)
	{
		if(!__fpl->points[i].buffer_spec)
		{
			if(__fpl->points[i].type!=NAVAID_SID_MAIN && __fpl->points[i].type!=NAVAID_STAR_MAIN)
			{
			   delete_from_fpl(__fpl,i);
			   break;
			}
		}
	}
}
void fpl_change_attr(int attr)
{
	if(K_get_char(row_e,3)==':' || K_get_char(row_e,3)=='.')
       K_change_attribute(row_e,COL_START_EDIT,5,attr);
    else
       K_change_attribute(row_e,1,10,attr);
}

void add_point(fpl_t *fpl,nv_point_t *nv_point,int index)
{
	fpl->points[index].type = nv_point->type;
	fpl->points[index].buffer_spec = nv_point->buffer_spec;
	fpl_change_attr(ATTRIBUTE_NORMAL);
	do_fpl_pages(INPUT_LOUTERPLUS);
	print_fpl_header(&fpls[fpl_number]);
    update_screen();
	Save_FPL(&fpls[fpl_number],fpl_number);
}
//=====================================================================================
BOOL print_alligned_fpl(fpl_t *__fpl)
{
   __fpl->point_in_use--;
   if(row_e==5 && __fpl->point_in_use>5)
   {
      index_in_fpl--;
	  scrool_list(__fpl,index_in_fpl-5,6);
      fpl_change_attr(ATTRIBUTE_INVERSE);
      update_screen();
	  return(TRUE);
    }
    if(row_e==5 && __fpl->point_in_use==5)
	{
	   index_in_fpl--;
	   print_fpl(__fpl,fpl_number," CRSR",ATTRIBUTE_INVERSE);
	   //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
	   fpl_change_attr(ATTRIBUTE_INVERSE);
	   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
	   update_screen();
	   return(TRUE);
    }
    print_str(row_e,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	return(FALSE);
}
//=====================================================================================
void activate_fpl0(fpl_t *__fpl,BOOL inverse)
{
   //====== Activate Active FPL ======================
   if(inverse)
   {
	   for(int i=0;i<__fpl->point_in_use;i++)
	   {
		   fpls[0].points[i].buffer_spec = __fpl->points[__fpl->point_in_use-i-1].buffer_spec;
		   fpls[0].points[i].type = __fpl->points[__fpl->point_in_use-i-1].type;
	   }
	   fpls[0].point_in_use = __fpl->point_in_use;
   }
   else
      copy_fpl(&fpls[0],__fpl);
   //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
   do_status_line(DISABLE_ENT,NULL);
   //=================================================
   fpl_number=-1;
   if(cursored)
      do_fpl_pages(INPUT_LCURSOR);
   Save_FPL(&fpls[0],0);

   if(mod_get_mode()!=OBS_MODE)
   {
	   set_route_leg(NULL,NULL,DTO_LEG);
	   fpl_set_navigation();
   }
}
//=====================================================================================
void delete_fpl_point(fpl_t *__fpl,int point_index)
{
   delete_from_fpl(__fpl,point_index);
   check_fpl_for_null(__fpl);
   
   if(!__fpl->point_in_use)
   {
	   __fpl->points[0].buffer_spec=NULL;
	   __fpl->point_in_use=1;
   }

   if(point_index >= __fpl->point_in_use)
         index_in_fpl = __fpl->point_in_use-1;

   if(__fpl->point_in_use<6)
   {
	  print_fpl(&fpls[fpl_number],fpl_number," CRSR",ATTRIBUTE_INVERSE);
	  K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
	  if(row_e > __fpl->point_in_use)
	     row_e = __fpl->point_in_use;
	  if(row_e == index_in_fpl) row_e=index_in_fpl + 1;
	  //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
	  fpl_change_attr(ATTRIBUTE_INVERSE);
   }
   else
   {
	  int ind;

	  if(index_in_fpl == row_e)
	     ind=index_in_fpl-row_e;
	  else if(row_e == 5)
	    ind=index_in_fpl-6;
	  else
	  {
	     ind=index_in_fpl-row_e-1;
		 index_in_fpl--;
	  }

	  scrool_list2(&fpls[fpl_number],ind,6);
      //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);  
	  fpl_change_attr(ATTRIBUTE_INVERSE); 
   }
   Save_FPL(&fpls[fpl_number],fpl_number);
}
nv_point_t *fpl_get_dtofpl(void)
{
	if(!fpl_number && cursored && !editing)
		return(&fpls[0].points[index_in_fpl]);
	return(NULL);
}
int get_dt_point(int index)
{
   int nav_mod = nav_mode();
   if(nav_mod != FPL_LEG && nav_mod != FPLDTO_LEG)
	   return(POINT_NONE);
   int leg_n = nav_mod==FPL_LEG?calc_active_leg():leg_fpldto_number;
   if(leg_n == index)
	   return(AFPL_POINT);
   else if(leg_n < index)
	   return(WILL_POINT);
   return(DONE_POINT);  
}
nv_point_t *get_RowPointType(int row,int *point_type)
{
    if(!fpl_number)
	{
	   int start_number,start_row;
	   calc_fpl0_indexes(&start_number,&start_row);

	   if(!cursored)
	   {
		   int ind = row-start_row;
		   if(row==5 && fpls[fpl_number].point_in_use > 4)
		   {
			  if(!start_row && fpls[fpl_number].point_in_use<6)
				  return(NULL);
			  *point_type = get_dt_point(fpls[fpl_number].point_in_use-1);
			  return((nv_point_t*)&fpls[fpl_number].points[fpls[fpl_number].point_in_use-1]);
		   }
		   if(start_number+ind <= fpls[fpl_number].point_in_use)
		   {
              *point_type = get_dt_point(start_number+ind-1);
			  return((nv_point_t*)&fpls[fpl_number].points[start_number+ind-1]);
		   }
		   else
			   return(NULL);
	   }
	   else
	   {
	      if(editing)
		     if(row>=row_e)
				 return(NULL);
		  int d_row = row_e-row;
		  int ind = index_in_fpl - d_row;
		  if( !(K_get_char(row,0)=='D' || (K_get_char(row,2)>='0' && K_get_char(row,2)<='9')) )
			  return(NULL);
		  if( !fpls[fpl_number].points[ind].buffer_spec )
			  return(NULL);
		  if(row==5 && fpls[fpl_number].point_in_use > 4)
		  {
		     if(!fpls[fpl_number].points[fpls[fpl_number].point_in_use-1].buffer_spec)
			    return(NULL);
			 if(!start_row && fpls[fpl_number].point_in_use<6)
			    return(NULL);
			 *point_type = get_dt_point(fpls[fpl_number].point_in_use-1);
			 return((nv_point_t*)&fpls[fpl_number].points[fpls[fpl_number].point_in_use-1]);
		   }
 		   *point_type = get_dt_point(ind);
		   return((nv_point_t*)&fpls[fpl_number].points[ind]);
	   }                 
	}
	else
	{
	   if(!cursored)
	   {
		   if(row>0 && row<5)
		   {
		      if(row <= fpls[fpl_number].point_in_use)
			  {
			     *point_type = FPL_POINT;
				 return((nv_point_t*)&fpls[fpl_number].points[row-1]);
			  }
			  else
			     return(NULL);
		   }
		   else if(row==5 && fpls[fpl_number].point_in_use > 4)
		   {
		      *point_type = FPL_POINT;
			  return((nv_point_t*)&fpls[fpl_number].points[fpls[fpl_number].point_in_use-1]);
		   }
		   else
		   {
			   return(NULL);
		   }
	   }
	   else
	   {
	      if(editing)
		     if(row>=row_e)
			    return(NULL);
		  int d_row = row_e-row;
		  int ind = index_in_fpl - d_row;
		  if( !(K_get_char(row,0)=='D' || (K_get_char(row,2)>='0' && K_get_char(row,2)<='9')) )
			  return(NULL);
		  if( !fpls[fpl_number].points[ind].buffer_spec )
			  return(NULL);
		  if(row==5 && fpls[fpl_number].point_in_use > 4)
		  {
			  if(!fpls[fpl_number].points[fpls[fpl_number].point_in_use-1].buffer_spec)
				  return(NULL);
			  *point_type = FPL_POINT;
			  return((nv_point_t*)&fpls[fpl_number].points[fpls[fpl_number].point_in_use-1]);
		   }
		  *point_type = FPL_POINT;
		  return((nv_point_t*)&fpls[fpl_number].points[ind]);
	   }
	}
	return(NULL);
}

// -------------------------------------------------------------------------------------------------
// Name				fpl_insert_sid_points()
// Description		Inserts the SID points to the active (0) flight plan.
// Parameters		-
// Return			-		
// Depends on
//		fp_sid_points
// -------------------------------------------------------------------------------------------------

bool fpl_insert_sid_points(nvdb_apt *current_apt, char* ss_name )
{
	// Check if exceeding max FP points limit. Must calculate with procedure title as well
	if ((fpls[0].point_in_use + current_apt->sids->size() + 1) >= MAX_FP_POINTS)
	{
		do_status_line(ADD_STATUS_MESSAGE, " FPL  FULL ");
		return false;
	}

	// Insert SID header
	insert_into_fpl(&fpls[0], NULL, 1);
	fpls[0].points[1].type = NAVAID_SID_MAIN;
	sprintf(fpls[0].points[1].alt_name, "%.6s-SID", ss_name);

	// Insert SID points one by one.
	for (int i = 0; i < current_apt->sids->size(); i++)
	{
		nv_point_t nv_pt;
		memcpy(fp_sid_points + i, get_sid_point(i), sizeof(nv_hdr) );
		nv_pt.buffer_spec = fp_sid_points + i;
		nv_pt.type = NAVAID_SS_POINT;
		insert_into_fpl(&fpls[0], &nv_pt, 2 + i);
	}

	// Check if the next FP point is not the same as the last SID point
	int sid_last_point = current_apt->sids->size() + 1;
	if (strcmp((char*)(fpls[0].points[sid_last_point].buffer_spec), (char*)(fpls[0].points[sid_last_point + 1].buffer_spec)) == 0)
	{
		delete_from_fpl(&fpls[0], sid_last_point + 1);		// Delete duplicate non SID point.
	}

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name				fpl_insert_star_points()
// Description		Inserts the STAR points to the active (0) flight plan.
// Parameters		-
// Return			-		
// Depends on
//		fp_star_points
// -------------------------------------------------------------------------------------------------

bool fpl_insert_star_points(nvdb_apt *current_apt, char* ss_name)
{
	char *fp_dest_ap;		// FP destination AP ICAO
	bool dest_from_end;

	// Check if exceeding max FP points limit. Must calculate with procedure title as well
	if ((fpls[0].point_in_use + current_apt->stars->size() + 1) >= MAX_FP_POINTS)
	{
		do_status_line(ADD_STATUS_MESSAGE, " FPL  FULL ");
		return false;
	}

	// Check if STAR airport is the last point in the FP or not. If yes, we insert the STAR 
	// before that, otherwise we append the STAR at the end.
	fp_dest_ap = ((nv_hdr_t*)(fpls[0].points[fpls[0].point_in_use - 1].buffer_spec))->ICAO_ID;
	dest_from_end = (strcmp(fp_dest_ap, current_apt->ICAO_ID) == 0) ? 1 : 0;
	
	// Insert STAR header
	int star_hdr_index = fpls[0].point_in_use - dest_from_end;		// insert before AP point if exists
	insert_into_fpl(&fpls[0], NULL, star_hdr_index);
	fpls[0].points[star_hdr_index].type = NAVAID_STAR_MAIN;
	sprintf(fpls[0].points[star_hdr_index].alt_name, "%.5s-STAR", ss_name);

	// Insert STAR points one by one.
	for (int i = 0; i < current_apt->stars->size(); i++)
	{
		nv_point_t nv_pt;
		memcpy(fp_star_points + i, get_star_point(i), sizeof(nv_hdr));
		nv_pt.buffer_spec = fp_star_points + i;
		nv_pt.type = NAVAID_SS_POINT;
		insert_into_fpl(&fpls[0], &nv_pt, fpls[0].point_in_use - dest_from_end);
	}

	// Check if the FP point preceding the procedure is not the same as the first STAR point
	// Be careful as STAR starts with the header. We must compare the points before and after that.
	if (strcmp((char*)(fpls[0].points[star_hdr_index - 1].buffer_spec), (char*)(fpls[0].points[star_hdr_index + 1].buffer_spec)) == 0)
	{
		delete_from_fpl(&fpls[0], star_hdr_index - 1);		// Delete duplicate non STAR point.
	}

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name				fpl_delete_sids_points()
// Description		Deletes all the SID points from the active (0) flight plan.
//					Takes into consideration that non SID points might had been inserted among the
//					SID points.
// Parameters		-
// Return			-		
// Depends on
//		delete_fpl_point()
// -------------------------------------------------------------------------------------------------

void fpl_delete_sids_points(void)
{
	int sid_main_index = -1;

	// Checks if we find a SID header at all.
	for ( int i = 0; i < fpls[0].point_in_use; i++ )
	{
		if( fpls[0].points[i].type == NAVAID_SID_MAIN )
		{
			sid_main_index = i;
			break;
		}
	}
	if ( sid_main_index == -1 )		// No SID in FP. 
		return;						// Terminate the process.
 
	K_DEBUG("[HG] Deleting existing SID %s in fpl_delete_sids_points()\n", fpls[0].points[sid_main_index].alt_name);

	// Go through the FP from the determined pint and deletes all SID points. 
	// When deleting a point, we don't have to step the index as the unchanged 
	// index will point to the next item already.
	do
	{
		switch ( fpls[0].points[sid_main_index].type )
		{
		case NAVAID_STAR_MAIN:								// Oops, a STAR header. 
			return;											// Terminate the process.
		case NAVAID_SID_MAIN:								// SID header. 
		case NAVAID_SS_POINT:								// Procedure point. 
			delete_fpl_point(&fpls[0], sid_main_index);		// Delete it.
			break;
		default:											// Non SID point. 
			sid_main_index++;								// Skip it.
			break;
		}
	} while ( sid_main_index < fpls[0].point_in_use );		// Loop until the end of the FP
}

// -------------------------------------------------------------------------------------------------
// Name				fpl_delete_stars_points()
// Description		Deletes all the STAR points from the active (0) flight plan.
//					Takes into consideration that non STAR points might had been inserted among the
//					STAR points.
// Parameters		-
// Return			-		
// Depends on
//		delete_fpl_point()
// -------------------------------------------------------------------------------------------------

void fpl_delete_stars_points(void)
{
	int star_main_index = -1;

	// Checks if we find a STAR header at all.
	for ( int i = 0; i < fpls[0].point_in_use; i++ )
	{
		if( fpls[0].points[i].type == NAVAID_STAR_MAIN)
		{
			star_main_index = i;
			break;
		}
	}
	if( star_main_index == -1 )				// No STAR in FP.
		return;								// Terminate the process.

	K_DEBUG("[HG] Deleting existing STAR %s in fpl_delete_stars_points()\n", fpls[0].points[star_main_index].alt_name);

	// Go through the FP fr	om the determined point and deletes all STAR points. 
	// When deleting a point, we don't have to step the index as the unchanged index 
	// will point to the next item already.
	do
	{
		switch ( fpls[0].points[star_main_index].type )
		{
		case NAVAID_STAR_MAIN:								// STAR header. 
		case NAVAID_SS_POINT:								// Procedure point. 
			delete_fpl_point(&fpls[0], star_main_index);	// Delete it.
			break;
		default:											// Non SID point. 
			star_main_index++;								// Skip it.
			break;
		}
	} while (star_main_index < fpls[0].point_in_use);		// Loop until the end of the FP
}

// -------------------------------------------------------------------------------------------------

int do_fpl_pages(int ACTION)
{
	if(ACTION == ACTION_TIMER)
	{
		if(!cursored)
		{
		   print_fpl(&fpls[fpl_number],fpl_number,NULL,0);
		}
		else if(cursored && !fpl_number) 
		{
		   if(on_delete)
		      print_fpl(&fpls[fpl_number],fpl_number," CRSR",ATTRIBUTE_INVERSE);
		   else
		      print_fpl_arrows(index_in_fpl,row_e);
		}
		update_screen();
		return(0);
	}
	//==========================================================================
	if(ACTION == INPUT_LCURSOR)
	{
	    //if(!fpl_number) return(0);
		//if(on_delete) return(0);
		
		if(!cursored)
		{
		    cursored = 1;
            index_in_fpl = 0;
            row_e = 1;

			if(!fpl_number)
			{
				calc_fpl0_indexes(&index_in_fpl,&row_e);
				index_in_fpl--;
				if(!fpls[fpl_number].points[index_in_fpl].buffer_spec)
				{
					if(fpls[fpl_number].points[index_in_fpl].type == NAVAID_SID_MAIN || fpls[fpl_number].points[index_in_fpl].type == NAVAID_STAR_MAIN)
					   index_in_fpl--;
				}

            }
			
            col_e = COL_START_EDIT;

			print_str(6,0,ATTRIBUTE_NORMAL," CRSR");
			K_change_attribute(6,1,4,ATTRIBUTE_INVERSE);

			if(is_useable_fpl(&fpls[fpl_number]) && fpl_number)
			{
				on_use=1;
				K_change_attribute(0,0,4,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE);
				//print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"ENT");
				do_status_line(ENABLE_ENT,NULL);
			}
			else
			{
			   if(!fpl_number && fpls[0].point_in_use < 6 )
			      print_fpl(&fpls[0],fpl_number," CRSR",ATTRIBUTE_INVERSE);

			   //K_change_attribute(row_e,col_e,5,ATTRIBUTE_INVERSE);
			   fpl_change_attr(ATTRIBUTE_INVERSE);
			}
			FPL_INPUT_MASK |= (INPUT_LOUTERMINUS|INPUT_LOUTERPLUS);
			cursored=1;
			update_screen();
		}
		else
		{
	        //if(!editing && !pre_approve)
			//{
			   cursored=on_use=on_load_0=on_use_invert=on_delete_point=on_delete=editing=pre_approve=0;
//			   print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			   do_status_line(DISABLE_ENT,NULL);
			   check_fpl_for_null(&fpls[fpl_number]);
			   update_screen();
			   FPL_INPUT_MASK &= ~(INPUT_LOUTERMINUS|INPUT_LOUTERPLUS);
			   if(fpl_number==-1)
			   {
				   fpl_number=0;
				   //fpl_set_navigation();
			   }
			//}
		}
	}
	//===========================================================================
	if(ACTION == INPUT_LINNERPLUS || ACTION == INPUT_LINNERMINUS)
	{
       if(on_delete_point)
		   return(0);
	   if(!cursored)
	   {
          if(ACTION == INPUT_LINNERPLUS)
		  {
			 fpl_number++;
			 if (fpl_number >= MAX_FLIGHTPLANS) fpl_number = 0;
		  }
		  if(ACTION == INPUT_LINNERMINUS)
		  {
             fpl_number--;
			 if (fpl_number<0) fpl_number = MAX_FLIGHTPLANS - 1;
		  }
	   }
	   else
	   {
//          if(!fpl_number)
//		     return(0);

		  if(index_in_fpl>=0 && !on_use && !on_use_invert && !on_load_0 )
		  {
		     if(!editing)
			 {
				 if (fpls[fpl_number].point_in_use >= MAX_FP_POINTS)
					 do_status_line(ADD_STATUS_MESSAGE," FPL  FULL ");
				 if (fpls[fpl_number].point_in_use >= MAX_FP_POINTS && fpls[fpl_number].points[MAX_FP_POINTS - 1].buffer_spec)
				    return(0);
				 if (fpls[fpl_number].point_in_use < MAX_FP_POINTS)
				 {
				    insert_into_fpl(&fpls[fpl_number],NULL,index_in_fpl);
				    row_e = print_cursored_fpl(&fpls[fpl_number],index_in_fpl,row_e);
				 }
				 col_e = COL_START_EDIT;
				 if(!fpl_number) clear_fpl_arrows();
			 }
			 char curr_char = K_get_char(row_e,col_e);
			 
			 if(editing || char_type(curr_char) != CHAR_ALPHA_NUM)
			 curr_char = ACTION == INPUT_LINNERPLUS ? K_next_char(curr_char) : K_prev_char(curr_char);
		     
			 K_set_char(row_e,col_e,curr_char);
		     K_change_attribute(row_e,col_e,1,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE);
			 editing=1;
			 //======================================================
			 char __ICAO_ID[6];
			 K_get_string(row_e,COL_START_EDIT,col_e,__ICAO_ID);
		     //======================================================
			 ids_list = find_ids_by_icao(__ICAO_ID,&ids_list_count);
			 if(ids_list)
			 {
			     get_icao_by_id(ids_list[0],__ICAO_ID);
				 print_str(row_e,COL_START_EDIT,ATTRIBUTE_NORMAL,"%-7s",__ICAO_ID);
				 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				 fpl_change_attr(ATTRIBUTE_INVERSE);
				 K_change_attribute(row_e,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
				 //print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"ENT");
				 do_status_line(ENABLE_ENT,NULL);
			 }
			 else
			 {
				 print_str(row_e,COL_START_EDIT,ATTRIBUTE_NORMAL,"%-7s",__ICAO_ID);
				 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				 fpl_change_attr(ATTRIBUTE_INVERSE);
				 K_change_attribute(row_e,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
				 //print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"   ");
				 do_status_line(ENABLE_ENT,NULL);
			 }
			 update_screen();
		  }
	   }
	}
    //=====================================================================================

	if(ACTION == INPUT_LOUTERPLUS)
	{
       if(on_delete_point)
		   return(0);
	   if(editing && !pre_approve)
	   {
	      if( (K_get_char(row_e,col_e)!=BLANK_CHAR) && (col_e<COL_END_EDIT) )
		  {
		     K_change_attribute(row_e,col_e,1,ATTRIBUTE_INVERSE);
		     col_e++;
		     K_change_attribute(row_e,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
		  }
	   }
	   if(cursored && !editing && !pre_approve )
	   {
		   if(on_use)
		   {
		      K_change_attribute(0,0,11,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
			  on_use=0;
			  on_use_invert=1;
		   }
	       else if(on_use_invert)
		   {
			   index_in_fpl=0;
			   on_use_invert=0;
			   if(fpls[fpl_number].point_in_use>=6)
			   {
                  row_e=0;
				  scrool_list2(&fpls[fpl_number],0,6);
				  print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
			   }
			   else
			   {
			      row_e=1;
				  print_fpl(&fpls[fpl_number],fpl_number," CRSR",ATTRIBUTE_INVERSE);
				  K_change_attribute(0,0,11,ATTRIBUTE_NORMAL);
			   }
			   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
			   K_change_attribute(row_e,4,5,ATTRIBUTE_INVERSE);
//			   print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			   do_status_line(DISABLE_ENT,NULL);
			   update_screen();
			   
		   }
		   else if(on_load_0)
		   {
		       print_fpl(&fpls[fpl_number],fpl_number," CRSR",ATTRIBUTE_INVERSE);
			   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
			   K_change_attribute(0,0,11,ATTRIBUTE_NORMAL);
			   K_change_attribute(1,4,5,ATTRIBUTE_INVERSE);

//			   print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			   do_status_line(DISABLE_ENT,NULL);
			   update_screen();
			   on_load_0=0;
			   index_in_fpl=0;
			   row_e=1;
		   }
           else if( index_in_fpl < fpls[fpl_number].point_in_use )
           {
              if(!fpls[fpl_number].points[index_in_fpl].buffer_spec 
				  ||
				  index_in_fpl == (MAX_FP_POINTS - 1)
				 )
			  {
			     if(fpls[fpl_number].points[index_in_fpl].type!=NAVAID_SID_MAIN && fpls[fpl_number].points[index_in_fpl].type!=NAVAID_STAR_MAIN)
				 {
				    //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				    fpl_change_attr(ATTRIBUTE_INVERSE);
			        return(0);
				 }
			  }
				  
			  if(row_e<4)
              {  
                 index_in_fpl++;
				 if(index_in_fpl == fpls[fpl_number].point_in_use)
				 {
					 fpls[fpl_number].points[index_in_fpl].buffer_spec = NULL;
					 fpls[fpl_number].point_in_use++;
				 }
                 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_NORMAL);
				 fpl_change_attr(ATTRIBUTE_NORMAL);
                 row_e++;
				 fpl_print_pt(index_in_fpl,index_in_fpl+1,&fpls[fpl_number],row_e);
                 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				 fpl_change_attr(ATTRIBUTE_INVERSE);
              }
              else if(index_in_fpl == (fpls[fpl_number].point_in_use-2))
              {
                 index_in_fpl++;
                 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_NORMAL);
				 fpl_change_attr(ATTRIBUTE_NORMAL);
                 row_e++;
                 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				 fpl_change_attr(ATTRIBUTE_INVERSE);				 
              }
              else if(index_in_fpl == (fpls[fpl_number].point_in_use-1))
              {
				 index_in_fpl++;
				 fpls[fpl_number].points[index_in_fpl].buffer_spec = NULL;
				 fpls[fpl_number].point_in_use++;

                 if(row_e==4)
				 {
				    K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_NORMAL);
					row_e++;
					print_str(row_e,0,ATTRIBUTE_NORMAL,"%3d:%-7s",index_in_fpl+1," ");
				 }
				 else
			        scrool_list(&fpls[fpl_number],index_in_fpl-5,6);

				 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				 fpl_change_attr(ATTRIBUTE_INVERSE);
              }
              else
              {
                 index_in_fpl++;
                 scrool_list(&fpls[fpl_number],index_in_fpl-4,5);
                 //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				 fpl_change_attr(ATTRIBUTE_INVERSE);
              }
              update_screen();
		   } 
	   }
	}
	if(ACTION == INPUT_LOUTERMINUS)
	{
       if(on_delete_point)
		   return(0);

	   if(editing && !pre_approve)
	   {
	      if(col_e > COL_START_EDIT)
	      {
	         K_change_attribute(row_e,col_e,1,ATTRIBUTE_INVERSE);
	         col_e--;
		     K_change_attribute(row_e,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
		  }
	   }
	   //=============== Move Cursor =============================================
	   if(cursored && !editing && !pre_approve && !on_use && !on_use_invert && !on_load_0)
	   {
	       if(index_in_fpl == 0 && fpl_number)
		   {
		       row_e=1;
			   print_fpl(&fpls[fpl_number],fpl_number," CRSR",ATTRIBUTE_INVERSE);
			   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
			   if(!is_useable_fpl(&fpls[fpl_number]))
			   {
			      on_load_0=1;
				  K_change_attribute(0,0,11,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
			   }
			   else
			   {
			      on_use=1;
                  K_change_attribute(0,0,4,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
			   }
			   //print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"ENT");
			   do_status_line(ENABLE_ENT,NULL);
			   update_screen();
		   }
		   else if(on_use_invert)
		   {
		      on_use_invert=0;
			  K_change_attribute(0,0,11,ATTRIBUTE_NORMAL);
			  K_change_attribute(0,0,4,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
			  on_use=1;
		   }
		   else if(index_in_fpl>0)
		   {
			   if(!fpls[fpl_number].points[index_in_fpl].buffer_spec)
			   {
				   if(fpls[fpl_number].points[index_in_fpl].type!=NAVAID_SID_MAIN && fpls[fpl_number].points[index_in_fpl].type!=NAVAID_STAR_MAIN)
				   {
				      if(print_alligned_fpl(&fpls[fpl_number]))
					     return(0);
				   }
			   }
			   index_in_fpl--;
			   if(row_e>0)
			   {
				   //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_NORMAL);
				   fpl_change_attr(ATTRIBUTE_NORMAL);
				   row_e--;
				   //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				   fpl_change_attr(ATTRIBUTE_INVERSE);
			   }
			   else
			   {
			       scrool_list(&fpls[fpl_number],index_in_fpl,5);
				   //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
				   fpl_change_attr(ATTRIBUTE_INVERSE);
      		   }
               update_screen();
		   }
	   }
	}
	//============================================================================
	if(ACTION == ACTION_SUP_PT_CREATED)
	{
		if(editing)
		{
			active_point.buffer_spec = get_current_sup();
			active_point.type = NAVAID_SUP;
			do_status_line(DISABLE_ENT,NULL);
			pre_approve=editing=0;
			add_point(&fpls[fpl_number],&active_point,index_in_fpl);
			unblock_all();
		}
	}
	if(ACTION == INPUT_ENTER)
	{
        if(on_delete_point)
		{
		   on_delete_point=0;
           if(fpls[fpl_number].points[index_in_fpl].type==NAVAID_SID_MAIN)
	          fpl_delete_sids_points();
		   else if(fpls[fpl_number].points[index_in_fpl].type==NAVAID_STAR_MAIN)
		      fpl_delete_stars_points();
		   else 
              delete_fpl_point(&fpls[fpl_number],index_in_fpl);
		   //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
		   do_status_line(DISABLE_ENT,NULL);
		   update_screen();
		   return(0);
		}
		else if(cursored && on_delete)
		{
		   if(nav_mode() == FPLDTO_LEG && !fpl_number)
	          dto_change_mode(DTO_MODE_NORMAL);
		   fpls[fpl_number].point_in_use=1;
		   fpls[fpl_number].points[0].buffer_spec = NULL;
		   Save_FPL(&fpls[fpl_number],fpl_number);
		   on_delete=0;
		   cursored=0;
//		   print_str(6,14,ATTRIBUTE_NORMAL,"   ");
		   do_status_line(DISABLE_ENT,NULL);
		   update_screen();
		}
		else if(editing)
		{
		   if(pre_approve)
		   {
		      sr_screen(SCREEN_RESTORE);
//			  print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			  do_status_line(DISABLE_ENT,NULL);
			  pre_approve=editing=0;
			  add_point(&fpls[fpl_number],&active_point,index_in_fpl);
			  unblock_all();
		   }
		   else
		   {
		      if(ids_list)
			  {
			     if(ids_list_count>1)
				 {
				    update_screen();
					build_ml_list();
					set_ml_mode(FPL_PAGE);
				 }
				 else
				 {
				    show_active_point(ids_list[0]);
					//K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
					fpl_change_attr(ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
		            //print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"ENT");
					do_status_line(ENABLE_ENT,NULL);
		            update_screen();
		            pre_approve = 1;
					block_all(FPL_PAGE);
				 }
			  }
			  else
			  {
				  char __ICAO_ID[6];
				  K_get_string(row_e,COL_START_EDIT,col_e,__ICAO_ID);

				  if(!sup_is_creating()) 
				  {
					  switch_rp(SUP_PAGE,0); 
					  sup_create_new_point(__ICAO_ID);
					  return(-1);
				  }		  
			  }
		   }
		}
		else if(on_load_0)
		{
		  //====== Load FPL 0 ==============================
		  copy_fpl(&fpls[fpl_number],&fpls[0]);
		  Save_FPL(&fpls[fpl_number],fpl_number);
		  //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
		  do_status_line(DISABLE_ENT,NULL);
		  //==========================================
		  do_fpl_pages(INPUT_LCURSOR);
		  //==========================================
		}
		else if(on_use)
           activate_fpl0(&fpls[fpl_number],FALSE);
		else if(on_use_invert)
		   activate_fpl0(&fpls[fpl_number],TRUE);
		else if (fpl_number == 0 && !cursored) {
			set_fpl0_from_af();
		}
		else
		{
		   if(!cursored) return(0);
		   nv_point_t __nvpt;
		   if(K_get_rp_point(&__nvpt)==FALSE)
			   return(0);
		   if (fpls[fpl_number].point_in_use < MAX_FP_POINTS)
		   {
		      insert_into_fpl(&fpls[fpl_number],&__nvpt,index_in_fpl);
			  row_e = print_cursored_fpl(&fpls[fpl_number],index_in_fpl,row_e);
			  //add_point(&fpls[fpl_number],&__nvpt,index_in_fpl); // HG: Isn't this duplicate action? insert_into_fpl() did it all.		  
		   }
		   else
		      do_status_line(ADD_STATUS_MESSAGE," FPL  FULL ");
		}
	}
	//=============================================================================
	if(ACTION == MULTI_LINE_SELECTED)
	{
	   if(current_item == -1)
	   {
	      sr_screen(SCREEN_RESTORE);
		  do_fpl_pages(INPUT_CLR);	   
		  unblock_all();
	   }
	   else
	   {
	      show_active_point(ids_list[current_item]);
	      //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
		  fpl_change_attr(ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
	      do_status_line(ENABLE_ENT,NULL);
	      update_screen();
	      pre_approve = 1;
	      block_all(FPL_PAGE);
	   }
	}
	//==============================================================================
	if(ACTION == ACTION_FREE_RESOURCES)
	{
       fpl_number=index_in_fpl=cursored=editing=row_e=col_e=pre_approve=on_load_0=on_use=leg_number=
	   on_delete=on_use_invert=on_delete_point=0;
       FPL_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_ENTER|INPUT_CLR;
	   for (int ii = 0; ii < MAX_FLIGHTPLANS; ii++ )
		   fpls[ii].point_in_use=0;
    }
	if(ACTION == ACTION_INIT)
	{
		Load_FPLS();
		return(0);
    }

	if(ACTION == INPUT_CLR)
	{
		if(cursored && !editing && !on_delete)
		{
  		   if(!fpls[fpl_number].points[index_in_fpl].buffer_spec)
		   {
		      if(fpls[fpl_number].points[index_in_fpl].type!=NAVAID_SID_MAIN && fpls[fpl_number].points[index_in_fpl].type!=NAVAID_STAR_MAIN)
			     return(0);
		   }
		   if(on_use || on_use_invert || on_load_0)
			   return(0);
		   //if(!fpl_number)
		   //   return(0);
           if(on_delete_point)
		   {
		      on_delete_point=0;
			  fpl_print_pt(index_in_fpl,index_in_fpl+1,&fpls[fpl_number],row_e);
			  //K_change_attribute(row_e,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
			  fpl_change_attr(ATTRIBUTE_INVERSE);
		      //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			  do_status_line(DISABLE_ENT,NULL);
		      update_screen();
		   }
		   else
		   {
		      on_delete_point=1;
			  if(fpls[fpl_number].points[index_in_fpl].type!=NAVAID_SID_MAIN && fpls[fpl_number].points[index_in_fpl].type!=NAVAID_STAR_MAIN)
                 print_str(row_e,0,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"DEL %-5s?",((nv_hdr_t *)fpls[fpl_number].points[index_in_fpl].buffer_spec)->ICAO_ID);
			  else
			  {
			     char temp_del_name[MAX_PATH];
				 sprintf(temp_del_name,"%s",fpls[fpl_number].points[index_in_fpl].alt_name);
				 char *s_tire_ptr = strstr(temp_del_name,"-");
				 *s_tire_ptr='\0';
				 print_str(row_e,0,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"DEL %-6s?",temp_del_name);
			  }
		      //print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"ENT");
			  do_status_line(ENABLE_ENT,NULL);
		      update_screen();
		   }
		}
		else if(editing/* && pre_approve*/)
		{
		   if(pre_approve)
		   {
		      sr_screen(SCREEN_RESTORE);
		      unblock_all();
		   }
		   do_status_line(DISABLE_ENT,NULL);
		   pre_approve=editing=0;
		   delete_fpl_point(&fpls[fpl_number],index_in_fpl);  
		   //update_screen();
		}
		else if(!cursored /*&& fpl_number*/)
		{
		     print_str(0,0,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"DELETE FPL?");
			 print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
			 //print_str(6,14,ATTRIBUTE_FLASH,"ENT");
			 do_status_line(ENABLE_ENT,NULL);
			 K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
			 update_screen();
			 on_delete=1;
			 cursored=1;
		}
		else if(cursored && on_delete)
		{
		   cursored=0;
		   on_delete=0;
		   //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
		   do_status_line(DISABLE_ENT,NULL);
		   update_screen();
		}
	}
	return(0);    
}

void calculate_fpls(void)
{
   fpl_set_navigation();
   if( !(nav_mode() == FPL_LEG || nav_mode() == FPLDTO_LEG)  || !is_useable_fpl(&fpls[0]) )
	   return;
   int leg_n = nav_mode() == FPL_LEG ? calc_active_leg() : leg_fpldto_number;

   FLOAT64 Lat,Lon;
   get_PPOS(&Lat,&Lon);
   FLOAT64 BaseLen;

   if(!fpls[0].points[leg_n].buffer_spec)
      leg_n++;
   BaseLen = get_S(((nv_hdr_t *)fpls[0].points[leg_n].buffer_spec)->Lat,
	                       ((nv_hdr_t *)fpls[0].points[leg_n].buffer_spec)->Lon,
   						   Lat,Lon)/1.852f;
   for(int i=leg_n+1;i<fpls[0].point_in_use;i++)
   {
	   if(!fpls[0].points[i].buffer_spec) continue;
	   BaseLen += fpls[0].points[i].abs_dis;
	   fpls[0].points[i].dis_from_curr = BaseLen;
   }
}
nv_point_t *fpl_get_active(int *index)
{
	int nav_mod = nav_mode();
	if(nav_mod != FPL_LEG && nav_mod != FPLDTO_LEG)
		return(NULL);
	int leg_n = nav_mod==FPL_LEG?leg_number:leg_fpldto_number;
	if(index) *index = leg_n;
	return(&fpls[0].points[leg_n]);
}
nv_point_t *fpl_get_next(int *index)
{
	int nav_mod = nav_mode();
	if(nav_mod != FPL_LEG && nav_mod != FPLDTO_LEG)
		return(NULL);
	int leg_n = nav_mod==FPL_LEG?leg_number:leg_fpldto_number;
	leg_n++;
	if(leg_n>=fpls[0].point_in_use) return(NULL);
	if(index) *index = leg_n;
	return(&fpls[0].points[leg_n]);
}

// -------------------------------------------------------------------------------
// Name:		fpl_get_last
// Descrption:	Returns last point from the active flight plan FP0
// Parameters:
//		index	Returned index in the FP if index is not NULL
// Return:
//		Pointer to the flight plan point structure
// Comment:
//		!!! Might refer to an index outside of the array. Should be fine-tuned !!!
// -------------------------------------------------------------------------------

nv_point_t *fpl_get_last(int *index)
{
	int ind;

	if (fpls[0].point_in_use == 0)
		return NULL;

	ind = fpls[0].points[fpls[0].point_in_use - 1].buffer_spec ? fpls[0].point_in_use - 1 : fpls[0].point_in_use - 2;	// !!! possible indexing out of range !!!
	if(index) 
		*index = ind;
	return(&fpls[0].points[ind]);
}

// -------------------------------------------------------------------------------

double fpl_get_SS(int p1,int p2,double Lat,double Lon)
{
   double SS;
   double a;
   get_LBU2(
	      ((nv_hdr_t *)fpls[0].points[p1].buffer_spec)->Lat,
          ((nv_hdr_t *)fpls[0].points[p1].buffer_spec)->Lon,
          ((nv_hdr_t *)fpls[0].points[p2].buffer_spec)->Lat,
		  ((nv_hdr_t *)fpls[0].points[p2].buffer_spec)->Lon,
	      Lat,Lon,&a,&SS
		  );
   return(fabs(cos(a/57.295779)*SS));
}

int calc_next_leg(void)
{
    int act;
	if(!fpl_get_active(&act)) return(0);
	if(!act) return(0);
    int next = act+1;
	if(next >= fpls[0].point_in_use) return(0);
	if(!fpls[0].points[next].buffer_spec) return(0);
	return(next);
}
BOOL navaid_to(double pLat,double pLon,double dLat,double dLon)
{
   double _BRG   = get_ZPU(pLat,pLon,dLat,dLon,get_MAGVAR());
   return fabs(__add_deg180(_BRG,-get_TK2()))<=90.0f;
}
int calc_active_leg(void)
{
	double Lat, Lon, prox, prev_dtk, speed;
	int index;
	int found = 0;
	char str[32];
	nv_hdr_t *src_point = NULL;
	nv_hdr_t *dst_point = NULL;

	if (!is_useable_fpl(&fpls[0]))
	   return(0);

	// The original logic of this function is to figure out which of the destination waypoints in your FPL0 is
	// the closest one to your PPOS in the direction your airplane is pointing.  That waypoint is then set as the active one.
	// That is not how the KLN90B works.  It sets the currently active waypoint as the first one and it remains there
	// unless it is manually changed (by a Direct-to from FPL0 or from SUPER NAV 5) or it auto-sequences because you 
	// arrive at the destination of the waypoint.  In case the original logic is necessary, I am conditioning the new
	// logic on a user-settable switch.  If that switch is false it will fall through to the old logic.

	// Note that sone waypoints aren't "real", that is they don't have a point in space assigned to them. These will be skipped.
	// 'buffer_spec' is a pointer to a waypoint definition structure.  If it is false (NULL) then the point isn't "real".

	get_PPOS(&Lat, &Lon);

	if (Correct_Sequencing) {
		// Find the FPL sequence number (starting at 0) of the current active leg if we are in DTO
		if (Current_Mode > 1) {
			rt_leg_t *__rt = get_rt_leg();
			if (__rt) {
				if (__rt->dst_icao_id) {
					Current_Active_Leg = 0;
					for (index = 0; index < fpls[0].point_in_use; index++) {
						if (fpls[0].points[index].buffer_spec) {
							src_point = (nv_hdr_t *)fpls[0].points[index].buffer_spec;
							if (strcmp(__rt->dst_icao_id, src_point->ICAO_ID) == 0) {
								Current_Active_Leg = index - 1;		// This is where we are really going with the DTO.
								break;
							}
						}
					}
				}
			}
		}
		// If that leg doesn't have a point, return - we can't do anything.
		if (!fpls[0].points[Current_Active_Leg].buffer_spec) {
			return (0);			
		}
		// Found out next real leg with a point
		src_point = (nv_hdr_t *)fpls[0].points[Current_Active_Leg].buffer_spec;
		prev_dtk = fpls[0].points[Current_Active_Leg].dtk;
		for (index = Current_Active_Leg + 1; index < fpls[0].point_in_use; index++) {
			if (!fpls[0].points[index].buffer_spec) {
				continue;
			}
			else {
				dst_point = (nv_hdr_t *)fpls[0].points[index].buffer_spec;
				found = 1;
				break;
			}
		}
		if (found) {
			prox = get_S(Lat, Lon, dst_point->Lat, dst_point->Lon) / 1.852f;
			speed = get_GS(1.852f);
			Sequencing_Distance = speed / 100;				// how close to we have to be?  Speed determines.
			if (prox <= Sequencing_Distance) {				// Are we close enough to move to the next leg?
				Current_Active_Leg = index;				// Yes -- Set and return the next "real" one.
				if (strcmp(Sequenced, dst_point->ICAO_ID) != 0) {	// But haven't sequenced it yet?
					if (CompareHeadings(fpls[0].points[index].dtk, fpls[0].points[index + 1].dtk) > 5) {
						if (!interface_obsout && (index + 1) < fpls[0].point_in_use) {
							sprintf(str, "ADJ NAV IND CRS TO %03d%c", K_ftol(fpls[0].points[index+1].dtk), 0xB0);
							msg_add_message(str);
						}
					}
					strcpy(Sequenced, dst_point->ICAO_ID);	// Record so we only sequence once per WP
				}
				if (index + 1 < fpls[0].point_in_use) {		// Are we at the end?
					return (index + 1);						// no - next one
				}
				else {
					return (index);							// yes--stay there
				}
			}
		}
		if (Current_Active_Leg + 1 < fpls[0].point_in_use) {
			return (Current_Active_Leg + 1);				// Stay on our current leg.
		}
		else {
			return (Current_Active_Leg);
		}
	}

   // Original Logic Starts Here

	static int fpl0_real_indexes[1024];
	int index_count = 0;
	int near_to_pt = -1;
	double len_to_nrpt;

	int leg_number = -1;
	int nearest_leg_S;

	int nearest_pt = -1;
	double len_nearest;

	for (int i = 0; i<fpls[0].point_in_use; i++)
	{
		if (!fpls[0].points[i].buffer_spec) continue;
		fpl0_real_indexes[index_count++] = i;
	}

   for(int i=0;i<index_count;i++)
   {
	  nv_hdr_t *dst_pt = (nv_hdr_t *)fpls[0].points[fpl0_real_indexes[i]].buffer_spec;
      //Find out nearest point in FPL
	  if(nearest_pt==-1)
	  {
		  nearest_pt = fpl0_real_indexes[i];
		  len_nearest = get_S(Lat,Lon,dst_pt->Lat,dst_pt->Lon);  
	  }
	  else
	  {
		  double temp_S = get_S(Lat,Lon,dst_pt->Lat,dst_pt->Lon);
		  if(temp_S<len_nearest)
		  {
			  len_nearest = temp_S;
			  nearest_pt  = fpl0_real_indexes[i];
		  }  
	  }
	  nv_hdr_t *src_pt = NULL;
	  if(!navaid_to(Lat,Lon,dst_pt->Lat,dst_pt->Lon)) continue;
	  if(i>0) // Not first point
	     src_pt = (nv_hdr_t *)fpls[0].points[fpl0_real_indexes[i-1]].buffer_spec;
	  if(near_to_pt==-1)
	  {
	     near_to_pt = fpl0_real_indexes[i];
		 len_to_nrpt = get_S(Lat,Lon,dst_pt->Lat,dst_pt->Lon);
	  }
	  else
	  {
	     double temp_S = get_S(Lat,Lon,dst_pt->Lat,dst_pt->Lon);
		 if(temp_S<len_to_nrpt)
		 {
		    len_to_nrpt = temp_S;
            near_to_pt = fpl0_real_indexes[i];
		 }
	  }
	  if(src_pt)
	  {
	     if(!navaid_to(Lat,Lon,src_pt->Lat,src_pt->Lon))   //From point
		 {
            if(leg_number==-1)
			{
			   leg_number=fpl0_real_indexes[i];
			   nearest_leg_S=get_S(Lat,Lon,dst_pt->Lat,dst_pt->Lon);
			}
			else
			{
			   double temp_S = get_S(Lat,Lon,dst_pt->Lat,dst_pt->Lon);
			   if(temp_S<nearest_leg_S)
			   {
				   leg_number=fpl0_real_indexes[i];
				   nearest_leg_S=temp_S;
			   }
			}
 		 }
	  }
   }
   if(leg_number!=-1)
      return(leg_number);
   if(near_to_pt!=-1)
   {
	  if(!near_to_pt)
	     return fpl0_real_indexes[1];
	   return near_to_pt;
   }
   if(nearest_pt!=-1)
   {
	   if(!nearest_pt)
		   return fpl0_real_indexes[1];
	   return nearest_pt;
   }
   return(0);
/*
   double min_len=2000000;
   double curr_len;
   int min_pt;
   double SS;
   BOOL r1=FALSE;
   double SS1;
   BOOL r2=FALSE;
   double SS2;
   BOOL less1=FALSE,less2=FALSE;
   int last_index,first_index=-1;
   for(int i=0;i<fpls[0].point_in_use;i++)
   {
	   if(!fpls[0].points[i].buffer_spec) continue;
	   curr_len = get_S(Lat,Lon,((nv_hdr_t *)fpls[0].points[i].buffer_spec)->Lat,
		                        ((nv_hdr_t *)fpls[0].points[i].buffer_spec)->Lon);
	   if(first_index==-1)
	   first_index=i;

	   if(curr_len<=min_len)
	   {
		   min_pt = i;
		   min_len = curr_len;
	   }

	   last_index=i;
   }
   if(!min_pt)
   {
	   if( fpls[0].points[min_pt].buffer_spec == fpls[0].points[last_index].buffer_spec )
	   {
	      if(fabs(__add_deg180(get_TK2(),-get_ZPU(Lat,Lon,((nv_hdr_t *)fpls[0].points[0].buffer_spec)->Lat,
			              ((nv_hdr_t *)fpls[0].points[0].buffer_spec)->Lon,
						  get_MAGVAR())))<=90.0f)
						  return(last_index);
	   }
	   return(1);
   }
   if(min_pt==fpls[0].point_in_use-1)
   {
	   if( fpls[0].points[min_pt].buffer_spec == fpls[0].points[first_index].buffer_spec )
	   {
		   if(fabs(__add_deg180(get_TK2(),-get_ZPU(Lat,Lon,((nv_hdr_t *)fpls[0].points[min_pt].buffer_spec)->Lat,
			   ((nv_hdr_t *)fpls[0].points[min_pt].buffer_spec)->Lon,
			   get_MAGVAR())))>=90.0f)
			   return(1);
	   } 
	   return(min_pt);
   }
   if(min_pt-1>=0)
   {
       int ind = fpls[0].points[min_pt-1].buffer_spec ? min_pt-1 : min_pt-2 >=0 ? min_pt-2 : -1;
	   if(ind>=0)
	   {
	      SS = fpl_get_SS(min_pt,ind,Lat,Lon);
		  SS1 = SS;
		  r1=TRUE;
		  if(fabs(SS) < fpls[0].points[min_pt].abs_dis*1.852f)
             less1=TRUE;
		  //   return(min_pt);
	   }
   }
   if(min_pt+1<fpls[0].point_in_use)
   {
	   int ind = fpls[0].points[min_pt+1].buffer_spec ? min_pt+1 : min_pt+2 < fpls[0].point_in_use ? min_pt+2 : -1;
	   if(ind>=0)
	   {
		  if(!fpls[0].points[ind].buffer_spec) return(min_pt);

		  //SS = fpl_get_SS(ind,min_pt,Lat,Lon);
		  SS = fpl_get_SS(min_pt,ind,Lat,Lon);
		  SS2 = SS;
		  r2=TRUE;
	      //if(fabs(SS) < fpls[0].points[min_pt+1].abs_dis*1.852f)
		  if(fabs(SS) < fpls[0].points[ind].abs_dis*1.852f)
             less2=TRUE;
		  //   return(min_pt+1);        
	   }
	   else
	      return(min_pt);
   }
   if(less1 && less2)
   {
       int ind1 = fpls[0].points[min_pt-1].buffer_spec ? min_pt-1 : min_pt-2 >=0 ? min_pt-2 : -1;
	   int ind2 = fpls[0].points[min_pt+1].buffer_spec ? min_pt+1 : min_pt+2 < fpls[0].point_in_use ? min_pt+2 : -1; 
	   if(ind1 >=0 && ind2 >=0)
	   {
		   double LBU1 = get_LBU(
			   ((nv_hdr_t *)fpls[0].points[ind1].buffer_spec)->Lat,
			   ((nv_hdr_t *)fpls[0].points[ind1].buffer_spec)->Lon,
			   ((nv_hdr_t *)fpls[0].points[min_pt].buffer_spec)->Lat,
			   ((nv_hdr_t *)fpls[0].points[min_pt].buffer_spec)->Lon,Lat,Lon,NULL,NULL);
		   double LBU2 = get_LBU(
			   ((nv_hdr_t *)fpls[0].points[ind2].buffer_spec)->Lat,
			   ((nv_hdr_t *)fpls[0].points[ind2].buffer_spec)->Lon,
			   ((nv_hdr_t *)fpls[0].points[min_pt].buffer_spec)->Lat,
			   ((nv_hdr_t *)fpls[0].points[min_pt].buffer_spec)->Lon,Lat,Lon,NULL,NULL);
           if(fabs(LBU1) < fabs(LBU2))
			   return(min_pt);
		   else
			   return(min_pt+1);
	   }
   }
   else if(less1) return(min_pt);
   else if(less2) return(min_pt+1);
   
   if(r1 && r2) return SS1<SS2 ? min_pt : min_pt+1;
   if(r1) return(min_pt);
   if(r2) return(min_pt+1);

   return(0);
*/
}

int fpl_get_legs(fpl_t *__fpl)
{
    int legs=0;
	for(int i=0;i<__fpl->point_in_use;i++)
		if(__fpl->points[i].buffer_spec) legs++;
	return(legs>1?legs-1:0);
}

BOOL get_leg(fpl_t *__fpl,rt_leg_t *rt_leg,int leg_number)
{
   int di=leg_number,si=leg_number-1;

   if((leg_number>__fpl->point_in_use-1) || (leg_number<1)) return(FALSE);
   for(int i=0;i<=leg_number;i++)
   {
	   if(!__fpl->points[i].buffer_spec)
	   {
	      di++;  si++;  break;
	   }
   }
   if(si < 0 ) return(FALSE);
   if(!__fpl->points[si].buffer_spec)
      si--;
   if(si<0) return(FALSE);
   if(!__fpl->points[di].buffer_spec) return(FALSE);
   rt_leg->Lat_src = ((nv_hdr_t *)__fpl->points[si].buffer_spec)->Lat;
   rt_leg->Lon_src = ((nv_hdr_t *)__fpl->points[si].buffer_spec)->Lon;
   rt_leg->Lat_dst = ((nv_hdr_t *)__fpl->points[di].buffer_spec)->Lat;
   rt_leg->Lon_dst = ((nv_hdr_t *)__fpl->points[di].buffer_spec)->Lon;
   strcpy(rt_leg->src_icao_id,((nv_hdr_t *)__fpl->points[si].buffer_spec)->ICAO_ID);
   strcpy(rt_leg->dst_icao_id,((nv_hdr_t *)__fpl->points[di].buffer_spec)->ICAO_ID);
   return(TRUE);
}

BOOL fpl_is_in_fpl(nv_hdr_t *__nv)
{
   for(int i=0;i<fpls[0].point_in_use-1;i++)
   {
	   if(!fpls[0].points[i].buffer_spec) continue;
	   if(
	      LONG2(__nv->Lat) == LONG2(((nv_hdr_t *)fpls[0].points[i].buffer_spec)->Lat)
		  &&
	      LONG2(__nv->Lon) == LONG2(((nv_hdr_t *)fpls[0].points[i].buffer_spec)->Lon)
	     )
		 return(TRUE);
   }
   return(FALSE);
}

BOOL fpl_get_active_LL(double *Lat,double *Lon)
{
   int ind = calc_active_leg();
   if(!ind) return(FALSE);
   nv_hdr_t *__nv = (nv_hdr_t*)fpls[0].points[ind].buffer_spec;
   *Lat = __nv->Lat;
   *Lon = __nv->Lon;
   return(TRUE);
}
BOOL fpl_get_next_LL(double *Lat,double *Lon)
{
	int ind = calc_next_leg();
	if(!ind) return(FALSE);
	nv_hdr_t *__nv = (nv_hdr_t*)fpls[0].points[ind].buffer_spec;
	*Lat = __nv->Lat;
	*Lon = __nv->Lon;
	return(TRUE);
}

int fpl_get_real_points(int fpl)
{
   int r_used=0;
   for(int i=0;i<fpls[fpl].point_in_use;i++)
   {
      if(fpls[fpl].points[i].buffer_spec) r_used++;
   }
   return(r_used);
}


int fpl_get_number_upt(void *buffer_spec)
{
	for ( int fpl = 0; fpl < MAX_FLIGHTPLANS; fpl++ )
	{
	    for(int point=0;point<fpls[fpl].point_in_use;point++)
		{
		    if(fpls[fpl].points[point].buffer_spec == buffer_spec)
				return(fpl);
		}
	}
	return(-1);
}

int fpl_get_nulls_before(int fpl_number,int stop_point)
{
   int nulls=0;
   for(int i=0;i<stop_point;i++)
	   if(!fpls[fpl_number].points[i].buffer_spec)
		   nulls++;
   return(nulls);
}