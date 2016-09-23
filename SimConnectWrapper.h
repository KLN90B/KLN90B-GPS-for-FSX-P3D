/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#pragma once

namespace swan2
{
	namespace lib
	{
		namespace fsx
		{
			class SimConnectWrapper : public core::SingletonStatic<SimConnectWrapper>
			{
			private:
				HANDLE		simConnect;
				HMODULE		moduleSimConnect;
				bool		isInitialized;
				std::string	simconnectModuleName;

				void		CheckSimVersion();

			public:
				SimConnectWrapper();
				~SimConnectWrapper();

				bool		Init(const std::string& name="kln90b");
				void		DeInit();

				HANDLE		Get();

				HRESULT		MapClientEventToSimEvent(SIMCONNECT_CLIENT_EVENT_ID EventID,const char* EventName="");
				HRESULT		TransmitClientEvent(SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_CLIENT_EVENT_ID EventID,DWORD dwData,SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_EVENT_FLAG Flags);
				HRESULT		SetSystemEventState(SIMCONNECT_CLIENT_EVENT_ID EventID,SIMCONNECT_STATE dwState);
				HRESULT		AddClientEventToNotificationGroup(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_CLIENT_EVENT_ID EventID,BOOL bMaskable=FALSE);
				HRESULT		RemoveClientEvent(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,SIMCONNECT_CLIENT_EVENT_ID EventID);
				HRESULT		SetNotificationGroupPriority(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,DWORD uPriority);
				HRESULT		ClearNotificationGroup(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID);
				HRESULT		RequestNotificationGroup(SIMCONNECT_NOTIFICATION_GROUP_ID GroupID,DWORD dwReserved=0,DWORD Flags=0);
				HRESULT		AddToDataDefinition(SIMCONNECT_DATA_DEFINITION_ID DefineID,const char* DatumName,const char* UnitsName,SIMCONNECT_DATATYPE DatumType=SIMCONNECT_DATATYPE_FLOAT64,float fEpsilon=0,DWORD DatumID=SIMCONNECT_UNUSED);
				HRESULT		ClearDataDefinition(SIMCONNECT_DATA_DEFINITION_ID DefineID);
				HRESULT		RequestDataOnSimObject(SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_DATA_DEFINITION_ID DefineID,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_PERIOD Period,SIMCONNECT_DATA_REQUEST_FLAG Flags=0,DWORD origin=0,DWORD interval=0,DWORD limit=0);
				HRESULT		RequestDataOnSimObjectType(SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_DATA_DEFINITION_ID DefineID,DWORD dwRadiusMeters,SIMCONNECT_SIMOBJECT_TYPE type);
				HRESULT		SetDataOnSimObject(SIMCONNECT_DATA_DEFINITION_ID DefineID,SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_SET_FLAG Flags,DWORD ArrayCount,DWORD cbUnitSize,void* pDataSet);
				HRESULT		MapInputEventToClientEvent(SIMCONNECT_INPUT_GROUP_ID GroupID,const char* szInputDefinition,SIMCONNECT_CLIENT_EVENT_ID DownEventID,DWORD DownValue=0,SIMCONNECT_CLIENT_EVENT_ID UpEventID=(SIMCONNECT_CLIENT_EVENT_ID)SIMCONNECT_UNUSED,DWORD UpValue=0,BOOL bMaskable=FALSE);
				HRESULT		SetInputGroupPriority(SIMCONNECT_INPUT_GROUP_ID GroupID,DWORD uPriority);
				HRESULT		RemoveInputEvent(SIMCONNECT_INPUT_GROUP_ID GroupID,const char* szInputDefinition);
				HRESULT		ClearInputGroup(SIMCONNECT_INPUT_GROUP_ID GroupID);
				HRESULT		SetInputGroupState(SIMCONNECT_INPUT_GROUP_ID GroupID,DWORD dwState);
				HRESULT		RequestReservedKey(SIMCONNECT_CLIENT_EVENT_ID EventID,const char* szKeyChoice1="",const char* szKeyChoice2="",const char* szKeyChoice3="");
				HRESULT		SubscribeToSystemEvent(SIMCONNECT_CLIENT_EVENT_ID EventID,const char* SystemEventName);
				HRESULT		UnsubscribeFromSystemEvent(SIMCONNECT_CLIENT_EVENT_ID EventID);
				HRESULT		WeatherRequestInterpolatedObservation(SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon,float alt);
				HRESULT		WeatherRequestObservationAtStation(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO);
				HRESULT		WeatherRequestObservationAtNearestStation(SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon);
				HRESULT		WeatherCreateStation(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO,const char* szName,float lat,float lon,float alt);
				HRESULT		WeatherRemoveStation(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szICAO);
				HRESULT		WeatherSetObservation(DWORD Seconds,const char* szMETAR);
				HRESULT		WeatherSetModeServer(DWORD dwPort,DWORD dwSeconds);
				HRESULT		WeatherSetModeTheme(const char* szThemeName);
				HRESULT		WeatherSetModeGlobal();
				HRESULT		WeatherSetModeCustom();
				HRESULT		WeatherSetDynamicUpdateRate(DWORD dwRate);
				HRESULT		WeatherRequestCloudState(SIMCONNECT_DATA_REQUEST_ID RequestID,float minLat,float minLon,float minAlt,float maxLat,float maxLon,float maxAlt,DWORD dwFlags=0);
				HRESULT		WeatherCreateThermal(SIMCONNECT_DATA_REQUEST_ID RequestID,float lat,float lon,float alt,float radius,float height,float coreRate=3.0f,float coreTurbulence=0.05f,float sinkRate=3.0f,float sinkTurbulence=0.2f,float coreSize=0.4f,float coreTransitionSize=0.1f,float sinkLayerSize=0.4f,float sinkTransitionSize=0.1f);
				HRESULT		WeatherRemoveThermal(SIMCONNECT_OBJECT_ID ObjectID);
				HRESULT		AICreateParkedATCAircraft(const char* szContainerTitle,const char* szTailNumber,const char* szAirportID,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		AICreateEnrouteATCAircraft(const char* szContainerTitle,const char* szTailNumber,int iFlightNumber,const char* szFlightPlanPath,double dFlightPlanPosition,BOOL bTouchAndGo,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		AICreateNonATCAircraft(const char* szContainerTitle,const char* szTailNumber,SIMCONNECT_DATA_INITPOSITION InitPos,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		AICreateSimulatedObject(const char* szContainerTitle,SIMCONNECT_DATA_INITPOSITION InitPos,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		AIReleaseControl(SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		AIRemoveObject(SIMCONNECT_OBJECT_ID ObjectID,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		AISetAircraftFlightPlan(SIMCONNECT_OBJECT_ID ObjectID,const char* szFlightPlanPath,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		ExecuteMissionAction(const GUID guidInstanceId);
				HRESULT		CompleteCustomMissionAction(const GUID guidInstanceId);
				HRESULT		Close();
				HRESULT		RetrieveString(SIMCONNECT_RECV* pData,DWORD cbData,void* pStringV,char** pszString,DWORD* pcbString);
				HRESULT		GetLastSentPacketID(DWORD* pdwError);
				HRESULT		Open(HANDLE* phSimConnect,LPCSTR szName,HWND hWnd,DWORD UserEventWin32,HANDLE hEventHandle,DWORD ConfigIndex);
				HRESULT		CallDispatch(DispatchProc pfcnDispatch,void* pContext);
				HRESULT		GetNextDispatch(SIMCONNECT_RECV** ppData,DWORD* pcbData);
				HRESULT		RequestResponseTimes(DWORD nCount,float* fElapsedSeconds);
				HRESULT		InsertString(char* pDest,DWORD cbDest,void** ppEnd,DWORD* pcbStringV,const char* pSource);
				HRESULT		CameraSetRelative6DOF(float fDeltaX,float fDeltaY,float fDeltaZ,float fPitchDeg,float fBankDeg,float fHeadingDeg);
				HRESULT		MenuAddItem(const char* szMenuItem,SIMCONNECT_CLIENT_EVENT_ID MenuEventID,DWORD dwData);
				HRESULT		MenuDeleteItem(SIMCONNECT_CLIENT_EVENT_ID MenuEventID);
				HRESULT		MenuAddSubItem(SIMCONNECT_CLIENT_EVENT_ID MenuEventID,const char* szMenuItem,SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID,DWORD dwData);
				HRESULT		MenuDeleteSubItem(SIMCONNECT_CLIENT_EVENT_ID MenuEventID,const SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID);
				HRESULT		RequestSystemState(SIMCONNECT_DATA_REQUEST_ID RequestID,const char* szState);
				HRESULT		SetSystemState(const char* szState,DWORD dwInteger,float fFloat,const char* szString);
				HRESULT		MapClientDataNameToID(const char* szClientDataName,SIMCONNECT_CLIENT_DATA_ID ClientDataID);
				HRESULT		CreateClientData(SIMCONNECT_CLIENT_DATA_ID ClientDataID,DWORD dwSize,SIMCONNECT_CREATE_CLIENT_DATA_FLAG Flags);
				HRESULT		AddToClientDataDefinition(SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,DWORD dwOffset,DWORD dwSizeOrType,float fEpsilon=0,DWORD DatumID=SIMCONNECT_UNUSED);
				HRESULT		ClearClientDataDefinition(SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID);
				HRESULT		RequestClientData(SIMCONNECT_CLIENT_DATA_ID ClientDataID,SIMCONNECT_DATA_REQUEST_ID RequestID,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,SIMCONNECT_CLIENT_DATA_PERIOD Period=SIMCONNECT_CLIENT_DATA_PERIOD_ONCE,SIMCONNECT_CLIENT_DATA_REQUEST_FLAG Flags=0,DWORD origin=0,DWORD interval=0,DWORD limit=0);
				HRESULT		SetClientData(SIMCONNECT_CLIENT_DATA_ID ClientDataID,SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID,SIMCONNECT_CLIENT_DATA_SET_FLAG Flags,DWORD dwReserved,DWORD cbUnitSize,void* pDataSet);
				HRESULT		FlightLoad(const char* szFileName);
				HRESULT		FlightSave(const char* szFileName,const char* szTitle,const char* szDescription,DWORD Flags);
				HRESULT		FlightPlanLoad(const char* szFileName);
				HRESULT		Text(SIMCONNECT_TEXT_TYPE type,float fTimeSeconds,SIMCONNECT_CLIENT_EVENT_ID EventID,DWORD cbUnitSize,void* pDataSet);
				HRESULT		SubscribeToFacilities(SIMCONNECT_FACILITY_LIST_TYPE type,SIMCONNECT_DATA_REQUEST_ID RequestID);
				HRESULT		UnsubscribeToFacilities(SIMCONNECT_FACILITY_LIST_TYPE type);
				HRESULT		RequestFacilitiesList(SIMCONNECT_FACILITY_LIST_TYPE type,SIMCONNECT_DATA_REQUEST_ID RequestID);
			};
		}
	}
}

#define SIM swan2::lib::fsx::SimConnectWrapper::Instance()
