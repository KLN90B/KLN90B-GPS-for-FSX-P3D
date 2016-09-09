#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

static int min_page_num=1;
static int max_page_num=2;
static int mod_page_number;
static int cursored;
static double cdi_modes[3]={5.0f,1.0f,0.30f};
static int cdi_mode;

static int on_obs_change;
static int on_cdi_change;

static int course_mode; 
static double obs_value;

static int MOD_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_ENTER|INPUT_CLR|INPUT_LCURSOR;

BOOL mod_handle_key(int INPUT_MASK)
{
	return(INPUT_MASK&MOD_INPUT_MASK);
}


void change_mode(int new_mode)
{
   rt_leg_t *__rt=get_rt_leg();   
   if(new_mode == OBS_MODE)
   {
     if(!__rt)
	 {
	    do_status_line(ADD_STATUS_MESSAGE,"NO ACTV WPT");
		return;
	 }
	 course_mode = OBS_MODE;
     obs_value = __rt->DTK;
	 if(__rt->DIS>200.0f)
		 msg_add_message("OBS WPT \x7 200NM");
	 dto_set(__rt->dstp);
   }
   if(new_mode == LEG_MODE)
   {
      course_mode = LEG_MODE;
	  if(__rt)
	  {
         double Lat,Lon;
		 get_PPOS(&Lat,&Lon);
         __rt->Lat_src=Lat; __rt->Lon_src=Lon;
		 __rt->leg_crs = OBS_CANCEL_MODE;
	  }
   }

}

double cdi_get_scale(void)
{
    return(cdi_modes[cdi_mode]);
}

void cdi_next_mode(void)
{
   cdi_mode++;
   if(cdi_mode>2) cdi_mode=0;
}

void cdi_prev_mode(void)
{
	cdi_mode--;
	if(cdi_mode<0) cdi_mode=2;
}
void obs_next_val(void)
{
	obs_value=__add_deg(obs_value,1);
}
void obs_prev_val(void)
{
	obs_value=__add_deg(obs_value,-1);
}

int mod_get_mode(void)
{
   return(course_mode);
}
int mod_get_obs(void)
{
   return(obs_value);
}

void next_page(void)
{
	mod_page_number++;
	if(mod_page_number>max_page_num) mod_page_number=min_page_num;
}
void prev_page(void)
{
	mod_page_number--;
	if(mod_page_number<min_page_num) mod_page_number=max_page_num;
}

