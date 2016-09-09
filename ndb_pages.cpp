#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

extern nav_db_t nav_db;


nvdb_ndb *current_ndb = NULL;

enum {COMPLETE_LIST,NR_LIST};
DWORD current_list = COMPLETE_LIST;
DWORD  ndb_list_index=0;
DWORD  in_nr_list_pos;

extern int count_of_elem;
extern char str_elems[99][12];
extern int MULTI_LINE_CALLER;
extern int current_item;

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;

extern nr_ndb_list nr_ndb_l;

//=========================================================================================================
static int NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;

BOOL ndb_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&NDB_INPUT_MASK);
}

//=========================================================================================================
void print_nr_ndb(nvdb_ndb *__ndb,int nr_index,BOOL cursored,char *status)
{
  	if(!__ndb)
	{
	   print_str(0,12,ATTRIBUTE_NORMAL," %-3s   NR %d"," ",nr_index+1);
	   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH);
	   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
	   print_str(2,12,ATTRIBUTE_NORMAL,"%-11s"," ");
       print_str(3,12,ATTRIBUTE_NORMAL,"FREQ ----.-");
       print_str(4,12,ATTRIBUTE_NORMAL,"%-5s---%cFR"," ",0xB0);                
	   print_str(5,12,ATTRIBUTE_NORMAL,"%-4s---.-NM"," ");
	}
	else
	{
	   print_str(0,12,ATTRIBUTE_NORMAL," %-3s   NR %d",__ndb->ICAO_ID,nr_index+1);
	   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH);
	   //**********************************************************************************
	   char __name[12];
	   strncpy(__name,__ndb->NAME,11);
	   __name[11]='\0';
	   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s",__name);
	   print_str(2,12,ATTRIBUTE_NORMAL,"%-11s",strlen(__ndb->NAME)>11?__ndb->NAME+11:" ");
	   //**********************************************************************************
       print_str(3,12,ATTRIBUTE_NORMAL,"FREQ   %4d",__ndb->FREQ/1000);
	   print_str(4,12,ATTRIBUTE_NORMAL,"%-5s%03d%c%s"," ",(DWORD)nr_ndb_l.list[nr_index].radial,0xB0,
		                          fabs(__add_deg180(get_TK2(),-nr_ndb_l.list[nr_index].radial))<=90.0f?"TO":"FR");
	   print_str(5,12,ATTRIBUTE_NORMAL,"%-4s%03d.%dNM"," ",(DWORD)(nr_ndb_l.list[nr_index].S/1.852f),
		                            ((DWORD)(nr_ndb_l.list[nr_index].S/1.852f*10.0f))%10);
	   print_navaid_arrow(__ndb);
	}
    print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
}
//=========================================================================================================
int ndb_compare(nvdb_ndb *ndb1,nvdb_ndb *ndb2)
{
		if(    
		    (ndb1->FREQ==ndb2->FREQ)
			&&
			(ndb1->Lat==ndb2->Lat)
		    &&
			(ndb1->Lon==ndb2->Lon)
		  )
		  return(0);

	return(1);
}
int find_in_nr_list(nvdb_ndb *c_ndb)
{
    
	for(int __i=0;__i<nr_ndb_l.nr_ndb_count && __i<9;__i++)
	{
		 if(!ndb_compare(&nav_db.ndbs[nr_ndb_l.list[__i].index],c_ndb))  
			 return(__i);
	}
	return(-1);
}

long get_ndb_index(long *ndb_id,int elements_count,nvdb_ndb *__ndb)
{
	for(int i=0;i<elements_count;i++)
	{
		if(!ndb_compare(&nav_db.ndbs[ndb_id[i]],__ndb))
		   return(i);
	}
	return(-1);
}

int ndb_dup_sort(const void *a1,const void *a2)
{
    FLOAT64 Lon,Lat;
    get_PPOS(&Lat,&Lon);
	return(
	       get_S(Lat,Lon,nav_db.ndbs[*(long *)a1].Lat,nav_db.ndbs[*(long *)a1].Lon)
	       -
	       get_S(Lat,Lon,nav_db.ndbs[*(long *)a2].Lat,nav_db.ndbs[*(long *)a2].Lon)
	      );
}

long *find_nbds_by_icao(char *icao_id_s,long *array_size)
{
    static DWORD ndb_indexes[1024];
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
		  if(is_ndb_id(ind,&local_index))
		  {
              ndb_indexes[i++] = local_index;
		  }
		}
	}
	if(i>0)
	{
	   if(array_size) *array_size=i;
	   return((long *)&ndb_indexes);
	}
	
	char reg_exp[15];
	sprintf(reg_exp,"^%s",icao_id_s);
	Regexp re(reg_exp);
    re.CompiledOK();

	for(int j=0;j<nav_db.ndbs_count;j++)
	{
		if(re.Match(nav_db.ndbs[j].ICAO_ID))
		{
           if(array_size) 
		      *array_size=1;
		   ndb_indexes[0]=j;
		   return((long *)&ndb_indexes);
		}
	}
	return(NULL);
}

