#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "nearest.h"
#include "vnav.h"
#include <stdio.h>
#include <math.h>


extern nav_db_t nav_db;
//extern nr_vor_list nr_vor_l;
extern dto_list_t K_dto_list;

//============================================================================
K_nav_box_t K_nav_box;
K_nav_lines_t K_nav_lines={0};
//*********************** NAV Pages Handlers *********************************
const int nav_first_page = 1;
const int nav_last_page  = 5;
static int left_page_number = 2;
static int right_page_number = 2;
static int cursored = 0;
static BOOL is_super5;
static BOOL is_super1;

static int r_cursored = 0;
static int l_cursored = 0;
static int s3_r_cursored;
static int s3_l_cursored;

static char *nav5_modes[]={"TK^ ","DTK^","HDG^","N^  ",NULL};
static int nav5_resolutions[]={1,2,3,5,7,10,15,20,30,40,60,80,120,160,240,360,480,1000,0};
static int curr_nav5_res = 7;
static int on_mode = 0;
static int on_res  = 0;
FLOAT64 nav_5_resolution = 20.0f;

double get_nav5_CRS (void);
int nav5res_index(double res);

FLOAT64 res_need(double pLat,double pLon)
{
	double __CRS = get_nav5_CRS();
	double Lat,Lon,_BRG,_S,_alpha;
	get_PPOS(&Lat,&Lon);
	_BRG   = get_ZPU(Lat,Lon,pLat,pLon,get_MAGVAR());
	_S     = get_S(Lat,Lon,pLat,pLon)/1.852f;
	_alpha = _BRG-__CRS;
	double y = _S*cos(_alpha/57.29578);
	double x = _S*sin(_alpha/57.29578);
	for(int i=0;nav5_resolutions[i];i++)
	{
		if(K_nav_box.curr_nav5_mode == NAV5_MODE_TN)
		{
			if( (fabs(y) <= nav5_resolutions[i]/2.0f) && (fabs(x) <= nav5_resolutions[i]/2.0f) )
				return(nav5_resolutions[i]);
		}
		else
		{
		   if( (fabs(y) <= nav5_resolutions[i]) && (fabs(x) <= nav5_resolutions[i]/2.0f) )
		      return(nav5_resolutions[i]);
		}
	}    
    return(1000.0f);
}

int nav5res_index(double res);

void set_near_resolution(rt_leg_t *__rt)
{
   double total_res=-1;
   if(nav_mode()==FPL_LEG || nav_mode()==FPLDTO_LEG)
   {
       double nLat,nLon;
	   double res1=-1,res2=-1;
	   //if(FALSE==fpl_get_active_LL(&aLat,&aLon)) return;
	   res1 = res_need(__rt->Lat_dst,__rt->Lon_dst);
	   if(TRUE==fpl_get_next_LL(&nLat,&nLon))
		   res2 = res_need(nLat,nLon);
	   total_res = res1>res2 ? res1 : res2; 
   }
   else if(nav_mode()==DTO_LEG)
   {
      total_res = res_need(__rt->Lat_dst,__rt->Lon_dst);
   }
   if(total_res>0)
   {
      nav_5_resolution =  nav5_resolutions[nav5res_index(total_res)];
   }
}
int nav5res_index(double res)
{
	for(int i=0;nav5_resolutions[i];i++)
	{
		if(res==(double)nav5_resolutions[i])
		{
			return(i);
		}
	}
    return(7);
}
enum {ON_S5_MIN=0,ON_VOR=0,ON_NDB=1,ON_APT=2,ON_MODE=3,ON_S5_MAX=3};
enum {ON_NDB_ON,ON_NDB_OFF};
enum {ON_VOR_MIN=0,ON_VOR_OFF=0,ON_VOR_TLH=1,ON_VOR_LH=2,ON_VOR_H=3,ON_VOR_MAX=3};
enum {ON_APT_ON,ON_APT_OFF};

enum {ON_S5L_MIN=0,ON_RES=0,ON_TBR=1,ON_DBR=2,ON_EFV=3,ON_S5L_MAX=3};
enum {EFV_MIN=0,EFV_ETE=0,EFV_FLY=1,EFV_VNV=2,EFV_MAX=2};
enum {DBR_MIN=0,DBR_DTK=0,DBR_BRG=1,DBR_RAD=2,DBR_MAX=2};
enum {TBR_MIN=0,TBR_TK=0,TBR_BRG=1,TBR_RAD=2,TBR_MAX=2};

static int s5_nearby_vis = 1;
static int s5_r_mode = ON_VOR;
static int s5_l_mode = ON_RES;
static int s5_ndb_on=0;
static int s5_vor_on=0;
static int s5_apt_on=0;

static int s5_efv=0;
static int s5_dbr=0;
static int s5_tbr=0;

char *get_s5_vor(int s5_vor_on)
{
  switch(s5_vor_on)
  {
  case ON_VOR_OFF:
	  return("OFF");
  case ON_VOR_TLH:
	  return("TLH");
  case ON_VOR_LH:
	  return(" LH");
  case ON_VOR_H:
	  return("  H");
  }
  return("");
}
BOOL is_visible_vor(int type)
{
	if(s5_vor_on==ON_VOR_TLH)
		return type>=1 && type<=3 ? TRUE : FALSE;
	if(s5_vor_on==ON_VOR_LH)
		return type==2 || type==3 ? TRUE : FALSE;
	if(s5_vor_on==ON_VOR_H)
		return type==3 ? TRUE : FALSE;

	return(FALSE);
}


//==============================================================================
static int NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_CLR|INPUT_ENTER;

BOOL nav_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&NAV_INPUT_MASK);
}
static int NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR;

BOOL nav_handle2_key(int INPUT_MASK)
{
    return(INPUT_MASK&NAV2_INPUT_MASK);
}
//===============================================================================
void nav5_res_prev(void)
{
   if(curr_nav5_res == 0) return;
   nav_5_resolution = nav5_resolutions[--curr_nav5_res];
}
void nav5_res_next(void)
{
	if(!nav5_resolutions[curr_nav5_res])
		curr_nav5_res = -1;

	if(!nav5_resolutions[curr_nav5_res+1])
	{ 
	    if(is_super5)
			curr_nav5_res++;
        return;
    }
    nav_5_resolution = nav5_resolutions[++curr_nav5_res];
}

void nav5_mode_prev(void)
{
   if(K_nav_box.curr_nav5_mode == NAV5_MODE_TK) return;
   K_nav_box.curr_nav5_mode--;
}
void nav5_mode_next(void)
{
   if(!nav5_modes[K_nav_box.curr_nav5_mode+1]) return;
   K_nav_box.curr_nav5_mode++;
}

void s5_mode_prev(void)
{
   s5_r_mode--;
   if(s5_r_mode<ON_S5_MIN) s5_r_mode=ON_S5_MAX;
}
void s5_mode_next(void)
{
   s5_r_mode++;
   if(s5_r_mode>ON_S5_MAX) s5_r_mode=ON_S5_MIN;
}

enum {DBROBS_DBR,DBROBS_OBS};
int dbrobs_mode;

void s5l_mode_prev(void)
{
	if(s5_l_mode == (ON_DBR+1) && mod_get_mode()==OBS_MODE && s5_dbr==DBR_DTK)
	{
	   s5_l_mode--;
	   dbrobs_mode=DBROBS_DBR;
	}
	else if(s5_l_mode == ON_DBR && mod_get_mode()==OBS_MODE && dbrobs_mode==DBROBS_DBR && s5_dbr==DBR_DTK)
	   dbrobs_mode=DBROBS_OBS;
	else 
	   s5_l_mode--;
	if(s5_l_mode<ON_S5L_MIN) s5_l_mode=ON_S5L_MAX;
}
void s5l_mode_next(void)
{
	if(s5_l_mode == (ON_DBR-1) && mod_get_mode()==OBS_MODE && s5_dbr==DBR_DTK)
	{
		s5_l_mode++;
		dbrobs_mode=DBROBS_OBS;
	}
	else if(s5_l_mode == ON_DBR && mod_get_mode()==OBS_MODE && dbrobs_mode==DBROBS_OBS && s5_dbr==DBR_DTK)
		dbrobs_mode=DBROBS_DBR;
	else 
		s5_l_mode++;
	if(s5_l_mode>ON_S5L_MAX) s5_l_mode=ON_S5L_MIN;
}

void s5_cur_mode_next(void)
{
   switch(s5_r_mode)
   {
   case ON_VOR:
	   {
	      if(s5_vor_on==ON_VOR_MAX) s5_vor_on=ON_VOR_MIN;
		  else
             s5_vor_on++;
		  return;
	   }
   case ON_NDB:
	   {
	      s5_ndb_on=1-s5_ndb_on;
		  return;
	   }
   case ON_APT:
	   {
	      s5_apt_on=1-s5_apt_on;
		  return;
	   }
   case ON_MODE:
	   {
          nav5_mode_next();
		  return;
	   }
   }
}

void s5_cur_mode_prev(void)
{
   switch(s5_r_mode)
   {
   case ON_VOR:
	   {
	      if(s5_vor_on==ON_VOR_MIN) s5_vor_on=ON_VOR_MAX;
		  else
             s5_vor_on--;
		  return;
	   }
   case ON_NDB:
	   {
	      s5_ndb_on=1-s5_ndb_on;
		  return;
	   }
   case ON_APT:
	   {
	      s5_apt_on=1-s5_apt_on;
		  return;
	   }
   case ON_MODE:
	   {
          nav5_mode_prev();
		  return;
	   }
   }
}

void s5_lcur_mode_next(void)
{
	switch(s5_l_mode)
	{
	case ON_EFV:
		{
			s5_efv++;
			if(s5_efv>EFV_MAX) s5_efv=EFV_MIN;
			break;
		}
	case ON_RES:
		{
			nav5_res_next();
			break;
		}
	case ON_DBR:
		{
			if(s5_dbr == DBR_DTK && mod_get_mode()==OBS_MODE && dbrobs_mode==DBROBS_OBS)
			  obs_next_val();
			else
			{
			   s5_dbr++;
			   if(s5_dbr>DBR_MAX) s5_dbr=DBR_MIN;
			}
		}
		break;
	case ON_TBR:
		{
			s5_tbr++;
			if(s5_tbr>TBR_MAX) s5_tbr=TBR_MIN;
			break;
		}
	}
}

