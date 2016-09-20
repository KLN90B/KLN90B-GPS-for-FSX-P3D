/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::core;
using namespace swan2::lib::fsx;

typedef HRESULT (__stdcall *XSimConnectMapClientEventToSimEvent						)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID EventID,const char* EventName);
typedef HRESULT (__stdcall *XSimConnectTransmitClientEvent							)(HANDLE hSimConnect,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_CLIENT_EVENT_ID EventID,DWORD dwData,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_EVENT_FLAG Flags);
typedef HRESULT (__stdcall *XSimConnectSetSystemEventState							)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID EventID,SIMCONNECT_STATE dwState);
typedef HRESULT (__stdcall *XSimConnectAddClientEventToNotificationGroup			)(HANDLE hSimConnect,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_CLIENT_EVENT_ID EventID,BOOL bMaskable);
typedef HRESULT (__stdcall *XSimConnectRemoveClientEvent							)(HANDLE hSimConnect,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_CLIENT_EVENT_ID EventID);
typedef HRESULT (__stdcall *XSimConnectSetNotificationGroupPriority					)(HANDLE hSimConnect,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,DWORD uPriority);
typedef HRESULT (__stdcall *XSimConnectClearNotificationGroup						)(HANDLE hSimConnect,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID);
typedef HRESULT (__stdcall *XSimConnectRequestNotificationGroup						)(HANDLE hSimConnect,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,DWORD dwReserved,DWORD Flags);
typedef HRESULT (__stdcall *XSimConnectAddToDataDefinition							)(HANDLE hSimConnect,SIMCONNECT_DATA_DEFINITION_ID DefineID,const char* DatumName,const char* UnitsName,SIMCONNECT_DATATYPE DatumType,float fEpsilon,DWORD DatumID);
typedef HRESULT (__stdcall *XSimConnectClearDataDefinition							)(HANDLE hSimConnect,SIMCONNECT_DATA_DEFINITION_ID DefineID);
typedef HRESULT (__stdcall *XSimConnectRequestDataOnSimObject						)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_DATA_DEFINITION_ID DefineID,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_PERIOD Period,SIMCONNECT_DATA_REQUEST_FLAG Flags,DWORD origin,DWORD interval,DWORD limit);
typedef HRESULT (__stdcall *XSimConnectRequestDataOnSimObjectType					)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_DATA_DEFINITION_ID DefineID,DWORD dwRadiusMeters,SIMCONNECT_SIMOBJECT_TYPE type);
typedef HRESULT (__stdcall *XSimConnectSetDataOnSimObject							)(HANDLE hSimConnect,SIMCONNECT_DATA_DEFINITION_ID DefineID,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_SET_FLAG Flags,DWORD ArrayCount,DWORD cbUnitSize,void* pDataSet);
typedef HRESULT (__stdcall *XSimConnectMapInputEventToClientEvent					)(HANDLE hSimConnect,SIMCONNECT_INPUT_GROUP_ID GroupID,const char* szInputDefinition,SIMCONNECT_CLIENT_EVENT_ID DownEventID,DWORD DownValue,SIMCONNECT_CLIENT_EVENT_ID UpEventID,DWORD UpValue,BOOL bMaskable);
typedef HRESULT (__stdcall *XSimConnectSetInputGroupPriority						)(HANDLE hSimConnect,SIMCONNECT_INPUT_GROUP_ID GroupID,DWORD uPriority);
typedef HRESULT (__stdcall *XSimConnectRemoveInputEvent								)(HANDLE hSimConnect,SIMCONNECT_INPUT_GROUP_ID GroupID,const char* szInputDefinition);
typedef HRESULT (__stdcall *XSimConnectClearInputGroup								)(HANDLE hSimConnect,SIMCONNECT_INPUT_GROUP_ID GroupID);
typedef HRESULT (__stdcall *XSimConnectSetInputGroupState							)(HANDLE hSimConnect,SIMCONNECT_INPUT_GROUP_ID GroupID,DWORD dwState);
typedef HRESULT (__stdcall *XSimConnectRequestReservedKey							)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID EventID,const char* szKeyChoice1,const char* szKeyChoice2,const char* szKeyChoice3);
typedef HRESULT (__stdcall *XSimConnectSubscribeToSystemEvent						)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID EventID,const char* SystemEventName);
typedef HRESULT (__stdcall *XSimConnectUnsubscribeFromSystemEvent					)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID EventID);
typedef HRESULT (__stdcall *XSimConnectWeatherRequestInterpolatedObservation		)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon,float alt);
typedef HRESULT (__stdcall *XSimConnectWeatherRequestObservationAtStation			)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO);
typedef HRESULT (__stdcall *XSimConnectWeatherRequestObservationAtNearestStation	)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon);
typedef HRESULT (__stdcall *XSimConnectWeatherCreateStation							)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO,const char* szName,float lat,float lon,float alt);
typedef HRESULT (__stdcall *XSimConnectWeatherRemoveStation							)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO);
typedef HRESULT (__stdcall *XSimConnectWeatherSetObservation						)(HANDLE hSimConnect,DWORD Seconds,const char* szMETAR);
typedef HRESULT (__stdcall *XSimConnectWeatherSetModeServer							)(HANDLE hSimConnect,DWORD dwPort,DWORD dwSeconds);
typedef HRESULT (__stdcall *XSimConnectWeatherSetModeTheme							)(HANDLE hSimConnect,const char* szThemeName);
typedef HRESULT (__stdcall *XSimConnectWeatherSetModeGlobal							)(HANDLE hSimConnect);
typedef HRESULT (__stdcall *XSimConnectWeatherSetModeCustom							)(HANDLE hSimConnect);
typedef HRESULT (__stdcall *XSimConnectWeatherSetDynamicUpdateRate					)(HANDLE hSimConnect,DWORD dwRate);
typedef HRESULT (__stdcall *XSimConnectWeatherRequestCloudState						)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,float minLat,float minLon,float minAlt,float maxLat,float maxLon,float maxAlt,DWORD dwFlags);
typedef HRESULT (__stdcall *XSimConnectWeatherCreateThermal							)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon,float alt,float radius,float height,float coreRate,float coreTurbulence,float sinkRate,float sinkTurbulence,float coreSize,float coreTransitionSize,float sinkLayerSize,float sinkTransitionSize);
typedef HRESULT (__stdcall *XSimConnectWeatherRemoveThermal							)(HANDLE hSimConnect,SIMCONNECT_OBJECT_ID ObjectID);
typedef HRESULT (__stdcall *XSimConnectAICreateParkedATCAircraft					)(HANDLE hSimConnect,const char* szContainerTitle,const char* szTailNumber,const char* szAirportID,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectAICreateEnrouteATCAircraft					)(HANDLE hSimConnect,const char* szContainerTitle,const char* szTailNumber,int iFlightNumber,const char* szFlightPlanPath,double dFlightPlanPosition,BOOL bTouchAndGo,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectAICreateNonATCAircraft						)(HANDLE hSimConnect,const char* szContainerTitle,const char* szTailNumber,SIMCONNECT_DATA_INITPOSITION InitPos,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectAICreateSimulatedObject						)(HANDLE hSimConnect,const char* szContainerTitle,SIMCONNECT_DATA_INITPOSITION InitPos,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectAIReleaseControl								)(HANDLE hSimConnect,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectAIRemoveObject								)(HANDLE hSimConnect,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectAISetAircraftFlightPlan						)(HANDLE hSimConnect,SIMCONNECT_OBJECT_ID ObjectID,const char* szFlightPlanPath,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectExecuteMissionAction							)(HANDLE hSimConnect,const GUID guidInstanceId);
typedef HRESULT (__stdcall *XSimConnectCompleteCustomMissionAction					)(HANDLE hSimConnect,const GUID guidInstanceId);
typedef HRESULT (__stdcall *XSimConnectClose										)(HANDLE hSimConnect);
typedef HRESULT (__stdcall *XSimConnectRetrieveString								)(SIMCONNECT_RECV* pData,DWORD cbData,void* pStringV,char** pszString,DWORD* pcbString);
typedef HRESULT (__stdcall *XSimConnectGetLastSentPacketID							)(HANDLE hSimConnect,DWORD* pdwError);
typedef HRESULT (__stdcall *XSimConnectOpen											)(HANDLE* phSimConnect,LPCSTR szName,HWND hWnd,DWORD UserEventWin32,HANDLE hEventHandle,DWORD ConfigIndex);
typedef HRESULT (__stdcall *XSimConnectCallDispatch									)(HANDLE hSimConnect,DispatchProc pfcnDispatch,void* pContext);
typedef HRESULT (__stdcall *XSimConnectGetNextDispatch								)(HANDLE hSimConnect,SIMCONNECT_RECV** ppData,DWORD* pcbData);
typedef HRESULT (__stdcall *XSimConnectRequestResponseTimes							)(HANDLE hSimConnect,DWORD nCount,float* fElapsedSeconds);
typedef HRESULT (__stdcall *XSimConnectInsertString									)(char* pDest,DWORD cbDest,void ** ppEnd,DWORD* pcbStringV,const char* pSource);
typedef HRESULT (__stdcall *XSimConnectCameraSetRelative6DOF						)(HANDLE hSimConnect,float fDeltaX,float fDeltaY,float fDeltaZ,float fPitchDeg,float fBankDeg,float fHeadingDeg);
typedef HRESULT (__stdcall *XSimConnectMenuAddItem									)(HANDLE hSimConnect,const char* szMenuItem,SIMCONNECT_CLIENT_EVENT_ID MenuEventID,DWORD dwData);
typedef HRESULT (__stdcall *XSimConnectMenuDeleteItem								)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID MenuEventID);
typedef HRESULT (__stdcall *XSimConnectMenuAddSubItem								)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID MenuEventID,const char* szMenuItem,SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID,DWORD dwData);
typedef HRESULT (__stdcall *XSimConnectMenuDeleteSubItem							)(HANDLE hSimConnect,SIMCONNECT_CLIENT_EVENT_ID MenuEventID,const SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID);
typedef HRESULT (__stdcall *XSimConnectRequestSystemState							)(HANDLE hSimConnect,SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szState);
typedef HRESULT (__stdcall *XSimConnectSetSystemState								)(HANDLE hSimConnect,const char* szState,DWORD dwInteger,float fFloat,const char* szString);
typedef HRESULT (__stdcall *XSimConnectMapClientDataNameToID						)(HANDLE hSimConnect,const char* szClientDataName,SIMCONNECT_CLIENT_DATA_ID ClientDataID);
typedef HRESULT (__stdcall *XSimConnectCreateClientData								)(HANDLE hSimConnect,SIMCONNECT_CLIENT_DATA_ID ClientDataID,DWORD dwSize,SIMCONNECT_CREATE_CLIENT_DATA_FLAG Flags);
typedef HRESULT (__stdcall *XSimConnectAddToClientDataDefinition					)(HANDLE hSimConnect,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,DWORD dwOffset,DWORD dwSizeOrType,float fEpsilon,DWORD DatumID);
typedef HRESULT (__stdcall *XSimConnectClearClientDataDefinition					)(HANDLE hSimConnect,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID);
typedef HRESULT (__stdcall *XSimConnectRequestClientData							)(HANDLE hSimConnect,SIMCONNECT_CLIENT_DATA_ID ClientDataID,SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,SIMCONNECT_CLIENT_DATA_PERIOD Period,SIMCONNECT_CLIENT_DATA_REQUEST_FLAG Flags,DWORD origin,DWORD interval,DWORD limit);
typedef HRESULT (__stdcall *XSimConnectSetClientData								)(HANDLE hSimConnect,SIMCONNECT_CLIENT_DATA_ID ClientDataID,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,SIMCONNECT_CLIENT_DATA_SET_FLAG Flags,DWORD dwReserved,DWORD cbUnitSize,void* pDataSet);
typedef HRESULT (__stdcall *XSimConnectFlightLoad									)(HANDLE hSimConnect,const char* szFileName);
typedef HRESULT (__stdcall *XSimConnectFlightSave									)(HANDLE hSimConnect,const char* szFileName,const char* szTitle,const char* szDescription,DWORD Flags);
typedef HRESULT (__stdcall *XSimConnectFlightPlanLoad								)(HANDLE hSimConnect,const char* szFileName);
typedef HRESULT (__stdcall *XSimConnectText											)(HANDLE hSimConnect,SIMCONNECT_TEXT_TYPE type,float fTimeSeconds,SIMCONNECT_CLIENT_EVENT_ID EventID,DWORD cbUnitSize,void* pDataSet);
typedef HRESULT (__stdcall *XSimConnectSubscribeToFacilities						)(HANDLE hSimConnect,SIMCONNECT_FACILITY_LIST_TYPE type,SIMCONNECT_DATA_REQUEST_ID RequestID);
typedef HRESULT (__stdcall *XSimConnectUnsubscribeToFacilities						)(HANDLE hSimConnect,SIMCONNECT_FACILITY_LIST_TYPE type);
typedef HRESULT (__stdcall *XSimConnectRequestFacilitiesList						)(HANDLE hSimConnect,SIMCONNECT_FACILITY_LIST_TYPE type,SIMCONNECT_DATA_REQUEST_ID RequestID);

