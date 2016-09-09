#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

static int min_page_num=0;
static int max_page_num=3;
static int set_page_number;
enum {RNW_MIN_LEN=1000,RNW_MAX_LEN=5000};
static int rnw_min_length;
static char *surf_strs[]={"HRD","SFT","HRD SFT"};
enum {RNW_TYPE_MIN=0,RNW_TYPE_HRD=0,RNW_TYPE_SFT=1,RNW_TYPE_HRDSFT=2,RNW_TYPE_MAX=2};
static int rnws_type;
enum {S3_ON_LEN,S3_ON_SUR};
static int s3_item;
static int cursored;
extern INT SCREEN_FORMAT;
enum {ARMED_UPDATE_DB,PREUPDATE_DB,UPDATE_DONE};

static int update_state;

static int SET_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_ENTER|INPUT_CLR|INPUT_LCURSOR;

BOOL set_handle_key(int INPUT_MASK)
{
	return(INPUT_MASK&SET_INPUT_MASK);
}

void check_set0_block(void)
{
	if(!set_page_number && !IsBlocked())
	{
	    SCREEN_FORMAT = FORMAT_ONEPART;
		block_all(SET_PAGE);
	}
	if(set_page_number && IsBlocked())
	{
		clear_screen();
		SCREEN_FORMAT = FORMAT_TWOPARTS;
		unblock_all();
	}
}

void set_next_page(void)
{
   set_page_number++;
   if(set_page_number>max_page_num) set_page_number=min_page_num;
   check_set0_block();
}

void set_prev_page(void)
{
	set_page_number--;
	if(set_page_number<min_page_num) set_page_number=max_page_num;
	check_set0_block();
}

void print_set0_page(void)
{
	print_str(0,0,ATTRIBUTE_NORMAL,"      U P D A T E      ");
	print_str(1,0,ATTRIBUTE_NORMAL,"   D A T A   B A S E   ");
	print_str(2,0,ATTRIBUTE_NORMAL,"%-23s"," ");
	if(!cursored)
	{
	   print_str(3,0,ATTRIBUTE_NORMAL,"   O N   G R O U N D   ");
	   print_str(4,0,ATTRIBUTE_NORMAL,"        O N L Y        ");
	   print_str(5,0,ATTRIBUTE_NORMAL,"%-23s"," ");
	   print_str(6,0,ATTRIBUTE_NORMAL,"SET 0");
	}
	else
	{
	   if(update_state == PREUPDATE_DB)
	   {
	      print_str(3,2,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"UPDATE PUBLISHED DB");
		  print_str(4,0,ATTRIBUTE_NORMAL,"%-23s"," ");
		  print_str(5,0,ATTRIBUTE_NORMAL,"%-23s"," ");
	   }
	   print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
	   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
	}
	update_screen();
}

