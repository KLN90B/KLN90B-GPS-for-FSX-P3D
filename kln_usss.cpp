#include "PCH.h"
#include "fslib.h"
#include "resource.h"
#include "kln90b.h"
#include "fs_gauge.h"
#include <stdio.h>
#include <math.h>

// Revision Log
// 150113 CEO 0.071B Added VERT_ADJ and HORZ_ADJ to adjust position of text window (solved bottom line chop)
// 150115 CEO 0.071B Fixed DWORD bug in get_magdec(), DTK is now right for negative lat/long
// 150115 CEO 0.071B Changed RUSSIAN to ANSI CHARSET creating font, and standardized on Lucida Console
// 150115 CEO 0.072B Cleaned up all level 1 compiler warnings - several uninitialized vars!
// 150115 CEO 0.072B Made VOR display always "FR" (from) in NAV2 page.
// 150116 CEO 0.072B KLN will control heading when AP is active, set on HDG, and NAV/GPS switch is GPS.
// 150116 CEO 0.072B The config file is now named KLN90B.CNF in the KLN90B folder in the FSX root where the navdata is.
// 150116 CEO 0.072B New option FONT=0|1 where 0 is the default Lucida Console and 1 is Terminus.  Others may be added.
// 150116 CEO 0.072B Pressing ENT when on FPL 0 and not in CRSR mode will load the MSFS flight plan into FPL 0
// 150116 CEO 0.072B AP nav calculation will track to next WP until that course is within 5 degrees of DTK.
// 150117 CEO 0.073B Made airport/navaid names all uppercase to match real KLN
// 150117 CEO 0.073B Fixed the "mangled angle" on NAV4 page.
// 150117 CEO 0.073B Fixed situation where font & pen were created multiple times.
// 150118 CEO 0.073B Now rebuilds database from FSX BGL databases if PTT nav data files are missing.
// 150118 CEO 0.074B Indicated Altitude and Kohlsman setting are now read from the air data interface
// 150119 CEO 0.074B Fixed bug where intersections in the MSFS flight plan weren't copied into FPL0.
// 150119 CEO 0.074B Changed waypoint-sequencing logic to reproduce what the real KLN90B does.
// 150119 CEO 0.074B Changed background image to a higher resolution gray bezel.
// 150119 CEO 0.074B Revised course intercept logic to be more aggressive in getting back on course.
// 150120 CEO 0.075B ILS frequencies added in place of previously unused APT 5.
// 150120 CEO 0.075B Now supports both HRD and SFT runway types.  
// 150120 CEO 0.075B corrected a couple more cases of string overflow on APT pages.
// 150120 CEO 0.075B Added options to be able to control AP HDG mode, ILS freq, and soft runways.
// 150120 CEO 0.075B Implemented the MSG-ENT shortcut to the nearest airport pages.
// 150121 CEO 0.076B Fixed bug where if you let it navigate to destination it would crash.
// 150121 CEO 0.076B Greatly improved on-course driving of the default AP heading.
// 150122 CEO 0.076B Implemented a click spot to switch to AP heading mode.
// 150123 CEO 0.076B Added sounds for the buttons and knobs.
// 150123 CEO 0.076B Disabled loading of STD/STARS but added option SIDSTAR=1 to load anyway.
// 150822 HRG 0.077B Displaying UTC time on init page
// 150823 HRG 0.077B SID/STAR bugs are fixed and feature is enabled	
// 150825 HRG 0.077B ILS frequences are added correctly for airports with add-on scenery.
// 150826 HRG 0.077B Debug file path\name can be defined in the config file [KLN90B] section DEBUGFILE parameter
// 150827 HRG 0.077B "EADT KLN 90B - native" Navdata FMC pack support is added keeping the traditional one as well
// 150827 HRG 0.077B Clears not used FP points from the FP file when saving
// 150829 HRG 0.077B SID/STAR transition support is added to APT+7 pages
// 150830 HRG 0.077B "PSS Airbus/Boeing/Dash" and "vasFMC Flight Management" pack support is added
// 150830 HRG 0.077B Maximum number of FP points in increased from the real life 30 to 50 to support longer SID/STARs (config later?)
// 150831 HRG 0.077B Offers adding the STAR airport to FP if it is not yet included

// TO DO:
// Enable altitude reporting with the ALT key.
// Look for CNF and FPLS files in airplane folder first
// Approaches and SID/STARS - APT 7 & 8.
// Version that uses Direct Input
// Version with screen only that is controled by Lvars
// TRIP pages
// CALC pages
// OTHER pages

#ifdef KLN_FOR_USSS
   #include <dinput.h>
   #include "dilib.h"
#endif

#define		GAUGE_NAME	    	"Kln90B"
#define		GAUGEHDR_VAR_NAME	gaugehdr_kln90b
#define		GAUGE_W			    276

char gaugehdr_kln90b_name[]	= GAUGE_NAME;
extern PELEMENT_HEADER kln90b_list;
extern MOUSERECT kln90b_rect[];
GAUGE_CALLBACK kln90bCallback;

int SoundOk = 0;							// SOUND
TGaugePlaySound GaugePlaySound;
TGaugeStopSound GaugeStopSound;
TTerminateSounds TerminateSounds;
HMODULE MGaugeSound;

#define MAP_LEFT_BORDER 208-11
#define MAP_TOP_BORDER  34-12
#define VERT_ADJ -16						// Adjust this to move the text display vertically
#define HORZ_ADJ +5							// Adjustment to the horizontal text display

HANDLE main_thread;
DWORD kln90b_thread_id;
CRITICAL_SECTION video_buffer_cs;
symbol video_buffer[7][23];
static symbol main_shadow_buffer[7][23];
void toggle_scanpull(void);
void toggle_apon(void);

int EXIT_MAIN_THREAD = 0;

int Current_Active_Leg = 0;			// The currently active flight plan leg. Set only in calc_active_leg() and when the FPL0 is loaded or reset.
int Correct_Sequencing = 1;			// 1 to use KLN90B correct logic.  0 to use the original "closest waypoint in the direction we're pointing is always active".
double Sequencing_Distance = 2.0f;	// How close we need to be to a waypoint to sequence to the next one in NM. 

HANDLE input_sem;
ULONG input_event[1024];
INT   events_count = 0;
CRITICAL_SECTION input_cs;
K_draw_t K_Draw;

extern INT SCREEN_FORMAT;
extern INT GLOBAL_STATE;
extern int INSELFTEST_CDI;
extern DWORD KLN90B_Font;

BOOL kln90b_hidden = FALSE;
long kln90b_win_id;
DWORD interface_kohlsman = 1;
DWORD interface_obsout = 0;
DWORD interface_aphdg = 1;
int force_nr_mode = 0;

int prev_gps = -1;

GAUGE_HEADER_FS700(GAUGE_W, gaugehdr_kln90b_name, &kln90b_list, kln90b_rect, kln90bCallback, 0, 0, 0);

FLOAT64 FSAPI	aponCallback(PELEMENT_ICON pelement)
{
	if (K_is_apon())
		return(1.0f);
	return(0.0f);
}

MAKE_ICON
(
apon_icon,
BMP_KLN90B_APON,
NULL,
NULL,
IMAGE_USE_ERASE /*| IMAGE_USE_TRANSPARENCY*/,
0,
587, 24,
MODULE_VAR_NONE, aponCallback,
ICON_SWITCH_TYPE_STEP_TO,
2,
0,
0
)

FLOAT64 FSAPI	pullscanCallback( PELEMENT_ICON pelement )
{
	if(K_is_scanpull())
		return(1.0f);
	return(0.0f);
}

MAKE_ICON
(		
 pullscan_icon,
 BMP_KLN90B_PULLIN,
 NULL,
 NULL,
 IMAGE_USE_ERASE /*| IMAGE_USE_TRANSPARENCY*/,
 0,
 572-6,140-5,
 MODULE_VAR_NONE,pullscanCallback,
 ICON_SWITCH_TYPE_STEP_TO,
 2,
 0,
 0
 )

 MAKE_STATIC
 (
 kln90b_image,
 BMP_KLN90B_BLANK,
 NULL,
 NULL,
 IMAGE_USE_ERASE|IMAGE_USE_BRIGHT|IMAGE_CREATE_DIBSECTION,
 0,
 MAP_LEFT_BORDER, MAP_TOP_BORDER
 )

 PELEMENT_HEADER     kln90b_next[] = { &kln90b_image.header, &pullscan_icon.header, &apon_icon.header, NULL };


MAKE_STATIC
(
 kln90b_background,
 BMP_KLN90B_BACKGROUND,
 kln90b_next,
 NULL,
 IMAGE_USE_TRANSPARENCY,
 0,
 0,0
 )

 PELEMENT_HEADER		kln90b_list	= &kln90b_background.header;

static  MOUSE_FUNCTION	  PowerOnOff;
static  MOUSE_FUNCTION	  BrightPlus;
static  MOUSE_FUNCTION	  BrightMinus;

static  MOUSE_FUNCTION	  RKnobPlus;
static  MOUSE_FUNCTION	  RKnobMinus;
static  MOUSE_FUNCTION	  LKnobPlus;
static  MOUSE_FUNCTION	  LKnobMinus;
static  MOUSE_FUNCTION	  EnterButton;
static  MOUSE_FUNCTION	  DButton;
static  MOUSE_FUNCTION	  RCursor;
static  MOUSE_FUNCTION	  LCursor;
static  MOUSE_FUNCTION	  PullScan;
static  MOUSE_FUNCTION	  CLR;
static  MOUSE_FUNCTION	  K_MSG;
static  MOUSE_FUNCTION	  AP_Toggle;
static  MOUSE_FUNCTION	  Window_Close;

MOUSE_BEGIN( kln90b_rect, HELP_NONE, 0, 0 )
MOUSE_CHILD_FUNCT(644 - 6, 38 - 7, 24, 24, CURSOR_HAND, MOUSE_LEFTSINGLE, PowerOnOff)
MOUSE_CHILD_FUNCT(619 - 6, 38 - 7, 24, 24, CURSOR_LEFTARROW, MOUSE_LEFTSINGLE, BrightMinus)
MOUSE_CHILD_FUNCT(680 - 6, 38 - 7, 24, 24, CURSOR_RIGHTARROW, MOUSE_LEFTSINGLE, BrightPlus)