extern "C" 
{
	XSimConnectMapClientEventToSimEvent						SimConnectMapClientEventToSimEvent					= NULL;
	XSimConnectTransmitClientEvent							SimConnectTransmitClientEvent						= NULL;
	XSimConnectSetSystemEventState							SimConnectSetSystemEventState						= NULL;
	XSimConnectAddClientEventToNotificationGroup			SimConnectAddClientEventToNotificationGroup			= NULL;
	XSimConnectRemoveClientEvent							SimConnectRemoveClientEvent							= NULL;
	XSimConnectSetNotificationGroupPriority					SimConnectSetNotificationGroupPriority				= NULL;
	XSimConnectClearNotificationGroup						SimConnectClearNotificationGroup					= NULL;
	XSimConnectRequestNotificationGroup						SimConnectRequestNotificationGroup					= NULL;
	XSimConnectAddToDataDefinition							SimConnectAddToDataDefinition						= NULL;
	XSimConnectClearDataDefinition							SimConnectClearDataDefinition						= NULL;
	XSimConnectRequestDataOnSimObject						SimConnectRequestDataOnSimObject					= NULL;
	XSimConnectRequestDataOnSimObjectType					SimConnectRequestDataOnSimObjectType				= NULL;
	XSimConnectSetDataOnSimObject							SimConnectSetDataOnSimObject						= NULL;
	XSimConnectMapInputEventToClientEvent					SimConnectMapInputEventToClientEvent				= NULL;
	XSimConnectSetInputGroupPriority						SimConnectSetInputGroupPriority						= NULL;
	XSimConnectRemoveInputEvent								SimConnectRemoveInputEvent							= NULL;
	XSimConnectClearInputGroup								SimConnectClearInputGroup							= NULL;
	XSimConnectSetInputGroupState							SimConnectSetInputGroupState						= NULL;
	XSimConnectRequestReservedKey							SimConnectRequestReservedKey						= NULL;
	XSimConnectSubscribeToSystemEvent						SimConnectSubscribeToSystemEvent					= NULL;
	XSimConnectUnsubscribeFromSystemEvent					SimConnectUnsubscribeFromSystemEvent				= NULL;
	XSimConnectWeatherRequestInterpolatedObservation		SimConnectWeatherRequestInterpolatedObservation		= NULL;
	XSimConnectWeatherRequestObservationAtStation			SimConnectWeatherRequestObservationAtStation		= NULL;
	XSimConnectWeatherRequestObservationAtNearestStation	SimConnectWeatherRequestObservationAtNearestStation	= NULL;
	XSimConnectWeatherCreateStation							SimConnectWeatherCreateStation						= NULL;
	XSimConnectWeatherRemoveStation							SimConnectWeatherRemoveStation						= NULL;
	XSimConnectWeatherSetObservation						SimConnectWeatherSetObservation						= NULL;
	XSimConnectWeatherSetModeServer							SimConnectWeatherSetModeServer						= NULL;
	XSimConnectWeatherSetModeTheme							SimConnectWeatherSetModeTheme						= NULL;
	XSimConnectWeatherSetModeGlobal							SimConnectWeatherSetModeGlobal						= NULL;
	XSimConnectWeatherSetModeCustom							SimConnectWeatherSetModeCustom						= NULL;
	XSimConnectWeatherSetDynamicUpdateRate					SimConnectWeatherSetDynamicUpdateRate				= NULL;
	XSimConnectWeatherRequestCloudState						SimConnectWeatherRequestCloudState					= NULL;
	XSimConnectWeatherCreateThermal							SimConnectWeatherCreateThermal						= NULL;
	XSimConnectWeatherRemoveThermal							SimConnectWeatherRemoveThermal						= NULL;
	XSimConnectAICreateParkedATCAircraft					SimConnectAICreateParkedATCAircraft					= NULL;
	XSimConnectAICreateEnrouteATCAircraft					SimConnectAICreateEnrouteATCAircraft				= NULL;
	XSimConnectAICreateNonATCAircraft						SimConnectAICreateNonATCAircraft					= NULL;
	XSimConnectAICreateSimulatedObject						SimConnectAICreateSimulatedObject					= NULL;
	XSimConnectAIReleaseControl								SimConnectAIReleaseControl							= NULL;
	XSimConnectAIRemoveObject								SimConnectAIRemoveObject							= NULL;
	XSimConnectAISetAircraftFlightPlan						SimConnectAISetAircraftFlightPlan					= NULL;
	XSimConnectExecuteMissionAction							SimConnectExecuteMissionAction						= NULL;
	XSimConnectCompleteCustomMissionAction					SimConnectCompleteCustomMissionAction				= NULL;
	XSimConnectClose										SimConnectClose										= NULL;
	XSimConnectRetrieveString								SimConnectRetrieveString							= NULL;
	XSimConnectGetLastSentPacketID							SimConnectGetLastSentPacketID						= NULL;
	XSimConnectOpen											SimConnectOpen										= NULL;
	XSimConnectCallDispatch									SimConnectCallDispatch								= NULL;
	XSimConnectGetNextDispatch								SimConnectGetNextDispatch							= NULL;
	XSimConnectRequestResponseTimes							SimConnectRequestResponseTimes						= NULL;
	XSimConnectInsertString									SimConnectInsertString								= NULL;
	XSimConnectCameraSetRelative6DOF						SimConnectCameraSetRelative6DOF						= NULL;
	XSimConnectMenuAddItem									SimConnectMenuAddItem								= NULL;
	XSimConnectMenuDeleteItem								SimConnectMenuDeleteItem							= NULL;
	XSimConnectMenuAddSubItem								SimConnectMenuAddSubItem							= NULL;
	XSimConnectMenuDeleteSubItem							SimConnectMenuDeleteSubItem							= NULL;
	XSimConnectRequestSystemState							SimConnectRequestSystemState						= NULL;
	XSimConnectSetSystemState								SimConnectSetSystemState							= NULL;
	XSimConnectMapClientDataNameToID						SimConnectMapClientDataNameToID						= NULL;
	XSimConnectCreateClientData								SimConnectCreateClientData							= NULL;
	XSimConnectAddToClientDataDefinition					SimConnectAddToClientDataDefinition					= NULL;
	XSimConnectClearClientDataDefinition					SimConnectClearClientDataDefinition					= NULL;
	XSimConnectRequestClientData							SimConnectRequestClientData							= NULL;
	XSimConnectSetClientData								SimConnectSetClientData								= NULL;
	XSimConnectFlightLoad									SimConnectFlightLoad								= NULL;
	XSimConnectFlightSave									SimConnectFlightSave								= NULL;
	XSimConnectFlightPlanLoad								SimConnectFlightPlanLoad							= NULL;
	XSimConnectText											SimConnectText										= NULL;
	XSimConnectSubscribeToFacilities						SimConnectSubscribeToFacilities						= NULL;
	XSimConnectUnsubscribeToFacilities						SimConnectUnsubscribeToFacilities					= NULL;
	XSimConnectRequestFacilitiesList						SimConnectRequestFacilitiesList						= NULL;
}

