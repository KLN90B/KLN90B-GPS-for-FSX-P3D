#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

extern nav_db_t nav_db;


nvdb_wpt *current_wpt = NULL;

enum {COMPLETE_LIST,NR_LIST};
static DWORD current_list = COMPLETE_LIST;
static DWORD  wpt_list_index=0;


extern int count_of_elem;
extern char str_elems[99][12];
extern int MULTI_LINE_CALLER;
extern int current_item;

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;

static int INT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;

BOOL int_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&INT_INPUT_MASK);
}
//=========================================================================================================
int wpt_compare(nvdb_wpt *wpt1,nvdb_wpt *wpt2)
{
		if(    
			(wpt1->Lat==wpt2->Lat)
		    &&
			(wpt1->Lon==wpt2->Lon)
			&&
			!(strcmp(wpt1->ICAO_ID,wpt2->ICAO_ID))
		  )
		  return(0);

	return(1);
}


long get_wpt_index(long *wpt_id,int elements_count,nvdb_wpt *__wpt)
{
	for(int i=0;i<elements_count;i++)
	{
		if(!wpt_compare(&nav_db.wpts[wpt_id[i]],__wpt))
		   return(i);
	}
	return(-1);
}

int wpt_dup_sort(const void *a1,const void *a2)
{
    FLOAT64 Lon,Lat;
    get_PPOS(&Lat,&Lon);
	return(
	       get_S(Lat,Lon,nav_db.wpts[*(long *)a1].Lat,nav_db.wpts[*(long *)a1].Lon)
	       -
	       get_S(Lat,Lon,nav_db.wpts[*(long *)a2].Lat,nav_db.wpts[*(long *)a2].Lon)
	      );
}

long *find_wpts_by_icao(char *icao_id_s,long *array_size)
{
    static DWORD wpt_indexes[1024];
	int i;
	DWORD icao_id = icao2id(icao_id_s);
	i=0;
	if(icao_index.find(icao_id) != icao_index.end())
	{
		i=0;
		for(vector<int>::iterator it = icao_index[icao_id].begin();it!=icao_index[icao_id].end();it++)
		{
		  int ind = *it;
		  long local_index;
		  if(is_wpt_id(ind,&local_index))
		  {
              wpt_indexes[i++] = local_index;
		  }
		}
	}
	if(i>0)
	{
	   if(array_size) *array_size=i;
	   return((long *)&wpt_indexes);
	}
	
	char reg_exp[15];
	sprintf(reg_exp,"^%s",icao_id_s);
	Regexp re(reg_exp);
    re.CompiledOK();

	for(int j=0;j<nav_db.wpts_count;j++)
	{
		if(re.Match(nav_db.wpts[j].ICAO_ID))
		{
           if(array_size) 
		      *array_size=1;
		   wpt_indexes[0]=j;
		   return((long *)&wpt_indexes);
		}
	}
	return(NULL);
}

