#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "vnav.h"
#include "utils.h"
#include <math.h>

static int SEL_altitude=0;
static int SEL_dis=0;
static double desired_angle;
nv_hdr_t vnav_pt_hdr;
static nv_point_t vnav_pt;
static int vnav_state=VNAV_INACTIVE;
static int advisory_H;
static double vnav_t_secs = 600.0f;

int vnav_get_ADV(void)
{
   return(advisory_H);
}
int get_IND_altitude(void)
{
   double gps_alt_foot = get_GPS_altitude()/0.3048;
   int pre_ind_H = (int)floor(gps_alt_foot);
   int last_two = pre_ind_H%100;
   pre_ind_H/=100;
   if(last_two>50) pre_ind_H++;
   return(pre_ind_H*100);
}
int get_SEL_altitude(void)
{
	return(SEL_altitude);
}
void set_SEL_altitude(int __alt)
{
	SEL_altitude=__alt;
	if(vnav_posable())
       vnav_state  = VNAV_ARMED;
	else
	   vnav_state  = VNAV_INACTIVE;
}
int get_SEL_dis(void)
{
	return(SEL_dis);
}
void set_SEL_dis(int __dis)
{
	SEL_dis=__dis;
	if(vnav_posable())
		vnav_state  = VNAV_ARMED;
	else
		vnav_state  = VNAV_INACTIVE;
}
BOOL vnav_posable(void)
{
   if(vnav_get_wpt()) return(TRUE);
	return(FALSE);
}

char *vnav_get_wpt(void)
{
   if(vnav_state!=VNAV_INACTIVE)
      return(vnav_pt_hdr.ICAO_ID);
   rt_leg_t *__rt = get_rt_leg();
   if(!__rt) return(NULL);
   return(__rt->dst_icao_id);
}

nv_hdr_t *vnav_get_navpt(void)
{
	if(vnav_state!=VNAV_INACTIVE)
       return(&vnav_pt_hdr);
	rt_leg_t *__rt = get_rt_leg();
	if(!__rt) return(NULL);
	static nv_hdr_t __tmp_hdr;
	__tmp_hdr.Lat = __rt->Lat_dst;
    __tmp_hdr.Lon = __rt->Lon_dst;
	strcpy(__tmp_hdr.ICAO_ID,__rt->dst_icao_id);
	return(&__tmp_hdr);
}

void start_vnav_calc(void)
{
    rt_leg_t *__rt = get_rt_leg();
	if(!__rt)
	{
	   vnav_state = VNAV_INACTIVE;
	   return;
	}
	strcpy(vnav_pt_hdr.ICAO_ID,__rt->dst_icao_id);
	vnav_pt_hdr.Lat = __rt->Lat_dst;
	vnav_pt_hdr.Lon = __rt->Lon_dst;
}
static double get_vnav_angle(void)
{
   double myLat,myLon;
   nv_hdr_t *nv_hdr = vnav_get_navpt();
   if(!nv_hdr) return(0.0f);
   get_PPOS(&myLat,&myLon);
   double d_H = (SEL_altitude-get_IND_altitude())*0.3048f;
   double d_S = get_S(myLat,myLon,nv_hdr->Lat,nv_hdr->Lon)*1000.0f-SEL_dis*1.852*1000;
   if(d_S<0) return(0.0f);
   double ang = d_S<1.0f ? 90.0f : FROM_RAD(atan(d_H/d_S));
   if(ang>9.9f) ang=9.9f;
   else if(ang<-9.9f) ang=-9.9f;
   return(ang);
}
double vnav_get_angle(void)
{
   if(vnav_state==VNAV_INACTIVE)
      return(get_vnav_angle());
   return(desired_angle);
}
void vnav_complete_edit(void)
{
   if(vnav_state==VNAV_INACTIVE)
      desired_angle = get_vnav_angle();
   nv_hdr_t *hdr = vnav_get_navpt();
   vnav_pt_hdr.Lat = hdr->Lat;
   vnav_pt_hdr.Lon = hdr->Lon;
   strcpy(vnav_pt_hdr.ICAO_ID,hdr->ICAO_ID);
   vnav_state    = VNAV_ARMED;
}
int vnav_get_state(void)
{
   return(vnav_state);
}
void vnav_set_cur_ang(void)
{
   desired_angle = get_vnav_angle();
   if(vnav_posable())
	   vnav_state  = VNAV_ARMED;
   else
	   vnav_state  = VNAV_INACTIVE;
}
void vnav_set_ang(double __ang)
{
	desired_angle = __ang;
	if(SEL_altitude<get_IND_altitude())
	  desired_angle=-desired_angle;
	if(vnav_posable())
	   vnav_state  = VNAV_ARMED;
	else
	   vnav_state  = VNAV_INACTIVE;
}
double vnav_in_time(void)
{
   if(vnav_state == VNAV_ARMED)
      return(vnav_t_secs);
   return(600.0f);
}
void update_vnav(void)
{
   rt_leg_t *__rt = get_rt_leg();
   if(!__rt)
      vnav_state=VNAV_INACTIVE;
   if(vnav_state!=VNAV_INACTIVE)
   {
      if(vnav_pt_hdr.Lat!=__rt->Lat_dst || vnav_pt_hdr.Lon!=__rt->Lon_dst)
         vnav_state=VNAV_INACTIVE;
   }
   if(vnav_state==VNAV_ARMED)
   {
      int cur_ang = (int)floor(get_vnav_angle()*10.0f);
	  int des_ang = (int)floor(desired_angle*10.0f);
	  if(cur_ang==des_ang) vnav_state=VNAV_WORKS;

	  double myLat,myLon;
	  get_PPOS(&myLat,&myLon);
	  nv_hdr_t *nv_hdr = vnav_get_navpt();
	  double real_S = (get_S(myLat,myLon,nv_hdr->Lat,nv_hdr->Lon)*1000.0f-SEL_dis*1.852*1000);
	  double need_S = (SEL_altitude-get_IND_altitude())*0.3048/tan(TO_RAD(desired_angle));	  
	  vnav_t_secs = 600.0f;
	  if(get_W()>20.0f)
	     vnav_t_secs = ((real_S-need_S))/(get_W()/3.6f);	  
   }
   if(vnav_state==VNAV_WORKS)
   {
	   double myLat,myLon;
	   get_PPOS(&myLat,&myLon);
	   nv_hdr_t *nv_hdr = vnav_get_navpt();
	   double d_S = (get_S(myLat,myLon,nv_hdr->Lat,nv_hdr->Lon)*1000.0f-SEL_dis*1.852*1000)/0.3048;
	   if(d_S<100.0f) 
	   {
	      vnav_state=VNAV_INACTIVE;
		  return;
	   }
	   double need_h = -d_S*tan(TO_RAD(desired_angle))+SEL_altitude;     
	   int pre_ind_H = (int)floor(need_h);
	   int last_two = pre_ind_H%100;
	   pre_ind_H/=100;
	   if(last_two>50) pre_ind_H++;
	   advisory_H = pre_ind_H*100;
   }
}