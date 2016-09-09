#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include <stdio.h>
#include <math.h>

#define ACTION_INTERNAL_IS_CREATING 0x08880000

extern nav_db_t nav_db;


nvdb_wpt *current_sup = NULL;

enum {COMPLETE_LIST,NR_LIST};
static long current_list = COMPLETE_LIST;
static long  sup_list_index=0;
enum {CRMODE_BEG,CRMODE_DATA};
static int CRMODE;

static int current_col = 0;
static int current_row = 0;

static nv_point_t ref_nv_point;


extern int count_of_elem;
extern char str_elems[99][12];
extern int MULTI_LINE_CALLER;
extern int current_item;

extern map<unsigned long,vector<int> >icao_index;
extern nav_db_t nav_db;

static int SUP_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;

BOOL sup_handle_key(int INPUT_MASK)
{
    return(INPUT_MASK&SUP_INPUT_MASK);
}
long get_sups_count(void)
{
	return(nav_db.sups_count+usr_spec_points(NAVAID_SUP));
}

nvdb_wpt *get_sup_by_id(long id)
{
	if(!get_sups_count()) return(NULL);
	if(id>=0 && id<nav_db.sups_count) return(&nav_db.sups[id]);
	else if(id<0) return((nvdb_wpt *)usr_get_point(id));
    long usr_number = id-nav_db.sups_count;
    return((nvdb_wpt *)usr_wpt_by_id(NAVAID_SUP,usr_number));
}
long get_sup_actual_index(long j)
{  
   if(j<nav_db.sups_count) return(j);
   long usr_number = j-nav_db.sups_count;
   return(usr_index_by_id(NAVAID_SUP,usr_number));
}
void sup_nvpt_deleted(nvdb_wpt *delete_point)
{
   if(current_sup == delete_point)
   {
      current_sup = get_sup_by_id(0);//&nav_db.sups[sup_list_index];
   }
}
//=========================================================================================================
int sup_compare(nvdb_wpt *sup1,nvdb_wpt *sup2)
{
		if(    
			(sup1->Lat==sup2->Lat)
		    &&
			(sup1->Lon==sup2->Lon)
			&&
			!(strcmp(sup1->ICAO_ID,sup2->ICAO_ID))
		  )
		  return(0);

	return(1);
}


long get_sup_index(long *sup_id,int elements_count,nvdb_wpt *__sup)
{
	for(int i=0;i<elements_count;i++)
	{
		if(!sup_compare(get_sup_by_id(sup_id[i]),__sup))
		   return(i);
	}
	return(-1);
}

int sup_dup_sort(const void *a1,const void *a2)
{
    FLOAT64 Lon,Lat;
    get_PPOS(&Lat,&Lon);
	nvdb_wpt *_s1 = get_sup_by_id(*(long *)a1);
	nvdb_wpt *_s2 = get_sup_by_id(*(long *)a2);

	return(
	       get_S(Lat,Lon,_s1->Lat,_s1->Lon)
	       -
	       get_S(Lat,Lon,_s2->Lat,_s2->Lon)
	      );
}

long *find_sups_by_icao(char *icao_id_s,long *array_size)
{
    static long sup_indexes[1024];
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
		  if(is_sup_id(ind,&local_index))
		  {
              sup_indexes[i++] = local_index;
		  }
		}
	}
	if(i>0)
	{
	   if(array_size) *array_size=i;
	   return((long *)&sup_indexes);
	}
	
	char reg_exp[15];
	sprintf(reg_exp,"^%s",icao_id_s);
	Regexp re(reg_exp);
    re.CompiledOK();

	for(int j=0;j<get_sups_count();j++)
	{
		if(re.Match(get_sup_by_id(j)->ICAO_ID))
		{
           if(array_size) 
		      *array_size=1;

		   sup_indexes[0]=get_sup_actual_index(j);
		   return((long *)&sup_indexes);
		}
	}
	return(NULL);
}

void print_sup(nvdb_wpt *__sup,char *status)
{
		if(!__sup)
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

  	    print_str(0,12,ATTRIBUTE_NORMAL," %-10s",__sup->ICAO_ID);
		//************************************************************
		print_str(1,12,ATTRIBUTE_NORMAL,"REF:  -----");
		print_str(2,12,ATTRIBUTE_NORMAL,"RAD: ---.-%c",0xB0);
		print_str(3,12,ATTRIBUTE_NORMAL,"DIS:---.-NM");
        //************************************************************
		K_deg deg;
		K_GetDegMin(__sup->Lat,&deg);
        print_str(4,12,ATTRIBUTE_NORMAL,"%c %.2d%c%.2d.%.2d'",__sup->Lat<0?'S':'N',(int)fabs(__sup->Lat),0xB0,deg.mins,deg.dec_mins);
		K_GetDegMin(__sup->Lon,&deg);
        print_str(5,12,ATTRIBUTE_NORMAL,"%c%.3d%c%.2d.%.2d'",__sup->Lon<0?'W':'E',(int)fabs(__sup->Lon),0xB0,deg.mins,deg.dec_mins);
        print_str(6,18,ATTRIBUTE_NORMAL,"%s",status);
		print_navaid_arrow(__sup);
}

