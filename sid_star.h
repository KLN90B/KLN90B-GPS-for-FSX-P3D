#include "fslib.h"
#include "kln90b.h"

#ifndef __SIDSTAR_H__
#define __SIDSTAR_H__
void ss_load_on_demand(nvdb_apt *__apt);
void ss_load_sid_rnws(nvdb_apt *__apt,char *sid_name);
void ss_load_star_rnws(nvdb_apt *__apt,char *star_name);
void ss_load_sid_trans(nvdb_apt *__apt, char *sid_name);
void ss_load_star_trans(nvdb_apt *__apt, char *star_name);
void ss_load_sid_points(nvdb_apt *__apt, char *sid_name, char *rnw_name, char *tran_name);
void ss_load_star_points(nvdb_apt *__apt, char *star_name, char *rnw_name, char *tran_name);
void ss_reload(nvdb_apt *__apt);
nv_hdr_t *get_sid_point(int index);
nv_hdr_t *get_star_point(int index);
#endif