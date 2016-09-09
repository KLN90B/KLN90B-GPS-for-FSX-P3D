#include "PCH.h"
#include "fslib.h"
#include "resource.h"
#include "kln90b.h"
#include <stdio.h>
#include <math.h>
//================================================================================
FLOAT64 IntGPSLbu;
FLOAT64 IntGPSLbuDiff;
FLOAT64 IntGpsPuDelta;
FLOAT64 IntDefApHDG;
BOOL IntGpsGot=0;
FLOAT64 last_dtk;
int     last_nav_mode;
static  BOOL TRANSIT;

extern FLOAT64 ExtGPSLbu;
extern FLOAT64 ExtGPSLbuDiff;
extern FLOAT64 ExtGpsPuDelta;
extern FLOAT64 ExtDefApHDG;
extern BOOL ExtGpsGot;
extern INT GLOBAL_STATE;
extern int Current_Mode;
extern DWORD interface_obsout;
extern int UseDefAP;
extern int interface_aphdg;

double Get_Bearing(double src_lat, double src_long, double dst_lat, double dst_long, double mag_var);
int CompareHeadings(int heading1, int heading2);

// Defines for the distance calculation formula
#ifndef	PI
#define		PI			3.1415926535897932384626433832795		//circle number
#endif

#define		HALFEARTH	10800				//half earth circumference in nautical miles
#define		RADIANS_TO_DEGREE_FACTOR			(180.0/PI)
#define		DEG_SIN( val )	sin( ( val)/RADIANS_TO_DEGREE_FACTOR)				//sinus in degrees
#define		DEG_COS( val )	cos( ( val)/RADIANS_TO_DEGREE_FACTOR)				//cosine in degrees
#define		DEG_TAN( val )	tan( ( val)/RADIANS_TO_DEGREE_FACTOR)				//tangens in degrees
#define		DEG_ASIN( val )	RADIANS_TO_DEGREE_FACTOR * asin((val ))				//arcsinus in degrees
#define		DEG_ACOS( val )	RADIANS_TO_DEGREE_FACTOR * acos((val ))			
//arccosine in degrees
#define		DEG_ATAN( val )	RADIANS_TO_DEGREE_FACTOR * atan((val ))				//arctangens in degrees
#define		DEG_ATAN2( val1 , val2 )	RADIANS_TO_DEGREE_FACTOR *atan2( val1, val2 )	//atan2 in degrees


#define		RADIANS_TO_MINUTE_FACTOR	(10800.0/PI)
#define		RADIANS_TO_MINUTE( val )	 (RADIANS_TO_MINUTE_FACTOR*( val ))

#define		MIN_SIN( val )	(sin( ( val)/RADIANS_TO_MINUTE_FACTOR))
#define		MIN_COS( val )	(cos( ( val)/RADIANS_TO_MINUTE_FACTOR))
#define		MIN_TAN( val )	(tan( ( val)/RADIANS_TO_MINUTE_FACTOR))
#define		MIN_ASIN( val )	(RADIANS_TO_MINUTE_FACTOR * asin((val )))
#define		MIN_ACOS( val )	(RADIANS_TO_MINUTE_FACTOR * acos((val )))
#define		MIN_ATAN( val )	(RADIANS_TO_MINUTE_FACTOR * atan((val )))
#define		MIN_ATAN2( val1 , val2 )	(RADIANS_TO_MINUTE_FACTOR *atan2( ( val1) , ( val2 ) ))