MOUSE_CHILD_FUNCT(576 - 6, 148 - 7, 635 - 576, 220 - 148, CURSOR_LEFTARROW, MOUSE_LEFTSINGLE | MOUSE_RIGHTSINGLE | MOUSE_WHEEL_UP | MOUSE_WHEEL_DOWN, RKnobMinus)
MOUSE_CHILD_FUNCT(684 - 6, 148 - 7, 727 - 684, 220 - 148, CURSOR_RIGHTARROW, MOUSE_LEFTSINGLE | MOUSE_RIGHTSINGLE | MOUSE_WHEEL_UP | MOUSE_WHEEL_DOWN, RKnobPlus)

MOUSE_CHILD_FUNCT(10 - 6, 148 - 7, 45, 220 - 148, CURSOR_LEFTARROW, MOUSE_LEFTSINGLE | MOUSE_RIGHTSINGLE | MOUSE_WHEEL_UP | MOUSE_WHEEL_DOWN, LKnobMinus)
MOUSE_CHILD_FUNCT(120 - 6, 148 - 7, 214 - 120, 220 - 148, CURSOR_RIGHTARROW, MOUSE_LEFTSINGLE | MOUSE_RIGHTSINGLE | MOUSE_WHEEL_UP | MOUSE_WHEEL_DOWN, LKnobPlus)

MOUSE_CHILD_FUNCT(514 - 6, 203 - 7, 40, 22, CURSOR_HAND, MOUSE_LEFTSINGLE, EnterButton)
MOUSE_CHILD_FUNCT(354 - 6, 203 - 7, 40, 22, CURSOR_HAND, MOUSE_LEFTSINGLE, DButton)
MOUSE_CHILD_FUNCT(632 - 6, 96 - 7, 40, 22, CURSOR_HAND, MOUSE_LEFTSINGLE, RCursor)

MOUSE_CHILD_FUNCT(646 - 6, 175 - 7, 40, 40, CURSOR_HAND, MOUSE_LEFTSINGLE, PullScan)
MOUSE_CHILD_FUNCT(433 - 6, 203 - 7, 40, 22, CURSOR_HAND, MOUSE_LEFTSINGLE, CLR)
MOUSE_CHILD_FUNCT(74 - 6, 96 - 7, 40, 22, CURSOR_HAND, MOUSE_LEFTSINGLE, LCursor)
MOUSE_CHILD_FUNCT(192 - 6, 203 - 7, 40, 22, CURSOR_HAND, MOUSE_LEFTSINGLE, K_MSG)

MOUSE_CHILD_FUNCT(587, 23, 15, 15, CURSOR_HAND, MOUSE_LEFTSINGLE, AP_Toggle)
MOUSE_CHILD_FUNCT(725, 0, 15, 15, CURSOR_HAND, MOUSE_LEFTSINGLE, Window_Close)

MOUSE_END

DWORD last_redraw = 0;
DWORD last_redraw_bl = 0;


void draw_ndb(HDC hdc,int x,int y)
{
	Arc(hdc,x-K_Draw.X_s/2+1,y-K_Draw.X_s/2+1,x+K_Draw.X_s/2-1,y+K_Draw.X_s/2-1,
		x-K_Draw.X_s/2+1,y-K_Draw.X_s/2+1,x-K_Draw.X_s/2+1,y-K_Draw.X_s/2+1);
}
void draw_vor(HDC hdc,int x,int y)
{
	Arc(hdc,x-K_Draw.X/2+1,y-K_Draw.X/2+1,x+K_Draw.X/2-1,y+K_Draw.X/2-1,
		x-K_Draw.X/2+1,y-K_Draw.X/2+1,x-K_Draw.X/2+1,y-K_Draw.X/2+1);
	Arc(hdc,x-1,y-1,x+1,y+1,x-1,y-1,x-1,y-1);

}
void draw_apt(HDC hdc,int x,int y)
{
	for(int i=K_Draw.X/2-1;i>1;i--)
	{
		MoveToEx(hdc,x-i,y,NULL);
		LineTo(hdc,x,y-i);
		LineTo(hdc,x+i,y);
		LineTo(hdc,x,y+i);
		LineTo(hdc,x-i,y);
	}
	//MoveToEx(hdc,x-1,y,NULL);
	//LineTo(hdc,x+1,y);
}

void print_char(HDC hdc,int X,int Y,char *str,int len,BOOL erase,BOOL inverse)
{
	HPEN pen;
	HBRUSH brush;

	if(inverse)
	{
		pen=K_Draw.h_pen_b; brush = K_Draw.h_brush_b;
	}
	else
	{
		pen=K_Draw.h_pen_t;brush=K_Draw.h_brush_t;
	}

	SelectObject(hdc,pen);
	SelectObject(hdc,brush);
	if(*str=='|')
	{
		if(erase) TextOut(hdc,X,Y," ",1);
		MoveToEx(hdc,X,Y-1,NULL);
		LineTo(hdc,X,Y+K_Draw.Y-1);
	}
	else if(*str=='{')
	{
		if(erase) TextOut(hdc,X,Y," ",1);
		MoveToEx(hdc,X+K_Draw.X,Y+K_Draw.Y/2-1,NULL);
		LineTo(hdc,X,Y+K_Draw.Y/2-1);
		LineTo(hdc,X,Y+K_Draw.Y-1);
	}
	else if(*str == '}' || *str == '>')
	{
		if(erase) TextOut(hdc,X,Y," ",1);
		if(*str == '}')
		{
			MoveToEx(hdc,X,Y-1,NULL);
			LineTo(hdc,X,Y+K_Draw.Y/2-1);
		}
		else
		{
			MoveToEx(hdc,X,Y+K_Draw.Y/2-1,NULL);
		}

		LineTo(hdc,X+K_Draw.X/2-1,Y+K_Draw.Y/2-1);
		BeginPath(hdc);
		LineTo(hdc,X+K_Draw.X/2-1,Y+K_Draw.Y/4-1-1);
		LineTo(hdc,X+K_Draw.X-1,Y+K_Draw.Y/2-1);
		LineTo(hdc,X+K_Draw.X/2-1,Y+K_Draw.Y-K_Draw.Y/4+1-1);
		LineTo(hdc,X+K_Draw.X/2-1,Y+K_Draw.Y/2-1);
		EndPath(hdc);
		SelectObject(hdc, !inverse ? K_Draw.h_brush_t : K_Draw.h_brush_b);
		FillPath(hdc);
	}
	else if(*str == '<')
	{
		if(erase) TextOut(hdc,X,Y," ",1);


		MoveToEx(hdc,X+K_Draw.X-1,Y+K_Draw.Y/2-1,NULL);
		LineTo(hdc,X+K_Draw.X/2,Y+K_Draw.Y/2-1);

		BeginPath(hdc);
		LineTo(hdc,X+K_Draw.X/2,Y+K_Draw.Y/4-1-1);
		LineTo(hdc,X,Y+K_Draw.Y/2-1);
		LineTo(hdc,X+K_Draw.X/2,Y+K_Draw.Y-K_Draw.Y/4+1-1);
		LineTo(hdc,X+K_Draw.X/2,Y+K_Draw.Y/2-1);
		EndPath(hdc);
		SelectObject(hdc,K_Draw.h_brush_t);
		FillPath(hdc);
	}
	else if(*str == DTO_CHAR)
	{
		TextOut(hdc,X,Y,"D ",2);
		MoveToEx(hdc,X,Y+K_Draw.Y/2-1,NULL);
		LineTo(hdc,X+K_Draw.X+1,Y+K_Draw.Y/2-1);
	}
	else if(*str == TO_CHAR)
	{
		TextOut(hdc,X,Y," ",1);
		MoveToEx(hdc,X,Y+K_Draw.Y/2+2,NULL);
		BeginPath(hdc);
		LineTo(hdc,X+K_Draw.X/2,Y+2);
		LineTo(hdc,X+K_Draw.X,Y+K_Draw.Y/2+2);
		LineTo(hdc,X,Y+K_Draw.Y/2+2);
		EndPath(hdc);
		SelectObject(hdc,K_Draw.h_brush_t);
		FillPath(hdc);
	}
	else if(*str == FROM_CHAR)
	{
		TextOut(hdc,X,Y," ",1);
		MoveToEx(hdc,X,Y+K_Draw.Y/2-2,NULL);
		BeginPath(hdc);
		LineTo(hdc,X+K_Draw.X/2,Y+K_Draw.Y-2);
		LineTo(hdc,X+K_Draw.X,Y+K_Draw.Y/2-2);
		LineTo(hdc,X,Y+K_Draw.Y/2-2);
		EndPath(hdc);
		SelectObject(hdc,K_Draw.h_brush_t);
		FillPath(hdc);
	}
	else if(*str == '^')
	{
		TextOut(hdc,X,Y," ",1);
		MoveToEx(hdc,X+K_Draw.X/2-1,Y+K_Draw.Y-1,NULL);
		LineTo(hdc,X+K_Draw.X/2-1,Y+K_Draw.Y/2-1);

		MoveToEx(hdc,X,Y+K_Draw.Y/2,NULL);
		BeginPath(hdc);
		LineTo(hdc,X+K_Draw.X/2-1,Y);
		LineTo(hdc,X+K_Draw.X-1,Y+K_Draw.Y/2);
		LineTo(hdc,X,Y+K_Draw.Y/2);
		EndPath(hdc);
		FillPath(hdc);
	}
	else if(*str == NDB_CHAR)
	{
		TextOut(hdc,X,Y," ",1);
		draw_ndb(hdc,X+K_Draw.X_s/2,Y+K_Draw.X_s/2);
	}  
	else if(*str == VOR_CHAR)
	{
		TextOut(hdc,X,Y," ",1);
		draw_vor(hdc,X+K_Draw.X_s/2,Y+K_Draw.X_s/2);
	}  
	else if(*str == APT_CHAR)
	{
		TextOut(hdc,X,Y," ",1);
		draw_apt(hdc,X+K_Draw.X_s/2,Y+K_Draw.X_s/2);
	}  
	else if(*str == MORE_CHAR)
	{
		TextOut(hdc,X,Y,">",1);
	}  
	else
	{
		if(!erase)
			SetBkMode(hdc,TRANSPARENT);
		TextOut(hdc,X,Y,str,len);
		if(!erase)
			SetBkMode(hdc,OPAQUE);
	}
}

