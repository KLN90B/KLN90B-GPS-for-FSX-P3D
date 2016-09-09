#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

//==============================================================================
static symbol __video_buffer[7][23];
static int wait_clear;

static int DTO_INPUT_MASK = INPUT_LINNERPLUS|INPUT_LINNERMINUS|
                            INPUT_LOUTERPLUS|INPUT_LOUTERMINUS|
							INPUT_ENTER|INPUT_CLR;
dto_list_t K_dto_list;

BOOL dto_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&DTO_INPUT_MASK);
}
//===============================================================================

static nv_point_t curr_point={0};
static nv_point_t temp_point={0};

static int dto_mode;
enum {DIRECT,ACTIVATE};
static int global_mode;
static int col_e;
const int  COL_START_EDIT = 2;
const int  COL_END_EDIT  = 6;
static int editing;
static long *ids_list;
static long ids_list_count;
static int pre_approve;

extern int  current_item;

nv_point_t *dto_get_cp(void)
{
  return(&curr_point);
}
void dto_change_mode(int MODE)
{
   if( (MODE == DTO_MODE_NORMAL) )
   {
      dto_mode=DTO_MODE_NORMAL;
	  nav_set_mode(DTO_LEG);
   }
   if( (MODE == DTO_MODE_FPL)  )
   {
      dto_mode=DTO_MODE_FPL;
	  nav_set_mode(FPLDTO_LEG);
   }
}
//extern _dto_list_t K_dto_list;
int set_current_point(int page_caller)
{
    dto_mode = DTO_MODE_NORMAL;
	nv_point_t __tp;
	if(K_get_rp_point(&__tp)) 
	{
		temp_point = __tp;
		return(0);
	}
	if(K_is_scanpull())
	{
	   if(K_dto_list.list_count>0 && K_dto_list.list_index>=0)
	   {
		   temp_point = K_dto_list.points[K_dto_list.list_index]; 
		   return(0);
	   }
	}
	if(page_caller == FPL_PAGE)
	{
       nv_point_t *dtofpl_pt = fpl_get_dtofpl();
	   if(dtofpl_pt)
	   {
		   temp_point.buffer_spec = dtofpl_pt->buffer_spec;
		   temp_point.type = dtofpl_pt->type;
		   dto_mode = DTO_MODE_FPL;
		   return(0);
	   }
	}

	if(nav_mode() == FPLDTO_LEG)
		dto_mode = DTO_MODE_FPL;
	rt_leg_t *__rt;
	__rt = get_rt_leg();
	if(__rt)
	   temp_point = *__rt->dstp;
	else
	   temp_point.buffer_spec = NULL;
    return(0);
}
void print_dto_page(void)
{
	print_str(0,0,ATTRIBUTE_NORMAL,"%s",global_mode == DIRECT ? "DIRECT TO: " : "ACTIVATE:  ");
	print_str(1,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	if(!editing)
	{
	   print_str(2,0,ATTRIBUTE_NORMAL,"  %-5s    ",temp_point.buffer_spec?((nv_hdr_t *)(temp_point.buffer_spec))->ICAO_ID:"-----");
	   K_change_attribute(2,2,5,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
	}
	print_str(3,0,ATTRIBUTE_NORMAL,"%-11s"," ");
    print_str(4,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(5,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	print_str(6,0,ATTRIBUTE_INVERSE," CRSR");
	K_change_attribute(6,0,1,ATTRIBUTE_NORMAL);

	if(temp_point.buffer_spec || wait_clear)
		//print_str(6,14,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH,"ENT");
		do_status_line(ENABLE_ENT,NULL);
	else
		//print_str(6,14,ATTRIBUTE_NORMAL,"   ");
		do_status_line(DISABLE_ENT,NULL);
}
nv_hdr_t *get_dtofpl(void)
{
   return(NULL);
}

void dto_set(nv_point_t *__pt)
{
   curr_point = *__pt;
   double Lat,Lon;
   nv_hdr_t __ppos_hdr;
   nv_point_t __ppos_point;
   get_PPOS(&__ppos_hdr.Lat,&__ppos_hdr.Lon);
   __ppos_hdr.ICAO_ID[0] = '\0';
   __ppos_point.buffer_spec = &__ppos_hdr;
   __ppos_point.type = -1;
   set_route_leg(&__ppos_point,&curr_point,DTO_LEG);
}

void final_dto(int page_called)
{

	do_status_line(DISABLE_ENT,NULL);
	if(temp_point.buffer_spec)
	{
		curr_point = temp_point;
		double Lat,Lon;
		nv_hdr_t __ppos_hdr;
		nv_point_t __ppos_point;
		get_PPOS(&__ppos_hdr.Lat,&__ppos_hdr.Lon);
		__ppos_hdr.ICAO_ID[0] = '\0';
		__ppos_point.buffer_spec = &__ppos_hdr;
		__ppos_point.type = -1;
		//if(dto_mode==DTO_MODE_NORMAL)
			set_route_leg(&__ppos_point,&curr_point,DTO_LEG);
		//else
		//	set_route_leg(&__ppos_point,&curr_point,FPLDTO_LEG);
		rt_leg_t *__rt = get_rt_leg();
		if(__rt)
		{
			if(mod_get_mode() == OBS_MODE)
			{
				if(global_mode == DIRECT)
				{ 
					mod_set_obs_value(__add_deg(__rt->OD,-get_MAGVAR()),__rt);
					char str[12];
					sprintf(str,"\x1> CRS %03d ",K_ftol(mod_get_obs()));
					do_status_line(ADD_STATUS_MESSAGE,str);
				}
				else
				{
					mod_set_obs_value(mod_get_obs(),__rt);
				}
			}
		}
		sr_screen_to_buf(SCREEN_RESTORE,__video_buffer);
		switch_lp(page_called,-1);
        if((page_called == NAV_PAGE && get_navpage_number(PAGE_LEFT)==5)&&(get_rp_type() == NAV_PAGE && get_navpage_number(PAGE_RIGHT)==5))
		   switch_rp(NAV_PAGE,5);	
		else
		   switch_rp(NAV_PAGE,1);
	}
	else
	{
		sr_screen_to_buf(SCREEN_RESTORE,__video_buffer);
		switch_lp(page_called,-1);
		if(wait_clear)
		{
			if(mod_get_mode() != OBS_MODE)
			{
				curr_point = temp_point;
				if(dto_mode == DTO_MODE_NORMAL)
					set_route_leg(NULL,NULL,DTO_LEG);
				else
					set_route_leg(NULL,NULL,FPLDTO_LEG);

			}
			wait_clear=0;
		}
	}
}

void show_point_from_list(int index,nv_point_t *__pt=NULL)
{
   if(__pt)
   {
      temp_point = *__pt;
	  show_nv_point(__pt);
   }  
   else
      temp_point = *(show_active_point(ids_list[index]));
   K_change_attribute(2,COL_START_EDIT,5,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
   do_status_line(ENABLE_ENT,NULL);
   update_screen();
   pre_approve = 1;
   block_all(DTO_PAGE);
}

int do_dto_page(int ACTION,DWORD extra)
{
    static int page_called;
	if(ACTION == ACTION_INIT)
	{
	   page_called = extra;
	   temp_point = curr_point;
	   set_current_point(page_called);
	   sr_screen_to_buf(SCREEN_SAVE,__video_buffer);
	   wait_clear=0;
	   global_mode=DIRECT;
	   editing=0;
	   pre_approve = temp_point.buffer_spec ? 0 : 1;
	}
    if(ACTION == INPUT_DTO)
	{
	   if(mod_get_mode()==OBS_MODE)
	   {
	      if(global_mode == DIRECT) global_mode = ACTIVATE;
		  else if(global_mode == ACTIVATE) global_mode = DIRECT;
	   }
	}
	if(ACTION == ACTION_TIMER)
	{
	    print_dto_page();
		update_screen();
	}
	if(ACTION == INPUT_LINNERPLUS || ACTION == INPUT_LINNERMINUS)
	{
       if(editing && pre_approve) return(0);

	   if(!K_is_scanpull())
	   {
	      if(!editing)
	      {
             col_e = COL_START_EDIT;
			 editing=1;
			 pre_approve=0;
		  }
          char curr_char = K_get_char(2,col_e);
		  if(editing || char_type(curr_char) != CHAR_ALPHA_NUM)
		  curr_char = ACTION == INPUT_LINNERPLUS ? K_next_char(curr_char) : K_prev_char(curr_char);
          K_set_char(2,col_e,curr_char);
		  K_change_attribute(2,col_e,1,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE);
		  //======================================================
 		  char __ICAO_ID[6];
		  K_get_string(2,COL_START_EDIT,col_e,__ICAO_ID);
		  //======================================================
		  ids_list = find_ids_by_icao(__ICAO_ID,&ids_list_count);
		  if(ids_list)
		  {
             get_icao_by_id(ids_list[0],__ICAO_ID);
			 print_str(2,COL_START_EDIT,ATTRIBUTE_NORMAL,"%-7s",__ICAO_ID);
			 K_change_attribute(2,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
			 K_change_attribute(2,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
             do_status_line(ENABLE_ENT,NULL);
		   }
		   else
		   {
			  print_str(2,COL_START_EDIT,ATTRIBUTE_NORMAL,"%-7s",__ICAO_ID);
		      K_change_attribute(2,COL_START_EDIT,5,ATTRIBUTE_INVERSE);
		      K_change_attribute(2,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
		      do_status_line(ENABLE_ENT,NULL);
		    }
            update_screen();	
	   }
	   else  // K_scan_is_pulled out
	   {
	     if(K_dto_list.list_count<1) return(0);
         
		 if(editing)
   	        editing=0;
		 else
		    ACTION == INPUT_LINNERPLUS ? dto_list_next() : dto_list_prev();
		 temp_point = K_dto_list.points[K_dto_list.list_index];   
		 pre_approve=0;
	   }
   }
//===============================================================================================
	if(ACTION == INPUT_LOUTERPLUS)
	{
		if(editing && !pre_approve)
		{
			if( (K_get_char(2,col_e)!=BLANK_CHAR) && (col_e<COL_END_EDIT) )
			{
				K_change_attribute(2,col_e,1,ATTRIBUTE_INVERSE);
				col_e++;
				K_change_attribute(2,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
			}
		}
	}
	if(ACTION == INPUT_LOUTERMINUS)
	{
	   if(editing && !pre_approve)
	   {
	      if(col_e > COL_START_EDIT)
		  {
		     K_change_attribute(2,col_e,1,ATTRIBUTE_INVERSE);
			 col_e--;
			 K_change_attribute(2,col_e,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
           }
	   }
	}
//===============================================================================================
    if(ACTION == ACTION_SUP_PT_CREATED)
	{
	   if(editing)
	   {
		   temp_point.buffer_spec = get_current_sup();
		   temp_point.type = NAVAID_SUP;
		   unblock_all();
		   final_dto(page_called);   
	   }
	}
	if(ACTION == INPUT_ENTER)
	{
		if(pre_approve)
		{
		   unblock_all();
		   final_dto(page_called);   
		}
		else
		{
		   if(editing)
		   {
			   if(ids_list)
			   {
				   if(ids_list_count>1)
				   {
					   update_screen();
					   fpl_build_ml_list(ids_list_count,ids_list);
					   set_ml_mode(DTO_PAGE);
				   }
				   else
				      show_point_from_list(0);
			   }
			   else 
			   {
				  char __ICAO_ID[6];
				  K_get_string(2,COL_START_EDIT,col_e,__ICAO_ID);
                  if(!sup_is_creating()) 
				  {
				     switch_rp(SUP_PAGE,0); 
					 sup_create_new_point(__ICAO_ID);
				     return(-1);
				  }
			      //switch_rp(SUP_PAGE,0);
				  //sup_create_new_point(__ICAO_ID);
			   }
		   }
		   else
	          show_point_from_list(0,&temp_point);
		}
	}
	if(ACTION == INPUT_CLR)
	{
		unblock_all();
		if(temp_point.buffer_spec)
		{
			temp_point.buffer_spec=NULL;
			wait_clear=1;
			pre_approve=1;
			editing=0;
		}
		else
		{
			sr_screen_to_buf(SCREEN_RESTORE,__video_buffer);
			switch_lp(page_called,-1);
			do_status_line(DISABLE_ENT,NULL);
		}	
	}
	if(ACTION == MULTI_LINE_SELECTED)
	{
		if(current_item == -1)
		   do_dto_page(INPUT_CLR,0);
		else
		   show_point_from_list(current_item);
	}
	if(ACTION == ACTION_FPLDTO_DONE)
	{
		if(dto_mode == DTO_MODE_FPL && curr_point.buffer_spec && nav_mode() == FPLDTO_LEG)
		{
	       dto_mode = DTO_MODE_NORMAL;
           curr_point.buffer_spec = NULL;
		   set_route_leg(NULL,NULL,FPLDTO_LEG);
		}
	}
	if(ACTION == ACTION_FREE_RESOURCES)
 	{
		curr_point.buffer_spec = NULL;
	}
	return(0);
}
extern fpl_t fpls[];

void build_dto_list(void)
{
	int check_index = 0;
	K_dto_list.list_index=0;
	if(nav_mode()==DTO_LEG)
	{
		nv_point_t *dto_pt = dto_get_cp();
		memmove(&K_dto_list.points[0],dto_pt,sizeof(nv_point_t));
		K_dto_list.list_index = 0;
		check_index++;
	}
	int fpl0_points = fpl_get_real_points(0);
	int fpl0_act = fpl_get_active_leg();

	K_dto_list.list_count=fpl0_points+check_index;
      
	for(int fpl0_index=0;fpl0_index<fpls[0].point_in_use;fpl0_index++)
	{
		if(!fpls[0].points[fpl0_index].buffer_spec) continue;
		memmove(&K_dto_list.points[check_index],&fpls[0].points[fpl0_index],sizeof(nv_point_t));
		if(fpl0_act == fpl0_index && fpl0_act>0) K_dto_list.list_index=fpl0_act;
		check_index++;
	}
}

BOOL is_dto_list_changed(void)
{
   int check_index = 0;
	if(nav_mode()==DTO_LEG)
	{
	   nv_point_t *dto_pt = dto_get_cp();
	   if(K_dto_list.list_count<1) return(TRUE);
	   if(K_dto_list.points[0].buffer_spec != dto_pt->buffer_spec) return(TRUE);
	   check_index++;
	}
    int fpl0_points = fpl_get_real_points(0);
	if(fpl0_points+check_index != K_dto_list.list_count) return(TRUE);
	for(int fpl0_index=0;fpl0_index<fpls[0].point_in_use;fpl0_index++)
	{
	    if(!fpls[0].points[fpl0_index].buffer_spec) continue;
		if(K_dto_list.points[check_index].buffer_spec!=fpls[0].points[fpl0_index].buffer_spec)
			return(TRUE);
		check_index++;
	}
	return(FALSE);
}

void dto_update_list()
{
   if(is_dto_list_changed())
	   build_dto_list();
}
void dto_init_list(void)
{
  K_dto_list.list_count=0;
}
void dto_list_next(void)
{
  if(!K_dto_list.list_count) return;
  K_dto_list.list_index++;
  if(K_dto_list.list_index>=K_dto_list.list_count) K_dto_list.list_index=0;
}
void dto_list_prev(void)
{
	if(!K_dto_list.list_count) return;
	K_dto_list.list_index--;
	if(!K_dto_list.list_index) K_dto_list.list_index=K_dto_list.list_count-1;
}
