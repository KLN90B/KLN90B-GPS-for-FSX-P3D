// --------------------------------------------------------------------------------
// Name:		apt8_page.cpp
// Description:	APT7 subpages implementation for non-precision approaches
//
// SUBPAGES SCREEN DEFINITIONS
//
// No approach
//
// 0 ICAO
// 1 NO APPROACH
// 2 FOR THIS
// 3 AIRPORT
// 4 IN DATABASE
// 5
// 6     APT 8
//
// Select Apprs    Select IAF		Display Points
//
// 0 ICAO IAP       APPR-ICAO		APPR-ICAO
// 1 1 TYP  RWY     IAF 1 NAME1		 1 POINT1 
// 2 2 TYP  RWY         2 NAME2		 2 POINT2 
// 3 3 TYP  RWY         3 NAME3		 3 POINT3 
// 4 4 TYP  RWY         4 NAME4		 X POINTX 
// 5 X TYP  RWY						LOAD IN FPL 
// 6      APT 8            CRSR		        CRSR
//
// ---------------------------------------------------------------------------------------


#include "PCH.h"
#include "regexp.h"
#include "nearest.h"
#include "sid_star.h"
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

typedef enum Apt8Subpage { APT8_SUBPG_NONE, APT8_SUBPG_APP } Apt8_Subpage_t;
Apt8_Subpage_t APT8_SUBPG = APT8_SUBPG_APP;

static int ss_start_offset;
static int ss_row;

typedef enum AptStage { SS_SEL_APP, SS_SEL_IAF, SS_SHOW_APP, SS_ACK_APP, SS_ACK_ARP } Apt8Stage_t;
static Apt8Stage_t ss_sel_state = SS_SEL_APP;

static char SelectedApproach[MAX_PATH];
static char SelectedIAF[MAX_PATH];

static bool apt8_init(void);
static void activate_ss_onload(int &current_row);

// -----------------------------------------------------------------------------
// Name:		x
// -----------------------------------------------------------------------------