#define is_entering_data() (K_get_attribute(current_row,current_col)==(ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH))

typedef struct _col_edit_t
{
	int count;
	int *cols;
}col_edit_t;

col_edit_t *get_col_edit_table(void)
{
	static int row1[5]={18,19,20,21,22};
	static int row2[3]={17,19,21};
	static int row3[4]={16,17,18,20};
	static int row4[7]={12,14,15,17,18,20,21};
	static int row5[7]={12,13,15,17,18,20,21};
	static col_edit_t col_table[5]={{5,row1},{3,row2},{4,row3},{7,row4},{7,row5}};  
	return(col_table);
}
void print_clear_sup(int str_number=0)
{
	char *strs[]={"REF:  -----","RAD: ---.-%c","DIS:---.-NM","- --%c--.--'","----%c--.--'"};
	if(!str_number)
	   for(int i=1;i<=5;i++)
	      print_str(i,12,ATTRIBUTE_NORMAL,strs[i-1],0xB0);
	else
       print_str(str_number,12,ATTRIBUTE_NORMAL,strs[str_number-1],0xB0);
	update_screen();
}

int get_start_edit_col(int __row)
{
	int cols_arr[5]={18,17,16,12,12};
	__row--;
	if(__row>=0 && __row<=4)
		return(cols_arr[__row]);
	return(-1);
}

void edit_next_char(int __row,int __col,int ACTION,char c_min,char c_max)
{
   char ch = K_get_char(current_row,current_col);
   if(ch=='-') ch=c_min;  else ACTION == INPUT_RINNERPLUS ? ch++ : ch--;
   if(ch < c_min) ch=c_max; else if(ch>c_max) ch=c_min;
   K_set_char(current_row,current_col,ch);
   K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
}


void undash_row(int __row)
{
	col_edit_t *table = get_col_edit_table();
	for(int i=0;i<table[__row-1].count;i++)
	{
		if(K_get_char(__row,table[__row-1].cols[i]) == '-')
			K_set_char(__row,table[__row-1].cols[i],'0');
		if((__row==5&&table[__row-1].cols[i]==13) || (__row==2&&table[__row-1].cols[i]==17))
			if(K_get_char(__row,table[__row-1].cols[i]+1) == '-')
			   K_set_char(__row,table[__row-1].cols[i]+1,'0');

	}
}

void dash_row(int __row)
{
	col_edit_t *table = get_col_edit_table();
	for(int i=0;i<table[__row-1].count;i++)
	{
       K_set_char(__row,table[__row-1].cols[i],'-');
       if((__row==5&&table[__row-1].cols[i]==13) || (__row==2&&table[__row-1].cols[i]==17))
          K_set_char(__row,table[__row-1].cols[i]+1,'-');
	}
}

static long *cr_ids_list;
static long cr_ids_list_count;