SimConnectWrapper::SimConnectWrapper() :	simConnect(NULL),
											moduleSimConnect(NULL),
											isInitialized(false),
											simconnectModuleName("")
{
}

SimConnectWrapper::~SimConnectWrapper()
{
	if(isInitialized)
		DeInit();
}

void SimConnectWrapper::CheckSimVersion()
{
	VersionInfo ver(static_cast<HMODULE>(0));
	unsigned short v=ver.GetVerAux();

	if(v==60905)					// FSX RTM
		simconnectModuleName="gauges/sc_rtm.dll";
	else if(v==61355||v==61357)		// FSX SP1 ENG and RUS
		simconnectModuleName="gauges/sc_sp1.dll";
	else if(v==61472)				// FSX SP2
		simconnectModuleName="gauges/sc_acc.dll";
	else if(v==61637)				// FSX Acceleration
		simconnectModuleName="gauges/sc_acc.dll";
	else
		simconnectModuleName="gauges/sc_acc.dll";
}

bool SimConnectWrapper::Init(const std::string& name)
{
	if(isInitialized)
		return true;

	CheckSimVersion();

	if(simconnectModuleName=="")
		EXCEPT("Can't get FSX version.");

	moduleSimConnect=LoadLibrary(simconnectModuleName.c_str());
	if(!moduleSimConnect) {
		EXCEPT("Could not load "+simconnectModuleName);
	}

	if(!(SimConnectMapClientEventToSimEvent					=	(XSimConnectMapClientEventToSimEvent					)GetProcAddress(moduleSimConnect,"SimConnect_MapClientEventToSimEvent"					)))	EXCEPT("Could not load SimConnectMapClientEventToSimEvent					");
	if(!(SimConnectTransmitClientEvent						=	(XSimConnectTransmitClientEvent							)GetProcAddress(moduleSimConnect,"SimConnect_TransmitClientEvent"						)))	EXCEPT("Could not load SimConnectTransmitClientEvent						");
	if(!(SimConnectSetSystemEventState						=	(XSimConnectSetSystemEventState							)GetProcAddress(moduleSimConnect,"SimConnect_SetSystemEventState"						)))	EXCEPT("Could not load SimConnectSetSystemEventState						");
	if(!(SimConnectAddClientEventToNotificationGroup		=	(XSimConnectAddClientEventToNotificationGroup			)GetProcAddress(moduleSimConnect,"SimConnect_AddClientEventToNotificationGroup"			)))	EXCEPT("Could not load SimConnectAddClientEventToNotificationGroup		");
	if(!(SimConnectRemoveClientEvent						=	(XSimConnectRemoveClientEvent							)GetProcAddress(moduleSimConnect,"SimConnect_RemoveClientEvent"							)))	EXCEPT("Could not load SimConnectRemoveClientEvent						");
	if(!(SimConnectSetNotificationGroupPriority				=	(XSimConnectSetNotificationGroupPriority				)GetProcAddress(moduleSimConnect,"SimConnect_SetNotificationGroupPriority"				)))	EXCEPT("Could not load SimConnectSetNotificationGroupPriority				");
	if(!(SimConnectClearNotificationGroup					=	(XSimConnectClearNotificationGroup						)GetProcAddress(moduleSimConnect,"SimConnect_ClearNotificationGroup"					)))	EXCEPT("Could not load SimConnectClearNotificationGroup					");
	if(!(SimConnectRequestNotificationGroup					=	(XSimConnectRequestNotificationGroup					)GetProcAddress(moduleSimConnect,"SimConnect_RequestNotificationGroup"					)))	EXCEPT("Could not load SimConnectRequestNotificationGroup					");
	if(!(SimConnectAddToDataDefinition						=	(XSimConnectAddToDataDefinition							)GetProcAddress(moduleSimConnect,"SimConnect_AddToDataDefinition"						)))	EXCEPT("Could not load SimConnectAddToDataDefinition						");
	if(!(SimConnectClearDataDefinition						=	(XSimConnectClearDataDefinition							)GetProcAddress(moduleSimConnect,"SimConnect_ClearDataDefinition"						)))	EXCEPT("Could not load SimConnectClearDataDefinition						");
	if(!(SimConnectRequestDataOnSimObject					=	(XSimConnectRequestDataOnSimObject						)GetProcAddress(moduleSimConnect,"SimConnect_RequestDataOnSimObject"					)))	EXCEPT("Could not load SimConnectRequestDataOnSimObject					");
	if(!(SimConnectRequestDataOnSimObjectType				=	(XSimConnectRequestDataOnSimObjectType					)GetProcAddress(moduleSimConnect,"SimConnect_RequestDataOnSimObjectType"				)))	EXCEPT("Could not load SimConnectRequestDataOnSimObjectType				");
	if(!(SimConnectSetDataOnSimObject						=	(XSimConnectSetDataOnSimObject							)GetProcAddress(moduleSimConnect,"SimConnect_SetDataOnSimObject"						)))	EXCEPT("Could not load SimConnectSetDataOnSimObject						");
	if(!(SimConnectMapInputEventToClientEvent				=	(XSimConnectMapInputEventToClientEvent					)GetProcAddress(moduleSimConnect,"SimConnect_MapInputEventToClientEvent"				)))	EXCEPT("Could not load SimConnectMapInputEventToClientEvent				");
	if(!(SimConnectSetInputGroupPriority					=	(XSimConnectSetInputGroupPriority						)GetProcAddress(moduleSimConnect,"SimConnect_SetInputGroupPriority"						)))	EXCEPT("Could not load SimConnectSetInputGroupPriority					");
	if(!(SimConnectRemoveInputEvent							=	(XSimConnectRemoveInputEvent							)GetProcAddress(moduleSimConnect,"SimConnect_RemoveInputEvent"							)))	EXCEPT("Could not load SimConnectRemoveInputEvent							");
	if(!(SimConnectClearInputGroup							=	(XSimConnectClearInputGroup								)GetProcAddress(moduleSimConnect,"SimConnect_ClearInputGroup"							)))	EXCEPT("Could not load SimConnectClearInputGroup							");
	if(!(SimConnectSetInputGroupState						=	(XSimConnectSetInputGroupState							)GetProcAddress(moduleSimConnect,"SimConnect_SetInputGroupState"						)))	EXCEPT("Could not load SimConnectSetInputGroupState						");
	if(!(SimConnectRequestReservedKey						=	(XSimConnectRequestReservedKey							)GetProcAddress(moduleSimConnect,"SimConnect_RequestReservedKey"						)))	EXCEPT("Could not load SimConnectRequestReservedKey						");
	if(!(SimConnectSubscribeToSystemEvent					=	(XSimConnectSubscribeToSystemEvent						)GetProcAddress(moduleSimConnect,"SimConnect_SubscribeToSystemEvent"					)))	EXCEPT("Could not load SimConnectSubscribeToSystemEvent					");
	if(!(SimConnectUnsubscribeFromSystemEvent				=	(XSimConnectUnsubscribeFromSystemEvent					)GetProcAddress(moduleSimConnect,"SimConnect_UnsubscribeFromSystemEvent"				)))	EXCEPT("Could not load SimConnectUnsubscribeFromSystemEvent				");
	if(!(SimConnectWeatherRequestInterpolatedObservation	=	(XSimConnectWeatherRequestInterpolatedObservation		)GetProcAddress(moduleSimConnect,"SimConnect_WeatherRequestInterpolatedObservation"		)))	EXCEPT("Could not load SimConnectWeatherRequestInterpolatedObservation	");
	if(!(SimConnectWeatherRequestObservationAtStation		=	(XSimConnectWeatherRequestObservationAtStation			)GetProcAddress(moduleSimConnect,"SimConnect_WeatherRequestObservationAtStation"		)))	EXCEPT("Could not load SimConnectWeatherRequestObservationAtStation		");
	if(!(SimConnectWeatherRequestObservationAtNearestStation=	(XSimConnectWeatherRequestObservationAtNearestStation	)GetProcAddress(moduleSimConnect,"SimConnect_WeatherRequestObservationAtNearestStation"	)))	EXCEPT("Could not load SimConnectWeatherRequestObservationAtNearestStation");
	if(!(SimConnectWeatherCreateStation						=	(XSimConnectWeatherCreateStation						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherCreateStation"						)))	EXCEPT("Could not load SimConnectWeatherCreateStation						");
	if(!(SimConnectWeatherRemoveStation						=	(XSimConnectWeatherRemoveStation						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherRemoveStation"						)))	EXCEPT("Could not load SimConnectWeatherRemoveStation						");
	if(!(SimConnectWeatherSetObservation					=	(XSimConnectWeatherSetObservation						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherSetObservation"						)))	EXCEPT("Could not load SimConnectWeatherSetObservation					");
	if(!(SimConnectWeatherSetModeServer						=	(XSimConnectWeatherSetModeServer						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherSetModeServer"						)))	EXCEPT("Could not load SimConnectWeatherSetModeServer						");
	if(!(SimConnectWeatherSetModeTheme						=	(XSimConnectWeatherSetModeTheme							)GetProcAddress(moduleSimConnect,"SimConnect_WeatherSetModeTheme"						)))	EXCEPT("Could not load SimConnectWeatherSetModeTheme						");
	if(!(SimConnectWeatherSetModeGlobal						=	(XSimConnectWeatherSetModeGlobal						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherSetModeGlobal"						)))	EXCEPT("Could not load SimConnectWeatherSetModeGlobal						");
	if(!(SimConnectWeatherSetModeCustom						=	(XSimConnectWeatherSetModeCustom						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherSetModeCustom"						)))	EXCEPT("Could not load SimConnectWeatherSetModeCustom						");
	if(!(SimConnectWeatherSetDynamicUpdateRate				=	(XSimConnectWeatherSetDynamicUpdateRate					)GetProcAddress(moduleSimConnect,"SimConnect_WeatherSetDynamicUpdateRate"				)))	EXCEPT("Could not load SimConnectWeatherSetDynamicUpdateRate				");
	if(!(SimConnectWeatherRequestCloudState					=	(XSimConnectWeatherRequestCloudState					)GetProcAddress(moduleSimConnect,"SimConnect_WeatherRequestCloudState"					)))	EXCEPT("Could not load SimConnectWeatherRequestCloudState					");
	if(!(SimConnectWeatherCreateThermal						=	(XSimConnectWeatherCreateThermal						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherCreateThermal"						)))	EXCEPT("Could not load SimConnectWeatherCreateThermal						");
	if(!(SimConnectWeatherRemoveThermal						=	(XSimConnectWeatherRemoveThermal						)GetProcAddress(moduleSimConnect,"SimConnect_WeatherRemoveThermal"						)))	EXCEPT("Could not load SimConnectWeatherRemoveThermal						");
	if(!(SimConnectAICreateParkedATCAircraft				=	(XSimConnectAICreateParkedATCAircraft					)GetProcAddress(moduleSimConnect,"SimConnect_AICreateParkedATCAircraft"					)))	EXCEPT("Could not load SimConnectAICreateParkedATCAircraft				");
	if(!(SimConnectAICreateEnrouteATCAircraft				=	(XSimConnectAICreateEnrouteATCAircraft					)GetProcAddress(moduleSimConnect,"SimConnect_AICreateEnrouteATCAircraft"				)))	EXCEPT("Could not load SimConnectAICreateEnrouteATCAircraft				");
	if(!(SimConnectAICreateNonATCAircraft					=	(XSimConnectAICreateNonATCAircraft						)GetProcAddress(moduleSimConnect,"SimConnect_AICreateNonATCAircraft"					)))	EXCEPT("Could not load SimConnectAICreateNonATCAircraft					");
	if(!(SimConnectAICreateSimulatedObject					=	(XSimConnectAICreateSimulatedObject						)GetProcAddress(moduleSimConnect,"SimConnect_AICreateSimulatedObject"					)))	EXCEPT("Could not load SimConnectAICreateSimulatedObject					");
	if(!(SimConnectAIReleaseControl							=	(XSimConnectAIReleaseControl							)GetProcAddress(moduleSimConnect,"SimConnect_AIReleaseControl"							)))	EXCEPT("Could not load SimConnectAIReleaseControl							");
	if(!(SimConnectAIRemoveObject							=	(XSimConnectAIRemoveObject								)GetProcAddress(moduleSimConnect,"SimConnect_AIRemoveObject"							)))	EXCEPT("Could not load SimConnectAIRemoveObject							");
	if(!(SimConnectAISetAircraftFlightPlan					=	(XSimConnectAISetAircraftFlightPlan						)GetProcAddress(moduleSimConnect,"SimConnect_AISetAircraftFlightPlan"					)))	EXCEPT("Could not load SimConnectAISetAircraftFlightPlan					");
	if(!(SimConnectExecuteMissionAction						=	(XSimConnectExecuteMissionAction						)GetProcAddress(moduleSimConnect,"SimConnect_ExecuteMissionAction"						)))	EXCEPT("Could not load SimConnectExecuteMissionAction						");
	if(!(SimConnectCompleteCustomMissionAction				=	(XSimConnectCompleteCustomMissionAction					)GetProcAddress(moduleSimConnect,"SimConnect_CompleteCustomMissionAction"				)))	EXCEPT("Could not load SimConnectCompleteCustomMissionAction				");
	if(!(SimConnectClose									=	(XSimConnectClose										)GetProcAddress(moduleSimConnect,"SimConnect_Close"										)))	EXCEPT("Could not load SimConnectClose									");
	if(!(SimConnectRetrieveString							=	(XSimConnectRetrieveString								)GetProcAddress(moduleSimConnect,"SimConnect_RetrieveString"							)))	EXCEPT("Could not load SimConnectRetrieveString							");
	if(!(SimConnectGetLastSentPacketID						=	(XSimConnectGetLastSentPacketID							)GetProcAddress(moduleSimConnect,"SimConnect_GetLastSentPacketID"						)))	EXCEPT("Could not load SimConnectGetLastSentPacketID						");
	if(!(SimConnectOpen										=	(XSimConnectOpen										)GetProcAddress(moduleSimConnect,"SimConnect_Open"										)))	EXCEPT("Could not load SimConnectOpen										");
	if(!(SimConnectCallDispatch								=	(XSimConnectCallDispatch								)GetProcAddress(moduleSimConnect,"SimConnect_CallDispatch"								)))	EXCEPT("Could not load SimConnectCallDispatch								");
	if(!(SimConnectGetNextDispatch							=	(XSimConnectGetNextDispatch								)GetProcAddress(moduleSimConnect,"SimConnect_GetNextDispatch"							)))	EXCEPT("Could not load SimConnectGetNextDispatch							");
	if(!(SimConnectRequestResponseTimes						=	(XSimConnectRequestResponseTimes						)GetProcAddress(moduleSimConnect,"SimConnect_RequestResponseTimes"						)))	EXCEPT("Could not load SimConnectRequestResponseTimes						");
	if(!(SimConnectInsertString								=	(XSimConnectInsertString								)GetProcAddress(moduleSimConnect,"SimConnect_InsertString"								)))	EXCEPT("Could not load SimConnectInsertString								");
	if(!(SimConnectCameraSetRelative6DOF					=	(XSimConnectCameraSetRelative6DOF						)GetProcAddress(moduleSimConnect,"SimConnect_CameraSetRelative6DOF"						)))	EXCEPT("Could not load SimConnectCameraSetRelative6DOF					");
	if(!(SimConnectMenuAddItem								=	(XSimConnectMenuAddItem									)GetProcAddress(moduleSimConnect,"SimConnect_MenuAddItem"								)))	EXCEPT("Could not load SimConnectMenuAddItem								");
	if(!(SimConnectMenuDeleteItem							=	(XSimConnectMenuDeleteItem								)GetProcAddress(moduleSimConnect,"SimConnect_MenuDeleteItem"							)))	EXCEPT("Could not load SimConnectMenuDeleteItem							");
	if(!(SimConnectMenuAddSubItem							=	(XSimConnectMenuAddSubItem								)GetProcAddress(moduleSimConnect,"SimConnect_MenuAddSubItem"							)))	EXCEPT("Could not load SimConnectMenuAddSubItem							");
	if(!(SimConnectMenuDeleteSubItem						=	(XSimConnectMenuDeleteSubItem							)GetProcAddress(moduleSimConnect,"SimConnect_MenuDeleteSubItem"							)))	EXCEPT("Could not load SimConnectMenuDeleteSubItem						");
	if(!(SimConnectRequestSystemState						=	(XSimConnectRequestSystemState							)GetProcAddress(moduleSimConnect,"SimConnect_RequestSystemState"						)))	EXCEPT("Could not load SimConnectRequestSystemState						");
	if(!(SimConnectSetSystemState							=	(XSimConnectSetSystemState								)GetProcAddress(moduleSimConnect,"SimConnect_SetSystemState"							)))	EXCEPT("Could not load SimConnectSetSystemState							");
	if(!(SimConnectMapClientDataNameToID					=	(XSimConnectMapClientDataNameToID						)GetProcAddress(moduleSimConnect,"SimConnect_MapClientDataNameToID"						)))	EXCEPT("Could not load SimConnectMapClientDataNameToID					");
	if(!(SimConnectCreateClientData							=	(XSimConnectCreateClientData							)GetProcAddress(moduleSimConnect,"SimConnect_CreateClientData"							)))	EXCEPT("Could not load SimConnectCreateClientData							");
	if(!(SimConnectAddToClientDataDefinition				=	(XSimConnectAddToClientDataDefinition					)GetProcAddress(moduleSimConnect,"SimConnect_AddToClientDataDefinition"					)))	EXCEPT("Could not load SimConnectAddToClientDataDefinition				");
	if(!(SimConnectClearClientDataDefinition				=	(XSimConnectClearClientDataDefinition					)GetProcAddress(moduleSimConnect,"SimConnect_ClearClientDataDefinition"					)))	EXCEPT("Could not load SimConnectClearClientDataDefinition				");
	if(!(SimConnectRequestClientData						=	(XSimConnectRequestClientData							)GetProcAddress(moduleSimConnect,"SimConnect_RequestClientData"							)))	EXCEPT("Could not load SimConnectRequestClientData						");
	if(!(SimConnectSetClientData							=	(XSimConnectSetClientData								)GetProcAddress(moduleSimConnect,"SimConnect_SetClientData"								)))	EXCEPT("Could not load SimConnectSetClientData							");
	if(!(SimConnectFlightLoad								=	(XSimConnectFlightLoad									)GetProcAddress(moduleSimConnect,"SimConnect_FlightLoad"								)))	EXCEPT("Could not load SimConnectFlightLoad								");
	if(!(SimConnectFlightSave								=	(XSimConnectFlightSave									)GetProcAddress(moduleSimConnect,"SimConnect_FlightSave"								)))	EXCEPT("Could not load SimConnectFlightSave								");
	if(!(SimConnectFlightPlanLoad							=	(XSimConnectFlightPlanLoad								)GetProcAddress(moduleSimConnect,"SimConnect_FlightPlanLoad"							)))	EXCEPT("Could not load SimConnectFlightPlanLoad							");
	if(!(SimConnectText										=	(XSimConnectText										)GetProcAddress(moduleSimConnect,"SimConnect_Text"										)))	EXCEPT("Could not load SimConnectText										");
	if(!(SimConnectSubscribeToFacilities					=	(XSimConnectSubscribeToFacilities						)GetProcAddress(moduleSimConnect,"SimConnect_SubscribeToFacilities"						)))	EXCEPT("Could not load SimConnectSubscribeToFacilities					");
	if(!(SimConnectUnsubscribeToFacilities					=	(XSimConnectUnsubscribeToFacilities						)GetProcAddress(moduleSimConnect,"SimConnect_UnsubscribeToFacilities"					)))	EXCEPT("Could not load SimConnectUnsubscribeToFacilities					");
	if(!(SimConnectRequestFacilitiesList					=	(XSimConnectRequestFacilitiesList						)GetProcAddress(moduleSimConnect,"SimConnect_RequestFacilitiesList"						)))	EXCEPT("Could not load SimConnectRequestFacilitiesList					");

	isInitialized=true;

	HRESULT hr=Open(&simConnect,name.c_str(),FindWindow("FS98MAIN",NULL),0,0,0);
	if(hr) {
		EXCEPT("Could not connect to Flight Simulator!");   
	}

	return true;
}

void SimConnectWrapper::DeInit()
{
	if(!isInitialized)
		return;

	Close();

	if(moduleSimConnect) {
		FreeLibrary(moduleSimConnect);
		moduleSimConnect=NULL;
	}

	isInitialized=false;
}

HANDLE SimConnectWrapper::Get() 
{
	if(!isInitialized)
		Init();
	return simConnect;
}

HRESULT SimConnectWrapper::MapClientEventToSimEvent(SIMCONNECT_CLIENT_EVENT_ID EventID,const char* EventName)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectMapClientEventToSimEvent(simConnect,EventID,EventName);
}

HRESULT SimConnectWrapper::TransmitClientEvent(SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_CLIENT_EVENT_ID EventID,DWORD dwData,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_EVENT_FLAG Flags)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectTransmitClientEvent(simConnect,ObjectID,EventID,dwData,GroupID,Flags);
}

HRESULT SimConnectWrapper::SetSystemEventState(SIMCONNECT_CLIENT_EVENT_ID EventID,SIMCONNECT_STATE dwState)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSetSystemEventState(simConnect,EventID,dwState);
}

HRESULT SimConnectWrapper::AddClientEventToNotificationGroup(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_CLIENT_EVENT_ID EventID,BOOL bMaskable)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAddClientEventToNotificationGroup(simConnect,GroupID,EventID,bMaskable);
}

HRESULT SimConnectWrapper::RemoveClientEvent(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_CLIENT_EVENT_ID EventID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRemoveClientEvent(Get(),GroupID,EventID);
}

HRESULT SimConnectWrapper::SetNotificationGroupPriority(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,DWORD uPriority)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSetNotificationGroupPriority(simConnect,GroupID,uPriority);
}

HRESULT SimConnectWrapper::ClearNotificationGroup(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectClearNotificationGroup(Get(),GroupID);
}

HRESULT SimConnectWrapper::RequestNotificationGroup(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,DWORD dwReserved,DWORD Flags)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestNotificationGroup(simConnect,GroupID,dwReserved,Flags);
}

HRESULT SimConnectWrapper::AddToDataDefinition(SIMCONNECT_DATA_DEFINITION_ID DefineID,const char* DatumName,const char* UnitsName,SIMCONNECT_DATATYPE DatumType,float fEpsilon,DWORD DatumID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAddToDataDefinition(simConnect,DefineID,DatumName,UnitsName,DatumType,fEpsilon,DatumID);
}

HRESULT SimConnectWrapper::ClearDataDefinition(SIMCONNECT_DATA_DEFINITION_ID DefineID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectClearDataDefinition(simConnect,DefineID);
}

HRESULT SimConnectWrapper::RequestDataOnSimObject(SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_DATA_DEFINITION_ID DefineID,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_PERIOD Period,SIMCONNECT_DATA_REQUEST_FLAG Flags,DWORD origin,DWORD interval,DWORD limit)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestDataOnSimObject(simConnect,RequestID,DefineID,ObjectID,Period,Flags,origin,interval,limit);
}

HRESULT SimConnectWrapper::RequestDataOnSimObjectType(SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_DATA_DEFINITION_ID DefineID,DWORD dwRadiusMeters,SIMCONNECT_SIMOBJECT_TYPE type)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestDataOnSimObjectType(simConnect,RequestID,DefineID,dwRadiusMeters,type);
}

HRESULT SimConnectWrapper::SetDataOnSimObject(SIMCONNECT_DATA_DEFINITION_ID DefineID,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_SET_FLAG Flags,DWORD ArrayCount,DWORD cbUnitSize,void* pDataSet)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSetDataOnSimObject(simConnect,DefineID,ObjectID,Flags,ArrayCount,cbUnitSize,pDataSet);
}

HRESULT SimConnectWrapper::MapInputEventToClientEvent(SIMCONNECT_INPUT_GROUP_ID GroupID,const char* szInputDefinition,SIMCONNECT_CLIENT_EVENT_ID DownEventID,DWORD DownValue,SIMCONNECT_CLIENT_EVENT_ID UpEventID,DWORD UpValue,BOOL bMaskable)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectMapInputEventToClientEvent(simConnect,GroupID,szInputDefinition,DownEventID,DownValue,UpEventID,UpValue,bMaskable);
}

HRESULT SimConnectWrapper::SetInputGroupPriority(SIMCONNECT_INPUT_GROUP_ID GroupID,DWORD uPriority)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSetInputGroupPriority(simConnect,GroupID,uPriority);
}

HRESULT SimConnectWrapper::RemoveInputEvent(SIMCONNECT_INPUT_GROUP_ID GroupID,const char* szInputDefinition)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRemoveInputEvent(simConnect,GroupID,szInputDefinition);
}

HRESULT SimConnectWrapper::ClearInputGroup(SIMCONNECT_INPUT_GROUP_ID GroupID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectClearInputGroup(simConnect,GroupID);
}

HRESULT SimConnectWrapper::SetInputGroupState(SIMCONNECT_INPUT_GROUP_ID GroupID,DWORD dwState)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSetInputGroupState(simConnect,GroupID,dwState);
}

HRESULT SimConnectWrapper::RequestReservedKey(SIMCONNECT_CLIENT_EVENT_ID EventID,const char* szKeyChoice1,const char* szKeyChoice2,const char* szKeyChoice3)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestReservedKey(simConnect,EventID,szKeyChoice1,szKeyChoice2,szKeyChoice3);
}

HRESULT SimConnectWrapper::SubscribeToSystemEvent(SIMCONNECT_CLIENT_EVENT_ID EventID,const char* SystemEventName)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSubscribeToSystemEvent(simConnect,EventID,SystemEventName);
}

HRESULT SimConnectWrapper::UnsubscribeFromSystemEvent(SIMCONNECT_CLIENT_EVENT_ID EventID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectUnsubscribeFromSystemEvent(simConnect,EventID);
}

HRESULT SimConnectWrapper::WeatherRequestInterpolatedObservation(SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon,float alt)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherRequestInterpolatedObservation(simConnect,RequestID,lat,lon,alt);
}

HRESULT SimConnectWrapper::WeatherRequestObservationAtStation(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherRequestObservationAtStation(simConnect,RequestID,szICAO);
}

HRESULT SimConnectWrapper::WeatherRequestObservationAtNearestStation(SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherRequestObservationAtNearestStation(simConnect,RequestID,lat,lon);
}

HRESULT SimConnectWrapper::WeatherCreateStation(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO,const char* szName,float lat,float lon,float alt)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherCreateStation(simConnect,RequestID,szICAO,szName,lat,lon,alt);
}

HRESULT SimConnectWrapper::WeatherRemoveStation(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherRemoveStation(simConnect,RequestID,szICAO);
}

HRESULT SimConnectWrapper::WeatherSetObservation(DWORD Seconds,const char* szMETAR)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherSetObservation(simConnect,Seconds,szMETAR);
}

HRESULT SimConnectWrapper::WeatherSetModeServer(DWORD dwPort,DWORD dwSeconds)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherSetModeServer(simConnect,dwPort,dwSeconds);
}

HRESULT SimConnectWrapper::WeatherSetModeTheme(const char* szThemeName)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherSetModeTheme(simConnect,szThemeName);
}

HRESULT SimConnectWrapper::WeatherSetModeGlobal()
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherSetModeGlobal(simConnect);
}

