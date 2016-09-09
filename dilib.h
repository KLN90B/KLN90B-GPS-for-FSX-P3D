#define DIRECTINPUT_VERSION 0x0800


#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//***********************************************************************************************
#define GET_DJOY_X(dj,n)        (LONG)dj[n].djoy_state.lX             
#define GET_DJOY_Y(dj,n)        (LONG)dj[n].djoy_state.lY              
#define GET_DJOY_Z(dj,n)        (LONG)dj[n].djoy_state.lZ
#define GET_DJOY_RX(dj,n)       (LONG)dj[n].djoy_state.lRx
#define GET_DJOY_RY(dj,n)       (LONG)dj[n].djoy_state.lRy
#define GET_DJOY_RZ(dj,n)       (LONG)dj[n].djoy_state.lRz
#define GET_DJOY_U(dj,n)        (LONG)dj[n].djoy_state.rglSlider[0]
#define GET_DJOY_V(dj,n)        (LONG)dj[n].djoy_state.rglSlider[1]
#define GET_DJOY_POV1(dj,n)     (LONG)dj[n].djoy_state.rgdwPOV[0]
#define GET_DJOY_POV2(dj,n)     (LONG)dj[n].djoy_state.rgdwPOV[1]
#define GET_DJOY_POV3(dj,n)     (LONG)dj[n].djoy_state.rgdwPOV[2]
#define GET_DJOY_POV4(dj,n)     (LONG)dj[n].djoy_state.rgdwPOV[3]
#define GET_DJOY_VX(dj,n)       (LONG)dj[n].djoy_state.lVX
#define GET_DJOY_VY(dj,n)       (LONG)dj[n].djoy_state.lVY
#define GET_DJOY_VZ(dj,n)       (LONG)dj[n].djoy_state.lVZ
#define GET_DJOY_VRX(dj,n)      (LONG)dj[n].djoy_state.lVRx
#define GET_DJOY_VRY(dj,n)      (LONG)dj[n].djoy_state.lVRy
#define GET_DJOY_VRZ(dj,n)      (LONG)dj[n].djoy_state.lVRz
#define GET_DJOY_VU(dj,n)       (LONG)dj[n].djoy_state.rglVSlider[0]
#define GET_DJOY_VV(dj,n)       (LONG)dj[n].djoy_state.rglVSlider[1]
#define GET_DJOY_AX(dj,n)       (LONG)dj[n].djoy_state.lAX
#define GET_DJOY_AY(dj,n)       (LONG)dj[n].djoy_state.lAY
#define GET_DJOY_AZ(dj,n)       (LONG)dj[n].djoy_state.lAZ
#define GET_DJOY_ARX(dj,n)      (LONG)dj[n].djoy_state.lARx
#define GET_DJOY_ARY(dj,n)      (LONG)dj[n].djoy_state.lARy
#define GET_DJOY_ARZ(dj,n)      (LONG)dj[n].djoy_state.lARz
#define GET_DJOY_AU(dj,n)       (LONG)dj[n].djoy_state.rglASlider[0]
#define GET_DJOY_AV(dj,n)       (LONG)dj[n].djoy_state.rglASlider[1]
#define GET_DJOY_FX(dj,n)	    (LONG)dj[n].djoy_state.lFX
#define GET_DJOY_FY(dj,n)	    (LONG)dj[n].djoy_state.lFY
#define GET_DJOY_FZ(dj,n)	    (LONG)dj[n].djoy_state.lFZ
#define GET_DJOY_FRX(dj,n)	    (LONG)dj[n].djoy_state.lFRx
#define GET_DJOY_FRY(dj,n)	    (LONG)dj[n].djoy_state.lFRy
#define GET_DJOY_FRZ(dj,n)	    (LONG)dj[n].djoy_state.lFRz
#define GET_DJOY_FU(dj,n)		(LONG)dj[n].djoy_state.rglFSlider[0]
#define GET_DJOY_FV(dj,n)		(LONG)dj[n].djoy_state.rglFSlider[1]
#define GET_DJOY_BUTTON(dj,n,m) (BYTE)((dj[n].djoy_state.rgbButtons[m-1])>>7)
//***********************************************************************************************
#define GET_AXES_COUNT(dj,n)    (dj)[(n)].axes_arr_count
#define GET_BUTTONS_COUNT(dj,n) (dj)[(n)].buttons_arr_count
#define GET_POVS_COUNT(dj,n)    (dj)[(n)].povs_arr_count
//***********************************************************************************************
#define AXIS_UNIQ 0
#define AXIS_SLIDER 0x10000