void edit_row(int __row,int ACTION)
{
   nv_point_t __nv;

	switch(__row)
   {
   case 1:
	   {
		   char ch = ACTION == INPUT_RINNERPLUS ? K_next_char(K_get_char(current_row,current_col)) : 
		                                          K_prev_char(K_get_char(current_row,current_col));
		   K_set_char(current_row,current_col,ch);	   

		   col_edit_t *table = get_col_edit_table();
		   int first_col = table[current_row-1].cols[0];
//		   for(int i=current_col+1;i<=last_col;i++)
//		      print_str(current_row,i,ATTRIBUTE_INVERSE," ");
		   char __ICAO_ID[6];
		   K_get_string(current_row,first_col,current_col,__ICAO_ID);
		   //======================================================
		   cr_ids_list = find_ids_by_icao(__ICAO_ID,&cr_ids_list_count);
		   if(cr_ids_list)
		   {
		       get_icao_by_id(cr_ids_list[0],NULL,&__nv);
			   print_str(current_row,first_col,ATTRIBUTE_INVERSE,"%-5s",
			             ((nv_hdr_t *)__nv.buffer_spec)->ICAO_ID);
		   }
		   else
		   {
			   print_str(current_row,first_col,ATTRIBUTE_INVERSE,"%-5s",__ICAO_ID);		   
		   }
		   K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
		   
	   }
	   break;
   case 2:
	   {
		   if(current_col==17)
		   {
			   if(!is_entering_data())
				   dash_row(current_row);
			   char decs = K_get_char(current_row,current_col);
			   char ones = K_get_char(current_row,current_col+1);
			   int sum;
			   if(decs == '-' || ones=='-') sum=0;
			   else
			   {
				   sum = (decs-'0')*10+ones-'0';
				   ACTION == INPUT_RINNERPLUS ? sum++ : sum--;
				   if(sum<0) sum = 35; else if(sum>35) sum=0;
			   }
			   print_str(current_row,current_col,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"%02d",sum);   
		   }
		   else if(current_col == 19 || current_col==21)
		   {
		      edit_next_char(current_row,current_col,ACTION,'0','9');
		   }
	   }
	   break;
   case 3:
	   {
		   if(!is_entering_data())
			   dash_row(current_row);
	       edit_next_char(current_row,current_col,ACTION,'0','9');
	   }
	   break;
   case 4:
	   {
		   if(!is_entering_data())
			   dash_row(current_row);
		   if(current_col==12)
		   {
			   char ch = K_get_char(current_row,current_col);
			   K_set_char(current_row,current_col,ch == '-' || ch == 'S' ? 'N' : 'S');
			   K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
		   }
		   if(current_col==14 || current_col==17)
			   edit_next_char(current_row,current_col,ACTION,'0',current_col==14 ? '8' : '5');
		   if(current_col==15 || current_col==18 || current_col==20 || current_col==21)
              edit_next_char(current_row,current_col,ACTION,'0','9');		   
	   }
	   break;
   case 5:
	   {
		  if(!is_entering_data())
			   dash_row(current_row);
	      if(current_col==12)
	      {
	         char ch = K_get_char(current_row,current_col);
		     K_set_char(current_row,current_col,ch == '-' || ch == 'E' ? 'W' : 'E');
		     K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH);
	      }
		  if(current_col==13)
		  {
		    char decs = K_get_char(current_row,current_col);
			char ones = K_get_char(current_row,current_col+1);
			int sum;
			if(decs == '-' || ones=='-') sum=0;
			else
			{
			   sum = (decs-'0')*10+ones-'0';
			   ACTION == INPUT_RINNERPLUS ? sum++ : sum--;
               if(sum<0) sum = 17; else if(sum>17) sum=0;
			}
			print_str(current_row,current_col,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"%02d",sum);
		  }
	      if(current_col==17)
	      {
	         edit_next_char(current_row,current_col,ACTION,'0','5');
	      }
	      if(current_col==15 || current_col==18 || current_col==20 || current_col==21)
	      {
             edit_next_char(current_row,current_col,ACTION,'0','9');		   
	      }   
	   }
	   break;
   }
}


void next_editing_col(int __row,int __col)
{
	col_edit_t *table = get_col_edit_table();
	int amount = table[__row-1].count;
    int last_col = table[__row-1].cols[amount-1];
	if(__col == last_col) current_col=table[__row-1].cols[0];
	else
	{
	   for(int i=0;i<amount;i++)
	   {
	      if(table[__row-1].cols[i]==__col)
		  {
		     current_col = table[__row-1].cols[i+1];
			 break;
		  }
	   }
	}
	K_change_attribute(__row,table[__row-1].cols[0],
		               table[__row-1].cols[amount-1]-table[__row-1].cols[0]+1,
					   ATTRIBUTE_INVERSE);
	K_change_attribute(__row,current_col,(__row==5&&current_col==13) || (__row==2&&current_col==17) ?2:1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH); 
}
void prev_editing_col(int __row,int __col)
{
	col_edit_t *table = get_col_edit_table();
	int amount = table[__row-1].count;
	int first_col = table[__row-1].cols[0];
	if(__col == first_col) current_col=table[__row-1].cols[amount-1];
	else
	{
		for(int i=0;i<amount;i++)
		{
			if(table[__row-1].cols[i]==__col)
			{
				current_col = table[__row-1].cols[i-1];
				break;
			}
		}
	}
	K_change_attribute(__row,table[__row-1].cols[0],
		table[__row-1].cols[amount-1]-table[__row-1].cols[0]+1,
		ATTRIBUTE_INVERSE);
	K_change_attribute(__row,current_col,(__row==5&&current_col==13) || (__row==2&&current_col==17)?2:1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH); 
}
void inverse_data_field(int __row,int ATTR)
{
	col_edit_t *table = get_col_edit_table();
	int amount = table[__row-1].count;

	K_change_attribute(__row,table[__row-1].cols[0],
		table[__row-1].cols[amount-1]-table[__row-1].cols[0]+1,
		ATTR);
}

