#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "calc_pages.h"
#include <stdio.h>
#include <math.h>


int external_request;
extern int force_nr_mode;

extern fpl_t fpls[];
extern fpl_t sim_gps_fpl;


void do_request(void)
{
    if(external_request)
	{
        switch(external_request)	   
		{
		case REQUEST_LOADFPL0:
			{
			   extern CRITICAL_SECTION cs_afp_fpl0;
			   EnterCriticalSection(&cs_afp_fpl0);
			      if(nav_mode() == FPLDTO_LEG)
					  set_route_leg(NULL,NULL,FPLDTO_LEG);
			      copy_fpl(&fpls[0],&sim_gps_fpl);
			      Save_FPL(&fpls[0],0);
			      fpl_set_navigation();
			   LeaveCriticalSection(&cs_afp_fpl0);
			}
			break;			
		}
		external_request=0;
	}
}

int count_of_elem;
char str_elems[100][12];
int MULTI_LINE_CALLER;
int current_item;

static BOOL __is_scanpull = 0;

void toggle_scanpull(void)
{
   __is_scanpull=1-__is_scanpull;
}
BOOL K_is_scanpull(void)
{
   return(__is_scanpull);
}

static BOOL __is_apon = 0;

void toggle_apon(void)
{
	__is_apon = 1 - __is_apon;
	UseDefAP = __is_apon;
}
BOOL K_is_apon(void)
{
	UseDefAP = __is_apon;
	return(__is_apon);
}
//============================================================================================
void multi_line_scrool_down(int item_in_list)
{
    item_in_list-=3;
	for(int i=0;i<3;i++)
		print_str(2+i,0,ATTRIBUTE_NORMAL,str_elems[item_in_list++]);
	print_str(5,0,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE,str_elems[item_in_list]);
	update_screen();
}

void multi_line_scrool_up(int item_in_list)
{
	print_str(2,0,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE,str_elems[item_in_list++]);
	for(int i=0;i<3;i++)
		print_str(3+i,0,ATTRIBUTE_NORMAL,str_elems[item_in_list++]);
	update_screen();
}

int multi_line(int ACTION)
{
    int i;
	static int item_in_list;
	static int item_on_screen;

	if(ACTION == ACTION_INIT)
	{
		sr_screen(SCREEN_SAVE);
		for(i=0;i<6;i++)
			print_str(i,0,ATTRIBUTE_NORMAL,"%11s"," ");

		print_str(0,1,ATTRIBUTE_NORMAL,"%-5s%5d",str_elems[99],count_of_elem);
		print_str(1,2,ATTRIBUTE_NORMAL,"TYPE AREA");
		print_str(6,0,ATTRIBUTE_NORMAL," ");
		print_str(6,1,ATTRIBUTE_INVERSE,"CRSR");
		for(i=0;i<4 && count_of_elem > i;i++)
			print_str(2+i,0,ATTRIBUTE_NORMAL,"%s",str_elems[i]);
        print_str(2,0,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE,str_elems[0]);
		update_screen();
		item_in_list = 0;
		item_on_screen = 2;
	}
    if(ACTION == INPUT_LOUTERPLUS)
	{
        if((item_on_screen < 5) && (item_in_list < (count_of_elem-1)))
		{
//		    if(item_in_list < (count_of_elem-1))
//			{
               print_str(item_on_screen++,0,ATTRIBUTE_NORMAL,str_elems[item_in_list++]);
               print_str(item_on_screen,0,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE,
     			         str_elems[item_in_list]);
			   update_screen();
//			}

		}
		else if(item_in_list < (count_of_elem-1))
		{
		   multi_line_scrool_down(++item_in_list);
		}
		else
		{
		    item_on_screen = 2;
			item_in_list   = 0;
			print_str(item_on_screen,0,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE,str_elems[item_in_list]);
			for(int i=0;i<3 && (item_in_list+i+1)<count_of_elem;i++)
			{
			    print_str(3+i,0,ATTRIBUTE_NORMAL,str_elems[item_in_list+i+1]);
			}
			update_screen();
		}
	}
	//==============================================================================
    if(ACTION == INPUT_LOUTERMINUS)
	{
        if(item_on_screen > 2)
		{
		    if(item_in_list > 0)
			{
               print_str(item_on_screen--,0,ATTRIBUTE_NORMAL,str_elems[item_in_list--]);
               print_str(item_on_screen,0,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE,
				         str_elems[item_in_list]);
			   update_screen();
			}
		}
		else if(item_in_list > 0)
		{
	       multi_line_scrool_up(--item_in_list);
		}
		else
		{
		   // Wrap Arround 
           item_in_list = count_of_elem-1;
		   item_on_screen = count_of_elem < 4 ? count_of_elem + 1:5;
		   print_str(item_on_screen,0,ATTRIBUTE_FLASH|ATTRIBUTE_INVERSE,str_elems[item_in_list]);
		   int j = item_in_list-1;
		   for(int i = item_on_screen-1;i>=2;i--,j--)
		   {
		       print_str(i,0,ATTRIBUTE_NORMAL,str_elems[j]);
		   }
		   update_screen();
		}
	}
    //=============================================================================
	if(ACTION == INPUT_ENTER || ACTION == INPUT_CLR)
	{
	   sr_screen(SCREEN_RESTORE);
	   current_item = ACTION == INPUT_CLR ? -1 : item_in_list;

	   switch(MULTI_LINE_CALLER)
	   {
	   case FPL_PAGE:
		   do_fpl_pages(MULTI_LINE_SELECTED);
		   break;
	   case DTO_PAGE:
		   do_dto_page(MULTI_LINE_SELECTED,0);
		   break;
	   }
	   return(1);
	}
	return(0);
}