HRESULT SimConnectWrapper::WeatherSetModeCustom()
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherSetModeCustom(simConnect);
}

HRESULT SimConnectWrapper::WeatherSetDynamicUpdateRate(DWORD dwRate)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherSetDynamicUpdateRate(simConnect,dwRate);
}

HRESULT SimConnectWrapper::WeatherRequestCloudState(SIMCONNECT_DATA_REQUEST_ID RequestID,float minLat,float minLon,float minAlt,float maxLat,float maxLon,float maxAlt,DWORD dwFlags)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherRequestCloudState(simConnect,RequestID,minLat,minLon,minAlt,maxLat,maxLon,maxAlt,dwFlags);
}

HRESULT SimConnectWrapper::WeatherCreateThermal(SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon,float alt,float radius,float height,float coreRate,float coreTurbulence,float sinkRate,float sinkTurbulence,float coreSize,float coreTransitionSize,float sinkLayerSize,float sinkTransitionSize)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherCreateThermal(simConnect,RequestID,lat,lon,alt,radius,height,coreRate,coreTurbulence,sinkRate,sinkTurbulence,coreSize,coreTransitionSize,sinkLayerSize,sinkTransitionSize);
}

HRESULT SimConnectWrapper::WeatherRemoveThermal(SIMCONNECT_OBJECT_ID ObjectID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectWeatherRemoveThermal(simConnect,ObjectID);
}

