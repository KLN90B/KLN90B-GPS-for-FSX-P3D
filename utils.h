#ifndef __UTIL_H__
#define __UTIL_H__

#define CALC_K(l) l.k = l.x2==l.x1 ? 7000.0f*1.852f*10e3 : (l.y2-l.y1)/(l.x2-l.x1)
#define CALC_B(l) l.b = (l.y1-l.k*l.x1)

#define TO_RAD(x)   (x)*(3.1415926535897932384626433832795f/180.0f)
#define FROM_RAD(x) (x)*(180.0f/3.1415926535897932384626433832795f)


#endif
