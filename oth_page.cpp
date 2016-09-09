#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"

extern long usr_ids[NAVAID_MAX+1][USR_MAX_POINTS];


static int OTH_INPUT_MASK = INPUT_LCURSOR;
static int cursored;
static int screen_row;
static int index_row;
static int del_index;
static BOOL wait_for_del;
static nvdb_usr *del_point;

BOOL oth_handle_key(int INPUT_MASK)
{
	return(INPUT_MASK&OTH_INPUT_MASK);
}

typedef struct _usr_list_t
{
   long point_index;
   int  point_type;
   BOOL is_active;
   int  fpl_number;
}usr_list_t;

usr_list_t usr_index_list[USR_MAX_POINTS];
int usr_index_count;


BOOL is_active(nvdb_usr *pt,int index)
{
	rt_leg_t *active_rt = get_rt_leg();
	usr_index_list[index].is_active=FALSE;
	if(active_rt)
	{
		if(active_rt->dstp->buffer_spec == (void *)pt)
			usr_index_list[index].is_active=TRUE;
    }
    return(usr_index_list[index].is_active);
}

int usr_list_sort(const void *el1,const void *el2)
{
   return strcmp(
	      usr_wpt_by_id2(((usr_list_t *)el1)->point_type,((usr_list_t *)el1)->point_index)->ICAO_ID,
          usr_wpt_by_id2(((usr_list_t *)el2)->point_type,((usr_list_t *)el2)->point_index)->ICAO_ID
		  );
}

void build_usr_list(void)
{
	const int types_count = 5;
	int order_types[types_count]={NAVAID_APT,NAVAID_VOR,NAVAID_NDB,NAVAID_WPT,NAVAID_SUP};
    int count_spec_points;
	usr_index_count=0;
	for(int type=0;type<types_count;type++)
	{
       count_spec_points = usr_spec_points(order_types[type]);
	   for(int upt=0;upt<count_spec_points;upt++)
	   {
	      usr_index_list[usr_index_count+upt].point_index = usr_ids[order_types[type]][upt];
		  usr_index_list[usr_index_count+upt].point_type = order_types[type];
		  nvdb_usr *priv_upt = usr_wpt_by_id2(order_types[type],usr_ids[order_types[type]][upt]);
		  usr_index_list[usr_index_count+upt].fpl_number = fpl_get_number_upt(priv_upt);
	   }
	   // Do sort of this points from [usr_index_count:count_spec_points]
	   qsort(&usr_index_list[usr_index_count],count_spec_points,sizeof(usr_list_t),usr_list_sort);
	   usr_index_count+=count_spec_points;
	}
}

const int oth_page_number=3;

static void print_users_page(int start_index,char *status,int stat_attr)
{
    int row=1;
	print_str(0,0,ATTRIBUTE_NORMAL," USER WPTS ");
	for(int i=start_index;i<start_index+5;i++)
	{
	    if(i<usr_index_count)
		{
		    nvdb_usr *upt = usr_wpt_by_id2(usr_index_list[i].point_type,usr_index_list[i].point_index);
			char info_str[3];
			is_active(upt,i);
            if(usr_index_list[i].is_active)
               sprintf(info_str,"> ");
			else if(usr_index_list[i].fpl_number>=0)
			   sprintf(info_str,"%2d",usr_index_list[i].fpl_number);
			else sprintf(info_str,"  ");
            print_str(row,0,ATTRIBUTE_NORMAL,"%-5s %s  %2s",upt->ICAO_ID,
				          act_nvtype2text(upt->usr_type),info_str);
		}
		else
			print_str(row,0,ATTRIBUTE_NORMAL,"%-11s"," ");
		row++;
	}
    if(status)
	{
        if(stat_attr&ATTRIBUTE_INVERSE)
		{
	       print_str(6,0,stat_attr,"%s",status);
		   K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);
		}
		else
		   print_str(6,0,stat_attr,"%s",status);
	}
	else
	   print_str(6,0,ATTRIBUTE_NORMAL,"     ");
}