HRESULT SimConnectWrapper::AICreateParkedATCAircraft(const char* szContainerTitle,const char* szTailNumber,const char* szAirportID,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAICreateParkedATCAircraft(simConnect,szContainerTitle,szTailNumber,szAirportID,RequestID);
}

HRESULT SimConnectWrapper::AICreateEnrouteATCAircraft(const char* szContainerTitle,const char* szTailNumber,int iFlightNumber,const char* szFlightPlanPath,double dFlightPlanPosition,BOOL bTouchAndGo,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAICreateEnrouteATCAircraft(simConnect,szContainerTitle,szTailNumber,iFlightNumber,szFlightPlanPath,dFlightPlanPosition,bTouchAndGo,RequestID);
}

HRESULT SimConnectWrapper::AICreateNonATCAircraft(const char* szContainerTitle,const char* szTailNumber,SIMCONNECT_DATA_INITPOSITION InitPos,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAICreateNonATCAircraft(simConnect,szContainerTitle,szTailNumber,InitPos,RequestID);
}

HRESULT SimConnectWrapper::AICreateSimulatedObject(const char* szContainerTitle,SIMCONNECT_DATA_INITPOSITION InitPos,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAICreateSimulatedObject(simConnect,szContainerTitle,InitPos,RequestID);
}