//*********************************************************************************
inline double __tg(double arg)
{
   return(sin(arg)/cos(arg));
   
}
inline double __ctg(double arg)
{
   return(cos(arg)/sin(arg));
}
inline double __cosec(double arg)
{
    return(sqrt(__ctg(arg)*__ctg(arg)+1));
}
inline double __sec(double arg)
{
    return(sqrt(__tg(arg)*__tg(arg)+1));
}
//**********************************************************************************
MODULE_VAR   north_speed_sim      = {NORTH_VELOCITY};
MODULE_VAR	 east_speed_sim       = {EAST_VELOCITY};
MODULE_VAR	 kurs_true_sim        = {PLANE_HEADING_DEGREES_TRUE};
MODULE_VAR	 kurs_mag_sim         = {PLANE_HEADING_DEGREES_MAGNETIC};
MODULE_VAR	 mag_var_sim          = {MAGNETIC_VAR};
MODULE_VAR	 longitude_sim	      = { PLANE_LONGITUDE };
MODULE_VAR	 latitude_sim         = { PLANE_LATITUDE };
MODULE_VAR   zulu_hour_sim        = { ZULU_HOUR };
MODULE_VAR   zulu_minute_sim      = { ZULU_MINUTE };
MODULE_VAR   zulu_day_sim         = { ZULU_DAY };
MODULE_VAR   zulu_year_sim        = { ZULU_YEAR };
MODULE_VAR   clock_second_sim     = { CLOCK_SECOND };
MODULE_VAR   airaft_on_ground_sim = { AIRCRAFT_ON_GROUND };
MODULE_VAR   gps_altitude         = { GPS_POSITION_ALT };
MODULE_VAR   nav_gps_switch = { GPS_DRIVES_NAV1 };
MODULE_VAR   ap_heading_hold = { AUTOPILOT_HEADING_LOCK };
MODULE_VAR   ap_active = { AUTOPILOT_ACTIVE };
MODULE_VAR   indicated_altitude = { ALT_FROM_BAROMETRIC_PRESSURE };
MODULE_VAR   baro_pressure = { KOHLSMAN_SETTING_HG };
MODULE_VAR   wind_dir = { AMBIENT_WIND_DIR };
MODULE_VAR   wind_vel = { AMBIENT_WIND_VEL };
//================================================================================================
void update_sim_vars(void)
{
   lookup_var(&north_speed_sim);
   lookup_var(&east_speed_sim);
   lookup_var(&kurs_true_sim);
   lookup_var(&kurs_mag_sim);
   lookup_var(&mag_var_sim);
   lookup_var(&longitude_sim);
   lookup_var(&latitude_sim);
   lookup_var(&zulu_hour_sim);
   lookup_var(&zulu_minute_sim);
   lookup_var(&zulu_day_sim);
   lookup_var(&zulu_year_sim);
   lookup_var(&clock_second_sim);
   lookup_var(&airaft_on_ground_sim);
   lookup_var(&gps_altitude);
   lookup_var(&nav_gps_switch);
   lookup_var(&ap_heading_hold);
   lookup_var(&ap_active);
   lookup_var(&indicated_altitude);
   lookup_var(&baro_pressure);
   lookup_var(&wind_dir);
   lookup_var(&wind_vel);
   ExtGPSLbu = IntGPSLbu;
   ExtGPSLbuDiff=IntGPSLbuDiff;
   ExtGpsPuDelta=IntGpsPuDelta;
   ExtGpsGot=IntGpsGot;
   ExtDefApHDG=IntDefApHDG;

}
void init_sim_vars(void)
{
  initialize_var(&north_speed_sim);
  initialize_var(&east_speed_sim);
  initialize_var(&kurs_true_sim);
  initialize_var(&kurs_mag_sim);
  initialize_var(&mag_var_sim);
  initialize_var(&longitude_sim);
  initialize_var(&latitude_sim);
  initialize_var(&zulu_hour_sim);
  initialize_var(&zulu_minute_sim);
  initialize_var(&zulu_day_sim);
  initialize_var(&zulu_year_sim);
  initialize_var(&clock_second_sim);
  initialize_var(&airaft_on_ground_sim);
  initialize_var(&gps_altitude);
  initialize_var(&nav_gps_switch);
  initialize_var(&ap_heading_hold);
  initialize_var(&ap_active);
  initialize_var(&indicated_altitude);
  initialize_var(&baro_pressure);
  initialize_var(&wind_dir);
  initialize_var(&wind_vel);
}
//================================================================================================
double get_ZPU(double src_lat,double src_long,double dst_lat,double dst_long,double mag_var)
{
double zpu;
double az_delta_long;
double az_delta_lat;
double az_ctg;

az_delta_long = dst_long - src_long;
if(az_delta_long > 180)
	az_delta_long = az_delta_long - 360;
if(az_delta_long < -180)
	az_delta_long = az_delta_long + 360;
if(az_delta_long == 0)
	az_delta_long = 0.0001;

	az_delta_lat = dst_lat - src_lat;
	///ЗПУ
	az_ctg = (cos(src_lat/57.29578)*tan(dst_lat/57.295779) - sin(src_lat/57.29578)*cos(az_delta_long/57.295779))/sin(az_delta_long/57.295779);
		if(az_ctg > 1000000)
			az_ctg = 1000000;
		if(az_ctg < -1000000)
			az_ctg = -1000000;
	if((az_delta_long < 0 && az_ctg > 0) || (az_delta_long > 0 && az_ctg < 0))
		zpu = atan(1/az_ctg)* 57.295779 + 180;
	else
		zpu = atan(1/az_ctg)* 57.295779;
	//=====
    zpu-=mag_var;
	if(zpu > 360)
		zpu = zpu - 360;
	if(zpu < 0)
		zpu = zpu + 360;		//Истинный ЗПУ в градусах
    return(zpu);
}
double get_S(double src_lat,double src_long,double dst_lat,double dst_long)
{
double az_delta_long;
double az_cos;
double kpu_daln;
az_delta_long = src_long - dst_long;
if(az_delta_long > 180)
	az_delta_long = az_delta_long - 360;
if(az_delta_long < -180)
	az_delta_long = az_delta_long + 360;
if(az_delta_long == 0)
	az_delta_long = 0.0001;

az_cos = sin(dst_lat/57.29578)*sin(src_lat/57.29578) + cos(dst_lat/57.29578)*cos(src_lat/57.29578)*cos(az_delta_long/57.295779);
kpu_daln = acos(az_cos)*57.29578 * 60 * 1.853;
	if(kpu_daln < 0)
		kpu_daln = -kpu_daln;		//Дальность в километрах
return kpu_daln;
}
double get_GPS_altitude(void)
{
   return(gps_altitude.var_value.n);
}

int get_ap_active(void)
{
	return (ap_active.var_value.n);
}
int get_nav_gps_switch(void)
{
	return (nav_gps_switch.var_value.n);
}
int get_ap_heading_hold(void)
{
	return (ap_heading_hold.var_value.n);
}
double get_baro_press(void)
{
	return (baro_pressure.var_value.n);
}
double get_indicated_altitude(void)
{
	return (indicated_altitude.var_value.n);
}
double get_wind_dir(void)
{
	return (wind_dir.var_value.n);
}
double get_wind_vel(void)
{
	return (wind_vel.var_value.n);
}


