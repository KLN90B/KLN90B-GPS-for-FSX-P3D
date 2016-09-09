#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "calc_pages.h"
#include "utils.h"
#include <math.h>

static int CALC_INPUT_MASK = INPUT_LCURSOR;
static BOOL cursored = 0;
static int desired_gs = 160;
static int desired_vs = 800;
static int cur_row;
static int cur_col;

BOOL calc_handle_key(int INPUT_MASK)
{
	return(INPUT_MASK&CALC_INPUT_MASK);
}
double calc_ANGLE(double _VS,double _GS)
{
	double dH = 0.00508f*_VS;
	double dS = _GS*1.852f/3.6f;
	double ang = _GS<1.0f ? 90.0f : FROM_RAD(atan(dH/dS));
	return(ang);
}
static void handle_change_value(int ACTION)
{
   if(!cursored) return;
   if(ACTION==INPUT_LINNERPLUS)
   {
      char ch = K_get_char(cur_row,cur_col);
	  ch++;
	  if(ch>'9') ch='0';
	  K_set_char(cur_row,cur_col,ch);
   }
   else if(ACTION==INPUT_LINNERMINUS)
   {
	   char ch = K_get_char(cur_row,cur_col);
	   ch--;
	   if(ch<'0') ch='9';
	   K_set_char(cur_row,cur_col,ch);
   }
   desired_gs = (K_get_char(2,6)-'0')*100+(K_get_char(2,7)-'0')*10+(K_get_char(2,8)-'0');
   desired_vs = (K_get_char(3,7)-'0')*1000+(K_get_char(3,8)-'0')*100+(K_get_char(3,9)-'0')*10+(K_get_char(3,10)-'0');
}
static void handle_change_pos(int ACTION)
{
   if(!cursored) return;
   if(ACTION==INPUT_LOUTERPLUS)
   {
      if(cur_row==2)
	  {
	     if(cur_col==8)
		 {
		    cur_row=3;
			cur_col=7;
		 }
		 else
		    cur_col++;
	  }
	  else if(cur_row==3)
	  {
		  if(cur_col==10)
		  {
			  cur_row=2;
			  cur_col=6;
		  }
		  else
			  cur_col++;
	  }
   }
   else if(ACTION==INPUT_LOUTERMINUS)
   {
	   if(cur_row==2)
	   {
		   if(cur_col==6)
		   {
			   cur_row=3;
			   cur_col=10;
		   }
		   else
			   cur_col--;
	   }
	   else if(cur_row==3)
	   {
		   if(cur_col==7)
		   {
			   cur_row=2;
			   cur_col=8;
		   }
		   else
			   cur_col--;
	   }  
   }
}

int do_calc_page(int ACTION)
{
   if(ACTION==ACTION_TIMER)
   {
      print_str(0,0,ATTRIBUTE_NORMAL," VNV ANGLE ");
	  print_str(1,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	  print_str(2,0,ATTRIBUTE_NORMAL,"%-6s%03dKT","GS:",desired_gs);
	  print_str(3,0,ATTRIBUTE_NORMAL,"%-7s%04d","FPM:",desired_vs);
	  double ang = calc_ANGLE(desired_vs,desired_gs);
	  print_str(4,0,ATTRIBUTE_NORMAL,"%-6s%2d.%d%c","ANGLE:",
		           (int)floor(ang),(int)floor(ang*10.0f)%10,(unsigned char)0xB0);
	  print_str(5,0,ATTRIBUTE_NORMAL,"%-11s"," ");
	  
	  if(cursored)
	  {
	     print_str(6,0,ATTRIBUTE_NORMAL," CRSR");
		 K_change_attribute(cur_row,cur_col,1,ATTRIBUTE_INVERSE);
		 K_change_attribute(6,1,4,ATTRIBUTE_INVERSE);
	  }
	  else
	     print_str(6,0,ATTRIBUTE_NORMAL,"CAL 4");
   }
   if(ACTION==INPUT_LCURSOR)
   {
      if(cursored)
	  {
	     cursored=FALSE;
		 CALC_INPUT_MASK=INPUT_LCURSOR;
	  }
	  else
	  {
	     cursored=TRUE;
         CALC_INPUT_MASK=INPUT_LCURSOR|INPUT_LINNERMINUS|INPUT_LINNERPLUS|INPUT_LOUTERMINUS|INPUT_LOUTERPLUS;
		 cur_row=2;
		 cur_col=6;
	  }
   }
   if(ACTION==INPUT_LINNERMINUS || ACTION==INPUT_LINNERPLUS)
      handle_change_value(ACTION);
   if(ACTION==INPUT_LOUTERMINUS || ACTION==INPUT_LOUTERPLUS)
	   handle_change_pos(ACTION);
   return(0);
}