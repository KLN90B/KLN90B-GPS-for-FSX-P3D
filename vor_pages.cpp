#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

extern nav_db_t nav_db;


nvdb_vor *current_vor = NULL;

enum {COMPLETE_LIST,NR_LIST};
static DWORD current_list = COMPLETE_LIST;
static DWORD  vor_list_index=0;
static DWORD  in_nr_list_pos;

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;

extern nr_vor_list nr_vor_l;


char *vor_type[]={"U","T","L","H"};

static int VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;

BOOL vor_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&VOR_INPUT_MASK);
}


void print_nr_vor(nvdb_vor *__vor,int nr_index,BOOL cursored,char *status)
{
  	if(!__vor)
	{
	   print_str(0,12,ATTRIBUTE_NORMAL," %-3s   NR %d"," ",nr_index+1);
	   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH);
	   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
	   print_str(2,12,ATTRIBUTE_NORMAL,"%-11s"," ");
	   print_str(3,12,ATTRIBUTE_NORMAL,"---.--  0E%c",0xB0);  
       print_str(4,12,ATTRIBUTE_NORMAL,"%-5s---%cFR"," ",0xB0);                
	   print_str(5,12,ATTRIBUTE_NORMAL,"%-4s---.-NM"," ");
	}
	else
	{
	   print_str(0,12,ATTRIBUTE_NORMAL," %-3s %s NR %d",__vor->ICAO_ID,
			                                         __vor->DME == 1?"D":" ",
													 nr_index+1);
	   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH);
	   //**********************************************************************************
	   char __name[12];
	   strncpy(__name,__vor->NAME,11);
	   __name[11]='\0';
	   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s",__name);
	   print_str(2,12,ATTRIBUTE_NORMAL,"%-11s",strlen(__vor->NAME)>11?__vor->NAME+11:" ");
	   //**********************************************************************************
	   print_str(2,21,ATTRIBUTE_NORMAL,"%2s",vor_type[__vor->TYPE]);
	   print_str(3,12,ATTRIBUTE_NORMAL,"%3d.%2d%3d%c%c",(__vor->FREQ)/1000000,
			                            (__vor->FREQ/10000)%100,
										(DWORD)fabs(get_magdec(__vor->Lon,__vor->Lat)),
										0xB0,
										get_magdec(__vor->Lon,__vor->Lat)<0?'W':'E');  
	   print_str(4,12,ATTRIBUTE_NORMAL,"%-5s%03d%c%s"," ",(DWORD)nr_vor_l.list[nr_index].radial,0xB0,
		                          fabs(__add_deg180(get_TK2(),-nr_vor_l.list[nr_index].radial))<=90.0f?"TO":"FR");
	   print_str(5,12,ATTRIBUTE_NORMAL,"%-4s%03d.%dNM"," ",(DWORD)(nr_vor_l.list[nr_index].S/1.852f),
		                            ((DWORD)(nr_vor_l.list[nr_index].S/1.852f*10.0f))%10);
	   print_navaid_arrow(__vor);
	}
    print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
}
//=========================================================================================================
int vor_compare(nvdb_vor *vor1,nvdb_vor *vor2)
{
		if(    
		    (vor1->FREQ==vor2->FREQ)
			&&
			(vor1->Lat==vor2->Lat)
		    &&
			(vor1->Lon==vor2->Lon)
		  )
		  return(0);

	return(1);
}
int find_in_nr_list(nvdb_vor *c_vor)
{
    
	for(int __i=0;__i<nr_vor_l.nr_vor_count && __i<9;__i++)
	{
		 if(!vor_compare(&nav_db.vors[nr_vor_l.list[__i].index],c_vor))  
			 return(__i);
	}
	return(-1);
}

long get_vor_index(long *vor_id,int elements_count,nvdb_vor *__vor)
{
	for(int i=0;i<elements_count;i++)
	{
		if(!vor_compare(&nav_db.vors[vor_id[i]],__vor))
		   return(i);
	}
	return(-1);
}