void print_apt8(nvdb_apt *__apt, char *status)
{
	int apps_cnt = __apt->apps->size();
	int s_row;
	char ss_name[MAX_PATH];
	char header[MAX_PATH];

	// If no approach information available
	if (apps_cnt == 0)
		APT8_SUBPG = APT8_SUBPG_NONE;

	switch (APT8_SUBPG)
	{
	case APT8_SUBPG_NONE:
		print_str(1, 12, ATTRIBUTE_NORMAL, "NO APPROACH");
		print_str(2, 12, ATTRIBUTE_NORMAL, "FOR THIS   ");
		print_str(3, 12, ATTRIBUTE_NORMAL, "AIRPORT    ");
		print_str(4, 12, ATTRIBUTE_NORMAL, "IN DATABASE");
		print_str(5, 12, ATTRIBUTE_NORMAL, "           ");
		break;

	case APT8_SUBPG_APP:
		// Print header
		switch (ss_sel_state)
		{
		case SS_SEL_APP:
			print_str(0, 12, ATTRIBUTE_NORMAL, "%s IAP", __apt->ICAO_ID);
			break;

		case SS_SEL_IAF:
			print_str(0, 12, ATTRIBUTE_NORMAL, "%s-%s", SelectedApproach, __apt->ICAO_ID);		// !!! NOT FINAL STRING VERSION. TYPE SHOUD BE ADDED: V25R, I25R, N25R !!!
			print_str(1, 12, ATTRIBUTE_NORMAL, "IAF ");
			break;
		case SS_SHOW_APP:
		case SS_ACK_APP:
		case SS_ACK_ARP:
			print_str(0, 12, ATTRIBUTE_NORMAL, "%s-%s", SelectedApproach, __apt->ICAO_ID);		// !!! NOT FINAL STRING VERSION. TYPE SHOUD BE ADDED: V25R, I25R, N25R !!!
			break;

		}
		
		// List Approaches, IAFs or Points from the actual top position: ss_start_offset
		s_row = 0;
		for (int i = ss_start_offset; i < ss_start_offset + 3; i++, s_row++)
		{
			if (i < apps_cnt)
			{
				sprintf(ss_name, "%s", (__apt->apps->at(i)).c_str());
				ss_name[6] = '\0';
				if (ss_sel_state == SS_SHOW_APP || ss_sel_state == SS_ACK_APP)
					print_str(2 + s_row - 1, 12, ATTRIBUTE_NORMAL, "%2d %-8s", i + 1, ss_name);
				else
					print_str(2 + s_row, 12, ATTRIBUTE_NORMAL, "%2d %-8s", i + 1, ss_name);
			}
			else
			{
				if (ss_sel_state == SS_SHOW_APP || ss_sel_state == SS_ACK_APP)
					print_str(2 + s_row - 1, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
				else
					print_str(2 + s_row, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
			}
		}
		if (apps_cnt > 3)
		{
			sprintf(ss_name, "%s", (__apt->apps->at(apps_cnt - 1)).c_str());
			ss_name[6] = '\0';
			if (ss_sel_state == SS_SHOW_APP || ss_sel_state == SS_ACK_APP)
				print_str(4, 12, ATTRIBUTE_NORMAL, "%2d %-8s", apps_cnt, ss_name);
			else
				print_str(5, 12, ATTRIBUTE_NORMAL, "%2d %-8s", apps_cnt, ss_name);
		}
		else
		{
			if (ss_sel_state == SS_SHOW_APP || ss_sel_state == SS_ACK_APP)
				print_str(4, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
			else
				print_str(5, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		}

		//
		if (ss_sel_state == SS_SHOW_APP || ss_sel_state == SS_ACK_APP)
		{
			int load_in_fpl_row;
			if (apps_cnt > 3)
				load_in_fpl_row = 5;
			else
				load_in_fpl_row = 1 + apps_cnt;
			print_str(load_in_fpl_row++, 12, ATTRIBUTE_NORMAL, "LOAD IN FPL");
			if (ss_sel_state == SS_ACK_APP)
				K_change_attribute(load_in_fpl_row - 1, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);

			// Erase all remaining rows
			for (; load_in_fpl_row<6; load_in_fpl_row++)
				print_str(load_in_fpl_row, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		}

		// --- ??? ---
		if (*status != 'A' && ss_row > 0)
		{
			if (ss_sel_state != SS_ACK_APP)
			{
				if (ss_sel_state == SS_SHOW_APP)
					K_change_attribute(ss_row - 1, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);
				else
					K_change_attribute(ss_row, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);
			}
		}

		break;
	}

	if (*status == 'A')
	{
		if (__apt->apps->size() > 0)
			print_str(6, 21, ATTRIBUTE_NORMAL, "+");
		ss_start_offset = 0;
	}
}

// -----------------------------------------------------------------------------
// Name:		do_apt8
// Description:	Processes actions passed by do_apt_page() in apt_pages.cpp
// -----------------------------------------------------------------------------

int do_apt8(nvdb_apt *current_apt, int &current_row, int ACTION, int &input_mask)
{
	int ss_index;
	int ind_in_ss;

	switch (ACTION)
	{
	case ACTION_INIT:
		apt8_init();
		return true;

	case ACTION_TIMER:
		if (current_row > 0)
		{
			print_apt(current_apt, "CRSR ");
			update_screen();
		}
		else
			ss_row = current_row;

		return 0;

	case INPUT_RCURSOR:
		ss_row = current_row;
		return 0;

	case INPUT_ROUTERMINUS:

		if (ss_sel_state == SS_ACK_APP || ss_sel_state == SS_ACK_ARP)			// no step back if "Load to FP" is highlighted already
			return(1);

		if (current_row >= 2)
		{
			if (current_row == 2)
			{
				if (ss_start_offset > 0)
					ss_start_offset--;
			}
			else
			{
				current_row--;
			}
			ss_row = current_row;
		}
		return 0;

	case INPUT_ROUTERPLUS:			// Step forward

		if (ss_sel_state == SS_ACK_APP || ss_sel_state == SS_ACK_ARP)			// no step forward if "Load to FP" is highlighted already
			return(1);

		switch (current_row)
		{
		case 0:
			if (current_apt->apps->size()>0)
			{
				current_row = 2;
				ss_start_offset = 0;
				ss_row = current_row;
				input_mask |= INPUT_ENTER;
				ss_sel_state = SS_SEL_APP;
			}
			break;

		case 1:
		case 2:
		case 3:
		case 4:
			ind_in_ss = ss_start_offset + (current_row - 2);

			int apps_count = current_apt->apps->size();
			if (current_row<4 && ind_in_ss<(apps_count - 1))
				current_row++;
			else
			{
				if ((ind_in_ss == apps_count - 2) && (current_row == 4))
					current_row = 5;
				else
				{
					if (ind_in_ss<(apps_count - 1))
						ss_start_offset++;
					else if (ss_sel_state == SS_SHOW_APP)
					{
						activate_ss_onload(current_row);
					}
				}
			}
			ss_row = current_row;
			return 0;

		case 5:
			if (ss_sel_state == SS_SHOW_APP)
				activate_ss_onload(current_row);
			return 0;
		}

		break;

	case INPUT_ENTER:
		ss_index = ss_start_offset + (current_row - 2);
		
		if (ss_sel_state == SS_SEL_APP)		// We are just selected an approach from the list
		{
			sprintf(SelectedApproach, "%s", (current_apt->apps->at(ss_index)).c_str());
			SelectedIAF[0] = '\0';
			ss_load_iafs(current_apt, SelectedApproach);
			if (current_apt->apps->size() > 1)
			{
				ss_sel_state = SS_SEL_IAF;
			}
			else
			{
				ss_load_app_points(current_apt, SelectedApproach, SelectedIAF);
				ss_sel_state = SS_SHOW_APP;
			}
			current_row = 2;
			ss_start_offset = 0;
			ss_row = current_row;
		}

		else if (ss_sel_state == SS_SEL_IAF)		// We are just selected an IAF from the list
		{
			sprintf(SelectedIAF, "%s", (current_apt->apps->at(ss_index)).c_str());
			sss_load_app_points(current_apt, SelectedApproach, SelectedIAF);
			ss_sel_state = SS_SHOW_APP;
			current_row = 2;
			ss_start_offset = 0;
			ss_row = current_row;
		}

		else if (ss_sel_state == SS_ACK_APP)		// We are just requested to load the approach procedure to FP0
		{
			fpl_delete_apps_points();								// Delete approach if exists already
			fpl_insert_app_points(current_apt, SelectedApproach);	// Insert approach points
			do_apt_page(INPUT_RCURSOR);

			// Checks if FP0 does not contain the airport as last navigation point
			if (!is_fp0_full())
			{
				nv_point_t *np;
				int index;

				np = fpl_get_last(&index);
				if ((np == NULL) || (strcmp(((nv_hdr_t*)(np->buffer_spec))->ICAO_ID, current_apt->ICAO_ID) != 0))
					ss_sel_state = SS_ACK_ARP;
				else
					do_apt_page(INPUT_RCURSOR);
			}
		}

		else if (ss_sel_state == SS_ACK_ARP)		// We are just requested to add airport to FP0
		{
			nv_point_t point;
			point.buffer_spec = current_apt;
			point.type = NAVAID_APT;
			append_to_fp0(&point);
			do_apt_page(INPUT_RCURSOR);
		}

		else	// not processed here
		{
			return 0;
		}

		return 1;

	default:
		break;

	}

	return 0;	// Not processed
}

// ---------------------------------------------------------------------------- -
// Name:		apt8_init
// Comment:
//		Called by do_apt_page() on ACTION == ACTION_INIT
// -----------------------------------------------------------------------------

static bool apt8_init(void)
{
	ss_sel_state = SS_SEL_APP;
	return true;
}


// ---------------------------------------------------------------------------- -
// Name:		apt8_reload_app
// Comment:
//		Called by do_apt_page() on ACTION == INPUT_RCURSOR
// -----------------------------------------------------------------------------

void apt8_reload_app(nvdb_apt *__apt)
{
	__apt->apps->clear();
	delete __apt->apps;
	__apt->apps = NULL;
//	ss_load_on_demand(__apt);

	ss_sel_state = SS_SEL_APP;
	APT8_SUBPG = APT8_SUBPG_APP;
}

// ===============================================================================
// STATIC ROUTINES
// ===============================================================================

// -------------------------------------------------------------------------------
// Name:		activate_ss_onload
// Dscription:	Activates the "LOAD IN FPL" status
// -------------------------------------------------------------------------------

static void activate_ss_onload( int &current_row)
{
	ss_sel_state = SS_ACK_APP;
	current_row = 2;
	ss_start_offset = 0;
	ss_row = current_row;

}