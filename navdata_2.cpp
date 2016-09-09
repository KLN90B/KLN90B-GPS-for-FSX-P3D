// ----------------------------------------------------------------------------------
// navdata_2.cpp
//
// "EADT KLN 90B - native" navigation package 
// See: https://www.navigraph.com/FmsDataManualInstall.aspx
//
// Routines to retrieve waypoint and SID/STAR data from the 
// Naigraph "EADT KLN 90B - native" package files.
//
// SID file naming:  Sids\<ICAO>.sid
// STAR file naming: Stars\<ICAO>.star
//
// SID and STAR files structure
//
// BADO1D,13L,,,,0,1,128,1,0,0,0,0,0,1,900,000,0.0,000,000,0,
// BADO1D,13L,BP713,+47.376639,+019.368361,1,1,000,0,0,0,0,0,0,0,0,000,0.0,000,000,0,
// BADO1D,13L,BP715,+47.609806,+019.594472,0,2,L016,0,0,0,2,0,6000,0,0,000,0.0,000,000,0,
// BADO1D,13L,BP718,+47.758167,+019.354806,0,0,000,0,0,0,2,0,8000,0,0,000,0.0,000,000,0,
//
// Procedure file value matching for sscanf(): 
//		BADO1D			%8[^,]
//		13L				%8[^,]
//		BP713			%8[^,]
//		+47.376639		%lf
//		+019.368361		%lf
//
// SID transition file naming:	Sids\<ICAO>.sidtrs
// STAR transition file naming: Stars\<ICAO>.startrs
//
// Transition file structure
//
// DEDHD,AFIVA1,JAYKK,+38.056531,-122.261819,0,0,000,0,0,0,0,0,0,0,0,000,0.0,000,000,0,
// DEDHD,AFIVA1,DEDHD,+38.335517,-122.112808,0,0,000,0,0,0,0,0,0,0,0,000,0.0,000,000,0,
// GRTFL,AFIVA1,JAYKK,+38.056531,-122.261819,0,0,000,0,0,0,0,0,0,0,0,000,0.0,000,000,0,
//
// Waypoint file name: Waypoints.txt
//
// Waypoint file structure:
// 
// BADOT | 49428333 | 8191389 | ED
// BADOV | 48021111 | 18815833 | LZ
// BADOW | 17735683 | -76313478 | MK
// BADOX | 39331945 | 33975278 | LT
//
// Waypoint file value matching for sscanf(): 
//		BADOW			%5[^|]
//		17735683		%lf
//		-76313478		%lf
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
typedef bool(*Reader_cb_t)(char *LineBuff, char *ProcName, char *RunwayName, char *TranName, void *List);

// Package specific helper routines
static char *GetProcFileName(char *icao_id, bool isSID);
static bool FileReader(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, bool isTrans, void *List, Reader_cb_t Reader);
static bool LoadProcNames(char *LineBuff, char *ProcName, char *RunwayName, char* TranName, void *ProcNameList);
static bool LoadTransitionNames(char *LineBuff, char *ProcName, char *RunwayName, char* TranName, void *TransitionNameList);
static bool LoadRunwayNames(char *LineBuff, char *ProcName, char *RunwayName, char* TranName, void *RunwayNameList);
static bool LoadTransitionPoints(char *LineBuff, char *ProcName, char *RunwayName, char* TranName, void *TransPointList);
static bool LoadProcPoints(char *LineBuff, char *ProcName, char *RunwayName, char* TranName, void *ProcPointList);
static bool PointCompare(nv_hdr_t x, nv_hdr_t y);

