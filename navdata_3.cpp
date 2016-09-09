// ----------------------------------------------------------------------------------
// navdata_3.cpp
//
// "PSS Airbus/Boeing/Dash" navigation package 
// See: https://www.navigraph.com/FmsDataManualInstall.aspx
//
// Routines to retrieve waypoint and SID/STAR data from the 
// Naigraph "PSS Airbus/Boeing/Dash" package files.
//
// SID file naming: PSSSID.dat
//
// SID files structure. 
//		Starts with SID then follows with the transition
//		Header: [Airport/SID/Runway/Transition]
//		If there are no transitions, transition name is filled with the terminating waypoint name 
//
// [KSFO/OFFSH9/RW01L /GVO]
// 1,   GVO,010,      , 91.000000, 181.000000,    ,CA, ,014,000.000,000.0,000.0, 0.000000,000.0,000,+,00420,00000,18000
// 1,   GVO,020,      , 91.000000, 181.000000,    ,VI, ,333,000.000,000.0,000.0, 0.000000,000.0,000, ,00000,00000,00000
// 1,   GVO,030, SEPDY, 37.685667,-122.363667,SFO ,CF, ,350,000.000,004.0,350.0, 0.000000,003.0,000,+,01600,00000,00000
// 1,   GVO,040,      , 91.000000, 181.000000,    ,VI,L,203,000.000,000.0,000.0, 0.000000,000.0,000, ,00000,00000,00000
// 1,   GVO,050, WAMMY, 37.541064,-122.724056,PYE ,CF, ,151,000.000,033.0,151.0, 0.000000,003.0,000, ,00000,00000,00000
// 1,   GVO,060, SEGUL, 36.963036,-122.572131,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,000,+,16000,00000,00000
// 2,   GVO,010, SEGUL, 36.963036,-122.572131,    ,IF, ,000,000.000,000.0,000.0, 0.000000,000.0,000,+,16000,00000,18000
// 2,   GVO,020, CYPRS, 36.422336,-122.432181,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,000,+,22000,00000,00000
// 2,   GVO,030, MCKEY, 35.483095,-121.083139,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,000, ,00000,00000,00000
// 3,   GVO,010, MCKEY, 35.483095,-121.083139,    ,IF, ,000,000.000,000.0,000.0, 0.000000,000.0,000, ,00000,00000,18000
// 3,   GVO,020,   MQO, 35.252256,-120.759567,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,000, ,00000,00000,00000
// 3,   GVO,030,   GVO, 34.531320,-120.091089,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,000, ,00000,00000,00000
//
// STAR file naming: PSSSTAR.dat
//
// STAR files structure
//		IF signs starting of transition and then starting of STAR itself
//		Header: [Airport/SID/Runway/Transition]
//		If there are no transitions, transition name is filled with the first waypoint name 
//
// [KSFO/STLER1/RW19L /PEENO]
// 4, PEENO,010, PEENO, 38.570258,-123.615533,    ,IF, ,000,000.000,000.0,000.0, 0.000000,000.0,000,+,28000,00000,18000
// 4, PEENO,020, DEEAN, 38.349164,-123.302289,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,280,B,27000,24000,00000
// 4, PEENO,030, LOZIT, 37.899325,-122.673195,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,250,-,11000,00000,00000
// 5, PEENO,010, LOZIT, 37.899325,-122.673195,    ,IF, ,000,000.000,000.0,000.0, 0.000000,000.0,250,-,11000,00000,18000
// 5, PEENO,020, STLER, 37.701364,-122.711064,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,000, ,00000,00000,00000
// 5, PEENO,030, WESLA, 37.664372,-122.480292,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,210, ,07000,00000,00000
// 5, PEENO,040, MVRKK, 37.736972,-122.454450,    ,TF, ,000,000.000,000.0,000.0, 0.000000,000.0,000, ,06000,00000,00000
// 5, PEENO,050, MVRKK, 37.736972,-122.454450,SFO ,FM, ,360,000.000,008.0,314.4, 0.000000,000.0,000, ,00000,00000,00000
//
// Procedure file value matching for sscanf(): 
//		4				%*1d
//		PEENO			%8[^,]
//		020				%*3d
//		DEEAN			%8[^,]
//		+47.376639		%lf
//		+019.368361		%lf
//
// Waypoint file name: PSSWPT.dat
//
// Waypoint file structure:
// 
// BADOT 49.428333    8.191389
// BADOV 48.021111   18.815833
// BADOW 17.735683  -76.313478
// BADOX 39.331945   33.975278
//
// Waypoint file value matching for sscanf(): 
//		BADOW			%5[^|]
//		17.735683		%lf
//		-76.313478		%lf
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
typedef bool(*HeaderReader_cb_t)(char *LineBuff, char *AirportICAO, char *ProcName, void *List);

