// ----------------------------------------------------------------------------------
// navdata_1.cpp
//
// Traditional KLN90B PTT navigation package 
//
// Routines to retrieve waypoint and SID/STAR data from the traditional KLN90B files.
// Theres is no official way to receive updated versions of these navigation files,
// thus these routines are only provided for backeard compatibility
//
// Procedure file stucture:
//
// SID/STAR storage in the files
//	[LHBP_STARS]
//	Count=24
//	[LHBP_STAR_000]
//	[LHBP_STAR_001]
//	...
//	[LHBP_STAR_023]
//
// Waypoint file structure:
// 
// ; ------------------------------------------------------
// ; AIRAC Cycle : 1504 (02 / APR / 2015 - 29 / APR / 2015) - Ver.1
// ; ------------------------------------------------------
// XONRA	 50.745556	  18.805833	15
// BP103	 47.273528	  19.321667	15
// VEBOS	 47.306389	  18.637222	15
// FD13L	 47.532845	  19.114181	15
//
// Read  description of navigation package routine rules in navdata.h
//
// Dependencies:
//		GetNavDatabaseDir()
//		K_DEBUG()
// ----------------------------------------------------------------------------------

#include "PCH.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include "kln90b.h"
#include "navdata.h"

using namespace std;

#define DB_DIRECTORY "DB\\0"
#define SID_STAR_FILENAME "KLN_sid_star"
#define WAYPOINTS_FILENAME "isec.txt"

// Package specific helper routines
static char *get_ss_file_name(char *icao_id);

