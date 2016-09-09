// ----------------------------------------------------------------------------------
// navdata_0.cpp
//
// Description: Main switchboard among navigation database packs
// ----------------------------------------------------------------------------------

#include "PCH.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <Shlwapi.h>	// for PathFileExists()
#include "kln90b.h"
#include "navdata.h"

#define	NAVDB Switchboard[GetNavDatabaseCode()]

typedef struct NavDBFunctions {
	bool (*LoadSIDNames)(char *AirportICAO, vector<string> &ProcNameList);
	bool (*LoadSTARNames)(char *AirportICAO, vector<string> &ProcNameList);
	bool (*LoadSIDRunways)(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList);
	bool (*LoadSTARRunways)(char *AirportICAO, char *STARName, vector<string> &RunwayNameList);
	bool(*LoadSIDTransitions)(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList);
	bool(*LoadSTARTransitions)(char *AirportICAO, char *STARName, vector<string> &TransitionNameList);
	bool(*LoadSIDPoints)(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList);
	bool(*LoadSTARPoints)(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList);
	bool (*LoadWaypoints)(vector<nvdb_wpt>& ints);
} NavDBFunctions_t;

static NavDBFunctions_t Switchboard[] = { 
	// No navdata package dummy functions
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },	
	// Traditional PTT pack
	{ LoadSIDNames_1, LoadSTARNames_1, LoadSIDRunways_1, LoadSTARRunways_1, NULL, NULL, LoadSIDPoints_1, LoadSTARPoints_1, LoadWaypoints_1 },	
	// "EADT KLN 90B" pack
	{ LoadSIDNames_2, LoadSTARNames_2, LoadSIDRunways_2, LoadSTARRunways_2, LoadSIDTransitions_2, LoadSTARTransitions_2, LoadSIDPoints_2, LoadSTARPoints_2, LoadWaypoints_2 },	
	// "PSS Airbus/Boeing/Dash" pack
	{ LoadSIDNames_3, LoadSTARNames_3, LoadSIDRunways_3, LoadSTARRunways_3, LoadSIDTransitions_3, LoadSTARTransitions_3, LoadSIDPoints_3, LoadSTARPoints_3, LoadWaypoints_3 },
	// "vasFMC Flight Management" pack
	{ LoadSIDNames_4, LoadSTARNames_4, LoadSIDRunways_4, LoadSTARRunways_4, LoadSIDTransitions_4, LoadSTARTransitions_4, LoadSIDPoints_4, LoadSTARPoints_4, LoadWaypoints_4 }
};

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDNames()
// Description:			Loads the SID name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	SID names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDNames(char *AirportICAO, vector<string> &ProcNameList)
{
	// PRINTLOG(5, "Entering LoadSIDNames() with ICAO = %s", AirportICAO);
	if (NAVDB.LoadSIDNames)
		return NAVDB.LoadSIDNames(AirportICAO, ProcNameList);

	return false;	// No procedure defined for this action in the navigation pack
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARNames()
// Description:			Loads the STAR name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	STAR names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARNames(char *AirportICAO, vector<string> &ProcNameList)
{
	if (NAVDB.LoadSTARNames)
		return NAVDB.LoadSTARNames(AirportICAO, ProcNameList);

	return false;	// No procedure defined for this action in the navigation pack
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDRunways()
// Description:			Loads the runway name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDRunways(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList)
{
	if (NAVDB.LoadSIDRunways)
		return NAVDB.LoadSIDRunways(AirportICAO, SIDName, RunwayNameList);

	return false;	// No procedure defined for this action in the navigation pack
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARRunways()
// Description:			Loads the runway name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARRunways(char *AirportICAO, char *STARName, vector<string> &RunwayNameList)
{
	if (NAVDB.LoadSTARRunways)
		return NAVDB.LoadSTARRunways(AirportICAO, STARName, RunwayNameList);

	return false;	// No procedure defined for this action in the navigation pack
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDTransitions()
// Description:			Loads the transition name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDTransitions(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList)
{
	if (NAVDB.LoadSIDTransitions)
		return NAVDB.LoadSIDTransitions(AirportICAO, SIDName, TransitionNameList);

	return false;	// No procedure defined for this action in the navigation pack
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARTransitions()
// Description:			Loads the transition name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARTransitions(char *AirportICAO, char *STARName, vector<string> &TransitionNameList)
{
	if (NAVDB.LoadSTARTransitions)
		return NAVDB.LoadSTARTransitions(AirportICAO, STARName, TransitionNameList);

	return false;	// No procedure defined for this action in the navigation pack
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSIDPoints()
// Description			Loads the navigation point list of the defined SID procedure into the provided list
// Parameters	
//		AirportICAO		Airport structure
//		SIDName			SID name for the associated navigation points should be retrieved
//		RunwayName		Runway name
//		SIDPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

bool LoadSIDPoints(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList)
{
	if (NAVDB.LoadSIDPoints)
		return NAVDB.LoadSIDPoints(AirportICAO, SIDName, RunwayName, TranName, SIDPointList);

	return false;	// No procedure defined for this action in the navigation pack
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSTARPoints()
// Description			Loads the navigation point list of the defined STAR procedure into the provided list
// Parameters	
//		AirportICAO		Airport structure
//		STARName			STAR name for the associated navigation points should be retrieved
//		RunwayName		Runway name
//		STARPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

bool LoadSTARPoints(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList)
{
	if (NAVDB.LoadSTARPoints)
		return NAVDB.LoadSTARPoints(AirportICAO, STARName, RunwayName, TranName, STARPointList);

	return false;	// No procedure defined for this action in the navigation pack
}

// ----------------------------------------------------------------------------------
// Name:				LoadWaypoints()
// Description:			Reading waysponts data into the provided data structure list
// Parameters:
//		ints			Waypoint data structure vector to fill in
// Return:
//		Result status
// ----------------------------------------------------------------------------------

bool LoadWaypoints(vector<nvdb_wpt>& ints)
{
	if (NAVDB.LoadWaypoints)
		return NAVDB.LoadWaypoints(ints);

	return false;	// No procedure defined for this action in the navigation pack
}

// ---------------------------------------------------------------------------------
// Name:		GetNavDatabaseCode
// Description:	Returns the navigation database package identifier
// Parameters:
//		-
// Return:
//		Navigation database identifier
// Comments:
//		Controlled by parameters in the [KLN90B] section of the configuration file:
//			NAVDATA		Defines the package identifier or 0 if no external package to be used
//			NAVDIR		Base directory of the navigation package without the ending backslash
//		Parameters are checked when the GPS is loaded with the aircraft.
// ---------------------------------------------------------------------------------

static long NavDatabaseCode = -1;		// Signing uninitialized
static char NavDatabaseDir[MAX_PATH] = "";
static char *NavDatabasePack[] = { 
	"None", 
	"Traditional PTT navigation pack", 
	"EADT KLN 90B - native", 
	"VAS FMC Flight Management - native" 
};

DWORD GetNavDatabaseCode()
{
	DWORD code = 0;

	if (NavDatabaseCode < 0)
	{
		NavDatabaseCode = 0;			// Assume no external database is defined.
		if (K_load_dword_param("KLN90B", "NAVDATA", &code))
			if ((code > 0) && (code <= NAVDBSSUPPORTED))
				if (K_load_string_param("KLN90B", "NAVDIR", NavDatabaseDir))
					if (PathFileExists(NavDatabaseDir))
						NavDatabaseCode = code;
		K_DEBUG("[HG] Selected database package: %s\n", NavDatabasePack[NavDatabaseCode]);
	}

	return NavDatabaseCode;
}

// ---------------------------------------------------------------------------------
// Name:		GetNavDatabaseDir
// Description:	Returns database folder defined in the config file
// Parameters:
//		-
// Return:		
//		Folder as pointer to static variable
// Comment:
//		Variable initialized by GetNavDatabaseCode()
// ---------------------------------------------------------------------------------

char *GetNavDatabaseDir()
{
	return NavDatabaseDir;
}