// Package specific helper routines
static FILE *OpenNavDBFile(const char *type);
static bool FileReaderForPoints(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, void *List, PointReader_cb_t LineReader);
static bool FileReaderForHeaders(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, void *List, HeaderReader_cb_t LineReader);
static bool LoadProcNames(char *LineBuff, char *AirportICAO, char *ProcName, void *ProcNameList);
static bool LoadTransitionNames(char *LineBuff, char *AirportICAO, char *ProcName, void *TransitionNameList);
static bool LoadRunwayNames(char *LineBuff, char *AirportICAO, char *ProcName, void *RunwayNameList);
static bool LoadProcPoints(char *LineBuff, void *ProcPointList);
static bool PointCompare(nv_hdr_t x, nv_hdr_t y);
static char* RightTrim(char *s);

// ==========================================================================================
// Package specific interface routines
// ==========================================================================================

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDNames_3()
// Description:			Loads the SID name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	SID names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_3(char *AirportICAO, vector<string> &ProcNameList)
{
	return(FileReaderForHeaders(AirportICAO, "", "", "", true, &ProcNameList, LoadProcNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARNames_3()
// Description:			Loads the STAR name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	STAR names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARNames_3(char *AirportICAO, vector<string> &ProcNameList)
{
	return(FileReaderForHeaders(AirportICAO, "", "", "", false, &ProcNameList, LoadProcNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDRunways_3()
// Description:			Loads the runway name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDRunways_3(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList)
{
	return(FileReaderForHeaders(AirportICAO, SIDName, "", "", true, &RunwayNameList, LoadRunwayNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARRunways_3()
// Description:			Loads the runway name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARRunways_3(char *AirportICAO, char *STARName, vector<string> &RunwayNameList)
{
	return(FileReaderForHeaders(AirportICAO, STARName, "", "", false, &RunwayNameList, LoadRunwayNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDTransitions_3()
// Description:			Loads the transition name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDTransitions_3(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList)
{
	return(FileReaderForHeaders(AirportICAO, SIDName, "", "", true, &TransitionNameList, LoadTransitionNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARTransitions_3()
// Description:			Loads the transition name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARTransitions_3(char *AirportICAO, char *STARName, vector<string> &TransitionNameList)
{
	return(FileReaderForHeaders(AirportICAO, STARName, "", "", false, &TransitionNameList, LoadTransitionNames));
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSIDPoints_3()
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

bool LoadSIDPoints_3(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList)
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
// Name					LoadSTARPoints_3()
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

bool LoadSTARPoints_3(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList)
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
// Name:				LoadWaypoints_3()
// Description:			Reading waysponts data into the provided data structure list
// Parameters:
//		ints			Waypoint data structure vector to fill in
// Return:
//		Result status
// ----------------------------------------------------------------------------------

bool LoadWaypoints_3(vector<nvdb_wpt>& ints)
{
	char buff[MAX_PATH];
	FILE *f;
	nvdb_wpt wpt;

	f = OpenNavDBFile("WPT");
	if (!f)
		return false;

	while (1)
	{
		memset( &wpt, 0, sizeof(nvdb_wpt) );
		if (fgets(buff, MAX_PATH - 1, f) == NULL)
			break;		// End of file

		if( sscanf(buff, "%5s%lf%lf", wpt.ICAO_ID, &wpt.Lat, &wpt.Lon) < 3)
			continue;	// Faulty line. At least ICAO, latitude and longitude should be present
 
		RightTrim(wpt.ICAO_ID);
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
// Return:
//		Fully qualified procedure file name as pointer to static variable
// File naming:
//		SIDs				<DBDIR>\PSSSID.dat
//		STARs				<DBDIR>\PSSSTAR.dat
//		Waypoints			<DBDIR>\PSSWPT.dat
// ------------------------------------------------------------------------------------------

static FILE *OpenNavDBFile( const char *type)
{
	static char file_name[MAX_PATH];
	FILE *f;

	sprintf(file_name, "%s\\PSS%s.dat", GetNavDatabaseDir(), type);
	return (fopen(file_name, "rb"));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadProcNames()
// Description:			Loads the procedure name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
//		LineBuff		A sigle line from the procedure file
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcName		Not used (placeholder for the generalized line reader)
//		ProcNameList	Procedure names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadProcNames(char *LineBuff, char *AirportICAO, char *ProcName, void *ProcNameList)
{
	char apt_name[10], proc_name[10], rnw_name[10], tran_name[10];
	vector<string> *p = (vector<string>*) ProcNameList;

	// Segment heading as [KSFO/ALWYS1/RW19L /INYOE]
	if (sscanf(LineBuff + 1, "%6[^/]/%6[^/]/%6[^/]/%6[^]]]", apt_name, proc_name, rnw_name, tran_name) < 4)
		return true;				// Unusable line. All four name should be present in a section header.

	RightTrim(proc_name);

	if (strcmp(apt_name, AirportICAO) != 0)
		return true;	// Not the searched airport

	// Save procedure name if not stored already
	if (find(p->begin(), p->end(), proc_name) == p->end())
		p->push_back(proc_name);

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadProcRunways()
// Description:			Loads the runway name list for a procedure into the provided name list.
// Parameters
//		LineBuff		A sigle line from the procedure file
// 		AirportICAO		ICAO code of the airport for which the runway names should be retrieved
//		ProcName		Procedure name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadRunwayNames(char *LineBuff, char *AirportICAO, char *ProcName, void *RunwayNameList)
{
	char apt_name[10], proc_name[10], rnw_name[10], tran_name[10];
	vector<string> *p = (vector<string>*) RunwayNameList;

	// Segment heading as [KSFO/ALWYS1/RW19L /INYOE]
	if (sscanf(LineBuff + 1, "%6[^/]/%6[^/]/%6[^/]/%6[^]]]", apt_name, proc_name, rnw_name, tran_name) < 4)
		return true;				// Unusable line. All four name should be present in a section header.

	RightTrim(proc_name);
	RightTrim(rnw_name);

	if ((strcmp(apt_name, AirportICAO) != 0) || (strcmp(proc_name, ProcName) != 0))
		return true;	// Not the searched airport - procedure pair

	// Save runway name if not stored already
	if (find(p->begin(), p->end(), rnw_name) == p->end())
		p->push_back(rnw_name);

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadTransitionRunways()
// Description:			Loads the transition name list for a procedure into the provided name list.
// Parameters
//		LineBuff		A sigle line from the procedure file
// 		AirportICAO		ICAO code of the airport for which the transition names should be retrieved
//		ProcName		Procedure name for which the associated transition(s)should be retrieved
//		TransitionNameList	Transition names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadTransitionNames(char *LineBuff, char *AirportICAO, char *ProcName, void *TransitionNameList)
{
	char apt_name[10], proc_name[10], rnw_name[10], tran_name[10];
	vector<string> *p = (vector<string>*) TransitionNameList;

	// Segment heading as [KSFO/ALWYS1/RW19L /INYOE]
	if (sscanf(LineBuff + 1, "%6[^/]/%6[^/]/%6[^/]/%6[^]]]", apt_name, proc_name, rnw_name, tran_name) < 4)
		return true;				// Unusable line. All four name should be present in a section header.

	if ((strcmp(apt_name, AirportICAO) != 0) || (strcmp(proc_name, ProcName) != 0))
		return true;	// Not the searched procedure

	// Save transition name if not stored already
	if (find(p->begin(), p->end(), tran_name) == p->end())
		p->push_back(tran_name);

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					LoadProcPoints()
// Description			LLoads the navigation point into the provided list
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
	double lat, lon;
	char* point_name;

	// Line codes values in fix width fields
	// 0123456789012345678901234567890123456789012
	//   xxxxxx     xxxxxx xxxxxxxxxx xxxxxxxxxxx
	// 1,   MWH,010,   MWH, 47.210864,-119.316817,...
	LineBuff[19] = '\0';		// end of waypoint
	LineBuff[30] = '\0';		// end of latitude
	LineBuff[42] = '\0';		// end of longitude
	for (point_name = LineBuff + 13; *point_name == ' '; point_name++);		// skips leading spaces
	lat = atof(LineBuff + 20);
	lon = atof(LineBuff + 31);
	if (point_name[0] == '\0')		// Empty name (also lat=91.0 and lon=181.0) means special no-point instruction line 
		return true;				// We skip, but continue

	// Save point data
	strcpy(nav_point.ICAO_ID, point_name);
	nav_point.Lat = lat;
	nav_point.Lon = lon;
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

	f = OpenNavDBFile(isSID ? "SID" : "STAR");
	if (!f)
	{
		return false;	// No procedure file found
	}

	while (true)			// Search for the appropriate section based on airport, procedure, runway and transition
	{
		if (fgets(LineBuff, MAX_PATH - 1, f) == NULL)
			break;			// End of file

		if (LineBuff[0] != '[')		// Not segment heading
			continue;				// Go on...

		if (LineReader(LineBuff, AirportICAO, ProcName, List))
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
	char apt_name[MAX_PATH], proc_name[MAX_PATH], rnw_name[MAX_PATH], tran_name[MAX_PATH];
	FILE *f;
	bool success = true;

	f = OpenNavDBFile(isSID ? "SID" : "STAR");
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

		if (LineBuff[0] != '[')		// Not segment heading
			continue;				// Go on...

		// Segment heading as [KSFO/ALWYS1/RW19L /INYOE]
		if (sscanf(LineBuff + 1, "%6[^/]/%6[^/]/%6[^/]/%6[^]]]", apt_name, proc_name, rnw_name, tran_name) < 4)
			continue;				// Unusable line. All four name should be present in a section header.

		RightTrim(proc_name);
		RightTrim(rnw_name);

		if ((strcmp(apt_name, AirportICAO) == 0) &&
			(strcmp(proc_name, ProcName) == 0) &&
			(strcmp(rnw_name + 2, RunwayName) == 0) &&					// skip "RW" from the runway designator
			((!*TranName) || (strcmp(tran_name, TranName) == 0)))		// the transition might be empty
			break;						// Found the appropriate section
	}

	while (success)						// Search for the points within the section
	{
		if (fgets(LineBuff, MAX_PATH - 1, f) == NULL)
			break;						// End of file

		if (!isdigit(LineBuff[0]))		// Each procedure point line starts with a point type number
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

// -------------------------------------------------------------------------------------------------
// Name:		RightTrim
// Description:	Deletes white space characters from the end of the string
// Parameters:
//		s		Pointer to the string to trim
// Return:
//		s		The same pointer to the trimmed string
// -------------------------------------------------------------------------------------------------

static char* RightTrim(char *s)
{
	int i = strlen(s);

	// Shorten the string until first non blank character or empty.
	while (i-- > 0)
	{
		switch (s[i])
		{
		case ' ':
		case '\t':
			s[i] = '\0';
			continue;		// Continues if blank was found
		}
		break;				// Otherwise breaks the loop
	}

	return s;
}