HRESULT SimConnectWrapper::AIReleaseControl(SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAIReleaseControl(simConnect,ObjectID,RequestID);
}

HRESULT SimConnectWrapper::AIRemoveObject(SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAIRemoveObject(simConnect,ObjectID,RequestID);
}

HRESULT SimConnectWrapper::AISetAircraftFlightPlan(SIMCONNECT_OBJECT_ID ObjectID,const char* szFlightPlanPath,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAISetAircraftFlightPlan(simConnect,ObjectID,szFlightPlanPath,RequestID);
}

HRESULT SimConnectWrapper::ExecuteMissionAction(const GUID guidInstanceId)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectExecuteMissionAction(simConnect,guidInstanceId);
}

HRESULT SimConnectWrapper::CompleteCustomMissionAction(const GUID guidInstanceId)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectCompleteCustomMissionAction(simConnect,guidInstanceId);
}

HRESULT SimConnectWrapper::Close()
{
	if(!isInitialized)
		return E_FAIL;

	HRESULT hr=SimConnectClose(simConnect);
	simConnect=NULL;
	return hr;
}

HRESULT SimConnectWrapper::RetrieveString(SIMCONNECT_RECV* pData,DWORD cbData,void* pStringV,char** pszString,DWORD* pcbString)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRetrieveString(pData,cbData,pStringV,pszString,pcbString);
}