BOOL check_entered_data(int __row)
{
    col_edit_t *table = get_col_edit_table();
    for(int i=0;i<table[__row-1].count;i++)
	{
	   if(K_get_char(__row,table[__row-1].cols[i]) == '-')
		   return(FALSE);
	}
	return(TRUE);
}

long find_nearby(long *__list,long __list_count)
{
   double max_S = +1e30,Lat,Lon;
   long __list_index=-1;
   get_PPOS(&Lat,&Lon);
   for(int i=0;i<__list_count;i++)
   {
      nv_point_t __nv; double temp_S;
	  get_icao_by_id(__list[i],NULL,&__nv);
	  temp_S = get_S(Lat,Lon,((nv_hdr_t*)__nv.buffer_spec)->Lat,((nv_hdr_t*)__nv.buffer_spec)->Lon);
	  if(temp_S < max_S) 
	  {
         max_S = temp_S; __list_index=i;
	  }
   }
   return(__list_index);
}
void sr_our_disp(int mode)
{
    static symbol main_part[6][11];
	static symbol status[5];
	if(mode==1)
	{
		for(int row=0;row<6;row++)
		{
			for(int col=12;col<23;col++)
			{
				main_part[row][col-12].ch        = K_get_char(row,col);
				main_part[row][col-12].attribute = K_get_attribute(row,col);
			}
		}
		for(int col=18;col<23;col++)
		{
			status[col-18].ch        = K_get_char(6,col);
			status[col-18].attribute = K_get_attribute(6,col);
		}
	}
	if(mode==0)
	{
		for(int row=0;row<6;row++)
		{
			for(int col=12;col<23;col++)
			{
			    K_set_char(row,col,main_part[row][col-12].ch);
				K_change_attribute(row,col,1,main_part[row][col-12].attribute);
			}
		}
		for(int col=18;col<23;col++)
		{
	       K_set_char(6,col,status[col-18].ch);
		   K_change_attribute(6,col,1,status[col-18].attribute);
		}
	}
}
BOOL check_for_rrd(void)
{
   if(!check_entered_data(1) || !check_entered_data(2) || !check_entered_data(3))
	   return(FALSE);
   if(!ref_nv_point.buffer_spec) return(FALSE);
   col_edit_t *table = get_col_edit_table();
   int first_col = table[0].cols[0];
   int last_col = table[0].cols[table[0].count-1];
   char __ICAO_ID[6];
   K_get_string(1,first_col,last_col,__ICAO_ID);
   for(int i=0;i<strlen(__ICAO_ID);i++)
   {
	   if(__ICAO_ID[i]==' ') 
	   {
	      __ICAO_ID[i]='\0'; break;   
	   }
   }
   if(strcmp(__ICAO_ID,((nv_hdr_t*)ref_nv_point.buffer_spec)->ICAO_ID)) return(FALSE);
   return(TRUE);
}
BOOL check_for_ll(void)
{
	if(!check_entered_data(4) || !check_entered_data(5))
		return(FALSE);
    return(TRUE);
}

void scroll_inverse(int __new_row)
{
	inverse_data_field(current_row,ATTRIBUTE_NORMAL);
	current_row=__new_row;
	if(current_row>5) current_row=1;
	else if(current_row<1) current_row=5;
	inverse_data_field(current_row,ATTRIBUTE_INVERSE);
	current_col = get_start_edit_col(current_row);
}
static char __cr_icao[6];

