#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"


extern dto_list_t K_dto_list;

static nv_point_t current_point;

static int ACT_INPUT_MASK = INPUT_RINNERMINUS|INPUT_RINNERPLUS;

BOOL act_handle_key(int INPUT_MASK)
{
	return(INPUT_MASK&ACT_INPUT_MASK);
}

static int apt_page;
static int apt_sub_page;

char *act_nvtype2text(int nv_type)
{
    switch(nv_type)
	{
	case NAVAID_NDB:
		return("N");
	case NAVAID_VOR:
		return("V");
	case NAVAID_APT:
		return("A");
	case NAVAID_WPT:
		return("I");
	case NAVAID_SUP:
		return("S");
	}
    return("");
}


int act_get_list_index(int *active=NULL)
{
	int ind=-1;
	rt_leg_t *__rt = get_rt_leg();
	for(int i=0;i<K_dto_list.list_count;i++)
	{
		if(__rt)
		  if(__rt->dstp->buffer_spec == K_dto_list.points[i].buffer_spec)
			  if(active) *active=i;

		if(K_dto_list.points[i].buffer_spec == current_point.buffer_spec)
		{
			ind=i;
			break;
		}
	}
	return(ind);
}

void print_point_from_list(void)
{
     if(!current_point.buffer_spec)
	 {
	    for(int i=0;i<6;i++)
			print_str(i,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		print_str(6,18,ATTRIBUTE_NORMAL,"ACT  ");
		return;
	 }
	 if(current_point.type==NAVAID_APT)
		apt_print_apt((nvdb_apt *)current_point.buffer_spec,"APT 1",apt_page,apt_sub_page);
	 else
	    show_nv_point(&current_point,FALSE);

	 char dir_str[4]={0};
	 rt_leg_t *__rt_leg=get_rt_leg();
     int nv_mod = nav_mode();
     int ind = act_get_list_index();

     if(ind>=0 && __rt_leg)
	 {
		 BOOL active_point = __rt_leg->dstp->buffer_spec==current_point.buffer_spec;
		 if(!ind && nv_mod==DTO_LEG)
		   sprintf(dir_str,"%c  ",active_point?'>':' ');
		 else if(!ind && nv_mod!=DTO_LEG)
		   sprintf(dir_str,"%c%2d",active_point?'>':' ',ind+1);
		 else
		 {
			 sprintf(dir_str,"%c%2d",active_point?'>':' ',nv_mod==DTO_LEG?ind:ind+1);
		 }
	 }
	 print_str(0,12,ATTRIBUTE_NORMAL,"%-3s %-5s %s",dir_str,
		                             ((nv_hdr_t*)current_point.buffer_spec)->ICAO_ID,
					                 act_nvtype2text(current_point.type));
	 K_change_attribute(0,12,1,get_arrow_attr());
	 if(current_point.type==NAVAID_VOR && ((nvdb_vor *)current_point.buffer_spec)->DME )
		 print_str(0,20,ATTRIBUTE_NORMAL,"%s","D");
	 print_str(6,18,ATTRIBUTE_NORMAL,"ACT");
	 update_screen();
}

void act_get_act_point(nv_point_t *nvpt)
{
   if(nvpt) *nvpt = current_point;
}
static void set_current_point(nv_point_t *nvpt)
{
   if(!nvpt) 
   {
      current_point.buffer_spec=NULL;
	  return;
   }
   current_point = *nvpt;
   if(current_point.type==NAVAID_APT)
   {
      apt_page=1; apt_sub_page=1;
   }
}

int do_act_page(int ACTION)
{
   if(ACTION == ACTION_SHOW)
   {
       rt_leg_t *__rt_leg = get_rt_leg();
	   if((!__rt_leg) || (K_dto_list.list_count<=0)) 
	   {
	      set_current_point(NULL);
		  return(0);
	   }
	   for(int i=0;i<K_dto_list.list_count;i++)
	   {
	      if(K_dto_list.points[i].buffer_spec == __rt_leg->dstp->buffer_spec)
		  {
			  set_current_point(&K_dto_list.points[i]);
			  return(0);
		  }
	   }
       set_current_point(&K_dto_list.points[0]);
   }
   if(ACTION == ACTION_TIMER)
   {
	   print_point_from_list();
   }
   if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS)
   {
      if(K_is_scanpull())
	  {
	     dto_update_list(); // If the "WAR" happened :)

		 int act=-1;
		 int ind = act_get_list_index(&act);
		 if(ind == -1 && act!=-1)
		 {
		    set_current_point(&K_dto_list.points[act]);
			return(0);
		 }
	     if(ind == -1 || !K_dto_list.list_count)
	     {
	        set_current_point(NULL);
		    return(0);
	     }
		 ind = ACTION == INPUT_RINNERPLUS ? ind+1 : ind-1;
		 if(K_dto_list.list_count>ind && ind>=0)
			 set_current_point(&K_dto_list.points[ind]);
		 else if(ind >= K_dto_list.list_count)
			 set_current_point(&K_dto_list.points[0]);
		 else 
			 set_current_point(&K_dto_list.points[K_dto_list.list_count-1]);
      }
	  else
	  {
         if(!current_point.buffer_spec) return(0);
		 if(current_point.type!=NAVAID_APT) return(0);
		 int n_apt_page=apt_page,n_sub_page=apt_sub_page;
		 if(ACTION == INPUT_RINNERMINUS) 
		 {
			 apt_prev_page_ext(apt_page,apt_sub_page,&n_apt_page,&n_sub_page,(nvdb_apt*)current_point.buffer_spec);
			 apt_page=n_apt_page;
			 apt_sub_page=n_sub_page;
		 }
		 if(ACTION == INPUT_RINNERPLUS) 
		 {
			 apt_next_page_ext(apt_page,apt_sub_page,&n_apt_page,&n_sub_page,(nvdb_apt*)current_point.buffer_spec);
			 apt_page=n_apt_page;
			 apt_sub_page=n_sub_page;
		 }
	  }
   }

   return(0);
}