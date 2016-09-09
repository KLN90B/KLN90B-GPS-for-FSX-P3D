#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include <stdio.h>


extern INT GLOBAL_STATE;
extern INT SCREEN_FORMAT;

int do_dbexpire(int ACTION)
{
    static char file_name[MAX_PATH];
	if(ACTION == ACTION_INIT)
	{	
	    GetModuleFileName(GetModuleHandle(NULL),file_name,sizeof(file_name)-1);
		int i=0;
	    for(i = strlen(file_name)-1;file_name[i]!='\\';i--);
		file_name[i+1]='\0';
		set_fs_dir(file_name);
		char db_status[24];
		char db_status2[24]="                       ";
		int is_needcreated = 0;
        
		switch(check_db(file_name))
		{
		case DB_ERRCRITICAL:
			sprintf(db_status,"%s","DATABASE CRITICAL ERROR");
			break;
     	case DB_NOTEXIST:
			sprintf(db_status,"%s", "DATABASE DOES NOT EXIST");
			sprintf(db_status2,"%s","   CREATING DATABASE   ");
			is_needcreated = 1;
			break;
     	case DB_EXPIRE:
			sprintf(db_status,"%s","  DATABASE IS EXPIRED  ");
			break;
		case DB_OK:
		    main_loop(ACTION_INIT);
			Load_NAVDB(file_name);
		    return(STATE_MAIN_LOOP);
		}
		
          print_str(0,0,ATTRIBUTE_NORMAL,"                       ");
		  print_str(1,0,ATTRIBUTE_NORMAL,db_status);
		  print_str(2,0,ATTRIBUTE_NORMAL,"       %s       ",get_date(NULL,UTC_TIME));
		  print_str(3,0,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,db_status2);
		  print_str(4,0,ATTRIBUTE_NORMAL,"                       ");
		  print_str(5,0,ATTRIBUTE_NORMAL,"                       ",ATTRIBUTE_NORMAL);
		  print_str(6,0,ATTRIBUTE_NORMAL,"      ENR-LEG     CRSR ",ATTRIBUTE_NORMAL);
		  print_str(6,18,ATTRIBUTE_INVERSE,"CRSR");
		  SCREEN_FORMAT = FORMAT_ONEPART;
          update_screen();

		  if(is_needcreated)
		  {
			  clear_screen();
			  print_str(0,6,ATTRIBUTE_NORMAL,"U P D A T E");
			  print_str(1,3,ATTRIBUTE_NORMAL,"D A T A   B A S E");
              print_str(6,0,ATTRIBUTE_NORMAL,"SET 0"); 
			  update_screen();
			  int ret_val = Create_NAVDB(file_name);
			  print_str(3,0,ATTRIBUTE_NORMAL,"  UPDATE PUBLISHED DB  ");
			  print_str(4,0,ATTRIBUTE_NORMAL,"       COMPLETED       ");
              print_str(6,0,ATTRIBUTE_NORMAL,"     ");
		  }
		  else
		  {
             print_str(1,0,ATTRIBUTE_NORMAL,db_status);
		     print_str(3,0,ATTRIBUTE_NORMAL,db_status2);
		  }
		  print_str(5,5,ATTRIBUTE_INVERSE|ATTRIBUTE_FLASH,"ACKNOWLEDGE?");
		  print_str(6,14,ATTRIBUTE_FLASH|ATTRIBUTE_NORMAL,"ENT");
		  print_str(6,18,ATTRIBUTE_INVERSE,"CRSR");
        update_screen();
	   return(-1);
	}
	if(ACTION == INPUT_ENTER)
	{
	    main_loop(ACTION_INIT);
		GLOBAL_STATE=STATE_MAIN_LOOP;
		Load_NAVDB(file_name);
		return(STATE_MAIN_LOOP);
	}
	return (STATE_MAIN_LOOP);
}