void print_oth_page(void)
{
   switch(oth_page_number)
   {
   case 3: // Users points
	  {
	     if(!cursored)
		    print_users_page(0,"OTH 3",ATTRIBUTE_NORMAL);
		 else
		 {
		    print_users_page(index_row," CRSR",ATTRIBUTE_INVERSE);
			if(wait_for_del)
			   print_str(0,0,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"DEL %-5s ?",del_point->ICAO_ID);
			else
			   K_change_attribute(screen_row,0,11,ATTRIBUTE_INVERSE);
		 }
		 update_screen();
	  }
      break;
   }
}

void cancel_del(void)
{
	if(wait_for_del)
	{
		wait_for_del=FALSE;
		unblock_all();
		sr_screen(SCREEN_RESTORE);
		print_oth_page();
		do_status_line(DISABLE_ENT,NULL);
	}
}

int do_oth_page(int ACTION)
{
   if(ACTION==ACTION_SHOW)
   {
      cursored=0;
	  wait_for_del=FALSE;
	  build_usr_list();
   }
   if(ACTION == ACTION_TIMER)
   {
	     print_oth_page();   
   }
   if(ACTION == INPUT_LCURSOR)
   {
      if(!cursored)
	  {
	     if(!usr_index_count) return(0);
		 cursored=1;
		 OTH_INPUT_MASK = INPUT_LCURSOR|INPUT_LOUTERPLUS|INPUT_LOUTERMINUS|INPUT_ENTER|INPUT_CLR;
		 screen_row=1;
		 index_row=0;
		 print_oth_page();
	  }
	  else
	  {
	     cursored=0;
		 cancel_del();
		 OTH_INPUT_MASK = INPUT_LCURSOR;
	  }
   }
   if(ACTION == INPUT_LOUTERPLUS)
   {
	   if(wait_for_del) return(0);
	   if(screen_row==5)      
	   {
		   if( (index_row+5) < usr_index_count )
			   index_row++;
	   }
	   else if( (screen_row+index_row) < usr_index_count )
		   screen_row++;
	   print_oth_page();
   }
   if(ACTION == INPUT_LOUTERMINUS)
   {
	   if(wait_for_del) return(0);
	   if(screen_row==1)      
	   {
		   if( index_row > 0 )
			   index_row--;
	   }
	   else 
	      screen_row--;
	   print_oth_page();
   }
   if(ACTION == INPUT_CLR)
   {
      if(!cursored) return(0);
	  if(wait_for_del)
	  {
         cancel_del();
		 return(0);
	  }
	  del_index = index_row+screen_row-1;
	  if(usr_index_list[del_index].fpl_number>=0)
         do_status_line(ADD_STATUS_MESSAGE,"USED IN FPL");                                       
	  if(usr_index_list[del_index].is_active)
	     do_status_line(ADD_STATUS_MESSAGE,"ACTIVE  WPT");
      else
	  {
         wait_for_del=TRUE;
		 block_all(OTH_PAGE);
         del_point = usr_wpt_by_id2(usr_index_list[del_index].point_type,usr_index_list[del_index].point_index);
		 nv_point_t __pt;
		 __pt.buffer_spec = del_point;	 
		 __pt.type = usr_index_list[del_index].point_type;
		 show_nv_point(&__pt,TRUE);
		 do_status_line(ENABLE_ENT,NULL);
		 print_oth_page();
	  }
   }
   if(ACTION == INPUT_ENTER)
   {
      if(!cursored) return(0);
	  if(!wait_for_del) return(0);
	  usr_del_point(usr_index_list[del_index].point_index);
	  build_usr_list();
	  wait_for_del=FALSE;
	  sr_screen(SCREEN_RESTORE);
	  unblock_all();
	  do_status_line(DISABLE_ENT,NULL);

	  if((index_row+5) >= usr_index_count)
		  index_row--;
	  if(index_row<0) index_row=0;
      if( (index_row+screen_row) > usr_index_count )
		  screen_row--;
      if(screen_row<1) screen_row=1;
	  /*
	  if( (index_row+screen_row) > usr_index_count )
	  {
         int coun = (index_row+screen_row) - usr_index_count;
		 if(index_row-coun<0)
			 index_row=0;
		 else 
			 index_row-=coun;         
	  }
	  if( (index_row+screen_row) > usr_index_count )
		  screen_row -= (index_row+screen_row) - usr_index_count;
	  if(screen_row<1) screen_row=1;
	  */
	  if(!usr_index_count)
		  do_oth_page(INPUT_LCURSOR);
      print_oth_page();
   }
   return(0);
}