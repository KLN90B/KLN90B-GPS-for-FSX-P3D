// ----------------------------------------------------------------------------------
// navdata_4.cpp
//
// "vasFMC Flight Management" navigation package 
// See: https://www.navigraph.com/FmsDataManualInstall.aspx
//
// Routines to retrieve waypoint and SID/STAR data from the 
// Naigraph "vasFMC Flight Management" package files.
//
// SID file naming: SID\<ICAO>.txt
//
// SID files structure. 
//		Starts with SID then follows with the transition
//		Header: Type|Airport|Runway|SID|ID?|Itemnum
//		If there are no transitions, transition name is filled with the terminating waypoint name 
//
// P|PORTE7|01L|AVE|0|9
// S| |91000000|181000000| |CA|0|1400|0|0|0|0|1|420|0|0|0|0|0
// S| |91000000|181000000| |VI|0|33300|0|0|0|0|0|0|0|0|0|0|0
// S|SEPDY|37685667|-122363667|SFO|CF|0|35000|400|35000|300|0|1|1900|0|0|0|0|0
// S| |91000000|181000000| |VI|1|20300|0|0|0|0|0|0|0|0|0|0|0
// S|PORTE|37489786|-122474578|PYE|CF|0|13500|4000|13500|700|0|1|9000|0|0|0|0|0
// S|OSI|37392500|-122281300| |TF|0|0|0|0|0|0|0|0|0|0|0|0|0
// S|WAGES|36991034|-121737061| |TF|0|0|0|0|0|0|1|20000|0|0|0|0|0
// S|WAGES|36991034|-121737061| |IF|0|0|0|0|0|0|1|20000|0|0|0|0|0
// S|AVE|35646978|-119978606| |TF|0|0|0|0|0|0|0|0|0|0|0|0|0
//
// STAR file naming: STAR\<ICAO>.txt
//
// STAR files structure
//		IF signs starting of transition and then starting of STAR itself
//		Header: Type|Airport|Runway|SID|ID?|Itemnum
//		If there are no transitions, transition name is filled with the first waypoint name 
//
// P|ALWYS1|19R|INYOE|0|10
// S|INYOE|37895622|-118764992| |IF|0|0|0|0|0|0|0|0|0|0|0|0|0
// S|DYAMD|37699161|-120404428| |TF|0|0|0|0|0|0|1|27000|0|0|0|0|0
// S|DYAMD|37699161|-120404428| |IF|0|0|0|0|0|0|1|27000|0|0|0|0|0
// S|LAANE|37658983|-120747392| |TF|0|0|0|0|0|280|3|26000|22000|0|0|0|0
// S|ALWYS|37633420|-120959431| |TF|0|0|0|0|0|0|0|0|0|0|0|0|0
// S|HEFLY|37683853|-121231031| |TF|0|0|0|0|0|250|3|19000|14000|0|0|0|0
// S|ARRTU|37733622|-121502989| |TF|0|0|0|0|0|250|0|10000|0|0|0|0|0
// S|ADDMM|37771028|-121708586| |TF|0|0|0|0|0|240|0|9000|0|0|0|0|0
// S|COGGR|37818981|-121976145| |TF|0|0|0|0|0|210|3|9000|7000|0|0|0|0
// S|BERKS|37860861|-122211678| |TF|0|0|0|0|0|210|0|5000|0|0|0|0|0
//
// Procedure file value matching for sscanf(): 
//		INYOE			%8[^,]
//		37699161		%lf
//		-120404428		%lf
//
// Waypoint file name: Waypoints.txt
//
// Waypoint file structure:
// 
// BADOT|49428333|8191389|ED
// BADOV|48021111|18815833|LZ
// BADOW|17735683|-76313478|MK
// BADOX|39331945|33975278|LT
//
// Waypoint file value matching for sscanf(): 
//		BADOW			%5[^|]
//		17.735683		%lf
//		-76.313478		%lf
//		MK				%4s
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

#define WAYPOINTS_FILENAME "Waypoints.txt"

// Line parser callback function type
typedef bool(*PointReader_cb_t)(char *LineBuff, void *List);
typedef bool(*HeaderReader_cb_t)(char *LineBuff, char *ProcName, void *List);

// Package specific helper routines
static FILE *OpenNavDBFile(const char *type, const char *icao);
static bool FileReaderForPoints(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, void *List, PointReader_cb_t LineReader);
static bool FileReaderForHeaders(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, void *List, HeaderReader_cb_t LineReader);
static bool LoadProcNames(char *LineBuff, char *ProcName, void *ProcNameList);
static bool LoadTransitionNames(char *LineBuff, char *ProcName, void *TransitionNameList);
static bool LoadRunwayNames(char *LineBuff, char *ProcName, void *RunwayNameList);
static bool LoadTransitionPoints(char *LineBuff, char *ProcName, char *RunwayName, char* TranName, void *TransPointList);
static bool LoadProcPoints(char *LineBuff, void *ProcPointList);
static bool PointCompare(nv_hdr_t x, nv_hdr_t y);

