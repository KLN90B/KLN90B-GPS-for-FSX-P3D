#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include "sid_star.h"
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

// APT7_PAGES.CPP
void print_apt7(nvdb_apt *__apt, char *status);
int do_apt7(nvdb_apt *current_apt, int &current_row, int ACTION, int &input_mask);
bool apt7_next_page(void);
bool apt7_prev_page(void);
void apt7_reload_ss(nvdb_apt *__apt);
// ---

nvdb_apt *current_apt = NULL;

int once = 0;
DWORD sidstars = 0;

enum {COMPLETE_LIST,NR_LIST};
static DWORD current_list = COMPLETE_LIST;
static DWORD  apt_list_index=0;
static DWORD  in_nr_list_pos;

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;
extern int force_nr_mode;

extern nr_apt_list nr_apt_l;
extern fpl_t fpls[];
//=========================================================================================================
static int APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERMINUS|INPUT_RINNERPLUS;

BOOL apt_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&APT_INPUT_MASK);
}
//==================================================================================
void print_nr_apt(nvdb_apt *__apt,int nr_index,BOOL cursored,char *status)
{
  	if(!__apt)
	{
	   print_str(0,12,ATTRIBUTE_NORMAL," %-4s  NR %d"," ",nr_index+1);
	   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH);
	   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
	   print_str(2,12,ATTRIBUTE_NORMAL,"%-11s"," ");
       print_str(3,12,ATTRIBUTE_NORMAL,"%-11s"," ");
       print_str(4,12,ATTRIBUTE_NORMAL,"%-5s---%cFR"," ",0xB0);                
	   print_str(5,12,ATTRIBUTE_NORMAL,"%-4s---.-NM"," ");
	}
	else
	{
	   print_str(0,12,ATTRIBUTE_NORMAL," %-4s  NR %d",__apt->ICAO_ID,nr_index+1);
	   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_FLASH);
	   //**********************************************************************************
	   char __name[12];
	   strncpy(__name,__apt->NAME,11);
	   __name[11]='\0';
	   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s",__name);
	   print_str(2,12,ATTRIBUTE_NORMAL,"%-11s",strlen(__apt->NAME)>11?__apt->NAME+11:" ");
	   //**********************************************************************************
	   print_str(3,12,ATTRIBUTE_NORMAL,"%6d' HRD",(LONG)(__apt->rws[0].len*3.281f));
	   print_str(4,12,ATTRIBUTE_NORMAL,"%-5s%03d%c%s"," ",(DWORD)nr_apt_l.list[nr_index].radial,0xB0,
		                          fabs(__add_deg180(get_HDG(),-nr_apt_l.list[nr_index].radial))<=90.0f?"TO":"FR");
	   print_str(5,12,ATTRIBUTE_NORMAL,"%-4s%03d.%dNM"," ",(DWORD)(nr_apt_l.list[nr_index].S/1.852f),
		                            ((DWORD)(nr_apt_l.list[nr_index].S/1.852f*10.0f))%10);
	   print_navaid_arrow(__apt);
	}
    print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
}
//=========================================================================================================
int apt_compare(nvdb_apt *apt1,nvdb_apt *apt2)
{
   if( !strcmp(apt1->ICAO_ID,apt2->ICAO_ID) )
   {
      if(apt1->freqs_count == apt2->freqs_count && apt1->rnws_count == apt2->rnws_count)
	     return(0);
	  if(apt1->rnws_count > apt2->rnws_count)
	     return(2);
	  else if(apt1->rnws_count < apt2->rnws_count)
	     return(3);
	  else if(apt1->freqs_count > apt2->freqs_count)
	     return(4);
	  else if(apt1->freqs_count < apt2->freqs_count)
	     return(5);
	}
	return(1);
}
int find_in_nr_list(nvdb_apt *c_apt)
{
    
	for(int __i=0;__i<nr_apt_l.nr_apt_count && __i<9;__i++)
	{
		 if(!apt_compare(&nav_db.apts[nr_apt_l.list[__i].index],c_apt))  
			 return(__i);
	}
	return(-1);
}

long get_apt_index(long *apt_id,int elements_count,nvdb_apt *__apt)
{
	for(int i=0;i<elements_count;i++)
	{
		if(!apt_compare(&nav_db.apts[apt_id[i]],__apt))
		   return(i);
	}
	return(-1);
}

int apt_dup_sort(const void *a1,const void *a2)
{
    FLOAT64 Lon,Lat;
    get_PPOS(&Lat,&Lon);
	return(
	       get_S(Lat,Lon,nav_db.apts[*(long *)a1].Lat,nav_db.apts[*(long *)a1].Lon)
	       -
	       get_S(Lat,Lon,nav_db.apts[*(long *)a2].Lat,nav_db.apts[*(long *)a2].Lon)
	      );
}

// ---------------------------------------------------------------------------------------
// Name:		find_apts_by_icao
// Description:	Finds all airport records matching the ICAO ID and returns their index
//				in a static array
// Parameters:	
//		icao_id_s	ICAO ID string of the airport
//		array_size	Returned number of matching airports
// Returns:		Pointer to the static array containing the airport indexes
// ---------------------------------------------------------------------------------------