double get_TRUE_COURSE(void)
{
   double kurs_true;
   GET_SIM_VAL(kurs_true_sim,kurs_true);
   if(kurs_true > 360)
	  return(kurs_true-360);
   if(kurs_true < 0)
  	  return(kurs_true+360);
   return(kurs_true);
}
double get_W(void)
{
   double north_speed,east_speed;

   GET_SIM_VAL(east_speed_sim,east_speed);
   east_speed*=0.3048*3.6;
   GET_SIM_VAL(north_speed_sim,north_speed);
   north_speed*=0.3048*3.6;

   return(pow((north_speed*north_speed + east_speed*east_speed),0.5));
}
double get_IPU(double w)
{
   double pu_true,east_speed,north_speed;

   GET_SIM_VAL(east_speed_sim,east_speed);
   east_speed*=0.3048*3.6;
   GET_SIM_VAL(north_speed_sim,north_speed);
   north_speed*=0.3048*3.6;

   if(w > 1)
   {
      pu_true = asin(east_speed/w)*57.295779;
	  if(north_speed < 0)
	     pu_true = 180 - pu_true;
	  if(pu_true > 360)
	     pu_true = pu_true - 360;
	  if(pu_true < 0)
	     pu_true = pu_true + 360;

   }
   else
	   pu_true=get_TCRS();
   return(pu_true);
}
double get_US(void)
{
   double w = get_W();
   double kurs_true = get_TRUE_COURSE();
   double pu_true = get_IPU(w);
   double us;
   if(w > 10)
   {
      us = pu_true - kurs_true;
	  if(us > 180)
	     us = us - 360;
	  if(us < -180)
	     us = us + 360;
   }
   else
      us=0;
   return(us);
}
double get_HDG(void)
{
   return(kurs_mag_sim.var_value.n);
}
double get_TCRS(void)
{
   return(kurs_true_sim.var_value.n);
}
LONG get_TK(void)
{
   LONG TK = (LONG)((get_HDG() + get_US()) * 100.0f);
   TK = (abs(TK) % 100) >= 50 ? (TK/100)+1 : TK/100;
   if(TK < 0 ) return(360+TK);
   if(TK > 360 ) TK = TK - 360;
   return(TK);
}
FLOAT64 get_TK2(void)
{
   FLOAT64 TK = get_HDG() + get_US();
   if(TK < 0.0f ) return(360.0f+TK);
   if(TK > 360.0f ) return(TK - 360.0f);
   return(TK);
}

LONG get_GS(double divisor)
{
   LONG GS = (LONG)(get_W()/divisor*100.0f);
   return((abs(GS) % 100) >= 50 ? (GS/100)+1 : GS/100);
}
double get_LBU(double src_lat,double src_long,double dst_lat,double dst_long,double craft_lat,double craft_long,double *delta,double *SS)
{
double kpu_daln;
double zpu_obr;
double zpu_obr_sam;
double lbu;
double zpu_obr_delta;

//Расстояние от конечного пункта маршрута до самолета
kpu_daln = get_S(craft_lat,craft_long,dst_lat,dst_long);
//Обратный путевой угол
zpu_obr = get_ZPU(dst_lat,dst_long,src_lat,src_long,0);
//Угол с конечной точки на самолет
zpu_obr_sam = get_ZPU(dst_lat,dst_long,craft_lat,craft_long,0);
//Угловое отклонение от ЗПУ
zpu_obr_delta = zpu_obr_sam - zpu_obr;
if(zpu_obr_delta > 180)
	zpu_obr_delta = zpu_obr_delta - 360;
if(zpu_obr_delta < -180)
	zpu_obr_delta = zpu_obr_delta + 360;
lbu = -kpu_daln*sin(zpu_obr_delta/57.295779);	//Боковое уклонение в километрах

if(delta)
   *delta = zpu_obr_delta;
if(SS)
   *SS = kpu_daln;
return lbu;
}

double get_LBU2(double src_lat,double src_long,double dst_lat,double dst_long,double craft_lat,double craft_long,double *delta,double *SS)
{
double kpu_daln;
double zpu_obr;
double zpu_obr_sam;
double lbu;
double zpu_obr_delta;

//Расстояние от конечного пункта маршрута до самолета
kpu_daln = get_S(craft_lat,craft_long,dst_lat,dst_long);
//Обратный путевой угол
zpu_obr = get_ZPU(dst_lat,dst_long,src_lat,src_long,0);
//Угол с конечной точки на самолет
zpu_obr_sam = get_ZPU(dst_lat,dst_long,craft_lat,craft_long,0);
//Угловое отклонение от ЗПУ
zpu_obr_delta = zpu_obr_sam - zpu_obr;
if(zpu_obr_delta > 180)
	zpu_obr_delta = zpu_obr_delta - 360;
if(zpu_obr_delta < -180)
	zpu_obr_delta = zpu_obr_delta + 360;
lbu = -kpu_daln*sin(zpu_obr_delta/57.295779);	//Боковое уклонение в километрах

if(delta)
   *delta = zpu_obr_delta;
if(SS)
   *SS = kpu_daln;
return lbu;
}

