#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include <stdio.h>

extern CRITICAL_SECTION video_buffer_cs;
extern symbol video_buffer[7][23];
extern symbol shadow_buffer[7][23];
extern int INSELFTEST_CDI;
extern INT GLOBAL_STATE;
extern char *monthes[];
extern DWORD interface_kohlsman;
enum {BUTTON_NONE,BUTTON_ENTER};
typedef struct _menu_item
{
   int menu_item;
   int row;
   int col;
   int len;
   int attribute;
   int add_buttons;
   int add_button_attribute;
}menu_item;

enum {POPMI_DATE,POPMI_TIME,POPMI_TZ,POPMI_BARO1,POPMI_BARO2,POPMI_BARO3,POPMI_APPROVE};
const int power_on_menu_count = 7;
menu_item power_on_menu[power_on_menu_count] = {
	{0,1,14,9,ATTRIBUTE_INVERSE,BUTTON_NONE,ATTRIBUTE_NORMAL},
	{1,2,12,5,ATTRIBUTE_INVERSE,BUTTON_NONE,ATTRIBUTE_NORMAL},
	{2,2,20,3,ATTRIBUTE_INVERSE,BUTTON_NONE,ATTRIBUTE_NORMAL},
	{3,4,17,2,ATTRIBUTE_INVERSE,BUTTON_NONE,ATTRIBUTE_NORMAL},
	{4,4,20,1,ATTRIBUTE_INVERSE,BUTTON_NONE,ATTRIBUTE_NORMAL},
	{5,4,21,1,ATTRIBUTE_INVERSE,BUTTON_NONE,ATTRIBUTE_NORMAL},
	{6,5,14,8,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,BUTTON_ENTER,ATTRIBUTE_FLASH}
};

enum {DAY,MONTH,YEAR1,YEAR2};

typedef struct _edit_item
{
   int row;
   int col;
   int len;
   int attribute;
}edit_item;

enum {HOUR,MIN1,MIN2};
char *TZs[]={"UTC","GST","GDT","ATS","ATD","EST","EDT","CST","CDT","MST","MDT","PST",
"PDT","AKS","AKD","HAS","HAD","SST","SDT"};