#pragma pack(4)
typedef struct _axis_info
{
	GUID axis_type;
	TCHAR axis_name[MAX_PATH];
	DWORD axis_subtype;
	DWORD axis_spec;
}axis_info;

typedef struct _button_info
{
	GUID button_type;
	TCHAR button_name[MAX_PATH];
	DWORD button_subtype;
}button_info;
typedef struct _pov_info
{
	GUID pov_type;
	TCHAR pov_name[MAX_PATH];
	DWORD pov_subtype;
}pov_info;

typedef struct _djoy_info
{
	GUID  device_id;
	TCHAR device_name[MAX_PATH];
	LPDIRECTINPUTDEVICE8 device_object;
	DWORD                count_of_axes;
	DWORD                count_of_buttons;
	DWORD                count_of_povs;
	axis_info            *axes;
	int                  axes_arr_count;
	button_info          *buttons;
	int                  buttons_arr_count;
	pov_info             *povs;
	int                  povs_arr_count;
	DIJOYSTATE2			djoy_state;
}djoy_info;
#pragma pack()

#define DI_device_id(dj)        (GUID)(dj)->device_id
#define DI_device_name(dj)      (TCHAR *)(dj)->device_name
#define DI_count_of_axes(dj)    (DWORD)(dj)->count_of_axes
#define DI_count_of_buttons(dj) (DWORD)(dj)->count_of_buttons
#define DI_count_of_povs(dj)    (DWORD)(dj)->count_of_povs
#define DI_djoy_state(dj)       (DIJOYSTATE2)(dj)->djoy_state;
//**********************************************************************************************
enum {OBJECT_AXIS,OBJECT_BUTTON,OBJECT_POV};
#define GUID_STR_LEN sizeof(TCHAR)*33

#ifndef __cplusplus
int init_dilib(void);
int deinit_dilib(void);
int get_djoys_state(djoy_info **djoy_array);
int GET_Djoy_count(void);
djoy_info *GET_DjoyInfo_By_GUID(GUID *__guid);
GUID *GET_DjoyGUID(int djoy_number);
DWORD GET_Axis_Value(int joy_number,int axis_index);
BOOL Is_Button_Pressed(int joy_number,int button_index);
DWORD GET_POV_Value(int joy_number,int pov_index);
TCHAR *GET_Djoy_Name(int djoy_number);
TCHAR *GET_Object_Name(int djoy_number,int object_type,int index);
djoy_info *get_djoy_state(int djoy_index);
DWORD DI_axis_value(djoy_info *dj,int index);
BYTE  DI_button_value(djoy_info *dj,int index);
DWORD DI_pov_value(djoy_info *dj,int index);
int GuidToStr(const GUID *guid,TCHAR *OutBuffer);
#else
extern "C" int init_dilib(void);
extern "C" int deinit_dilib(void);
extern "C" int get_djoys_state(djoy_info **djoy_array);
extern "C" int GET_Djoy_count(void);
extern "C" djoy_info *GET_DjoyInfo_By_GUID(GUID *__guid);
extern "C" GUID *GET_DjoyGUID(int djoy_number);
extern "C" DWORD GET_Axis_Value(int joy_number,int axis_index);
extern "C" BOOL Is_Button_Pressed(int joy_number,int button_index);
extern "C" DWORD GET_POV_Value(int joy_number,int pov_index);
extern "C" TCHAR *GET_Djoy_Name(int djoy_number);
extern "C" TCHAR *GET_Object_Name(int djoy_number,int object_type,int index);
extern "C" djoy_info *get_djoy_state(int djoy_index);
extern "C" DWORD DI_axis_value(djoy_info *dj,int index);
extern "C" BYTE  DI_button_value(djoy_info *dj,int index);
extern "C" DWORD DI_pov_value(djoy_info *dj,int index);
extern "C" int GuidToStr(const GUID *guid,TCHAR *OutBuffer);
#endif
