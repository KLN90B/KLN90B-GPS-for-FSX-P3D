#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "vnav.h"
#include <stdio.h>

extern symbol video_buffer[7][23];
extern symbol shadow_buffer[7][23];
extern CRITICAL_SECTION video_buffer_cs;
extern int EXIT_MAIN_THREAD;

extern HANDLE input_sem;
extern ULONG input_event[];
extern INT   events_count;
extern CRITICAL_SECTION input_cs;
int INSELFTEST_CDI;
INT GLOBAL_STATE  = STATE_OFF;
INT SCREEN_FORMAT = FORMAT_BLACK;
DWORD KLN90B_Font = 0;
int ind_alt, prev_ind_alt;
double baro_press, prev_baro_press;
extern DWORD interface_kohlsman;
extern DWORD interface_obsout;
void toggle_apon(void);

void do_timeout(void)
{
   static ULONG called_count = 0;
   static DWORD self_test_timer = 0;

   if(GLOBAL_STATE == STATE_INSELFTEST && !self_test_timer)
   {
       self_test_timer = GetTickCount();
	   return;
   }
   if(GLOBAL_STATE == STATE_INSELFTEST)
   {
       if( (GetTickCount() - self_test_timer) < 5000 )
		   return;
	   INSELFTEST_CDI=1;
       GLOBAL_STATE = STATE_INSELFTEST_STAGE1;
	   self_test_timer = 0;
	   print_str(0,0,ATTRIBUTE_NORMAL,"DIS  34.5NM DATE/TIME  ");
	   print_str(1, 0, ATTRIBUTE_NORMAL, "+++++%c+++++   %s", FROM_CHAR, get_date(NULL, UTC_TIME));
	   K_change_attribute(1,0,11,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL|ATTRIBUTE_TRANSPARENT);
	   K_change_attribute(1,5,1,ATTRIBUTE_NORMAL);
	   
	   if (interface_kohlsman) {
		   baro_press = get_baro_press();
	   }
	   else {
		   baro_press = 29.92;
	   }
	   ind_alt = int(get_indicated_altitude());
	   prev_baro_press = baro_press;
	   print_str(2, 0, ATTRIBUTE_NORMAL, "OBS IN ---%c %sUTC", 0xB0, get_time(NULL, UTC_TIME));
	   print_str(3,0,ATTRIBUTE_NORMAL,"   OUT 315%c ALT %5dFT",0xB0,ind_alt);         
	   print_str(4,0,ATTRIBUTE_NORMAL,"RMI    130%c BARO:%4.2f\"",0xB0,baro_press);
	   print_str(5,0,ATTRIBUTE_NORMAL,"ANNUN    ON   APPROVE? ");
	   print_str(6,0,ATTRIBUTE_NORMAL,"      ENR-LEG     CRSR ");
	   K_change_attribute(6,6,7,ATTRIBUTE_NORMAL|ATTRIBUTE_BSMALL);

	   if (interface_kohlsman) {
		   print_str(5, 14, ATTRIBUTE_INVERSE, "APPROVE?");
		   print_str(6, 14, ATTRIBUTE_FLASH, "ENT");
	   }
	   else {
		   print_str(4, 17, ATTRIBUTE_INVERSE, "29");
	   }
	   print_str(6, 18, ATTRIBUTE_INVERSE, "CRSR");
	   do_poweron_page(ACTION_INIT);
	   GLOBAL_STATE = STATE_INSELFTEST_STAGE1;
	   SCREEN_FORMAT = FORMAT_TWOPARTS;
	   update_screen();
	   return;
   }
   if (GLOBAL_STATE == STATE_INSELFTEST_STAGE1)
   {
		ind_alt = int(get_indicated_altitude());
		if (ind_alt != prev_ind_alt) {
			prev_ind_alt = ind_alt;
			print_str(3, 16, ATTRIBUTE_NORMAL, "%5dFT", ind_alt);
			update_screen();
		}
		if (interface_kohlsman) {
			baro_press = get_baro_press();
			if (baro_press != prev_baro_press) {
				prev_baro_press = baro_press;
				print_str(4, 17, ATTRIBUTE_NORMAL, "%4.2f\"", baro_press);
				update_screen();
			}
		}
	}
	   if (GLOBAL_STATE == STATE_MAIN_LOOP)
   {
	   main_loop(ACTION_TIMER);
       do_status_line(ACTION_TIMER,NULL);
       build_nearest_list();
       calulate_navdata();
       calculate_fpls();
       calc_flying_time(0);
	   dto_update_list();
	   update_vnav();
   }
}