// ==========================================================================================
// Package specific interface routines
// ==========================================================================================

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDNames_4()
// Description:			Loads the SID name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	SID names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_4(char *AirportICAO, vector<string> &ProcNameList)
{
	return(FileReaderForHeaders(AirportICAO, "", "", "", true, &ProcNameList, LoadProcNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARNames_4()
// Description:			Loads the STAR name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	STAR names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARNames_4(char *AirportICAO, vector<string> &ProcNameList)
{
	return(FileReaderForHeaders(AirportICAO, "", "", "", false, &ProcNameList, LoadProcNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDRunways_4()
// Description:			Loads the runway name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDRunways_4(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList)
{
	return(FileReaderForHeaders(AirportICAO, SIDName, "", "", true, &RunwayNameList, LoadRunwayNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARRunways_4()
// Description:			Loads the runway name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARRunways_4(char *AirportICAO, char *STARName, vector<string> &RunwayNameList)
{
	return(FileReaderForHeaders(AirportICAO, STARName, "", "", false, &RunwayNameList, LoadRunwayNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDTransitions_4()
// Description:			Loads the transition name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDTransitions_4(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList)
{
	return(FileReaderForHeaders(AirportICAO, SIDName, "", "", true, &TransitionNameList, LoadTransitionNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARTransitions_4()
// Description:			Loads the transition name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARTransitions_4(char *AirportICAO, char *STARName, vector<string> &TransitionNameList)
{
	return(FileReaderForHeaders(AirportICAO, STARName, "", "", false, &TransitionNameList, LoadTransitionNames));
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSIDPoints_4()
// Description			Loads the navigation point list of the defined SID procedure and transition
//						into the provided list
// Parameters	
//		AirportICAO		Airport structure
//		SIDName			SID name for the associated navigation points should be retrieved
//		RunwayName		Runway name
//		TranName		Transition name
//		SIDPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

bool LoadSIDPoints_4(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList)
{
	// Reads SID with transition
	if (!FileReaderForPoints(AirportICAO, SIDName, RunwayName, TranName, true, &SIDPointList, LoadProcPoints))
		return false;			// Error reading SID

	// Deletes duplicated consecutive points (possible SID - transition common point)
	// See algorithm desctription here: http://www.cplusplus.com/reference/algorithm/unique/
	std::vector<nv_hdr_t>::iterator it;											// Creates iterator
	it = std::unique(SIDPointList.begin(), SIDPointList.end(), PointCompare);	// Removes consecutive duplicates based on name
	SIDPointList.resize(std::distance(SIDPointList.begin(), it));				// Shrinks the vector to the reduced length

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSTARPoints_4()
// Description			Loads the navigation point list of the defined STAR transition end procedure
//						into the provided list
// Parameters	
//		AirportICAO		Airport structure
//		STARName			STAR name for the associated navigation points should be retrieved
//		RunwayName		Runway name
//		TranName		Transition name
//		STARPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

bool LoadSTARPoints_4(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList)
{
	// Reads STAR with transition
	if (!FileReaderForPoints(AirportICAO, STARName, RunwayName, TranName, false, &STARPointList, LoadProcPoints))
		return false;			// Error reading STAR

	// Deletes duplicated consecutive points (possible transition - STAR common point)
	// See algorithm desctription here: http://www.cplusplus.com/reference/algorithm/unique/
	std::vector<nv_hdr_t>::iterator it;											// Creates iterator
	it = std::unique(STARPointList.begin(), STARPointList.end(), PointCompare);	// Removes consecutive duplicates based on name
	STARPointList.resize(std::distance(STARPointList.begin(), it));				// Shrinks the vector to the reduced length

	return true;
}

// ----------------------------------------------------------------------------------
// Name:				LoadWaypoints_4()
// Description:			Reading waysponts data into the provided data structure list
// Parameters:
//		ints			Waypoint data structure vector to fill in
// Return:
//		Result status
// ----------------------------------------------------------------------------------

bool LoadWaypoints_4(vector<nvdb_wpt>& ints)
{
	char buff[MAX_PATH];
	FILE *f;
	nvdb_wpt wpt;

	f = OpenNavDBFile("WPT", "");
	if (!f)
		return false;

	while (1)
	{
		memset( &wpt, 0, sizeof(nvdb_wpt) );
		if (fgets(buff, MAX_PATH - 1, f) == NULL)
			break;		// End of file

		if( sscanf(buff, "%5[^|]|%lf|%lf|%5s", wpt.ICAO_ID, &wpt.Lat, &wpt.Lon, &wpt.REG_ID) < 3)
			continue;	// Faulty line. At least ICAO, latitude and longitude should be present
 
		wpt.Lat /= 1000000L;
		wpt.Lon /= 1000000L;

		ints.push_back(wpt);	// Store the point in the list.
	}

	fclose(f);
	return true;
}


// ==========================================================================================
// Package specific helper routines
// ==========================================================================================

// ------------------------------------------------------------------------------------------
// Name:		GetProcFileName
// Description:	Assembles SID/STAR file name based on airport ICAO
// Parameters:
//		type		nav db file type: "SID", "STAR" or "WPT"
//		icao		airport ICAO designation is type <> "WPT"
// Return:
//		Fully qualified procedure file name as pointer to static variable
// File naming:
//		SIDs				<DBDIR>\SID\<ICAO>.txt
//		STARs				<DBDIR>\STAR\<ICAO>.txt
//		Waypoints			<DBDIR>\Waypoints.txt
// ------------------------------------------------------------------------------------------

static FILE *OpenNavDBFile( const char *type, const char *icao)
{
	static char file_name[MAX_PATH];
	FILE *f;

	if (stricmp(type, "WPT") == 0)
		sprintf(file_name, "%s\\Waypoints.txt", GetNavDatabaseDir());
	else if (stricmp(type, "SID") == 0)
		sprintf(file_name, "%s\\SID\\%s.txt", GetNavDatabaseDir(), icao);
	else
		sprintf(file_name, "%s\\STAR\\%s.txt", GetNavDatabaseDir(), icao);


	return (fopen(file_name, "rb"));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadProcNames()
// Description:			Adds the procedure name to the provided name list if not duplicate.
// Parameters
//		LineBuff		A sigle line from the procedure file
//		ProcName		Not used (placeholder for the generalized line reader)
//		ProcNameList	Procedure names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadProcNames(char *LineBuff, char *ProcName, void *ProcNameList)
{
	char proc_name[10], rnw_name[10], tran_name[10];
	vector<string> *p = (vector<string>*) ProcNameList;

	// Segment heading as P|PORTE7|01L|AVE|0|9
	if (sscanf(LineBuff + 2, "%6[^|]|%6[^|]|%6[^|]/%6[^|]", proc_name, rnw_name, tran_name) < 3)
		return true;				// Unusable line. All 3 names should be present in a section header.

	// Save procedure name if not stored already
	if (find(p->begin(), p->end(), proc_name) == p->end())
		p->push_back(proc_name);

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadProcRunways()
// Description:			Adds the runway name to the provided name list if not duplicate.
// Parameters
//		LineBuff		A sigle line from the procedure file
//		ProcName		Name of procedure for the runways are looked for
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadRunwayNames(char *LineBuff, char *ProcName, void *RunwayNameList)
{
	char proc_name[10], rnw_name[10], tran_name[10];
	vector<string> *p = (vector<string>*) RunwayNameList;

	strcpy(rnw_name, "RW");		// Precede actual runway name with 'RW' as used in the BGL files

	// Segment heading as P|PORTE7|01L|AVE|0|9
	if (sscanf(LineBuff + 2, "%6[^|]|%6[^|]|%6[^|]/%6[^|]", proc_name, rnw_name + 2, tran_name) < 3)
		return true;				// Unusable line. All 3 names should be present in a section header.

	if (strcmp(proc_name, ProcName) != 0)
		return true;	// Not the searched procedure

	// Save runway name if not stored already
	if (find(p->begin(), p->end(), rnw_name) == p->end())
		p->push_back(rnw_name);

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadTransitionNames()
// Description:			Adds the transition name to the provided name list.
// Parameters
//		LineBuff		A sigle line from the procedure file
//		ProcName		Name of procedure for the runways are looked for
//		TransitionNameList	Transition names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadTransitionNames(char *LineBuff, char *ProcName, void *TransitionNameList)
{
	char proc_name[10], rnw_name[10], tran_name[10];
	vector<string> *p = (vector<string>*) TransitionNameList;

	// Segment heading as P|PORTE7|01L|AVE|0|9
	if (sscanf(LineBuff + 2, "%6[^|]|%6[^|]|%6[^|]/%6[^|]", proc_name, rnw_name, tran_name) < 3)
		return true;				// Unusable line. All 3 names should be present in a section header.

	if (strcmp(proc_name, ProcName) != 0)
		return true;	// Not the searched procedure

	// Save transition name if not stored already
	if (find(p->begin(), p->end(), tran_name) == p->end())
		p->push_back(tran_name);

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					LoadProcPoints()
// Description			Loads the navigation point into the provided list
// Parameters	
//		LineBuff		A sigle line from the procedure file
//		ProcPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

static bool LoadProcPoints(char *LineBuff, void *ProcPointList)
{
	vector<nv_hdr_t> *p = (vector<nv_hdr_t>*) ProcPointList;
	nv_hdr_t nav_point = { "", 0L, 0L };

	// S|SENZY|37666500|-122485167|...
	if (sscanf(LineBuff + 2, "%5[^|]|%lf|%lf", nav_point.ICAO_ID, &nav_point.Lat, &nav_point.Lon) < 3)
		return true;					// Unusable line. Point name and coordinates can't be read.
	if (nav_point.ICAO_ID[0] == ' ')	// Empty name (also lat=91.0 and lon=181.0) means special no-point instruction line 
		return true;					// We skip, but continue

	nav_point.Lat /= 1000000L;
	nav_point.Lon /= 1000000L;

	// Save point data
	p->push_back(nav_point);

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					FileReaderForHeaders()
// Description			
// Parameters	
//		AirportICAO		Airport structure
//		ProcName		Procedure name 
//		RunwayName		Runway name
//		TranName		Transition name
//		isSID			true for SID, false for STAR
//		List			Data structure list where the information should be returned
//		LineReader		Line parser function pointer
// Return	
//		True if continue; false if stop reading file		
// -------------------------------------------------------------------------------------------------

static bool FileReaderForHeaders(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, void *List, HeaderReader_cb_t LineReader)
{
	char LineBuff[MAX_PATH];
	FILE *f;

	f = OpenNavDBFile(isSID ? "SID" : "STAR", AirportICAO);
	if (!f)
	{
		return false;	// No procedure file found
	}

	while (true)			// Search for the appropriate section based on airport, procedure, runway and transition
	{
		if (fgets(LineBuff, MAX_PATH - 1, f) == NULL)
			break;			// End of file

		if (LineBuff[0] != 'P')		// Not segment heading
			continue;				// Go on...

		if (LineReader(LineBuff, ProcName, List))
			continue;
		else
			break;
	}

	fclose(f);
	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					FileReaderForPoints()
// Description			
// Parameters	
//		AirportICAO		Airport structure
//		ProcName		Procedure name 
//		RunwayName		Runway name
//		TranName		Transition name
//		isSID			true for SID, false for STAR
//		List			Data structure list where the information should be returned
//		LineReader		Line parser function pointer
// Return	
//		True if continue; false if stop reading file		
// -------------------------------------------------------------------------------------------------

static bool FileReaderForPoints(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, void *List, PointReader_cb_t LineReader)
{
	char LineBuff[MAX_PATH];
	char proc_name[MAX_PATH], rnw_name[MAX_PATH], tran_name[MAX_PATH];
	FILE *f;
	bool success = true;

	f = OpenNavDBFile(isSID ? "SID" : "STAR", AirportICAO);
	if (!f)
	{
		return false;	// No procedure file found.
	}

	while (true)			// Search for the appropriate section based on airport, procedure, runway and transition
	{
		if (fgets(LineBuff, MAX_PATH - 1, f) == NULL)
		{
			success = false;		// End of file and we hadn't found the section
			break;
		}

		if (LineBuff[0] != 'P')		// Not segment heading
			continue;				// Go on...

		// Segment heading as P|PORTE7|01L|AVE|0|9
		if (sscanf(LineBuff + 2, "%6[^|]|%6[^|]|%6[^|]/%6[^|]", proc_name, rnw_name, tran_name) < 3)
			continue;				// Unusable line. All 3 names and counter should be present in a section header.

		if ((strcmp(proc_name, ProcName) == 0) &&
			(strcmp(rnw_name, RunwayName) == 0) &&						// skip "RW" from the runway designator
			((!*TranName) || (strcmp(tran_name, TranName) == 0)))		// the transition might be empty
			break;						// Found the appropriate section
	}

	while (success)						// Search for the points within the section
	{
		if (fgets(LineBuff, MAX_PATH - 1, f) == NULL)
			break;						// End of file

		if (LineBuff[0] != 'S')			// Each procedure point line starts with a 'S' field
			break;						// End of procedure segment

		success = LineReader(LineBuff, List);
	}

	fclose(f);
	return success;
}

// -------------------------------------------------------------------------------------------------
// Name					PointCompare()
// Description			Compares two navigation points based on their ICAO name		
// Parameters	
//		x, y			Naigation points to compare
// Return	
//		True if the two points hase the same ICAO name	
// Comment
//		Used as a predicate function in a vector unique operation.
//		Could be changed to compare coordinates as well, but this is enough here.
// -------------------------------------------------------------------------------------------------

static bool PointCompare(nv_hdr_t x, nv_hdr_t y)
{
	return (strcmp(x.ICAO_ID, y.ICAO_ID) == 0); 
};

