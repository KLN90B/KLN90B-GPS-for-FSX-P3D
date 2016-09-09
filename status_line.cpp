#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

extern INT SCREEN_FORMAT;
extern INT GLOBAL_STATE;
static int K_ENT;
static int K_RENT;
static int K_MSG;
static int K_STAT_MESS;
static char STATUS_MESSAGE[14];
static DWORD status_message_beg;
static DWORD message_beg;

int get_msg_stat(void)
{
   if(K_MSG) return(MSG_KMSG);
   return(MSG_NONE);
}
int do_status_line(int ACTION,char *str)
{
   if(ACTION == ACTION_INIT)
   {
      K_ENT=K_RENT=K_MSG=K_STAT_MESS=0;
	  status_message_beg=message_beg=0;
      return(0);
   }
   if(ACTION == ACTION_TIMER)
   {
      if(K_STAT_MESS)
	     if(GetTickCount()-status_message_beg>=5000)
		    K_STAT_MESS=0;
	  if(K_MSG)
		 if(GetTickCount()-message_beg>=30000)
		 {
		    K_MSG=0;
		    set_s5_message(FALSE);
		 }
   }
   if(ACTION == ENABLE_ENT)
	   K_ENT=1;
   if(ACTION == DISABLE_ENT)
	   K_ENT=0;
   if(ACTION == ENABLE_RENT)
	   K_RENT=1;
   if(ACTION == DISABLE_RENT)
	   K_RENT=0;
   if(ACTION == ENABLE_FMES)
   {
	   set_s5_message(TRUE);
	   K_MSG=1;
	   message_beg=GetTickCount();
   }
   if(ACTION == DISABLE_FMES)
   {
	   set_s5_message(FALSE);
	   K_MSG=0;
   }
   if(ACTION == ADD_STATUS_MESSAGE)
   {
      K_STAT_MESS=1;
      strcpy(STATUS_MESSAGE,str);
      status_message_beg=GetTickCount();
   }
   // Check for super pages and other
   if(GLOBAL_STATE != STATE_MAIN_LOOP) return(0);
   //if(! ( (SCREEN_FORMAT == FORMAT_TWOPARTS) || (SCREEN_FORMAT == FORMAT_ONEPART) ) )  return(0);
   //
   
   if(K_STAT_MESS)
   {
       print_str(6,6,ATTRIBUTE_INVERSE,"%s",STATUS_MESSAGE);
	   return(0);
   }
   if(set_is0_active())
	   print_str(6,5,ATTRIBUTE_NORMAL,"%-18s"," ");
   else if(get_super_type()!=SUPER_NAV5)
   {
      int cMode = mod_get_mode();
	  if(cMode == OBS_MODE)
	  {
	     if(get_rt_leg())
			 print_str(6,6,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL,"ENR:%03d ",K_ftol(mod_get_obs()));
		 else
			 print_str(6,6,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL,"ENR:--- ");
	  }
	  else if(cMode == LEG_MODE)
         print_str(6,6,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL,"ENR-LEG ");
   }

   if(K_MSG)
   {
       if(get_super_type() == SUPER_NAV5)
	      print_str(5,8,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"MSG");
	   else
	      print_str(6,14,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"MSG");
   }else if(K_ENT || K_RENT)
   {
      print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"ENT");
   }
   else if(get_super_type()!=SUPER_NAV5)
   {
      print_str(6,14,ATTRIBUTE_NORMAL,"   ");
   }
   
   
   if(get_super_type()!=SUPER_NAV5)
   {
      if(K_get_char(6,5)!=' ') print_str(6,5,ATTRIBUTE_NORMAL," ");
      if(K_get_char(6,17)!=' ') print_str(6,17,ATTRIBUTE_NORMAL," ");
   }
   
   update_screen();

   return(0);
}