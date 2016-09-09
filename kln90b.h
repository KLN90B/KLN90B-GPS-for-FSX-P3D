#ifndef __H_KLN90B__
#define __H_KLN90B__
#include "nav_db.h"

#define _CRT_SECURE_NO_DEPRECATE							// To prevent lots of warnings!
#define _CRT_SECURE_NO_WARNINGS								// These don't seem to work though in VS2013

#define LONG3(d) (long)((d)*100.0f)
#define LONG2(d) (long)((d)*10.0f)
#define LONG1(d) (long)((d)*1.0f)

#define KLN_CONFIG_NAME "kln90b.cnf"

// [KLN90B]
// TC=nnn		color of text, default 0,128,0 this specifies all 3 at once!
// TB=nnn       color of background, default is 11,11,11 this specifies all 3 at once!
// EXTINTDB=0   1 means there's an external intersection database (? where/what?)
// [DI_CONFIG]
// xxx=nnn		Direct Input atribute (Usss version only)

typedef VOID(*TGaugePlaySound)(LPTSTR, LPTSTR, int);		// SOUND
typedef VOID(*TGaugeStopSound)(LPTSTR);
typedef VOID(*TTerminateSounds)();
void PlayASound(char *sound, char *stop, int type);	

typedef struct _character
{
	COLORREF text_color;
	COLORREF text_background;
	CHAR     ch;
#define ATTRIBUTE_FLASH        0x80000000
#define ATTRIBUTE_INVERSE      0x40000000
#define ATTRIBUTE_ISSPACE      0x20000000
#define ATTRIBUTE_SMALL        0x10000000
#define ATTRIBUTE_BSMALL       0x04000000
#define ATTRIBUTE_TRANSPARENT  0x02000000
#define ATTRIBUTE_REDRAW       0x00000001
#define ATTRIBUTE_NORMAL       0x00000000
	ULONG    attribute;
}symbol;

enum {INPUT_ONOFF, INPUT_BTNMORE=2, INPUT_BTNLESS=4, INPUT_ROUTERPLUS=8, INPUT_ROUTERMINUS=16,
      INPUT_RINNERPLUS=32, INPUT_RINNERMINUS=64,INPUT_ENTER=128,
      INPUT_LINNERPLUS=256, INPUT_LINNERMINUS=512,INPUT_DTO=1024,INPUT_RCURSOR=2048,
      INPUT_LOUTERMINUS=4096,INPUT_LOUTERPLUS=8192,INPUT_PULLSCAN=16384,INPUT_CLR=32768,
	  INPUT_LCURSOR = 65536, INPUT_MSG = INPUT_LCURSOR << 1,
	  INPUT_APON = 262144, INPUT_CLOSE = 524288
};

enum {REQUEST_LOADFPL0=1};

enum {STATE_OFF,STATE_INSELFTEST,STATE_INSELFTEST_STAGE1,STATE_DBEXPIRE,STATE_MAIN_LOOP};
enum {FORMAT_BLACK,FORMAT_TWOPARTS,FORMAT_ONEPART};

enum datetime_t { UTC_TIME, LOCAL_TIME, SIM_TIME };

enum {CHAR_UNKNOWN=-1,CHAR_ALPHA_NUM=0,DTO_CHAR=1,FROM_CHAR=2,TO_CHAR=3,NDB_CHAR=4,VOR_CHAR=5,APT_CHAR=6,MORE_CHAR=7,BLANK_CHAR = 0x20};
int char_type(char ch);

void main_loop(int ACTION);
void print_str(int row,int col,ULONG attribute,char *text,...);
char K_get_char(int row,int col);
void K_change_attribute(int row,int col,int len,ULONG attribute);
ULONG K_get_attribute(int row,int col);
void K_set_char(int row,int col,char ch);
void clear_screen(void);

enum {SCREEN_SAVE,SCREEN_RESTORE};
void sr_screen(int ACTION);
void sr_screen_to_buf(int ACTION,void *__temp_buffer);

enum {DB_OK,DB_NOTEXIST,DB_EXPIRE,DB_ERRCRITICAL};
int check_db(char *fs_base_dir);
int Create_NAVDB(char *FS_MainDir);
void Load_NAVDB(char *FS_MainDir);
int do_dbexpire(int ACTION);
void do_poweron_page(int ACTION);
char *get_date(char *buff, datetime_t time_type);
char *get_time(char *buff, datetime_t time_type);
void init_console(void);
void update_screen(void);
char K_next_char(char ch);
char K_prev_char(char ch);
char *K_get_string(int start_row,int start_col,int end_col,char *_string);
char *K_trim_string(char *_string);