void print_wpt(nvdb_wpt *__wpt,char *status)
{
		if(!__wpt)
		{
		  print_str(0,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		  print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		  print_str(2,12,ATTRIBUTE_NORMAL,"CREATE NEW ");
		  print_str(3,12,ATTRIBUTE_NORMAL,"WPT AT:    ");
		  print_str(4,12,ATTRIBUTE_NORMAL,"USER POS?  ");
		  print_str(5,12,ATTRIBUTE_NORMAL,"PRES POS?  ");
		  print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
		  return;
		}

  	    print_str(0,12,ATTRIBUTE_NORMAL," %-10s",__wpt->ICAO_ID);
		//************************************************************
		print_str(1,12,ATTRIBUTE_NORMAL,"REF:  -----");
		print_str(2,12,ATTRIBUTE_NORMAL,"RAD: ---.-%c",0xB0);
		print_str(3,12,ATTRIBUTE_NORMAL,"DIS:---.-NM");
        //************************************************************
		K_deg deg;
		K_GetDegMin(__wpt->Lat,&deg);
        print_str(4,12,ATTRIBUTE_NORMAL,"%c %.2d%c%.2d.%.2d'",__wpt->Lat<0?'S':'N',(int)fabs(__wpt->Lat),0xB0,deg.mins,deg.dec_mins);
		K_GetDegMin(__wpt->Lon,&deg);
        print_str(5,12,ATTRIBUTE_NORMAL,"%c%.3d%c%.2d.%.2d'",__wpt->Lon<0?'W':'E',(int)fabs(__wpt->Lon),0xB0,deg.mins,deg.dec_mins);
        print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
		print_navaid_arrow(__wpt);
}

int do_wpt_page(int ACTION)
{
    static int cursored = 0;
	static int current_col = 0;
	static int current_row = 0;
    static long written = 0;  
    static long *wpt_id = NULL;
	static int dup_ident = 0;
	static DWORD dup_ident_start = 0;
	static int dup_index = 0;

	int icao_ed_col = 13;
	int icao_ed_row = 0;
	int name_ed_col = 12;
	int name_ed_row = 1;

	if(ACTION == ACTION_INIT)
	{
	   cursored=0;
	   current_list=COMPLETE_LIST;
	   INT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
	   return(0);
	}
	if(ACTION == ACTION_SHOW)
	{
		if(!nav_db.wpts_count)
			do_status_line(ADD_STATUS_MESSAGE,"NO INT WPTS");
	}
	if(!nav_db.ndbs_count)
	{
		current_wpt=NULL;
		current_list=COMPLETE_LIST;
		print_ndb(NULL,"INT  ");
		return(0);
	}

    if(ACTION == PAGE_FORCE_CHANGE)
	{
       if(cursored && current_list == COMPLETE_LIST)
       do_wpt_page(INPUT_RCURSOR);
	   return(0);
	}

	if(ACTION == ACTION_FREE_RESOURCES)
	{
        current_wpt=NULL;
		wpt_id=NULL;
        cursored=current_col=current_row=written=dup_ident=
		dup_ident_start=dup_index=0;
		return(0);
	}
	if(ACTION == ACTION_TIMER && !cursored)
	{
		if(!current_wpt)
		{
			wpt_list_index = 0;
			current_wpt = &nav_db.wpts[wpt_list_index];
     	}
		if(current_list == COMPLETE_LIST)
	       print_wpt(current_wpt,"INT  ");
		else if(current_list == NR_LIST)
		{
/*		   
		   int ind = find_in_nr_list(current_wpt);
		   if(ind>=0)
		   {
		      print_nr_wpt(current_wpt,ind,cursored,"NDB  ");
              in_nr_list_pos=ind;
		   }
		   else
		   {
			   print_wpt(current_wpt,"NDB  ");
			   current_list = COMPLETE_LIST;
		   }
*/
		}
		update_screen();
		return(1);
	}
	if(ACTION == ACTION_TIMER && cursored)
	{
       if(current_list == NR_LIST)
	   {
		   /*
		   if(nr_wpt_l.nr_wpt_count > in_nr_list_pos)
		   {
		      wpt_list_index = nr_wpt_l.list[in_nr_list_pos].index;
		      current_wpt = &nav_db.wpts[wpt_list_index];
		   }
		   else
		   {
		      current_wpt    = NULL;
		      wpt_list_index = -1;
		   }
		   print_nr_wpt(current_wpt,in_nr_list_pos,cursored,"NDB  ");
		   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_INVERSE);
		   update_screen();
		   */
	   }
	}
	//==========================================================================
	if(ACTION_TIMER && dup_ident_start)
	{
	    if(GetTickCount()-dup_ident_start >= 5000)
		{
            //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG    ");
			dup_ident_start = 0;
     	}
	}
	//==========================================================================
	if(ACTION == INPUT_RCURSOR )
	{
		if(1-cursored)
		{
		   INT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		   if( current_list == COMPLETE_LIST && !K_is_scanpull() )
		   {
		      current_col=icao_ed_col;
		      current_row=icao_ed_row;
		      print_wpt(current_wpt,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      //=========================================================================
		      wpt_id = find_wpts_by_icao(current_wpt->ICAO_ID,&written);
		      if(wpt_id)
			     qsort(wpt_id,written,sizeof(*wpt_id),wpt_dup_sort);
		      print_wpt(current_wpt,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      if(written>1)
		      {
				 do_status_line(ADD_STATUS_MESSAGE," DUP IDENT ");
                 dup_index = get_wpt_index(wpt_id,written,current_wpt);
			     wpt_list_index = wpt_id[dup_index];
			     dup_ident = 1;
	             dup_ident_start = GetTickCount();
			     if(K_is_scanpull())
                    K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_INVERSE);
              }

              update_screen();
			  cursored=1;
		   }
		   else if( current_list == NR_LIST )
			   cursored=1;
		}
		else 
		{
		   	if(K_is_scanpull())
			   INT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			else
               INT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
		   if( current_list == COMPLETE_LIST )
		   {
		      dup_ident_start=0;
			  wpt_id=NULL;
		      written=0;
		      dup_ident=0;
			  cursored=0;
		   }
		   else if( current_list == NR_LIST )
			   cursored=0;
		}
	}
	//==========================================================================
	if(ACTION == INPUT_ROUTERPLUS)
	{
		if(K_is_scanpull()) return(1);
		if(K_get_char(current_row,current_col) != BLANK_CHAR && current_col<icao_ed_col+4)
		{
            K_change_attribute(current_row,current_col,1,ATTRIBUTE_NORMAL);
			current_col++;
			K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		}
	}
	//==========================================================================
	if(ACTION == INPUT_ROUTERMINUS)
	{
		if(K_is_scanpull()) return(1);

		if(current_col > icao_ed_col)
		{
            K_change_attribute(current_row,current_col,1,ATTRIBUTE_NORMAL);
			current_col--;
			K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		}
	}
	//==========================================================================
	if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS)
	{
        if(K_is_scanpull() && dup_ident)
		{
			if(ACTION == INPUT_RINNERPLUS) dup_index = dup_index < (written-1) ? ++dup_index : 0;
			if(ACTION == INPUT_RINNERMINUS) dup_index = dup_index > 0 ? --dup_index : (written-1);
			wpt_list_index = wpt_id[dup_index];
		    current_wpt = &nav_db.wpts[wpt_list_index];
			print_wpt(current_wpt,"CRSR ");
			K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_INVERSE);
		}
		else if(K_is_scanpull() && !cursored)
		{
			if(ACTION == INPUT_RINNERPLUS) 
			{
				if( 
					wpt_list_index < (nav_db.wpts_count-1)
					&& 
					(current_list==COMPLETE_LIST)
				  )
				{
				   current_wpt=&nav_db.wpts[++wpt_list_index];
				}
				else if(current_list==NR_LIST)
				{
				    /*
					if(in_nr_list_pos < 8)
					{
				        in_nr_list_pos++;
						if(in_nr_list_pos < nr_wpt_l.nr_wpt_count)
						{
						   wpt_list_index=nr_wpt_l.list[in_nr_list_pos].index;
						   current_wpt=&nav_db.wpts[wpt_list_index];
						}
						else
						{
						   wpt_list_index=0;
						   current_wpt=&nav_db.wpts[wpt_list_index];
						   current_list=COMPLETE_LIST;
						}
					}
					*/
				}
			}
			//======================================================================================================
			if(ACTION == INPUT_RINNERMINUS)
			{
			    if(!wpt_list_index && (current_list==COMPLETE_LIST))
				{
	                /*if(nr_wpt_l.nr_wpt_count>0)
					{
					    current_list=NR_LIST;
						in_nr_list_pos=0;
						wpt_list_index=nr_wpt_l.list[0].index;
						current_wpt=&nav_db.wpts[wpt_list_index];
					}
					*/
				}
				else if(current_list==NR_LIST)
				{
				    /*
					if(in_nr_list_pos>0)
					{
				        in_nr_list_pos--;
						if(in_nr_list_pos < nr_wpt_l.nr_wpt_count)
						{
						   wpt_list_index=nr_wpt_l.list[in_nr_list_pos].index;
						   current_wpt=&nav_db.wpts[wpt_list_index];
						}
						else
						   current_list=COMPLETE_LIST;
					}
					*/
				}
				else
				{
				    wpt_list_index = wpt_list_index > 0? --wpt_list_index : 0;
					current_wpt = &nav_db.wpts[wpt_list_index];
				}
			}
        }
		else if(K_is_scanpull() && cursored)
		{
		    if(ACTION == INPUT_RINNERMINUS && current_list==NR_LIST)
			{
				//in_nr_list_pos = in_nr_list_pos ? --in_nr_list_pos : 0;
			}
		    if(ACTION == INPUT_RINNERPLUS && current_list==NR_LIST)
			{
				//in_nr_list_pos = in_nr_list_pos<8 ? ++in_nr_list_pos : 8;
			}
		}
		else if(!K_is_scanpull() && cursored)
		{
		   char curr_char = K_get_char(current_row,current_col);
		   curr_char = ACTION == INPUT_RINNERPLUS ? K_next_char(curr_char) : K_prev_char(curr_char);
		   K_set_char(current_row,current_col,curr_char);

		   char curr_id[6];
		   K_get_string(icao_ed_row,icao_ed_col,current_col,curr_id);
		   wpt_id = find_wpts_by_icao(curr_id,&written);
		   if(wpt_id)
		   {
		      qsort(wpt_id,written,sizeof(*wpt_id),wpt_dup_sort);
			  wpt_list_index = wpt_id[0];
			  current_wpt = &nav_db.wpts[wpt_list_index];
			  print_wpt(current_wpt,"CRSR ");
			  K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			  if(written>1)
			  {
                 //print_str(6,6,ATTRIBUTE_INVERSE," DUP IDENT ");
				 do_status_line(ADD_STATUS_MESSAGE," DUP IDENT ");
				 dup_index = 0;
				 dup_ident = 1;
	             dup_ident_start = GetTickCount();
			  }
			  else if(written>0)
			  {
				 dup_ident_start=0;
				 dup_ident = 0;
				 //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG    ");
			  }
			  else
			  {
                 //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
				 dup_ident_start=0;
				 dup_ident = 0;
				 //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG ");
			  }
		      update_screen();
		    }
		    else
		    {
			   print_wpt(NULL,"CRSR ");
			   print_str(0,12,ATTRIBUTE_NORMAL," %-10s",curr_id);
			   K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		       K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			   //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			   dup_ident_start=0;
			   //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG ");
		       update_screen();
		    }
		}
	}
	//==========================================================================
	if(ACTION == INPUT_PULLSCAN)
	{
	    if(current_list == COMPLETE_LIST)
		{
		   if(dup_ident && K_is_scanpull())
		   {
		      K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_INVERSE);
			  //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			  dup_ident_start=0;
			  //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG ");
			  update_screen();
		   }
		   if(dup_ident && !K_is_scanpull())
		   {
		      K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_NORMAL);
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      dup_ident_start=0;
		      //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG    ");
		      update_screen();
		   }
		   if(!dup_ident && K_is_scanpull() && cursored)
			   //main_loop(INPUT_RCURSOR);
			   do_wpt_page(INPUT_RCURSOR);
		   if(!cursored && K_is_scanpull())
               INT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		   if(!cursored && !K_is_scanpull())
			   INT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
		}
        if(!K_is_scanpull() && current_list == NR_LIST)
		{
		   current_list = COMPLETE_LIST;
		   if(cursored)
		      //main_loop(INPUT_RCURSOR);
			  do_wpt_page(INPUT_RCURSOR);
		}
	}
	//==========================================================================
	return(1);
}
nvdb_wpt *get_current_wpt(void)
{
   return(current_wpt);
}