extern INT GLOBAL_STATE;
extern INT SCREEN_FORMAT;
const int right_pages = 8;
const int right_pages_arr[right_pages]={ACT_PAGE,DT_PAGE,NAV_PAGE,APT_PAGE,VOR_PAGE,NDB_PAGE,WPT_PAGE,SUP_PAGE};
int inc_right_page(int page)
{
	return(page == right_pages-1?0:page+1);
}
int dec_right_page(int page)
{
	return(page == 0?right_pages-1:page-1);
}
//***************************************************************************
static int right_page_index = 2;
static int r_cursored = 0;

static int left_page_index = 2;
const int left_pages = 6;
const int global_left_pages = 2;
const int left_pages_arr[left_pages+global_left_pages]={MOD_PAGE,FPL_PAGE,NAV_PAGE,CALC_PAGE,SET_PAGE,OTH_PAGE,DTO_PAGE,MSG_PAGE};

static int is_multi_line = 0;
static int is_blocked = 0;
static int page_blocker;

BOOL IsBlocked(void)
{
  return(is_blocked);
}


void block_all(int page)
{
   is_blocked = 1;
   page_blocker = page;
}
void do_blocked(int ACTION)
{
   switch(page_blocker)
   {
   case FPL_PAGE:
	   do_fpl_pages(ACTION);
	   break;
   case DTO_PAGE:
	   do_dto_page(ACTION,0);
	   break;
   case SET_PAGE:
	   do_set_page(ACTION);
	   break;
   case OTH_PAGE:
	   do_oth_page(ACTION);
	   break;
   }
}
void unblock_all(void)
{
   is_blocked = 0;
}

int get_lp_type(void)
{
   return(left_pages_arr[left_page_index]);
}
int get_rp_type(void)
{
  return(right_pages_arr[right_page_index]);
}
//****************************************************************************
void switch_lp(int page_type,int extra)
{
	left_page_index = find_of((int *)left_pages_arr,page_type,left_pages+global_left_pages);
	if(page_type == NAV_PAGE)
		set_navpage_number(PAGE_LEFT,extra);
	check_super();
}
void switch_rp(int page_type,int extra)
{
	switch(right_pages_arr[right_page_index])
	{
	case NDB_PAGE:
		{
		  do_ndb_page(PAGE_FORCE_CHANGE);
		  break;
		}
	case VOR_PAGE:
		{
		  do_vor_page(PAGE_FORCE_CHANGE);
		  break;
		}
	case APT_PAGE:
		{
		  do_apt_page(PAGE_FORCE_CHANGE);
		  break;
		}
	case WPT_PAGE:
		{
		  do_wpt_page(PAGE_FORCE_CHANGE);
		  break;
		}
	case SUP_PAGE:
		{
		  do_sup_page(PAGE_FORCE_CHANGE);
		  break;
		}
	}
	right_page_index = find_of((int *)right_pages_arr,page_type,right_pages);
	if(right_pages_arr[right_page_index] == NAV_PAGE)
	{
		   set_navpage_number(PAGE_RIGHT,extra);
	}
	check_super();
}
//=============================================================================
int lp_kind(void)
{
   if(left_pages_arr[left_page_index] == MSG_PAGE)
	   return(FRONT_PAGE);
   return(REGULAR_PAGE);
}
int inc_left_page(int page)
{
	if(left_pages_arr[left_page_index] == MOD_PAGE)
		do_status_line(DISABLE_ENT,NULL);
	return(page == left_pages-1?0:page+1);
}
int dec_left_page(int page)
{
	if(left_pages_arr[left_page_index] == MOD_PAGE)
		do_status_line(DISABLE_ENT,NULL);
	return(page == 0?left_pages-1:page-1);
}