void s5_lcur_mode_prev(void)
{
	switch(s5_l_mode)
	{
	case ON_EFV:
		{
			s5_efv--;
			if(s5_efv<EFV_MIN) s5_efv=EFV_MAX;
		}
		break;
	case ON_RES:
		{
			nav5_res_prev();
		}
		break;
	case ON_DBR:
		{
			if(s5_dbr == DBR_DTK && mod_get_mode()==OBS_MODE && dbrobs_mode==DBROBS_OBS)
				obs_prev_val();
			else
			{
			   s5_dbr--;
			   if(s5_dbr<DBR_MIN) s5_dbr=DBR_MAX;
			}
		}
		break;
	case ON_TBR:
		{
			s5_tbr--;
			if(s5_tbr<TBR_MIN) s5_tbr=TBR_MAX;
		}
		break;
	}
}


double get_nav5_CRS(void)
{
   if(K_nav_box.curr_nav5_mode == NAV5_MODE_TK)
	   return(get_TK());
   if(K_nav_box.curr_nav5_mode == NAV5_MODE_DTK)
   {
	   rt_leg_t *curr_rt = get_rt_leg();
	   if(curr_rt)
		   return(curr_rt->DTK);
   }
   if(K_nav_box.curr_nav5_mode == NAV5_MODE_HDG)
   {
      return(get_HDG());
   }
   return(0);
}
int inc_nav_page(int curr_page)
{
	return(curr_page == nav_last_page?nav_first_page:curr_page+1);
}
int dec_nav_page(int curr_page)
{
	return(curr_page == nav_first_page?nav_last_page:curr_page-1);
}
void set_navpage_number(int page_pos,int page_number)
{
    if(page_number < 0)
		return;
	if(page_pos == PAGE_LEFT)
		left_page_number=page_number?page_number:left_page_number;
	if(page_pos == PAGE_RIGHT)
       right_page_number=page_number;
}
int get_navpage_number(int page_pos)
{
	if(page_pos == PAGE_LEFT)
       return(left_page_number);
	if(page_pos == PAGE_RIGHT)
       return(right_page_number);
	return (0);
}
//======================================================================================

void calc_box(void)
{
	if(K_nav_box.curr_nav5_mode != NAV5_MODE_TN)
	{
	   K_nav_box.box_lines[H_U].x1 = -nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_U].x2 = nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_U].y1 = K_nav_box.box_lines[H_U].y2 = K_nav_box.box_lines[H_U].b = nav_5_resolution;
	   K_nav_box.box_lines[H_U].k  = 0;

	   K_nav_box.box_lines[H_D].x1 = -nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_D].x2 = nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_D].y1 = K_nav_box.box_lines[H_D].y2 = K_nav_box.box_lines[H_D].b = -nav_5_resolution*0.3f;
	   K_nav_box.box_lines[H_D].k  = 0;

	   K_nav_box.box_lines[V_L].x1 = K_nav_box.box_lines[V_L].x2 = -nav_5_resolution/2.0f;
	   K_nav_box.box_lines[V_L].y1 = -nav_5_resolution*0.3f;
	   K_nav_box.box_lines[V_L].y2 = nav_5_resolution;
	   K_nav_box.box_lines[V_L].k  = 7000.0f * 1.852f * 10e3;
	   K_nav_box.box_lines[V_L].b  = K_nav_box.box_lines[V_L].y1 - K_nav_box.box_lines[V_L].k*K_nav_box.box_lines[V_L].x1;

	   K_nav_box.box_lines[V_R].x1 = K_nav_box.box_lines[V_R].x2 = nav_5_resolution/2.0f;
	   K_nav_box.box_lines[V_R].y1 = -nav_5_resolution*0.3f;
	   K_nav_box.box_lines[V_R].y2 = nav_5_resolution;
	   K_nav_box.box_lines[V_R].k  = 7000.0f * 1.852f * 10e3;
	   K_nav_box.box_lines[V_R].b  = K_nav_box.box_lines[V_R].y1 - K_nav_box.box_lines[V_R].k*K_nav_box.box_lines[V_R].x1;
	   K_nav_box.start_x = -nav_5_resolution/2.0f;
	   K_nav_box.stop_x = nav_5_resolution/2.0f;
	   K_nav_box.start_y = -nav_5_resolution*0.3f;
	   K_nav_box.stop_y = nav_5_resolution;
	}
	else
	{
	   K_nav_box.box_lines[H_U].x1 = -nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_U].x2 = nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_U].y1 = K_nav_box.box_lines[H_U].y2 = K_nav_box.box_lines[H_U].b = nav_5_resolution*0.5f;
	   K_nav_box.box_lines[H_U].k  = 0;

	   K_nav_box.box_lines[H_D].x1 = -nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_D].x2 = nav_5_resolution/2.0f;
	   K_nav_box.box_lines[H_D].y1 = K_nav_box.box_lines[H_D].y2 = K_nav_box.box_lines[H_D].b = -nav_5_resolution*0.5f;
	   K_nav_box.box_lines[H_D].k  = 0;

	   K_nav_box.box_lines[V_L].x1 = K_nav_box.box_lines[V_L].x2 = -nav_5_resolution/2.0f;
	   K_nav_box.box_lines[V_L].y1 = -nav_5_resolution*0.5f;
	   K_nav_box.box_lines[V_L].y2 = nav_5_resolution*0.5f;
	   K_nav_box.box_lines[V_L].k  = 7000.0f * 1.852f * 10e3;
	   K_nav_box.box_lines[V_L].b  = K_nav_box.box_lines[V_L].y1 - K_nav_box.box_lines[V_L].k*K_nav_box.box_lines[V_L].x1;

	   K_nav_box.box_lines[V_R].x1 = K_nav_box.box_lines[V_R].x2 = nav_5_resolution/2.0f;
	   K_nav_box.box_lines[V_R].y1 = -nav_5_resolution*0.5f;
	   K_nav_box.box_lines[V_R].y2 = nav_5_resolution*0.5f;
	   K_nav_box.box_lines[V_R].k  = 7000.0f * 1.852f * 10e3;
	   K_nav_box.box_lines[V_R].b  = K_nav_box.box_lines[V_R].y1 - K_nav_box.box_lines[V_R].k*K_nav_box.box_lines[V_R].x1;
	   K_nav_box.start_x = -nav_5_resolution/2.0f;
	   K_nav_box.stop_x = nav_5_resolution/2.0f;
	   K_nav_box.start_y = -nav_5_resolution*0.5f;
	   K_nav_box.stop_y = nav_5_resolution*0.5;
	}
}
#define TOL(a) (long)(a*1000)

BOOL is_in_nav_box(double x,double y)
{
	if( TOL(x) >= TOL(K_nav_box.start_x) && TOL(x) <= TOL(K_nav_box.stop_x)
		&& TOL(y)>=TOL(K_nav_box.start_y) && TOL(y)<=TOL(K_nav_box.stop_y))
      return(TRUE);
	return(FALSE);
}

BOOL is_on_line(K_point_t *point,K_line_t *line)
{
	double __x_start = line->x1 < line->x2 ? line->x1 : line->x2;
	double __y_start = line->y1 < line->y2 ? line->y1 : line->y2;

	double __x_stop = line->x1 == __x_start ? line->x2 : line->x1;
	double __y_stop = line->y1 == __y_start ? line->y2 : line->y1;
	

	if( TOL(point->x) >= TOL(__x_start) && TOL(point->x) <= TOL(__x_stop) && TOL(point->y)>=TOL(__y_start) && TOL(point->y)<=TOL(__y_stop))
      return(TRUE);
	return(FALSE);
}
K_point_t *get_inter_point(K_line_t *line1,K_line_t *line2)
{
   static K_point_t __private_point;
   if(line2->k == line1->k)
	   return(NULL);

   __private_point.x = (line1->b-line2->b)/(line2->k-line1->k);
   __private_point.y = line1->k*__private_point.x + line1->b;

   return(&__private_point);
}

BOOL set_intersection_3(K_line_t *line)
{
	K_point_t *int_point;
	int pts_in_use=0;
	K_point_t pts[2];

	while(1)
	{
		if((int_point = get_inter_point(&K_nav_box.box_lines[H_U],line)))
		{
			if(is_in_nav_box(int_point->x,int_point->y))
			{
				pts[0].x = int_point->x;
				pts[0].y = int_point->y;
				pts_in_use++;
			}
		}
		if((int_point = get_inter_point(&K_nav_box.box_lines[H_D],line)))
		{
			if(is_in_nav_box(int_point->x,int_point->y))
			{
				if(!pts_in_use)
				{
					pts[0].x = int_point->x;
					pts[0].y = int_point->y;
					pts_in_use++;
				}
				else if(pts_in_use)
				{
					pts[1].x = int_point->x;
					pts[1].y = int_point->y;
					pts_in_use++;
					break;
				}
			}
		}

		if((int_point = get_inter_point(&K_nav_box.box_lines[V_L],line)))
		{
			if(is_in_nav_box(int_point->x,int_point->y))
			{
				if(!pts_in_use)
				{
					pts[0].x = int_point->x;
					pts[0].y = int_point->y;
					pts_in_use++;
				}
				else if(pts_in_use)
				{  
					pts[1].x = int_point->x;
					pts[1].y = int_point->y;
					pts_in_use++;
					break;
				}
			}
		}
		if((int_point = get_inter_point(&K_nav_box.box_lines[V_R],line)))
		{
			if(is_in_nav_box(int_point->x,int_point->y))
			{
				if(!pts_in_use)
				{
					pts[0].x = int_point->x;
					pts[0].y = int_point->y;
					pts_in_use++;
				}
				else if(pts_in_use)
				{
					pts[1].x = int_point->x;
					pts[1].y = int_point->y;
					pts_in_use++;
					break;
				}
			}
		}

		break;
	}
	if(pts_in_use<2)
		return(FALSE);
	line->x1 = pts[0].x;
	line->y1 = pts[0].y;
	line->x2 = pts[1].x;
	line->y2 = pts[1].y;
	return(TRUE);
}