//********************************************************************************************
void do_input(int ACTION)
{
    switch(GLOBAL_STATE)
	{
	case STATE_INSELFTEST_STAGE1:
		do_poweron_page(ACTION);
		break;
	case STATE_DBEXPIRE:
	    do_dbexpire(ACTION);
	    break;
	case STATE_MAIN_LOOP:
		main_loop(ACTION);
		break;
	}
	do_timeout();
}

enum {BRT_PLUS,BRT_MINUS};

static int rgb_green[3];//={0,128,0};
static int rgb_black[3];//={11,11,11};

static COLORREF green_color;// = RGB(0,128,0);
static COLORREF black_color;// = RGB(11,11,11);

void do_brigthness(int ACTION)
{
   if(GLOBAL_STATE == STATE_OFF)
	   return;
   if(ACTION == BRT_PLUS)
   {
      if(rgb_green[1]+10 > 255)
	  {
		 rgb_green[1]=255;
	  }
      else
	  {
         rgb_green[0]++;
	     rgb_green[2]++;
	     rgb_black[0]+=2;
	     rgb_black[1]+=2;
	     rgb_black[2]+=2;
         rgb_green[1]+=10;
	  }
      green_color = RGB(rgb_green[0],rgb_green[1],rgb_green[2]);
      black_color = RGB(rgb_black[0],rgb_black[1],rgb_black[2]);
   }
   else if(ACTION == BRT_MINUS)
   {
      if(rgb_green[1]-10 < 128)
	  {
	     rgb_green[1]=128;
	  }
      else
	  {
         rgb_green[0]--;
	     rgb_green[2]--;
	     rgb_black[0]-=2;
	     rgb_black[1]-=2;
	     rgb_black[2]-=2;
         rgb_green[1]-=10;
	  }
      green_color = RGB(rgb_green[0],rgb_green[1],rgb_green[2]);
      black_color = RGB(rgb_black[0],rgb_black[1],rgb_black[2]);
   }
   shadow_buffer[0][0].text_color = green_color;
   shadow_buffer[0][0].text_background = black_color;
   K_save_dword_param("KLN90B","TC",green_color);
   K_save_dword_param("KLN90B","TB",black_color);
   update_screen();
}

void setup_display(void)
{
   DWORD dw_temp;
   char tempstr[256] = "";

   K_load_dword_param("INTERFACE", "KOHLSMAN", &interface_kohlsman);
   K_load_dword_param("INTERFACE", "OBSOUT", &interface_obsout);
   K_load_dword_param("KLN90B", "FONT", &KLN90B_Font);
   if (K_load_dword_param("KLN90B", "TC", &dw_temp))
   {
      green_color = dw_temp;
	  rgb_green[0]=GetRValue(green_color);rgb_green[1]=GetGValue(green_color);rgb_green[2]=GetBValue(green_color);
   }
   else
   {
      rgb_green[0]=0;rgb_green[1]=128;rgb_green[2]=0;
	  green_color = RGB(0,128,0);
   }
   if(K_load_dword_param("KLN90B","TB",&dw_temp))
   {
      black_color = dw_temp;
	  rgb_black[0]=GetRValue(black_color);rgb_black[1]=GetGValue(black_color);rgb_black[2]=GetBValue(black_color);
   }
   else
   {
      rgb_black[0]=11;rgb_black[1]=11;rgb_black[2]=11;
	  green_color = RGB(0,128,0);
   }
   shadow_buffer[0][0].text_color = green_color;
   shadow_buffer[0][0].text_background = black_color;
}