void Out_String(K_string *str,int is_space,int beg=0,int len=23)
{
	for(int col=beg;col<beg+len;col++)
	{
		BOOL erase=TRUE;
		BOOL inverse=FALSE;

		if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_TRANSPARENT)
			erase=FALSE;
		if(col>0)
			if(main_shadow_buffer[str->row][col-1].ch == DTO_CHAR)
				erase=FALSE;     
		if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_INVERSE)
		{
			SetBkColor(str->hdc,str->text_color);
			SetTextColor(str->hdc,str->text_background);
			inverse=TRUE;
		}
		else
		{
			SetTextColor(str->hdc,str->text_color);
			SetBkColor(str->hdc,str->text_background);
		}
		if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_FLASH)
		{
			if(is_space && (main_shadow_buffer[str->row][col].attribute&ATTRIBUTE_INVERSE))
		 {

			 SetTextColor(str->hdc,str->text_color);
			 SetBkColor(str->hdc,str->text_background);
			 if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_SMALL)
			 {
				 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y," ",1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font_s);
				 print_char(str->hdc,(str->XX)/2+col*(K_Draw.X_s),str->Y+K_Draw.Y/2-K_Draw.Y_s/2,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font);
			 }
			 else if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_BSMALL)
			 {
				 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y," ",1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font_s);
				 print_char(str->hdc,(str->XX)/2+col*(str->X)+(K_Draw.X-K_Draw.X_s),str->Y+K_Draw.Y/2-K_Draw.Y_s/2,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font);
			 }
			 else
				 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
		 }
			else if(is_space && !(main_shadow_buffer[str->row][col].attribute&ATTRIBUTE_INVERSE))
		 {
			 SetTextColor(str->hdc,str->text_color);
			 SetBkColor(str->hdc,str->text_background);
			 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y," ",1,erase,inverse);
		 }
			else
		 {
			 if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_SMALL)
			 {
				 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y," ",1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font_s);
				 print_char(str->hdc,(str->XX)/2+col*(K_Draw.X_s),str->Y+K_Draw.Y/2-K_Draw.Y_s/2,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font);
			 }
			 else if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_BSMALL)
			 {
				 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y," ",1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font_s);
				 print_char(str->hdc,(str->XX)/2+col*(str->X)+(K_Draw.X-K_Draw.X_s),str->Y+K_Draw.Y/2-K_Draw.Y_s/2,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
				 SelectObject(str->hdc,K_Draw.h_font);
			 }
			 else
				 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);			  
		 }
		}
		else
		{
			if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_SMALL)
		 {
			 print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y," ",1,erase,inverse);
			 SelectObject(str->hdc,K_Draw.h_font_s);
			 print_char(str->hdc,(str->XX)/2+col*(K_Draw.X_s),str->Y+K_Draw.Y/2-K_Draw.Y_s/2,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
			 SelectObject(str->hdc,K_Draw.h_font);
		 }
			else if(main_shadow_buffer[str->row][col].attribute & ATTRIBUTE_BSMALL)
			{
				print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y," ",1,erase,inverse);
				SelectObject(str->hdc,K_Draw.h_font_s);
				print_char(str->hdc,(str->XX)/2+col*(str->X)+(K_Draw.X-K_Draw.X_s),str->Y+K_Draw.Y/2-K_Draw.Y_s/2,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
				SelectObject(str->hdc,K_Draw.h_font);
			}
			else
				print_char(str->hdc,(str->XX)/2+col*(str->X),str->Y,&main_shadow_buffer[str->row][col].ch,1,erase,inverse);
		}
	}   
}

//=================================================================================================
extern FLOAT64 nav_5_resolution;
extern K_nav_lines_t K_nav_lines;
extern K_nav_box_t K_nav_box;

void draw_rnws(HDC hdc,int start_x,int len_x,int start_y,int len_y,rnw_t *rnws,int count,double resolution)
{
	double all_miles,dots_in_mile_x,dots_in_mile_y;
	SelectObject(hdc,K_Draw.h_pen_t);  

	all_miles      =  resolution;
	dots_in_mile_x =  len_x / resolution;
	dots_in_mile_y =  len_y / resolution;

	for(int i=0;i<count;i++)
	{
		long x1 = rnws[i].apt3_line.x1*dots_in_mile_x;
		long x2 = rnws[i].apt3_line.x2*dots_in_mile_x;
		long y1 = rnws[i].apt3_line.y1*dots_in_mile_y;
		long y2 = rnws[i].apt3_line.y2*dots_in_mile_y;

		x1 = start_x + len_x/2 + x1;
		x2 = start_x + len_x/2 + x2;
		y1 = start_y + len_y/2 - y1;
		y2 = start_y + len_y/2 - y2;

		if(x1 < start_x) x1 = start_x;
		if(x2 < start_x) x2 = start_x;
		if(x1 > start_x+len_x) x1=start_x+len_x;
		if(x2 > start_x+len_x) x2=start_x+len_x;

		if(y1 < start_y) y1 = start_y;
		if(y2 < start_y) y2 = start_y;
		if(y1 > start_y+len_y) y1=start_y+len_y;
		if(y2 > start_y+len_y) y2=start_y+len_y;
		char _str[3];
		MoveToEx(hdc,x1,y1,NULL);
		LineTo  (hdc,x2,y2);
	}

	for(int i=count-1;i>=0;i--)
	{
		long x1 = rnws[i].apt3_line.x1*dots_in_mile_x;
		long x2 = rnws[i].apt3_line.x2*dots_in_mile_x;
		long y1 = rnws[i].apt3_line.y1*dots_in_mile_y;
		long y2 = rnws[i].apt3_line.y2*dots_in_mile_y;

		x1 = start_x + len_x/2 + x1;
		x2 = start_x + len_x/2 + x2;
		y1 = start_y + len_y/2 - y1;
		y2 = start_y + len_y/2 - y2;

		if(x1 < start_x) x1 = start_x;
		if(x2 < start_x) x2 = start_x;
		if(x1 > start_x+len_x) x1=start_x+len_x;
		if(x2 > start_x+len_x) x2=start_x+len_x;

		if(y1 < start_y) y1 = start_y;
		if(y2 < start_y) y2 = start_y;
		if(y1 > start_y+len_y) y1=start_y+len_y;
		if(y2 > start_y+len_y) y2=start_y+len_y;

		SelectObject(hdc,(HFONT)K_Draw.h_font_s);
		TextOut(hdc,x1-K_Draw.X_s/2,y1-K_Draw.Y_s/2,rnws[i].apt3_line.pDes,strlen(rnws->apt3_line.pDes));
		TextOut(hdc,x2-K_Draw.X_s/2,y2-K_Draw.Y_s/2,rnws[i].apt3_line.sDes,strlen(rnws->apt3_line.sDes));
	}  
}

void draw_arrows(HDC hdc,int x1,int y1,int x2,int y2)
{
	double len = sqrt((double)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));

	double p3_x = ((x1-x2) / len);
	double p3_y = ((y1-y2) / len);

	double x1_1 = (p3_x-p3_y)*0.7071;
	double y1_1 = (p3_x+p3_y)*0.7071;

	double x1_2 = (p3_y+p3_x)*0.7071;
	double y1_2 = (p3_y-p3_x)*0.7071;

	double posx = /*p3_x*len*(1/8.0)*/p3_x*K_Draw.X_s+x2;
	double posy = /*p3_y*len*(1/8.0)*/p3_y*K_Draw.Y_s*0.5+y2;

	double x11 = /*x1_1*(0.125f)*len*/x1_1*K_Draw.X_s+posx;
	double y11 = /*y1_1*(0.125f)*len*/y1_1*K_Draw.Y_s*1.3+posy;

	double x12 = /*x1_2*(0.125f)*len*/x1_2*K_Draw.X_s+posx;
	double y12 = /*y1_2*(0.125f)*len*/y1_2*K_Draw.Y_s*1.3+posy;
	POINT old_pl;

	MoveToEx(hdc,posx,posy,&old_pl);
	LineTo(hdc,x11,y11);

	MoveToEx(hdc,posx,posy,NULL);
	LineTo(hdc,x12,y12);

	MoveToEx(hdc,old_pl.x,old_pl.y,NULL);
}

void print_str_lr(HDC hdc,int x,int y,char *str,int left_right,int beg_x)
{
	if(!left_right)
	{
		int pixel_len = strlen(str)*K_Draw.X_s;
		int s_x = x-K_Draw.X_s-pixel_len;
		if(s_x>=beg_x)
		{
			TextOut(hdc,s_x,y-K_Draw.Y_s/2,str,strlen(str));
			return;
		}
	}
	TextOut(hdc,x+K_Draw.X_s,y-K_Draw.Y_s/2,str,strlen(str));
}

void print_str_c(HDC hdc,int x,int y,char *str,int beg_x,int sm)
{
	int pixel_len = strlen(str)*K_Draw.X_s;
	int s_x = x-pixel_len/2;
	if(s_x>=beg_x)
	{
		if(sm)
			TextOut(hdc,s_x,y+K_Draw.Y_s/2+1,str,strlen(str));
		else
			TextOut(hdc,s_x,y+K_Draw.Y/2+1,str,strlen(str)); 
		return;
	}
	if(sm)
		TextOut(hdc,x,y+K_Draw.Y_s/2+1,str,strlen(str));
	else
		TextOut(hdc,x,y+K_Draw.Y/2+1,str,strlen(str));
}