double get_obs_LBU(double dst_lat,double dst_long,double craft_lat,double craft_long,double OBS,double BRG)
{
	double kpu_daln;
	double zpu_obr;
	double zpu_obr_sam;
	double lbu;
	double zpu_obr_delta;

	//Расстояние от конечного пункта маршрута до самолета
	kpu_daln = get_S(craft_lat,craft_long,dst_lat,dst_long);
	//Обратный путевой угол
	zpu_obr = __add_deg(OBS,180.0f);
	//Угол с конечной точки на самолет
	zpu_obr_sam = get_ZPU(dst_lat,dst_long,craft_lat,craft_long,get_magdec(dst_long,dst_lat));//BRG;
	//Угловое отклонение от ЗПУ
	zpu_obr_delta = zpu_obr_sam - zpu_obr;
	if(zpu_obr_delta > 180)
		zpu_obr_delta = zpu_obr_delta - 360;
	if(zpu_obr_delta < -180)
		zpu_obr_delta = zpu_obr_delta + 360;
	lbu = -kpu_daln*sin(zpu_obr_delta/57.295779);	//Боковое уклонение в километрах

	return lbu;
}


BOOL on_the_ground(void)
{
  int gr;
  GET_SIM_VAL(airaft_on_ground_sim,gr);
  return(gr);
}

double get_MAGVAR(void)
{
    double __lat = latitude_sim.var_value.n/40007000.0f*360.0f;
	if(__lat>74.0f || __lat<-60.0f)
		return(0.0f);
	return(mag_var_sim.var_value.n);
}

void get_PPOS(double *latitude,double *longitude)
{
    GET_SIM_VAL(longitude_sim,*longitude);
    GET_SIM_VAL(latitude_sim,*latitude);
	*longitude=*longitude/281474976710656.0f*360.0f;
	*latitude=*latitude/40007000.0f*360.0f;
}

K_dite_time_t *calc_flying_time(int time_type=0)
{
   static BOOL is_gs_useable=FALSE;
   static K_dite_time_t fl_time;
   static K_dite_time_t dep_time;
   static DWORD last_calc=0;
   if(get_GS(1.852)<30)
   {
	   is_gs_useable=FALSE;
	   fl_time.hour=0; fl_time.min=0; fl_time.sec=0;
	   return(NULL);
   }
   static K_dite_time_t last;
   if(!is_gs_useable)
   {
	   is_gs_useable=TRUE;
	   fl_time.hour=0; fl_time.min=0; fl_time.sec=0;
	   get_DT(&last);
	   dep_time=last;
	   return(time_type == 1 ? &dep_time : &fl_time);
   }

   K_dite_time_t now;
   get_DT(&now);

   if((now.hour!=last.hour) || (now.min!=last.min) || (now.sec!=last.sec))
   {
	  K_dite_time_t *__subtm = sub_DT(&now,&last);
      int overflow=0;
      if(fl_time.sec+__subtm->sec>59)
      {
	     overflow=1;
	     fl_time.sec = (fl_time.sec+__subtm->sec)-60;
      }
      else
	     fl_time.sec = fl_time.sec+__subtm->sec;

	  fl_time.min+=overflow;
      if((fl_time.min+__subtm->min)>59)
      {
         overflow=1;
         fl_time.min = (fl_time.min+__subtm->min)-60;
      }
      else
      {
        overflow=0; 
        fl_time.min = fl_time.min+__subtm->min;
      }
      fl_time.hour+=__subtm->hour+overflow;
      if(fl_time.hour>99) fl_time.hour=99;
      last=now;
   }
   return(time_type == 1 ? &dep_time : &fl_time);
}
//======================================================================
static rt_leg_t rt_leg;
static int is_route = 0;
static int dto_active    = 0;
static int fpl_active    = 0;
static int fpldto_active = 0;
//========================= Default Autopilot =========================
void ApDefCalc(FLOAT64 __lbu, FLOAT64 __dis, FLOAT64 __dtk, double Lat_dst, double Lon_dst)
{

	double Lat, Lon, track, winddir, windvel, bear;
	int diffleft, diffright;

	FLOAT64 dis = __dis - 10.0f < 0 ? __dis : 10.0f;
	if (dis > __dis) dis = __dis;
	if (dis < 0.0001)
		dis = 0.0001;

	FLOAT64 res_hdg = __dtk - atan(__lbu / dis*(180.0f / 3.1415f)) - get_US();
	if (res_hdg > 360) res_hdg = res_hdg - 360.0f;
	if (res_hdg < 0) res_hdg = res_hdg + 360.0f;

	// at this point 'res_hdg' is the track we want to stay on if we are already on course.

	get_PPOS(&Lat, &Lon);
	track = get_TK();

	// Get the no-wind course to the next WP
	bear = Get_Bearing(Lat, Lon, Lat_dst, Lon_dst, get_MAGVAR());

	// Adjust it to intercept
	double IA = bear - __dtk;
	if (abs(IA) < 90 && __dis > 11) {
		if (abs(IA) > 5) {
			if (IA > 0) IA = 25;
			if (IA < 0) IA = -25;
		}
		else {
			IA *= 3;
		}
		if (IA < -30) IA = -30;
		if (IA > 30) IA = 30;
		bear += IA;
		if (bear < 0) bear += 360;
		if (bear > 360) bear -= 360;
	}

	// Apply the wind correction angle
	winddir = get_wind_dir();
	windvel = get_wind_vel();
	double GS = get_GS(1.852f);
	double WCA = DEG_ASIN((windvel / GS) * (DEG_SIN(winddir - bear)));
	double dbear = bear;
	dbear = dbear + WCA;
	if (dbear < 0) dbear += 360;
	if (dbear > 360) dbear -= 360;
	bear = int(dbear + 0.5f);

	IntDefApHDG = bear;				// the final result
 }