int pop_edit_date(int ACTION)
{
   static int day = 0;
   static int month = -1;
   static int year_1 = -1;
   static int year_2 = -1;
   static int current_item = DAY;
   static edit_item this_items[4]={
	   {1,14,2,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH},
	   {1,17,3,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH},
	   {1,21,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH},
	   {1,22,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH}
   };
   if(ACTION == -10)
   {
      day = 0;
      month = -1;
      year_1 = -1;
      year_2 = -1;
	  current_item = DAY;

	   
	     print_str(1,14,ATTRIBUTE_INVERSE,"__ ___ __");
		 shadow_buffer[1][14].attribute |= ATTRIBUTE_FLASH;
		 shadow_buffer[1][15].attribute |= ATTRIBUTE_FLASH;
	  update_screen();
	  return(0);
   }

   if(ACTION == INPUT_ROUTERPLUS || ACTION == INPUT_ROUTERMINUS)
   {
      if( ACTION == INPUT_ROUTERPLUS )
	  {
	     if(current_item == YEAR2) current_item = DAY; else current_item++;
	  }

      if( ACTION == INPUT_ROUTERMINUS )
	  {
	     if(current_item == DAY) current_item = YEAR2; else current_item--;
	  }
       
	     for(int col=0;col<9;col++)
			 shadow_buffer[1][14+col].attribute = ATTRIBUTE_INVERSE;
		 for(int i=0;i<this_items[current_item].len;i++)
			 shadow_buffer[this_items[current_item].row][this_items[current_item].col+i].attribute = 
			 this_items[current_item].attribute;
	  update_screen();
	  return(0);
   }
   //***************** Change Data **************************************************************
   if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS)
   {
      if( current_item == DAY )
	  {
         if( ACTION == INPUT_RINNERPLUS  )
		 {
		     if( day == 31 ) day=1; else day++;
		 }
		 if( ACTION == INPUT_RINNERMINUS )
		 {
		     if( day == 0 || day == 1 ) day = 31; else day--;
		 }
		 char str_day[3]={0,0,0};
		 sprintf(str_day,"%.2d",day);
	      
	        print_str(1,14,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,str_day);
	     update_screen();
	  }
	  if( current_item == MONTH )
	  {
         if( ACTION == INPUT_RINNERPLUS  )
		 {
		     if( month == 11 ) month=0; else month++;
		 }
		 if( ACTION == INPUT_RINNERMINUS )
		 {
		     if( month == 0 || month == -1 ) month = 11; else month--;
		 }
	      
	        print_str(1,17,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,monthes[month]);
	     update_screen();
	  }
	  if( current_item == YEAR1)
	  {
         if( ACTION == INPUT_RINNERPLUS  )
		 {
		     if( year_1 == 9 ) year_1=0; else year_1++;
		 }
		 if( ACTION == INPUT_RINNERMINUS )
		 {
		     if( year_1 == 0 || year_1 == -1 ) year_1 = 9; else year_1--;
		 }
		 char str_year_1[2]={0,0};
		 sprintf(str_year_1,"%.1d",year_1);
	      
	        print_str(1,21,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,str_year_1);
	     update_screen();
	  }
	  if( current_item == YEAR2)
	  {
         if( ACTION == INPUT_RINNERPLUS  )
		 {
		     if( year_2 == 9 ) year_2=0; else year_2++;
		 }
		 if( ACTION == INPUT_RINNERMINUS )
		 {
		     if( year_2 == 0 || year_2 == -1 ) year_2 = 9; else year_2--;
		 }
		 char str_year_2[2]={0,0};
		 sprintf(str_year_2,"%.1d",year_2);
	      
	        print_str(1,22,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,str_year_2);
	     update_screen();
	  }
   }
   if(ACTION == -11)
   {
      return(day && (month>=0) && (year_1>=0) && (year_2 >=0));
   }

   if(day && (month>=0) && (year_1>=0) && (year_2 >=0) )
   {
	      
	        print_str(6,14,ATTRIBUTE_FLASH,"ENT");
	     update_screen();
        return(1);
   }
   return(0);
}

int pop_edit_time(int ACTION)
{
   static int hour = -1;
   static int min1 = -1;
   static int min2 = -1;
   
   static int current_item = HOUR;

   static edit_item this_items[3]={
	   {2,12,2,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH},
	   {2,15,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH},
	   {2,16,1,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH},
   };
   if(ACTION == -10)
   {
      hour = -1;
      min1 = -1;
      min2 = -1;
	  current_item = HOUR;

	   
	     print_str(2,12,ATTRIBUTE_INVERSE,"__:__");
		 shadow_buffer[2][12].attribute |= ATTRIBUTE_FLASH;
		 shadow_buffer[2][13].attribute |= ATTRIBUTE_FLASH;
	  update_screen();
	  return(0);
   }

   if(ACTION == INPUT_ROUTERPLUS || ACTION == INPUT_ROUTERMINUS)
   {
      if( ACTION == INPUT_ROUTERPLUS )
	  {
	     if(current_item == MIN2) current_item = HOUR; else current_item++;
	  }

      if( ACTION == INPUT_ROUTERMINUS )
	  {
	     if(current_item == HOUR) current_item = MIN2; else current_item--;
	  }
       
	     for(int col=0;col<5;col++)
			 shadow_buffer[2][12+col].attribute = ATTRIBUTE_INVERSE;

		 for(int i=0;i<this_items[current_item].len;i++)
			 shadow_buffer[this_items[current_item].row][this_items[current_item].col+i].attribute = 
			 this_items[current_item].attribute;
	  update_screen();
	  return(0);
   }
   //***************** Change Data **************************************************************
   if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS)
   {
      if( current_item == HOUR )
	  {
         if( ACTION == INPUT_RINNERPLUS  )
		 {
		     if( hour == 23 ) hour=0; else hour++;
		 }
		 if( ACTION == INPUT_RINNERMINUS )
		 {
		     if( hour == 0 || hour == -1 ) hour = 23; else hour--;
		 }
		 char str_hour[3]={0,0,0};
		 sprintf(str_hour,"%.2d",hour);
	      
	        print_str(2,12,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,str_hour);
	     update_screen();
	  }
	  if( current_item == MIN1 )
	  {
         if( ACTION == INPUT_RINNERPLUS  )
		 {
		     if( min1 == 5 ) min1=0; else min1++;
		 }
		 if( ACTION == INPUT_RINNERMINUS )
		 {
		     if( min1 == 0 || min1 == -1 ) min1 = 5; else min1--;
		 }
		 char str_min1[2]={0,0};
		 sprintf(str_min1,"%.1d",min1);
	      
	        print_str(2,15,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,str_min1);
	     update_screen();
	  }
	  if( current_item == MIN2 )
	  {
         if( ACTION == INPUT_RINNERPLUS  )
		 {
		     if( min2 == 9 ) min2=0; else min2++;
		 }
		 if( ACTION == INPUT_RINNERMINUS )
		 {
		     if( min2 == 0 || min2 == -1 ) min2 = 9; else min2--;
		 }
		 char str_min2[2]={0,0};
		 sprintf(str_min2,"%.1d",min2);
	      
	        print_str(2,16,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,str_min2);
	     update_screen();
	  }
   }

   if(ACTION == -11)
   {
      return((hour>=0) && (min1>=0) && (min2 >=0));
   }

   if( (hour>=0) && (min1>=0) && (min2 >=0) )
   {
	      
	        print_str(6,14,ATTRIBUTE_FLASH,"ENT");
	     update_screen();
        return(1);
   }
   return(0);
}