void draw_nav5_page(HDC hdc,int start_x,int len_x,int start_y,int len_y,BOOL s5)
{
	int sam_nach_y,sam_nach_x,len_x_aircr,len_y_aircr;
	double all_miles,dots_in_mile_x,dots_in_mile_y;
	SelectObject(hdc,K_Draw.h_pen_t);  
	if(K_nav_box.curr_nav5_mode != NAV5_MODE_TN)
	{
		sam_nach_y =  len_y * 0.654f;
		sam_nach_x =  len_x * 0.446f;
		len_x_aircr = len_x * 0.09f;
		len_y_aircr = len_y * 0.103f;
		all_miles = 1.3f*nav_5_resolution;
		dots_in_mile_x = len_x / nav_5_resolution;
		dots_in_mile_y = len_y / all_miles;
	}
	else
	{
		len_x_aircr = len_x * 0.09f;
		len_y_aircr = len_y * 0.103f;

		sam_nach_y     =  len_y * 0.5f - len_y_aircr/2.0f;
		sam_nach_x     =  len_x * 0.5f - len_x_aircr/2.0f;
		all_miles      =  nav_5_resolution;
		dots_in_mile_x =  len_x / nav_5_resolution;
		dots_in_mile_y =  len_y / nav_5_resolution;
	}
	//========================================================================================================
	//if(s5)
	//   SelectObject(hdc,(HPEN)K_Draw.h_pen_t_s);
	//else
	SelectObject(hdc,(HPEN)K_Draw.h_pen_t);

	int last_x=-100000,last_y=-100000;
	int left_right=1;

	SetTextColor(hdc,K_Draw.text_color);
	SetBkColor(hdc,K_Draw.text_background);

	for(int i=0;i<K_nav_lines.legs_count;i++)
	{
		if(K_nav_lines.rt_legs[i].numbers_only)
			continue;
		long x1 = K_nav_lines.rt_legs[i].x1*dots_in_mile_x;
		long y1 = K_nav_lines.rt_legs[i].y1*dots_in_mile_y;
		x1 = start_x + len_x/2 + x1;
		y1 = (start_y+sam_nach_y+len_y_aircr/2)-y1;

		if(x1 < start_x) x1 = start_x;
		if(x1 > start_x+len_x) x1=start_x+len_x;

		if(y1 < start_y) y1 = start_y;
		if(y1 > start_y+len_y) y1=start_y+len_y;
		MoveToEx(hdc,x1,y1,NULL);
		break;
	}
	for(int i=0;i<K_nav_lines.legs_count;i++)
	{
		long x2 = K_nav_lines.rt_legs[i].x2*dots_in_mile_x;
		long y2 = K_nav_lines.rt_legs[i].y2*dots_in_mile_y;
		x2 = start_x + len_x/2 + x2;
		y2 = (start_y+sam_nach_y+len_y_aircr/2)-y2;


		if(x2 < start_x) x2 = start_x;
		if(x2 > start_x+len_x) x2=start_x+len_x;


		if(y2 < start_y) y2 = start_y;
		if(y2 > start_y+len_y) y2=start_y+len_y;

		if(K_nav_lines.rt_legs[i].is_dto_line)
		{
			long x1 = K_nav_lines.rt_legs[i].x1*dots_in_mile_x;
			long y1 = K_nav_lines.rt_legs[i].y1*dots_in_mile_y;
			x1 = start_x + len_x/2 + x1;
			y1 = (start_y+sam_nach_y+len_y_aircr/2)-y1;

			if(x1 < start_x) x1 = start_x;
			if(x1 > start_x+len_x) x1=start_x+len_x;

			if(y1 < start_y) y1 = start_y;
			if(y1 > start_y+len_y) y1=start_y+len_y;

			if(K_nav_lines.rt_legs[i].is_dto_line!=3)
		 {
			 MoveToEx(hdc,x1,y1,NULL);
			 LineTo  (hdc,x2,y2);
		 }
		 if(K_nav_lines.rt_legs[i].is_arrow)
		 {
			 draw_arrows(hdc,x1,y1,x2,y2);
		 }
		}
		else
		{
			if(!K_nav_lines.rt_legs[i].numbers_only)
			{
				long x1 = K_nav_lines.rt_legs[i].x1*dots_in_mile_x;
				long y1 = K_nav_lines.rt_legs[i].y1*dots_in_mile_y;
				x1 = start_x + len_x/2 + x1;
				y1 = (start_y+sam_nach_y+len_y_aircr/2)-y1;

				if(x1 < start_x) x1 = start_x;
				if(x1 > start_x+len_x) x1=start_x+len_x;

				if(y1 < start_y) y1 = start_y;
				if(y1 > start_y+len_y) y1=start_y+len_y;
				MoveToEx  (hdc,x1,y1,NULL);
				LineTo  (hdc,x2,y2);
				if(K_nav_lines.rt_legs[i].is_arrow)
				{
					long x1 = K_nav_lines.rt_legs[i].x1*dots_in_mile_x;
					long y1 = K_nav_lines.rt_legs[i].y1*dots_in_mile_y;
					x1 = start_x + len_x/2 + x1;
					y1 = (start_y+sam_nach_y+len_y_aircr/2)-y1;

					if(x1 < start_x) x1 = start_x;
					if(x1 > start_x+len_x) x1=start_x+len_x;

					if(y1 < start_y) y1 = start_y;
					if(y1 > start_y+len_y) y1=start_y+len_y;
					draw_arrows(hdc,x1,y1,x2,y2);
				}
			}
		}
	}
	// ============================================== For Text =================================
	if(K_nav_lines.legs_count)
	{
		long x1 = K_nav_lines.rt_legs[0].x1*dots_in_mile_x;
		long y1 = K_nav_lines.rt_legs[0].y1*dots_in_mile_y;
		x1 = start_x + len_x/2 + x1;
		y1 = (start_y+sam_nach_y+len_y_aircr/2)-y1;

		if(x1 < start_x) x1 = start_x;
		if(x1 > start_x+len_x) x1=start_x+len_x;

		if(y1 < start_y) y1 = start_y;
		if(y1 > start_y+len_y) y1=start_y+len_y;

		char _str[3];
		if(!s5)
		{
			if(K_nav_lines.rt_legs[0].n1)
			{
				_snprintf(_str,3,"%d",K_nav_lines.rt_legs[0].n1);
				_str[2]='\0';
				SelectObject(hdc,(HFONT)K_Draw.h_font_s);
				TextOut(hdc,x1-K_Draw.X_s/2,y1-K_Draw.Y_s/2,_str,strlen(_str)); 
			}
		}
		else
		{
			if(K_nav_lines.rt_legs[0].n1)
			{
				RECT rect; rect.left=x1-K_Draw.X_s/2+1; rect.top=y1-K_Draw.X_s/2+1;
				rect.right = x1+K_Draw.X_s/2-1; rect.bottom = y1+K_Draw.X_s/2-1;
				::FillRect(hdc,&rect,K_Draw.h_brush_t);
				SelectObject(hdc,(HFONT)K_Draw.h_font_s);
				print_str_lr(hdc,x1,y1,K_nav_lines.rt_legs[0].n1_icao,left_right,start_x);
				left_right = 1-left_right;		 
			}
		}	  
	}

	for(int i=0;i<K_nav_lines.legs_count;i++)
	{
		long x2 = K_nav_lines.rt_legs[i].x2*dots_in_mile_x;
		long y2 = K_nav_lines.rt_legs[i].y2*dots_in_mile_y;
		x2 = start_x + len_x/2 + x2;
		y2 = (start_y+sam_nach_y+len_y_aircr/2)-y2;


		if(x2 < start_x) x2 = start_x;
		if(x2 > start_x+len_x) x2=start_x+len_x;


		if(y2 < start_y) y2 = start_y;
		if(y2 > start_y+len_y) y2=start_y+len_y;

		if(K_nav_lines.rt_legs[i].is_dto_line)
		{
			if(K_nav_lines.rt_legs[i].is_arrow)
			{
				if(s5)
				{
					if(K_nav_lines.rt_legs[i].is_dto_line==1)
					{
						RECT rect; rect.left=x2-K_Draw.X_s/2+1; rect.top=y2-K_Draw.X_s/2+1;
						rect.right = x2+K_Draw.X_s/2-1; rect.bottom = y2+K_Draw.X_s/2-1;
						::FillRect(hdc,&rect,K_Draw.h_brush_t);
						SelectObject(hdc,(HFONT)K_Draw.h_font_s);
						print_str_lr(hdc,x2,y2,K_nav_lines.rt_legs[i].n2_icao,left_right,start_x);
						left_right = 1-left_right;		 
					}
				}
				else
				{
					if(K_nav_lines.rt_legs[i].is_dto_line==1)
					{
						SelectObject(hdc,(HFONT)K_Draw.h_font_s);
						TextOut(hdc,x2-K_Draw.X_s/2,y2-K_Draw.Y_s/2,"*",1);
					}
				}
			}
		}
		else
		{
			char _str[3];
			if(!s5)
			{
				if(K_nav_lines.rt_legs[i].n2)
				{
					_snprintf(_str,3,"%d",K_nav_lines.rt_legs[i].n2);
					_str[2]='\0';
					SelectObject(hdc,(HFONT)K_Draw.h_font_s);
					TextOut(hdc,x2-K_Draw.X_s/2,y2-K_Draw.Y_s/2,_str,strlen(_str)); 
				}
			}
			else
			{
				if(K_nav_lines.rt_legs[i].n2)
				{
					RECT rect; rect.left=x2-K_Draw.X_s/2+1; rect.top=y2-K_Draw.X_s/2+1;
					rect.right = x2+K_Draw.X_s/2-1; rect.bottom = y2+K_Draw.X_s/2-1;
					::FillRect(hdc,&rect,K_Draw.h_brush_t);
					SelectObject(hdc,(HFONT)K_Draw.h_font_s);
					print_str_lr(hdc,x2,y2,K_nav_lines.rt_legs[i].n2_icao,left_right,start_x);
					left_right = 1-left_right;		
				}
			}
		}
	} 
	//=============================== Rnws ======================================================
	for(int i=0;i<K_nav_lines.rnws_count;i++)
	{
		long x1 = K_nav_lines.rnws[i].x1*dots_in_mile_x;
		long x2 = K_nav_lines.rnws[i].x2*dots_in_mile_x;
		long y1 = K_nav_lines.rnws[i].y1*dots_in_mile_y;
		long y2 = K_nav_lines.rnws[i].y2*dots_in_mile_y;
		x1 = start_x + len_x/2 + x1;
		x2 = start_x + len_x/2 + x2;
		y1 = (start_y+sam_nach_y+len_y_aircr/2)-y1;
		y2 = (start_y+sam_nach_y+len_y_aircr/2)-y2;

		if(x1 < start_x) x1 = start_x;
		if(x2 < start_x) x2 = start_x;
		if(x1 > start_x+len_x) x1=start_x+len_x;
		if(x2 > start_x+len_x) x2=start_x+len_x;

		if(y1 < start_y) y1 = start_y;
		if(y2 < start_y) y2 = start_y;
		if(y1 > start_y+len_y) y1=start_y+len_y;
		if(y2 > start_y+len_y) y2=start_y+len_y;
		char _str[3];

		MoveToEx(hdc,x1,y1,NULL);
		LineTo  (hdc,x2,y2);

		if(K_nav_lines.rnws[i].numbers_only)
		{
			if(!K_nav_lines.rnws[i].n1)
		 {
			 SelectObject(hdc,(HFONT)K_Draw.h_font_s);
			 TextOut(hdc,x1-K_Draw.X_s/2,y1-K_Draw.Y_s/2,K_nav_lines.rnws[i].pDes,strlen(K_nav_lines.rnws[i].pDes));
		 }
		 if(!K_nav_lines.rnws[i].n2)
		 {
			 SelectObject(hdc,(HFONT)K_Draw.h_font_s);
			 TextOut(hdc,x2-K_Draw.X_s/2,y2-K_Draw.Y_s/2,K_nav_lines.rnws[i].sDes,strlen(K_nav_lines.rnws[i].sDes));
		 }
		}
	}
	//===========================================================================================
	SetBkMode(hdc,TRANSPARENT); 
	for(int i=0;i<K_nav_lines.points_count;i++)
	{
		long x = K_nav_lines.points[i].x*dots_in_mile_x;
		long y = K_nav_lines.points[i].y*dots_in_mile_y;

		x = start_x + len_x/2 + x;
		y = (start_y+sam_nach_y+len_y_aircr/2)-y;

		if(x < start_x) x = start_x;
		if(x > start_x+len_x) x=start_x+len_x;

		if(y < start_y) y = start_y;
		if(y > start_y+len_y) y=start_y+len_y;

		switch(K_nav_lines.points[i].pt_type)
		{
		case NAVAID_NDB:
			{
				SelectObject(hdc,(HFONT)K_Draw.h_font_s);
				SelectObject(hdc,(HPEN)K_Draw.h_pen_t);
				draw_ndb(hdc,x,y);
				print_str_c(hdc,x,y,K_nav_lines.points[i].ICAO_ID,start_x,1);
				break;
			}
		case NAVAID_VOR:
			{
				SelectObject(hdc,(HFONT)K_Draw.h_font_s);
				draw_vor(hdc,x,y);
				print_str_c(hdc,x,y,K_nav_lines.points[i].ICAO_ID,start_x,0); 
				break;
			}
		case NAVAID_APT:
			{
				SelectObject(hdc,(HFONT)K_Draw.h_font_s);
				if(nav_5_resolution>2.0f)
					draw_apt(hdc,x,y);
				print_str_c(hdc,x,y,K_nav_lines.points[i].ICAO_ID,start_x,0);   
				break;
			}
		}
	}
	SetBkMode(hdc,OPAQUE); 
	//*******************************************************************************************
	SelectObject(hdc,(HPEN)K_Draw.h_pen_t);
	if( K_nav_box.curr_nav5_mode == NAV5_MODE_TK || 
		K_nav_box.curr_nav5_mode == NAV5_MODE_HDG
		)
	{
		//============================ Fusel ===================================================
		RECT rect;

		rect.left = start_x+len_x/2-2;//start_x+len_x/2-len_x_aircr/2-2;
		rect.top = start_y+sam_nach_y-1;
		rect.bottom = start_y+sam_nach_y+len_y_aircr+2;
		rect.right = start_x + len_x/2+1;//start_x+len_x/2+len_x_aircr/2+2;
		::FillRect(hdc,&rect,K_Draw.h_brush_b);

		MoveToEx(hdc,start_x + len_x/2,start_y+sam_nach_y,NULL);
		LineTo  (hdc,start_x + len_x/2,start_y+sam_nach_y+len_y_aircr);
		MoveToEx(hdc,start_x+len_x/2-1,start_y+sam_nach_y,NULL);
		LineTo  (hdc,start_x+len_x/2-1,start_y+sam_nach_y+len_y_aircr);
		//============================ Wing ====================================================
		MoveToEx(hdc,start_x+len_x/2-len_x_aircr/2,start_y+sam_nach_y+len_y_aircr/2,NULL);
		LineTo  (hdc,start_x+len_x/2+len_x_aircr/2,start_y+sam_nach_y+len_y_aircr/2);
		MoveToEx(hdc,start_x+len_x/2-len_x_aircr/2,start_y+sam_nach_y+len_y_aircr/2-1,NULL);
		LineTo  (hdc,start_x+len_x/2+len_x_aircr/2,start_y+sam_nach_y+len_y_aircr/2-1);
		if((start_y+sam_nach_y+len_y_aircr/2-2) < (start_y+sam_nach_y))
		{
			MoveToEx(hdc,start_x+len_x/2-len_x_aircr/2,start_y+sam_nach_y+len_y_aircr/2-2,NULL);
			LineTo  (hdc,start_x+len_x/2+len_x_aircr/2,start_y+sam_nach_y+len_y_aircr/2-2);
		}
		//============================ STAB ======================================================
		MoveToEx(hdc,start_x+len_x/2-2,start_y+sam_nach_y+len_y_aircr-1,NULL);
		LineTo(hdc,start_x+len_x/2+2,start_y+sam_nach_y+len_y_aircr-1);
		MoveToEx(hdc,start_x+len_x/2-2,start_y+sam_nach_y+len_y_aircr,NULL);
		LineTo(hdc,start_x+len_x/2+2,start_y+sam_nach_y+len_y_aircr);
		//*******************************************************************************************
	}
	else
	{
		int st_x = start_x + len_x/2;
		int st_y = start_y+sam_nach_y+len_y_aircr/2;

		for(int i=len_x_aircr/2;i>=0;i--)
		{
			MoveToEx(hdc,st_x-i,st_y,NULL);
			LineTo(hdc,st_x,st_y-i);
			LineTo(hdc,st_x+i,st_y);
			LineTo(hdc,st_x,st_y+i);
			LineTo(hdc,st_x-i,st_y);
		}
		MoveToEx(hdc,st_x-1,st_y,NULL);
		LineTo(hdc,st_x+1,st_y);
	}
}