HRESULT SimConnectWrapper::GetLastSentPacketID(DWORD* pdwError)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectGetLastSentPacketID(simConnect,pdwError);
}

HRESULT SimConnectWrapper::Open(HANDLE* phSimConnect,LPCSTR szName,HWND hWnd,DWORD UserEventWin32,HANDLE hEventHandle,DWORD ConfigIndex)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectOpen(phSimConnect,szName,hWnd,UserEventWin32,hEventHandle,ConfigIndex);
}

HRESULT SimConnectWrapper::CallDispatch(DispatchProc pfcnDispatch,void* pContext)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectCallDispatch(simConnect,pfcnDispatch,pContext);
}

HRESULT SimConnectWrapper::GetNextDispatch(SIMCONNECT_RECV** ppData,DWORD* pcbData)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectGetNextDispatch(simConnect,ppData,pcbData);
}

HRESULT SimConnectWrapper::RequestResponseTimes(DWORD nCount,float* fElapsedSeconds)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestResponseTimes(simConnect,nCount,fElapsedSeconds);
}

HRESULT SimConnectWrapper::InsertString(char* pDest,DWORD cbDest,void** ppEnd,DWORD* pcbStringV,const char* pSource)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectInsertString(pDest,cbDest,ppEnd,pcbStringV,pSource);
}