// ==========================================================================================
// Package specific interface routines
// ==========================================================================================

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDNames_1()
// Description:			Loads the SID name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	SID names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_1(char *AirportICAO, vector<string> &ProcNameList)
{
	// Allocate buffers for GetPrivateProfileString() section and key parameters
	char ss_header[MAX_PATH];
	char ss_value[MAX_PATH];
	char proc_fname[MAX_PATH];

	// Get name of procedure file
	strcpy(proc_fname, get_ss_file_name(AirportICAO));

	// Read number of SIDs
	sprintf(ss_header, "%s_SIDS", AirportICAO);
	GetPrivateProfileString(ss_header, "Count", "NULL", ss_value, sizeof(ss_value), proc_fname);
	if (strcmp(ss_value, "NULL"))
	{
		// Read SID name list and sort it
		int sids_count = atol(ss_value);
		for (int sid = 0; sid < sids_count; sid++)
		{
			sprintf(ss_header, "%s_SID_%03d", AirportICAO, sid);
			GetPrivateProfileString(ss_header, "Id", "NULL", ss_value, sizeof(ss_value), proc_fname);
			// Item should exist, but if not, let's continue with the next one
			if (!strcmp(ss_value, "NULL"))
				continue;
			// Store SID name if not duplicate
			if ( find(ProcNameList.begin(), ProcNameList.end(), ss_value) == ProcNameList.end())
				ProcNameList.push_back(ss_value);
		}
	}

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARNames_1()
// Description:			Loads the STAR name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	STAR names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARNames_1(char *AirportICAO, vector<string> &ProcNameList)
{
	// Allocate buffers for GetPrivateProfileString() section and key parameters
	char ss_header[MAX_PATH];
	char ss_value[MAX_PATH];
	char proc_fname[MAX_PATH];

	// Get name of procedure file
	strcpy(proc_fname, get_ss_file_name(AirportICAO));

	// Read number of STARs
	sprintf(ss_header, "%s_STARS", AirportICAO);
	GetPrivateProfileString(ss_header, "Count", "NULL", ss_value, sizeof(ss_value), proc_fname);
	if (strcmp(ss_value, "NULL"))
	{
		// Read STAR name list and sort it
		int stars_count = atol(ss_value);
		for (int star = 0; star<stars_count; star++)
		{
			sprintf(ss_header, "%s_STAR_%03d", AirportICAO, star);
			GetPrivateProfileString(ss_header, "Id", "NULL", ss_value, sizeof(ss_value), proc_fname);
			// Item should exist, but if not, let's continue with the next one
			if (!strcmp(ss_value, "NULL"))
				continue;
			// Store STAR name if not duplicate
			if (find(ProcNameList.begin(), ProcNameList.end(), ss_value) == ProcNameList.end())
				ProcNameList.push_back(ss_value);
		}
	}

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDRunways_1()
// Description:			Loads the runway name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDRunways_1(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList)
{
	char ss_header[MAX_PATH];
	char ss_value[MAX_PATH];
	char rnw_str[MAX_PATH];
	int sids_count;
	int sid;

	sprintf(ss_header, "%s_SIDS", AirportICAO);
	GetPrivateProfileString(ss_header, "Count", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
	if (strcmp(ss_value, "NULL"))
	{
		sids_count = atol(ss_value);
		for (sid = 0; sid < sids_count; sid++)
		{
			sprintf(ss_header, "%s_SID_%03d", AirportICAO, sid);
			GetPrivateProfileString(ss_header, "Id", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
			if (!strcmp(ss_value, "NULL")) continue;
			if (!strcmp(ss_value, SIDName))
			{
				GetPrivateProfileString(ss_header, "RNW", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
				sprintf(rnw_str, "RW%s", ss_value);
				RunwayNameList.push_back(rnw_str);
			}
		}
	}

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARRunways_1()
// Description:			Loads the runway name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARRunways_1(char *AirportICAO, char *STARName, vector<string> &RunwayNameList)
{
	char ss_header[MAX_PATH];
	char ss_value[MAX_PATH];
	char rnw_str[MAX_PATH];
	int stars_count;
	int star;

	sprintf(ss_header, "%s_STARS", AirportICAO);
	GetPrivateProfileString(ss_header, "Count", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
	if (strcmp(ss_value, "NULL"))
	{
		stars_count = atol(ss_value);
		for (star = 0; star < stars_count; star++)
		{
			sprintf(ss_header, "%s_STAR_%03d", AirportICAO, star);
			GetPrivateProfileString(ss_header, "Id", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
			if (!strcmp(ss_value, "NULL")) continue;
			if (!strcmp(ss_value, STARName))
			{
				GetPrivateProfileString(ss_header, "RNW", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
				sprintf(rnw_str, "RW%s", ss_value);
				RunwayNameList.push_back(rnw_str);
			}
		}
	}

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSIDPoints_1()
// Description			Loads the navigation point list of the defined SID procedure into the provided list
// Parameters	
//		AirportICAO		Airport structure
//		SIDName			SID name for the associated navigation points should be retrieved
//		RunwayName		Runway name
//		TranName		Transition name
//		SIDPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

bool LoadSIDPoints_1(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList)
{
	char ss_header[MAX_PATH];
	char ss_value[MAX_PATH];
	char pt_value[MAX_PATH];
	nv_hdr_t nav_point;
	int i, sid;

	// Looking for the proper section in the procedure file as for example...
	// File: KLN_sid_star_LH.dat
	// [LHBP_SIDS]
	// Count = 42
	sprintf(ss_header, "%s_SIDS", AirportICAO);
	GetPrivateProfileString(ss_header, "Count", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
	if (strcmp(ss_value, "NULL"))		// SID counter parameter is found
	{
		int sids_count = atol(ss_value);

		// Go through all the SIDs to find the defined one based on name and runway...
		// [LHBP_SID_000]
		// Id = BADO1D
		//	RNW = 13L
		for (sid = 0; sid < sids_count; sid++)
		{
			sprintf(ss_header, "%s_SID_%03d", AirportICAO, sid);
			GetPrivateProfileString(ss_header, "Id", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
			if (!strcmp(ss_value, "NULL"))		// if SID with this index does not exist
				continue;
			if (!strcmp(ss_value, SIDName))		// if name of SID is what we are looking for
			{
				GetPrivateProfileString(ss_header, "RNW", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
				if (!strcmp(ss_value, RunwayName))			// if runway is what we are looking for
				{
					for (i = 0; i < MAX_SS_POINTS; i++)
					{
						sprintf(pt_value, "Point%02d", i);
						GetPrivateProfileString(ss_header, pt_value, "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
						if (!strcmp(ss_value, "NULL"))	// if procedure point with this index does not exist, then the previous was
							break;						// the last point, so terminate search
						K_GetParam(ss_value, "Name=", STRING_T, nav_point.ICAO_ID);
						K_GetParam(ss_value, "Lat=", DOUBLE_T, &nav_point.Lat);
						K_GetParam(ss_value, "Lon=", DOUBLE_T, &nav_point.Lon);
						SIDPointList.push_back(nav_point);
					}
					break;		// We have found it, don't search for more.
				}
			}
		}
	}

	return true;
}

// Name					LoadSTARPoints_1()
// Description			Loads the navigation point list of the defined STAR procedure into the provided list
// Parameters	
//		AirportICAO		Airport structure
//		STARName			STAR name for the associated navigation points should be retrieved
//		RunwayName		Runway name
//		TranName		Transition name
//		STARPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

bool LoadSTARPoints_1(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList)
{
	char ss_header[MAX_PATH];
	char ss_value[MAX_PATH];
	char pt_value[MAX_PATH];
	nv_hdr_t nav_point;
	int i;

	// Looking for the proper section in the procedure file as for example...
	// File: KLN_sid_star_LH.dat
	// [LHBP_STARS]
	// Count = 24
	sprintf(ss_header, "%s_STARS", AirportICAO);
	GetPrivateProfileString(ss_header, "Count", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
	if (strcmp(ss_value, "NULL"))					// if name of STAR is what we are looking for
	{
		int stars_count = atol(ss_value);

		// Go through all the STARs to find the defined one based on name and runway...
		// [LHBP_STAR_001]
		// Id = ABON1R
		// RNW = 13R
		for (int star = 0; star < stars_count; star++)
		{
			sprintf(ss_header, "%s_STAR_%03d", AirportICAO, star);
			GetPrivateProfileString(ss_header, "Id", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
			if (!strcmp(ss_value, "NULL"))		// if STAR with this index does not exist
				continue;
			if (!strcmp(ss_value, STARName))		// if runway is what we are looking for
			{
				GetPrivateProfileString(ss_header, "RNW", "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
				if (!strcmp(ss_value, RunwayName))		// if runway is what we are looking for
				{
					for (i = 0; i < MAX_SS_POINTS; i++)
					{
						sprintf(pt_value, "Point%02d", i);
						GetPrivateProfileString(ss_header, pt_value, "NULL", ss_value, sizeof(ss_value), get_ss_file_name(AirportICAO));
						if (!strcmp(ss_value, "NULL"))		// if procedure point with this index does not exist, then the previous was
							break;							// the last point, so terminate search
						K_GetParam(ss_value, "Name=", STRING_T, nav_point.ICAO_ID);
						K_GetParam(ss_value, "Lat=", DOUBLE_T, &nav_point.Lat);
						K_GetParam(ss_value, "Lon=", DOUBLE_T, &nav_point.Lon);
						STARPointList.push_back(nav_point);
					}
					break;		// We have found it, don't search for more.
				}
			}
		}
	}

	return true;
}

// ----------------------------------------------------------------------------------
// Name:				LoadWaypoints_1()
// Description:			Reading waysponts data into the provided data structure list
// Parameters:
//		ints			Waypoint data structure vector to fill in
// Return:
//		Result status
// Comment:				Originally this file had been placed in the base directory directly
// ----------------------------------------------------------------------------------

bool LoadWaypoints_1(vector<nvdb_wpt>& ints)
{
	char buffer[MAX_PATH];
	char sub_type[4];
	nvdb_wpt *__wpts;
	char file_name[MAX_PATH];
	nvdb_wpt temp_wpt;

	sprintf(file_name, "%s\\%s", GetNavDatabaseDir(), WAYPOINTS_FILENAME);
	K_DEBUG("[HG] Waypoint file name: %s\n", file_name);

	FILE *int_file = fopen(file_name, "rb");
	if (!int_file)
	{
		K_DEBUG("[HG] Error opening waypoint file %s in Load_WPTS()\n", file_name);
		return false;
	}
	while (1)
	{
		fgets(buffer, sizeof(buffer) - 1, int_file);
		if (buffer[0] == ';') continue;
		if (feof(int_file))
			break;
		memset(&temp_wpt, 0, sizeof(temp_wpt));
		int str_row = 0;
		char temp_str[MAX_PATH];
		int tstr_ind;
		//========================== ICAO ID ==============================
		while (buffer[str_row] == 0x20 || buffer[str_row] == 0x09)
			str_row++;
		tstr_ind = 0;
		while (buffer[str_row] != 0x20 && buffer[str_row] != 0x09)
			temp_str[tstr_ind++] = buffer[str_row++];
		temp_str[tstr_ind] = '\0';
		strcpy(temp_wpt.ICAO_ID, temp_str);
		//========================== Latitude ==============================
		while (buffer[str_row] == 0x20 || buffer[str_row] == 0x09)
		{
			if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
				break;
			str_row++;
		}
		if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
			continue;
		tstr_ind = 0;
		while (buffer[str_row] != 0x20 && buffer[str_row] != 0x09)
		{
			if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
				break;
			temp_str[tstr_ind++] = buffer[str_row++];
		}
		if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
			continue;
		temp_str[tstr_ind] = '\0';
		temp_wpt.Lat = atof(temp_str);
		//========================== Longitude ==============================
		while (buffer[str_row] == 0x20 || buffer[str_row] == 0x09)
		{
			if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
				break;
			str_row++;
		}
		if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
			continue;
		tstr_ind = 0;
		while (buffer[str_row] != 0x20 && buffer[str_row] != 0x09)
		{
			if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
				break;
			temp_str[tstr_ind++] = buffer[str_row++];
		}
		if (buffer[str_row] == '\n' || buffer[str_row] == '\r')
			continue;
		temp_str[tstr_ind] = '\0';
		temp_wpt.Lon = atof(temp_str);
		ints.push_back(temp_wpt);
	}

	fclose( int_file );
	return true;
}

// ==========================================================================================
// Package specific helper routines
// ==========================================================================================

// ------------------------------------------------------------------------------------------
// Name:		get_ss_file_name
// Description:	Assembles SID/STAR file name based on airport ICAO
// Parameters:
//		icao_id		Airport ICAO code
// Return:
//		Fully qualified procedure file name as pointer to static variable
// ------------------------------------------------------------------------------------------

static char *get_ss_file_name(char *icao_id)
{
	static char sid_star_file[MAX_PATH];

	sprintf(sid_star_file, "%s\\Sid_Star\\%s_%c%c.dat", GetNavDatabaseDir(), SID_STAR_FILENAME, icao_id[0], icao_id[1]);

	return(sid_star_file);
} 