//=====================================================================
rt_leg_t *get_rt_leg(void)
{
	//return (is_route && get_GS(1.852f)>30.0f) ? &rt_leg : NULL;
	return is_route  ? &rt_leg : NULL;
}

double get_DTK(rt_leg_t *track,FLOAT64 craft_Lat,FLOAT64 craft_Lon,double delta,FLOAT64 SS)
{
	double s_from_beg = track->LEN - fabs(SS*cos(delta/57.29578));
	if(s_from_beg<0) s_from_beg=0;

	double cos_S = sin(track->Lat_src/57.29578)*sin(track->Lat_dst/57.29578) + 
		           cos(track->Lat_src/57.29578)*cos(track->Lat_dst/57.29578) * 
				   cos((track->Lon_dst-track->Lon_src)/57.29578);

	double _cos_S = cos(s_from_beg/(57.29578 * 60 * 1.853)); 
	double _sin_S = sin(s_from_beg/(57.29578 * 60 * 1.853)); 
	
	double Lat = 57.29578 * asin( sin(track->Lat_src/57.29578) * 
		                          _cos_S + cos(track->Lat_src/57.29578) * _sin_S * 
								  cos(track->OD/57.29578)
								 );

	double cos_Lon_S_Lon = (_cos_S - sin(track->Lat_src/57.29578)*sin(Lat/57.29578))/
		                   (cos(track->Lat_src/57.29578)*cos(Lat/57.29578));
	if(cos_Lon_S_Lon < -1.0f) cos_Lon_S_Lon=-1.0f;
	if(cos_Lon_S_Lon > 1.0f) cos_Lon_S_Lon=1.0f;

	double Lon;

	double s_Lon = track->Lon_src<0?track->Lon_src+360.0f:track->Lon_src;
	double d_Lon = track->Lon_dst<0?track->Lon_dst+360.0f:track->Lon_dst;

	if(s_Lon < d_Lon) Lon = (acos(cos_Lon_S_Lon) + s_Lon/57.29578)*57.29578;
	else Lon = (-acos(cos_Lon_S_Lon) + s_Lon/57.29578)*57.29578;
    if(Lon>180) Lon = Lon-360.0; 
    return get_ZPU(Lat,Lon,track->Lat_dst,track->Lon_dst,get_magdec(Lon,Lat));

	//return get_ZPU(craft_Lat,craft_Lon,track->Lat_dst,track->Lon_dst,get_magdec(craft_Lon,craft_Lat));
}

void inform_dtk_changed(FLOAT64 new_dtk)
{
	
   char str[24];
   sprintf(str,"ADJ NAV IND CRS TO %03d%c",K_ftol(new_dtk),0xB0);
   msg_add_message(str);
}

BOOL is_dtos_active(void)
{
	if(dto_active || fpldto_active)
		return(TRUE);
	return(FALSE);
}
int nav_mode(void)
{
	if(dto_active)    return(DTO_LEG);
	if(fpldto_active) return(FPLDTO_LEG);
	if(fpl_active)    return(FPL_LEG);
	return(-1);
}
void nav_set_mode(int MODE)
{
	if(!is_dtos_active()) return;
	if(MODE == DTO_LEG)
	{
		dto_active=1;fpldto_active=0;
	}
	if(MODE == FPLDTO_LEG)
	{
		dto_active=0;fpldto_active=1;
	}
}

BOOL rt_eq(rt_leg_t *rt1,rt_leg_t *rt2)
{
   if(
	   (rt1->Lat_src == rt2->Lat_src)
	   &&
	   (rt1->Lon_src == rt2->Lon_src)
	   &&
	   (rt1->Lat_dst == rt2->Lat_dst)
	   &&
	   (rt1->Lon_dst == rt2->Lon_dst)
	   )
	   return(TRUE);
   return(FALSE);
}

void check_adj_nav(FLOAT64 actual_dtk)
{
	if(actual_dtk != last_dtk && last_dtk != INVALID_ANGLE && get_GS(1.852)>30)
		if(fabs(actual_dtk-last_dtk)>5)
			inform_dtk_changed(actual_dtk);
	last_dtk=actual_dtk;
}