HRESULT SimConnectWrapper::CameraSetRelative6DOF(float fDeltaX,float fDeltaY,float fDeltaZ,float fPitchDeg,float fBankDeg,float fHeadingDeg)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectCameraSetRelative6DOF(simConnect,fDeltaX,fDeltaY,fDeltaZ,fPitchDeg,fBankDeg,fHeadingDeg);
}

HRESULT SimConnectWrapper::MenuAddItem(const char* szMenuItem,SIMCONNECT_CLIENT_EVENT_ID MenuEventID,DWORD dwData)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectMenuAddItem(simConnect,szMenuItem,MenuEventID,dwData);
}

HRESULT SimConnectWrapper::MenuDeleteItem(SIMCONNECT_CLIENT_EVENT_ID MenuEventID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectMenuDeleteItem(simConnect,MenuEventID);
}

HRESULT SimConnectWrapper::MenuAddSubItem(SIMCONNECT_CLIENT_EVENT_ID MenuEventID,const char* szMenuItem,SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID,DWORD dwData)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectMenuAddSubItem(simConnect,MenuEventID,szMenuItem,SubMenuEventID,dwData);
}

HRESULT SimConnectWrapper::MenuDeleteSubItem(SIMCONNECT_CLIENT_EVENT_ID MenuEventID,const SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectMenuDeleteSubItem(simConnect,MenuEventID,SubMenuEventID);
}

HRESULT SimConnectWrapper::RequestSystemState(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szState)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestSystemState(simConnect,RequestID,szState);
}

HRESULT SimConnectWrapper::SetSystemState(const char* szState,DWORD dwInteger,float fFloat,const char* szString)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSetSystemState(simConnect,szState,dwInteger,fFloat,szString);
}

HRESULT SimConnectWrapper::MapClientDataNameToID(const char* szClientDataName,SIMCONNECT_CLIENT_DATA_ID ClientDataID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectMapClientDataNameToID(simConnect,szClientDataName,ClientDataID);
}

HRESULT SimConnectWrapper::CreateClientData(SIMCONNECT_CLIENT_DATA_ID ClientDataID,DWORD dwSize,SIMCONNECT_CREATE_CLIENT_DATA_FLAG Flags)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectCreateClientData(simConnect,ClientDataID,dwSize,Flags);
}

HRESULT SimConnectWrapper::AddToClientDataDefinition(SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,DWORD dwOffset,DWORD dwSizeOrType,float fEpsilon,DWORD DatumID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectAddToClientDataDefinition(simConnect,DefineID,dwOffset,dwSizeOrType,fEpsilon,DatumID);
}

HRESULT SimConnectWrapper::ClearClientDataDefinition(SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectClearClientDataDefinition(simConnect,DefineID);
}

HRESULT SimConnectWrapper::RequestClientData(SIMCONNECT_CLIENT_DATA_ID ClientDataID,SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,SIMCONNECT_CLIENT_DATA_PERIOD Period,SIMCONNECT_CLIENT_DATA_REQUEST_FLAG Flags,DWORD origin,DWORD interval,DWORD limit)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestClientData(simConnect,ClientDataID,RequestID,DefineID,Period,Flags,origin,interval,limit);
}

HRESULT SimConnectWrapper::SetClientData(SIMCONNECT_CLIENT_DATA_ID ClientDataID,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,SIMCONNECT_CLIENT_DATA_SET_FLAG Flags,DWORD dwReserved,DWORD cbUnitSize,void* pDataSet)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSetClientData(simConnect,ClientDataID,DefineID,Flags,dwReserved,cbUnitSize,pDataSet);
}

HRESULT SimConnectWrapper::FlightLoad(const char* szFileName)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectFlightLoad(simConnect,szFileName);
}

HRESULT SimConnectWrapper::FlightSave(const char* szFileName,const char* szTitle,const char* szDescription,DWORD Flags)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectFlightSave(simConnect,szFileName,szTitle,szDescription,Flags);
}

HRESULT SimConnectWrapper::FlightPlanLoad(const char* szFileName)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectFlightPlanLoad(simConnect,szFileName);
}

HRESULT SimConnectWrapper::Text(SIMCONNECT_TEXT_TYPE type,float fTimeSeconds,SIMCONNECT_CLIENT_EVENT_ID EventID,DWORD cbUnitSize,void* pDataSet)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectText(simConnect,type,fTimeSeconds,EventID,cbUnitSize,pDataSet);
}

HRESULT SimConnectWrapper::SubscribeToFacilities(SIMCONNECT_FACILITY_LIST_TYPE type,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectSubscribeToFacilities(simConnect,type,RequestID);
}

HRESULT SimConnectWrapper::UnsubscribeToFacilities(SIMCONNECT_FACILITY_LIST_TYPE type)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectUnsubscribeToFacilities(simConnect,type);
}

HRESULT SimConnectWrapper::RequestFacilitiesList(SIMCONNECT_FACILITY_LIST_TYPE type,SIMCONNECT_DATA_REQUEST_ID RequestID)
{
	if(!isInitialized)
		return E_FAIL;

	return SimConnectRequestFacilitiesList(simConnect,type,RequestID);
}