void draw_cdi(HDC hdc,int start_x,int start_y,BOOL is_big)
{

	FLOAT64 xtk,scale;
	BOOL is_cdi = get_CDI(&xtk,&scale);
	xtk=-xtk;

	if(is_big)
	{
		SelectObject(hdc,K_Draw.h_pen_t_s);
		SelectObject(hdc,K_Draw.h_brush_t);

		for(int col=1;col<23;col+=2)
		{
			if(col==11) continue;

			MoveToEx(hdc,start_x+col*K_Draw.X,start_y+K_Draw.Y,NULL);
			LineTo(hdc,start_x+(col+1)*K_Draw.X-1,start_y+K_Draw.Y);
			LineTo(hdc,start_x+col*K_Draw.X,start_y+K_Draw.Y);

			MoveToEx(hdc,start_x+col*K_Draw.X+K_Draw.X/2,start_y+K_Draw.Y-K_Draw.X/2,NULL);
			LineTo(hdc,start_x+col*K_Draw.X+K_Draw.X/2,start_y+K_Draw.Y-K_Draw.X/2+K_Draw.X-1);
			LineTo(hdc,start_x+col*K_Draw.X+K_Draw.X/2,start_y+K_Draw.Y-K_Draw.X/2);

			int x_c = start_x+col*K_Draw.X+K_Draw.X/2;
			int y_c = start_y+K_Draw.Y;

			BeginPath(hdc);
			Ellipse(hdc,x_c-K_Draw.X/2+1,y_c-K_Draw.X/2+1,x_c+K_Draw.X/2-1,y_c+K_Draw.X/2-1);
			EndPath(hdc);
			FillPath(hdc);
		} 
		int x_2 = K_Draw.X/2;
		int y_2 = K_Draw.Y/2;
		int x_c = start_x + 11*K_Draw.X + x_2;
		int y_c = start_y + K_Draw.Y;
		SelectObject(hdc,K_Draw.h_pen_t);
		SelectObject(hdc,K_Draw.h_brush_t);
		if(is_cdi)
		{
			if(get_FT())
			{
				MoveToEx(hdc,x_c-K_Draw.X,y_c+y_2/2,NULL);
				BeginPath(hdc);
				LineTo(hdc,x_c,start_y+y_2/2);
				LineTo(hdc,x_c+K_Draw.X,y_c+y_2/2);
				LineTo(hdc,x_c-K_Draw.X,y_c+y_2/2);
				EndPath(hdc);
			}
			else
			{
				MoveToEx(hdc,x_c-K_Draw.X,y_c-y_2/2,NULL);
				BeginPath(hdc);
				LineTo(hdc,x_c,(start_y+2*K_Draw.Y)-y_2/2);
				LineTo(hdc,x_c+K_Draw.X,y_c-y_2/2);
				LineTo(hdc,x_c-K_Draw.X,y_c-y_2/2);
				EndPath(hdc);
			}
			FillPath(hdc);
			int x_dest = xtk*((5.0/scale)*2.0f*(FLOAT64)K_Draw.X);
			x_dest+=start_x+K_Draw.X*11+x_2;
			if(x_dest<start_x) x_dest=start_x+x_2/2;
			if(x_dest>23*K_Draw.X) x_dest = 23*K_Draw.X-x_2/2;
			RECT rect;
			rect.left = x_dest-x_2/2;
			rect.right = x_dest+x_2/2;
			rect.top = start_y;
			rect.bottom = start_y + 2*K_Draw.Y;
			::FillRect(hdc,&rect,K_Draw.h_brush_t);
		}
		else
		{
			SetTextColor(hdc,K_Draw.text_background);
			SetBkColor(hdc,K_Draw.text_color);
			TextOut(hdc,x_2+K_Draw.X*6,y_c-y_2,"F  L   A  G",strlen("F  L   A  G"));
		}
	}   
	else
	{
		if(is_cdi)
		{
			int x_2 = K_Draw.X/2;
			int x_dest = xtk*((5.0/scale)*1.0f*(FLOAT64)K_Draw.X);
			x_dest+=start_x+K_Draw.X*5+x_2;
			if(x_dest<start_x) x_dest=start_x+x_2/2;
			if(x_dest>(start_x+11*K_Draw.X)) x_dest = (start_x+11*K_Draw.X)-x_2/2;
			RECT rect;
			rect.left = x_dest-x_2/2+1;
			rect.right = x_dest+x_2/2-1;
			rect.top = start_y;
			rect.bottom = start_y + K_Draw.Y;
			::FillRect(hdc,&rect,K_Draw.h_brush_t); 
		}
	}
}