void read_magdec(char *FS_MagDec_File);
double get_magdec(double Lon,double Lat);
void K_DEBUG(char *format,...);
void K_DEBUG2(char *format,...);
void PRINTLOG(DWORD Verbosity, char *Format, ...);
void init_debug(void);
void deinit_debug(void);
void K_print_debug_buffer(void);
//*****************************************************************************
typedef struct _K_deg
{
   DWORD degrees;
   DWORD mins;
   DWORD dec_mins;
}K_deg;
void K_GetDegMin(FLOAT64 whole_degree,K_deg *deg_ptr);
double DM2DEG(double deg,double min,double dec_min);

char *K_FormName(char *src,char *dst,int count);
BOOL is_bgl_compressed(char *file_name);
double get_US(void);
LONG get_TK(void);
FLOAT64 get_TK2(void);
LONG get_GS(double divisor);
double get_HDG(void);
double get_IPU(double w);
double get_W(void);
double get_MAGVAR(void);
double get_TCRS(void);
void get_PPOS(double *latitude,double *longitude);
double get_S(double src_lat,double src_long,double dst_lat,double dst_long);
double get_ZPU(double src_lat,double src_long,double dst_lat,double dst_long,double mag_var);

int get_ap_active();
int get_nav_gps_switch();
int get_ap_heading_hold();
double get_baro_press();
double get_indicated_altitude();

int inc_nav_page(int curr_page);
int dec_nav_page(int curr_page);
void do_nav_page(int ACTION,int page_type);
int do_ndb_page(int ACTION);
int do_vor_page(int ACTION);
int do_apt_page(int ACTION);
int do_wpt_page(int ACTION);
int do_sup_page(int ACTION);

enum {NAV_PAGE,NDB_PAGE,VOR_PAGE,APT_PAGE,WPT_PAGE,SUP_PAGE,DT_PAGE,ACT_PAGE,CALC_PAGE,DTO_PAGE=1000,MSG_PAGE=1001};
enum {FPL_PAGE=1,SET_PAGE=2,OTH_PAGE=3,MOD_PAGE=4};

enum {PAGE_LEFT,PAGE_RIGHT};
enum {ACTION_INIT=-10,ACTION_TIMER=-9,MULTI_LINE_SELECTED=-8,ACTION_FREE_RESOURCES=-7,
      ACTION_FPLDTO_DONE=-6,PAGE_FORCE_CHANGE=-5,ACTION_SHOW=-4,ACTION_HIDE=-3,ACTION_SUP_PT_CREATED=-2};

enum {STRING_T,INT_T,DOUBLE_T};
void K_GetParam(char *main_str,char *sub_str,int TYPE,void *target_buffer,char beg='[',char end=']');
unsigned long icao2id(char *buffer);
void UnLoad_NAVDB(void);

void mem_init(void);
void mem_deinit(void);
void *K_malloc(DWORD bytes);
void K_free(void *ptr);

BOOL is_ndb_id(long ind,long *local_index);
BOOL is_vor_id(long ind,long *local_index);
BOOL is_apt_id(long ind,long *local_index);
BOOL is_wpt_id(long ind,long *local_index);
BOOL is_sup_id(long ind,long *local_index);


BOOL K_is_scanpull(void);
BOOL K_is_apon(void);
void build_nearest_list(void);

int ndb_compare(nvdb_ndb *ndb1,nvdb_ndb *ndb2);
int vor_compare(nvdb_vor *vor1,nvdb_vor *vor2);
int apt_compare(nvdb_apt *apt1,nvdb_apt *apt2);

int do_dto_page(int ACTION,DWORD extra);

int get_lp_type(void);
int get_rp_type(void);
void switch_lp(int page_type,int extra);
void switch_rp(int page_type,int extra);

nvdb_ndb *get_current_ndb(void);
nvdb_vor *get_current_vor(void);
nvdb_apt *get_current_apt(void);
nvdb_wpt *get_current_wpt(void);
nvdb_wpt *get_current_sup(void);

//===================== NAVPAGES ===========================
void set_navpage_number(int page_pos,int page_number);
int get_navpage_number(int page_pos);
//===================== For Navigation =====================
#pragma pack(1)

// Navigation point data structure
typedef struct nv_hdr
{
	char   ICAO_ID[6];
	double Lon;
	double Lat;
} nv_hdr_t;