void calc_ap_navigation(rt_leg_t *__rt)
{
	double Lat,Lon;
	get_PPOS(&Lat,&Lon);

	double __delta,__SS;

	if(mod_get_mode()==LEG_MODE && (__rt->leg_crs == LEG_MODE) )
	{
		__rt->LBU = get_LBU(__rt->Lat_src,__rt->Lon_src,__rt->Lat_dst,__rt->Lon_dst,Lat,Lon,&__delta,&__SS)/1.852f;
		__rt->DTK = get_DTK(__rt,Lat,Lon,__delta,__SS);
	}
	else if(mod_get_mode()==OBS_MODE || (__rt->leg_crs==OBS_CANCEL_MODE))
	{
		if(__rt->leg_crs!=OBS_CANCEL_MODE)
			__rt->DTK = mod_get_obs();
		__rt->LBU = get_obs_LBU(__rt->Lat_dst,__rt->Lon_dst,Lat,Lon,__rt->DTK,__rt->BRG)/1.852f;
	}

	//__rt->LBU = get_LBU(__rt->Lat_src,__rt->Lon_src,__rt->Lat_dst,__rt->Lon_dst,Lat,Lon,&__delta,&__SS)/1.852f;
	//__rt->DTK = get_DTK(__rt,Lat,Lon,__delta,__SS);
    //__rt->DIS = get_S(Lat,Lon,__rt->Lat_dst,__rt->Lon_dst)/1.852f;

	IntGpsPuDelta = get_TK2() - __rt->DTK;
	if(IntGpsPuDelta > 180)
		IntGpsPuDelta = IntGpsPuDelta - 360.0f;
	if(IntGpsPuDelta < -180)
		IntGpsPuDelta = IntGpsPuDelta + 360.0f;
	IntGPSLbu = __rt->LBU * 1.852f * 1000.0f;
	IntGPSLbuDiff = get_GS(1.852f)*sin(IntGpsPuDelta/(double)57.295779);
	ApDefCalc(__rt->LBU,__rt->DIS,__rt->DTK,__rt->Lat_dst,__rt->Lon_dst);
    // check_adj_nav(__rt->DTK);      // DO NOT do this here.  It sends spurious messages.  Should only be done on waypoint auto-advance.

//	{
//		double delta__,SS__;
//		get_LBU(__rt->Lat_src,__rt->Lon_src,__rt->Lat_dst,__rt->Lon_dst,Lat,Lon,&delta__,&SS__)/1.852f;
//		get_DTK(__rt,Lat,Lon,delta__,SS__);
//		get_S(Lat,Lon,__rt->Lat_dst,__rt->Lon_dst)/1.852f;
//	}
	last_nav_mode = nav_mode();
}