void build_draw(HDC hdc,PIXPOINT *dim)
{
	if(dim->x != K_Draw.x || dim->y != K_Draw.y)
	{
		if(K_Draw.h_font)
			DeleteObject(K_Draw.h_font);
		if(K_Draw.h_font_s)
			DeleteObject(K_Draw.h_font_s);

		K_Draw.X = (long)(double)((double)dim->x / (double)23.0f);
		K_Draw.Y = (long)(double)((double)(dim->y-3) / (double)8);

		K_Draw.X_s = K_Draw.X*0.67+1;
		K_Draw.Y_s = K_Draw.Y*0.72+1;

		LOGFONT lf;
		lf.lfHeight = -K_Draw.Y; 
		lf.lfWidth = -K_Draw.X; 
		lf.lfEscapement = 0; 
		lf.lfOrientation = 0; 
		lf.lfWeight = 250;	// Thickness.  Standard is FW_REGULAR.
		lf.lfItalic = 0; 
		lf.lfUnderline = 0; 
		lf.lfStrikeOut = 0; 
		lf.lfCharSet = ANSI_CHARSET;
		lf.lfOutPrecision=0;//OUT_TT_ONLY_PRECIS;//0; 
		lf.lfClipPrecision=0; 
		lf.lfQuality=0; 
		lf.lfPitchAndFamily=FIXED_PITCH; 
		strcpy(lf.lfFaceName, "Lucida Console");
		if (KLN90B_Font == 1) {
			strcpy(lf.lfFaceName, "Terminus");
			K_Draw.h_font = CreateFont(-K_Draw.Y, -K_Draw.X, 0, 0, 500, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, FIXED_PITCH, "Terminus");
			K_Draw.h_font_s = CreateFont(-K_Draw.Y_s, -K_Draw.X_s, 0, 0, 500, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, FIXED_PITCH, "Terminus");
		} else {
			K_Draw.h_font = CreateFont(-K_Draw.Y, -K_Draw.X, 0, 0, 500, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, FIXED_PITCH, "Lucida Console");
			K_Draw.h_font_s = CreateFont(-K_Draw.Y_s, -K_Draw.X_s, 0, 0, 500, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, FIXED_PITCH, "Lucida Console");
		}
		K_Draw.x = dim->x;
		K_Draw.y = dim->y;
	}
	if(K_Draw.text_color!=main_shadow_buffer[0][0].text_color
		||
		K_Draw.text_background != main_shadow_buffer[0][0].text_background
		)
	{
		K_Draw.text_color        = main_shadow_buffer[0][0].text_color;
		K_Draw.text_background   = main_shadow_buffer[0][0].text_background;
		if(K_Draw.h_pen_b)
			DeleteObject(K_Draw.h_pen_b);
		if(K_Draw.h_pen_t)
			DeleteObject(K_Draw.h_pen_t);
		if(K_Draw.h_pen_t_s)
			DeleteObject(K_Draw.h_pen_t_s);
		if(K_Draw.h_brush_t)
			DeleteObject(K_Draw.h_brush_t);
		if(K_Draw.h_brush_b)
			DeleteObject(K_Draw.h_brush_b);

		K_Draw.h_pen_b   = CreatePen(PS_SOLID,0,K_Draw.text_background);
		K_Draw.h_pen_t   = CreatePen(PS_SOLID,0,K_Draw.text_color);
		K_Draw.h_pen_t_s = CreatePen(PS_SOLID,2,K_Draw.text_color);
		K_Draw.h_brush_t = CreateSolidBrush(K_Draw.text_color);
		K_Draw.h_brush_b = CreateSolidBrush(K_Draw.text_background);
		//K_DEBUG("K_Draw.h_pen_b=[0x%08X] K_Draw.h_pen_t=[0x%08X]\n",K_Draw.h_pen_b,K_Draw.h_pen_t);
	}

}

FLOAT64 ExtGPSLbu;
FLOAT64 ExtGPSLbuDiff;
FLOAT64 ExtGpsPuDelta;
FLOAT64 ExtDefApHDG;
BOOL ExtGpsGot=0;
BOOL UseDefAP;