void make_point_and_exit(void)
{
   char temp_str[4];
   double Lat=-100000000.0f,Lon=-100000000.0f;
   if(CRMODE == CRMODE_DATA)
   {
      if(current_row==5)
      {
         char prefix = K_get_char(4,12);

	     //Lat = atof(K_get_string(4,14,15,temp_str))+(atof(K_get_string(4,17,18,temp_str))+atof(K_get_string(4,20,21,temp_str))/100.0f) / 60.0f;
		 Lat = DM2DEG(atof(K_get_string(4,14,15,temp_str)),atof(K_get_string(4,17,18,temp_str)),atof(K_get_string(4,20,21,temp_str)));
		 if(prefix=='S') Lat*=-1.0f; prefix = K_get_char(5,12);
	     //Lon = atof(K_get_string(5,13,15,temp_str))+(atof(K_get_string(5,17,18,temp_str))+atof(K_get_string(5,20,21,temp_str))/100.0f) / 60.0f;
		 Lon = DM2DEG(atof(K_get_string(5,13,15,temp_str)),atof(K_get_string(5,17,18,temp_str)),atof(K_get_string(5,20,21,temp_str)));
	     if(prefix=='W') Lon*=-1.0f;
      }
      if(current_row==3)
      {
         double __rad = atof(K_get_string(2,17,19,temp_str))+atof(K_get_string(2,21,21,temp_str))/10.0f;
	     double __dis = atof(K_get_string(3,16,18,temp_str))+atof(K_get_string(3,20,20,temp_str))/10.0f;
	     get_LL(((nv_hdr_t *)ref_nv_point.buffer_spec)->Lat,
		        ((nv_hdr_t *)ref_nv_point.buffer_spec)->Lon,__dis*1.852f,__rad,&Lat,&Lon);
      }
   }
   else
	   get_PPOS(&Lat,&Lon);
   if(Lat!=-100000000.0f && Lon!=-100000000.0f)
   {
      K_trim_string(__cr_icao);
	  nv_hdr_t new_hdr;
	  nv_point_t new_point;
	  strcpy(new_hdr.ICAO_ID,__cr_icao);
	  new_hdr.Lat=Lat;
	  new_hdr.Lon=Lon;
	  new_point.buffer_spec = &new_hdr;
	  new_point.type=NAVAID_SUP;
	  long usr_index = add_usr_point(&new_point);
	  if(usr_index < 0)
         current_sup = get_sup_by_id(usr_index);
	  do_sup_page(INPUT_RCURSOR);
	  if(current_sup)
	  {
	     int left_page = get_lp_type();
	     if(left_page == FPL_PAGE)
		    do_fpl_pages(ACTION_SUP_PT_CREATED);
	     if(left_page == DTO_PAGE)
		    do_dto_page(ACTION_SUP_PT_CREATED,0);
      }
   } 
}
void do_create_input(int ACTION)
{
    static int old_col;
    static BOOL SHOW_POINT;  
	if(ACTION == INPUT_ENTER)
	{
	   if(CRMODE == CRMODE_BEG)
	   {
	      K_get_string(0,13,17,__cr_icao);
		  if(usr_get_pt_count()>=USR_MAX_POINTS)
		  {
		     do_status_line(ADD_STATUS_MESSAGE,"USR DB FULL");
			 return;
	      }
		  if(current_row==4)
		  {
		     do_status_line(ADD_STATUS_MESSAGE,"ENT LAT/LON");			 
			 CRMODE = CRMODE_DATA;
			 old_col = current_col;
			 current_col = get_start_edit_col(current_row);
		     print_clear_sup();
			 cr_ids_list=NULL;
			 cr_ids_list_count=0;
		     K_change_attribute(4,12,10,ATTRIBUTE_INVERSE);
			 SHOW_POINT=FALSE;
		  }
		  else if(current_row==5)
		  {
			  make_point_and_exit();
			  return;
		  }

	   }
	   if(CRMODE == CRMODE_DATA)
	   {
	      if(SHOW_POINT)
		  {  
		     sr_our_disp(0);
			 SHOW_POINT=FALSE;
			 inverse_data_field(current_row,ATTRIBUTE_NORMAL);
			 current_row++;
			 inverse_data_field(current_row,ATTRIBUTE_INVERSE);
			 current_col = get_start_edit_col(current_row);
		  }
		  else if(current_row==4) // Entering Latitude
		  {
		     if(is_entering_data() && !check_entered_data(current_row))
				 undash_row(current_row);
			 if(check_entered_data(current_row))
			 {
                inverse_data_field(current_row,ATTRIBUTE_NORMAL);
				current_row=5;
				inverse_data_field(current_row,ATTRIBUTE_INVERSE);
				current_col = get_start_edit_col(current_row);
			 }
		  }
		  else if(current_row==1) // Entering REF
		  {
			  if(check_entered_data(current_row))
			  {
				  if(cr_ids_list && cr_ids_list_count)
				  {
					  long nearby_id = find_nearby(cr_ids_list,cr_ids_list_count);
					  if(nearby_id>=0) 
					  {
						  get_icao_by_id(cr_ids_list[nearby_id],NULL,&ref_nv_point);
						  sr_our_disp(1);
						  SHOW_POINT=TRUE;
						  show_nv_point(&ref_nv_point,FALSE);
					  }
				  }
			  }
		  }
		  else if(current_row==2) // Entering RAD
		  {
             if(is_entering_data() && !check_entered_data(current_row))
				 undash_row(current_row);
			 scroll_inverse(current_row+1);

		  }
		  else if(current_row == 3)
		  {
		      if(is_entering_data() && !check_entered_data(current_row))
				  undash_row(current_row);
			  if(check_for_rrd())
			  {
				  make_point_and_exit();
				  return;
			  }
			  scroll_inverse(current_row+1);

		  }
		  else if(current_row == 5)
		  {
			 if(is_entering_data() && !check_entered_data(current_row))
				  undash_row(current_row);
		     if(check_for_ll())
			 {
			    make_point_and_exit();
				return;
			 }
			 scroll_inverse(current_row+1);
		  }
	   }
	}
	if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS)
	{
	   if(CRMODE != CRMODE_DATA) return;
	   if(!SHOW_POINT)
          edit_row(current_row,ACTION);
    }
    if(ACTION == INPUT_ROUTERPLUS || ACTION == INPUT_ROUTERMINUS)
	{
	    if(!SHOW_POINT)
		{
		   if(is_entering_data())
		   ACTION == INPUT_ROUTERPLUS ? next_editing_col(current_row,current_col) : 
		                                 prev_editing_col(current_row,current_col);
		   else
		   {
		      inverse_data_field(current_row,ATTRIBUTE_NORMAL);
			  ACTION == INPUT_ROUTERPLUS ? current_row++ : current_row--;
			  if(current_row>5) current_row=1;
			  else if(current_row<1) current_row=5;
			  inverse_data_field(current_row,ATTRIBUTE_INVERSE);	   
			  current_col = get_start_edit_col(current_row);
		   }
		}
	}
	if(ACTION == INPUT_CLR)
	{
	   if(SHOW_POINT)
	   {
	      sr_our_disp(0);
		  SHOW_POINT=FALSE;
		  ref_nv_point.buffer_spec=NULL;
	   }
	   else if(is_entering_data() || check_entered_data(current_row))
	   {
	      print_clear_sup(current_row);
		  inverse_data_field(current_row,ATTRIBUTE_INVERSE);	  
		  current_col = get_start_edit_col(current_row);
	   }
	}
    if(SHOW_POINT)
	   do_status_line(ENABLE_RENT,NULL);
	if(is_entering_data())
	   do_status_line(ENABLE_RENT,NULL);
	else if(current_row==3)
	   do_status_line(check_for_rrd() ? ENABLE_RENT : DISABLE_RENT,NULL);
	else if(current_row==5)
	   do_status_line(check_for_ll() ? ENABLE_RENT : DISABLE_RENT,NULL);
	else if(current_row==1 && cr_ids_list && cr_ids_list_count)
	   do_status_line(ENABLE_RENT,NULL);
	else 
	   do_status_line(DISABLE_RENT,NULL);
}