void changed_l_page(void)
{
	if(left_pages_arr[left_page_index]==OTH_PAGE)
		do_oth_page(ACTION_SHOW);
}

void L_input_handler(int ACTION)
{
    BOOL change=FALSE;
	if(ACTION == INPUT_LOUTERPLUS)
	{
		left_page_index = inc_left_page(left_page_index);
	    change=TRUE;
	}
    if(ACTION == INPUT_LOUTERMINUS)
	{
		left_page_index = dec_left_page(left_page_index);
		change=TRUE;
	}
	if(change)
		changed_l_page();
	check_super();
}
void changed_r_page(void)
{
   if(right_pages_arr[right_page_index] == ACT_PAGE)
	   do_act_page(ACTION_SHOW);
   if(right_pages_arr[right_page_index]==NDB_PAGE)
	   do_ndb_page(ACTION_SHOW);
   if(right_pages_arr[right_page_index]==VOR_PAGE)
	   do_vor_page(ACTION_SHOW);
   if(right_pages_arr[right_page_index]==APT_PAGE)
	   do_apt_page(ACTION_SHOW);
   if(right_pages_arr[right_page_index]==WPT_PAGE)
	   do_wpt_page(ACTION_SHOW);
   if(right_pages_arr[right_page_index]==SUP_PAGE)
	   do_sup_page(ACTION_SHOW);
}
void R_input_handler(int ACTION)
{
    BOOL change=FALSE;
	if(ACTION == INPUT_ROUTERPLUS)
	{
		right_page_index = inc_right_page(right_page_index);
		change=TRUE;
	}
    if(ACTION == INPUT_ROUTERMINUS)
	{
		right_page_index = dec_right_page(right_page_index);
		change=TRUE;
	}
	check_super();
	if(change)
	   changed_r_page();
}
//=================================================================================
void set_ml_mode(int page)
{
   multi_line(ACTION_INIT);
   is_multi_line=1;
   MULTI_LINE_CALLER=page;
}

void cancel_msg_page(saved_pages_t *saved_pages)
{
   do_msg_page(ACTION_HIDE);
   switch_lp(saved_pages->lp,0);
}