int vor_dup_sort(const void *a1,const void *a2)
{
    FLOAT64 Lon,Lat;
    get_PPOS(&Lat,&Lon);
	return(
	       get_S(Lat,Lon,nav_db.vors[*(long *)a1].Lat,nav_db.vors[*(long *)a1].Lon)
	       -
	       get_S(Lat,Lon,nav_db.vors[*(long *)a2].Lat,nav_db.vors[*(long *)a2].Lon)
	      );
}

long *find_vors_by_icao(char *icao_id_s,long *array_size)
{
    static DWORD vor_indexes[1024];
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
		  if(is_vor_id(ind,&local_index))
		  {
              vor_indexes[i++] = local_index;
		  }
		}
	}
	if(i>0)
	{
	   if(array_size) *array_size=i;
	   return((long *)&vor_indexes);
	}
	
	char reg_exp[15];
	sprintf(reg_exp,"^%s",icao_id_s);
	Regexp re(reg_exp);
    re.CompiledOK();

	for(int j=0;j<nav_db.vors_count;j++)
	{
		if(re.Match(nav_db.vors[j].ICAO_ID))
		{
           if(array_size) 
		      *array_size=1;
		   vor_indexes[0]=j;
		   return((long *)&vor_indexes);
		}
	}
	return(NULL);
}


void print_vor(nvdb_vor *__vor,char *status)
{
	    if(!__vor)
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
	    double Lon = __vor->Lon;
		double Lat = __vor->Lat;

		print_str(0,12,ATTRIBUTE_NORMAL," %-3s %-6s",__vor->ICAO_ID,
			                                         __vor->DME == 1?"D":" ");
		//====================================================================
		char __name[12];
		strncpy(__name,__vor->NAME,11);
		__name[11]='\0';
		print_str(1,12,ATTRIBUTE_NORMAL,"%-11s",__name);
		print_str(2,12,ATTRIBUTE_NORMAL,"%-11s",strlen(__vor->NAME)>11?__vor->NAME+11:" ");
		//====================================================================
		print_str(2,21,ATTRIBUTE_NORMAL,"%2s",vor_type[__vor->TYPE]);

		print_str(3,12,ATTRIBUTE_NORMAL,"%3d.%02d%3d%c%c",(__vor->FREQ)/1000000,
			                            (__vor->FREQ/10000)%100,
										(long)(floor(fabs((double)__vor->mag_var))),
										0xB0,
										__vor->mag_var<0?'W':'E');  
		K_deg deg;
		K_GetDegMin(Lat,&deg);
        print_str(4,12,ATTRIBUTE_NORMAL,"%c %.2d%c%.2d.%.2d'",Lat<0?'S':'N',(int)fabs(Lat),0xB0,deg.mins,deg.dec_mins);
		K_GetDegMin(Lon,&deg);
        print_str(5,12,ATTRIBUTE_NORMAL,"%c%.3d%c%.2d.%.2d'",Lon<0?'W':'E',(int)fabs(Lon),0xB0,deg.mins,deg.dec_mins);
        print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
		print_navaid_arrow(__vor);
		update_screen();
  
}

//=========================================================================================================