void calulate_navdata(void)
{
   DWORD last_calc=0;
   rt_leg_t __rt;

   if(!is_route) {TRANSIT=FALSE;return;}
   // Denis Okan about speed of refresh navigation data ====================================
   if(GetTickCount()-last_calc<1500)
   return;
   //=======================================================================================
   double current_dtk;
   double Lon,Lat,delta,SS;
   int CM = mod_get_mode();
   static rt_leg_t new_leg;
   static rt_leg_t old_leg;


   get_PPOS(&Lat,&Lon);

   rt_leg.DIS = get_S(Lat,Lon,rt_leg.Lat_dst,rt_leg.Lon_dst)/1.852f;
   if(rt_leg.leg_crs != LEG_MODE)
	   rt_leg.BRG = get_ZPU(Lat,Lon,rt_leg.Lat_dst,rt_leg.Lon_dst,get_magdec(rt_leg.Lon_dst,rt_leg.Lat_dst));
   else
       rt_leg.BRG = get_ZPU(Lat,Lon,rt_leg.Lat_dst,rt_leg.Lon_dst,get_MAGVAR());
   rt_leg.FT = fabs(__add_deg180(get_TK2(),-rt_leg.BRG))<=90.0f?TRUE:FALSE;
   rt_leg.ETE = rt_leg.DIS / get_GS(1.852f);

   if(CM==LEG_MODE && (rt_leg.leg_crs == LEG_MODE) )
   {
      rt_leg.LBU = get_LBU(rt_leg.Lat_src,rt_leg.Lon_src,rt_leg.Lat_dst,rt_leg.Lon_dst,Lat,Lon,&delta,&SS)/1.852f;
      rt_leg.DTK = get_DTK(&rt_leg,Lat,Lon,delta,SS);
   }
   else if(CM==OBS_MODE || (rt_leg.leg_crs==OBS_CANCEL_MODE))
   {
      if(rt_leg.leg_crs!=OBS_CANCEL_MODE)
         rt_leg.DTK = mod_get_obs();
      rt_leg.LBU = get_obs_LBU(rt_leg.Lat_dst,rt_leg.Lon_dst,Lat,Lon,rt_leg.DTK,rt_leg.BRG)/1.852f;
   }
   current_dtk = rt_leg.DTK;
   //================== For communication with ABSU ================================
   while(TRANSIT)
   {
      if(nav_mode()==DTO_LEG || mod_get_mode()!=LEG_MODE) 
	  {
		  TRANSIT=FALSE;
		  break;
	  }
	  FLOAT64 ret = fpl_get_next_leg_dtk(NULL,NULL,&__rt);     
	  if(ret == INVALID_ANGLE)
	  {
		  TRANSIT=FALSE;
		  return;
	  }
      if( rt_eq(&new_leg,&__rt) && rt_eq(&old_leg,&rt_leg) )
	  {
		  calc_ap_navigation(&new_leg);
		  if(nav_mode() == FPLDTO_LEG)
		  {
             if(fpl_get_actual_track(&__rt))
			 {
			    if(rt_eq(&new_leg,&__rt))
				{
					TRANSIT=FALSE;
					fpldto_active=fpldto_active=0;
					calculate_fpls();
					return;			
				}
			 }
			 else
			    TRANSIT=FALSE;
		  }
	  }
	  else
	  {
		  TRANSIT=FALSE;
		  return;
	  }

	  break;
   }
   if(!TRANSIT)
   {
	   calc_ap_navigation(&rt_leg);
//	   fpl_get_next_leg_dtk(NULL,NULL,NULL);
   }

   if(rt_leg.src_icao_id && rt_leg.dst_icao_id && !TRANSIT)
   {
      // We have active FPL route leg 
      rt_leg_t __rt;
      FLOAT64 n_LBU,n_DIS;
      FLOAT64 n_DTK = fpl_get_next_leg_dtk(&n_LBU,&n_DIS,&__rt);
      FLOAT64 a_DTK = rt_leg.OD;
      if(n_DTK!=INVALID_ANGLE && a_DTK!=INVALID_ANGLE )
      {
         FLOAT64 UR = n_DTK - a_DTK;
         if(UR > 180.0f) UR -=360;
         if(UR < -180) UR+=360;
         if(UR < 0) UR = -UR;
         if(UR > 110.0f) UR = 110.0f;
         FLOAT64 w = get_W();
         FLOAT64 lur = (w*w/(3.6f*3.6f*9.81f*0.354f)*tan(UR/57.295779/2) + w/3.6f*3.51f) / 1852.0f;
         if( (fabs(rt_leg.LBU)<1.0f) && (fabs(__add_deg180(get_TK2(),-rt_leg.BRG))<=90.0f) && (lur >= rt_leg.DIS))
         {          	 
			//Change route leg
            __rt.LEN = get_S(__rt.Lat_src,__rt.Lon_src,__rt.Lat_dst,__rt.Lon_dst);
			new_leg = __rt;
            old_leg = rt_leg;
			TRANSIT = TRUE;
			calc_ap_navigation(&new_leg);
			return;
         }
      }
   }
   //==============================================================================================
   if(!rt_leg.src_icao_id && rt_leg.dst_icao_id && !TRANSIT)
   {
      if(!fpldto_active) return;
	  if(rt_leg.leg_crs == OBS_MODE) return;

      if((rt_leg.leg_crs == OBS_CANCEL_MODE) && !rt_leg.FT)
	  {
         do_dto_page(ACTION_FPLDTO_DONE,0);
		 return;
	  }
	  rt_leg_t priv_rt;
      double n_DTK = fpl_get_next_leg_dtk(NULL,NULL,&priv_rt);
	  
	  if(n_DTK != INVALID_ANGLE)
	  {
		  FLOAT64 UR = n_DTK - rt_leg.OD;
		  if(UR > 180.0f) UR -=360;
		  if(UR < -180) UR+=360;
		  if(UR < 0) UR = -UR;
		  if(UR > 110.0f) UR = 110.0f;
		  FLOAT64 w = get_W();
		  FLOAT64 lur = (w*w/(3.6f*3.6f*9.81f*0.354f)*tan(UR/57.295779/2) + w/3.6f*3.51f) / 1852.0f;
		  if( (fabs(rt_leg.LBU)<1.0f) && (fabs(__add_deg180(get_TK2(),-rt_leg.BRG))<=90.0f) && (lur >= rt_leg.DIS))
		  {          	 
			  //Change route leg
			  priv_rt.LEN = get_S(priv_rt.Lat_src,priv_rt.Lon_src,priv_rt.Lat_dst,priv_rt.Lon_dst);
			  new_leg = priv_rt;
			  old_leg = rt_leg;
			  TRANSIT = TRUE;
			  calc_ap_navigation(&new_leg);
			  return;
		  }
         return;            	  
	  }

      if(!rt_leg.FT && rt_leg.DIS<=10.0f) 
         do_dto_page(ACTION_FPLDTO_DONE,0);
   }
}


