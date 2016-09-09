#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include "nav_db.h"
#include <tchar.h>
#include <setupapi.h>
#include <stdio.h>
#include "bglzip.h"

extern char ndbs_file[MAX_PATH];
extern char vors_file[MAX_PATH];
extern char wpts_file[MAX_PATH];
extern char sups_file[MAX_PATH];
extern char apts_file[MAX_PATH];

#define MAGVAR(d) ((double)d>=180.0 ? -((double)d-360.0) : -((double)d))

FILE *ndbs_fp;
FILE *wpts_fp;
FILE *sups_fp;
FILE *vors_fp;
FILE *apts_fp;
FILE *current_bg_file;

LONG total_ndbs;
LONG total_apts;
LONG total_vors;
LONG total_wpts;
LONG total_sups;

char *scenery_cfg = "scenery.cfg";
UCHAR *private_buffer;
DWORD private_buffer_size;
DWORD private_buffer_position;
int is_compressed;

DWORD load_soft = 1;
DWORD load_ils = 1;

unsigned long __stdcall FSGetUncompressedBGLData(char const *,unsigned long *,void * *)
{
   return(0);
}
unsigned long __stdcall FSCompleteBGLOperation(unsigned long,void *)
{
   return(0);
}
//******* Old Bgl Section parsing *********************************************************
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
FILE *K_fopen(char *file_name,char *mode)
{
   
   if(is_bgl_compressed(file_name))
   {
      is_compressed=1;
	  private_buffer_position=0;
	  if(FSGetUncompressedBGLData(file_name,&private_buffer_size,(PVOID *)&private_buffer))
	  {
		  private_buffer=NULL;
		  is_compressed=0;
	  }
   }
   else
   {
		  private_buffer=NULL;
		  is_compressed=0;
   }
   return(fopen(file_name,mode));
}
void K_fclose(FILE *fp)
{
	if( private_buffer && is_compressed )
	{
	   FSCompleteBGLOperation(private_buffer_size,private_buffer);
	   is_compressed=0;
	   private_buffer=NULL;
	   private_buffer_position=0;
	}
	if(fp) fclose(fp);
}
void K_fseek(FILE *fp,long offset,int start_position)
{
    if( private_buffer && is_compressed )
		private_buffer_position=offset;
	fseek(fp,offset,start_position);
}
void K_fread(void *buffer,int size,int unused,FILE *fp)
{
    if( private_buffer && is_compressed )
	{
	   memmove(buffer,private_buffer+private_buffer_position,size);
	}
	else
		fread(buffer,size,1,fp);
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void make_old_ngl_name(char *src_str,char *dst_str,int src_len)
{
   int i;
   for(i=0;i<src_len;i++)
	   dst_str[i]=src_str[i];
   dst_str[i--]='\0';

   while(dst_str[i] == 0x20)
   {
      dst_str[i]='\0';
      if(--i<0) break;
   }
   while(*dst_str==0x20)
	   memmove(dst_str,dst_str+1,strlen(dst_str)+1);
   if(!strlen(dst_str))
   {
      dst_str[0]='0';dst_str[1]='\0';
   }
}

void parse_ilsvor(DWORD chan_number,int type,long offset)
{
    old_ilsvor  iv;   
	old_ILS_GS2 ig;
	old_ILS_GS2 ig2;
	old_dme     dme;

	K_fseek(current_bg_file, offset, SEEK_SET);
	K_fread(&iv,sizeof(old_ilsvor),1,current_bg_file);
    if(iv.type == 0x05)
	{
	   //ILS
	   if(iv.code & 0x40 )
	   {
          K_fread(&ig,sizeof(old_ILS_GS),1,current_bg_file);
	   }
	   /*
	   if(iv.code & 0x1)
	   {
	      fread(&dme,sizeof(dme),1,current_bg_file);
	   }
	   */
	}
	else if(iv.type == 0x06)
	{
	   //ILS 2
	   if(iv.code & 0x40 )
	   {
          K_fread(&ig2,sizeof(old_ILS_GS2),1,current_bg_file);
	   }
	   /*if(iv.code & 0x1)
	   {
	      fread(&dme,sizeof(dme),1,fp);
	   }
	   */
#ifdef _NBD_DEBUG
	   int iii;
	   printf("ILS: Ident: [");
	   for(iii=0;iii<5;iii++)
		   printf("%c",iv.ident[iii]);
	   printf("] Name: [");
   	   for(iii=0;iii<24;iii++)
		   printf("%c",iv.name[iii]);
#endif
	   float Div = 1 << 24;
	   float Lon = (DWORD)((DWORD)iv.lonhi << 16 | (DWORD)iv.lonlo) * 360.0f / Div;
	   float Lat = (DWORD)((DWORD)iv.lathi << 16 | (DWORD)iv.latlo) * 2.0f / lat_meters_degree;
#ifdef _NBD_DEBUG
	   printf("] MagVar: %f Lon: %f Lat: %f\n",iv.magvar*360.f/65536.0f,Lon,Lat);
#endif
	}
    else if(iv.type ==  0x04)
	{
	   // VOR
#ifdef _NBD_DEBUG
	   int iii;
	   printf("VOR%s Ident: [",iv.code & 0x1 ? "/DME":"");
	   for(iii=0;iii<5;iii++)
		   printf("%c",iv.ident[iii]);
	   printf("] Name: [");
   	   for(iii=0;iii<24;iii++)
		   printf("%c",iv.name[iii]);
#endif

   	   float Div = 1 << 24;
	   float Lon = (DWORD)((DWORD)iv.lonhi << 16 | (DWORD)iv.lonlo) * 360.0f / Div;
	   float Lat = (DWORD)((DWORD)iv.lathi << 16 | (DWORD)iv.latlo) * 2.0f / lat_meters_degree;

#ifdef _NBD_DEBUG
	   printf("] MagVar: %f Lon: %f Lat: %f\n",iv.magvar*360.f/65536.0f,Lon,Lat);
#endif
	   int vor_type;
	   if(iv.range < 33 )
		   vor_type=1;
	   else if( iv.range < 55)
		   vor_type=2;
	   else
		   vor_type=3;
	   char icao_id[6];
	   char __name[25];
	   make_old_ngl_name((char *)iv.ident,icao_id,5);
	   make_old_ngl_name((char *)iv.name,__name,24);

	   total_vors++;
	   fprintf(vors_fp,"Lon=[%f] Lat=[%f] FREQ=[%lu] TYPE=[%d] ICAO_ID=[%s] "
		               "REG_ID=[0] APT_ID=[0] NAME=[%s] DME=[%s]\n",
					   Lon,Lat,chan_number,vor_type,icao_id,__name,iv.code&0x1?"YES":"NO");
	   /*if(iv.code & 0x1)
	   {
	      fread(&dme,sizeof(dme),1,fp);
	   }
	   */
	}
}

void parse_channel(DWORD chanel_number,long chan_off,long main_offset)
{
	int i=0;
	while(1)
	{
	   K_fseek(current_bg_file,(main_offset+chan_off)+sizeof(rel_band_set)*i,SEEK_SET);
	   rel_band_set rbs;
	   K_fread(&rbs,sizeof(rel_band_set),1,current_bg_file);
	   if(rbs.op_code == 0)
		   break;
       parse_ilsvor(chanel_number,rbs.op_code,rbs.offset+main_offset);
	   i++;
	}
}
//========================================================================================
void parse_old_bgl(char *file_name)
{
	FILE_HEADER2 bgl_header;
	band_set bs;
	current_bg_file = K_fopen(file_name,"rb");
	K_fread(&bgl_header,sizeof(bgl_header),1,current_bg_file);

	int vor_ptr = bgl_header.FL_vor_ptr; 
    int lo_chan = bgl_header.FL_vor_lo; 
    int hi_chan = bgl_header.FL_vor_hi; 
    int i;
	
    long current_offset = bgl_header.FL_vor_ptr;

	for(i=0;i<hi_chan-lo_chan+1;i++)
	{
		K_fseek(current_bg_file,current_offset,SEEK_SET);    
		K_fread(&bs,sizeof(band_set),1,current_bg_file);
		if(bs.op_code)
		{
			DWORD ch_num = (DWORD)((108.0f+0.05f*(float)(lo_chan+i))*1000000.0f);
#ifdef _NBD_DEBUG
			printf("%f\n",ch_num);
#endif
			parse_channel(ch_num,bs.offset,bgl_header.FL_vor_ptr);
		}
		current_offset+=sizeof(band_set);
	}
	K_fclose(current_bg_file);
}
//=========================================================================================
BOOL is_bgl_compressed(char *file_name)
{
   DWORD buffer_size;
   UCHAR *buffer;

   DWORD ret = FSGetUncompressedBGLData(file_name,&buffer_size,(PVOID *)&buffer);
   if(!ret)
	   FSCompleteBGLOperation(buffer_size,buffer);
   return(ret == 0);
}
//******* Parse Names Section ************************************************************
_names_list_t global_names={NULL,0,NULL,0,NULL,0,NULL,0,NULL,0};

char **build_names(long list_offset,int list_count)
{
	char TEMP_NAME[MAX_PATH]={0};
    char **ret_strs = (char **)malloc(list_count*sizeof(char *));

	DWORD *list = (DWORD *)malloc(list_count*sizeof(DWORD));
	fseek(current_bg_file,list_offset,SEEK_SET);
	fread(list,list_count*sizeof(DWORD),1,current_bg_file);
	for(int i=0;i<list_count;i++)
	{
	   fseek(current_bg_file,list_offset+list[i]+sizeof(DWORD)*list_count,SEEK_SET);
	   char ch='\0';
	   char *temp_buffer = TEMP_NAME;
	   do
	   {
	       fread(&ch,sizeof(ch),1,current_bg_file);
		   *temp_buffer++=ch;
	   }while(ch != 0);
	   ret_strs[i] = (char *)malloc(sizeof(char)*strlen(TEMP_NAME)+1);
	   memcpy(ret_strs[i],TEMP_NAME,strlen(TEMP_NAME)+1);
	}
	free(list);
	return(ret_strs);
}
icao_list *build_icao_list(long list_offset,int list_count)
{
    icao_list *list = (icao_list *)malloc(sizeof(icao_list)*list_count);

	fseek(current_bg_file,list_offset,SEEK_SET);
	fread(list,list_count*sizeof(icao_list),1,current_bg_file);
	return(list);
}
void parse_names_group(int count,int offset,int len)
{
    names_list names;
	fseek(current_bg_file,offset,SEEK_SET);
	fread(&names,sizeof(names),1,current_bg_file);

	global_names.icao_list = build_icao_list(offset+names.icao_offset,names.icao_ids);

	global_names.icao_ids = names.icao_ids;
	global_names.apt_names     = build_names(offset+names.airport_offset,names.airport_names);
	global_names.names_count = names.airport_names;
	global_names.city_names    = build_names(offset+names.city_offset,names.city_names);
	global_names.cities_count = names.city_names;
	global_names.country_names = build_names(offset+names.country_offset,names.country_names);
	global_names.countries_count = names.country_names;

	global_names.region_names = build_names(offset+names.region_offset,names.region_names);
	global_names.regions_count = names.region_names;

}

void parse_names(long offset,long sec_count)
{
    section_header *headers = (section_header *)malloc(sizeof(section_header)*sec_count);
	fseek(current_bg_file,offset,SEEK_SET);
	fread(headers,sizeof(section_header)*sec_count,1,current_bg_file);
	for(int i=0;i<sec_count;i++)
	{
#ifdef _NBD_DEBUG	  
      printf("\tCount: %d FileOffset: %X Size: %X\n",(headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
#endif
	  parse_names_group((headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
	}
	free(headers);
}

#define FREE_GL_NAMES(n,c) if(n) { for(int __i=0;__i<c;__i++){free(n[__i]);}free(n);n=NULL;c=0; }
#define FREE_GL_LIST(n,c) if(n) { free(n);n=NULL;c=0;}
void clean_parse_names(void)
{
   FREE_GL_NAMES(global_names.apt_names,global_names.names_count);
   FREE_GL_NAMES(global_names.city_names,global_names.cities_count);
   FREE_GL_NAMES(global_names.country_names,global_names.countries_count);
   FREE_GL_NAMES(global_names.region_names,global_names.regions_count);
   FREE_GL_LIST(global_names.icao_list,global_names.icao_ids);
}
//******* Prase Airports Section *********************************************************
void parse_apt_group(int count,int offset,int len)
{
	airport *apt;
	char *buffer,*for_free;
	char *savebuffer;
	char surface[16];
	DWORD icd;
	char pils[16], sils[16];
	buffer = (char *)malloc(sizeof(char)*len);
	for_free = buffer;
	fseek(current_bg_file,offset,SEEK_SET);
	fread(buffer,len,1,current_bg_file);
	int rest_of_records = len;
    int apt_number = -1;
	int rnw_n = -1;
	int com_n = -1;

	savebuffer = buffer;

	while(rest_of_records)
	{	   
		switch (*(WORD*)buffer) {
		case 0x003c:
			// airport
		{
			savebuffer = buffer;
			apt_number++;
			airport *apt = (airport *)buffer;
			double Lon = (DWORD)apt->longitude*(360.0 / (3 * 0x10000000)) - 180.0;
			double Lat = 90.0 - (DWORD)apt->latitude * (180.0 / (2 * 0x10000000));

			char _apt_id[0x49] = { 0 };
			convert_id(apt->icao_id, _apt_id);

			int __icao_ind = 0;
			for (__icao_ind = 0; __icao_ind < global_names.icao_ids; __icao_ind++) {
				if (apt->icao_id == global_names.icao_list[__icao_ind].icao_id) {
					break;
				}
			}
			total_apts++;

			// Preprocesses airport subrecords to determine the number of runways and com equipments
			rnw_n = com_n = 0;
			int remaining_record_length = ((sub_common *)buffer)->size_of_record - sizeof(airport);
			char *p = buffer + sizeof(airport);
			while (remaining_record_length > 0)
			{
				switch ( ((sub_common*) p)->id )
				{
				case 0x0004:	// runway
					rnw_n++;
					break;
				case 0x0012:	// COM
					com_n++;
					break;
				}
				remaining_record_length -= ((sub_common *)p)->size_of_record;
				p += ((sub_common *)p)->size_of_record;
			}

			// Saves basic airport data including number of runways and COMs subrecords that will be saved later
			fprintf(apts_fp, "ICAO_ID=[%s] Lon=[%f] Lat=[%f] Alt=[%f] NAME=[%s] CITY=[%s] COUNTRY=[%s] MV=[%.2f] RNWS=[%d] COMS=[%d]\n",
				_apt_id, 
				Lon, Lat,
				((FLOAT64)apt->altitude) / 1000.0f, 
				global_names.apt_names[global_names.icao_list[__icao_ind].apt_i],
				global_names.city_names[global_names.icao_list[__icao_ind].city_i],
				global_names.country_names[global_names.icao_list[__icao_ind].coun_i], 
				MAGVAR(apt->mag_var), rnw_n, com_n);

			// Process all this airport's subrecords
			rnw_n = com_n = 0;
			int rest_of_subrecords = ((sub_common *)buffer)->size_of_record;
			rest_of_subrecords -= sizeof(airport);
			buffer += sizeof(airport);
			while (rest_of_subrecords) {
				switch (*(WORD*)buffer) {
					case 0x0019: {		// NAME
						name_sub *ns = (name_sub *)buffer;
					}
					break;
					case 0x0004: {		// RUNWAY
						rnw_sub *rws = (rnw_sub *)buffer;
						strcpy(surface, "SFT");
						BOOL is_hrd = rws->surface_type == 0x000 ||			
							rws->surface_type == 0x0004 ||
							rws->surface_type == 0x0017 ||
							rws->surface_type == 0x0012 ||
							rws->surface_type == 0x0011;
						if (is_hrd) strcpy(surface, "HRD");
						if (is_hrd || load_soft) {
							double RLon = (DWORD)rws->longitude*(360.0 / (3 * 0x10000000)) - 180.0;
							double RLat = 90.0 - (DWORD)rws->latitude * (180.0 / (2 * 0x10000000));
							if (strcmp(_apt_id, "KAVL") == 0) {			// HG: What is this ??????
								strcpy(pils, "ABC");
							}
							icd = rws->icao_id_pri_ils << 5;
							convert_id(icd, pils);
							icd = rws->icao_id_sec_ils << 5;
							convert_id(icd, sils);
							fprintf(apts_fp, "RNW%d=[SRF={%s} rLat={%f} rLon={%f} PN={%d} PD={%d}"
								" SN={%d} SD={%d} LEN={%f} WID={%f} HDG={%f} PF={%d} LF={%d} PI={%s} SI={%s}] \n",
								++rnw_n, surface, RLat, RLon, rws->rnw_number, rws->rnw_designat,
								rws->sec_rnw_number, rws->sec_rnw_designat, rws->lenght,
								rws->width, rws->heading, (int)rws->pattern_flags, (int)rws->light_flags,
								pils, sils);
						}
					}
					break;
					case 0x0012: {			// COM 
						sub_com *sc = (sub_com *)buffer;
						fprintf(apts_fp, "COM%d=[TYPE={%d} FREQ={%lu}] \n", ++com_n,
							sc->type, sc->freq);
					}
					break;
					case 0x0013: {			// ILS
						ils_vor *ils = (ils_vor *)buffer;
					}
					break;
					case 0x0024: {			// Approach
						sub_approach *sa = (sub_approach *)buffer;
					}
					 break;
					case 0x0022: {			//Way Point
						way_point *wpt = (way_point *)buffer;
					}
					break;
					} // switch
					rest_of_subrecords -= ((sub_common *)buffer)->size_of_record;
					buffer += ((sub_common *)buffer)->size_of_record;
			} // while

//			if (rnw_n > -1 && com_n > -1) {
//				fprintf(apts_fp, "RNWS=[%d] COMS=[%d]\n", rnw_n, com_n);
//			}
		} // case 0x003c
		break;
		} // switch
		buffer = savebuffer;
		rest_of_records-=((sub_common *)buffer)->size_of_record;
		buffer += ((sub_common *)buffer)->size_of_record;
	} // while
	free(for_free);
} // function

void parse_airports(long offset,long apt_count)
{
	K_load_dword_param("KLN90B", "RUNWAYS", &load_soft);
	section_header *headers = (section_header *)malloc(sizeof(section_header)*apt_count);
	fseek(current_bg_file,offset,SEEK_SET);
	fread(headers,sizeof(section_header)*apt_count,1,current_bg_file);
	for(int i=0;i<apt_count;i++)
	{
	       parse_apt_group((headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
	}
    free(headers);	
}
//******* WayPoinys Parse Section ********************************************************
void parse_wpt_group(int count,int offset,int len)
{
    way_point *wpt_ptr;
	char *buffer,*for_free;

	fseek(current_bg_file,offset,SEEK_SET);
	buffer = (char *)malloc(sizeof(char)*len);
	for_free = buffer;
	fread(buffer,len,1,current_bg_file);
	int rest_of_records = len;

	while(rest_of_records)
	{
	   wpt_ptr = (way_point *)buffer;
       char wpt_id[0x49];
	   convert_id(wpt_ptr->icao_id,wpt_id);

	   double Lon = (DWORD) wpt_ptr->logitude*(360.0 /(3*0x10000000))-180.0;
	   double Lat = 90.0 - (DWORD) wpt_ptr->latitude * (180.0 / (2 * 0x10000000));
       if( wpt_id[0]!='0' && wpt_id[1]!='\0' ) 
	   {
	      if(wpt_ptr->type != 2)
		  {
		     total_wpts++;	   
             fprintf(wpts_fp,"Lon=[%f] Lat=[%f] ICAO_ID=[%s] ",Lon,Lat,wpt_id);

	         convert_id( ( wpt_ptr->reg_icaoid &  0x7FF ) << 5,wpt_id);
             fprintf(wpts_fp,"REG_ID=[%s] ",wpt_id);
	         convert_id( ( wpt_ptr->reg_icaoid >> 11    ) << 5,wpt_id);
	         fprintf(wpts_fp,"APT_ID=[%s] TYPE=[%d] ROUTES=[%d] SUB_TYPE=[WPT]\n",wpt_id,wpt_ptr->type,wpt_ptr->routes_count);
		  }
		  else
		  {
		     total_sups++;	   
             fprintf(sups_fp,"Lon=[%f] Lat=[%f] ICAO_ID=[%s] ",Lon,Lat,wpt_id);

	         convert_id( ( wpt_ptr->reg_icaoid &  0x7FF ) << 5,wpt_id);
             fprintf(sups_fp,"REG_ID=[%s] ",wpt_id);
	         convert_id( ( wpt_ptr->reg_icaoid >> 11    ) << 5,wpt_id);
	         fprintf(sups_fp,"APT_ID=[%s] TYPE=[%d] ROUTES=[%d]\n",wpt_id,wpt_ptr->type,wpt_ptr->routes_count);  
		  }
	   }
	   rest_of_records-=wpt_ptr->header.size_of_record;
	   buffer+=wpt_ptr->header.size_of_record;
	}
	free(for_free);
}
void parse_wpts(long offset,long sec_count)
{
    section_header *headers = (section_header *)malloc(sizeof(section_header)*sec_count);
	fseek(current_bg_file,offset,SEEK_SET);
	fread(headers,sizeof(section_header)*sec_count,1,current_bg_file);
	for(int i=0;i<sec_count;i++)
	{
#ifdef _NBD_DEBUG
	  printf("\tCount: %d FileOffset: %X Size: %X\n",(headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
#endif
	  parse_wpt_group((headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
	}
	free(headers);
	
}
//******* VOR_ILS Parse Section **********************************************************

void parse_vorils_group(int count,int offset,int len)
{
    ils_vor *vi_ptr;
	char *buffer,*for_free;

	fseek(current_bg_file,offset,SEEK_SET);
	buffer = (char *)malloc(sizeof(char)*len);
	for_free = buffer;

	fread(buffer,len,1,current_bg_file);
	int rest_of_records = len;

	while(rest_of_records)
	{
	   vi_ptr = (ils_vor *)buffer;

	   if(vi_ptr->type != 0x04 || load_ils)		
	   {
	      double Lon = (DWORD) vi_ptr->longitude*(360.0 /(3*0x10000000))-180.0;
	      double Lat = 90.0 - (DWORD) vi_ptr->latitude * (180.0 / (2 * 0x10000000));

          char vi_id[0x49];
	      convert_id(vi_ptr->icao_id,vi_id);

		  total_vors++;
		  fprintf(vors_fp,"Lon=[%f] Lat=[%f] FREQ=[%lu] TYPE=[%d] ICAO_ID=[%s] Alt=[%f] ",Lon,Lat,vi_ptr->freq,vi_ptr->type,vi_id,vi_ptr->elevation/1000.0f);

	      convert_id( ( vi_ptr->reg_apt_icaoid &  0x7FF ) << 5,vi_id);
	      fprintf(vors_fp,"REG_ID=[%s] ",vi_id);
	      convert_id( ( vi_ptr->reg_apt_icaoid >> 11    ) << 5,vi_id);
		  fprintf(vors_fp, "APT_ID=[%s] ", vi_id);
			//********************************************************************************
			int rest_of_record = vi_ptr->header.size_of_record - sizeof(ils_vor);
			char *temp_buffer = (char *)vi_ptr + sizeof(ils_vor);

			while(rest_of_record)
			{
				if(*(WORD *)temp_buffer == 0x0019  )
				{
					name_sub *ns = (name_sub *)temp_buffer;
					fprintf(vors_fp,"NAME=[%s] ",
					K_FormName(ns->str,NULL,ns->header.size_of_record-sizeof(ns->header)));
				}
				rest_of_record-=((sub_common *)temp_buffer)->size_of_record;
				temp_buffer+=((sub_common *)temp_buffer)->size_of_record;
			}
			fprintf(vors_fp,"DME=[%s] MV=[%.2f]\n",vi_ptr->flags&0x10?"YES":"NO",MAGVAR(vi_ptr->mag_var));
			//********************************************************************************
	   }
	   rest_of_records-=vi_ptr->header.size_of_record;
	   buffer+=vi_ptr->header.size_of_record;
	}
	free(for_free);
}

void parse_vorils(long offset,long sec_count)
{
	K_load_dword_param("KLN90B", "ILSFREQ", &load_ils);
	section_header *headers = (section_header *)malloc(sizeof(section_header)*sec_count);
	fseek(current_bg_file,offset,SEEK_SET);
	fread(headers,sizeof(section_header)*sec_count,1,current_bg_file);
	for(int i=0;i<sec_count;i++)
	{
#ifdef _NBD_DEBUG
	  printf("\tCount: %d FileOffset: %X Size: %X\n",(headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
#endif
	  parse_vorils_group((headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
	}
	free(headers);
}
//*******  NDB Parse Section       *******************************************************
void parse_ndb_group(int count,int offset,int len)
{
    ndb *ndb_ptr;
	char *buffer,*for_free;

	fseek(current_bg_file,offset,SEEK_SET);
	buffer = (char *)malloc(sizeof(char)*len);
	for_free = buffer;

	fread(buffer,len,1,current_bg_file);
	int rest_of_records = len;

	while(rest_of_records)
	{
	   ndb_ptr = (ndb *)buffer;
       char ndb_id[0x49];
	   convert_id(ndb_ptr->icao_id,ndb_id);

	   double Lon = (DWORD) ndb_ptr->longitude*(360.0 /(3*0x10000000))-180.0;
	   double Lat = 90.0 - (DWORD) ndb_ptr->latitude * (180.0 / (2 * 0x10000000));
       total_ndbs++;
	   fprintf(ndbs_fp,"Lon=[%f] Lat=[%f] ICAO_ID=[%s] Type=[%d] ",Lon,Lat,ndb_id,(int)ndb_ptr->type);

	   convert_id( ( ndb_ptr->reg_apt_icaoid &  0x7FF ) << 5,ndb_id);
	   fprintf(ndbs_fp,"REGION_ID=[%s] ",ndb_id);
	   convert_id( ( ndb_ptr->reg_apt_icaoid >> 11    ) << 5,ndb_id);
	   fprintf(ndbs_fp,"APT_ID=[%s] ",ndb_id);

       int rest_of_record = ndb_ptr->header.size_of_record - sizeof(ndb);
	   char *temp_buffer = (char *)ndb_ptr + sizeof(ndb);

	   fprintf(ndbs_fp,"FREQ=[%.8lu] ",ndb_ptr->freq);

	   while(rest_of_record)
	   {
	      if(*(WORD *)temp_buffer == 0x0019  )
		  {
		     name_sub *ns = (name_sub *)temp_buffer;
			 fprintf(ndbs_fp,"NAME=[%s]\n",
				 K_FormName(ns->str,NULL,ns->header.size_of_record-sizeof(ns->header)));
		  }
		  rest_of_record-=((sub_common *)temp_buffer)->size_of_record;
		  temp_buffer+=((sub_common *)temp_buffer)->size_of_record;
	   }

	   rest_of_records-=ndb_ptr->header.size_of_record;
	   buffer+=ndb_ptr->header.size_of_record;
	}
	free(for_free);
}
void parse_ndbs(long offset,long sec_count)
{
    section_header *headers = (section_header *)malloc(sizeof(section_header)*sec_count);
	fseek(current_bg_file,offset,SEEK_SET);
	fread(headers,sizeof(section_header)*sec_count,1,current_bg_file);
	for(int i=0;i<sec_count;i++)
	{
#ifdef _NBD_DEBUG	  
      printf("\tCount: %d FileOffset: %X Size: %X\n",(headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
#endif
	  parse_ndb_group((headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
	}
	free(headers);

}
//-------------------------------- Markers ------------------------------------------------
void parse_marker_group(int count,int offset,int len)
{
    marker *marker_ptr;
	char *buffer,*for_free;

	fseek(current_bg_file,offset,SEEK_SET);
	buffer = (char *)malloc(sizeof(char)*len);
	for_free = buffer;

	fread(buffer,len,1,current_bg_file);
	int rest_of_records = len;

	while(rest_of_records)
	{
	   marker_ptr = (marker *)buffer;
       char marker_id[0x49];
	   convert_id(marker_ptr->icao_id,marker_id);

	   double Lon = (DWORD) marker_ptr->longitude*(360.0 /(3*0x10000000))-180.0;
	   double Lat = 90.0 - (DWORD) marker_ptr->latitude * (180.0 / (2 * 0x10000000));
       
	   if( (marker_id[0]!='0' && marker_id[1]!='\0') )
	   {
	      total_wpts++;
	      fprintf(wpts_fp,"Lon=[%f] Lat=[%f] ICAO_ID=[%s] ",Lon,Lat,marker_id);

	      convert_id( marker_ptr->reg_id ,marker_id);
	      fprintf(wpts_fp,"REGION_ID=[%s] TYPE=[%d] SUB_TYPE=[MRK]\n",marker_id,marker_ptr->type);
	   }
	   rest_of_records-=marker_ptr->header.size_of_record;
	   buffer+=marker_ptr->header.size_of_record;
	}
	free(for_free);
}
void parse_markers(long offset,long sec_count)
{
    section_header *headers = (section_header *)malloc(sizeof(section_header)*sec_count);
	fseek(current_bg_file,offset,SEEK_SET);
	fread(headers,sizeof(section_header)*sec_count,1,current_bg_file);
	for(int i=0;i<sec_count;i++)
	{
#ifdef _NBD_DEBUG	  
      printf("\tCount: %d FileOffset: %X Size: %X\n",(headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
#endif
	  parse_marker_group((headers+i)->number_of_records,(headers+i)->file_offset,(headers+i)->size_of_subsections);
	}
	free(headers);

}
//****************************************************************************************
void parse_new_bgl(char *file_name)
{
	bgl_header hdr;
	current_bg_file=fopen(file_name,"rb");
	fread(&hdr,sizeof(hdr),1,current_bg_file);
	fseek(current_bg_file,hdr.size_of_header,SEEK_SET);
	object_pointer *obj_ptrs = (object_pointer *)malloc(sizeof(object_pointer)*hdr.section_count);
	fread(obj_ptrs,sizeof(object_pointer)*hdr.section_count,1,current_bg_file);
//********************** My Fucking Patch For Names *****************************************
	for(UINT i=0;i<hdr.section_count;i++)
	{
	   if((obj_ptrs+i)->section_type == 0x27)
	   {
        //Name List
		  parse_names((obj_ptrs+i)->file_offset,(obj_ptrs+i)->sub_secs_count);
	   }
	}
//*******************************************************************************************

	for(UINT i=0;i<hdr.section_count;i++)
	{
#ifdef _NBD_DEBUG
	   printf("%X %X %X %X\n",(obj_ptrs+i)->section_type,
		                   (obj_ptrs+i)->sub_secs_count,
						   (obj_ptrs+i)->file_offset,
						   (obj_ptrs+i)->size_of_header);
#endif
	   if((obj_ptrs+i)->section_type == 0x22)
	   {
	       // Way points
		   parse_wpts((obj_ptrs+i)->file_offset,(obj_ptrs+i)->sub_secs_count);
	   }
       if((obj_ptrs+i)->section_type == 0x03)
	   {
	       // Airports
		   parse_airports((obj_ptrs+i)->file_offset,(obj_ptrs+i)->sub_secs_count);
	   }
	   if((obj_ptrs+i)->section_type == 0x13)
	   {
	       // Ils_Vor
		   parse_vorils((obj_ptrs+i)->file_offset,(obj_ptrs+i)->sub_secs_count);
	   }
	   if((obj_ptrs+i)->section_type == 0x17)
	   {
	       // Ndb
		   parse_ndbs((obj_ptrs+i)->file_offset,(obj_ptrs+i)->sub_secs_count);
	   }
	   if((obj_ptrs+i)->section_type == 0x18)
	   {
	      // Marker
	      parse_markers((obj_ptrs+i)->file_offset,(obj_ptrs+i)->sub_secs_count);
	   }
	}
    free(obj_ptrs);
	fclose(current_bg_file);
	clean_parse_names();
}
//****************************************************************************************
enum {NEW_BGL,OLD_BGL,COMP_BGL};
int kind_of_bgl(char *cFileName)
{
    FILE *temp_fp = fopen(cFileName,"rb");
	if(!temp_fp) return(-1);
	USHORT type=0;
	fread(&type,sizeof(type),1,temp_fp);
	fclose(temp_fp);
	switch(type)
	{
	case 0x0003:
	case 0x0004:
		if(is_bgl_compressed(cFileName))
		   return(COMP_BGL);
		return(OLD_BGL);
	case 0x0201:
		return(NEW_BGL);
	}
	return(-1);
}
//****************************************************************************
#define PSTR "%s"

UINT __stdcall __MsgHandler(PVOID Context,UINT Notification,UINT_PTR Param1,UINT_PTR Param2)
{
   static TCHAR MustExtractedName[MAX_PATH]={0};
   static TCHAR *extract_dir = (TCHAR *)Context;
   int i;

   switch(Notification)
   {
   case SPFILENOTIFY_FILEINCABINET:
	   {
		   TCHAR NameInCabinet[MAX_PATH]={0};
		   FILE_IN_CABINET_INFO *cab_info = (FILE_IN_CABINET_INFO *)Param1;
		   TCHAR                *cab_name = (TCHAR *)Param2;
		   for(i=_tcslen(cab_info->NameInCabinet);i>=0;i--)
		   {
		      if(cab_info->NameInCabinet[i]==_T('\\'))
			     break;
		   }
		   _stprintf(NameInCabinet,_T(PSTR),cab_info->NameInCabinet+i+1);
		   _tcslwr(NameInCabinet);

		   if(_tcsstr(NameInCabinet,_T(".bgl")))
		   {
		      _stprintf(cab_info->FullTargetName,_T(PSTR PSTR),extract_dir,NameInCabinet);
			  _tcscpy(MustExtractedName,cab_info->FullTargetName);
			  //_tprintf(PSTR"\n",cab_info->NameInCabinet);
			  return(FILEOP_DOIT);
		   }
		   return(FILEOP_SKIP);
	   }
   case SPFILENOTIFY_NEEDNEWCABINET:
	   {
	      break;
	   }
   case SPFILENOTIFY_FILEEXTRACTED:
	   {
          switch(kind_of_bgl(MustExtractedName))
	      {
	         case NEW_BGL:
		        parse_new_bgl(MustExtractedName);
		        break;
		     case OLD_BGL:
   		     case COMP_BGL:
		        parse_old_bgl(MustExtractedName);
		        break;
	      }
		  DeleteFile(MustExtractedName);
		  //_tprintf(_T("Extracted File: "PSTR"\n"),MustExtractedName);
		  break;
	   }
   }
   return(NO_ERROR);
}
//****************************************************************************
void mav_db_main(char *FS_MainDir)
{
//	parse_old_bgl("E:\\fs9\\Addon Scenery\\RSBN\\Scenery\\RSBN.bgl");

   ndbs_fp = fopen(ndbs_file,"w+");
   wpts_fp = fopen(wpts_file,"w+");
   sups_fp = fopen(sups_file,"w+");
   vors_fp = fopen(vors_file,"w+");
   apts_fp = fopen(apts_file,"w+");

   int r;
	char file_name[MAX_PATH];
    char local[MAX_PATH];
	char remote[MAX_PATH];
	char area_name[MAX_PATH];
    char full_name_bgl[MAX_PATH];
	sprintf(file_name,"%s%s",FS_MainDir,scenery_cfg);
	int areas=0,i;
    
	for(i=0;i<999;i++)
	{
        sprintf(area_name,"Area.%.3d",i);
		GetPrivateProfileString(area_name,"Local","NULL",local,sizeof(local),file_name);
		if(local[0] && strcmp(local,"NULL"))
			areas++;
		else
		{
		    GetPrivateProfileString(area_name,"Remote","NULL",remote,sizeof(remote),file_name);
		    if(remote[0] && strcmp(remote,"NULL"))
				areas++;
		}
	}
	int curr_area=0;
	
	total_ndbs=0; total_apts=0; total_vors=0; total_wpts=0; total_sups=0;

	for(i=0;i<999;i++)
	{
        sprintf(area_name,"Area.%.3d",i);
		GetPrivateProfileString(area_name,"Local","NULL",local,sizeof(local),file_name);
		if(local[0] && strcmp(local,"NULL"))
		{
		   print_str(3,1,ATTRIBUTE_NORMAL,"%3d PERCENT COMPLETE",curr_area*100/areas);
		   update_screen();
		   char area_title[MAX_PATH];
		   GetPrivateProfileString(area_name,"Title","NULL",area_title,sizeof(area_title),file_name);
           WIN32_FIND_DATA FindFileData;
		   char full_path[MAX_PATH];
		   memset(full_path,0,sizeof(full_path));
		   sprintf(full_path,"%s%s\\Scenery\\%s",FS_MainDir,local,"*.BGL");
		   HANDLE find_handle = FindFirstFile(full_path,&FindFileData);
		   if(find_handle != INVALID_HANDLE_VALUE)
		   {
		       
			   do
			   {
			       sprintf(full_name_bgl,"%s%s\\Scenery\\%s",FS_MainDir,local,FindFileData.cFileName);
				   switch(kind_of_bgl(full_name_bgl))
				   {
				   case NEW_BGL:
					   parse_new_bgl(full_name_bgl);					
					   break;
				   //case OLD_BGL:
   				   //case COMP_BGL:
				   //   parse_old_bgl(full_name_bgl);
				   //   break;
				   }
			   }while(FindNextFile(find_handle,&FindFileData));
			   FindClose(find_handle);
		   }
           curr_area++;
           continue;
		}
		else
		{
		    GetPrivateProfileString(area_name,"Remote","NULL",remote,sizeof(remote),file_name);
		    if(remote[0] && strcmp(remote,"NULL"))
		    {
		       print_str(3,1,ATTRIBUTE_NORMAL,"%3d PERCENT COMPLETE",curr_area*100/areas);
		       update_screen();
               char area_title[MAX_PATH];
			   int i;
			   if( strstr(remote,"[FS_DISC4]") )
			   {
			       char real_path[MAX_PATH];
                   char kln_config[MAX_PATH];
				   char cd_path[MAX_PATH];
				   sprintf(kln_config,"%s/KLN90B/%s",FS_MainDir,KLN_CONFIG_NAME);
                   GetPrivateProfileString("KLN90","CDPATH","NULL",cd_path,sizeof(cd_path),kln_config);
				   sprintf(real_path,"%s%s",cd_path,remote+strlen("[FS_DISC4]"));
				   BOOL ret = SetupIterateCabinet(real_path,0,__MsgHandler,FS_MainDir);
		           GetPrivateProfileString(area_name,"Title","NULL",area_title,sizeof(area_title),file_name);
			   }
			   curr_area++;
		       continue;
			}
		}
	}
	fprintf(ndbs_fp,"%.8X\n",total_ndbs);
	fprintf(wpts_fp,"%.8X\n",total_wpts);
	fprintf(sups_fp,"%.8X\n",total_sups);
	fprintf(vors_fp,"%.8X\n",total_vors);
	fprintf(apts_fp,"%.8X\n",total_apts);

	fflush(ndbs_fp);
    fflush(wpts_fp);
    fflush(vors_fp);
    fflush(apts_fp);
	fclose(ndbs_fp);
	fclose(wpts_fp);
	fclose(sups_fp);
	fclose(vors_fp);
	fclose(apts_fp);
}

