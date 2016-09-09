#include "fslib.h"

#ifndef __FSGAUGE_H__
#define __FSGAUGE_H__
typedef struct _K_draw_t
{
  int x;
  int y;
  HFONT h_font;
  HFONT h_font_s;
  HPEN  h_pen_b;
  HPEN  h_pen_t;
  HPEN  h_pen_t_s;
  HBRUSH h_brush_t;
  HBRUSH h_brush_b;
  HDC    hdc;
  COLORREF text_color;
  COLORREF text_background;
  int X;
  int Y;
  int X_s;
  int Y_s;
}K_draw_t;

typedef struct _K_string
{
   int row;
   HDC hdc;
   COLORREF text_color;
   COLORREF text_background;
   int XX;
   int X;
   int Y;
}K_string;
__inline FLOAT64 ANGL48_TO_FLOAT64 (const ANGL48 angl48)
{
	return (FLOAT64)(SINT64)(angl48.i64 & ~0xFFFF) * (1/4294967296.0);
}
__inline FLOAT64 ANGL48_TO_DEGREES (const ANGL48 angle)
{
	return ANGL48_TO_FLOAT64(angle) * (360.0 / (65536.0 * 65536.0));
}
__inline ANGL48 FLOAT64_TO_ANGL48 (const FLOAT64 angle)
{
	ANGL48 r;
	r.i64 = (SINT64)(angle * 4294967296.0);
	return r;
}

__inline ANGL48 DEGREES_TO_ANGL48 (const FLOAT64 angle)
{
	return FLOAT64_TO_ANGL48(angle * ((65536.0 * 65536.0) / 360.0));
}

__inline FLOAT64 LAT_METERS48_TO_RADIANS (SIF48 sif48Lat)
{
	return (FLOAT64)(SINT64)(sif48Lat.i64 & ~0xFFFF) * ((1.0 / (40007000.0/(3.14159265358*2))) * (1/4294967296.0));
}

__inline SIF48 FLOAT64_TO_SIF48 (FLOAT64 number)
{
	SIF48 r;
	r.i64 = (SINT64)(number * 4294967296.0);
	return r;
}
__inline SIF48 LAT_RADIANS_TO_METERS48 (const FLOAT64 radians)
{
	return FLOAT64_TO_SIF48(radians / ((1.0 / (40007000.0/(3.14159265358*2)))));
}
__inline FLOAT64 RADIANS_TO_DEGREES (const FLOAT64 radians)
{
	return radians*((double)180.0/ (double)3.14159265358);
}
__inline FLOAT64 DEGREES_TO_RADIANS (const FLOAT64 degrees)
{
	return degrees/((double)180.0/ (double)3.14159265358);
}
__inline FLOAT64 ALT_DEGREES_TO_MET(const FLOAT64 degrees)
{
   return degrees*(40005000.0 / 360);
}
__inline FLOAT64 ALT_MET_TO_DEGREES(const FLOAT64 met)
{
   return met/(40005000.0 / 360);
}
void do_input_event(long event);
void check_for_buttons(void);

#endif