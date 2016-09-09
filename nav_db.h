#include "fslib.h"
#include <vector>
#include <map>


using namespace std;

#ifndef __NAVDB_H__
#define __NAVDB_H__
#pragma pack(1)
#pragma warning(disable:4200)

typedef struct _sub_common
{
   WORD id;
   DWORD size_of_record;
}sub_common;

typedef struct _marker
{
   sub_common header;
   BYTE heading;
   BYTE type;
   DWORD longitude;
   DWORD latitude;
   DWORD altitude;
   DWORD icao_id;
   WORD  reg_id;
   WORD  unknown1;
}marker;

typedef struct _icao_list
{
  BYTE  reg_i;
  BYTE  coun_i;
  WORD  state_i;
  WORD  city_i;
  WORD  apt_i;
  DWORD icao_id;
  DWORD unknown1;
  DWORD unknown2;
}icao_list;

typedef struct _airport
{
   sub_common header;
   BYTE  rnw_count;
   BYTE  com_count;
   BYTE  start_count;
   BYTE  approach_count;
   BYTE  apr_count_del_flag;
   BYTE  heli_count;
   DWORD longitude;
   DWORD latitude;
   DWORD altitude;
   DWORD tower_long;
   DWORD tower_lat;
   DWORD tower_alt;
   float mag_var;
   DWORD icao_id;
   DWORD unk1;
   DWORD unk2;
   BYTE unk3;
   BYTE traffic_scalar;
   WORD unk4;
} airport;

typedef struct _name_sub
{
   sub_common header;
   char  str[];
}name_sub;

typedef struct _rnw_sub
{
   sub_common header;
   WORD  surface_type;
   BYTE  rnw_number;
   BYTE  rnw_designat;
   BYTE  sec_rnw_number;
   BYTE  sec_rnw_designat;
   DWORD icao_id_pri_ils;
   DWORD icao_id_sec_ils;
   DWORD longitude;
   DWORD latitude;
   DWORD elevation;
   float lenght;
   float width;
   float heading;
   float pattern_alitude;
   WORD  marking_flags;
   BYTE  light_flags;
   BYTE  pattern_flags;
}rnw_sub;
typedef struct _sub_com
{
   sub_common header;
   WORD type;
   DWORD freq;
   char  name[];
}sub_com;
typedef struct _sub_approach
{
   sub_common header;
   BYTE       suffix;
   BYTE       rnw_number;
   BYTE       type_rnwdes_gps_flag;
   BYTE       transit_count;
   BYTE       approach_leg_count;
   BYTE       missed_approach_count;
   DWORD      fix_type_ident;
   DWORD      fix_regid_icaoid_apt;
   float      altitude;
   float      heading;
   float      missed_alitude;
}sub_approach;

typedef struct _way_point
{
  sub_common header;
  BYTE       type;
  BYTE       routes_count;
  DWORD      logitude;
  DWORD      latitude;
  float      magvar;
  DWORD      icao_id;
  DWORD      reg_icaoid;
//**************************** Optional Part
  BYTE       route_type;
  CHAR       name[8];
  DWORD      next_type_id;
  DWORD      next_reg_aptid;
  float      next_alt_min;
  DWORD      prev_type_id;
  DWORD		 prev_reg_aptid;
  float		 prev_alt_min;
}way_point;

typedef struct _bgl_header
{
    WORD bgl_id;
	WORD version;
	DWORD size_of_header;
	DWORD unknown1[3];
	DWORD section_count;
}bgl_header;

typedef struct _object_pointer
{
    DWORD section_type;
	DWORD unknown;
	DWORD sub_secs_count;
	DWORD file_offset;
	DWORD size_of_header;
}object_pointer;
typedef struct _section_header
{
  DWORD id;
  DWORD number_of_records;
  DWORD file_offset;
  DWORD size_of_subsections;
}section_header;

typedef struct _ils_vor
{
   sub_common header;
   BYTE type;
   BYTE flags;
   DWORD longitude;
   DWORD latitude;
   DWORD elevation;
   DWORD freq;
   float range;
   float mag_var;
   DWORD icao_id;
   DWORD reg_apt_icaoid;
}ils_vor;
typedef struct _ils_heading
{
	sub_common header;
	WORD unknown;
	float heading;
	float width;
}ils_heading;
typedef struct _ils_glide
{
	sub_common header;
	WORD unknown;
	DWORD longitude;
	DWORD latitude;
	DWORD elevation;
	float range;
	float pitch;
}ils_glide;

typedef struct _ilsvor_dme
{
	sub_common header;
	WORD unknown;
	DWORD longitude;
	DWORD latitude;
	DWORD elevation;
	float range;
}ilsvor_dme;

typedef struct _ndb
{
   sub_common header;
   WORD type;
   DWORD freq;
   DWORD longitude;
   DWORD latitude;
   DWORD elevation;
   float range;
   float mag_var;
   DWORD icao_id;
   DWORD reg_apt_icaoid;
}ndb;
typedef struct _names_list
{
    sub_common header;
	WORD region_names;
	WORD country_names;
	WORD state_names;
	WORD city_names;
	WORD airport_names;
	WORD icao_ids;
	DWORD region_offset;
	DWORD country_offset;
	DWORD state_offset;
	DWORD city_offset;
	DWORD airport_offset;
	DWORD icao_offset;
}names_list;

