// --------------------------------------------------------------------------------
// Name:		apt7_pages.cpp
// Description:	APT7 subpages implementation for procedure selection
//
// SUBPAGES SCREEN DEFINITIONS
//
// No procedure
//
// 0  [ICAO]
// 1 NO SID/STAR
// 2 FOR THIS
// 3 AIRPORT
// 4 IN DATABASE
// 5
// 6     APT+7
//
// Display SIDs     Select Runway	Select Trans     Display Points
//
// 0  [ICAO]        [SSNAME]-SID     [SSNAME]-SID     [SSNAME]-SID
// 1 SELECT SID     RUNWAY           TRANSITION        1 [POINT1] 
// 2  1 [SID01]      1 [RNWY1]        1 [TRANS1]       2 [POINT2] 
// 3  2 [SID02]      2 [RNWY2]        2 [TRANS2]       3 [POINT3] 
// 4  3 [SID03]                       3 [TRANS3]       X [POINTX] 
// 5  X [SIDXX]                       X [TRANSX]      LOAD IN FPL 
// 6      APT+7            CRS               CRS              CRS
//
// Display STARs    Select Trans     Select Runway    Display Points    Load airport
//
// 0 [ICAO]         [SSNAME]-STAR    [SSNAME]-STAR    [SSNAME]-STAR     [SSNAME]-STAR
// 1 SELECT STAR    TRANSITION       RUNWAY            1 [POINT1]       PRESS ENT       
// 2  1 [STAR1]      1 [TRANS1]        1 [RNWY1]       2 [POINT2]       TO ADD [ICAO]        
// 3  2 [STAR2]      2 [TRANS2]        2 [RNWY2]       3 [POINT3]       AND STAR TO
// 4  3 [STAR3]      3 [TRANS3]                        X [POINTX]       FPL 0
// 5  X [STARX]      X [TRANSX]                       LOAD IN FPL         APPROVE?
// 6      APT+7            CRS               CRS              CRS
//
// ---------------------------------------------------------------------------------------


#include "PCH.h"
//#include "fslib.h"
//#include "kln90b.h"
//#include "nav_db.h"
#include "regexp.h"
#include "nearest.h"
#include "sid_star.h"
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

//extern nav_db_t nav_db;

typedef enum Apt7Subpage { APT7_SUBPG_NONE, APT7_SUBPG_SID, APT7_SUBPG_STAR } Apt7_Subpage_t;
Apt7_Subpage_t APT7_SUBPG = APT7_SUBPG_SID;

static int ss_start_offset;
static int ss_row;

typedef enum Apt7Stage { SS_SEL_SS, SS_SEL_RNW, SS_SEL_TRA, SS_SHOW_SS, SS_ACK_SS, SS_ACK_AP } Apt7Stage_t;
static Apt7Stage_t ss_sel_state = SS_SEL_SS;

static char SelectedProcedure[MAX_PATH];
static char SelectedRunway[MAX_PATH];
static char SelectedTransition[MAX_PATH];

static bool apt7_init(void);
static void activate_ss_onload(int &current_row);

// -----------------------------------------------------------------------------
// Name:		x
// -----------------------------------------------------------------------------