void print_mod2_page(void)
{
	if(course_mode == OBS_MODE)
	{
		print_str(0,0,ATTRIBUTE_NORMAL,"ACTIVE MODE");
		print_str(1,0,ATTRIBUTE_NORMAL,"%-11s"," ");
		print_str(3,0,ATTRIBUTE_NORMAL,"OBS:%03d%c   ",(LONG)obs_value,0xB0);
		do_status_line(DISABLE_ENT,NULL);
		if(cursored && on_obs_change)
			K_change_attribute(3,4,4,ATTRIBUTE_INVERSE);
	}
	else
	{
		print_str(0,0,ATTRIBUTE_NORMAL,"PRESS ENT  ");
		print_str(1,0,ATTRIBUTE_NORMAL,"TO ACTIVATE");
		print_str(3,0,ATTRIBUTE_NORMAL,"OBS:---%c   ",0xB0);
		do_status_line(ENABLE_ENT,NULL);
	}
	print_str(2,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(4,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(5,0,ATTRIBUTE_NORMAL,"CDI:%c%.2fNM",0xB1,cdi_modes[cdi_mode]);
	if(cursored)
	{
		if(on_cdi_change)
		   K_change_attribute(5,5,4,ATTRIBUTE_INVERSE);
		print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
		K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
	}
	else
	{
		print_str(6,0,ATTRIBUTE_NORMAL,"MOD 2");
	}
	update_screen();
}

void print_mod1_page(void)
{
	if(course_mode == LEG_MODE)
	{
	   print_str(0,0,ATTRIBUTE_NORMAL,"ACTIVE MODE");
	   print_str(1,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	   do_status_line(DISABLE_ENT,NULL);
	}
	else
	{
	   print_str(0,0,ATTRIBUTE_NORMAL,"PRESS ENT  ");
	   print_str(1,0,ATTRIBUTE_NORMAL,"TO ACTIVATE");
	   do_status_line(ENABLE_ENT,NULL);
	}
	print_str(2,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(3,0,ATTRIBUTE_NORMAL,"%-11s","LEG");
	print_str(4,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(5,0,ATTRIBUTE_NORMAL,"CDI:%c%.2fNM",0xB1,cdi_modes[cdi_mode]);
    if(cursored)
	{
	   K_change_attribute(5,5,4,ATTRIBUTE_INVERSE);
	   print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
	   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
	}
	else
	{
	   print_str(6,0,ATTRIBUTE_NORMAL,"MOD 1");
	}
	update_screen();
}

void print_mod_page(int number)
{
	switch(number) 
	{
	case 1:
		{
			print_mod1_page();
		}
		break;
	case 2:
		{
			print_mod2_page();
		}
		break;
	default:
		break;
	}
}

int do_mod_page(int ACTION)
{
	if(ACTION == ACTION_INIT)
	{
		mod_page_number=1;
		cursored=0;
		cdi_mode=0;
		course_mode=LEG_MODE; 
		return(0);
	}
	if(ACTION == ACTION_TIMER)
	{
		print_mod_page(mod_page_number);	
	}
	if(ACTION == INPUT_LCURSOR)
	{
	    cursored=1-cursored;
		if(cursored) 
		{
			on_cdi_change=on_obs_change=0;
			if(mod_page_number == 1)
			{
				on_cdi_change=1;
			}
			else if(mod_page_number == 2)
			{
			   if(course_mode == OBS_MODE)
			      on_obs_change=1;
			   else
				  on_cdi_change=1;

			}
			MOD_INPUT_MASK = INPUT_LOUTERPLUS|INPUT_LOUTERMINUS|INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_ENTER|INPUT_CLR|INPUT_LCURSOR;
		}
		else 
		{
			MOD_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|INPUT_ENTER|INPUT_CLR|INPUT_LCURSOR;
		}
	}
	if(ACTION == INPUT_LOUTERMINUS || ACTION == INPUT_LOUTERPLUS)
	{
	   if(cursored)
	   {
	      if(mod_page_number == 2 && course_mode == OBS_MODE)
		  {
		     on_cdi_change=1-on_cdi_change;
			 on_obs_change=1-on_obs_change;
		  }
	   }
	}
	if(ACTION == INPUT_LINNERPLUS)
	{
	   if(cursored)
	   {
	      if(on_cdi_change)
		     cdi_next_mode();
		  if(on_obs_change)
			  obs_next_val();
	   }
	   else
		   next_page();
	}
	if(ACTION == INPUT_LINNERMINUS)
	{
		if(cursored)
		{
			if(on_cdi_change)
			   cdi_prev_mode();
			if(on_obs_change)
               obs_prev_val();
		}
		else
			prev_page();
	}
	if(ACTION == INPUT_ENTER)
	{
	   if(mod_page_number == 2 && course_mode == LEG_MODE)
	   {
	      change_mode(OBS_MODE);
	   }
	   else if(mod_page_number==1 && course_mode == OBS_MODE)
	   {
	      change_mode(LEG_MODE);
	   }
	}
	return (0);
}

FLOAT64 mod_get_scale(void)
{
   return(cdi_modes[cdi_mode]);
}

void mod_set_obs_value(FLOAT64 new_vlaue,rt_leg_t *__rt)
{
   obs_value = new_vlaue;
   if(__rt->DIS>200.0f)
	   msg_add_message("OBS WPT \x7 200NM");
}