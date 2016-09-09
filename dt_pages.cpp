#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include <math.h>

static int dt_page = 1;
static int max_dt_pages = 4;
static int min_dt_pages = 1;

static int DT_INPUT_MASK = INPUT_RINNERPLUS|INPUT_RINNERMINUS;

BOOL dt_handle_key(int INPUT_MASK)
{
	return(INPUT_MASK&DT_INPUT_MASK);
}


void next_dt_page(void)
{
   dt_page++;
   if(dt_page>max_dt_pages) dt_page=min_dt_pages;
}

void prev_dt_page(void)
{
	dt_page--;
	if(dt_page<min_dt_pages) dt_page=max_dt_pages;
}


void print_dt_page(void)
{
   int left_page = get_lp_type();
   int cFplNum;
   int blank_lines = 5;
   fpl_t *cFpl = fpl_GetCurrentFPL(&cFplNum);
   double Lat,Lon;
   get_PPOS(&Lat,&Lon);
   
   if(left_page == FPL_PAGE && dt_page!=4)
   {
       blank_lines=0;
	   for(int i=1;i<6;i++)
	   {
	      int pt_type;
		  nv_point_t *pt = get_RowPointType(i,&pt_type);
		  if(pt_type == DONE_POINT) pt=NULL;

		  if(pt)
		  {
			 if(cFplNum && pt->buffer_spec == cFpl->points[0].buffer_spec)
			 {
                print_str(i,12,ATTRIBUTE_NORMAL,"%-11s"," ");
				continue;
		     }
			 LONG dis;
			 LONG ete_hr;
			 LONG ete_min;

			 if(!cFplNum)
			 {
			    if(pt_type == AFPL_POINT)
				{
					dis = get_S(Lat,Lon,((nv_hdr_t*)pt->buffer_spec)->Lat,((nv_hdr_t*)pt->buffer_spec)->Lon)/1.852f;
				}
				else if(pt_type == WILL_POINT)
					dis = (LONG)pt->dis_from_curr;//(get_S(Lat,Lon,pt->Lat,pt->Lon)/1.852f);
				else
					dis = -1;
				double ete = get_GS(1.852f) > 30 ? (double)dis / get_GS(1.852f) : 0;
				ete_hr = floor(ete);
                if(ete_hr>9999) ete_hr=9999;
				ete_min = (ete - ete_hr) * 60.0f;
				if(dis<0)
					ete_hr=-1;
			 }
			 else
			 {
				dis = (LONG)pt->dis_from_beg;
				ete_hr = -1;
			 }
			 if(dis<1000) 
			 {
			    if(dt_page==3)
				{
				  if(dis>=0)
			         print_str(i,12,ATTRIBUTE_NORMAL,"%3d    %03d%c",dis,(LONG)pt->dtk,0xB0);
				  else
                     print_str(i,12,ATTRIBUTE_NORMAL,"%-11s"," ");
                  continue;
				}
				if(ete_hr>=0)
				{
					if(get_GS(1.852f) < 30.0f)
					{
					   print_str(i,12,ATTRIBUTE_NORMAL,"%3d   --:--",dis);
					}
					else
					{
					   if(dt_page==1)
					   {
					      print_str(i,12,ATTRIBUTE_NORMAL,"%3d%5d:%02d",dis,ete_hr,ete_min);
					   }
					   else if(dt_page==2)
					   {
						  K_dite_time_t __dt; __dt.hour=ete_hr;__dt.min=ete_min;
						  get_NOW_plus_this(&__dt);
						  print_str(i,12,ATTRIBUTE_NORMAL,"%3d%5d:%02d",dis,__dt.hour,__dt.min);
					   }
					}
				}
				else
				{
					if(dis<0)
					{
						//print_str(i,12,ATTRIBUTE_NORMAL,"----  --:--");
						print_str(i,12,ATTRIBUTE_NORMAL,"%-11s"," ");
					}
					else
					   print_str(i,12,ATTRIBUTE_NORMAL,"%3d%-8s",dis," ");
				}
			 } 
			 else
			 {
			    if(dis>9999) dis = 9999;
				if(dt_page==3)
				{
				   print_str(i,12,ATTRIBUTE_NORMAL,"%4d   %03d%c",dis,(LONG)pt->dtk,0xB0);
				   continue;
				}
			    if(ete_hr>=0)
					
					if(get_GS(1.852f) < 30.0f)
					{
						print_str(i,12,ATTRIBUTE_NORMAL,"%4d  --:--",dis);
					}
					else
					{
					  if(dt_page==1)
						  print_str(i,12,ATTRIBUTE_NORMAL,"%4d%4d:%02d",dis,ete_hr,ete_min);
					  else if(dt_page==2)
					  {
						  K_dite_time_t __dt; __dt.hour=ete_hr;__dt.min=ete_min;
						  get_NOW_plus_this(&__dt);
						  print_str(i,12,ATTRIBUTE_NORMAL,"%4d%4d:%02d",dis,__dt.hour,__dt.min);					  
					  }

					}
				else
					print_str(i,12,ATTRIBUTE_NORMAL,"%4d%-7s",dis," ");
		     }
		  }
		  else
		     print_str(i,12,ATTRIBUTE_NORMAL,"%-11s"," ");
	   }
   }
   else
   {
      double ete,ete_hr,ete_min;
      
	  if(nav_mode() == FPL_LEG || nav_mode() == FPLDTO_LEG )
	  {     
          int act_ind,next_ind,last_ind;
		  nv_point_t *ap = fpl_get_active(&act_ind);
		  nv_point_t *np = fpl_get_next(&next_ind);
		  nv_point_t *lp   = fpl_get_last(&last_ind);
		  if(ap==lp) { np=ap; next_ind=last_ind;}
          if(!ap || !lp || !np)
		  {
		     blank_lines=5;
		  }
		  else
		  {
		     LONG l_dis,n_dis;
			 LONG a_dis  = get_S(Lat,Lon,((nv_hdr_t*)ap->buffer_spec)->Lat,((nv_hdr_t*)ap->buffer_spec)->Lon)/1.852f;
             
			 if(ap==lp)	l_dis = a_dis;
			 else l_dis = (LONG)lp->dis_from_curr;
             if(ap==np) n_dis = a_dis;
			 else n_dis = (LONG)np->dis_from_curr;
		     LONG grs = get_GS(1.852f);

		     if(dt_page!=4)
			 {
				print_str(0,12,ATTRIBUTE_NORMAL," >%2d %-6s",act_ind+1,((nv_hdr_t*)ap->buffer_spec)->ICAO_ID);
			    K_change_attribute(0,13,1,get_arrow_attr());
		        print_str(1,12,ATTRIBUTE_NORMAL,"DIS%6dNM",a_dis);
			 }
			 if(dt_page==1 || dt_page==2)
			 {
		        print_str(3,12,ATTRIBUTE_NORMAL,"  %2d %-6s",last_ind+1,((nv_hdr_t*)lp->buffer_spec)->ICAO_ID);
                print_str(4,12,ATTRIBUTE_NORMAL,"DIS%6dNM",l_dis);
			 }
			 else if(dt_page==3)
			 {
				 print_str(3,12,ATTRIBUTE_NORMAL,"  %2d %-6s",next_ind+1,((nv_hdr_t*)np->buffer_spec)->ICAO_ID);
				 print_str(4,12,ATTRIBUTE_NORMAL,"DIS%6dNM",n_dis);			 
			 }
			 if(dt_page==3)
			 {
			    print_str(2,12,ATTRIBUTE_NORMAL,"DTK    %03d%c",(LONG)ap->dtk,0xB0);
				print_str(5,12,ATTRIBUTE_NORMAL,"DTK    %03d%c",(LONG)np->dtk,0xB0);
			 }
			 else if(dt_page==1||dt_page==2||dt_page==4)
			 {
		        if(grs > 30)
		        {
			       double ete = (double)a_dis / grs;
				   double ete_hr = floor(ete);
                   if(ete_hr>9999) ete_hr=9999;
				   double ete_min = (ete - ete_hr) * 60.0f;

				   if(dt_page==1)
				      print_str(2,12,ATTRIBUTE_NORMAL,"ETE%5d:%02d",(LONG)ete_hr,(LONG)ete_min);
				   else if(dt_page==2)
				   {
					  K_dite_time_t __dt; __dt.hour=(LONG)ete_hr;__dt.min=(LONG)ete_min;
					  get_NOW_plus_this(&__dt);			
					  print_str(2,12,ATTRIBUTE_NORMAL,"   %02d:%02dUTC",__dt.hour,__dt.min);
				   }
				   ete = (double)l_dis / grs;
				   ete_hr = floor(ete);
                   if(ete_hr>9999) ete_hr=9999;
				   ete_min = (ete - ete_hr) * 60.0f;

				   if(dt_page==1 || dt_page==4)
				     print_str(5,12,ATTRIBUTE_NORMAL,"ETE%5d:%02d",(LONG)ete_hr,(LONG)ete_min);
				   if(dt_page==2 || dt_page==4)
				   {
				   	  K_dite_time_t __dt; __dt.hour=(LONG)ete_hr;__dt.min=(LONG)ete_min;
					  get_NOW_plus_this(&__dt);
					  if(dt_page==2)
					     print_str(5,12,ATTRIBUTE_NORMAL,"   %02d:%02dUTC",__dt.hour,__dt.min);
					  if(dt_page==4)
					     print_str(3,12,ATTRIBUTE_NORMAL,"ETA   %02d:%02d",__dt.hour,__dt.min);
				   }
		         }
		         else
		        {
			       if(dt_page==1)
				   {
				      print_str(2,12,ATTRIBUTE_NORMAL,"ETE   --:--");
			          print_str(5,12,ATTRIBUTE_NORMAL,"ETE   --:--");
				   }
				   else if(dt_page==2)
				   {
					  print_str(2,12,ATTRIBUTE_NORMAL,"   --:--UTC");
					  print_str(5,12,ATTRIBUTE_NORMAL,"   --:--UTC");				   
				  }
				  else if(dt_page==4)
				  {
					  print_str(3,12,ATTRIBUTE_NORMAL,"ETA   --:--");
					  print_str(5,12,ATTRIBUTE_NORMAL,"ETE   --:--");
				  }
		        }
			    if(dt_page==4)
				{
 				   print_str(0,12,ATTRIBUTE_NORMAL," %c%-5s UTC",ap==lp?'>':0x20,((nv_hdr_t*)lp->buffer_spec)->ICAO_ID);
				   K_change_attribute(0,13,1,get_arrow_attr());
				   K_dite_time_t *_temptm = calc_flying_time(1);
				   if(_temptm)
					  print_str(1,12,ATTRIBUTE_NORMAL,"DEP   %02d:%02d",_temptm->hour,_temptm->min);
				   else
				      print_str(1,12,ATTRIBUTE_NORMAL,"DEP   --:--");
                   _temptm=calc_flying_time(0);
				   if(_temptm)
				      print_str(4,12,ATTRIBUTE_NORMAL,"FLT   %2d:%02d",_temptm->hour,_temptm->min);
				   else
					  print_str(4,12,ATTRIBUTE_NORMAL,"FLT   --:--");
				   
				   K_dite_time_t __curr_time;
				   get_DT(&__curr_time);
				   print_str(2,12,ATTRIBUTE_NORMAL,"TIME  %02d:%02d",__curr_time.hour,__curr_time.min);
                   
				}
			 }
             blank_lines=0;
		  }
	  }
   }

   if(left_page == FPL_PAGE || nav_mode() == DTO_LEG || blank_lines==5)
   {
      if(dt_page==1)
	     print_str(0,12,ATTRIBUTE_NORMAL,"DIS     ETE");
	  else if(dt_page==2)
		 print_str(0,12,ATTRIBUTE_NORMAL,"DIS     UTC");
	  else if(dt_page==3)
		 print_str(0,12,ATTRIBUTE_NORMAL,"DIS     DTK");
	  else if(dt_page==4 && blank_lines==5)
		 print_str(0,12,ATTRIBUTE_NORMAL,"           ");
   }
   for(int i=1;i<blank_lines+1;i++)
      print_str(i,12,ATTRIBUTE_NORMAL,"%-11s"," ");
   print_str(6,18,ATTRIBUTE_NORMAL,"D/T %d",dt_page);
}

void print_dt_page(int page_number)
{
//  switch(page_number)
//  {
//  case 1:
//	  {
	     print_dt_page();
//		 break;
//	  }
//  }
  update_screen();
}

int do_dt_pages(int ACTION)
{
   if(ACTION == ACTION_INIT)
   {
      dt_page=1;
   }
   if(ACTION == ACTION_TIMER)
   {
      print_dt_page(dt_page);
   }
   if(ACTION == INPUT_RINNERMINUS)
      prev_dt_page();
   if(ACTION == INPUT_RINNERPLUS)
	  next_dt_page();
   return(0);
}