void print_set1_page(void)
{
    print_str(0,0,ATTRIBUTE_NORMAL,"INIT POSN  ");
	print_str(1,0,ATTRIBUTE_NORMAL,"WPT:       ");
	FLOAT64 long_deg;
	FLOAT64 lat_deg;
	K_deg deg;
	get_PPOS(&lat_deg,&long_deg);
	K_GetDegMin(lat_deg,&deg);
	print_str(2,0,ATTRIBUTE_NORMAL,"%c %.2d%c%.2d.%.2d'",lat_deg<0?'S':'N',(int)fabs(lat_deg),0xB0,deg.mins,deg.dec_mins);
	K_GetDegMin(long_deg,&deg);
	print_str(3,0,ATTRIBUTE_NORMAL,"%c%.3d%c%.2d.%.2d'",long_deg<0?'W':'E',(int)fabs(long_deg),0xB0,deg.mins,deg.dec_mins);
    print_str(4,0,ATTRIBUTE_NORMAL,"%3d KT %03d%c",get_GS(1.852),K_ftol(get_HDG()),0xB0);
	print_str(5,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(6,0,ATTRIBUTE_NORMAL,"SET 1");
	update_screen();
}
extern char *monthes[];
void print_set2_page(void)
{
	print_str(0,0,ATTRIBUTE_NORMAL," DATE/TIME ");
	print_str(1,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	K_dite_time_t __dt;
    get_DT(&__dt);
	print_str(2,0,ATTRIBUTE_NORMAL,"  %02d %s %02d",__dt.day,monthes[__dt.month],__dt.year%100);
	print_str(3,0,ATTRIBUTE_NORMAL,"%02d:%02d:%02dUTC",__dt.hour,__dt.min,__dt.sec);
    print_str(4,0,ATTRIBUTE_NORMAL,"CORD UNIV/Z");
    print_str(5,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(6,0,ATTRIBUTE_NORMAL,"SET 2");
	update_screen();
}

void print_set3_page(void)
{
    print_str(0,0,ATTRIBUTE_NORMAL,"NEAREST APT");
	print_str(1,0,ATTRIBUTE_NORMAL," CRITERIA  ");
	print_str(2,0,ATTRIBUTE_NORMAL,"MIN LENGTH:");
	print_str(3,0,ATTRIBUTE_NORMAL,"%10d'",rnw_min_length);
	print_str(4,0,ATTRIBUTE_NORMAL,"SURFACE:   ");
	print_str(5,0,ATTRIBUTE_NORMAL,"%11s",surf_strs[rnws_type]);
    
	if(cursored)
	{
	   if(s3_item == S3_ON_LEN)
		   K_change_attribute(3,6,4,ATTRIBUTE_INVERSE);
	   else if(s3_item == S3_ON_SUR)
           K_change_attribute(5,0,11,ATTRIBUTE_INVERSE);
	   print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
	   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
	}
	else
	   print_str(6,0,ATTRIBUTE_NORMAL,"SET 3");
	update_screen();
}


void print_set_page(int number)
{
   switch(number) 
   {
   case 0:
	   {
		   print_set0_page();
	   }
	   break;
    case 1:
		{
		   print_set1_page();
		}
   	    break;
	case 2:
		{
			print_set2_page();
		}
		break;
	case 3:
		{
			print_set3_page();
		}
		break;
    default:
		break;
   }
}

extern MODULE_VAR airaft_on_ground_sim;

void handle_set0_input(int ACTION)
{
	int grnd;
	GET_SIM_VAL(airaft_on_ground_sim,grnd);
	if(!(grnd && !get_GS(1.852)))
		return;
	cursored=1-cursored;
	if(cursored)
	{
		if(update_state == ARMED_UPDATE_DB)
		{
			update_state = PREUPDATE_DB;
			do_status_line(ENABLE_ENT,NULL);
		}
	}
	else
	{
		if(update_state == PREUPDATE_DB)
		{
			update_state = ARMED_UPDATE_DB;
			do_status_line(DISABLE_ENT,NULL);
		}
	}
}

void handle_set3_input(int ACTION)
{
   if(ACTION == INPUT_LCURSOR)
   {
      cursored=1-cursored;
      if(cursored)
         SET_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_ENTER|INPUT_CLR|INPUT_LCURSOR|INPUT_LOUTERMINUS|INPUT_LOUTERPLUS;
	  else
	     SET_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_ENTER|INPUT_CLR|INPUT_LCURSOR;
   }
   if(ACTION==INPUT_LOUTERMINUS||ACTION==INPUT_LOUTERPLUS)
   {
       if(cursored)
	      s3_item=1-s3_item;
   }
   if(ACTION==INPUT_LINNERMINUS)
   {
	   if(cursored)
	   {
		   if(s3_item == S3_ON_LEN)
		   {
			   rnw_min_length-=100;
			   if( rnw_min_length < RNW_MIN_LEN )
				   rnw_min_length = RNW_MIN_LEN;
		   }
	   }
   }
   if(ACTION==INPUT_LINNERPLUS)
   {
	   if(cursored)
	   {
		   if(s3_item == S3_ON_LEN)
		   {
			   rnw_min_length+=100;
			   if( rnw_min_length > RNW_MAX_LEN )
				   rnw_min_length = RNW_MAX_LEN;
		   }
	   }
   }
}

int do_set_page(int ACTION)
{
    if(ACTION == ACTION_INIT)
	{
	  set_page_number=1;
	  cursored=0;
	  rnw_min_length=5000;
	  rnws_type=RNW_TYPE_HRD;
	  update_state = ARMED_UPDATE_DB;
	  s3_item=S3_ON_LEN;
	  return(0);
	}
    if(ACTION == ACTION_TIMER)
	{
       print_set_page(set_page_number);	
	}
	if(ACTION == INPUT_ENTER)
	{
	   if(!set_page_number)
	   {
		   if(update_state == PREUPDATE_DB)
		   {
		      char K_dir[MAX_PATH];
			  print_str(6,0,ATTRIBUTE_NORMAL,"SET 0");
			  do_status_line(DISABLE_ENT,NULL);
			  do_status_line(ACTION_TIMER,NULL);
			  get_fs_dir(K_dir);
		      Create_NAVDB(K_dir);
		      print_str(3,0,ATTRIBUTE_NORMAL,"  UPDATE PUBLISHED DB  ");
		      print_str(4,0,ATTRIBUTE_NORMAL,"       COMPLETED       ");
			  print_str(5,0,ATTRIBUTE_NORMAL,"      ACKNOWLEDGE?     ");
			  K_change_attribute(5,6,12,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE);
		      do_status_line(ENABLE_ENT,NULL);  
			  print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
			  K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
			  update_state = UPDATE_DONE;
		   }
		   else if(update_state == UPDATE_DONE)
		   {
		       do_status_line(DISABLE_ENT,NULL);
			   update_state = ARMED_UPDATE_DB;
			   cursored=0;
		   }
	   }
	}
	if(ACTION == INPUT_LINNERMINUS)
	{
		if(cursored)
		{
			if(set_page_number==3)
			   handle_set3_input(ACTION);
		}
		else
		{
			set_prev_page();
		}
	}
	if(ACTION == INPUT_LINNERPLUS)
	{
		if(cursored)
		{
			if(set_page_number==3)
			   handle_set3_input(ACTION);
		}
		else
		{
		   set_next_page();
		}
	}
    if(ACTION == INPUT_LCURSOR)
	{
	   if(set_page_number==3)
	      handle_set3_input(ACTION);
	   else if(!set_page_number)
	      handle_set0_input(ACTION);
	}
	if(ACTION == INPUT_LOUTERMINUS)
	{
		if(set_page_number==3)
			handle_set3_input(ACTION);
	}
	if(ACTION == INPUT_LOUTERPLUS)
	{
		if(set_page_number==3)
			handle_set3_input(ACTION);
	}
	return (0);
}

BOOL set_is0_active(void)
{
   return(!set_page_number);
}

int set_get_rml(void)
{
   return(rnw_min_length);
}