void set_route_leg(nv_point_t *src_point,nv_point_t *dst_point,int leg_type)
{
	if(!src_point && !dst_point && leg_type == DTO_LEG)
	{
	   is_route=IntGpsGot=0;
	   dto_active = 0;
	   fpldto_active = 0;
	
	   //fpl_set_navigation();
	   return;
	}
	if(!src_point && !dst_point && leg_type == FPL_LEG)
	{
	   if(is_dtos_active())
		   return;
       is_route=IntGpsGot=0;
	   fpl_active = 0;
	   return;
	}
	if(!src_point && !dst_point && leg_type == FPLDTO_LEG)
	{
	   is_route=IntGpsGot=0;
	   fpldto_active = 0;
	   dto_active=0;
	   fpl_continue_navigation();
	   return;
	}
    if(leg_type == FPL_LEG)
	{
	   if(is_dtos_active())
	   {
	      if((rt_leg.leg_crs == OBS_CANCEL_MODE) && !rt_leg.FT)
		     dto_active=fpldto_active=0;
		  else
			  return;
		     
	   }
	   fpl_active=1;
	   Current_Mode = 1;
	}
	if(leg_type == DTO_LEG)
	{
	   fpl_active=0;
	   dto_active=1;
	   fpldto_active=0;
	   Current_Mode = 2;
	}
	if(leg_type == FPLDTO_LEG)
	{
	    fpl_active=0;
		fpldto_active=1;
		dto_active=0;
		fpl_set_fpldto();
		Current_Mode = 3;
	}


	rt_leg.Lon_src = ((nv_hdr_t*)src_point->buffer_spec)->Lon;
	rt_leg.Lat_src = ((nv_hdr_t*)src_point->buffer_spec)->Lat;
	rt_leg.src_icao_id = ((nv_hdr_t*)src_point->buffer_spec)->ICAO_ID[0]=='\0'?NULL:((nv_hdr_t*)src_point->buffer_spec)->ICAO_ID;
	rt_leg.dst_icao_id = ((nv_hdr_t*)dst_point->buffer_spec)->ICAO_ID;
	rt_leg.Lon_dst = ((nv_hdr_t*)dst_point->buffer_spec)->Lon;
    rt_leg.Lat_dst = ((nv_hdr_t*)dst_point->buffer_spec)->Lat;
	rt_leg.OD = get_ZPU(rt_leg.Lat_src,rt_leg.Lon_src,rt_leg.Lat_dst,rt_leg.Lon_dst,0);
	rt_leg.LEN = get_S(rt_leg.Lat_src,rt_leg.Lon_src,rt_leg.Lat_dst,rt_leg.Lon_dst);
    rt_leg.srcp = src_point;
	rt_leg.dstp = dst_point;
    rt_leg.leg_crs = mod_get_mode();
	is_route=IntGpsGot=1;
    calulate_navdata();
}
void init_navigation(void)
{
   dto_active=0;
   fpldto_active=0;
   fpl_active=0;
   is_route=IntGpsGot=0;
   last_dtk=INVALID_ANGLE;
   last_nav_mode=UNKNOWN_LEG;
   TRANSIT=FALSE;
}
BOOL get_CDI(FLOAT64 *xtk,FLOAT64 *scale)
{
   if(GLOBAL_STATE == STATE_INSELFTEST_STAGE1)
   {
      if(xtk)
	     *xtk=-2.5f;
	  if(scale)
		 *scale=5.0f;
	  return(TRUE);
   }
   if(!is_route) return(FALSE);
   if(xtk)
	   *xtk=rt_leg.LBU;
   if(scale)
	   *scale = mod_get_scale();
   return(TRUE);
}
BOOL get_FT(void)
{
   if(!is_route) return(FALSE);
   return(rt_leg.FT);
}

// ------------------------- Added by CEO to calculate course bearings -------------------------------

double	GrtCrcDist(double	*pdblInboundCourse,
	double	dblFromLat, double	dblFromLong,
	double	dblToLat, double	dblToLong);

double Get_Bearing(double src_lat, double src_long, double dst_lat, double dst_long, double mag_var)
{
	double dbld, ibc, obc, doubled;
	double magcrs;

	doubled = GrtCrcDist(&ibc, src_lat, src_long, dst_lat, dst_long);
	doubled = GrtCrcDist(&obc, dst_lat, dst_long, src_lat, src_long);
	magcrs = obc - mag_var;
	if (magcrs < 0) magcrs += 360;
	if (magcrs > 360) magcrs -= 360;
	return (magcrs);
}




// Return great circle distance:
// Input:  (lat,lon)  in degrees
// Output: returns distance in minutes (nautical miles)
//         sets parameter 1 to course in degrees


double	GrtCrcDist(double	*pdblInboundCourse,
	double	dblFromLat, double	dblFromLong,
	double	dblToLat, double	dblToLong)
{
	double	dblDist = 0, dblCourse = 180, dblDeltaTheta = 0, dblDeltaPhi = 0;

	dblDeltaPhi = (dblToLong - dblFromLong)*0.5;
	dblDeltaTheta = (dblToLat - dblFromLat)*0.5;
	dblDist = DEG_SIN(dblDeltaTheta)*DEG_SIN(dblDeltaTheta) +
		DEG_COS(dblFromLat)*DEG_COS(dblToLat)*DEG_SIN(dblDeltaPhi)*DEG_SIN(dblDeltaPhi);
	if (dblDist>0)
	{
		dblDist = sqrt(dblDist);
		if (dblDist<1)
			dblDist = 2 * MIN_ASIN(dblDist);
		else
			dblDist = HALFEARTH;
		dblCourse = DEG_ACOS(
			(DEG_SIN(dblFromLat) - DEG_SIN(dblToLat)*MIN_COS(dblDist)) /
			(DEG_COS(dblToLat)*MIN_SIN(dblDist))
			);
	}
	if (DEG_SIN(2.0*dblDeltaPhi)>0)
		dblCourse = 360 - dblCourse;
	if (pdblInboundCourse)
		*pdblInboundCourse = dblCourse;
	if (dblFromLong == dblToLong && dblCourse != dblCourse) {		// test for bug
		if (dblFromLat < dblToLat) {
			*pdblInboundCourse = 180;
		}
		else {
			*pdblInboundCourse = 0;
		}
	}
	return	dblDist;
}

// Returns the difference in degrees between two headings

int CompareHeadings(int heading1, int heading2)
{
	int diff;

	diff = 0;
	if (heading1 > heading2) {
		diff = heading1 - heading2;
		if (diff > 180) {
			diff = (heading2 + 360) - heading1;
		}
	}
	if (heading2 > heading1) {
		diff = heading2 - heading1;
		if (diff > 180) {
			diff = (heading1 + 360) - heading2;
		}
	}
	return (diff);
}