void do_poweron_page(int ACTION)
{
    static int current_item = 6;
	static int is_editing = 0;
	static int current_TZ = 0;
	int new_item;
	int c_byte;

    //****************** Init Section ***********************************************************************
	if(ACTION == -10)
	{
		if (interface_kohlsman) {
			current_item = 6;
		}
		else {
			current_item = 3;
		}
	   is_editing = 0;
	   return;
	}
	if(ACTION == INPUT_ENTER)
	{
       	switch(current_item)
		{
		case POPMI_APPROVE:
			{
			   INSELFTEST_CDI=0;
				int ret = do_dbexpire(-10);
			   if(ret==-1)
			    GLOBAL_STATE = STATE_DBEXPIRE;
               else
			   {
			    GLOBAL_STATE = ret;
			   }
			   return;
			}
			break;
		case POPMI_DATE:
			if(is_editing)
			{
               if( pop_edit_date(-11) )
			   {
			      is_editing = 0;
				   
			         for(int c_byte=0;c_byte<power_on_menu[current_item].len;c_byte++)
					 {
		                shadow_buffer[power_on_menu[current_item].row][power_on_menu[current_item].col+c_byte].attribute
			            = power_on_menu[current_item].attribute;
					 }
					 print_str(6,14,ATTRIBUTE_NORMAL,"   ");
				  update_screen();
			   }
			}
			break;
		case POPMI_TIME:
			if(is_editing)
			{
               if( pop_edit_time(-11) )
			   {
			      is_editing = 0;
				   
			         for(int c_byte=0;c_byte<power_on_menu[current_item].len;c_byte++)
					 {
		                shadow_buffer[power_on_menu[current_item].row][power_on_menu[current_item].col+c_byte].attribute
			            = power_on_menu[current_item].attribute;
					 }
					 print_str(6,14,ATTRIBUTE_NORMAL,"   ");
				  update_screen();
			   }
			}
			break;
		}
		return;
	}
    if(ACTION == INPUT_RINNERPLUS || ACTION == INPUT_RINNERMINUS)
	{
	    switch(current_item)
		{
		case POPMI_DATE:
			if(!is_editing)
			{
               pop_edit_date(-10);
			   is_editing = 1;
			   return;
			}
			pop_edit_date(ACTION);
			break;
		case POPMI_TIME:
			if(!is_editing)
			{
               pop_edit_time(-10);
			   is_editing = 1;
			   return;
			}
			pop_edit_time(ACTION);
			break;
		case POPMI_TZ:
			//************** Cnage TZ *********************************************
			if(ACTION == INPUT_RINNERPLUS)
			   if(current_TZ == 18) current_TZ=0; else current_TZ++;
			if(ACTION == INPUT_RINNERMINUS)
			   if(current_TZ == 0) current_TZ=18; else current_TZ--;
			 
			  print_str(power_on_menu[current_item].row,power_on_menu[current_item].col,ATTRIBUTE_INVERSE,TZs[current_TZ]);
			update_screen();
			//*********************************************************************
			break;
		case POPMI_BARO1:
			//*************** Baro Main Digit ****************
			{
			int val = (int)(shadow_buffer[4][17].ch-'0')*10+(int)(shadow_buffer[4][18].ch-'0');
			if(ACTION == INPUT_RINNERPLUS)
				if(val == 99 ) val=0; else val++;
			if(ACTION == INPUT_RINNERMINUS)
				if(val == 0 ) val = 99; else val--;
 			char str_baro1[3]={0,0,0};
			sprintf(str_baro1,"%.2d",val);
			 
			  print_str(power_on_menu[current_item].row,power_on_menu[current_item].col,ATTRIBUTE_INVERSE,str_baro1);
			update_screen();
			}
			break;
		case POPMI_BARO2:
			//*************** Baro Digit 1****************
			{
			int val  = (int)(video_buffer[4][20].ch-'0');
			if(ACTION == INPUT_RINNERPLUS)
				if(val == 9 ) val=0; else val++;
			if(ACTION == INPUT_RINNERMINUS)
				if(val == 0 ) val=9; else val--;
 			char str_baro2[2]={0,0};
			sprintf(str_baro2,"%.1d",val);
			 
			  print_str(4,20,ATTRIBUTE_INVERSE,str_baro2);
			update_screen();
			}
			break;
		case POPMI_BARO3:
			//*************** Baro Digit 1****************
			{
			int val  = (int)(shadow_buffer[4][21].ch-'0');
			if(ACTION == INPUT_RINNERPLUS)
				if(val == 9 ) val=0; else val++;
			if(ACTION == INPUT_RINNERMINUS)
				if(val == 0 ) val=9; else val--;
 			char str_baro3[2]={0,0};
			sprintf(str_baro3,"%.1d",val);
			 
			  print_str(4,21,ATTRIBUTE_INVERSE,str_baro3);
			update_screen();
			}
			break;
		}

		return;
	}

	if(ACTION == INPUT_ROUTERPLUS || ACTION == INPUT_ROUTERMINUS)
	{
	     
		 if(is_editing)
		 {
			switch(current_item)
			{
			case POPMI_DATE:
				pop_edit_date(ACTION);
				break;
			case POPMI_TIME:
				pop_edit_time(ACTION);
				break;
			}
			return;
		 }
//*************************** For MENU Navigation only ********************************************************
		 if(ACTION == INPUT_ROUTERPLUS)
		 {
		    if( current_item == (power_on_menu_count - 1) )
		       new_item = 0;
		    else
		       new_item = current_item+1;
		 }
		 else
		 {
		    if( current_item == 0 )
		       new_item = power_on_menu_count-1;
		    else
		       new_item = current_item-1;
		 }

		  
		    for(c_byte=0;c_byte<power_on_menu[current_item].len;c_byte++)
			{
		       shadow_buffer[power_on_menu[current_item].row][power_on_menu[current_item].col+c_byte].attribute
			   = ATTRIBUTE_NORMAL;
			}
            if(power_on_menu[current_item].add_buttons != BUTTON_NONE)
			{
			   print_str(6,14,ATTRIBUTE_NORMAL,"   ");
			}
			for(c_byte=0;c_byte<power_on_menu[new_item].len;c_byte++)
			{
		       shadow_buffer[power_on_menu[new_item].row][power_on_menu[new_item].col+c_byte].attribute
			   = power_on_menu[new_item].attribute;
			}
			if(power_on_menu[new_item].add_buttons == BUTTON_ENTER )
			{
			   print_str(6,14,power_on_menu[new_item].add_button_attribute,"ENT");
			}
		 update_screen();
		 current_item = new_item;
//*************************** For MENU Navigation only END *****************************************************
	}

}
