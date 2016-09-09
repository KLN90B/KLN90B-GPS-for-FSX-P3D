#include "fslib.h"
#include "kln90b.h"

#ifndef __VNAV_H__
#define __VNAV_H__
int get_IND_altitude(void);
int get_SEL_altitude(void);
void set_SEL_altitude(int __alt);
int get_SEL_dis(void);
void set_SEL_dis(int __dis);
BOOL vnav_posable(void);
char *vnav_get_wpt(void);
void start_vnav_calc(void);
double get_vnav_angle(void);
void update_vnav(void);
double vnav_get_angle(void);
void vnav_complete_edit(void);
int vnav_get_state(void);
int vnav_get_ADV(void);
void vnav_set_cur_ang(void);
void vnav_set_ang(double __ang);
double vnav_in_time(void);
enum {VNAV_INACTIVE,VNAV_ARMED,VNAV_IN10,VNAV_WORKS};
#endif