// ==========================================================================================
// Package specific interface routines
// ==========================================================================================

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDNames_2()
// Description:			Loads the SID name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	SID names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_2(char *AirportICAO, vector<string> &ProcNameList)
{
	return(FileReader(AirportICAO, "", "", "", true, false, &ProcNameList, LoadProcNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARNames_2()
// Description:			Loads the STAR name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		ProcNameList	STAR names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARNames_2(char *AirportICAO, vector<string> &ProcNameList)
{
	return(FileReader(AirportICAO, "", "", "", false, false, &ProcNameList, LoadProcNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDRunways_2()
// Description:			Loads the runway name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDRunways_2(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList)
{
	return(FileReader(AirportICAO, SIDName, "", "", true, false, &RunwayNameList, LoadRunwayNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARRunways_2()
// Description:			Loads the runway name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARRunways_2(char *AirportICAO, char *STARName, vector<string> &RunwayNameList)
{
	return(FileReader(AirportICAO, STARName, "", "", false, false, &RunwayNameList, LoadRunwayNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSIDTransitions_2()
// Description:			Loads the transition name list for a SID into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		SIDName			SID name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSIDTransitions_2(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList)
{
	return(FileReader(AirportICAO, SIDName, "", "", true, true, &TransitionNameList, LoadTransitionNames));
}

// ------------------------------------------------------------------------------------------
// Name:				LoadSTARTransitions_2()
// Description:			Loads the transition name list for a STAR into the provided name list.
// Parameters
// 		AirportICAO		ICAO code of the airport for which the procedure names should be retrieved
//		STARName		STAR name for which the associated runway(s)should be retrieved
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

bool LoadSTARTransitions_2(char *AirportICAO, char *STARName, vector<string> &TransitionNameList)
{
	return(FileReader(AirportICAO, STARName, "", "", false, true, &TransitionNameList, LoadTransitionNames));
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSIDPoints_2()
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

bool LoadSIDPoints_2(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList)
{
	// Reads SID
	if (!FileReader(AirportICAO, SIDName, RunwayName, TranName, true, false, &SIDPointList, LoadProcPoints))
		return false;			// Error reading SID
	// Reads transition points if transition is defined
	if (strlen(TranName) != 0)
		if (!FileReader(AirportICAO, SIDName, RunwayName, TranName, true, true, &SIDPointList, LoadTransitionPoints))
			return false;		// Error reading transition

	// Deletes duplicated consecutive points (possible SID - transition common point)
	// See algorithm desctription here: http://www.cplusplus.com/reference/algorithm/unique/
	std::vector<nv_hdr_t>::iterator it;											// Creates iterator
	it = std::unique(SIDPointList.begin(), SIDPointList.end(), PointCompare);	// Removes consecutive duplicates based on name
	SIDPointList.resize(std::distance(SIDPointList.begin(), it));				// Shrinks the vector to the reduced length

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					LoadSTARPoints_2()
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

bool LoadSTARPoints_2(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList)
{
	// Reads transition points if transition is defined
	if (strlen(TranName) != 0)
		if (!FileReader(AirportICAO, STARName, RunwayName, TranName, false, true, &STARPointList, LoadTransitionPoints))
			return false;		// Error reading transition
	// Reads STAR
	if (!FileReader(AirportICAO, STARName, RunwayName, TranName, false, false, &STARPointList, LoadProcPoints))
		return false;			// Error reading STAR

	// Deletes duplicated consecutive points (possible transition - STAR common point)
	// See algorithm desctription here: http://www.cplusplus.com/reference/algorithm/unique/
	std::vector<nv_hdr_t>::iterator it;											// Creates iterator
	it = std::unique(STARPointList.begin(), STARPointList.end(), PointCompare);	// Removes consecutive duplicates based on name
	STARPointList.resize(std::distance(STARPointList.begin(), it));				// Shrinks the vector to the reduced length

	return true;
}

// ----------------------------------------------------------------------------------
// Name:				LoadWaypoints_2()
// Description:			Reading waysponts data into the provided data structure list
// Parameters:
//		ints			Waypoint data structure vector to fill in
// Return:
//		Result status
// ----------------------------------------------------------------------------------

bool LoadWaypoints_2(vector<nvdb_wpt>& ints)
{
	char buff[MAX_PATH];
	char file_name[MAX_PATH];
	FILE *f;
	nvdb_wpt wpt;

	sprintf(file_name, "%s\\%s", GetNavDatabaseDir(), WAYPOINTS_FILENAME);
	K_DEBUG("[HG] Waypoint file name: %s\n", file_name);

	f = fopen(file_name, "rb");
	if (!f)
	{
		K_DEBUG("[HG] Error opening waypoint file %s in LoadWaypoints_2()\n", file_name);
		return false;
	}
	while (1)
	{
		memset( &wpt, 0, sizeof(nvdb_wpt) );
		if (fgets(buff, MAX_PATH - 1, f) == NULL)
			break;		// End of file

		if( sscanf(buff, "%5[^|]|%lf|%lf|%4s", wpt.ICAO_ID, &wpt.Lat, &wpt.Lon, wpt.REG_ID) < 3)
			continue;	// Faulty line. At least ICAO, latitude and longitude should be present
 
		wpt.Lat /= 1000000L;	// The coordinates are stored as multiplied integers in the file.
		wpt.Lon /= 1000000L;	// Let's decode by dividing them by 1 000 000.
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
//		icao_id		Airport ICAO code
//		isSID		Signes if SID (true) or STAR (false)
// Return:
//		Fully qualified procedure file name as pointer to static variable
// Procedure file naming:
//		SIDs				<DBDIR>\Sids\<ICAO>.sid
//		DID transitions		<DBDIR>\Sids\<ICAO>.sidtrs
//		STARs				<DBDIR>\Stars\<ICAO>.star
//		STAR transitions	<DBDIR>\Stars\<ICAO>.startrs
// ------------------------------------------------------------------------------------------

static char *GetProcFileName(char *icao_id, bool isSID, bool isTrans)
{
	static char file_name[MAX_PATH];

	if (isSID)		// SID
		sprintf(file_name, "%s\\Sids\\%s.sid%s", GetNavDatabaseDir(), icao_id, isTrans ? "trs" : "");
	else			// STAR
		sprintf(file_name, "%s\\Stars\\%s.star%s", GetNavDatabaseDir(), icao_id, isTrans ? "trs" : "");

	K_DEBUG("[HG] Procedure file name: %s\n", file_name);
	return(file_name);
}

// ------------------------------------------------------------------------------------------
// Name:				LoadProcNames()
// Description:			Loads the procedure name list of the airport defined in the ICAO_ID member 
//						into the provided name list.
// Parameters
//		LineBuff		A sigle line from the procedure file
//		ProcName		Not used (placeholder for the generalized line reader)
//		RunwayName		Not used (placeholder for the generalized line reader)
//		TranName		Not used (placeholder for the generalized line reader)
//		ProcNameList	Procedure names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadProcNames(char *LineBuff, char *ProcName, char *RunwayName, char *TranName, void *ProcNameList)
{
	char proc_name[10];
	vector<string> *p = (vector<string>*) ProcNameList;

	if (sscanf(LineBuff, "%8[^,]", proc_name) < 1)
		return true;	// Faulty line. Procedure name can't be read.

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
//		ProcName		Procedure name for which the associated runway(s)should be retrieved
//		RunwayName		Not used (placeholder for the generalized line reader)
//		TranName		Not used (placeholder for the generalized line reader)
//		RunwayNameList	Runway names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadRunwayNames(char *LineBuff, char *ProcName, char *RunwayName, char *TranName, void *RunwayNameList)
{
	char runway_name[10], proc_name[10];
	vector<string> *p = (vector<string>*) RunwayNameList;

	strcpy(runway_name, "RW");		// Precede actual runway name with 'RW' as used in the BGL files

	if (sscanf(LineBuff, "%8[^,],%8[^,]", proc_name, runway_name + 2) < 2)
		return true;	// Faulty line. Procedure name and runway name can't be read.
	if (strcmp(proc_name, ProcName) != 0)
		return true;	// Not the searched procedure

	// Save runway name if not stored already
	if (find(p->begin(), p->end(), runway_name) == p->end())
		p->push_back(runway_name);

	return true;
}

// ------------------------------------------------------------------------------------------
// Name:				LoadTransitionNames()
// Description:			Loads the transition name list for a procedure into the provided name list.
// Parameters
//		LineBuff		A sigle line from the procedure file
//		ProcName		Procedure name for which the associated transition(s)should be retrieved
//		RunwayName		Not used (placeholder for the generalized line reader)
//		TranName		Not used (placeholder for the generalized line reader)
//		TransitionNameList	Transition names list returned
// Return:
//		Success status
// ------------------------------------------------------------------------------------------

static bool LoadTransitionNames(char *LineBuff, char *ProcName, char *RunwayName, char *TranName, void *TransitionNameList)
{
	char tran_name[10], proc_name[10];
	vector<string> *p = (vector<string>*) TransitionNameList;

	if (sscanf(LineBuff, "%8[^,],%8[^,]", tran_name, proc_name) < 2)
		return true;	// Faulty line. Procedure name and transition name can't be read.
	if (strcmp(proc_name, ProcName) != 0)
		return true;	// Not the searched procedure

	// Save transition name if not stored already
	if (find(p->begin(), p->end(), tran_name) == p->end())
		p->push_back(tran_name);

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					LoadTransitionPoints()
// Description			Loads the transition point list of the defined procedure into the provided list
// Parameters	
//		LineBuff		A sigle line from the procedure file
//		ProcName		Procedure name for the associated navigation points should be retrieved
//		RunwayName		Not used (placeholder for the generalized line reader)
//		TranName		Runway name for the associated navigation points should be retrieved
//		ProcPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

static bool LoadTransitionPoints(char *LineBuff, char *ProcName, char *RunwayName, char *TranName, void *TransPointList)
{
	char tran_name[10], proc_name[10];
	vector<nv_hdr_t> *p = (vector<nv_hdr_t>*) TransPointList;
	nv_hdr_t nav_point = { "", 0L, 0L };

	// Transition,SID,Point,Lat,Lon,...
	// Transition, STAR,Point, Lat,Lon,...
	// RZS,OFFSH9,MCKEY,+35.483095,-121.083139,...
	if (sscanf(LineBuff, "%8[^,],%8[^,],%8[^,],%lf,%lf", tran_name, proc_name, nav_point.ICAO_ID, &nav_point.Lat, &nav_point.Lon) < 5)
		return true;	// Unusable line. Point name and coordinates can't be read.
	if ((strcmp(proc_name, ProcName) != 0) || (strcmp(tran_name, TranName) != 0))
		return true;	// Not the searched procedure - transition pair

	// Save point data
	p->push_back(nav_point);

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					LoadProcPoints()
// Description			Loads the navigation point list of the defined procedure into the provided list
// Parameters	
//		LineBuff		A sigle line from the procedure file
//		ProcName		Procedure name for the associated navigation points should be retrieved
//		RunwayName		Runway name for the associated navigation points should be retrieved
//		TranName		Not used (placeholder for the generalized line reader)
//		ProcPointList	Data structure list where the point information should be returned
// Return	
//		Success status		
// -------------------------------------------------------------------------------------------------

static bool LoadProcPoints(char *LineBuff, char *ProcName, char *RunwayName, char *TranName, void *ProcPointList)
{
	char runway_name[10], proc_name[10];
	vector<nv_hdr_t> *p = (vector<nv_hdr_t>*) ProcPointList;
	nv_hdr_t nav_point = { "", 0L, 0L };

	// BADO1D,13L,BP713,+47.376639,+019.368361,...
	if (sscanf(LineBuff, "%8[^,],%8[^,],%8[^,],%lf,%lf", proc_name, runway_name, nav_point.ICAO_ID, &nav_point.Lat, &nav_point.Lon) < 5)
		return true;	// Unusable line. Point name and coordinates can't be read.
	if (!((strcmp(proc_name, ProcName) == 0) && (strcmp(runway_name, RunwayName) == 0)))
		return true;	// Not the searched procedure - runway pair

	// Save point data
	p->push_back(nav_point);

	return true;
}

// -------------------------------------------------------------------------------------------------
// Name					FileReader()
// Description			
// Parameters	
//		AirportICAO		Airport structure
//		ProcName		Procedure name 
//		RunwayName		Runway name
//		TranName		Transition name
//		isSID			true for SID, false for STAR
//		isTrans			true for transition, false for SID/STAR procedures
//		List			Data structure list where the information should be returned
//		LineReader		Line parser function pointer
// Return	
//		True if continue; false if stop reading file		
// -------------------------------------------------------------------------------------------------

static bool FileReader(char *AirportICAO, char *ProcName, char *RunwayName, char *TranName, bool isSID, bool isTrans, void *List, Reader_cb_t LineReader)
{
	char LineBuff[MAX_PATH];
	char file_name[MAX_PATH];
	FILE *f;

	strcpy(file_name, GetProcFileName(AirportICAO, isSID, isTrans));
	f = fopen(file_name, "rb");
	if (!f)
	{
		return false;	// No procedure for this airport
	}
	while (1)
	{
		if (fgets(LineBuff, MAX_PATH - 1, f) == NULL)
			break;		// End of file

		if (LineReader(LineBuff, ProcName, RunwayName, TranName, List))
			continue;
		else
			break;
	}

	fclose(f);
	return true;
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
