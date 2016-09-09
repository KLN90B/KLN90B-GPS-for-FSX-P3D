#ifndef __NEAREST_H__
#define __NEAREST_H__

#define MAX_IN_LIST 256

typedef struct _nr_ndb
{
    unsigned long index;
	double   S;
	double   radial;
}nr_ndb;

typedef struct _nr_ndb_list
{
    nr_ndb list[MAX_IN_LIST];
	long nr_ndb_count;
}nr_ndb_list;

typedef struct _nr_vor
{
    unsigned long index;
	double   S;
	double   radial;
}nr_vor;

typedef struct _nr_vor_list
{
    nr_vor list[MAX_IN_LIST];
	long nr_vor_count;
}nr_vor_list;

typedef struct _nr_apt
{
    unsigned long index;
	double   S;
	double   radial;
}nr_apt;

typedef struct _nr_apt_list
{
    nr_apt list[MAX_IN_LIST];
	long nr_apt_count;
}nr_apt_list;
#endif