static int _IsSpace = 0;
void FSAPI kln90bCallback ( PGAUGEHDR pgauge, SINT32 service_id, UINT32 extra_data )
{

	static int is_space = 1;
	int once = 0;
	int built = 0;
	switch (service_id)
	{
	case PANEL_SERVICE_PRE_INITIALIZE:
		{
			if (MGaugeSound == NULL) {					// SOUND
				MGaugeSound = LoadLibrary("KLN90B\\Programs\\GaugeSound");
				if (MGaugeSound == NULL) {
					SoundOk = 0;
				}
				else {
					GaugePlaySound = (TGaugePlaySound)GetProcAddress(MGaugeSound, "GaugePlaySound");
					GaugeStopSound = (TGaugeStopSound)GetProcAddress(MGaugeSound, "GaugeStopSound");
					TerminateSounds = (TTerminateSounds)GetProcAddress(MGaugeSound, "TerminateSounds");
					SoundOk = 1;
				}
			}
			else {
				SoundOk = 1;
			}
			break;
		}
	case PANEL_SERVICE_PRE_KILL:
		{
			unregister_var_by_name("GPS_N1_GOT");
			unregister_var_by_name("GPS_N1_LBU");
			unregister_var_by_name("GPS_N1_LBU_DIF");
			unregister_var_by_name("GPS_N1_PU_DELTA");
		}
		break;
	case PANEL_SERVICE_PRE_INSTALL:
		{
			register_var_by_name(&ExtGPSLbu,TYPE_FLOAT64,"GPS_N1_LBU");
			register_var_by_name(&ExtGPSLbuDiff,TYPE_FLOAT64,"GPS_N1_LBU_DIF");
			register_var_by_name(&ExtGpsGot,TYPE_BOOL,"GPS_N1_GOT");
			register_var_by_name(&ExtGpsPuDelta,TYPE_FLOAT64,"GPS_N1_PU_DELTA");
			kln90b_win_id = pgauge->reserved3 ? *(long *)pgauge->reserved3 : -1;
	}
		break;
	case PANEL_SERVICE_PRE_UPDATE:
		{
			if (!once) {
				K_load_dword_param("KLN90B", "FONT", &KLN90B_Font);
				K_load_dword_param("INTERFACE", "APHDG", &interface_aphdg);
			}
			update_sim_vars();
			if (UseDefAP || (get_ap_active() && get_nav_gps_switch() && get_ap_heading_hold()))
			{
				if (interface_aphdg) {
					trigger_key_event(KEY_HEADING_BUG_SET, K_ftol(ExtDefApHDG));
				}
			}
			if (get_nav_gps_switch() != prev_gps) {
				prev_gps = get_nav_gps_switch();
			}
#ifdef KLN_FOR_USSS
            check_for_buttons();
#endif
		}
		break;
	case PANEL_SERVICE_PRE_DRAW:
		{
			K_print_debug_buffer();
			if( GetTickCount() - last_redraw_bl >= 500 )
			{
				is_space = 1-is_space;
				_IsSpace = is_space;
				last_redraw_bl = GetTickCount();
				last_redraw = GetTickCount();
			}
			else if( GetTickCount() - last_redraw >= 200 )
			{
				last_redraw = GetTickCount();
			}
			else
				break;


			EnterCriticalSection(&video_buffer_cs);
			memcpy(main_shadow_buffer,video_buffer,sizeof(main_shadow_buffer));
			LeaveCriticalSection(&video_buffer_cs);

			PELEMENT_STATIC_IMAGE pelement = (PELEMENT_STATIC_IMAGE)(pgauge->elements_list[0]->next_element[0]);

			if (pelement)
			{
				HDC hdc = pelement->hdc;			
				// 7 Rows 23 Columns
				PIXPOINT dim = pelement->image_data.final->dim;
				int X = (long)(double)((double)dim.x / (double)23.0f);
				int Y = (long)(double)((double)(dim.y-3) / (double)7);
				if (!built) {
					built = true;
					build_draw(hdc, &dim);
				}
				PIXPOINT pos = pelement->position;

				if (hdc)
				{
					RECT client_rect;
					client_rect.left = 0;
					client_rect.right = dim.x;
					client_rect.top = 0;
					client_rect.bottom = dim.y;

					//******************** Set for Font Format ************************************
					if(SCREEN_FORMAT == FORMAT_BLACK)
					{			   
						if(GLOBAL_STATE == STATE_OFF)
						{
							::FillRect(hdc, &client_rect, (HBRUSH) GetStockObject (BLACK_BRUSH));
							SET_OFF_SCREEN (pelement);
							break;
						}
						::FillRect(hdc, &client_rect,K_Draw.h_brush_b);
					}
					else
						::FillRect(hdc, &client_rect,K_Draw.h_brush_b);
					if(!K_Draw.h_font || !K_Draw.h_font_s || !K_Draw.h_pen_b || !K_Draw.h_pen_t ) 
					{
						//K_DEBUG("GDI Bug !!!\n");
						break;
					}
					SelectObject(hdc,(HFONT)K_Draw.h_font);
					TEXTMETRIC text_met;
					GetTextMetrics(hdc,&text_met);
					int YY = dim.y - (text_met.tmHeight * 7) + VERT_ADJ; 
					int XX = dim.x - (text_met.tmMaxCharWidth * 23) + HORZ_ADJ;
					//X = text_met.tmMaxCharWidth;
					int row,col;

					K_string str;
					str.hdc = hdc;
					str.text_background = K_Draw.text_background;
					str.text_color      = K_Draw.text_color;
					str.X  = X;
					str.XX = XX;
					int super_page = get_super_type(); //get_lp_type() == NAV_PAGE && get_rp_type() == NAV_PAGE
					//&& get_navpage_number(PAGE_LEFT)==5 && 
					//get_navpage_number(PAGE_RIGHT)==5;

					if(super_page!=SUPER_NAV5)
					{
						for(row=0;row<7;row++)
						{
							str.row  = row;
							str.Y    = row == 6 && (super_page != SUPER_NAV5) ? YY/2 + Y*row + 3 : YY/2+Y*row;
							Out_String(&str,is_space);		   
						}
					}

					if(super_page != SUPER_NAV5)
					{
						SelectObject(hdc,K_Draw.h_pen_b);
						// Horisontal Lines
						MoveToEx(hdc,XX/2,YY/2 + Y*6,NULL);
						LineTo(hdc,dim.x-XX/2,YY/2 + Y*6);
						MoveToEx(hdc,XX/2,YY/2 + Y*6 + 1,NULL);
						LineTo(hdc,dim.x-XX/2,YY/2 + Y*6 + 1);
						MoveToEx(hdc,XX/2,YY/2 + Y*6 + 2,NULL);
						LineTo(hdc,dim.x-XX/2,YY/2 + Y*6 + 2);
						SelectObject(hdc,K_Draw.h_pen_t);
					}
					SelectObject(hdc,K_Draw.h_pen_t);
					if(super_page == SUPER_NAV5)
					{

						RECT _rect; _rect.left=XX/2+X*8;_rect.right=_rect.left+X*15;
						_rect.top=YY/2;_rect.bottom=_rect.top+Y*7+3;
						::FillRect(hdc,&_rect,K_Draw.h_brush_b);
						draw_nav5_page(hdc,XX/2+X*8,X*15,YY/2,Y*7+3,TRUE);

						_rect.left=XX/2;_rect.right=_rect.left+X*8;
						_rect.top=YY/2;_rect.bottom=_rect.top+Y*7+3;
						::FillRect(hdc,&_rect,K_Draw.h_brush_b);

						MoveToEx(hdc, XX/2+X*7+X/2,YY/2,NULL);
						LineTo(hdc, XX/2+X*7+X/2,dim.y-YY/2);

						SelectObject(hdc,(HFONT)K_Draw.h_font);
						for(row=0;row<7;row++)
						{
							str.row  = row;
							str.Y    = YY/2+Y*row;
							Out_String(&str,is_space,0,7);		   
						}

						SelectObject(hdc,(HFONT)K_Draw.h_font);
						str.row  = 6; str.Y = YY/2+Y*6;
						char dis_str[5];
						K_get_string(6,8,8+3,dis_str);
						K_trim_string(dis_str);
						int attr = K_get_attribute(6,8);
						int str_dis_len=0;
						if(attr & ATTRIBUTE_INVERSE) str_dis_len=4;
						else str_dis_len = strlen(dis_str);

						Out_String(&str,is_space,8,str_dis_len);		   			   

						if(K_nav_lines.is_r_menu)
						{
							RECT _rect; _rect.left=XX/2+X*14+X/2; _rect.right=dim.x - XX/2;
							_rect.top=YY/2;_rect.bottom=YY/2+Y*4+1;
							::FillRect(hdc,&_rect,K_Draw.h_brush_b);
							SelectObject(hdc,(HFONT)K_Draw.h_font);
							for(row=0;row<4;row++)
							{
								str.row  = row;
								str.Y    = YY/2+Y*row;
								Out_String(&str,is_space,15,8);		   
							}
							SelectObject(hdc,K_Draw.h_pen_t);
							MoveToEx(hdc, XX/2+X*14+X/2,YY/2,NULL);
							LineTo(hdc, XX/2+X*14+X/2,YY/2+Y*4+1);
							LineTo(hdc, dim.x - XX/2,YY/2+Y*4+1);      
						}
						if(K_nav_lines.is_dto_list)
						{
							str.row  = 6;
							str.Y    = YY/2+6*Y;
							Out_String(&str,is_space,18,5);		      
						}
						if(K_nav_lines.is_msg)
						{
							str.row  = 5;
							str.Y    = YY/2+5*Y;
							Out_String(&str,is_space,8,3);		      		      
						}

					}
					else if(get_lp_type() == NAV_PAGE && get_navpage_number(PAGE_LEFT)==5)
					{
						draw_nav5_page(hdc,XX/2,X*11,YY/2,Y*6,FALSE);       
					}
					else if(get_rp_type() == NAV_PAGE && get_navpage_number(PAGE_RIGHT)==5)
					{
						if(IsBlocked() == FALSE && lp_kind()!=FRONT_PAGE)
							draw_nav5_page(hdc,XX/2+X*12,X*11,YY/2,Y*6,FALSE);
					}
					if(super_page == SUPER_NAV1)
					{
						draw_cdi(hdc, XX / 2, (YY / 2 + Y * 1), TRUE);
					}
					else if(get_rp_type() == NAV_PAGE && get_navpage_number(PAGE_RIGHT)==1)
					{
						if(IsBlocked() == FALSE && lp_kind()!=FRONT_PAGE)
							draw_cdi(hdc, XX / 2 + X * 12, (YY / 2 + Y * 1), FALSE);
					}
					else if(get_lp_type() == NAV_PAGE && get_navpage_number(PAGE_LEFT)==1)
					{					   
						draw_cdi(hdc, XX / 2, (YY / 2 + Y * 1), FALSE);
					}
					else if(get_rp_type() == APT_PAGE && do_draw_rnws())
					{
						if(IsBlocked() == FALSE && lp_kind()!=FRONT_PAGE)
						{
							nvdb_apt *__apt = get_current_apt();
							if(__apt)
							{
								if(__apt->rws)
									draw_rnws(hdc,XX/2+X*12,X*11,YY/2,Y*6,__apt->rws,
									__apt->rnws_count,__apt->apt3_resolution);
							}
						}
					}
					else if(GLOBAL_STATE == STATE_INSELFTEST_STAGE1 && INSELFTEST_CDI)
					{
						draw_cdi(hdc, XX / 2, (YY / 2 + Y * 1), FALSE);
					}
					if(
						SCREEN_FORMAT == FORMAT_TWOPARTS
						||
						SCREEN_FORMAT == FORMAT_ONEPART
						)
					{

						POINT old_pos;               
						SelectObject(hdc,K_Draw.h_pen_b);
						MoveToEx(hdc,XX/2,YY/2 + Y*6,&old_pos);
						LineTo(hdc,dim.x-XX/2,YY/2 + Y*6);
						MoveToEx(hdc,XX/2,YY/2 + Y*6 + 2,&old_pos);
						LineTo(hdc,dim.x-XX/2,YY/2 + Y*6 + 2);


						SelectObject(hdc,K_Draw.h_pen_t);
						// Horisontal Line
						MoveToEx(hdc,XX/2,YY/2 + Y*6 + 1,&old_pos);
						LineTo(hdc,dim.x-XX/2,YY/2 + Y*6 + 1);
						// Vertical Line Left
						MoveToEx(hdc, XX/2+X*5+X/2 ,YY/2 + Y*6 + 1,&old_pos);
						LineTo(hdc,XX/2+X*5+X/2,dim.y-YY/2);
						// Vertical Line Right
						MoveToEx(hdc, XX/2+X*17+X/2 ,YY/2 + Y*6 + 1,&old_pos);
						LineTo(hdc,XX/2+X*17+X/2,dim.y-YY/2);
						if(SCREEN_FORMAT == FORMAT_TWOPARTS)
						{
							// Vertical Line Center of Screen 
							MoveToEx(hdc, XX/2+X*11+X/2,YY/2,&old_pos);
							LineTo(hdc, XX/2+X*11+X/2,YY/2 + Y*6 + 1);
						}
					}
					//**********************************************************************************
					SET_OFF_SCREEN (pelement);
				}
			}
		}
		break;
	}
}

void do_input_event(long event)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=event;
	LeaveCriticalSection(&input_cs);
	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);
}

static BOOL FSAPI PowerOnOff(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_ONOFF;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}
static BOOL FSAPI BrightPlus(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_BTNMORE;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}
static BOOL FSAPI BrightMinus(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_BTNLESS;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}

static BOOL FSAPI AP_Toggle(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++] = INPUT_APON;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem, 1, &prev_val);

	return(TRUE);
}

static BOOL FSAPI Window_Close(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++] = INPUT_CLOSE;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem, 1, &prev_val);

	return(TRUE);
}

#define WHEEL_DOWN 8192
#define WHEEL_UP   16384


static BOOL FSAPI RKnobPlus(PPIXPOINT pPoint, FLAGS32 flags)
{
	//if(!(flags & 0x80000000 || flags & 0x20000000))  return(TRUE);

	EnterCriticalSection(&input_cs);
	if     (flags & 0x80000000)
		input_event[events_count++]=INPUT_ROUTERPLUS; 
	else if(flags & 0x20000000)
		input_event[events_count++]=INPUT_RINNERPLUS;
	else if(flags & WHEEL_DOWN)
		input_event[events_count++]=INPUT_RINNERMINUS;
	else if(flags & WHEEL_UP)
		input_event[events_count++]=INPUT_RINNERPLUS;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}
static BOOL FSAPI RKnobMinus(PPIXPOINT pPoint, FLAGS32 flags)
{
	//if(!(flags & 0x80000000 || flags & 0x20000000))  return(TRUE);
	EnterCriticalSection(&input_cs);
	if     (flags & 0x80000000)
		input_event[events_count++]=INPUT_ROUTERMINUS; 
	else if(flags & 0x20000000)
		input_event[events_count++]=INPUT_RINNERMINUS;
	else if(flags & WHEEL_DOWN)
		input_event[events_count++]=INPUT_RINNERMINUS;
	else if(flags & WHEEL_UP)
		input_event[events_count++]=INPUT_RINNERPLUS;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}