// Flight plan navigation point data structure
enum {	NAVAID_MIN = 0, NAVAID_NDB = 0, NAVAID_VOR = 1, NAVAID_APT = 2, NAVAID_WPT = 3, NAVAID_SUP = 4, NAVAID_MAX = 4, 
		NAVAID_NULL = 5, NAVAID_SID_MAIN = 6, NAVAID_STAR_MAIN = 7, NAVAID_SS_POINT };
typedef struct nv_point
{
	int type;
	void *buffer_spec;
	FLOAT64 abs_dis;
	FLOAT64 dis_from_beg;
	FLOAT64 dis_from_curr;
	FLOAT64 dtk;
	char alt_name[20];
} nv_point_t;

// Flight plan data structure
#define MAX_FP_POINTS	50
typedef struct fpl
{
	nv_point_t points[MAX_FP_POINTS + 1];
	int point_in_use;
} fpl_t;

#define	MAX_SS_POINTS	30

// ---

enum {DTO_LEG,FPL_LEG,FPLDTO_LEG,UNKNOWN_LEG};
void set_route_leg(nv_point_t *src_point,nv_point_t *dst_point,int leg_type);

void fpl_set_navigation(void);
void fpl_continue_navigation(void);
int fpl_get_number_upt(void *buffer_spec);
int fpl_get_nulls_before(int fpl_number,int stop_point);
nv_point_t *fpl_get_last(int *index);
//void insert_into_fpl(fpl_t *__fpl, nv_point_t *nv_point, int index);
bool is_fp0_full();
void append_to_fp0(nv_point_t *nv_point);

typedef struct _rt_leg_t
{
   double Lon_src;
   double Lon_dst;
   double Lat_src;
   double Lat_dst;
   char   *dst_icao_id;
   char   *src_icao_id;
   double DIS;
   double ETE;
   double BRG;
   double LBU;
   double DTK;
   double OD;
   double LEN;
   BOOL FT;
   nv_point_t *srcp;
   nv_point_t *dstp;
   int leg_crs;
}rt_leg_t;
#pragma pack()
rt_leg_t *get_rt_leg(void);
void calulate_navdata(void);
//============================================================
void set_ml_mode(int page);
//============================================================
LONG round_deg(double deg);
//================= Input Handlers here ======================
BOOL fpl_handle_key(int INPUT_MASK);
BOOL nav_handle_key(int INPUT_MASK);
BOOL nav_handle2_key(int INPUT_MASK);
BOOL dto_handle_key(int INPUT_MASK);
BOOL oth_handle_key(int INPUT_MASK);

BOOL ndb_handle_key(int INPUT_MASK);
BOOL vor_handle_key(int INPUT_MASK);
BOOL apt_handle_key(int INPUT_MASK);
BOOL int_handle_key(int INPUT_MASK);
BOOL sup_handle_key(int INPUT_MASK);
BOOL dt_handle_key(int INPUT_MASK);
BOOL act_handle_key(int INPUT_MASK);
//=================== FPL ====================================
int do_fpl_pages(int ACTION);
//============================================================
void print_ndb(nvdb_ndb *__ndb,char *status);
void print_vor(nvdb_vor *__vor,char *status);
void print_apt(nvdb_apt *__apt,char *status);
void apt_print_apt(nvdb_apt *__apt,char *status,int apt_page=1,int sub_page=1);
void apt_prev_page_ext(int main_page,int sub_page,int *p_main_page,int *p_sub_page,nvdb_apt *__apt);
void apt_next_page_ext(int main_page,int sub_page,int *p_main_page,int *p_sub_page,nvdb_apt *__apt);
void print_wpt(nvdb_wpt *__wpt,char *status);
//=============================================================
void block_all(int page);
void do_block(int ACTION);
void unblock_all(void);
//=============================================================
void fpl_set_next_leg(void);
//=============================================================
inline double __add_deg(double ang,double addition)
{
	ang+=addition; if(ang>360.0f) return(ang-360.0f); if(ang<0.0f) return(360.0f+ang); return(ang);
}
inline double __add_deg180(double ang,double addition)
{
	ang+=addition; if(ang>180.0f) return(ang-360.0f); if(ang<-180.0f) return(360.0f+ang); return(ang);
}
//===============================================================
void update_sim_vars(void);
void init_sim_vars(void);
//================================================================
enum {H_U,H_D,V_L,V_R};