void print_ndb(nvdb_ndb *__nbd,char *status)
{
		if(!__nbd)
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

  	    print_str(0,12,ATTRIBUTE_NORMAL," %-10s",__nbd->ICAO_ID);
		//************************************************************
		char __name[12];
		strncpy(__name,__nbd->NAME,11);
		__name[11]='\0';
		print_str(1,12,ATTRIBUTE_NORMAL,"%-11s",__name);
		print_str(2,12,ATTRIBUTE_NORMAL,"%-11s",strlen(__nbd->NAME)>11?__nbd->NAME+11:" ");
		//************************************************************
		DWORD freq_whole = __nbd->FREQ/1000;
		DWORD freq_fract = (__nbd->FREQ%1000)/100;
		char freq_str[7]={0};
		if(!freq_fract)
			sprintf(freq_str,"%6d",freq_whole);
		else 
            sprintf(freq_str,"%4d.%d",freq_whole,freq_fract);

		print_str(3,12,ATTRIBUTE_NORMAL,"FREQ %6s",freq_str);
        
		K_deg deg;
		K_GetDegMin(__nbd->Lat,&deg);
        print_str(4,12,ATTRIBUTE_NORMAL,"%c %.2d%c%.2d.%.2d'",__nbd->Lat<0?'S':'N',(int)fabs(__nbd->Lat),0xB0,deg.mins,deg.dec_mins);
		K_GetDegMin(__nbd->Lon,&deg);
        print_str(5,12,ATTRIBUTE_NORMAL,"%c%.3d%c%.2d.%.2d'",__nbd->Lon<0?'W':'E',(int)fabs(__nbd->Lon),0xB0,deg.mins,deg.dec_mins);
        print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
		print_navaid_arrow(__nbd);
}