static BOOL FSAPI EnterButton(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_ENTER;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}
static BOOL FSAPI LKnobPlus(PPIXPOINT pPoint, FLAGS32 flags)
{
	//if(!(flags & 0x80000000 || flags & 0x20000000))  return(TRUE);
	EnterCriticalSection(&input_cs);
	if     (flags & 0x80000000)
		input_event[events_count++]=INPUT_LOUTERPLUS; 
	else if(flags & 0x20000000)
		input_event[events_count++]=INPUT_LINNERPLUS;
	else if(flags & WHEEL_DOWN)
		input_event[events_count++]=INPUT_LINNERMINUS;
	else if(flags & WHEEL_UP)
		input_event[events_count++]=INPUT_LINNERPLUS;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}
static BOOL FSAPI LKnobMinus(PPIXPOINT pPoint, FLAGS32 flags)
{
	//if(!(flags & 0x80000000 || flags & 0x20000000))  return(TRUE);
	EnterCriticalSection(&input_cs);
	if     (flags & 0x80000000)
		input_event[events_count++]=INPUT_LOUTERMINUS; 
	else if(flags & 0x20000000)
		input_event[events_count++]=INPUT_LINNERMINUS;
	else if(flags & WHEEL_DOWN)
		input_event[events_count++]=INPUT_LINNERMINUS;
	else if(flags & WHEEL_UP)
		input_event[events_count++]=INPUT_LINNERPLUS;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}

static BOOL FSAPI DButton(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_DTO;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}

static BOOL FSAPI RCursor(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_RCURSOR;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}
static BOOL FSAPI LCursor(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_LCURSOR;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}

static BOOL FSAPI PullScan(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_PULLSCAN;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);

	return(TRUE);
}

static BOOL FSAPI CLR(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_CLR;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);
	return(TRUE);
}

static BOOL FSAPI K_MSG(PPIXPOINT pPoint, FLAGS32 flags)
{
	EnterCriticalSection(&input_cs);
	input_event[events_count++]=INPUT_MSG;
	LeaveCriticalSection(&input_cs);

	LONG prev_val;
	ReleaseSemaphore(input_sem,1,&prev_val);
	return(TRUE);
}

//================== Tablo gauge =====================================
#undef		GAUGE_NAME
#undef		GAUGEHDR_VAR_NAME
#undef		GAUGE_W

#define		GAUGE_NAME	    	"K9BTABLO"
#define		GAUGEHDR_VAR_NAME	gaugehdr_k9btab
#define		GAUGE_W			    276

char gaugehdr_k9btab_name[]	= GAUGE_NAME;
extern PELEMENT_HEADER k9btab_list;

GAUGE_HEADER_FS700(GAUGE_W, gaugehdr_k9btab_name, &k9btab_list, NULL, NULL, 0, 0, 0);

FLOAT64 FSAPI	k9btabCallback( PELEMENT_ICON pelement )
{
	static DWORD is_space=0;
	static DWORD last_redraw_bl=0;
	if( GetTickCount() - last_redraw_bl >= 500 )
	{
		is_space = 1-is_space;
		last_redraw_bl = GetTickCount();
	}

	if(get_msg_stat()==MSG_KMSG && !is_space) return(1.0f);
	return(0.0f);	
}

MAKE_ICON(k9btab_icon,BMP_TABLO_MSGOFF,NULL,NULL,IMAGE_USE_ERASE|IMAGE_USE_TRANSPARENCY,0,15,11,MODULE_VAR_NONE,k9btabCallback,ICON_SWITCH_TYPE_STEP_TO,2,0,0)

PELEMENT_HEADER     k9btab_next[] = { &k9btab_icon.header, NULL };


MAKE_STATIC(k9btab_background,BMP_TABLO_BACKGROUND,k9btab_next,NULL,IMAGE_USE_TRANSPARENCY,0,0,0)

PELEMENT_HEADER		k9btab_list	= &k9btab_background.header;
//=====================================================================


extern "C" DWORD WINAPI klb90b_main(PVOID argument);
extern int external_request;

void __stdcall FSEventHandler(DWORD event_id,VOID *buffer_specific,PVOID user_buffer,DWORD user_param)
{
	if(event_id == 0xF001)
	{
		//kln90b_hidden = !kln90b_hidden;
		//if(kln90b_hidden)
		//	panel_window_close_ident(kln90b_win_id);
		//else
		//	panel_window_open_ident(kln90b_win_id);
        panel_window_toggle(18000);
	}
	if(event_id == 0xF002)
	{
		set_fpl0_from_af();
	}

	if(event_id == 0xF003)
	{
		toggle_apon();
	}
#ifdef KLN_FOR_USSS
	if(event_id == 0xF004)
	{
	   show_input_dlg();
	}
#endif
}

extern CRITICAL_SECTION cs_afp_fpl0;
void FSAPI	module_init(void)
{
	try {
		SIM.Init();

		init_sim_vars();
		memset(&K_Draw,0,sizeof(K_Draw));
		mem_init();
		for(int y=0;y<7;y++)
			for(int x=0;x<23;x++)
			{
				video_buffer[y][x].ch = ' ';
				video_buffer[y][x].text_background = RGB(11,11,11);
				video_buffer[y][x].text_color      = RGB(0,128,0);
				video_buffer[y][x].attribute       = 0;
			}
			UseDefAP=FALSE;
			InitializeCriticalSection(&video_buffer_cs);
			init_debug();
			input_sem = CreateSemaphore(NULL,0,1024,NULL);
			InitializeCriticalSection(&input_cs);
			InitializeCriticalSection(&cs_afp_fpl0);

			HRESULT hr;

			hr = SIM.MapClientEventToSimEvent(EVENT_MENU_HIDE_SHOW);
			hr = SIM.MapClientEventToSimEvent(EVENT_MENU_AFP_TO_FPL0);
			hr = SIM.MapClientEventToSimEvent(EVENT_MENU_AP_TOGGLE);

			hr = SIM.MenuAddItem("KLN90B", EVENT_MENU_KLN, 123450);

			hr = SIM.MenuAddSubItem(EVENT_MENU_KLN, "Hide/Show", EVENT_MENU_HIDE_SHOW,   123451);
			hr = SIM.MenuAddSubItem(EVENT_MENU_KLN, "AFP->FPL0", EVENT_MENU_AFP_TO_FPL0, 123452);
			hr = SIM.MenuAddSubItem(EVENT_MENU_KLN, "AP Toggle", EVENT_MENU_AP_TOGGLE,   123453);

			hr = SIM.AddClientEventToNotificationGroup(GROUP_MENU, EVENT_MENU_HIDE_SHOW);
			hr = SIM.AddClientEventToNotificationGroup(GROUP_MENU, EVENT_MENU_AFP_TO_FPL0);
			hr = SIM.AddClientEventToNotificationGroup(GROUP_MENU, EVENT_MENU_AP_TOGGLE);

			hr = SIM.SetNotificationGroupPriority(GROUP_MENU, SIMCONNECT_GROUP_PRIORITY_HIGHEST);

			main_thread = CreateThread(NULL,0,klb90b_main,0,0,&kln90b_thread_id);

			/*

			RegiserFSEventHandler(0x8,FSEventHandler,(DWORD)&kln90b_list,0x80050000);
			FSMenuAppend(0,0xF000,(DWORD)"PT Team",0);
			FSMenuAppend(0xF000,0xF001,(DWORD)"KLN-90B Hide/Show",0);
			FSMenuAppend(0xF000,0xF002,(DWORD)"KLN-90B AFP->FPL0",0);
			FSMenuAppend(0xF000,0xF003,(DWORD)"KLN-90B AP Toggle",0);
*/
#ifdef KLN_FOR_USSS
			init_dilib();
			input_config_init();
			FSMenuAppend(0xF000,0xF004,(DWORD)"KLN-90B DI Config",0);      
			AfxWinInit(GetModuleHandle("kln90b.gau"),NULL,GetCommandLine(),0);
#endif

	} catch(swan2::lib::core::Exception& ex) {
		MessageBox(NULL,ex.what(),"Exception occured",0);
		return;
	}
}		

void FSAPI	module_deinit(void)
{
	try {
		UseDefAP=FALSE;
		EXIT_MAIN_THREAD = 1;
		WaitForSingleObject(main_thread,INFINITE);
		DeleteCriticalSection(&video_buffer_cs);
		DeleteCriticalSection(&input_cs);
		DeleteCriticalSection(&cs_afp_fpl0);
		deinit_debug();
		CloseHandle(input_sem);
		mem_deinit();
#ifdef KLN_FOR_USSS
		deinit_dilib();
		input_config_clear();
		FSMenuDelete(0xF004);
#endif
		HRESULT hr;

		hr = SIM.MenuDeleteItem(EVENT_MENU_HIDE_SHOW);
		hr = SIM.RemoveClientEvent(GROUP_MENU, EVENT_MENU_HIDE_SHOW);

		hr = SIM.MenuDeleteItem(EVENT_MENU_AFP_TO_FPL0);
		hr = SIM.RemoveClientEvent(GROUP_MENU, EVENT_MENU_AFP_TO_FPL0);

		hr = SIM.MenuDeleteItem(EVENT_MENU_AP_TOGGLE);
		hr = SIM.RemoveClientEvent(GROUP_MENU, EVENT_MENU_AP_TOGGLE);

		hr = SIM.MenuDeleteItem(EVENT_MENU_KLN);

/*
		FSMenuDelete(0xF003);
		FSMenuDelete(0xF002);
		FSMenuDelete(0xF001);
		FSMenuDelete(0xF000);
		UnRegiserFSEventHandler(0x8,FSEventHandler,(DWORD)&kln90b_list,0x80050000);
*/

		SIM.DeInit();
	} catch(swan2::lib::core::Exception& ex) {
		MessageBox(NULL,ex.what(),"Exception occured",0);
		return;
	}
}	

BOOL WINAPI __DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)	
{														
	return TRUE;										
}

/* This is the module's import table.	*/				
GAUGESIMPORT	ImportTable =							
{														
	{ 0x0000000F, (PPANELS)NULL },
	{ 0x00000000, NULL }								
};						
/* This is the module's export table.	*/				
GAUGESLINKAGE	Linkage =								
{														
	0x00000013,											
		module_init,										
		module_deinit,										
		0,													
		0,
		FS9LINK_VERSION, 
	{
		&gaugehdr_kln90b, 
#ifdef KLN_FOR_USSS
		&gaugehdr_k9btab,
#endif
		0 
	}
};