long *find_apts_by_icao(char *icao_id_s,long *array_size)
{
    static DWORD apt_indexes[1024];
	int i, j;
	int ind, int_it;
	DWORD icao_id = icao2id(icao_id_s);
	i=0;
	if (strcmp(icao_id_s, "LHBP") == 0)
		j = 0;
	if(icao_index.find(icao_id) != icao_index.end())
	{
		i = 0;
		for (ind = 0; ind != icao_index[icao_id].size(); ind++) 
		{
			int_it = icao_index[icao_id][ind];
			long local_index;
			if(is_apt_id(int_it,&local_index))
			{
				apt_indexes[i++] = local_index;
			}
		}
	}

	if( i > 0 )
	{
	   if(array_size) 
		   *array_size=i;
	   return((long *)&apt_indexes);
	}
	
	// Return first incomplete match (based on first defined characters) 
	char reg_exp[15];
	sprintf(reg_exp,"^%s",icao_id_s);
	Regexp re(reg_exp);
    re.CompiledOK();

	for(j = i = 0; j < nav_db.apts_count; j++)
	{
		if (re.Match(nav_db.apts[j].ICAO_ID))
		{
			apt_indexes[i++] = j;
			if (array_size)
		      *array_size = i;
			break;					// we are not looking for all matches, only the 1st one.
		}
	}

	return(i > 0 ? (long *)&apt_indexes : NULL);
}

static int apt_page_num;
static int apt_min_page = 1;
static int apt_max_page = 7;
static int apt_sub_page;
static int apt_last_sub_page;


void apt_next_page(void)
{  
	if(apt_page_num == 3 && current_apt->rnws_count && apt_sub_page<apt_last_sub_page)
	{
		apt_sub_page++;
		return;
	}
	if(apt_page_num == 4 && current_apt->freqs_count && apt_sub_page<apt_last_sub_page)
	{
		apt_sub_page++;
		return;
	}
	if (apt_page_num == 5 && current_apt->rnws_count && apt_sub_page<apt_last_sub_page)
	{
		apt_sub_page++;
		return;
	}
	if (apt_page_num == 7)
	{
		if (apt7_next_page())
		return;
	}
    apt_page_num++; 
	if(apt_page_num>apt_max_page) apt_page_num=apt_min_page;

	if(apt_page_num == 3)
	{
		apt_last_sub_page = current_apt->rnws_count % 2 ? current_apt->rnws_count/2 + 2 :  current_apt->rnws_count/2 + 1;
		if(current_apt->rnws_count)
			apt_sub_page=1;
		else
			apt_sub_page=0;
	}
	if(apt_page_num == 4)
	{
		apt_last_sub_page = current_apt->freqs_count % 5 ? current_apt->freqs_count/5 + 1 :  current_apt->freqs_count/5;
		if(current_apt->freqs_count)
			apt_sub_page=1;
		else
			apt_sub_page=0;
	}
	if (apt_page_num == 5)
	{
		apt_last_sub_page = current_apt->rnws_count % 2 ? current_apt->rnws_count/2 + 1 :  current_apt->rnws_count/2;
		if(current_apt->rnws_count)
			apt_sub_page=1;
		else
			apt_sub_page=0;
	}
}

void check_multi_page(void)
{
   if(!current_apt) 
   {
      apt_page_num=1;
	  return;
   }
   if(apt_page_num==3)
   {
      if(current_apt->rnws_count>0)
	  {
	     apt_last_sub_page = current_apt->rnws_count % 2 ? current_apt->rnws_count/2 + 2 :  current_apt->rnws_count/2 + 1;
		 apt_sub_page=2;
	  }
	  else
		  apt_sub_page=0;
   }
   if(apt_page_num==4)
   {
       if(current_apt->freqs_count>0)
	   {
	      apt_sub_page=1;
	      apt_last_sub_page = apt_last_sub_page = current_apt->freqs_count % 5 ? current_apt->freqs_count/5 + 1 :  current_apt->freqs_count/5;
	   }
	   else
	      apt_sub_page=0;
   }
   if (apt_page_num == 5)
   {
	   if (current_apt->rnws_count>0)
	   {
		   apt_last_sub_page = current_apt->rnws_count % 2 ? current_apt->rnws_count / 2 + 1 : current_apt->rnws_count / 2;
		   apt_sub_page = 1;
	   }
	   else
		   apt_sub_page = 0;
   }

}
void apt_prev_page(void)
{
	if((apt_page_num==3||apt_page_num==4||apt_page_num==5) && apt_sub_page>1)
	{
		apt_sub_page--;   
		return;
	}
	if (apt_page_num == 7) 
	{
		if (apt7_prev_page())
			return;
	}
    apt_page_num--;

	if(apt_page_num<apt_min_page) apt_page_num=apt_max_page;

	if(apt_page_num == 3)
	{
		if(current_apt->rnws_count)
		{
			apt_last_sub_page = current_apt->rnws_count % 2 ? current_apt->rnws_count/2 + 2 :  current_apt->rnws_count/2 + 1;
			apt_sub_page = apt_last_sub_page;
		}
		else
			apt_sub_page=0;
	}
	if(apt_page_num == 4)
	{
		if(current_apt->freqs_count)
		{
			apt_last_sub_page = apt_last_sub_page = current_apt->freqs_count % 5 ? current_apt->freqs_count/5 + 1 :  current_apt->freqs_count/5;
			apt_sub_page = apt_last_sub_page;
		}
		else
			apt_sub_page=0;
	}
	if (apt_page_num == 5)
	{
		if (current_apt->rnws_count)
		{
			apt_last_sub_page = current_apt->rnws_count % 2 ? current_apt->rnws_count / 2 + 1 : current_apt->rnws_count / 2;
			apt_sub_page = apt_last_sub_page;
		}
		else
			apt_sub_page = 0;
	}
}