int do_sup_page(int ACTION)
{
    static int cursored = 0;
    static long written = 0;  
    static long *sup_id = NULL;
	static int dup_ident = 0;
	static DWORD dup_ident_start = 0;
	static int dup_index = 0;

	int icao_ed_col = 13;
	int icao_ed_row = 0;
	int name_ed_col = 12;
	int name_ed_row = 1;

	if(ACTION == ACTION_INIT)
	{
		SUP_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
		cursored=0;
		current_list=COMPLETE_LIST;
		return(0);
	}

	if(ACTION == ACTION_SHOW)
	{
		if(!get_sups_count())
			do_status_line(ADD_STATUS_MESSAGE,"NO SUP WPTS");
	}
	if(!get_sups_count() && !cursored)
	{
		current_sup=NULL;
		current_list=COMPLETE_LIST;
		print_ndb(NULL,"SUP  ");
	}

	if(ACTION == PAGE_FORCE_CHANGE)
	{
       if(cursored && current_list == COMPLETE_LIST)
       do_sup_page(INPUT_RCURSOR);
	   return(0);
	}
    if(ACTION == ACTION_INTERNAL_IS_CREATING)
	{
	   return cursored!=0 && current_list==COMPLETE_LIST && current_row!=0;
	}
    if(ACTION == ACTION_FREE_RESOURCES)
	{
        current_sup=NULL;
		sup_id=NULL;
        cursored=current_col=current_row=written=dup_ident=
		dup_ident_start=dup_index=0;
		return(0);
	}
	if(ACTION == ACTION_TIMER && !cursored)
	{
		if(!current_sup)
		{
			sup_list_index = 0;
			current_sup = get_sup_by_id(sup_list_index);
     	}
		if(current_list == COMPLETE_LIST)
	       print_sup(current_sup,"SUP  ");
		update_screen();
		return(1);
	}
	if(ACTION == ACTION_TIMER && cursored)
	{
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
		   SUP_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_ROUTERPLUS|INPUT_ROUTERMINUS|INPUT_RINNERPLUS|INPUT_RINNERMINUS|INPUT_ENTER|INPUT_CLR;
		   if( current_list == COMPLETE_LIST && !K_is_scanpull() )
		   {
		      current_col=icao_ed_col;
		      current_row=icao_ed_row;
		      print_sup(current_sup,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      //=========================================================================
		      if(current_sup)
			  {
			     sup_id = find_sups_by_icao(current_sup->ICAO_ID,&written);
		         if(sup_id)
			        qsort(sup_id,written,sizeof(*sup_id),sup_dup_sort);
			  }
			  else
			  {
			    sup_id=NULL; written=0;
			  }
		      print_sup(current_sup,"CRSR ");
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
		      if(written>1)
		      {
                 //print_str(6,6,ATTRIBUTE_INVERSE," DUP IDENT ");
				 do_status_line(ADD_STATUS_MESSAGE," DUP IDENT ");
                 dup_index = get_sup_index(sup_id,written,current_sup);
			     sup_list_index = sup_id[dup_index];
			     dup_ident = 1;
	             dup_ident_start = GetTickCount();
			     if(K_is_scanpull())
                    K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_INVERSE);
              }
              update_screen();
			  cursored=1;
		   }
		}
		else 
		{
		   if(K_is_scanpull())
		      SUP_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		   else
		      SUP_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;
		   if( current_list == COMPLETE_LIST )
		   {
		      //print_str(6,6,ATTRIBUTE_NORMAL,"ENR-LEG    ");
		      dup_ident_start=0;
		      sup_id=NULL;
		      written=0;
		      dup_ident=0;
			  cursored=0;
			  do_status_line(DISABLE_RENT,NULL);
		   }
		}
	}
	//==========================================================================
	if(ACTION == INPUT_ROUTERPLUS)
	{
		if(K_is_scanpull()) return(1);
		if(K_get_char(current_row,current_col) != BLANK_CHAR && current_row==0 && current_col<icao_ed_col+4)
		{
            K_change_attribute(current_row,current_col,1,ATTRIBUTE_NORMAL);
			current_col++;
			K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
      	}
		else
		{
		   if(!sup_id)
		   {
		      if(current_row==0) 
			  {
				  K_change_attribute(current_row,current_col,1,ATTRIBUTE_NORMAL);
				  current_row=4;
				  CRMODE=CRMODE_BEG;
				  do_status_line(ENABLE_RENT,NULL);
			  }
			  else if(current_row==4 && CRMODE==CRMODE_BEG) 
			  {
				  K_change_attribute(current_row,12,11,ATTRIBUTE_NORMAL);
				  current_row=5;
				  do_status_line(ENABLE_RENT,NULL);
			  }
			  else if(current_row==5 && CRMODE==CRMODE_BEG) 
			  {
			      K_change_attribute(current_row,12,11,ATTRIBUTE_NORMAL);
				  current_row=0;
				  do_status_line(DISABLE_RENT,NULL);
			  }
			  else
			  {
			     do_create_input(ACTION);
				 return(0);
			  }
			  if(current_row)
			     K_change_attribute(current_row,12,11,ATTRIBUTE_INVERSE);
			  else
                 K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		   }
		}
		
	}
	//==========================================================================
	if(ACTION == INPUT_ROUTERMINUS)
	{
		if(K_is_scanpull()) return(1);

		if(current_col > icao_ed_col && current_row==0 )
		{
            K_change_attribute(current_row,current_col,1,ATTRIBUTE_NORMAL);
			current_col--;
			K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		}
		else
		{
			if(!sup_id)
			{
				if(current_row==0) 
				{ 
					K_change_attribute(current_row,current_col,1,ATTRIBUTE_NORMAL);
					current_row=5;
					CRMODE=CRMODE_BEG;
					do_status_line(ENABLE_RENT,NULL);
				}
				else if(current_row==5 && CRMODE==CRMODE_BEG ) 
				{
					K_change_attribute(current_row,12,11,ATTRIBUTE_NORMAL);
					current_row=4;
					do_status_line(ENABLE_RENT,NULL);
				}
				else if(current_row==4 && CRMODE==CRMODE_BEG ) 
				{
					K_change_attribute(current_row,12,11,ATTRIBUTE_NORMAL);
					current_row=0;
					do_status_line(DISABLE_RENT,NULL);
				}
				else
				{
				  do_create_input(ACTION);
				  return(0);
				}
				if(current_row)
				   K_change_attribute(current_row,12,11,ATTRIBUTE_INVERSE);
				else
				   K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
			}
		}
	}
	//==========================================================================
	if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS)
	{
        if(current_row!=0 && cursored)
		{
		    do_create_input(ACTION);
			return(0);
		}
		if(K_is_scanpull() && dup_ident)
		{
			if(ACTION == INPUT_RINNERPLUS) dup_index = dup_index < (written-1) ? ++dup_index : 0;
			if(ACTION == INPUT_RINNERMINUS) dup_index = dup_index > 0 ? --dup_index : (written-1);
			sup_list_index = sup_id[dup_index];
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		    current_sup = get_sup_by_id(sup_list_index);
			print_sup(current_sup,"CRSR ");
			K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_INVERSE);
		}
		else if(K_is_scanpull() && !cursored)
		{
			if(ACTION == INPUT_RINNERPLUS) 
			{
				if( 
					//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					sup_list_index < (get_sups_count()-1)
					&& 
					(current_list==COMPLETE_LIST)
				  )
				{
				   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				   current_sup=get_sup_by_id(++sup_list_index);
				}
			}
			//======================================================================================================
			if(ACTION == INPUT_RINNERMINUS)
			{
			    sup_list_index = sup_list_index > 0? --sup_list_index : 0;
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				current_sup = get_sup_by_id(sup_list_index);
			}
        }
		else if(!K_is_scanpull() && cursored)
		{
		   char curr_char = K_get_char(current_row,current_col);
		   curr_char = ACTION == INPUT_RINNERPLUS ? K_next_char(curr_char) : K_prev_char(curr_char);
		   K_set_char(current_row,current_col,curr_char);

		   char curr_id[6];
		   K_get_string(icao_ed_row,icao_ed_col,current_col,curr_id);
		   sup_id = find_sups_by_icao(curr_id,&written);
		   if(sup_id)
		   {
		      qsort(sup_id,written,sizeof(*sup_id),sup_dup_sort);
			  sup_list_index = sup_id[0];
			  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			  current_sup = get_sup_by_id(sup_list_index);
			  print_sup(current_sup,"CRSR ");
			  K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			  if(written>1)
			  {
				 do_status_line(ADD_STATUS_MESSAGE," DUP IDENT ");
				 dup_index = 0;
				 dup_ident = 1;
	             dup_ident_start = GetTickCount();
			  }
			  else if(written>0)
			  {
				 dup_ident_start=0;
				 dup_ident = 0;
			  }
			  else
			  {
				 dup_ident_start=0;
				 dup_ident = 0;
			  }
		      update_screen();
		    }
		    else
		    {
			   print_sup(NULL,"CRSR ");
			   print_str(0,12,ATTRIBUTE_NORMAL," %-10s",curr_id);
			   K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		       K_change_attribute(6,18,4,ATTRIBUTE_INVERSE);
			   dup_ident_start=0;
		       update_screen();
		    }
		}
	}
	//==========================================================================
	if(ACTION == INPUT_PULLSCAN)
	{
		if(current_row)
		   do_sup_page(INPUT_RCURSOR);
		if(current_list == COMPLETE_LIST)
		{
		   if(dup_ident && K_is_scanpull())
		   {
		      K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_INVERSE);
			  dup_ident_start=0;
			  update_screen();
		   }
		   if(dup_ident && !K_is_scanpull())
		   {
		      K_change_attribute(icao_ed_row,icao_ed_col,5,ATTRIBUTE_NORMAL);
		      K_change_attribute(current_row,current_col,1,ATTRIBUTE_INVERSE);
		      dup_ident_start=0;
		      update_screen();
		   }
		   if(!dup_ident && K_is_scanpull() && cursored)
			   do_sup_page(INPUT_RCURSOR);
		   if(!cursored && K_is_scanpull())
               SUP_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN|INPUT_RINNERPLUS|INPUT_RINNERMINUS;
		   if(!cursored && !K_is_scanpull())
			   SUP_INPUT_MASK = INPUT_RCURSOR|INPUT_PULLSCAN;

		}
	}
	//==========================================================================
    if(ACTION == INPUT_ENTER)
	{
		if(current_row && cursored)
		   do_create_input(ACTION);
	}
	if(ACTION == INPUT_CLR)
	{
	   if(current_row && cursored)
		   do_create_input(ACTION);
	}
	return(1);
}
nvdb_wpt *get_current_sup(void)
{
   BOOL is_enter = do_sup_page(ACTION_INTERNAL_IS_CREATING);
   return(is_enter ? NULL : current_sup);
}
BOOL sup_is_creating(void)
{
   return(do_sup_page(ACTION_INTERNAL_IS_CREATING));
}
int sup_create_new_point(char *icao_code)
{
	if(K_is_scanpull()) return(-1);
    if(do_sup_page(ACTION_INTERNAL_IS_CREATING)) return(-2);

	do_sup_page(PAGE_FORCE_CHANGE);
	nvdb_wpt *_curr=current_sup;
	current_sup=NULL;
    do_sup_page(INPUT_RCURSOR);
    print_str(0,13,ATTRIBUTE_NORMAL,"%-5s",icao_code);
	current_row=4;
	current_col=13+strlen(icao_code);
	CRMODE=CRMODE_BEG;
	do_status_line(ENABLE_RENT,NULL);
	K_change_attribute(current_row,12,11,ATTRIBUTE_INVERSE);
	update_screen();
	current_sup=_curr;
	return(0);
}