typedef struct _K_npoint_t
{
   double x;
   double y;
   int type;
   char ICAO_ID[6];
   int pt_type;
}K_npoint_t;

enum {NAV5_MODE_TK,NAV5_MODE_DTK,NAV5_MODE_HDG,NAV5_MODE_TN};
typedef struct _K_nav_box_t
{
   K_line_t box_lines[4];
   double start_x;
   double start_y;
   double stop_x;
   double stop_y;
   int curr_nav5_mode;
}K_nav_box_t;

typedef struct _K_nav_lines_t
{
   K_line_t rt_legs[30];
   K_line_t rnws[30];
   K_npoint_t points[30];
   int points_count;
   int legs_count;
   int rnws_count;
   BOOL is_r_menu;
   BOOL is_dto_list;
   BOOL is_msg;
}K_nav_lines_t;
typedef struct K_point_t
{
   double x;
   double y;
}K_point_t;


int fpl_get_leg(void);
void fpl_set_curr_leg(void);
BOOL is_dto(void);
void set_kln90_dir(char *dir_name);
int get_kln90_dir(char *buffer);
void set_fs_dir(char *dir_name);
int get_fs_dir(char *buffer);
char *GetFsMainDir(void);
int nav_mode(void);
nv_hdr_t *get_dtofpl(void);
void fpl_set_fpldto_index(int index);
nv_point_t *fpl_get_dtofpl(void);
void fpl_set_fpldto(void);
void init_navigation(void);

void set_fpl0_from_af(void);

nv_hdr_t *get_navaid_buffer(long local_index,int type);
void activate_fpl0(fpl_t *__fpl,BOOL inverse);
void copy_fpl(fpl_t *dst,fpl_t *src);
void Save_FPL(fpl_t *__fpl,int f_number);
//FLOAT64 fpl_get_next_leg_dtk(FLOAT64 *,FLOAT64 *);
FLOAT64 fpl_get_next_leg_dtk(FLOAT64 *__lbu,FLOAT64 *__dis,rt_leg_t *__rt);
BOOL fpl_get_actual_track(rt_leg_t *__rt);
FLOAT64 fpl_get_active_leg_dtk(void);

const double INVALID_ANGLE = -1000000.55f;
LONG K_ftol(double arg);

fpl_t *fpl_GetCurrentFPL(int *fpl_number_ptr);
enum {POINT_NONE,DONE_POINT,FROM_POINT,TO_POINT,WILL_POINT,FPL_POINT,AFPL_POINT};
nv_point_t *get_RowPointType(int row,int *point_type);
int do_dt_pages(int ACTION);
int do_act_page(int ACTION);
void calculate_fpls(void);

nv_point_t *fpl_get_active(int *index);
nv_point_t *fpl_get_last(int *index);
nv_point_t *fpl_get_next(int *index);
void check_super(void);
BOOL get_CDI(FLOAT64 *xtk,FLOAT64 *scale);

enum {SUPER_NAV1,SUPER_NAV5,SUPER_NONE};
int get_super_type(void);
BOOL get_FT(void);
void get_LL(FLOAT64 Lat_src,FLOAT64 Lon_src,FLOAT64 S,FLOAT64 OD,FLOAT64 *dLat,FLOAT64 *dLon);
double get_LBU(double src_lat,double src_long,double dst_lat,double dst_long,double craft_lat,double craft_long,double *delta,double *SS);
double get_LBU2(double src_lat,double src_long,double dst_lat,double dst_long,double craft_lat,double craft_long,double *delta,double *SS);
int fpl_get_legs(fpl_t *__fpl);
BOOL get_leg(fpl_t *__fpl,rt_leg_t *rt_leg,int leg_number);
BOOL IsBlocked(void);
int fpl_get_active_leg(void);

enum {ADD_STATUS_MESSAGE,ENABLE_ENT,DISABLE_ENT,ENABLE_FMES,DISABLE_FMES,ENABLE_RENT,DISABLE_RENT};
enum {NO_ACTION,NO_MORE_MESSAGES};
int do_status_line(int ACTION,char *str);
BOOL K_get_rp_point(nv_point_t *pt);
enum {DTO_MODE_NORMAL,DTO_MODE_FPL};
nv_point_t *dto_get_cp(void);
void dto_change_mode(int MODE);
void nav_set_mode(int MODE);
BOOL fpl_is_in_fpl(nv_hdr_t *__nv);
BOOL fpl_get_active_LL(double *Lat,double *Lon);
BOOL fpl_get_next_LL(double *Lat,double *Lon);
int fpl_get_real_points(int fpl);