void do_initalize(void)
{
   do_nav_page(ACTION_INIT,0);
   do_ndb_page(ACTION_INIT);
   do_vor_page(ACTION_INIT);
   do_wpt_page(ACTION_INIT);
   do_sup_page(ACTION_INIT);
   do_status_line(ACTION_INIT,NULL);
   dto_init_list();
   do_set_page(ACTION_INIT);
   do_mod_page(ACTION_INIT);
   do_dt_pages(ACTION_INIT);
   init_navigation();
   setup_display();
}
void do_onoff(void)
{
    static int is_on = 0;
    
	if(FSVersion < 0x00000900)
		return;

	if(!is_on)
	{
	   print_str(0,1,ATTRIBUTE_NORMAL,"GPS             ORS 20");
	   print_str(1,1,ATTRIBUTE_NORMAL,"%c1994 ALLIEDSIGNAL INC",(char)0xA9);
	   print_str(2,0,ATTRIBUTE_NORMAL,"                       ");
	   print_str(3,0,ATTRIBUTE_NORMAL,"                       ");
	   print_str(4,0,ATTRIBUTE_NORMAL,"    S/W Rev 0.077B     ");
	   print_str(5,0,ATTRIBUTE_NORMAL,"      2015-08-30      ");
	   print_str(6,1,ATTRIBUTE_INVERSE,"SELF TEST IN PROGRESS");
	   update_screen();
	   is_on=1;
	   GLOBAL_STATE = STATE_INSELFTEST;
	   do_initalize();
	}
	else
	{
       	
		clear_screen();
		update_screen();
		is_on=0;
		//shadow_buffer[0][0].text_background=RGB(11,11,11);
		//shadow_buffer[0][0].text_color=RGB(0,128,0);
        //rgb_green[0]=0;	rgb_green[1]=128;  rgb_green[2]=0;
	    //rgb_black[0]=11; rgb_black[1]=11; rgb_black[2]=11;
		GLOBAL_STATE = STATE_OFF;
		SCREEN_FORMAT = FORMAT_BLACK;
		UnLoad_NAVDB();
		set_route_leg(NULL,NULL,FPL_LEG);
		set_route_leg(NULL,NULL,DTO_LEG);
		do_dto_page(ACTION_FREE_RESOURCES,0);
	}	
}

extern "C" DWORD WINAPI klb90b_main(PVOID argument)
{
	SIMCONNECT_RECV* pData;
	DWORD cbData;
	HRESULT hr;

	int  key;
	init_console();
    while(1)
    {
		hr = SIM.GetNextDispatch(&pData, &cbData);

		if (SUCCEEDED(hr)) {
			switch(pData->dwID) {
				case SIMCONNECT_RECV_ID_EVENT: {
					SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;

					switch(evt->uEventID) {
						case EVENT_MENU_HIDE_SHOW:
							panel_window_toggle(18000);
						break;

						case EVENT_MENU_AFP_TO_FPL0:
							set_fpl0_from_af();
						break;

						case EVENT_MENU_AP_TOGGLE:
							toggle_apon();
						break;
					}break;
										   
				}
			}
		}

		DWORD wait_val = WaitForSingleObject(input_sem,500);

		if(EXIT_MAIN_THREAD)
		{
			if(GLOBAL_STATE != STATE_OFF)
				do_onoff();
			break;
		}

		if(wait_val == WAIT_TIMEOUT)
		{
			do_timeout();
			continue;
		}
		if (wait_val == WAIT_OBJECT_0)
		{
			key = input_event[0];
			EnterCriticalSection(&input_cs);
			memmove(input_event, &input_event[1], (events_count - 1)*sizeof(ULONG));
			events_count--;
			LeaveCriticalSection(&input_cs);
			if (key == INPUT_ONOFF) {
				do_onoff();
				PlayASound("button.wav", "", 0);			
			}
			else if (key == INPUT_APON) {
				toggle_apon();
			}
			else if (key == INPUT_CLOSE) {
				trigger_key_event(KEY_PANEL_ID_TOGGLE, 18000);
			}
			else if (key == INPUT_BTNMORE) {
				do_brigthness(BRT_PLUS);
			}
			else if (key == INPUT_BTNLESS) {
				do_brigthness(BRT_MINUS);
			}
			else if (key == INPUT_ROUTERPLUS || key == INPUT_ROUTERMINUS ||
				key == INPUT_LOUTERMINUS || key == INPUT_LOUTERPLUS) {
				PlayASound("outerknob.wav", "", 0);
				do_input(key);
			}
			else if (key == INPUT_RINNERPLUS || key == INPUT_RINNERMINUS ||
				key == INPUT_LINNERPLUS || key == INPUT_LINNERMINUS) {
				PlayASound("innerknob.wav", "", 0);
				do_input(key);
			}
			else if (key == INPUT_ENTER || key == INPUT_DTO ||
				key == INPUT_RCURSOR || key == INPUT_PULLSCAN ||
				key == INPUT_CLR || key == INPUT_LCURSOR ||
				key == INPUT_MSG) {
				PlayASound("button.wav", "", 0);
				do_input(key);
			}
			continue;
		}
		break;
		Sleep(500);
	}
	return(0);
}