char *comm2text(int type)
{
   switch(type)
   {
   case 0x0001:
	   return("ATIS");
   case 0x0002:
	   return("MCOM");
   case 0x0003:
	   return("UNIC");
   case 0x0004:
	   return("CTAF");
   case 0x0005:
	   return("GRND");
   case 0x0006:
	   return("TWR ");
   case 0x0007:
	   return("CLR ");
   case 0x0008:
	   return("APR ");
   case 0x0009:
	   return("DEP ");
   case 0x000A:
	   return("CTR ");
   case 0x000B:
	   return("FSS "); // ?????
   case 0x000C:
	   return("ASOS");
   case 0x000D:
	   return("AWOS");
   }
   return("");
}

// ------------------------------------------------------------------------------
// Name:		print_ap
// Description:	Display Airport pages
//
// ------------------------------------------------------------------------------

void print_apt(nvdb_apt *__apt,char *status)
{
	   // --- No active airport. Offer creating custom waypoint ---	
       if(!__apt)
		{
		   apt_page_num=1;
		   print_str(0,12,ATTRIBUTE_NORMAL," %-10s"," ");
		   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		   print_str(2,12,ATTRIBUTE_NORMAL,"CREATE NEW ");
		   print_str(3,12,ATTRIBUTE_NORMAL,"WPT AT:    ");
           print_str(4,12,ATTRIBUTE_NORMAL,"USER POS?  ");
           print_str(5,12,ATTRIBUTE_NORMAL,"PRES POS?  ");
           print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
		   update_screen();
		   return;
		}

	   // --- Displays Airport ICAO on header in most cases ---
		if(apt_page_num==3 && apt_sub_page==1)
		{
		    for(int i=0;i<6;i++)
				print_str(i,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		}
		else
		   print_str(0,12,ATTRIBUTE_NORMAL," %-10s",__apt->ICAO_ID);

		// --------------------------------------------------

		if(apt_page_num==1)
		{
		   char __name[12];
		   strncpy(__name,__apt->NAME,11);
		   __name[11]='\0';
		   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s",	__name);
		   print_str(2,12,ATTRIBUTE_NORMAL,"%-11s",strlen(__apt->NAME)>11?__apt->NAME+11:" ");
		}

		// --------------------------------------------------

		else if (apt_page_num == 2)
		{
			char __city[12] = "";
			char __city2[12] = "";
			strncpy(__city, __apt->CITY, 11);
			__city[11]='\0';
			print_str(1,12,ATTRIBUTE_NORMAL,"%-11s",__city);
			if (strlen(__apt->CITY) > 11) {
				strncpy(__city2, __apt->CITY + 11, 11);
				__city2[11] = '\0';
			}
			else {
				strcpy(__city2, " ");
			}
			print_str(2, 12, ATTRIBUTE_NORMAL, "%-11s", __city2);
		}

		// --------------------------------------------------

		else if (apt_page_num == 3)
		{
		   if(apt_sub_page>1)
		   {
			  char rt_pat1[4]={0};
			  char rt_pat2[4]={0};
			  char surf[16] = "";

			  int start_rnw_index = 2*(apt_sub_page-2);
		      int count_rnw_print = start_rnw_index+2 <= __apt->rnws_count ? 2 : 1;
		      for(int i=0;i<count_rnw_print;i++)
		      {
		         print_str(2+2*i,12,ATTRIBUTE_NORMAL,"%-3s/%-3s %-3s",
			               __apt->rws[start_rnw_index+i].apt3_line.pDes,
						   __apt->rws[start_rnw_index+i].apt3_line.sDes,
						   __apt->rws[start_rnw_index+i].light_flag?"L":" ");
				 strcpy(surf, __apt->rws[start_rnw_index + i].surf);
			     print_str(2+2*i+1,12,ATTRIBUTE_NORMAL,"%6d' %-3s",
				           (LONG)(__apt->rws[start_rnw_index+i].len/0.305f), surf);
                 
				 if(__apt->rws[start_rnw_index+i].pattern_flag&4)
					 _snprintf(!i ? rt_pat1 : rt_pat2 ,sizeof(rt_pat1),"%s",__apt->rws[start_rnw_index+i].apt3_line.pDes);
				 else if(__apt->rws[start_rnw_index+i].pattern_flag&32)
					 _snprintf(!i ? rt_pat1 : rt_pat2 ,sizeof(rt_pat1),"%s",__apt->rws[start_rnw_index+i].apt3_line.sDes);
		      }
		      if(count_rnw_print==1)
		      {
			     print_str(4,12,ATTRIBUTE_NORMAL,"%-11s"," ");
			     print_str(5,12,ATTRIBUTE_NORMAL,"%-11s"," ");	      
		      }	
			  if( strlen(rt_pat1) || strlen(rt_pat2) )
			     print_str(1,12,ATTRIBUTE_NORMAL," RT %-3s %-3s",rt_pat1,rt_pat2);
			  else
			     print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," "); 
		   }

		   else if (!apt_sub_page)
		   {
			   print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
			   print_str(2,12,ATTRIBUTE_NORMAL,"RUNWAY DATA");
			   print_str(3,12,ATTRIBUTE_NORMAL,"    NOT    ");
			   print_str(4,12,ATTRIBUTE_NORMAL," AVAILABLE ");
			   print_str(5,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		   }
		}

		// --------------------------------------------------

		else if (apt_page_num == 4)
		{
			if(apt_sub_page>0)
			{
			   int start_freq_index = 5*(apt_sub_page-1);
			   int count_freq_print = start_freq_index+5 <= __apt->freqs_count ? 5 : __apt->freqs_count%5;
			   int i=0;
			   for(i=0;i<count_freq_print;i++)
			   {
                  print_str(i+1,12,ATTRIBUTE_NORMAL,"%-4s %3d.%02d",comm2text(__apt->freqs[start_freq_index+i].type),__apt->freqs[start_freq_index+i].freq/1000000,(__apt->freqs[start_freq_index+i].freq%1000000)/10000);
			   }
			   for(;i<5;i++)
			      print_str(i+1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
			}
			else if(!apt_sub_page)
			{
				print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
				print_str(2,12,ATTRIBUTE_NORMAL," COMM FREQ ");
				print_str(3,12,ATTRIBUTE_NORMAL," DATA  NOT ");
				print_str(4,12,ATTRIBUTE_NORMAL," AVAILABLE ");
				print_str(5,12,ATTRIBUTE_NORMAL,"%-11s"," ");
			}
		}

		// --------------------------------------------------

		else if (apt_page_num == 5)
		{
			char ilsstr[32] = "";
			char ilspt[32] = "";

			//print_str(1,12,ATTRIBUTE_NORMAL,"REMARKS:   ");
            //print_str(2,12,ATTRIBUTE_NORMAL,"%-11s"," "); 
			//print_str(3,12,ATTRIBUTE_NORMAL,"%-11s"," "); 
			//print_str(4,12,ATTRIBUTE_NORMAL,"%-11s"," "); 
			//print_str(5,12,ATTRIBUTE_NORMAL,"%-11s"," "); 

			print_str(1,12,ATTRIBUTE_NORMAL,"RWY ILS    ");
			{
				if (apt_sub_page>0)
				{
					char rt_pat1[4] = { 0 };
					char rt_pat2[4] = { 0 };

					int start_rnw_index = 2 * (apt_sub_page - 1);
					int count_rnw_print = start_rnw_index + 2 <= __apt->rnws_count ? 2 : 1;
					for (int i = 0; i<count_rnw_print; i++)
					{
						if (__apt->rws[start_rnw_index + i].pILS == 0) {
							strcpy(ilspt, "NO ILS");
						}
						else {
							sprintf(ilsstr, "%5d", __apt->rws[start_rnw_index + i].pILS/10000);
							ilspt[0] = ilsstr[0]; 
							ilspt[1] = ilsstr[1];
							ilspt[2] = ilsstr[2];
							ilspt[3] = '.';
							ilspt[4] = ilsstr[3];
							ilspt[5] = ilsstr[4];
							ilspt[6] = 0;
						}
						print_str(2 + 2 * i, 12, ATTRIBUTE_NORMAL, "%-3s %-7s",
							__apt->rws[start_rnw_index + i].apt3_line.pDes, ilspt);
						if (__apt->rws[start_rnw_index + i].sILS == 0) {
							strcpy(ilspt, "NO ILS");
						}
						else {
							sprintf(ilsstr, "%5d", __apt->rws[start_rnw_index + i].sILS/10000);
							ilspt[0] = ilsstr[0];
							ilspt[1] = ilsstr[1];
							ilspt[2] = ilsstr[2];
							ilspt[3] = '.';
							ilspt[4] = ilsstr[3];
							ilspt[5] = ilsstr[4];
							ilspt[6] = 0;
						}
						print_str(2 + 2 * i + 1, 12, ATTRIBUTE_NORMAL, "%-3s %-7s",
							__apt->rws[start_rnw_index + i].apt3_line.sDes, ilspt);

					}
					if (count_rnw_print == 1)
					{
						print_str(4, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
						print_str(5, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
					}
				}
				else if (!apt_sub_page)
				{
					print_str(1, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
					print_str(2, 12, ATTRIBUTE_NORMAL, " ILS DATA  ");
					print_str(3, 12, ATTRIBUTE_NORMAL, "    NOT    ");
					print_str(4, 12, ATTRIBUTE_NORMAL, " AVAILABLE ");
					print_str(5, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
				}
			}
		}

		// --------------------------------------------------

		else if (apt_page_num == 6)
		{
			print_str(1,12,ATTRIBUTE_NORMAL,"%-11s"," ");
			print_str(2,12,ATTRIBUTE_NORMAL,"NO FUEL    "); 
			print_str(3,12,ATTRIBUTE_NORMAL,"%-11s"," "); 
			print_str(4,12,ATTRIBUTE_NORMAL,"NO OXYGEN  "); 
			print_str(5,12,ATTRIBUTE_NORMAL,"NO FEE INFO"); 
		}

		// --------------------------------------------------

		else if(apt_page_num==7)
		{
			print_apt7(__apt, status);
		}
		
		// --------------------------------------------------

		if(apt_page_num==1)
		{
		   print_str(3,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		   K_deg deg;
		   K_GetDegMin(__apt->Lat,&deg);
		   print_str(4,12,ATTRIBUTE_NORMAL,"%c %.2d%c%.2d.%.2d'",__apt->Lat<0?'S':'N',(int)fabs(__apt->Lat),0xB0,deg.mins,deg.dec_mins);
		   K_GetDegMin(__apt->Lon,&deg);
		   print_str(5,12,ATTRIBUTE_NORMAL,"%c%.3d%c%.2d.%.2d'",__apt->Lon<0?'W':'E',(int)fabs(__apt->Lon),0xB0,deg.mins,deg.dec_mins);
        }

		// --------------------------------------------------

		else if (apt_page_num == 2)
		{
		   print_str(3,12,ATTRIBUTE_NORMAL,"ELV%6dFT",(LONG)(__apt->Alt*3.281));
		   K_change_attribute(3,21,2,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL);
		   print_str(4,12,ATTRIBUTE_NORMAL,"%-11s"," "); print_str(5,12,ATTRIBUTE_NORMAL,"%-11s"," ");
		}

		if(*status == 'A')
		{
		   print_str(6,18,ATTRIBUTE_NORMAL,"APT %d",apt_page_num);
		   if(apt_page_num==3)
		   {
			   if(__apt->rnws_count>0)
				   print_str(6,21,ATTRIBUTE_NORMAL,"+");
		   }
		   if(apt_page_num==4)
		   {
			   if(apt_last_sub_page>1)
			      print_str(6,21,ATTRIBUTE_NORMAL,"+");
		   }
		   if (apt_page_num == 5)
		   {
			   if (__apt->rnws_count>0)
				   print_str(6, 21, ATTRIBUTE_NORMAL, "+");
		   }
		}
		else
		{
		   print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
		   K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		}
		if(!(apt_page_num==3 && apt_sub_page==1))
		   print_navaid_arrow(__apt);
		update_screen();
		return;
}



// -------------------------------------------------------------------------------------------------
// Name				do_apt_page()
// Description		Performs the appropriate action on the Aiport page based in the action ID
// Parameters	
//		ACTION		Action to perform
// Returns			0 or 1 ???		
// Depends on		
//		?
// Main stages
//		force_nr_mode (overflows)
//		ACTION == ACTION_INIT (explicit return 0)
//		ACTION == ACTION_SHOW (overflows)
//		!nav_db.apts_count (explicit return 0)
//		ACTION == PAGE_FORCE_CHANGE (explicit return 0)
//		ACTION == ACTION_FREE_RESOURCES (explicit return 0)
//		ACTION == ACTION_TIMER && !cursored  (explicit return 1)
//		ACTION == ACTION_TIMER && cursored  (overflows)
//		ACTION_TIMER && dup_ident_start (overfow) Is this OK? Should not be: if(ACTION == ACTION_TIMER && dup_ident_start)
//		ACTION == INPUT_RCURSOR (overfow)
//		ACTION == INPUT_ROUTERPLUS (overflow)
//		ACTION == INPUT_ENTER (overflow)
//		ACTION == INPUT_ROUTERMINUS (overflow)
//		ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS (overflow)
//		ACTION == INPUT_PULLSCAN (overflow)
//		return 0
// Comments
//		Confusing mixing of non exclusive and exclusive stage conditions (with return or overflow)
//		Not logical status, see HGCOMMENT in the code
// -------------------------------------------------------------------------------------------------

int do_apt_page(int ACTION)
{
    static int cursored = 0;
	static int current_col = 0;
	static int current_row = 0;
    static long written = 0;  
    static long *apt_id = NULL;
	static int dup_ident = 0;
	static DWORD dup_ident_start = 0;
	static int dup_index = 0;

	int icao_ed_col = 13;
	int icao_ed_row = 0;
	int name_ed_col = 12;
	int name_ed_row = 1;


	enum {
		ACTION_INIT = -10, ACTION_TIMER = -9, MULTI_LINE_SELECTED = -8, ACTION_FREE_RESOURCES = -7,
		ACTION_FPLDTO_DONE = -6, PAGE_FORCE_CHANGE = -5, ACTION_SHOW = -4, ACTION_HIDE = -3, ACTION_SUP_PT_CREATED = -2
	};

	// Logging only...
	if ((ACTION != ACTION_TIMER) && (ACTION >= ACTION_INIT && ACTION <= ACTION_SUP_PT_CREATED) )
	{
		static char* actionstr[] = { "ACTION_INIT", "ACTION_TIMER", "MULTI_LINE_SELECTED", "ACTION_FREE_RESOURCES", "ACTION_FPLDTO_DONE", "PAGE_FORCE_CHANGE", "ACTION_SHOW", "ACTION_HIDE", "ACTION_SUP_PT_CREATED", "-1", "0", "1", "2", };
		K_DEBUG("[HG] ENTERING: do_apt_page(%s) | force_nr_mode=%d | dup_ident_start=%d | cursored=%d\n", actionstr[ACTION + 10], force_nr_mode, dup_ident_start, cursored);
	}
	// ... logging end

	if (force_nr_mode) {
		current_list = NR_LIST;
		in_nr_list_pos = 0;
		apt_list_index = nr_apt_l.list[0].index;
		current_apt = &nav_db.apts[apt_list_index];
		check_multi_page();
		force_nr_mode = 0;
	}

	if(ACTION == ACTION_INIT)
	{
		build_nearest_list();
		apt_page_num=4;
		if(nr_apt_l.nr_apt_count>0)
		{
			current_apt = &nav_db.apts[nr_apt_l.list[0].index];
			ss_load_on_demand(current_apt);
		}
		else
		{
			if(nav_db.apts_count)
			{
				current_apt = &nav_db.apts[0];
			    ss_load_on_demand(current_apt);
			}
			else
			{
				current_apt=NULL;
				do_status_line(ADD_STATUS_MESSAGE,"NO APT WPTS");
			}

		}
		check_multi_page();
		APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		do_apt7(current_apt, current_row, ACTION, APT_INPUT_MASK);
		return(0);
	}
	if(ACTION == ACTION_SHOW)
	{
		if(!nav_db.apts_count)
			do_status_line(ADD_STATUS_MESSAGE,"NO APT WPTS");
	}
	if(!nav_db.apts_count)
	{
		current_apt=NULL;
		current_list=COMPLETE_LIST;
		print_ndb(NULL, "APT 1");
		return(0);
    }
   if(ACTION == PAGE_FORCE_CHANGE)
	{
       if(cursored && current_list == COMPLETE_LIST)
       do_apt_page(INPUT_RCURSOR);
	   return(0);
	}
    if(ACTION == ACTION_FREE_RESOURCES)
	{
        current_apt=NULL;
		apt_id=NULL;
        cursored=current_col=current_row=written=dup_ident=
		dup_ident_start=dup_index=0;
		return(0);
	}
	if(ACTION == ACTION_TIMER && !cursored)
	{
		if(!current_apt)
		{
			apt_list_index = 0;
			current_apt = &nav_db.apts[apt_list_index];
			ss_load_on_demand(current_apt);
			check_multi_page();
     	}
		if(current_list == COMPLETE_LIST)
	       print_apt(current_apt,"APT 1");
		else if(current_list == NR_LIST)
		{
		   int ind = find_in_nr_list(current_apt);
		   if(ind>=0)
		   {
		      print_nr_apt(current_apt,ind,cursored,"APT 1");
              in_nr_list_pos=ind;
		   }
		   else
		   {
			   print_apt(current_apt,"APT 1");
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
		   if(nr_apt_l.nr_apt_count > in_nr_list_pos)
		   {
		      apt_list_index = nr_apt_l.list[in_nr_list_pos].index;
		      current_apt = &nav_db.apts[apt_list_index];
			  ss_load_on_demand(current_apt);
			  check_multi_page();
		   }
		   else
		   {
		      current_apt    = NULL;
		      apt_list_index = -1;
		   }
		   print_nr_apt(current_apt,in_nr_list_pos,cursored,"APT 1");
		   K_change_attribute(0,19,4,ATTRIBUTE_NORMAL|ATTRIBUTE_INVERSE);
		   update_screen();
	   }
	   else
	   {
		   do_apt7(current_apt, current_row, ACTION, APT_INPUT_MASK);
	   }
	}
	//==========================================================================
	if(ACTION_TIMER && dup_ident_start)			// HGCOMMENT: Is this OK? Should not be: if(ACTION == ACTION_TIMER && dup_ident_start)
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
		      APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			  current_col=icao_ed_col;
		      current_row=icao_ed_row;
			  do_apt7(current_apt, current_row, ACTION, APT_INPUT_MASK); 
			  check_multi_page();
			  print_apt(current_apt,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      //=========================================================================
		      apt_id = find_apts_by_icao(current_apt->ICAO_ID,&written);
		      if(apt_id)
			     qsort(apt_id,written,sizeof(*apt_id),apt_dup_sort);
              check_multi_page();
		      print_apt(current_apt,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      if(written>1)
		      {
                 //print_str(6,6,ATTRIBUTE_INVERSE," DUP IDENT ");
				 do_status_line(ADD_STATUS_MESSAGE," DUP IDENT ");
                 dup_index = get_apt_index(apt_id,written,current_apt);
			     apt_list_index = apt_id[dup_index];
			     dup_ident = 1;
	             dup_ident_start = GetTickCount();
			     if(K_is_scanpull())
                    K_change_attribute(icao_ed_row,icao_ed_col,4,ATTRIBUTE_INVERSE);
              }
		      //else 
              //   print_str(6,14,ATTRIBUTE_NORMAL,"   ");

              update_screen();
			  cursored=1;
		   }
		   else if( current_list == NR_LIST )
		   {
			   APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			   cursored=1;
		   }
		}
		else 
		{
		   if( current_list == COMPLETE_LIST )
		   {
		      if(K_is_scanpull())
			     APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			  else
			     APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERMINUS|INPUT_RINNERPLUS;
		      dup_ident_start=0;
			  apt_id=NULL;
		      written=0;
		      dup_ident=0;
			  cursored=0;
			  apt7_reload_ss(current_apt);
		   }
		   else if( current_list == NR_LIST )
		   {
			   APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
			   cursored=0;
		   }
		}
	}

	// --------------------------------------------------
	// When rotating the outer knob clockwise (positive direction)
	if (ACTION == INPUT_ROUTERPLUS)
	{
		if(K_is_scanpull()) 
			return(1);

		// If cursor is in the ICAO name field and not the last character...
		if ((current_row == 0) &&
			(K_get_char(0, current_col) != BLANK_CHAR) &&
			(current_col < icao_ed_col + 3))
		{
			K_change_attribute(current_row, current_col, 1, ATTRIBUTE_NORMAL);
			current_col++;
			K_change_attribute(current_row, current_col, 1, ATTRIBUTE_INVERSE);
		}

		else if (apt_page_num == 7)
			do_apt7(current_apt, current_row, ACTION, APT_INPUT_MASK);
	}

	// --------------------------------------------------
	// When pressing ENTER on the Airport page
	//
	// Workflow:
	//		SID:  SS_SEL_SS -> SS_SEL_RNW -> (SS_SEL_TRA) -> Load to FP
	//		STAR: SS_SEL_SS -> (SS_SEL_TRA) -> SS_SEL_RNW -> Load to FP 

	if(ACTION == INPUT_ENTER)
	{
		do_apt7(current_apt, current_row, ACTION, APT_INPUT_MASK);
	}

	// --------------------------------------------------
	// When rotating the outer knob couterclockwise (negative direction)
	if(ACTION == INPUT_ROUTERMINUS)
	{
		if(K_is_scanpull()) return(1);

		if(current_col > icao_ed_col)
		{
            K_change_attribute(current_row,current_col,1,ATTRIBUTE_NORMAL);
			current_col--;
			K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		}


		if(( apt_page_num == 7) && (current_row >= 2))
			do_apt7(current_apt, current_row, ACTION, APT_INPUT_MASK);
	}

	// --------------------------------------------------
	// When rotating the inner knob to any direction
	if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS )
	{ 
        if(K_is_scanpull() && dup_ident)
		{
			if(ACTION == INPUT_RINNERPLUS) dup_index = dup_index < (written-1) ? ++dup_index : 0;
			if(ACTION == INPUT_RINNERMINUS) dup_index = dup_index > 0 ? --dup_index : (written-1);
			apt_list_index = apt_id[dup_index];
		    current_apt = &nav_db.apts[apt_list_index];
			ss_load_on_demand(current_apt);
			check_multi_page();
			print_apt(current_apt,"CRSR ");
			K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			K_change_attribute(icao_ed_row,icao_ed_col,4,ATTRIBUTE_INVERSE);
		}
		else if(K_is_scanpull() && !cursored)
		{
			if(ACTION == INPUT_RINNERPLUS) 
			{
				if( 
					apt_list_index < (nav_db.apts_count-1)
					&& 
					(current_list==COMPLETE_LIST)
				  )
				{
				   current_apt=&nav_db.apts[++apt_list_index];
				   ss_load_on_demand(current_apt);
				   check_multi_page();
				}
				else if(current_list==NR_LIST)
				{
				    if(in_nr_list_pos < 9)
					{
				        in_nr_list_pos++;
						if(in_nr_list_pos < nr_apt_l.nr_apt_count)
						{
						   apt_list_index=nr_apt_l.list[in_nr_list_pos].index;
						   current_apt=&nav_db.apts[apt_list_index];
						   ss_load_on_demand(current_apt);
						   check_multi_page();
						}
						else
						{
						   apt_list_index=0;
						   current_apt=&nav_db.apts[apt_list_index];
						   check_multi_page();
						   current_list=COMPLETE_LIST;
						}
					}
				}
			}
			//======================================================================================================
			if(ACTION == INPUT_RINNERMINUS)
			{
			    if(!apt_list_index && (current_list==COMPLETE_LIST))
				{
	                if(nr_apt_l.nr_apt_count>0)
					{
					    current_list=NR_LIST;
						in_nr_list_pos=0;
						apt_list_index=nr_apt_l.list[0].index;
						current_apt=&nav_db.apts[apt_list_index];
						check_multi_page();
					}
				}
				else if(current_list==NR_LIST)
				{
				    if(in_nr_list_pos>0)
					{
				        in_nr_list_pos--;
						if(in_nr_list_pos < nr_apt_l.nr_apt_count)
						{
						   apt_list_index=nr_apt_l.list[in_nr_list_pos].index;
						   current_apt=&nav_db.apts[apt_list_index];
						   check_multi_page();
						}
						else
						{
						   current_list=COMPLETE_LIST;
						   check_multi_page();
						}
					}
				}
				else
				{
				    apt_list_index = apt_list_index > 0? --apt_list_index : 0;
					current_apt = &nav_db.apts[apt_list_index];
					ss_load_on_demand(current_apt);
					check_multi_page();
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
		   apt_id = find_apts_by_icao(curr_id,&written);
		   if(apt_id)
		   {
		      qsort(apt_id,written,sizeof(*apt_id),apt_dup_sort);
			  apt_list_index = apt_id[0];
			  current_apt = &nav_db.apts[apt_list_index];
			  ss_load_on_demand(current_apt);
			  check_multi_page();
			  print_apt(current_apt,"CRSR ");
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
			   print_apt(NULL,"CRSR ");
			   print_str(0,12,ATTRIBUTE_NORMAL," %-10s",curr_id);
			   K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		       K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			   //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			   dup_ident_start=0;
			   //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG ");
		       update_screen();
		    }
		}
		else
		{
		   if(ACTION == INPUT_RINNERPLUS)
		   {
		      apt_next_page();
		   }
		   if(ACTION == INPUT_RINNERMINUS)
		   {
		      apt_prev_page();
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
		      K_change_attribute(icao_ed_row,icao_ed_col,4,ATTRIBUTE_INVERSE);
			  //print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			  dup_ident_start=0;
			  //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG ");
			  update_screen();
		   }
		   if(dup_ident && !K_is_scanpull())
		   {
		      K_change_attribute(icao_ed_row,icao_ed_col,4,ATTRIBUTE_NORMAL);
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      dup_ident_start=0;
		      //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG    ");
		      update_screen();
		   }
		   if(!dup_ident && K_is_scanpull() && cursored)
			   //main_loop(INPUT_RCURSOR);
			   do_apt_page(INPUT_RCURSOR);
		   if(!cursored && K_is_scanpull())
               APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		   if(!cursored && !K_is_scanpull())
			   APT_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;

		}
        if(!K_is_scanpull() && current_list == NR_LIST)
		{
		   current_list = COMPLETE_LIST;
		   if(cursored)
		      //main_loop(INPUT_RCURSOR);
			  do_apt_page(INPUT_RCURSOR);
		}
	}
	//==========================================================================
	return(1);
}

nvdb_apt *get_current_apt(void)
{
   return(current_apt);
}

BOOL do_draw_rnws(void)
{
   if(apt_page_num==3 && apt_sub_page==1 && current_list==COMPLETE_LIST)
	   return(TRUE);
   else
	   return(FALSE);
}
void apt_print_apt(nvdb_apt *__apt,char *status,int apt_page,int sub_page)
{
   int c_apt_page=apt_page_num;
   int c_apt_sub_page=apt_sub_page;

   apt_page_num=apt_page;
   apt_sub_page=sub_page;
   print_apt(__apt,status);
   apt_page_num=c_apt_page;
   apt_sub_page=c_apt_sub_page;
}



void apt_next_page_ext(int main_page,int sub_page,int *p_main_page,int *p_sub_page,nvdb_apt *__apt)
{  
	static int max_main_page=4;
	static int min_main_page=1;
	
	int __last_page;
	if(main_page==3)
       __last_page=__apt->rnws_count % 2 ? __apt->rnws_count/2 + 2 :  __apt->rnws_count/2 + 1;
	if(main_page==4)
       __last_page=__apt->freqs_count % 5 ? __apt->freqs_count/5 + 1 :  __apt->freqs_count/5;

	if(main_page == 3 && __apt->rnws_count && sub_page<__last_page)
	{		
		if((sub_page+1)==1) *p_sub_page=2; else *p_sub_page=(sub_page+1);
		return;
	}
	if(main_page == 4 && __apt->freqs_count && sub_page<__last_page)
	{
		*p_sub_page=sub_page+1;
		return;
	}
	*p_main_page = main_page+1;

	if(*p_main_page>max_main_page) *p_main_page=min_main_page;

	if(*p_main_page == 3)
	{
		__last_page = __apt->rnws_count % 2 ? __apt->rnws_count/2 + 2 :  __apt->rnws_count/2 + 1;
		if(__apt->rnws_count)
			*p_sub_page=2;
		else
			*p_sub_page=0;
	}
	if(*p_main_page == 4)
	{
		__last_page = __apt->freqs_count % 5 ? __apt->freqs_count/5 + 1 :  __apt->freqs_count/5;
		if(__apt->freqs_count)
			*p_sub_page=1;
		else
			*p_sub_page=0;
	}
}

void apt_prev_page_ext(int main_page,int sub_page,int *p_main_page,int *p_sub_page,nvdb_apt *__apt)
{  
	static int max_main_page=4;
	static int min_main_page=1;
    
	int __last_page;
	if(main_page==3)
		__last_page=__apt->rnws_count % 2 ? __apt->rnws_count/2 + 2 :  __apt->rnws_count/2 + 1;
	if(main_page==4)
		__last_page=__apt->freqs_count % 5 ? __apt->freqs_count/5 + 1 :  __apt->freqs_count/5;

	if((main_page==3||main_page==4||main_page==5) && sub_page>1)
	{
		*p_sub_page = sub_page-1;
		if(main_page==3 && *p_sub_page==1)
			*p_main_page=2;
		return;
	}
	*p_main_page = main_page-1;

	if(*p_main_page<min_main_page) *p_main_page=max_main_page;

	if(*p_main_page == 3)
	{
		if(__apt->rnws_count)
		{
			__last_page = __apt->rnws_count % 2 ? __apt->rnws_count/2 + 2 :  __apt->rnws_count/2 + 1;
			*p_sub_page = __last_page;
		}
		else
			*p_sub_page=0;
	}
	if(*p_main_page == 4)
	{
		if(__apt->freqs_count)
		{
			__last_page = __apt->freqs_count % 5 ? __apt->freqs_count/5 + 1 :  __apt->freqs_count/5;
			*p_sub_page = __last_page;
		}
		else
			*p_sub_page=0;
	}
}