typedef struct __names_list_t
{
   char **apt_names;
   int names_count;
   char **city_names;
   int cities_count;
   char **country_names;
   int countries_count;
   char **region_names;
   int regions_count;
   icao_list *icao_list;
   int icao_ids;
}_names_list_t;

//**************** For old 2000 Bgl Style *********************************
//typedef SHORT VAR16;
//typedef INT VAR32;
typedef int SINT32;
//typedef LONG ANGL32;
typedef __int64 SINT64;
typedef GUID GUID128;

typedef struct __FS_HEADER{ // FLIGHT SIMULATOR 7.00 BGL file header
VAR16 FL_world_set; // 00 world set number (unused)
SINT32 FL_north_bounds; // 02 north bounds (NSEW bounds)
SINT32 FL_south_bounds; // 06 south bound
ANGL32 FL_east_bounds; // 10 east bound
ANGL32 FL_west_bounds; // 14 west bound
UINT32 FL_vor_ptr; // 18 VOR data
VAR16 FL_vor_lo; // 22 lowest vor freq (channel 0-199)
VAR16 FL_vor_hi; // 24 highest vor freq (108.00-117.95
UINT32 FL_free[6]; // 26 (MUST BE 0)
UINT32 FL_msa_data; // 50 Minimum Safe Altitude Data
UINT32 FL_terrain_data; // 54 new terrain data
UINT32 FL_object_ptr; // 58 OBJECT data
UINT32 FL_library_ptr; // 62 LIBRARY data
UINT32 FL_facilities_ptr; // 66 FACILITIES data
UINT32 FL_free_70; // 70 (MUST BE 0)
UINT32 FL_free_74; // 74 (MUST BE 0)
UINT32 FL_adf_data; // 78 ADF data
UINT32 FL_dynamic_data; // 82 DYNAMIC data
SINT64 FL_library_min; // 86 Library id min
SINT64 FL_library_max; // 94 Library id max
UINT32 FL_misc_data; //102 MISC data (ground alt db)
UINT32 FL_title; //106 TITLE AND DESCRIPTION
UINT32 FL_magvar; //110 MAGVAR data
UINT32 FL_exceptions; //114 EXCEPTION LIST data
UINT32 FL_magic; //118 magic number (0x87654321)
VAR32 FL_spare2; //122
VAR16 FL_spare3; //126
GUID128 FL_guid; //128 GUID
UINT32 FL_product_id; //144 PRODUCT ID
UINT32 FL_build_num; //148 PRODUCT BUILD NUMBER
UINT32 FL_ptr_count; //152 data pointers count
UINT32 FL_facility2_name_list; //156 facility2 name list
UINT32 FL_facility2_band_list; //160 facility2 latband list
UINT32 FL_facility2_data; //164 facility2 object list
} FILE_HEADER2;
//*********************************************************************************
typedef struct _band_set
{
  BYTE op_code;
  DWORD offset;
}band_set;
typedef struct _rel_band_set
{
  BYTE op_code;
  WORD lat_min;
  WORD lat_max;
  DWORD offset;
}rel_band_set;

typedef struct _old_ilsvor
{
  BYTE   type;     // Type ( ILS, VOR )
  BYTE   range;    // range in 2048M units
  WORD   magvar;   // magnetic variation in PseudoDegrees
  BYTE   code;     // code
  WORD   latlo;    // latitude of station in 2 Meter Units (low 16-bits)
  BYTE   lathi;    // hi 8-bits
  WORD   lonlo;    // longitude of station in 24-bit PseudoDegrees
  BYTE   lonhi;    // hi 8-bits
  WORD   alt;      // altitude of station in Meters
  WORD   heading;  // localizer direction (ANGL16) (0 for VORs)
  BYTE   ident[5]; 
  BYTE   name[24];
}old_ilsvor;

typedef struct _old_ILS_GS
{
  WORD latlo;    // latitude of station in 2 Meter Units (low 16-bits)
  BYTE lathi;    
  WORD lonlo;    // longitude of station in 24-bit PseudoDegrees
  BYTE lonhi;
  WORD alt;      // altitude of station in Meters
  WORD slope;    // Glideslope 2 * 65536 * sin (angle)
}old_ILS_GS;

typedef struct _old_ILS_GS2
{
   old_ILS_GS main_ils;
   WORD width_scaler;
}old_ILS_GS2;

typedef struct _old_dme
{
BYTE  code;             //DME entity opcode
DWORD FEOP_DME_MAGIC1;  //DME magic number 1
WORD  FEOP_DME_MAGIC2;  //DME magic number 2
WORD  FEOP_DME_MAGIC3;  // DME magic number 3
WORD  latlo;            //latitude of station in 2 Meter Units (low 16-bits)
BYTE  lathi;            
WORD  lonlo;            //longitude of station in 24-bit PseudoDegrees
BYTE  lonhi;
WORD  alt;              //altitude of station in Meters
}old_dme;