//enum {VOR_TERMINAL=1,VOR_LOW=2,VOR_HIGH=3};

typedef struct _dto_list_t
{
	nv_point_t points[MAX_FP_POINTS + 1];
	int list_count;
	int list_index;
}dto_list_t;

void dto_update_list(void);
void dto_init_list(void);
void dto_list_next(void);
void dto_list_prev(void);

enum {LEG_MODE,OBS_MODE,OBS_CANCEL_MODE};
int do_mod_page(int ACTION);
double cdi_get_scale(void);
BOOL mod_handle_key(int INPUT_MASK);
int mod_get_mode(void);
int mod_get_obs(void);


int do_set_page(int ACTION);
BOOL set_handle_key(int INPUT_MASK);
#define GET_SIM_VAL(v1,v2) do { v2=v1.var_value.n; }while(0);

typedef struct _K_dite_time_t
{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
}K_dite_time_t;
void get_DT(K_dite_time_t *__dt);
void get_NOW_plus_this(K_dite_time_t *add);
K_dite_time_t *sub_DT(K_dite_time_t *newer,K_dite_time_t *later);
K_dite_time_t *calc_flying_time(int time_type);

BOOL set_is0_active(void);
int set_get_rml(void);
FLOAT64 mod_get_scale(void);
int do_msg_page(int ACTION);
void msg_add_message(char *message);
typedef struct _saved_pages_t
{
	int lp;
	int rp;
}saved_pages_t;
void set_s5_message(BOOL s5_msg);
enum {FRONT_PAGE,REGULAR_PAGE};
int lp_kind(void);
void dto_set(nv_point_t *__pt);

void obs_next_val(void);
void obs_prev_val(void);
void mod_set_obs_value(FLOAT64 new_vlaue,rt_leg_t *__rt);
long *find_ids_by_icao(char *icao_id_s,long *array_size);
//void get_icao_by_id(long point_id,char *__ICAO_ID);
void get_icao_by_id(long point_id,char *__ICAO_ID,nv_point_t *point=NULL);
nv_point_t *show_active_point(long point_id);
void fpl_build_ml_list(int count,long *list);
void show_nv_point(nv_point_t *__pt,BOOL save_screen=TRUE);
void print_navaid_arrow(void *buffer_spec);
int get_arrow_attr(void);
BOOL on_the_ground(void);

#define CALC_K(l) l.k = l.x2==l.x1 ? 7000.0f*1.852f*10e3 : (l.y2-l.y1)/(l.x2-l.x1)
#define CALC_B(l) l.b = (l.y1-l.k*l.x1)

char *get_RNW_des(int des);
void calc_log_line(double sLat,double sLon,double dLat,double dLon,double cLat,double cLon,K_line_t *result_line,double north=0,double mag_var=0);
BOOL do_draw_rnws(void);
int find_of(int *array,int value,int max_values);

void K_save_dword_param(char *section,char *attribute,DWORD value);
BOOL K_load_dword_param(char *section,char *attribute,DWORD *value);
void K_save_string_param(char *section,char *attribute,const TCHAR *value);
BOOL K_load_string_param(char *section,char *attribute,TCHAR *value);

long add_usr_point(nv_point_t *new_point);
void usr_del_point(long index);
long usr_get_pt_count(void);
nvdb_usr *usr_get_point(long index,int type=0);
long usr_spec_points(int nav_type);
nvdb_usr *usr_wpt_by_id(int nav_type,int index);
nvdb_usr *usr_wpt_by_id2(int nav_type,int index);
long usr_index_by_id(int nav_type,int index);
void act_get_act_point(nv_point_t *nvpt);
char *act_nvtype2text(int nv_type);
int do_oth_page(int ACTION);
void sup_nvpt_deleted(nvdb_wpt *delete_point);

void show_input_dlg(void);
void input_config_init(void);
void input_config_clear(void);

enum {MSG_NONE,MSG_KMSG};
int get_msg_stat(void);
double get_GPS_altitude(void);
void insert_into_fpl(fpl_t *__fpl,nv_point_t *nv_point,int index);
void delete_from_fpl(fpl_t *__fpl, int index);
void fpl_delete_sids_points(void);
void fpl_delete_stars_points(void);
bool fpl_insert_sid_points(nvdb_apt *current_apt, char* ss_name);
bool fpl_insert_star_points(nvdb_apt *current_apt, char* ss_name);

#endif