BOOL set_intersection_2(K_line_t *line)
{
    K_point_t *int_point;
	int pts_in_use=0;
	K_point_t pts[2];

	while(1)
	{
	   if((int_point = get_inter_point(&K_nav_box.box_lines[H_U],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		   {
			   pts[0].x = int_point->x;
			   pts[0].y = int_point->y;
			   pts_in_use++;
		   }
	   }
	   if((int_point = get_inter_point(&K_nav_box.box_lines[H_D],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		   {
		      if(!pts_in_use)
			  {
			     pts[0].x = int_point->x;
			     pts[0].y = int_point->y;
			     pts_in_use++;
              }
			  else if(pts_in_use)
			  {
			     pts[1].x = int_point->x;
			     pts[1].y = int_point->y;
			     pts_in_use++;
				 break;
			  }
		   }
	   }

	   if((int_point = get_inter_point(&K_nav_box.box_lines[V_L],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		   {
		      if(!pts_in_use)
		      {
			     pts[0].x = int_point->x;
			     pts[0].y = int_point->y;
		         pts_in_use++;
              }
		      else if(pts_in_use)
		      {  
			     pts[1].x = int_point->x;
			     pts[1].y = int_point->y;
		         pts_in_use++;
		         break;
		      }
		   }
	   }
	   if((int_point = get_inter_point(&K_nav_box.box_lines[V_R],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		   {
		      if(!pts_in_use)
		      {
			     pts[0].x = int_point->x;
			     pts[0].y = int_point->y;
		         pts_in_use++;
              }
		      else if(pts_in_use)
		      {
				 pts[1].x = int_point->x;
			     pts[1].y = int_point->y;
		         pts_in_use++;
		         break;
		      }
		   }
	   }

	   break;
	}
    if(pts_in_use<2)
		return(FALSE);
	line->x1 = pts[0].x;
    line->y1 = pts[0].y;
	line->x2 = pts[1].x;
    line->y2 = pts[1].y;
    return(TRUE);
}
BOOL set_intersection(K_line_t *line,int point_index)
{
    K_point_t *int_point;

	while(1)
	{
	   if((int_point = get_inter_point(&K_nav_box.box_lines[H_U],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		      break;
	   }
	   if((int_point = get_inter_point(&K_nav_box.box_lines[H_D],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		      break;                  
	   }
	   if((int_point = get_inter_point(&K_nav_box.box_lines[V_L],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		      break;
	   }
	   if((int_point = get_inter_point(&K_nav_box.box_lines[V_R],line)))
	   {
		   if(is_in_nav_box(int_point->x,int_point->y) && is_on_line(int_point,line))
		      break;
	   }
       int_point=NULL;
	   break;
	}
    if(!int_point)
		return(FALSE);
	if(point_index == 1)
	{
		line->x1 = int_point->x;
		line->y1 = int_point->y;
	}
	else if(point_index == 2)
	{
		line->x2 = int_point->x;
		line->y2 = int_point->y;
	}
    return(TRUE);
}
//******************************************************************************************
extern fpl_t fpls[];
extern nr_apt_list nr_apt_l;
extern nav_db_t nav_db;

extern nr_ndb_list nr_ndb_l;
extern nr_vor_list nr_vor_l;
extern nr_apt_list nr_apt_l;

void calc_nr_points(void)
{
   
   double __CRS = get_nav5_CRS();
   double _BRG,_S,_alpha;
   K_nav_lines.points_count = 0;
   if(!is_super5) return;
   if(!s5_nearby_vis) return;
   FLOAT64 Lat,Lon;
   get_PPOS(&Lat,&Lon);

   if(s5_ndb_on)
   {
      for(int i=0;i<nr_ndb_l.nr_ndb_count;i++)
      {
	     nv_hdr_t *__nv = (nv_hdr_t *)&nav_db.ndbs[nr_ndb_l.list[i].index];
	     if(fpl_is_in_fpl(__nv)) continue;
	     _BRG   = get_ZPU(Lat,Lon,__nv->Lat,__nv->Lon,get_MAGVAR());
         _S     = get_S(Lat,Lon,__nv->Lat,__nv->Lon)/1.852f;
         _alpha = _BRG-__CRS;
         K_nav_lines.points[K_nav_lines.points_count].y = _S*cos(_alpha/57.29578);
	     K_nav_lines.points[K_nav_lines.points_count].x = _S*sin(_alpha/57.29578);
         BOOL is_inbox = is_in_nav_box(K_nav_lines.points[K_nav_lines.points_count].x,K_nav_lines.points[K_nav_lines.points_count].y);
         if(is_inbox)
         {
		    strcpy(K_nav_lines.points[K_nav_lines.points_count].ICAO_ID,nav_db.ndbs[nr_ndb_l.list[i].index].ICAO_ID);
		    K_nav_lines.points[K_nav_lines.points_count].pt_type = NAVAID_NDB;
	        K_nav_lines.points_count++;
         }
      }
   }
   if(s5_vor_on)
   {
      for(int i=0;i<nr_vor_l.nr_vor_count;i++)
      {
	     if(fpl_is_in_fpl((nv_hdr_t *)&nav_db.vors[nr_vor_l.list[i].index])) continue;
		 if(!is_visible_vor(nav_db.vors[nr_vor_l.list[i].index].TYPE)) continue;

		 nv_hdr_t *__nv = (nv_hdr_t *)&nav_db.vors[nr_vor_l.list[i].index];
		 
		 _BRG   = get_ZPU(Lat,Lon,__nv->Lat,__nv->Lon,get_MAGVAR());
		 _S     = get_S(Lat,Lon,__nv->Lat,__nv->Lon)/1.852f;
		 _alpha = _BRG-__CRS;
     
	     K_nav_lines.points[K_nav_lines.points_count].y = _S*cos(_alpha/57.29578);
	     K_nav_lines.points[K_nav_lines.points_count].x = _S*sin(_alpha/57.29578);

         BOOL is_inbox = is_in_nav_box(K_nav_lines.points[K_nav_lines.points_count].x,K_nav_lines.points[K_nav_lines.points_count].y);
         if(is_inbox)
         {
		    strcpy(K_nav_lines.points[K_nav_lines.points_count].ICAO_ID,nav_db.vors[nr_vor_l.list[i].index].ICAO_ID);
		    K_nav_lines.points[K_nav_lines.points_count].pt_type = NAVAID_VOR;
	        K_nav_lines.points_count++;
         }
	  }
   }
   if(s5_apt_on)
   {
      for(int i=0;i<nr_apt_l.nr_apt_count;i++)
      {
	     if(fpl_is_in_fpl((nv_hdr_t *)&nav_db.apts[nr_apt_l.list[i].index])) continue;
	     
		 nv_hdr_t *__nv = (nv_hdr_t *)&nav_db.apts[nr_apt_l.list[i].index];

		 _BRG   = get_ZPU(Lat,Lon,__nv->Lat,__nv->Lon,get_MAGVAR());
		 _S     = get_S(Lat,Lon,__nv->Lat,__nv->Lon)/1.852f;
		 _alpha = _BRG-__CRS;
      
	     K_nav_lines.points[K_nav_lines.points_count].y = _S*cos(_alpha/57.29578);
	     K_nav_lines.points[K_nav_lines.points_count].x = _S*sin(_alpha/57.29578);

         BOOL is_inbox = is_in_nav_box(K_nav_lines.points[K_nav_lines.points_count].x,K_nav_lines.points[K_nav_lines.points_count].y);
         if(is_inbox)
         {
		    strcpy(K_nav_lines.points[K_nav_lines.points_count].ICAO_ID,nav_db.apts[nr_apt_l.list[i].index].ICAO_ID);
		    K_nav_lines.points[K_nav_lines.points_count].pt_type = NAVAID_APT;
	        K_nav_lines.points_count++;
         }
      }
   }
}

void calc_rnws(void)
{
   double __CRS = get_nav5_CRS();
   double _BRG1,_S1,_alpha1,_BRG2,_S2,_alpha2;
   K_nav_lines.rnws_count = 0;

   if(!is_super5) return;
   if(nav_5_resolution>2.0f) return;
   if(nr_apt_l.nr_apt_count<1) return;
   nvdb_apt *apt = &nav_db.apts[nr_apt_l.list[0].index];
   if(apt->rnws_count<1) return;

   double Lat,Lon;
   get_PPOS(&Lat,&Lon);

   for(int i=0;i<apt->rnws_count;i++)
   {
	  _BRG1   = get_ZPU(Lat,Lon,apt->rws[i].bLat,apt->rws[i].bLon,get_MAGVAR());
      _S1     = get_S(Lat,Lon,apt->rws[i].bLat,apt->rws[i].bLon)/1.852f;
      _alpha1 = _BRG1-__CRS;
	  _BRG2   = get_ZPU(Lat,Lon,apt->rws[i].eLat,apt->rws[i].eLon,get_MAGVAR());
      _S2     = get_S(Lat,Lon,apt->rws[i].eLat,apt->rws[i].eLon)/1.852f;
      _alpha2 = _BRG2-__CRS;
      
	  K_nav_lines.rnws[K_nav_lines.rnws_count].y1 = _S1*cos(_alpha1/57.29578);
      K_nav_lines.rnws[K_nav_lines.rnws_count].x1 = _S1*sin(_alpha1/57.29578);
      K_nav_lines.rnws[K_nav_lines.rnws_count].y2 = _S2*cos(_alpha2/57.29578);
      K_nav_lines.rnws[K_nav_lines.rnws_count].x2 = _S2*sin(_alpha2/57.29578);

      CALC_K(K_nav_lines.rnws[K_nav_lines.rnws_count]);
      CALC_B(K_nav_lines.rnws[K_nav_lines.rnws_count]);

      BOOL is_inbox_1 = is_in_nav_box(K_nav_lines.rnws[K_nav_lines.rnws_count].x1,K_nav_lines.rnws[K_nav_lines.rnws_count].y1);
      BOOL is_inbox_2 = is_in_nav_box(K_nav_lines.rnws[K_nav_lines.rnws_count].x2,K_nav_lines.rnws[K_nav_lines.rnws_count].y2);
      BOOL ret = FALSE;
      
     if(is_inbox_1 && is_inbox_2)
     {
        K_nav_lines.rnws[K_nav_lines.rnws_count].n1 = 0;
	    K_nav_lines.rnws[K_nav_lines.rnws_count].n2 = 0;
	    ret=TRUE;
     }
     else if(!is_inbox_1 && is_inbox_2)
     {
        K_nav_lines.rnws[K_nav_lines.rnws_count].n1 = -1;
	    K_nav_lines.rnws[K_nav_lines.rnws_count].n2 = 0;
	    ret = set_intersection(&K_nav_lines.rnws[K_nav_lines.rnws_count],1);
     }
     else if(is_inbox_1 && !is_inbox_2)
     {
        K_nav_lines.rnws[K_nav_lines.rnws_count].n1 = 0;
	    K_nav_lines.rnws[K_nav_lines.rnws_count].n2 = -1;
	    ret = set_intersection(&K_nav_lines.rnws[K_nav_lines.rnws_count],2);
     }
     else if(!is_inbox_1 && !is_inbox_2)
     {
        K_nav_lines.rnws[K_nav_lines.rnws_count].n1 = -1;
	    K_nav_lines.rnws[K_nav_lines.rnws_count].n2 = -1;
	    ret = set_intersection_2(&K_nav_lines.rnws[K_nav_lines.rnws_count]);
     }
     if(ret)
     {
        if(nav_5_resolution == 2.0f)
		{
		   if(!i)
              K_nav_lines.rnws[K_nav_lines.rnws_count].numbers_only=1;
		   else
              K_nav_lines.rnws[K_nav_lines.rnws_count].numbers_only=0;
		}
	    else if(nav_5_resolution == 1.0f)
			K_nav_lines.rnws[K_nav_lines.rnws_count].numbers_only=1;
		else
            K_nav_lines.rnws[K_nav_lines.rnws_count].numbers_only=0;

		_snprintf(K_nav_lines.rnws[K_nav_lines.rnws_count].pDes,sizeof(K_nav_lines.rnws[K_nav_lines.rnws_count].pDes)-1,
			"%02d%s",(int)apt->rws[i].pNum,get_RNW_des(apt->rws[i].pDes));

		_snprintf(K_nav_lines.rnws[K_nav_lines.rnws_count].sDes,sizeof(K_nav_lines.rnws[K_nav_lines.rnws_count].sDes)-1,
			"%02d%s",(int)apt->rws[i].sNum,get_RNW_des(apt->rws[i].sDes));

        K_nav_lines.rnws_count++;
     }
   }
}

void calc_fpl_data(int is_dto_active)
{
   K_nav_lines.legs_count = 0;
   double __CRS = get_nav5_CRS();
   double Lat,Lon;
   get_PPOS(&Lat,&Lon);
   double _BRG1,_S1,_alpha1,_BRG2,_S2,_alpha2;
   int curr_fpl_leg = fpl_get_active_leg();

     for(int i=1;i<=fpl_get_legs(&fpls[0]);i++)
     {
	    rt_leg_t __rt;
		__rt.src_icao_id = K_nav_lines.rt_legs[K_nav_lines.legs_count].n1_icao;
		__rt.dst_icao_id = K_nav_lines.rt_legs[K_nav_lines.legs_count].n2_icao;

		if(!get_leg(&fpls[0],&__rt,i)) continue;
		_BRG1   = get_ZPU(Lat,Lon,__rt.Lat_src,__rt.Lon_src,get_MAGVAR());
        _S1     = get_S(Lat,Lon,__rt.Lat_src,__rt.Lon_src)/1.852f;
        _alpha1 = _BRG1-__CRS;
	    _BRG2   = get_ZPU(Lat,Lon,__rt.Lat_dst,__rt.Lon_dst,get_MAGVAR());
	    _S2     = get_S(Lat,Lon,__rt.Lat_dst,__rt.Lon_dst)/1.852f;
        _alpha2 = _BRG2-__CRS;
      
	  K_nav_lines.rt_legs[K_nav_lines.legs_count].y1 = _S1*cos(_alpha1/57.29578);
	  K_nav_lines.rt_legs[K_nav_lines.legs_count].x1 = _S1*sin(_alpha1/57.29578);
	  K_nav_lines.rt_legs[K_nav_lines.legs_count].y2 = _S2*cos(_alpha2/57.29578);
	  K_nav_lines.rt_legs[K_nav_lines.legs_count].x2 = _S2*sin(_alpha2/57.29578);


	  CALC_K(K_nav_lines.rt_legs[K_nav_lines.legs_count]);
	  CALC_B(K_nav_lines.rt_legs[K_nav_lines.legs_count]);

      BOOL is_inbox_1 = is_in_nav_box(K_nav_lines.rt_legs[K_nav_lines.legs_count].x1,K_nav_lines.rt_legs[K_nav_lines.legs_count].y1);
	  BOOL is_inbox_2 = is_in_nav_box(K_nav_lines.rt_legs[K_nav_lines.legs_count].x2,K_nav_lines.rt_legs[K_nav_lines.legs_count].y2);
      BOOL ret = FALSE;
      
      if(is_inbox_1 && is_inbox_2)
	  {
		 K_nav_lines.rt_legs[K_nav_lines.legs_count].n1 = i;
		 K_nav_lines.rt_legs[K_nav_lines.legs_count].n2 = i+1;
		 int nulls = fpl_get_nulls_before(0,curr_fpl_leg);
         if((curr_fpl_leg-nulls) == i)
			 K_nav_lines.rt_legs[K_nav_lines.legs_count].is_arrow=1;
		 else
             K_nav_lines.rt_legs[K_nav_lines.legs_count].is_arrow=0;
		 ret=TRUE;
	  }
	  else if(!is_inbox_1 && is_inbox_2)
	  {
		  K_nav_lines.rt_legs[K_nav_lines.legs_count].n1 = 0;
		  K_nav_lines.rt_legs[K_nav_lines.legs_count].n2 = i+1;
		  int nulls = fpl_get_nulls_before(0,curr_fpl_leg);
          if((curr_fpl_leg-nulls) == i)
		     K_nav_lines.rt_legs[K_nav_lines.legs_count].is_arrow=1;
		  else
             K_nav_lines.rt_legs[K_nav_lines.legs_count].is_arrow=0;
		  ret = set_intersection(&K_nav_lines.rt_legs[K_nav_lines.legs_count],1);
	  }
	  else if(is_inbox_1 && !is_inbox_2)
	  {
		  K_nav_lines.rt_legs[K_nav_lines.legs_count].n1 = i;
		  K_nav_lines.rt_legs[K_nav_lines.legs_count].n2 = 0;
   	      K_nav_lines.rt_legs[K_nav_lines.legs_count].is_arrow=0;
		  ret = set_intersection(&K_nav_lines.rt_legs[K_nav_lines.legs_count],2);
	  }
	  else if(!is_inbox_1 && !is_inbox_2)
	  {
		  K_nav_lines.rt_legs[K_nav_lines.legs_count].n1 = 0;
		  K_nav_lines.rt_legs[K_nav_lines.legs_count].n2 = 0;
   	      K_nav_lines.rt_legs[K_nav_lines.legs_count].is_arrow=0;
		  ret = set_intersection_2(&K_nav_lines.rt_legs[K_nav_lines.legs_count]);
	  }
	  if(ret)
	  {
		  K_nav_lines.rt_legs[K_nav_lines.legs_count].is_dto_line=0;
          if(mod_get_mode() == LEG_MODE)
		  {
		     if(is_dto_active)
			 {
			    if(nav_mode() == FPLDTO_LEG)
				{
				   if(i<=curr_fpl_leg)
					   K_nav_lines.rt_legs[K_nav_lines.legs_count].numbers_only=1;
				   else
					   K_nav_lines.rt_legs[K_nav_lines.legs_count].numbers_only=0;
				}
				else 
				   K_nav_lines.rt_legs[K_nav_lines.legs_count].numbers_only=1;
			 }
			 else
		        K_nav_lines.rt_legs[K_nav_lines.legs_count].numbers_only=0;
		  }
		  else
		     K_nav_lines.rt_legs[K_nav_lines.legs_count].numbers_only=1;
          if(is_dto_active)
             K_nav_lines.rt_legs[K_nav_lines.legs_count].is_arrow=0;
		  K_nav_lines.legs_count++;
	  }
   }
}
//**************************************************************************************
void calc_nav5_data(void)
{
   if(is_super5)
	   K_nav_lines.is_r_menu = r_cursored;
   rt_leg_t *curr_rt = get_rt_leg();
   if(K_nav_box.curr_nav5_mode == NAV5_MODE_TK && get_GS(1.852f)<2)
   {
      K_nav_lines.legs_count = 0;
	  K_nav_lines.rnws_count = 0;
	  K_nav_lines.points_count = 0;
	  return;
   }

   calc_box();
   calc_rnws();
   calc_nr_points();

   if(!curr_rt)
   {
      K_nav_lines.legs_count = 0;
	  return;
   }

   if(curr_rt->dst_icao_id && curr_rt->src_icao_id)
      calc_fpl_data(0);
   else
   {
      calc_fpl_data(1);
	  int dto_leg = K_nav_lines.legs_count;
	  // D-TO Mode
	  double __CRS = get_nav5_CRS();
	  double Lat,Lon;
	  get_PPOS(&Lat,&Lon);
	  double _BRG1,_S1,_alpha1,_BRG2,_S2,_alpha2;
	  double pLat=curr_rt->Lat_dst;
	  double pLon=curr_rt->Lon_dst;

	  if(mod_get_mode() == OBS_MODE)
	  {
		_S1       = curr_rt->LBU;//curr_rt->DIS;// get_S(Lat,Lon,sLat,sLon)/1.852f;
		_BRG1 = curr_rt->DTK - __CRS;
		_alpha1 = _BRG1 - 90;

		_BRG2   = get_ZPU(Lat,Lon,pLat,pLon,get_MAGVAR());
		_S2     = get_S(Lat,Lon,pLat,pLon)/1.852f;
		_alpha2 = _BRG2-__CRS;	    

	  }
	  else
	  {
		  _BRG1   = get_ZPU(Lat,Lon,curr_rt->Lat_src,curr_rt->Lon_src,get_MAGVAR());
	      _S1     = get_S(Lat,Lon,curr_rt->Lat_src,curr_rt->Lon_src)/1.852f;
          _alpha1 = _BRG1-__CRS;
	      _BRG2   = get_ZPU(Lat,Lon,curr_rt->Lat_dst,curr_rt->Lon_dst,get_MAGVAR());
	      _S2     = get_S(Lat,Lon,curr_rt->Lat_dst,curr_rt->Lon_dst)/1.852f;
          _alpha2 = _BRG2-__CRS;
	  }
      
	  strcpy(K_nav_lines.rt_legs[dto_leg].n2_icao,curr_rt->dst_icao_id);

	  K_nav_lines.rt_legs[dto_leg].y1 = _S1*cos(_alpha1/57.29578);
	  K_nav_lines.rt_legs[dto_leg].x1 = _S1*sin(_alpha1/57.29578); 	 
	  K_nav_lines.rt_legs[dto_leg].y2 = _S2*cos(_alpha2/57.29578);
	  K_nav_lines.rt_legs[dto_leg].x2 = _S2*sin(_alpha2/57.29578);

	  CALC_K(K_nav_lines.rt_legs[dto_leg]);
	  CALC_B(K_nav_lines.rt_legs[dto_leg]);

      BOOL add_dto_line=FALSE;
	  K_line_t arrow_obs_line;
	  if(mod_get_mode() == OBS_MODE)
	  {
		  //arrow_obs_line = K_nav_lines.rt_legs[dto_leg];
		  //set_intersection_3(&arrow_obs_line);
		  //add_dto_line=TRUE;
		  set_intersection_3(&K_nav_lines.rt_legs[dto_leg]);
	  }
      BOOL is_inbox_1 = is_in_nav_box(K_nav_lines.rt_legs[dto_leg].x1,K_nav_lines.rt_legs[dto_leg].y1);
	  BOOL is_inbox_2 = is_in_nav_box(K_nav_lines.rt_legs[dto_leg].x2,K_nav_lines.rt_legs[dto_leg].y2);
      BOOL ret = FALSE;

      //if(mod_get_mode()==LEG_MODE)
	  //{
	     if(is_inbox_1 && is_inbox_2)
	     {
		    K_nav_lines.rt_legs[dto_leg].is_arrow=1;
		    ret=TRUE;
	     }
	     else if(!is_inbox_1 && is_inbox_2)
	     {
	        ret = set_intersection(&K_nav_lines.rt_legs[dto_leg],1);
		    K_nav_lines.rt_legs[dto_leg].is_arrow=1;
	     }
	     else if(is_inbox_1 && !is_inbox_2)
	     {
	        ret = set_intersection(&K_nav_lines.rt_legs[dto_leg],2);
		    K_nav_lines.rt_legs[dto_leg].is_arrow=0;
	     }
	     else if(!is_inbox_1 && !is_inbox_2)
	     {
	        ret = set_intersection_2(&K_nav_lines.rt_legs[dto_leg]);
		    K_nav_lines.rt_legs[dto_leg].is_arrow=0;
	     }
	  if(ret)
	  {
         K_nav_lines.rt_legs[dto_leg].n1=K_nav_lines.rt_legs[dto_leg].n2=0;
		 
		 if(nav_mode() == FPLDTO_LEG)
		    K_nav_lines.rt_legs[dto_leg].is_dto_line=2;
		 else 
            K_nav_lines.rt_legs[dto_leg].is_dto_line=1;

		 K_nav_lines.legs_count = dto_leg+1;
		 if(mod_get_mode() == OBS_MODE)
            K_nav_lines.rt_legs[dto_leg].is_arrow=0; 
	  }
      if(add_dto_line)
	  {
         // K_nav_lines.rt_legs[dto_leg+1] = arrow_obs_line;  <--- commented because 'arrow_obs_line' not initialized
		 K_nav_lines.legs_count++;
		 K_nav_lines.rt_legs[dto_leg+1].is_dto_line=2;
		 K_nav_lines.rt_legs[dto_leg+1].is_arrow=0;
	  }
   }
}

void print_super_nav5(void)
{
    rt_leg_t *__rt_leg = get_rt_leg();

	if(!__rt_leg)
	{
	   print_str(0,0,ATTRIBUTE_NORMAL,"--.- NM%-16s"," ");
	}
	else
	{
	   if(__rt_leg->DIS<100)
          print_str(0,0,ATTRIBUTE_NORMAL,"%2d.%d NM%-16s",(DWORD)__rt_leg->DIS,((DWORD)(__rt_leg->DIS*10.0f))%10," ");
	   else
          print_str(0,0,ATTRIBUTE_NORMAL,"%4d NM%-16s",(DWORD)__rt_leg->DIS," ");   
	}
//=========== ALWAYS PRINT ========================================================================================================================================
	print_str(1,0,ATTRIBUTE_NORMAL,"%-5s%-18s",__rt_leg?__rt_leg->dst_icao_id:" "," ");
	K_change_attribute(1,0,5,get_arrow_attr()); // For flashing 

	int CM = mod_get_mode();
	if(CM == LEG_MODE)
	   print_str(2,0,ATTRIBUTE_NORMAL|ATTRIBUTE_SMALL,"ENR-LEG");
	else if(CM==OBS_MODE)
		print_str(2,0,ATTRIBUTE_NORMAL|ATTRIBUTE_SMALL,"ENR:%03d",K_ftol(mod_get_obs()));
	print_str(2,7,ATTRIBUTE_NORMAL,"%-16s"," ");
	
	print_str(3,0,ATTRIBUTE_NORMAL,"%4lu KT%-16s",get_GS(1.852f)," ");

	if(l_cursored && (s5_l_mode == ON_EFV))
	{
		char *str_efv[]={"ETE    ","FLY    ","VNAV   "};
		print_str(4,0,ATTRIBUTE_INVERSE,"%s",str_efv[s5_efv]);
	}
	else
	{
		if(s5_efv == EFV_VNV)
		{
		   print_str(4,0,ATTRIBUTE_NORMAL,"V OFF  ");
		}
		if(s5_efv == EFV_FLY)
		{
		   if(!__rt_leg)
		   {
		      print_str(4,0,ATTRIBUTE_NORMAL,"--.-NM ");
		   }
		   else
		   {
			   if(fabs(__rt_leg->LBU)<1.0f)
			      print_str(4,0,ATTRIBUTE_NORMAL," .%02dNM%c",((DWORD)(fabs(__rt_leg->LBU)*100.0f))%100,__rt_leg->LBU<0?'>':'<');
			   else
			      print_str(4,0,ATTRIBUTE_NORMAL,"%2d.%dNM%c",fabs(__rt_leg->LBU)>99.9f?(DWORD)99:(DWORD)fabs(__rt_leg->LBU),
                               ((DWORD)(fabs(__rt_leg->LBU)*10.0f))%10,__rt_leg->LBU<0?'>':'<');
		   }	
		}
		if(s5_efv == EFV_ETE)
		{
		   if(get_GS(1.852f)>2 && __rt_leg)
		      print_str(4,3,ATTRIBUTE_NORMAL,"%1d:%02d",(DWORD)__rt_leg->ETE>9?9:(DWORD)__rt_leg->ETE,(DWORD)((__rt_leg->ETE - (DWORD)__rt_leg->ETE)*60.0f));
		   else
		      print_str(4,3,ATTRIBUTE_NORMAL,"-:--");
		   print_str(4,0,ATTRIBUTE_NORMAL|ATTRIBUTE_SMALL,"ETE");
		}
	}
    //DBR
	int small_attr=ATTRIBUTE_SMALL|ATTRIBUTE_NORMAL;
	int norm_attr=ATTRIBUTE_NORMAL;
	if(l_cursored && (s5_l_mode == ON_DBR))
	{
       small_attr=ATTRIBUTE_SMALL|ATTRIBUTE_INVERSE;
	   norm_attr=ATTRIBUTE_NORMAL|ATTRIBUTE_INVERSE;
	}
	if(s5_dbr==DBR_BRG)
	{
       if(__rt_leg)
	      print_str(5,3,ATTRIBUTE_NORMAL,"%03d%c",K_ftol(__rt_leg->BRG),0xB0);
	   else
	      print_str(5,3,ATTRIBUTE_NORMAL,"---%c",0xB0);
	   print_str(5,0,small_attr,"BRG");
	}
	if(s5_dbr==DBR_RAD)
	{
	   if(__rt_leg)
	   {
	      double Lat,Lon;
	      get_PPOS(&Lat,&Lon);
		  double __RAD = get_ZPU(__rt_leg->Lat_dst,__rt_leg->Lon_dst,Lat,Lon,
		  get_magdec(__rt_leg->Lat_dst,__rt_leg->Lon_dst));
		  print_str(5,3,ATTRIBUTE_NORMAL,"%03d%c",K_ftol(__RAD),0xB0);
	   }
	   else
	   {
	      print_str(5,3,ATTRIBUTE_NORMAL,"---%c",0xB0);
	   }
	   print_str(5,0,small_attr,"RAD");
    }
	if(s5_dbr==DBR_DTK)
	{
	   if(CM==OBS_MODE)
	   {
		  print_str(5,0,
			        dbrobs_mode == DBROBS_DBR ? small_attr : 
		                                        ATTRIBUTE_NORMAL|ATTRIBUTE_SMALL,"OBS");
		  print_str(5,3,
			        dbrobs_mode == DBROBS_OBS ? norm_attr :
												ATTRIBUTE_NORMAL,":%03d%-16s",mod_get_obs()," ");
          K_change_attribute(5,3,1,ATTRIBUTE_NORMAL);
	   }
	   else
	   {
	      print_str(5,0,small_attr,"%s","DTK");
	      if(!__rt_leg)
             print_str(5,3,ATTRIBUTE_NORMAL,"---%c%-16s",0xB0," ");
	      else
	         print_str(5,3,ATTRIBUTE_NORMAL,"%03d%c%-16s",K_ftol(__rt_leg->DTK),0xB0," ");
	   }
	}
	//TBR
	small_attr=ATTRIBUTE_SMALL|ATTRIBUTE_NORMAL;
	if(l_cursored && (s5_l_mode == ON_TBR))
       small_attr=ATTRIBUTE_SMALL|ATTRIBUTE_INVERSE;
    if(s5_tbr == TBR_TK)
    {
       if(get_GS(1.852f)>2)
          print_str(6,3,ATTRIBUTE_NORMAL,"%03lu%c%-16s",get_TK(),0xB0," ");
       else
	      print_str(6,3,ATTRIBUTE_NORMAL,"---%c%-16s",0xB0," ");
	   print_str(6,0,small_attr," TK",0xB0," ");
	}
	if(s5_tbr==TBR_BRG)
	{
	   if(__rt_leg)
	     print_str(6,3,ATTRIBUTE_NORMAL,"%03d%c",K_ftol(__rt_leg->BRG),0xB0);
	   else
	     print_str(6,3,ATTRIBUTE_NORMAL,"---%c",0xB0);
	   print_str(6,0,small_attr,"BRG");
    }
    if(s5_tbr==TBR_RAD)
    {
       if(__rt_leg)
	   {
	      double Lat,Lon;
		  get_PPOS(&Lat,&Lon);
		  double __RAD = get_ZPU(__rt_leg->Lat_dst,__rt_leg->Lon_dst,Lat,Lon,
		  get_magdec(__rt_leg->Lat_dst,__rt_leg->Lon_dst));
		  print_str(6,3,ATTRIBUTE_NORMAL,"%03d%c",K_ftol(__RAD),0xB0);
	   }
	   else
	   {
	      print_str(6,3,ATTRIBUTE_NORMAL,"---%c",0xB0);
	   }
	   print_str(6,0,small_attr,"RAD");
	}
	char res_str[5]={0};
	if(!nav5_resolutions[curr_nav5_res])
	{
		if(l_cursored && (s5_l_mode == ON_RES))
		   _snprintf(res_str,sizeof(res_str)-1,"AUTO");
		else
           _snprintf(res_str,sizeof(res_str)-1,"%-4d",(DWORD)nav_5_resolution);
	}
	else
        _snprintf(res_str,sizeof(res_str)-1,"%-4d",(DWORD)nav_5_resolution);
	print_str(6,8,l_cursored && (s5_l_mode == ON_RES) ? ATTRIBUTE_INVERSE : ATTRIBUTE_NORMAL,"%s",res_str);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DTO LIST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if(K_is_scanpull())
	{
	   if(K_dto_list.list_count && K_dto_list.list_index>=0)
	   {
	      K_nav_lines.is_dto_list=TRUE;
		  print_str(6,18,ATTRIBUTE_INVERSE,"%-5s",
			             ((nv_hdr_t*)K_dto_list.points[K_dto_list.list_index].buffer_spec)->ICAO_ID);
	   }
	   else
         K_nav_lines.is_dto_list=FALSE;
	}
	else
	   K_nav_lines.is_dto_list=FALSE;
	//======================================= IF RCURSOR ==============================
    if(r_cursored)
	{
		print_str(0,15,ATTRIBUTE_NORMAL,"%cVOR:%s",VOR_CHAR,get_s5_vor(s5_vor_on));
		print_str(1,15,ATTRIBUTE_NORMAL,"%cNDB:%s",NDB_CHAR,s5_ndb_on?" ON":"OFF");
		print_str(2,15,ATTRIBUTE_NORMAL,"%cAPT:%s",APT_CHAR,s5_apt_on?" ON":"OFF");
		print_str(3,19,ATTRIBUTE_NORMAL,"%s",nav5_modes[K_nav_box.curr_nav5_mode]);
		int row;
		switch(s5_r_mode)
		{
		case ON_VOR:
			{
			row=0;
			break;
			}
		case ON_NDB:
			{
			row=1;
			break;
			}
		case ON_APT:
			{
			row=2;
			break;
			}
		case ON_MODE:
			{
			row=3;
			break;
			}
		}
		K_change_attribute(row,row==3?19:20,row==3?4:3,ATTRIBUTE_NORMAL|ATTRIBUTE_INVERSE);
		char mode_val[5]={0};
		switch(K_nav_box.curr_nav5_mode)
		{
		case NAV5_MODE_TK:
			{
			   if(get_GS(1.852)>2) _snprintf(mode_val,sizeof(mode_val)-1,"%03d%c",get_TK(),0xB0); 
			   else _snprintf(mode_val,sizeof(mode_val)-1,"---%c",0xB0); 
			}
			break;
		case NAV5_MODE_DTK:
			{
			   if(__rt_leg) _snprintf(mode_val,sizeof(mode_val)-1,"%03d%c",K_ftol(__rt_leg->DTK),0xB0); 
			   else _snprintf(mode_val,sizeof(mode_val)-1,"---%c",0xB0); 
			}
			break;
		case NAV5_MODE_TN:
			{
			   _snprintf(mode_val,sizeof(mode_val)-1,"%-4s"," ");
			}
			break;
		case NAV5_MODE_HDG:
			{
			   _snprintf(mode_val,sizeof(mode_val)-1,"%03d%c",K_ftol(get_HDG()),0xB0); 
			}
			break;
		}
		print_str(3,15,ATTRIBUTE_NORMAL,"%s",mode_val);
	}
}

void print_nav5_page(int col_offset)
{
   calc_nav5_data();

   if(is_super5)
   {
      if(col_offset)
 	      return;
      print_super_nav5();
	  return;
   }

   print_str(0,col_offset,ATTRIBUTE_NORMAL,"%-11s"," ");
   print_str(1,col_offset,ATTRIBUTE_NORMAL,"%-11s"," ");
   print_str(2,col_offset,ATTRIBUTE_NORMAL,"%-11s"," ");
   print_str(3,col_offset,ATTRIBUTE_NORMAL,"%-11s"," ");
   print_str(4,col_offset,ATTRIBUTE_NORMAL,"%-11s"," ");
   if(K_nav_box.curr_nav5_mode == NAV5_MODE_TK)
   {
      if(get_GS(1.852f)>2)
		  print_str(5,col_offset,ATTRIBUTE_NORMAL,"%03d%c   %4d",(long)get_nav5_CRS(),0xB0,(DWORD)nav_5_resolution);
	  else
		  print_str(5,col_offset,ATTRIBUTE_NORMAL,"---%c   %4d",0xB0,(DWORD)nav_5_resolution);
   }
   else if(K_nav_box.curr_nav5_mode != NAV5_MODE_TN)
   {
      print_str(5,col_offset,ATTRIBUTE_NORMAL,"%03d%c   %4d",(long)get_nav5_CRS(),0xB0,(DWORD)nav_5_resolution);
   }
   else
      print_str(5,col_offset,ATTRIBUTE_NORMAL,"N^     %4d",(DWORD)nav_5_resolution);
   if(!cursored)
   {
      print_str(6,col_offset>0?18:0,ATTRIBUTE_NORMAL,"NAV 5");
   }
   else
   {
       if(on_mode)
	   {
          print_str(5,col_offset,ATTRIBUTE_NORMAL,"%s   %4d",nav5_modes[K_nav_box.curr_nav5_mode],(DWORD)nav_5_resolution);
		  K_change_attribute(5,col_offset,4,ATTRIBUTE_INVERSE);
	   }
       if(on_res)
	   {
		  K_change_attribute(5,col_offset+7,4,ATTRIBUTE_INVERSE);
	   }
	   print_str(6,col_offset>0?18:0,ATTRIBUTE_NORMAL,col_offset>0?"CRSR ":" CRSR");
	   K_change_attribute(6,col_offset>0?18:1,4,ATTRIBUTE_INVERSE);
   }
}
//======================================================================================
static int nav4_cur_row;
static int nav4_cur_col;
void nav4_handle_change_val(int ACTION)
{
   if(!r_cursored) return;
   if(ACTION==INPUT_RINNERPLUS)
   {
       if(nav4_cur_row==3||nav4_cur_row==4||nav4_cur_row==5) //SEL Alt,Dis
	   {
	      if(nav4_cur_col==12)
		  {
		     return;
		  }
		  else
		  {
		     char ch = K_get_char(nav4_cur_row,nav4_cur_col);
		     ch++;
		     if(ch>'9') ch='0';
		     K_set_char(nav4_cur_row,nav4_cur_col,ch);
		  }
	   }
   }
   else if(ACTION==INPUT_RINNERMINUS)
   {   
	   if(nav4_cur_row==3||nav4_cur_row==4||nav4_cur_row==5) //SEL Alt,Dis
	   {
	      if(nav4_cur_col==12)
		  {
		     return;
		  }
		  else
		  {
	         char ch = K_get_char(nav4_cur_row,nav4_cur_col);
	         ch--;
	         if(ch<'0') ch='9';
	         K_set_char(nav4_cur_row,nav4_cur_col,ch);
		  }
      }
   }
   int sel_H = (K_get_char(3,16)-'0')*10000+(K_get_char(3,17)-'0')*1000+(K_get_char(3,18)-'0')*100;
   int sel_DIS = (K_get_char(4,19)-'0')*10+(K_get_char(4,20)-'0');
   int sel_ANG = (K_get_char(5,19)-'0')*10+(K_get_char(5,21)-'0');
   set_SEL_altitude(sel_H);
   set_SEL_dis(sel_DIS);
   vnav_set_ang(((double)sel_ANG)/10.0f);
}
void nav4_handle_change_pos(int ACTION)
{
	if(!r_cursored) return;
	if(ACTION==INPUT_ROUTERPLUS)
	{
	   if(nav4_cur_row==3) // SEL Alt
	   {
	      if(nav4_cur_col==18)
		  {
	         if(vnav_posable())
			 {
			    nav4_cur_row++;
			    nav4_cur_col=19;
				vnav_complete_edit();
			 }
			 else
			    do_status_line(ADD_STATUS_MESSAGE,"INVALID VNV");
		  }
		  else
		     nav4_cur_col++;
	   }
	   else if(nav4_cur_row==4) // DIS
	   {
	      if(nav4_cur_col==12)
		  {
		     nav4_cur_col=19;
		  }
		  else if(nav4_cur_col==20)
		  {
		     nav4_cur_row++;
			 nav4_cur_col=19;
		  }
		  else
             nav4_cur_col++;
	   }
	   else if(nav4_cur_row==5)
	   {
	      if(nav4_cur_col==19) nav4_cur_col+=2;
	   }
	}
	else if(ACTION==INPUT_ROUTERMINUS)
	{   
		if(nav4_cur_row==3) // SEL Alt
		{
			if(nav4_cur_col==16)
			{

			}
			else
				nav4_cur_col--;
		}
		else if(nav4_cur_row==4) // DIS
		{
			if(nav4_cur_col==19)
			{
               nav4_cur_row=4;
			   nav4_cur_col=12;
			}
			else if(nav4_cur_col>19)
			   nav4_cur_col--;
		}
		else if(nav4_cur_row==5)
		{
		   if(nav4_cur_col==21) nav4_cur_col-=2;
		}
	}
}
//============================================================================================
void nav4_edit_done(void)
{
   if(nav4_cur_row==4 || nav4_cur_row==5)
   {
      vnav_complete_edit();
   }
}
void print_nav4_page(int col_offset)
{
   int ind_H = get_IND_altitude();
   int sel_H = get_SEL_altitude();
   int sel_DIS = get_SEL_dis();
   char *dest_pt_id=vnav_get_wpt();
   double angle=vnav_get_angle();
   int vnv_state = vnav_get_state();

   if(vnv_state==VNAV_ARMED)
   {
      double vnav_time = vnav_in_time();
	  if(vnav_time<600)
	  {
	     double mins = vnav_time/60.0f;
		 double secs = ((vnav_time/60.0f)-floor(vnav_time/60.0f))*60.0f;
		 print_str(0,col_offset,ATTRIBUTE_NORMAL,"VNV IN %d:%02d",(int)mins,(int)secs);
	  }
	  else
	     print_str(0,col_offset,ATTRIBUTE_NORMAL,"VNV ARMED  ");
   }
   else if(vnv_state==VNAV_WORKS)
       print_str(0,col_offset,ATTRIBUTE_NORMAL,"VNV %5dFT",vnav_get_ADV());
   else
      print_str(0,col_offset,ATTRIBUTE_NORMAL,"VNV INACTV ");
   print_str(1,col_offset,ATTRIBUTE_NORMAL,"%-11s"," ");
   print_str(2,col_offset,ATTRIBUTE_NORMAL,"IND %05dFT",ind_H);
   print_str(3,col_offset,ATTRIBUTE_NORMAL,"SEL:%05dFT",sel_H);
   print_str(4,col_offset,ATTRIBUTE_NORMAL,"%-5s:-%02dNM",dest_pt_id?dest_pt_id:" ",sel_DIS);
   print_str(5,col_offset,ATTRIBUTE_NORMAL,"ANGLE:%c%.1f%c",angle<0?'-':'+',fabs(angle),0xB0);
   if(col_offset>0 && r_cursored)
   {
      print_str(6,18,ATTRIBUTE_NORMAL,"CRSR ");
	  K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
	  if(nav4_cur_row==4&&nav4_cur_col==12)
	     K_change_attribute(nav4_cur_row,nav4_cur_col,5,ATTRIBUTE_INVERSE);
	  else
	     K_change_attribute(nav4_cur_row,nav4_cur_col,1,ATTRIBUTE_INVERSE);
	  if(nav4_cur_row==4)
	     vnav_set_cur_ang();
   }
   else
      print_str(6,col_offset>0?18:0,ATTRIBUTE_NORMAL,"NAV 4");
}
void print_nav3_page(int col_offset,rt_leg_t *__rt_leg)
{
   if(!__rt_leg)
   {
      print_str(0,col_offset,ATTRIBUTE_NORMAL,"           ");
	  print_str(1,col_offset,ATTRIBUTE_NORMAL,"%s   ---%c","DTK ",0xB0);
      print_str(3,col_offset,ATTRIBUTE_NORMAL,"FLY  --.-NM");
   }
   else
   {
	   print_str(0,col_offset,ATTRIBUTE_NORMAL,"%-5s>%-5s",
		   __rt_leg->src_icao_id?__rt_leg->src_icao_id:mod_get_mode()==LEG_MODE && __rt_leg->leg_crs!=OBS_CANCEL_MODE?"    \x1":"     ",
				 __rt_leg->dst_icao_id);
	   K_change_attribute(0,col_offset+5,1,get_arrow_attr());
	   print_str(1,col_offset,ATTRIBUTE_NORMAL,"%s   %03d%c",mod_get_mode()==LEG_MODE?"DTK ":"OBS:",mod_get_mode()==LEG_MODE?K_ftol(__rt_leg->DTK):mod_get_obs(),0xB0);
	   if(fabs(__rt_leg->LBU)>99.9)
	   {
		   print_str(3,col_offset,ATTRIBUTE_NORMAL,"FLY %c%4dNM",
			   __rt_leg->LBU<0?'R':'L',(LONG)fabs(__rt_leg->LBU));
	   }
	   else
	   {
	    print_str(3,col_offset,ATTRIBUTE_NORMAL,"FLY %c%2d.%dNM",
		         __rt_leg->LBU<0?'R':'L',(LONG)fabs(__rt_leg->LBU),
		   ((LONG)(fabs(__rt_leg->LBU)*10.0f))%10 );
	   }

   }
   if(get_GS(1.852f)>2)
      print_str(2,col_offset,ATTRIBUTE_NORMAL,"TK     %.3lu%c",get_TK(),0xB0);
   else
	  print_str(2,col_offset,ATTRIBUTE_NORMAL,"TK     ---%c",0xB0);

   print_str(4,col_offset,ATTRIBUTE_NORMAL,"MSA -----FT");
   print_str(5,col_offset,ATTRIBUTE_NORMAL,"ESA -----FT");
   if(col_offset && s3_r_cursored)
   {
      print_str(6,18,ATTRIBUTE_INVERSE,"CRSR ");
	  K_change_attribute(6,22,1,ATTRIBUTE_NORMAL);
	  K_change_attribute(1,19,3,ATTRIBUTE_INVERSE);
   }
   else if(!col_offset && s3_l_cursored)
   {
	   print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
	   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
	   K_change_attribute(1,7,3,ATTRIBUTE_INVERSE);
   }
   else
      print_str(6,col_offset>0?18:0,ATTRIBUTE_NORMAL,"NAV 3");
}
void print_super_nav1(rt_leg_t *__rt_leg)
{
	if(!__rt_leg)
	{
      print_str(0,0,ATTRIBUTE_NORMAL,"%-23s"," "); 
      print_str(3,0,ATTRIBUTE_NORMAL,"DIS  --.-NM   ETE --:--");
      print_str(4,0,ATTRIBUTE_NORMAL,"GS    %3dKT   BRG  ---%c",get_GS(1.852),0xB0);
	}
	else
	{
		print_str(0,0,ATTRIBUTE_NORMAL,"%-6s%-5s>%-5s%-6s"," ",
			__rt_leg->src_icao_id?__rt_leg->src_icao_id:mod_get_mode()==LEG_MODE && __rt_leg->leg_crs!=OBS_CANCEL_MODE?"    \x1":"     ",
				  __rt_leg->dst_icao_id," "); 
		K_change_attribute(0,11,1,get_arrow_attr());
		if(__rt_leg->DIS<100)
      	   print_str(3,0,ATTRIBUTE_NORMAL,"DIS  %2d.%dNM",
		             (DWORD)__rt_leg->DIS,
					 ((DWORD)(__rt_leg->DIS*10.0f))%10);
		else
           print_str(3,0,ATTRIBUTE_NORMAL,"DIS%6dNM",(DWORD)__rt_leg->DIS);
		if(get_GS(1.852f)>2)
		{
		   print_str(3,11,ATTRIBUTE_NORMAL,"   ETE %2d:%02d",
			         (DWORD)__rt_leg->ETE>99?99:(DWORD)__rt_leg->ETE,
					 (DWORD)((__rt_leg->ETE - (DWORD)__rt_leg->ETE)*60.0f));
		}
		else
		{
           print_str(3,11,ATTRIBUTE_NORMAL,"   ETE --:--");
		}
		print_str(4,0,ATTRIBUTE_NORMAL,"GS    %3luKT   BRG  %03d%c",get_GS(1.852f),round_deg(__rt_leg->BRG),0xB0);
	}

	print_str(5,0,ATTRIBUTE_NORMAL,"%-23s"," ");
	print_str(1,0,ATTRIBUTE_NORMAL,"%-23s"," ");
	print_str(2,0,ATTRIBUTE_NORMAL,"%-23s"," ");

	print_str(6,0,ATTRIBUTE_NORMAL,"NAV 1");
	print_str(6,18,ATTRIBUTE_NORMAL,"NAV 1");
}

void print_nav1_page(int col_offset,rt_leg_t *__rt_leg,int is_super)
{
   if(is_super1)
   {
      if(col_offset)
 	      return;
      print_super_nav1(__rt_leg);
	  return;
   }
	if(!__rt_leg)
	{
      print_str(0,col_offset,ATTRIBUTE_NORMAL,"           "); 
	  print_str(1,col_offset,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL,"+++++ +++++");
      print_str(1,col_offset+2,ATTRIBUTE_INVERSE,"F L A G");
      print_str(2,col_offset,ATTRIBUTE_NORMAL,"DIS  --.-NM");
      print_str(4,col_offset,ATTRIBUTE_NORMAL,"ETE   --:--");
      print_str(5,col_offset,ATTRIBUTE_NORMAL,"BRG    ---%c",0xB0);
	}
	else
	{
		print_str(0,col_offset,ATTRIBUTE_NORMAL,"%-5s>%-5s",
			__rt_leg->src_icao_id?__rt_leg->src_icao_id:mod_get_mode()==LEG_MODE && __rt_leg->leg_crs!=OBS_CANCEL_MODE?"    \x1":"     ",
				  __rt_leg->dst_icao_id); 
		K_change_attribute(0,col_offset+5,1,get_arrow_attr());
		if(__rt_leg->DIS<100)
      	   print_str(2,col_offset,ATTRIBUTE_NORMAL,"DIS  %2d.%dNM",
		             (DWORD)__rt_leg->DIS,
					 ((DWORD)(__rt_leg->DIS*10.0f))%10);
		else
           print_str(2,col_offset,ATTRIBUTE_NORMAL,"DIS%6dNM",(DWORD)__rt_leg->DIS);
		if(get_GS(1.852f)>2)
		{
		   print_str(4,col_offset,ATTRIBUTE_NORMAL,"ETE   %2d:%02d",
			         (DWORD)__rt_leg->ETE>99?99:(DWORD)__rt_leg->ETE,
					 (DWORD)((__rt_leg->ETE - (DWORD)__rt_leg->ETE)*60.0f));
		}
		else
		{
           print_str(4,col_offset,ATTRIBUTE_NORMAL,"ETE   --:--");
		}
		print_str(5,col_offset,ATTRIBUTE_NORMAL,"BRG    %03d%c",round_deg(__rt_leg->BRG),0xB0);

		print_str(1,col_offset,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL|ATTRIBUTE_TRANSPARENT,"+++++ +++++");
		K_set_char(1,col_offset+5,__rt_leg?fabs(__add_deg180(__rt_leg->BRG,-get_TK2()))<=90.0f?TO_CHAR:FROM_CHAR:FROM_CHAR);
		K_change_attribute(1,col_offset+5,1,ATTRIBUTE_NORMAL);

	}
	print_str(3,col_offset,ATTRIBUTE_NORMAL,"GS    %3luKT",get_GS(1.852f));
	print_str(6,col_offset>0?18:0,ATTRIBUTE_NORMAL,"NAV 1");
}
void show_nav_page(int page_number,int page_type)
{

	int col_offset   = page_type == PAGE_LEFT ? 0 : 12;
	int col_offset_s = page_type == PAGE_LEFT ? 0 : 18;
    rt_leg_t *__rt_leg = get_rt_leg();

	switch(page_number)
	{
	case 1:
		{
		   print_nav1_page(col_offset,__rt_leg,0);
		   update_screen();
		}
		break;
	case 2:
		{
		   print_str(0,col_offset,ATTRIBUTE_NORMAL,"PRESENT POS");
		   print_str(1,col_offset,ATTRIBUTE_NORMAL,"           ");
		   FLOAT64 long_deg;
		   FLOAT64 lat_deg;
		   K_deg deg;
		   get_PPOS(&lat_deg,&long_deg);

	       if(nr_vor_l.nr_vor_count>0)
		   {
			  print_str(2,col_offset,ATTRIBUTE_NORMAL,"%3s  %03d%c%s",
			            nav_db.vors[nr_vor_l.list[0].index].ICAO_ID,
						//(DWORD)nr_vor_l.list[0].radial,
						K_ftol(get_ZPU(nav_db.vors[nr_vor_l.list[0].index].Lat,nav_db.vors[nr_vor_l.list[0].index].Lon,lat_deg,long_deg,get_magdec(nav_db.vors[nr_vor_l.list[0].index].Lat,nav_db.vors[nr_vor_l.list[0].index].Lon))),
						0xB0,
						//fabs(__add_deg180(get_TK2(),-nr_vor_l.list[0].radial))<=90.0f?"TO":"FR"
						"FR"
						);
		      print_str(3,col_offset,ATTRIBUTE_NORMAL,"    %03d.%dNM",
				        (DWORD)(nr_vor_l.list[0].S/1.852f),
						((DWORD)(nr_vor_l.list[0].S/1.852f*10.0f))%10
						);
		   }
		   else
		   {
	          print_str(2,col_offset,ATTRIBUTE_NORMAL,"---  ---%cFR",0xB0);
		      print_str(3,col_offset,ATTRIBUTE_NORMAL, "   ----.-NM");
		   }
     	   K_GetDegMin(lat_deg,&deg);
		   print_str(4,col_offset,ATTRIBUTE_NORMAL,"%c %.2d%c%.2d.%.2d'",lat_deg<0?'S':'N',(int)fabs(lat_deg),0xB0,deg.mins,deg.dec_mins);
		   K_GetDegMin(long_deg,&deg);
           print_str(5,col_offset,ATTRIBUTE_NORMAL,"%c%.3d%c%.2d.%.2d'",long_deg<0?'W':'E',(int)fabs(long_deg),0xB0,deg.mins,deg.dec_mins);
		
		   print_str(6,col_offset_s,ATTRIBUTE_NORMAL,"NAV 2");
		   update_screen();
		}
		break;
	case 3:
		{
		   print_nav3_page(col_offset,__rt_leg);
		   update_screen();
		}
		break;
	case 4:
		{
		   print_nav4_page(col_offset);
		   update_screen();
		}
		break;
	case 5:
		{
		   print_nav5_page(col_offset);
		   update_screen();
		}
		break;
	}
}
extern int SCREEN_FORMAT;
void check_super(void)
{
	if(nav_last_page == right_page_number && nav_last_page == left_page_number
		&& get_lp_type() == NAV_PAGE && get_rp_type() == NAV_PAGE)
	{
       is_super5 = TRUE;
       SCREEN_FORMAT = FORMAT_BLACK;
	   cursored=0;
	}
	else if(nav_first_page == right_page_number && nav_first_page == left_page_number
		   && get_lp_type() == NAV_PAGE && get_rp_type() == NAV_PAGE)
	{
       is_super1 = TRUE;   
       SCREEN_FORMAT = FORMAT_ONEPART;
	   cursored=0;
	}
	else
	{
	  if(get_lp_type() != MSG_PAGE)
	     SCREEN_FORMAT = FORMAT_TWOPARTS;
	  is_super5 = FALSE;
	  is_super1 = FALSE;
	  //print_str(6,5,ATTRIBUTE_NORMAL," ENR-LEG     ");
	  print_str(0,11,ATTRIBUTE_NORMAL," ");
	  curr_nav5_res = nav5res_index(nav_5_resolution);
	}
}

int get_super_type(void)
{
   if(is_super5)
	  return(SUPER_NAV5);
   if(is_super1)
      return(SUPER_NAV1);
   return(SUPER_NONE);
}

void do_nav_page(int ACTION,int page_type)
{
	if(ACTION == ACTION_INIT || ACTION == ACTION_FREE_RESOURCES)
	{
	    cursored=on_mode=on_res=r_cursored=l_cursored=0;
		NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_CLR|INPUT_ENTER;
		NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR;
		is_super5 = is_super1 = FALSE;
		s5_r_mode = ON_VOR; s5_l_mode=ON_RES;
        s5_ndb_on=s5_vor_on=s5_apt_on=0;
		s5_efv=s5_dbr=s5_tbr=s3_r_cursored=s3_l_cursored=0;
		s5_nearby_vis = 1;
		left_page_number = 2;
		right_page_number = 2;
		return;
	}
	if(ACTION == ACTION_TIMER)
	{
	    if(is_super5 && !nav5_resolutions[curr_nav5_res])
		{
		   rt_leg_t *__rt = get_rt_leg();
		   if(__rt)
              set_near_resolution(__rt);
		}
		if(s3_l_cursored && mod_get_mode()!=OBS_MODE && left_page_number==3)
		{
			s3_l_cursored=0;
			NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_CLR|INPUT_ENTER;
		}
		if(s3_r_cursored && mod_get_mode()!=OBS_MODE && right_page_number == 3)
		{
			s3_r_cursored=0;
			NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR|INPUT_CLR|INPUT_ENTER;
		}
		if(page_type==PAGE_LEFT)
		   show_nav_page(left_page_number,PAGE_LEFT);
		if(page_type==PAGE_RIGHT)
		   show_nav_page(right_page_number,PAGE_RIGHT);
		return;
	}
	if(ACTION == INPUT_CLR)
	{
	   if(is_super5)
		   s5_nearby_vis=1-s5_nearby_vis;
	}
    if(ACTION == INPUT_LCURSOR)
	{
		if(is_super5)
		{
			l_cursored=1-l_cursored;
			if(l_cursored)
			{
				NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_LOUTERPLUS|INPUT_LOUTERMINUS|INPUT_CLR|INPUT_ENTER;
				s5_l_mode = ON_RES;
			}
			else
				NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_CLR|INPUT_ENTER;
			return;
	   }
		if((mod_get_mode() == OBS_MODE) && (left_page_number == 3))
		{
			s3_l_cursored=1-s3_l_cursored;
			if(s3_l_cursored)
				NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_LOUTERPLUS|INPUT_LOUTERMINUS|INPUT_CLR|INPUT_ENTER;
			else
				NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_CLR|INPUT_ENTER;
			return;
		}
		if( left_page_number != nav_last_page )
	      return;
	   if(!cursored)
	   {
	       cursored=1;
		   on_mode = 0;
		   on_res = 1;
		   NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_LOUTERPLUS|INPUT_LOUTERMINUS|INPUT_CLR|INPUT_ENTER;
	   }
	   else
	   {
	       cursored=on_mode=on_res = 0;
		   NAV_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_LCURSOR|INPUT_CLR|INPUT_ENTER;
	   }
	}
    if(ACTION == INPUT_RCURSOR)
	{
	   if(is_super5)
	   {
		   r_cursored=1-r_cursored;
		   if(r_cursored)
		   {
              NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS;
			  s5_r_mode = ON_VOR;		  
		   }
		   else
              NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR;
	       return;
	   }
       if((mod_get_mode() == OBS_MODE) && (right_page_number == 3))
	   {
	      s3_r_cursored=1-s3_r_cursored;
		  if(s3_r_cursored)
			  NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS|INPUT_CLR|INPUT_ENTER;
		  else
			  NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR|INPUT_CLR|INPUT_ENTER;
		  return;
	   }
	   if(right_page_number == 4)
	   {
		   r_cursored=1-r_cursored;
		   if(r_cursored)
		   {
		      NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS;
			  nav4_cur_row=3;
			  nav4_cur_col=12+4;
		   }
		   else
		   {
		      nav4_edit_done();
		      NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR;
		   }
		   return;
	   }
	   if( right_page_number != nav_last_page )
	      return;
	   if(!cursored)
	   {
	       cursored=1;
		   on_mode = 0;
		   on_res = 1;
		   NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS;
	   }
	   else
	   {
	       cursored=on_mode=on_res = 0;
		   NAV2_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_RCURSOR;
	   }
	}

	if(ACTION == INPUT_LINNERPLUS)
	{
		if(is_super5)
		{
			if(l_cursored)
			{
				s5_lcur_mode_next();
				return;
			}

		}
       if(s3_l_cursored && left_page_number==3)
	   {
	      obs_next_val();	  
		  return;
	   }
	   if(!cursored || (cursored && left_page_number!=5))
	   {
          left_page_number=inc_nav_page(left_page_number);	   
		  check_super();
	   }
	   else
	   {
	      if(on_mode)
			  nav5_mode_next();
		  if(on_res)
			  nav5_res_next();
	   }
	}
	if(ACTION == INPUT_LINNERMINUS)
	{
		if(is_super5)
		{
			if(l_cursored)
			{
				s5_lcur_mode_prev();
				return;
			}
		}
		if(s3_l_cursored && left_page_number==3)
		{
			obs_prev_val();
			return;
		}
	    if(!cursored || (cursored && left_page_number!=5)) 
		{
           left_page_number=dec_nav_page(left_page_number);
		   check_super();
		}
		else
		{
	      if(on_mode)
			  nav5_mode_prev();
		  if(on_res)
			  nav5_res_prev();
		}
	}
	if(ACTION == INPUT_RINNERPLUS)
	{
	   if(is_super5)
	   {
	       if(K_is_scanpull())
		   {
		      dto_list_next();
			  return;
		   }
		   if(r_cursored)
		   {
		      s5_cur_mode_next();
			  return;
		   }
	   
	   }
	   if(s3_r_cursored && right_page_number==3)
	   {
		   obs_next_val();
		   return;
	   }
	   if(r_cursored && right_page_number==4)
	   {
		   nav4_handle_change_val(ACTION);
		   return;
	   }
       if(!cursored || (cursored && right_page_number!=5))
	   {
	      right_page_number=inc_nav_page(right_page_number);	   
		  check_super();
	   }
	   else
	   {
	      if(on_mode)
			  nav5_mode_next();
		  if(on_res)
			  nav5_res_next();
	   }
	}
	if(ACTION == INPUT_RINNERMINUS)
	{
	   if(is_super5)
	   {
		   if(K_is_scanpull())
		   {
		      dto_list_prev();
			  return;
		   }
		   if(r_cursored)
		   {
		      s5_cur_mode_prev();
			  return;
		   }
	   }
	   if(s3_r_cursored && right_page_number==3)
	   {
		   obs_prev_val();
		   return;
	   }
	   if(r_cursored && right_page_number==4)
	   {
		   nav4_handle_change_val(ACTION);
		   return;
	   }  
	   if(!cursored || (cursored && right_page_number!=5)) 
	   {
          right_page_number=dec_nav_page(right_page_number);
		  check_super();
	   }
	   else
	   {
	      if(on_mode)
			  nav5_mode_prev();
		  if(on_res)
			  nav5_res_prev();
		}
	}
	if(ACTION == INPUT_LOUTERPLUS || ACTION == INPUT_LOUTERMINUS )
	{
		if(is_super5)
		{
			if(l_cursored)
			{
				ACTION == INPUT_LOUTERPLUS ? s5l_mode_next(): s5l_mode_prev();
				return;
			}   
		}
		if(ACTION == INPUT_LOUTERPLUS)
		{
		   if(s3_l_cursored && left_page_number==3)
			   return;
		}
		if(ACTION == INPUT_LOUTERMINUS)
		{
			if(s3_l_cursored && left_page_number==3)
				return;
		}
		on_mode=1-on_mode;
	    on_res=1-on_res;
	}
	if(ACTION == INPUT_ROUTERPLUS || ACTION == INPUT_ROUTERMINUS )
	{   
	   if(is_super5)
	   {
	       if(r_cursored)
		   {
			  ACTION == INPUT_ROUTERPLUS ? s5_mode_next(): s5_mode_prev();
			  return;
		   }   
	   }
	   if(r_cursored && right_page_number==4)
	   {
          nav4_handle_change_pos(ACTION);
		  return;
	   }
	   if(ACTION == INPUT_ROUTERPLUS)
	   {
		   if(s3_r_cursored && right_page_number==3)
			   return;
	   }
	   if(ACTION == INPUT_ROUTERMINUS)
	   {
		   if(s3_r_cursored && right_page_number==3)
			   return;
	   }
	   on_mode=1-on_mode;
	   on_res=1-on_res;
	}
}

void set_s5_message(BOOL s5_msg)
{
   K_nav_lines.is_msg = s5_msg;
}