//======== For run-time NavDB usage =============================
enum {NVDB_NDB,NVDB_VOR,NVDB_APT,NVDB_WPT,NVDB_SUP};

typedef struct _nvdb_entry
{
    int TYPE;
	void *element;
}nvdb_entry;
typedef struct _nvdb_ndb
{
   char   ICAO_ID[6];
   double Lon;
   double Lat;
   char   REGION_ID[6];
   char   APT_ID[6];
   unsigned long FREQ;
   char   NAME[23];
}nvdb_ndb;

typedef struct _nvdb_vor
{
   char          ICAO_ID[6];
   double        Lon;
   double        Lat;
   unsigned long FREQ;
   int           TYPE; 
   char          REG_ID[6];
   char          APT_ID[6];
   char          NAME[23];
   int           DME;
   double        mag_var;
}nvdb_vor;

typedef struct _K_line_t
{
	double x1;
	double x2;
	double y1;
	double y2;
	double k;
	double b;
	int    n1;
	int    n2;
	char   n1_icao[6];
	char   n2_icao[6];
	int    is_arrow;
	int    is_dto_line;
	int    numbers_only;
	char   pDes[4];
	char   sDes[4];
}K_line_t;


typedef struct _rnw_t
{
	double bLat;
	double bLon;
	double eLat;
	double eLon;
	float  len;
	BYTE   pNum;
	BYTE   pDes;
	BYTE   sNum;
	BYTE   sDes;
	float  HDG;
	BYTE   light_flag;
	BYTE   pattern_flag;
	K_line_t apt3_line;
	char surf[4];
	char pILSicao[6];
	char sILSicao[6];
	DWORD pILS;
	DWORD sILS;
}rnw_t;

typedef struct _freq_t
{
	DWORD freq;
	int type;
}freq_t;

typedef struct _nvdb_apt
{
   char   ICAO_ID[6];
   double Lon;
   double Lat;
   double Alt;
   char   NAME[0x48];
   char   CITY[0x48];
   char   COUNTRY[0x48];
   char   REG_ID[0x48];
   int    rnws_count;
   rnw_t  *rws;
   int    freqs_count;
   freq_t  *freqs;
   double apt3_resolution;
   vector<string> *sids;
   vector<string> *stars;
   vector<string> *apps;
} nvdb_apt;

typedef struct _nvdb_wpt
{
	char   ICAO_ID[6];
	double Lon;
	double Lat;
	char   REG_ID[6]; 
	char   APT_ID[6];
	int    TYPE;
	int    ROUTES; 
	int    SUB_TYPE;
}nvdb_wpt;

#define USR_MAX_POINTS 250
#define MAIN_POINTS_COUNT (long)(nav_db.apts_count+nav_db.ndbs_count+nav_db.vors_count+nav_db.wpts_count+nav_db.sups_count)

typedef struct _nvdb_usr
{
	char   ICAO_ID[6];
	double Lon;
	double Lat;
	char   REG_ID[6]; 
	char   APT_ID[6];
	int    usr_type;
	int    padding[2]; 
}nvdb_usr;

typedef struct _earth_quad
{
	vector<int> navaids;
}earth_quad_t;

typedef struct _nav_db_t
{
   nvdb_ndb   *ndbs;
   LONG        ndbs_count;
   nvdb_vor   *vors;
   LONG        vors_count;
   nvdb_apt   *apts;
   LONG        apts_count;
   nvdb_wpt   *wpts;
   LONG        wpts_count;
   nvdb_wpt   *sups;
   LONG        sups_count;
   nvdb_usr   *usrs;
   LONG        usrs_count;
   earth_quad_t navaids[360][181];
}nav_db_t;

typedef struct _naviad_info
{
	int global_index;
	int type;
	int local_index;
}naviad_info;


#pragma pack()

const float lat_meters_degree = 40007000.0f/360.0f;
const float lon_meters_degree = 40075000.0f/360.0f;


void convert_id(unsigned long id_code,char *buffer);
void parse_wpt_group(int count,int offset,int len);
void parse_wpts(long offset,long sec_count);
void parse_apt_group(int count,int offset,int len);
void parse_airports(long offset,long apt_count);

void parse_vorils(long offset,long sec_count);
void parse_vorils_group(int count,int offset,int len);

void parse_ndb_group(int count,int offset,int len);
void parse_ndbs(long offset,long sec_count);

void parse_ilsvor(int type,long offset);
void parse_channel(float chanel_number,long chan_off,long main_offset);
void read_magdec(char *FS_MagDec_File);
float get_magdec(float Lon,float Lat);
void mav_db_main(char *FS_MainDir);
long *find_apts_by_icao(char *icao_id_s, long *array_size);

nvdb_wpt *get_sup_by_id(long id);
long get_sups_count(void);
long get_sup_actual_index(long j);
void insert_to_navaids(double Lon,double Lat,naviad_info *nv,char *icao_id);
void drop_from_navaids(double Lon,double Lat,naviad_info *nv,char *icao_id);
void init_usr_points(void);
int sup_create_new_point(char *icao_code);
BOOL sup_is_creating(void);
#endif


