// ------------------------------------------------------------------------------------------
// NAVDATA package specific routines
//
// Package numbers:
//		0	Traditional PTT package
//		1	EADT KLN 90B - native
//		2	VAS FMC Flight Management - native
//
// Routines to be implemented for each package:
//		LoadSIDNames_X			Reads SID name list for an airport
//		LoadSTARNames_X			Reads STAR name list for an airport
//		LoadSIDRunways_X		Reads runway name list for a SID / airport
//		LoadSTARRunways_X		Reads runway name list for a STAR / airport
//		LoadSIDPoints_X			Reads navigation point list for a SID / runway / airport
//		LoadSTARPoints_X		Reads navigation point list for a STAR / runway / airport
//		Load_WPTS_X				Reads complete waypoints data
//
// Navigation package selection is defined in the KLN90B configuration file:
//		[KLN90B] section
//		NAVDATA=X				X = package number. Delete the parameter if no external package to be used.
//								This makes the old EXTINTDB parameter obsolate
// ------------------------------------------------------------------------------------------

// Number of supported navigation databases
#define	NAVDBSSUPPORTED 4

DWORD GetNavDatabaseCode();
char *GetNavDatabaseDir();


// ------------------------------------------------------------------------------------------
// Switchboard data handling functions in navdata_0.cpp
// ------------------------------------------------------------------------------------------

bool LoadSIDNames(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSTARNames(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSIDRunways(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList);
bool LoadSTARRunways(char *AirportICAO, char *STARName, vector<string> &RunwayNameList);
bool LoadSIDTransitions(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList);
bool LoadSTARTransitions(char *AirportICAO, char *STARName, vector<string> &TransitionNameList);
bool LoadSIDPoints(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList);
bool LoadSTARPoints(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList);
bool LoadWaypoints(vector<nvdb_wpt>& ints);

// ------------------------------------------------------------------------------------------
// Traditional PTT data handling functions in navdata_1.cpp
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_1(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSTARNames_1(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSIDRunways_1(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList);
bool LoadSTARRunways_1(char *AirportICAO, char *STARName, vector<string> &RunwayNameList);
bool LoadSIDPoints_1(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList);
bool LoadSTARPoints_1(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList);
bool LoadWaypoints_1(vector<nvdb_wpt>& ints);

// ------------------------------------------------------------------------------------------
// "EADT KLN 90B - native" data handling functions in navdata_2.cpp
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_2(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSTARNames_2(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSIDRunways_2(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList);
bool LoadSTARRunways_2(char *AirportICAO, char *STARName, vector<string> &RunwayNameList);
bool LoadSIDTransitions_2(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList);
bool LoadSTARTransitions_2(char *AirportICAO, char *STARName, vector<string> &TransitionNameList);
bool LoadSIDPoints_2(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList);
bool LoadSTARPoints_2(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList);
bool LoadWaypoints_2(vector<nvdb_wpt>& ints);

// ------------------------------------------------------------------------------------------
// "PSS Airbus/Boeing/Dash" data handling functions in navdata_3.cpp
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_3(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSTARNames_3(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSIDRunways_3(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList);
bool LoadSTARRunways_3(char *AirportICAO, char *STARName, vector<string> &RunwayNameList);
bool LoadSIDTransitions_3(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList);
bool LoadSTARTransitions_3(char *AirportICAO, char *STARName, vector<string> &TransitionNameList);
bool LoadSIDPoints_3(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList);
bool LoadSTARPoints_3(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList);
bool LoadWaypoints_3(vector<nvdb_wpt>& ints);

// ------------------------------------------------------------------------------------------
// "vasFMC Flight Management" data handling functions in navdata_4.cpp
// ------------------------------------------------------------------------------------------

bool LoadSIDNames_4(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSTARNames_4(char *AirportICAO, vector<string> &SIDProcList);
bool LoadSIDRunways_4(char *AirportICAO, char *SIDName, vector<string> &RunwayNameList);
bool LoadSTARRunways_4(char *AirportICAO, char *STARName, vector<string> &RunwayNameList);
bool LoadSIDTransitions_4(char *AirportICAO, char *SIDName, vector<string> &TransitionNameList);
bool LoadSTARTransitions_4(char *AirportICAO, char *STARName, vector<string> &TransitionNameList);
bool LoadSIDPoints_4(char *AirportICAO, char *SIDName, char *RunwayName, char *TranName, vector<nv_hdr_t> &SIDPointList);
bool LoadSTARPoints_4(char *AirportICAO, char *STARName, char *RunwayName, char *TranName, vector<nv_hdr_t> &STARPointList);
bool LoadWaypoints_4(vector<nvdb_wpt>& ints);