void main_loop(int ACTION)
{
   static int dto_active = 0;
   static saved_pages_t saved_pages;
   if(ACTION == ACTION_INIT)
   {
        left_page_index = find_of((int *)left_pages_arr,NAV_PAGE,left_pages+global_left_pages);
	    right_page_index = find_of((int *)right_pages_arr,APT_PAGE,right_pages);
		dto_active = r_cursored = is_multi_line = is_blocked = external_request = 0;
		clear_screen();
		print_str(0,0,ATTRIBUTE_NORMAL,"PRESENT POS PRESENT POS");
		print_str(2,0,ATTRIBUTE_NORMAL,"---  ---%cFR ---  ---%cFR",0xB0,0xB0);
		print_str(3,0,ATTRIBUTE_NORMAL,"   ----.-NM    ----.-NM");
		print_str(4,0,ATTRIBUTE_NORMAL,"- --%c--.--' - --%c--.--'",0xB0,0xB0);
		print_str(5,0,ATTRIBUTE_NORMAL,"----%c--.--' ----%c--.--'",0xB0,0xB0);
		print_str(6,0,ATTRIBUTE_NORMAL,"NAV 2 ENR-LEG     NAV 2");
		K_change_attribute(6,6,7,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL);
        SCREEN_FORMAT = FORMAT_TWOPARTS;
		update_screen();
		return;
	}
    if(is_multi_line)
	{
	   int ret = multi_line(ACTION);
	   if(ret == 1)
	   {
	      is_multi_line=0;
		  r_cursored=0;
	   }
	   return;
	}
	if(is_blocked)
	{
	    do_blocked(ACTION);
		return;
	}
	//*******************************************************************************
	if(ACTION == INPUT_DTO)
	{
		if(left_pages_arr[left_page_index] != DTO_PAGE)
		{
			if(left_pages_arr[left_page_index] == MSG_PAGE) cancel_msg_page(&saved_pages);
			do_dto_page(ACTION_INIT,left_pages_arr[left_page_index]);
			left_page_index = find_of((int *)left_pages_arr,DTO_PAGE,left_pages+global_left_pages);		
			check_super();
		}
		else
           do_dto_page(INPUT_DTO,0);
		return;
	}
	if(ACTION == INPUT_MSG)
	{
		if(left_pages_arr[left_page_index] != MSG_PAGE)
		{
		   saved_pages.lp = left_pages_arr[left_page_index]; saved_pages.rp = left_pages_arr[right_page_index];
		   do_msg_page(ACTION_SHOW);
           switch_lp(MSG_PAGE,0);
		}
		else
		{
		   int ret = do_msg_page(ACTION);
		   if(ret == NO_MORE_MESSAGES)
			   cancel_msg_page(&saved_pages);
		}
	}
    //==== Timer ============================================================
	if(ACTION == ACTION_TIMER)
	{
	  //************** Left Part Of Screen ****************
		 switch(left_pages_arr[left_page_index])
	     {
	     case NAV_PAGE:
		    do_nav_page(ACTION,PAGE_LEFT);
		    break;
	     case FPL_PAGE:
		    do_fpl_pages(ACTION);
		    break;
		 case DTO_PAGE:
			 do_dto_page(ACTION,0);
			 break;
		 case MOD_PAGE:
			 do_mod_page(ACTION);
			 break;
		 case OTH_PAGE:
			 do_oth_page(ACTION);
			 break;
		 case SET_PAGE:
			 do_set_page(ACTION);
			 break;
		 case MSG_PAGE:
			 do_msg_page(ACTION);
			 break;
		 case CALC_PAGE:
			 do_calc_page(ACTION);
			 break;

		 }
	  //************** Right Part Of Screen ****************
	     if( left_pages_arr[left_page_index] != MSG_PAGE )
		 {
		    switch(right_pages_arr[right_page_index])
	        {
	        case NAV_PAGE:
		       do_nav_page(ACTION,PAGE_RIGHT);
			   break;
		    case NDB_PAGE:
			   do_ndb_page(ACTION);
		       break;
		    case VOR_PAGE:
			   do_vor_page(ACTION);
			   break;
		    case APT_PAGE:
			   do_apt_page(ACTION);
			   break;
		    case WPT_PAGE:
			   do_wpt_page(ACTION);
			   break;
		    case SUP_PAGE:
			   do_sup_page(ACTION);
			   break;
			case DT_PAGE:
				do_dt_pages(ACTION);
				break;
			case ACT_PAGE:
				do_act_page(ACTION);
				break;
			}
	     }
	  do_request();
	  if(K_is_scanpull())
		  dto_update_list();
      return;
	}
	//=======================================================================
	if(ACTION == INPUT_PULLSCAN)
	{
	   toggle_scanpull();
	   do_ndb_page(INPUT_PULLSCAN);
	   do_vor_page(INPUT_PULLSCAN);
	   do_apt_page(INPUT_PULLSCAN);
	   do_sup_page(INPUT_PULLSCAN);
	   do_wpt_page(INPUT_PULLSCAN);
	   if(K_is_scanpull())
		   dto_update_list();
	   return;
	}
	//======================= INPUT EVENTS ==================================
	if( 
		ACTION == INPUT_LINNERPLUS  ||
		ACTION == INPUT_LINNERMINUS ||
		ACTION == INPUT_LOUTERPLUS  ||
		ACTION == INPUT_LOUTERMINUS ||
		ACTION == INPUT_ENTER       ||
		ACTION == INPUT_CLR         ||
		ACTION == INPUT_LCURSOR
	  )
	{
		if (left_pages_arr[left_page_index] == MSG_PAGE) {
			cancel_msg_page(&saved_pages);
			if (ACTION == INPUT_ENTER) {
				force_nr_mode = 1;
				switch_rp(APT_PAGE, 1);
			}
		}
		  BOOL input_handled = FALSE;

		   switch(left_pages_arr[left_page_index])
		   {
		   case  NAV_PAGE:
			   {   
                  input_handled=nav_handle_key(ACTION);
				  if(input_handled)
				     do_nav_page(ACTION,PAGE_LEFT);
			   }
			   break;
		   case  DTO_PAGE:
			   {
                  input_handled=dto_handle_key(ACTION);
				   if(input_handled)
				   {
				     if(-1 == do_dto_page(ACTION,0))
						 return;
				   }
			   }
			   break;
		   case  FPL_PAGE:
			   {
				   input_handled=fpl_handle_key(ACTION);
				   if(input_handled)
				   {
					   if(-1 == do_fpl_pages(ACTION))
					      return;
				   }
			   }
			   break;
		   case  MOD_PAGE:
			   {
				   input_handled=mod_handle_key(ACTION);
				   if(input_handled)
					   do_mod_page(ACTION);
			   }
			   break;
		   case  SET_PAGE:
			   {
				   input_handled=set_handle_key(ACTION);
				   if(input_handled)
					   do_set_page(ACTION);
			   }
			   break;
		   case  OTH_PAGE:
			   {
				   input_handled=oth_handle_key(ACTION);
				   if(input_handled)
					   do_oth_page(ACTION);
			   }
			   break;
		   case  CALC_PAGE:
			   {   
				   input_handled=calc_handle_key(ACTION);
				   if(input_handled)
					   do_calc_page(ACTION);
			   }
			   break;
		   }
		   if(input_handled == FALSE)
			   L_input_handler(ACTION);
	}
	//******************** Right screen part ******************************************
	if(
		ACTION == INPUT_RINNERPLUS  || 
        ACTION == INPUT_RINNERMINUS ||
        ACTION == INPUT_ROUTERMINUS ||
	    ACTION == INPUT_ROUTERPLUS  ||
		ACTION == INPUT_RCURSOR     ||
		ACTION == INPUT_ENTER       ||
		ACTION == INPUT_CLR
	   )
	{
        //*******************************************************************************
		if (left_pages_arr[left_page_index] == MSG_PAGE) {
			cancel_msg_page(&saved_pages);
			if (ACTION == INPUT_ENTER) {
				force_nr_mode = 1;
				switch_rp(APT_PAGE, 1);
			}
		}

		BOOL input_handled = FALSE;

		switch(right_pages_arr[right_page_index])
		{
		case  NAV_PAGE:
			   {   
                  input_handled=nav_handle2_key(ACTION);
				  if(input_handled)
				     do_nav_page(ACTION,PAGE_RIGHT);
			   }
			   break;
		case NDB_PAGE:
			{
              input_handled=ndb_handle_key(ACTION);
			  if(input_handled)
			     do_ndb_page(ACTION);
			}
			break;
		case VOR_PAGE:
			{
              input_handled=vor_handle_key(ACTION);
			  if(input_handled)
			     do_vor_page(ACTION);
			}
			break;
		case APT_PAGE:
			{
              input_handled=apt_handle_key(ACTION);
			  if(input_handled)
			     do_apt_page(ACTION);
			}
			break;
		case WPT_PAGE:
			{
              input_handled=int_handle_key(ACTION);
			  if(input_handled)
			     do_wpt_page(ACTION);
			}
			break;
		case SUP_PAGE:
			{
				input_handled=sup_handle_key(ACTION);
				if(input_handled)
					do_sup_page(ACTION);
			}
			break;
		case DT_PAGE:
			{
				input_handled=dt_handle_key(ACTION);
				if(input_handled)
					do_dt_pages(ACTION);
			}
			break;
		case ACT_PAGE:
			{
				input_handled=act_handle_key(ACTION);
				if(input_handled)
					do_act_page(ACTION);
			}
			break;
		}
		if(input_handled == FALSE)
		   R_input_handler(ACTION);
        //*******************************************************************************
	}
}