int do_ndb_page(int ACTION)
{
    static int cursored = 0;
	static int current_col = 0;
	static int current_row = 0;
    static long written = 0;  
    static long *ndb_id = NULL;
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
		NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
		return(0);
	}
	if(ACTION == ACTION_SHOW)
	{
		if(!nav_db.ndbs_count)
			do_status_line(ADD_STATUS_MESSAGE,"NO NDB WPTS");
	}
    if(!nav_db.ndbs_count)
	{
	   current_ndb=NULL;
	   current_list=COMPLETE_LIST;
	   print_ndb(NULL,"NDB  ");
	   return(0);
	}

	if(ACTION == PAGE_FORCE_CHANGE)
	{
       if(cursored && current_list == COMPLETE_LIST)
       do_ndb_page(INPUT_RCURSOR);
	   return(0);
	}
	if(ACTION == ACTION_FREE_RESOURCES)
	{
        current_ndb=NULL;
		//K_free(ndb_id);
		ndb_id=NULL;
        cursored=current_col=current_row=written=dup_ident=
		dup_ident_start=dup_index=0;
		return(0);
	}
	if(ACTION == ACTION_TIMER && !cursored)
	{
		if(!current_ndb)
		{
			ndb_list_index = 0;
			current_ndb = &nav_db.ndbs[ndb_list_index];
     	}
		if(current_list == COMPLETE_LIST)
	       print_ndb(current_ndb,"NDB  ");
		else if(current_list == NR_LIST)
		{
		   int ind = find_in_nr_list(current_ndb);
		   if(ind>=0)
		   {
		      print_nr_ndb(current_ndb,ind,cursored,"NDB  ");
              in_nr_list_pos=ind;
		   }
		   else
		   {
			   print_ndb(current_ndb,"NDB  ");
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
		   if(nr_ndb_l.nr_ndb_count > in_nr_list_pos)
		   {
		      ndb_list_index = nr_ndb_l.list[in_nr_list_pos].index;
		      current_ndb = &nav_db.ndbs[ndb_list_index];
		   }
		   else
		   {
		      current_ndb    = NULL;
		      ndb_list_index = -1;
		   }
		   print_nr_ndb(current_ndb,in_nr_list_pos,cursored,"NDB  ");
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
		      NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			  current_col=icao_ed_col;
		      current_row=icao_ed_row;
		      print_ndb(current_ndb,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      //=========================================================================
		      ndb_id = find_nbds_by_icao(current_ndb->ICAO_ID,&written);
		      
			  if(ndb_id)
		         qsort(ndb_id,written,sizeof(*ndb_id),ndb_dup_sort);
		      print_ndb(current_ndb,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      if(written>1)
		      {
                 //print_str(6,6,ATTRIBUTE_INVERSE," DUP IDENT ");
				 do_status_line(ADD_STATUS_MESSAGE," DUP IDENT ");
                 dup_index = get_ndb_index(ndb_id,written,current_ndb);
			     ndb_list_index = ndb_id[dup_index];
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
			   NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			   cursored=1;
		   }
		}
		else 
		{
		   
		   if( current_list == COMPLETE_LIST )
		   {
		      if(K_is_scanpull())
				 NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			  else
			     NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
			  //current_ndb = temp_ndb;
		      //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG    ");
		      dup_ident_start=0;
		      //K_free(ndb_id);
			  ndb_id=NULL;
		      written=0;
		      dup_ident=0;
			  cursored=0;
		   }
		   else if( current_list == NR_LIST )
		   {
			   NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			   cursored=0;
		   }
		}
	}
	//==========================================================================
	if(ACTION == INPUT_ROUTERPLUS)
	{
		if(K_is_scanpull()) return(1);
		if(K_get_char(current_row,current_col) != BLANK_CHAR && current_col<icao_ed_col+2)
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
			ndb_list_index = ndb_id[dup_index];
		    current_ndb = &nav_db.ndbs[ndb_list_index];
			print_ndb(current_ndb,"CRSR ");
			K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			K_change_attribute(icao_ed_row,icao_ed_col,3,ATTRIBUTE_INVERSE);
		}
		else if(K_is_scanpull() && !cursored)
		{
			if(ACTION == INPUT_RINNERPLUS) 
			{
				if( 
					ndb_list_index < (nav_db.ndbs_count-1)
					&& 
					(current_list==COMPLETE_LIST)
				  )
				{
				   current_ndb=&nav_db.ndbs[++ndb_list_index];
				}
				else if(current_list==NR_LIST)
				{
				    if(in_nr_list_pos < 9)
					{
				        in_nr_list_pos++;
						if(in_nr_list_pos < nr_ndb_l.nr_ndb_count)
						{
						   ndb_list_index=nr_ndb_l.list[in_nr_list_pos].index;
						   current_ndb=&nav_db.ndbs[ndb_list_index];
						}
						else
						{
						   ndb_list_index=0;
						   current_ndb=&nav_db.ndbs[ndb_list_index];
						   current_list=COMPLETE_LIST;
						}
					}
				}
			}
			//======================================================================================================
			if(ACTION == INPUT_RINNERMINUS)
			{
			    if(!ndb_list_index && (current_list==COMPLETE_LIST))
				{
	                if(nr_ndb_l.nr_ndb_count>0)
					{
					    current_list=NR_LIST;
						in_nr_list_pos=0;
						ndb_list_index=nr_ndb_l.list[0].index;
						current_ndb=&nav_db.ndbs[ndb_list_index];
					}
				}
				else if(current_list==NR_LIST)
				{
				    if(in_nr_list_pos>0)
					{
				        in_nr_list_pos--;
						if(in_nr_list_pos < nr_ndb_l.nr_ndb_count)
						{
						   ndb_list_index=nr_ndb_l.list[in_nr_list_pos].index;
						   current_ndb=&nav_db.ndbs[ndb_list_index];
						}
						else
						   current_list=COMPLETE_LIST;
					}
				}
				else
				{
				    ndb_list_index = ndb_list_index > 0? --ndb_list_index : 0;
					current_ndb = &nav_db.ndbs[ndb_list_index];
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
		   ndb_id = find_nbds_by_icao(curr_id,&written);
		   if(ndb_id)
		   {
		      qsort(ndb_id,written,sizeof(*ndb_id),ndb_dup_sort);
			  ndb_list_index = ndb_id[0];
			  current_ndb = &nav_db.ndbs[ndb_list_index];
			  print_ndb(current_ndb,"CRSR ");
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
			   print_ndb(NULL,"CRSR ");
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
			   do_ndb_page(INPUT_RCURSOR);
		   if(!cursored && K_is_scanpull())
               NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		   if(!cursored && !K_is_scanpull())
			   NDB_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;

		}
        if(!K_is_scanpull() && current_list == NR_LIST)
		{
		   current_list = COMPLETE_LIST;
		   if(cursored)
		      //main_loop(INPUT_RCURSOR);
			  do_ndb_page(INPUT_RCURSOR);
		}
	}
	//==========================================================================
	return(1);
}

nvdb_ndb *get_current_ndb(void)
{
   return(current_ndb);
}