int do_vor_page(int ACTION)
{
    static int cursored = 0;
	static int current_col = 0;
	static int current_row = 0;
    static long written = 0;  
    static long *vor_id = NULL;
	static int dup_ident = 0;
	static DWORD dup_ident_start = 0;
	static int dup_index = 0;

	int icao_ed_col = 13;
	int icao_ed_row = 0;
	int name_ed_col = 12;
	int name_ed_row = 1;

	if(ACTION == ACTION_INIT)
	{
		current_list=COMPLETE_LIST;
		cursored=0;
		VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
		return(0);
	}
	if(ACTION == ACTION_SHOW)
	{
		if(!nav_db.vors_count)
			do_status_line(ADD_STATUS_MESSAGE,"NO VOR WPTS");
	}
	if(!nav_db.vors_count)
	{
		current_vor=NULL;
		current_list=COMPLETE_LIST;
		print_vor(NULL,"VOR  ");
		return(0);
	}
	
	if(ACTION == PAGE_FORCE_CHANGE)
	{
	   if(cursored && current_list == COMPLETE_LIST)
       do_vor_page(INPUT_RCURSOR);
	   return(0);
	}

	if(ACTION == ACTION_FREE_RESOURCES)
	{
		current_vor=NULL;
		vor_id=NULL;
        cursored=current_col=current_row=written=dup_ident=
		dup_ident_start=dup_index=0;
		return(0);
	}
	if(ACTION == ACTION_TIMER && !cursored)
	{
		if(!current_vor)
		{
			vor_list_index = 0;
			current_vor = &nav_db.vors[vor_list_index];
     	}
		if(current_list == COMPLETE_LIST)
	       print_vor(current_vor,"VOR  ");
		else if(current_list == NR_LIST)
		{
		   int ind = find_in_nr_list(current_vor);
		   if(ind>=0)
		   {
		      print_nr_vor(current_vor,ind,cursored,"VOR  ");
              in_nr_list_pos=ind;
		   }
		   else
		   {
			   print_vor(current_vor,"VOR  ");
			   current_list = COMPLETE_LIST;
		   }
		}
		update_screen();
		return(1);
	}
	if(ACTION == ACTION_TIMER && cursored)
	{
	   if(current_list == NR_LIST)
	   {
		   if(nr_vor_l.nr_vor_count > in_nr_list_pos)
		   {
		      vor_list_index = nr_vor_l.list[in_nr_list_pos].index;
		      current_vor = &nav_db.vors[vor_list_index];
		   }
		   else
		   {
		      current_vor    = NULL;
		      vor_list_index = -1;
		   }
		   print_nr_vor(current_vor,in_nr_list_pos,cursored,"VOR  ");
		   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_INVERSE);
		   update_screen();
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
		   if( current_list == COMPLETE_LIST && !K_is_scanpull() )
		   {
              VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			  current_col=icao_ed_col;
		      current_row=icao_ed_row;
		      print_vor(current_vor,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      //=========================================================================
		      vor_id = find_vors_by_icao(current_vor->ICAO_ID,&written);
		      if(vor_id)
			     qsort(vor_id,written,sizeof(*vor_id),vor_dup_sort);
		      print_vor(current_vor,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      if(written>1)
		      {
                 do_status_line(ADD_STATUS_MESSAGE," DUP IDENT ");
                 dup_index = get_vor_index(vor_id,written,current_vor);
			     vor_list_index = vor_id[dup_index];
			     dup_ident = 1;
	             dup_ident_start = GetTickCount();
			     if(K_is_scanpull())
                    K_change_attribute(icao_ed_row,icao_ed_col,3,ATTRIBUTE_INVERSE);
              }
		      //else 
              //   print_str(6,14,ATTRIBUTE_NORMAL,"   ");

              update_screen();
			  cursored=1;
		   }
		   else if( current_list == NR_LIST )
		   {
			   VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			   cursored=1;
		   }
		}
		else 
		{
		   if( current_list == COMPLETE_LIST )
		   {
		      if(K_is_scanpull())
			     VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			  else
			     VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
		      dup_ident_start=0;
			  vor_id=NULL;
		      written=0;
		      dup_ident=0;
			  cursored=0;
		   }
		   else if( current_list == NR_LIST )
		   {
			   VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			   cursored=0;
		   }
		}
	}
	//=========================================================================
	if(ACTION == INPUT_ROUTERPLUS && current_col<icao_ed_col+2)
	{
		if(K_is_scanpull()) return(1);
		if(K_get_char(current_row,current_col) != BLANK_CHAR)
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
			vor_list_index = vor_id[dup_index];
		    current_vor = &nav_db.vors[vor_list_index];
			print_vor(current_vor,"CRSR ");
			K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			K_change_attribute(icao_ed_row,icao_ed_col,3,ATTRIBUTE_INVERSE);
		}
		else if(K_is_scanpull() && !cursored)
		{
			if(ACTION == INPUT_RINNERPLUS) 
			{
				if( 
					vor_list_index < (nav_db.vors_count-1)
					&& 
					(current_list==COMPLETE_LIST)
				  )
				{
				   current_vor=&nav_db.vors[++vor_list_index];
				}
				else if(current_list==NR_LIST)
				{
				    if(in_nr_list_pos < 9)
					{
				        in_nr_list_pos++;
						if(in_nr_list_pos < nr_vor_l.nr_vor_count)
						{
						   vor_list_index=nr_vor_l.list[in_nr_list_pos].index;
						   current_vor=&nav_db.vors[vor_list_index];
						}
						else
						{
						   vor_list_index=0;
						   current_vor=&nav_db.vors[vor_list_index];
						   current_list=COMPLETE_LIST;
						}
					}
				}
			}
			//======================================================================================================
			if(ACTION == INPUT_RINNERMINUS)
			{
			    if(!vor_list_index && (current_list==COMPLETE_LIST))
				{
	                if(nr_vor_l.nr_vor_count>0)
					{
					    current_list=NR_LIST;
						in_nr_list_pos=0;
						vor_list_index=nr_vor_l.list[0].index;
						current_vor=&nav_db.vors[vor_list_index];
					}
				}
				else if(current_list==NR_LIST)
				{
				    if(in_nr_list_pos>0)
					{
				        in_nr_list_pos--;
						if(in_nr_list_pos < nr_vor_l.nr_vor_count)
						{
						   vor_list_index=nr_vor_l.list[in_nr_list_pos].index;
						   current_vor=&nav_db.vors[vor_list_index];
						}
						else
						   current_list=COMPLETE_LIST;
					}
				}
				else
				{
				    vor_list_index = vor_list_index > 0? --vor_list_index : 0;
					current_vor = &nav_db.vors[vor_list_index];
				}
			}
        }
		else if(K_is_scanpull() && cursored)
		{
		    if(ACTION == INPUT_RINNERMINUS && current_list==NR_LIST)
			{
				in_nr_list_pos = in_nr_list_pos ? --in_nr_list_pos : 0;
			}
		    if(ACTION == INPUT_RINNERPLUS && current_list==NR_LIST)
			{
				in_nr_list_pos = in_nr_list_pos<8 ? ++in_nr_list_pos : 8;
			}
		}
		else if(!K_is_scanpull() && cursored)
		{
		   char curr_char = K_get_char(current_row,current_col);
		   curr_char = ACTION == INPUT_RINNERPLUS ? K_next_char(curr_char) : K_prev_char(curr_char);
		   K_set_char(current_row,current_col,curr_char);

		   char curr_id[6];
		   K_get_string(icao_ed_row,icao_ed_col,current_col,curr_id);
		   vor_id = find_vors_by_icao(curr_id,&written);
		   if(vor_id)
		   {
		      qsort(vor_id,written,sizeof(*vor_id),vor_dup_sort);
			  vor_list_index = vor_id[0];
			  current_vor = &nav_db.vors[vor_list_index];
			  print_vor(current_vor,"CRSR ");
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
			   print_vor(NULL,"CRSR ");
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
		      K_change_attribute(icao_ed_row,icao_ed_col,3,ATTRIBUTE_INVERSE);
			  //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			  dup_ident_start=0;
			  //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG ");
			  update_screen();
		   }
		   if(dup_ident && !K_is_scanpull())
		   {
		      K_change_attribute(icao_ed_row,icao_ed_col,3,ATTRIBUTE_NORMAL);
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      dup_ident_start=0;
		      //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG    ");
		      update_screen();
		   }
		   if(!dup_ident && K_is_scanpull() && cursored)
			   //main_loop(INPUT_RCURSOR);
			   do_vor_page(INPUT_RCURSOR);
		   if(!cursored && K_is_scanpull())
               VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		   if(!cursored && !K_is_scanpull())
			   VOR_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;

		}
        if(!K_is_scanpull() && current_list == NR_LIST)
		{
		   current_list = COMPLETE_LIST;
		   if(cursored)
		      //main_loop(INPUT_RCURSOR);
			  do_vor_page(INPUT_RCURSOR);
		}
	}
	//==========================================================================
 	return(1);
}
nvdb_vor *get_current_vor(void)
{
   return(current_vor);
}