void print_apt7(nvdb_apt *__apt, char *status)
{
	int stars_cnt = __apt->stars->size();
	int sids_cnt = __apt->sids->size();
	int s_row;
	char ss_name[MAX_PATH];
	char header[MAX_PATH];

	// If no either SID or STAR information available
	if ((stars_cnt == 0) && (sids_cnt == 0))
		APT7_SUBPG = APT7_SUBPG_NONE;

	switch (APT7_SUBPG)
	{
	case APT7_SUBPG_NONE:
		print_str(5, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		print_str(1, 12, ATTRIBUTE_NORMAL, "NO SID/STAR");
		print_str(2, 12, ATTRIBUTE_NORMAL, "FOR THIS   ");
		print_str(3, 12, ATTRIBUTE_NORMAL, "AIRPORT    ");
		print_str(4, 12, ATTRIBUTE_NORMAL, "IN DATABASE");
		print_str(5, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		break;

	case APT7_SUBPG_SID:
		// 1st line: Overwrite with SID name in the header with '-SID' ending when selecting runway, transition or showing points
		if (ss_sel_state == SS_SEL_RNW || ss_sel_state == SS_SEL_TRA || ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
		{
			sprintf(header, "%s", SelectedProcedure);
			header[6] = '\0';
			strcat(header, "-SID");
			print_str(0, 12, ATTRIBUTE_NORMAL, "%-11s", header);
		}
		if (ss_sel_state == SS_SEL_SS)
			print_str(1, 12, ATTRIBUTE_NORMAL, "SELECT SID ");
		else if (ss_sel_state == SS_SEL_RNW)
			print_str(1, 12, ATTRIBUTE_NORMAL, "RUNWAY     ");
		else if (ss_sel_state == SS_SEL_TRA)
			print_str(1, 12, ATTRIBUTE_NORMAL, "TRANSITION ");
		
		// List Procedures, Runways, Transitions or Points from the actual top position: ss_start_offset
		s_row = 0;
		for (int i = ss_start_offset; i < ss_start_offset + 3; i++, s_row++)
		{
			if (i < sids_cnt)
			{
				sprintf(ss_name, "%s", (__apt->sids->at(i)).c_str());
				ss_name[6] = '\0';
				if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
					print_str(2 + s_row - 1, 12, ATTRIBUTE_NORMAL, "%2d %-8s", i + 1, ss_name);
				else
					print_str(2 + s_row, 12, ATTRIBUTE_NORMAL, "%2d %-8s", i + 1, ss_name);
			}
			else
			{
				if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
					print_str(2 + s_row - 1, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
				else
					print_str(2 + s_row, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
			}
		}
		if (sids_cnt > 3)
		{
			sprintf(ss_name, "%s", (__apt->sids->at(sids_cnt - 1)).c_str());
			ss_name[6] = '\0';
			if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
				print_str(4, 12, ATTRIBUTE_NORMAL, "%2d %-8s", sids_cnt, ss_name);
			else
				print_str(5, 12, ATTRIBUTE_NORMAL, "%2d %-8s", sids_cnt, ss_name);
		}
		else
		{
			if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
				print_str(4, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
			else
				print_str(5, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		}

		//
		if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
		{
			int load_in_fpl_row;
			if (sids_cnt > 3)
				load_in_fpl_row = 5;
			else
				load_in_fpl_row = 1 + sids_cnt;
			print_str(load_in_fpl_row++, 12, ATTRIBUTE_NORMAL, "LOAD IN FPL");
			if (ss_sel_state == SS_ACK_SS)
				K_change_attribute(load_in_fpl_row - 1, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);

			// Erase all remaining rows
			for (; load_in_fpl_row<6; load_in_fpl_row++)
				print_str(load_in_fpl_row, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		}

		// --- ??? ---
		if (*status != 'A' && ss_row > 0)
		{
			if (ss_sel_state != SS_ACK_SS)
			{
				if (ss_sel_state == SS_SHOW_SS)
					K_change_attribute(ss_row - 1, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);
				else
					K_change_attribute(ss_row, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);
			}
		}

		break;

	case APT7_SUBPG_STAR:
		// 1st line: Overwrite with STAR name in the header with '-STAR' ending when selecting runway, transition or showing points
		if (ss_sel_state == SS_SEL_RNW || ss_sel_state == SS_SEL_TRA || ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS || ss_sel_state == SS_ACK_AP)
		{
			sprintf(header, "%s", SelectedProcedure);
			header[6] = '\0';
			strcat(header, "-STAR");
			print_str(0, 12, ATTRIBUTE_NORMAL, "%-11s", header);
		}
		// 2nd line: 
		if (ss_sel_state == SS_SEL_SS)
			print_str(1, 12, ATTRIBUTE_NORMAL, "SELECT STAR");
		else if (ss_sel_state == SS_SEL_RNW)
			print_str(1, 12, ATTRIBUTE_NORMAL, "RUNWAY     ");
		else if (ss_sel_state == SS_SEL_TRA)
			print_str(1, 12, ATTRIBUTE_NORMAL, "TRANSITION ");
		else if (ss_sel_state == SS_ACK_AP)
		{
			print_str(1, 12, ATTRIBUTE_NORMAL, "PRESS ENT  ");
			print_str(2, 12, ATTRIBUTE_NORMAL, "TO ADD %s", __apt->ICAO_ID);
			print_str(3, 12, ATTRIBUTE_NORMAL, "AFTER STAR ");
			print_str(4, 12, ATTRIBUTE_NORMAL, "TO FPL 0   ");
			print_str(5, 12, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH, "  APPROVE? ");
			break;		// Nothing more to display
		}

		// List Procedures, Runways, Transitions or Points from the actual top position: ss_start_offset
		s_row = 0; // (ss_sel_state == SS_SHOW_SS) ? 1 : 2;
		for (int i = ss_start_offset; i < ss_start_offset + 3; i++, s_row++)
		{
			if (i < stars_cnt)
			{
				sprintf(ss_name, "%s", (__apt->stars->at(i)).c_str());
				ss_name[6] = '\0';
				if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
					print_str(2 + s_row - 1, 12, ATTRIBUTE_NORMAL, "%2d %-8s", i + 1, ss_name);
				else
					print_str(2 + s_row, 12, ATTRIBUTE_NORMAL, "%2d %-8s", i + 1, ss_name);
			}
			else
			{
				if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
					print_str(2 + s_row - 1, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
				else
					print_str(2 + s_row, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
			}
		}

		// In the last available row display the last item from the list
		if (stars_cnt > 3)
		{
			sprintf(ss_name, "%s", (__apt->stars->at(stars_cnt - 1)).c_str());
			ss_name[6] = '\0';
			if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
				print_str(4, 12, ATTRIBUTE_NORMAL, "%2d %-8s", stars_cnt, ss_name);
			else
				print_str(5, 12, ATTRIBUTE_NORMAL, "%2d %-8s", stars_cnt, ss_name);
		}
		// or clear it if no item left
		else
		{
			if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
				print_str(4, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
			else
				print_str(5, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		}

		// 
		if (ss_sel_state == SS_SHOW_SS || ss_sel_state == SS_ACK_SS)
		{
			int load_in_fpl_row;
			if (stars_cnt > 3)
				load_in_fpl_row = 5;
			else
				load_in_fpl_row = 1 + stars_cnt;

			print_str(load_in_fpl_row++, 12, ATTRIBUTE_NORMAL, "LOAD IN FPL");
			if (ss_sel_state == SS_ACK_SS)
				K_change_attribute(load_in_fpl_row - 1, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);

			// Erase all remaining rows
			for (; load_in_fpl_row<6; load_in_fpl_row++)
				print_str(load_in_fpl_row, 12, ATTRIBUTE_NORMAL, "%-11s", " ");
		}

		// --- ??? ---
		if (*status != 'A' && ss_row > 0)
		{
			if (ss_sel_state != SS_ACK_SS)
			{
				if (ss_sel_state == SS_SHOW_SS)
					K_change_attribute(ss_row - 1, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);
				else
					K_change_attribute(ss_row, 12, 11, ATTRIBUTE_INVERSE | ATTRIBUTE_FLASH);
			}
		}

		break;
	}

	if (*status == 'A')
	{
		if (__apt->stars->size()>0 && __apt->sids->size()>0)
			print_str(6, 21, ATTRIBUTE_NORMAL, "+");
		ss_start_offset = 0;
	}
}

// -----------------------------------------------------------------------------
// Name:		do_apt7
// Description:	Processes actions passed by do_apt_page() in apt_pages.cpp
// -----------------------------------------------------------------------------

int do_apt7(nvdb_apt *current_apt, int &current_row, int ACTION, int &input_mask)
{
	int ss_index;
	int ind_in_ss;

	switch (ACTION)
	{
	case ACTION_INIT:
		apt7_init();
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

		if (ss_sel_state == SS_ACK_SS || ss_sel_state == SS_ACK_AP)			// no step back if "Load to FP" is highlighted already
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

		if (ss_sel_state == SS_ACK_SS || ss_sel_state == SS_ACK_AP)			// no step forward if "Load to FP" is highlighted already
			return(1);

		switch (current_row)
		{
		case 0:
			if ((current_apt->sids->size()>0 || current_apt->stars->size()>0))
			{
				current_row = 2;
				ss_start_offset = 0;
				ss_row = current_row;
				input_mask |= INPUT_ENTER;
				ss_sel_state = SS_SEL_SS;
			}
			break;

		case 1:
		case 2:
		case 3:
		case 4:
			ind_in_ss = ss_start_offset + (current_row - 2);
			if (APT7_SUBPG == APT7_SUBPG_SID)
			{
				//SID handling
				int sids_count = current_apt->sids->size();
				if (current_row<4 && ind_in_ss<(sids_count - 1))
					current_row++;
				else
				{
					if ((ind_in_ss == sids_count - 2) && (current_row == 4))
						current_row = 5;
					else
					{
						if (ind_in_ss<(sids_count - 1))
							ss_start_offset++;
						else if (ss_sel_state == SS_SHOW_SS)
						{
							activate_ss_onload(current_row);
						}
					}
				}
			}
			else
			{
				//STAR handling
				int stars_count = current_apt->stars->size();
				if (current_row<4 && ind_in_ss<(stars_count - 1))
					current_row++;
				else
				{
					if ((ind_in_ss == stars_count - 2) && (current_row == 4))
						current_row = 5;
					else
					{
						if (ind_in_ss<(stars_count - 1))
							ss_start_offset++;
						else if (ss_sel_state == SS_SHOW_SS)
						{
							activate_ss_onload(current_row);
						}

					}
				}
			}
			ss_row = current_row;
			return 0;

		case 5:
			if (ss_sel_state == SS_SHOW_SS)
				activate_ss_onload(current_row);
			return 0;
		}

		break;

	case INPUT_ENTER:
		ss_index = ss_start_offset + (current_row - 2);
		
		if (ss_sel_state == SS_SEL_SS)		// We are just selected a SID/STAR from the list
		{
			switch (APT7_SUBPG)
			{
			case APT7_SUBPG_SID:
				sprintf(SelectedProcedure, "%s", (current_apt->sids->at(ss_index)).c_str());
				ss_load_sid_rnws(current_apt, SelectedProcedure);
				ss_sel_state = SS_SEL_RNW;
				break;

			case APT7_SUBPG_STAR:
				sprintf(SelectedProcedure, "%s", (current_apt->stars->at(ss_index)).c_str());
				ss_load_star_trans(current_apt, SelectedProcedure);
				if (current_apt->stars->size() > 1)
				{
					ss_sel_state = SS_SEL_TRA;
				}
				else
				{
					ss_load_star_rnws(current_apt, SelectedProcedure);
					ss_sel_state = SS_SEL_RNW;
				}
				break;
			}

			current_row = 2;
			ss_start_offset = 0;
			ss_row = current_row;
		}

		else if (ss_sel_state == SS_SEL_RNW)		// We are just selected a runway from the list
		{
			switch (APT7_SUBPG)
			{
			case APT7_SUBPG_SID:
				sprintf(SelectedRunway, "%s", (current_apt->sids->at(ss_index)).c_str());
				SelectedTransition[0] = '\0';
				ss_load_sid_trans(current_apt, SelectedProcedure);
				if (current_apt->sids->size() > 1)
				{
					ss_sel_state = SS_SEL_TRA;
				}
				else
				{
					ss_load_sid_points(current_apt, SelectedProcedure, SelectedRunway + 2, SelectedTransition);
					ss_sel_state = SS_SHOW_SS;
				}
				break;

			case APT7_SUBPG_STAR:
				sprintf(SelectedRunway, "%s", (current_apt->stars->at(ss_index)).c_str());
				ss_load_star_points(current_apt, SelectedProcedure, SelectedRunway + 2, SelectedTransition);
				ss_sel_state = SS_SHOW_SS;
				break;
			}

			current_row = 2;
			ss_start_offset = 0;
			ss_row = current_row;
		}

		else if (ss_sel_state == SS_SEL_TRA)		// We are just selected a transition from the list
		{
			switch (APT7_SUBPG)
			{
			case APT7_SUBPG_SID:
				sprintf(SelectedTransition, "%s", (current_apt->sids->at(ss_index)).c_str());
				ss_load_sid_points(current_apt, SelectedProcedure, SelectedRunway + 2, SelectedTransition);
				ss_sel_state = SS_SHOW_SS;
				break;

			case APT7_SUBPG_STAR:
				sprintf(SelectedTransition, "%s", (current_apt->stars->at(ss_index)).c_str());
				ss_load_star_rnws(current_apt, SelectedProcedure);
				ss_sel_state = SS_SEL_RNW;
				break;
			}

			current_row = 2;
			ss_start_offset = 0;
			ss_row = current_row;
		}

		else if (ss_sel_state == SS_ACK_SS)		// We are just requested to load the SID/STAR procedure to FP0
		{
			switch (APT7_SUBPG)
			{
			case APT7_SUBPG_SID:
				fpl_delete_sids_points();							// Delete SID if exists already
				fpl_insert_sid_points(current_apt, SelectedProcedure);	// Insert SID points
				do_apt_page(INPUT_RCURSOR);
				break;

			case APT7_SUBPG_STAR:
				fpl_delete_stars_points();							// Delete STAR if exists already
				fpl_insert_star_points(current_apt, SelectedProcedure);	// Insert STAR points

				// Checks if FP0 does not contain the airport as last navigation point
				if (!is_fp0_full())
				{
					nv_point_t *np;
					int index;

					np = fpl_get_last(&index);
					if ((np == NULL) || (strcmp(((nv_hdr_t*)(np->buffer_spec))->ICAO_ID, current_apt->ICAO_ID) != 0))
						ss_sel_state = SS_ACK_AP;
					else
						do_apt_page(INPUT_RCURSOR);
				}
				
				break;
			}

//			do_apt_page(INPUT_ENTER);
		}

		else if (ss_sel_state == SS_ACK_AP)		// We are just requested to add airport to FP0
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

// -----------------------------------------------------------------------------
// Name:		apt7_next_page
// Description:	Turn to next page
// -----------------------------------------------------------------------------

bool apt7_next_page(void)
{
	if (APT7_SUBPG == APT7_SUBPG_SID)
	{
		APT7_SUBPG = APT7_SUBPG_STAR;		// Next after SID page is STAR page
		return true;						// Paging had been processed
	}
	else
	{										// Next after STAR is not controlled here
		APT7_SUBPG = APT7_SUBPG_SID;		// but reset to SID page
		return false;						// Paging had not been processed
	}
}

// -----------------------------------------------------------------------------
// Name:		apt7_prev_page
// Description:	Turn to previous page
// -----------------------------------------------------------------------------

bool apt7_prev_page(void)
{
	if (APT7_SUBPG == APT7_SUBPG_STAR)
	{
		APT7_SUBPG = APT7_SUBPG_SID;		// Next before STAR page is SID page
		return true;						// Paging had been processed
	}

	return false;						// Paging had not been processed
}

// ---------------------------------------------------------------------------- -
// Name:		apt7_init
// Comment:
//		Called by do_apt_page() on ACTION == ACTION_INIT
// -----------------------------------------------------------------------------

static bool apt7_init(void)
{
	ss_sel_state = SS_SEL_SS;
	return true;
}


// ---------------------------------------------------------------------------- -
// Name:		apt7_reload_ss
// Comment:
//		Called by do_apt_page() on ACTION == INPUT_RCURSOR
// -----------------------------------------------------------------------------

void apt7_reload_ss(nvdb_apt *__apt)
{
	__apt->sids->clear();
	__apt->stars->clear();
	delete __apt->sids;
	delete __apt->stars;
	__apt->sids = NULL;
	__apt->stars = NULL;
	ss_load_on_demand(__apt);

	ss_sel_state = SS_SEL_SS;
	APT7_SUBPG = APT7_SUBPG_SID;
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
	ss_sel_state = SS_ACK_SS;
	current_row = 2;
	ss_start_offset = 0;
	ss_row = current_row;

}