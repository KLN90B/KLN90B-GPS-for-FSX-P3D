#include "PCH.h"
#ifdef KLN_FOR_USSS
#include "fslib.h"
#include "resource.h"
#include "kln90b.h"
#include "fs_gauge.h"
#include <afxext.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "dilib.h"

static djoy_info djoys[1024]={0};
static int num_of_djoys=0;
static LPDIRECTINPUT8  di_object;
static LONG volatile inited=0; 
static CRITICAL_SECTION crit_sec;


extern "C" int GuidToStr(const GUID *guid,TCHAR *OutBuffer)
{
	if(!guid || !OutBuffer)
		return(0);
	sprintf(OutBuffer,"%.8X_%.4X_%.4X_%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",guid->Data1,guid->Data2,guid->Data3,
		guid->Data4[0],guid->Data4[1],guid->Data4[2],
		guid->Data4[3],guid->Data4[4],guid->Data4[5],
		guid->Data4[6],guid->Data4[7]);
	return(1);
}
//***********************************************************************************************
static BOOL CALLBACK EnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi,LPVOID pvRef)
{

	djoy_info *curr_djoy = (djoy_info *)pvRef;


	if(!(curr_djoy && curr_djoy->device_object))
		return DIENUM_CONTINUE;

	if( lpddoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg; 
		diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diprg.diph.dwHow        = DIPH_BYID; 
		diprg.diph.dwObj        = lpddoi->dwType; // Specify the enumerated axis
		diprg.lMin              = -16383; 
		diprg.lMax              = +16383; 
		if( FAILED( (curr_djoy->device_object)->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
			return DIENUM_CONTINUE;
	}

	if(
		lpddoi->guidType == GUID_XAxis  ||
		lpddoi->guidType == GUID_YAxis  ||
		lpddoi->guidType == GUID_ZAxis  || 
		lpddoi->guidType == GUID_RxAxis ||
		lpddoi->guidType == GUID_RyAxis ||
		lpddoi->guidType == GUID_RzAxis ||
		lpddoi->guidType == GUID_Slider 
		)
	{
		if(curr_djoy->axes)
		{
			memcpy(curr_djoy->axes[curr_djoy->axes_arr_count].axis_name,lpddoi->tszName,sizeof(TCHAR)*MAX_PATH);
			curr_djoy->axes[curr_djoy->axes_arr_count].axis_type = lpddoi->guidType;
			curr_djoy->axes[curr_djoy->axes_arr_count].axis_subtype = lpddoi->dwType;
			curr_djoy->axes[curr_djoy->axes_arr_count].axis_spec = AXIS_UNIQ;
		}
		if(lpddoi->guidType == GUID_Slider)
		{
			int slider_count = 0;
			for(int i=0;i<curr_djoy->axes_arr_count;i++)
			{
				if(curr_djoy->axes[i].axis_spec & AXIS_SLIDER)
					slider_count++;
			}
			curr_djoy->axes[curr_djoy->axes_arr_count].axis_spec = AXIS_SLIDER | slider_count;
		}
		curr_djoy->axes_arr_count++;
	}
	if( lpddoi->guidType == GUID_Button )
	{
		if(curr_djoy->buttons)
		{
			memcpy(curr_djoy->buttons[curr_djoy->buttons_arr_count].button_name,lpddoi->tszName,sizeof(TCHAR)*MAX_PATH);
			curr_djoy->buttons[curr_djoy->buttons_arr_count].button_type = lpddoi->guidType;
			curr_djoy->buttons[curr_djoy->buttons_arr_count].button_subtype = lpddoi->dwType;
			curr_djoy->buttons_arr_count++;
		}
	}
	if( lpddoi->guidType == GUID_POV    )
	{
		if(curr_djoy->povs)
		{
			memcpy(curr_djoy->povs[curr_djoy->povs_arr_count].pov_name,lpddoi->tszName,sizeof(TCHAR)*MAX_PATH);
			curr_djoy->povs[curr_djoy->povs_arr_count].pov_type = lpddoi->guidType;
			curr_djoy->povs[curr_djoy->povs_arr_count].pov_subtype = lpddoi->dwType;
			curr_djoy->povs_arr_count++;
		}
	}
	return DIENUM_CONTINUE;
}
//***********************************************************************************************
static BOOL CALLBACK DIEnumDevices(LPCDIDEVICEINSTANCE lpddi,LPVOID pvRef)
{

	LPDIRECTINPUT8 di_obj = (LPDIRECTINPUT8)pvRef;
	HRESULT result;
	DIDEVCAPS dev_caps;

	if(!di_obj)
		return(DIENUM_CONTINUE);

	BYTE type = LOBYTE(LOWORD(lpddi->dwDevType));

	if(type == DI8DEVTYPE_JOYSTICK ||
		type == DI8DEVTYPE_1STPERSON ||
		type == DI8DEVTYPE_DRIVING ||
		type == DI8DEVTYPE_FLIGHT ||
		type == DI8DEVTYPE_GAMEPAD ||
		type == DI8DEVTYPE_SUPPLEMENTAL
		)
	{
		djoys[num_of_djoys].device_id = lpddi->guidInstance;
		memcpy(djoys[num_of_djoys].device_name,lpddi->tszInstanceName,sizeof(TCHAR)*MAX_PATH);
		result = di_obj->CreateDevice( lpddi->guidInstance, &(djoys[num_of_djoys].device_object), NULL );
		if(FAILED(result))
		{
			return(DIENUM_CONTINUE);
		}
		dev_caps.dwSize = sizeof(dev_caps);
		result = (djoys[num_of_djoys].device_object)->GetCapabilities(&dev_caps);
		if(FAILED(result))
		{
			return(DIENUM_CONTINUE);
		}

		djoys[num_of_djoys].count_of_axes    = dev_caps.dwAxes;
		djoys[num_of_djoys].count_of_buttons = dev_caps.dwButtons;
		djoys[num_of_djoys].count_of_povs    = dev_caps.dwPOVs;

		djoys[num_of_djoys].axes   = (axis_info *)   HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(axis_info)*dev_caps.dwAxes);
		if(!djoys[num_of_djoys].axes)
			djoys[num_of_djoys].axes_arr_count=-1;
		else
			djoys[num_of_djoys].axes_arr_count=0;

		djoys[num_of_djoys].buttons= (button_info *) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(button_info)*dev_caps.dwButtons);
		if(!djoys[num_of_djoys].buttons)
			djoys[num_of_djoys].buttons_arr_count=-1;
		else
			djoys[num_of_djoys].buttons_arr_count=0;

		djoys[num_of_djoys].povs   = (pov_info *)    HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(pov_info)*dev_caps.dwPOVs);
		if(!djoys[num_of_djoys].povs)
			djoys[num_of_djoys].povs_arr_count=-1;
		else
			djoys[num_of_djoys].povs_arr_count=0;

		result = (djoys[num_of_djoys].device_object)->EnumObjects(EnumDeviceObjectsCallback,&(djoys[num_of_djoys]),DIDFT_ALL);
		if(FAILED(result))
		{
			return(DIENUM_CONTINUE);
		}
		num_of_djoys++;
	}
	return(DIENUM_CONTINUE);
}
#undef InterlockedCompareExchange
#define InterlockedCompareExchange _InterlockedCompareExchange
extern "C" __declspec(dllimport)  LONG WINAPI InterlockedCompareExchange(LONG volatile* Destination,LONG Exchange,LONG Comperand);
#pragma intrinsic(_InterlockedCompareExchange)

extern "C" int init_dilib(void)
{
	HRESULT         result; 

	if(inited)
		return(0);
	if(inited == InterlockedCompareExchange(&inited,1,0))
		return(0);
	num_of_djoys=0;

	result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,IID_IDirectInput8, (void**)&di_object, NULL); 
	if(FAILED(result))
	{
		return(-1);
	}
	result = di_object->EnumDevices(DI8DEVCLASS_GAMECTRL,DIEnumDevices,di_object,DIEDFL_ATTACHEDONLY);
	if(FAILED(result))
	{
		return(-2);
	}
	for(int i=0;i<num_of_djoys;i++)
	{
		result = djoys[i].device_object->SetDataFormat( &c_dfDIJoystick2 );
		DIPROPDWORD auto_cent;
		auto_cent.diph.dwSize       = sizeof(DIPROPDWORD); 
		auto_cent.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		auto_cent.diph.dwHow        = DIPH_DEVICE; 
		auto_cent.diph.dwObj        = 0;//lpddoi->dwType; // Specify the enumerated axis
		auto_cent.dwData            = DIPROPAUTOCENTER_ON;
		result = (djoys[i].device_object)->SetProperty(DIPROP_AUTOCENTER,&auto_cent.diph);
		//	   result = djoys[i].device_object->SetCooperativeLevel( NULL, DISCL_EXCLUSIVE | 
	}
	InitializeCriticalSection(&crit_sec);
	return(0);
}
//***********************************************************************************************
extern "C" int deinit_dilib(void)
{
	for(int i=0;i<num_of_djoys;i++)
	{
		if(djoys[i].device_object)
			SAFE_RELEASE(djoys[i].device_object);
		if(djoys[i].axes)
			HeapFree(GetProcessHeap(),0,djoys[i].axes);
		if(djoys[i].buttons)
			HeapFree(GetProcessHeap(),0,djoys[i].buttons);
		if(djoys[i].povs)
			HeapFree(GetProcessHeap(),0,djoys[i].povs);
	}
	if(di_object)
		SAFE_RELEASE(di_object);
	DeleteCriticalSection(&crit_sec);
	InterlockedCompareExchange(&inited,0,1);
	return(0);
}
//***********************************************************************************************
extern "C" djoy_info *get_djoy_state(int djoy_index)
{

	if(!((djoy_index < num_of_djoys) && (djoy_index >= 0)))
		return(0);
	if(!djoys[djoy_index].device_object)
		return(0);

	HRESULT         result; 

	EnterCriticalSection(&crit_sec);
	while(1)
	{ 
		result = (djoys[djoy_index].device_object)->Poll(); 
		if( FAILED(result) )  
		{
			result = (djoys[djoy_index].device_object)->Acquire();
			while( result == DIERR_INPUTLOST ) 
				result = (djoys[djoy_index].device_object)->Acquire();
		}
		else
		{
			result = djoys[djoy_index].device_object->GetDeviceState( sizeof(DIJOYSTATE2), &djoys[djoy_index].djoy_state ) ;
			if( !FAILED( result ) )
		 {
			 LeaveCriticalSection(&crit_sec);
			 return(&djoys[djoy_index]);
		 }
		}
	}
	LeaveCriticalSection(&crit_sec);
}
//***********************************************************************************************
extern "C" int get_djoys_state(djoy_info **djoy_array)
{
	for(int i=0;i<num_of_djoys;i++)
	{
		get_djoy_state(i);
	}
	*djoy_array=djoys;
	return(i);
}
//***********************************************************************************************
extern "C" int GET_Djoy_count(void)
{
	return(num_of_djoys);
}
//***********************************************************************************************
extern "C" djoy_info *GET_DjoyInfo_By_GUID(GUID *__guid)
{
	for(int i=0;i<num_of_djoys;i++);
	{
		if(djoys[i].device_id == *__guid)
			return(&djoys[i]);
	}
	return(NULL);
}
//***********************************************************************************************
extern "C" GUID *GET_DjoyGUID(int djoy_number)
{
	if(djoy_number < num_of_djoys)
		return(&(djoys[djoy_number].device_id));
	return(NULL);
}
//***********************************************************************************************
extern "C" DWORD GET_Axis_Value(int joy_number,int axis_index)
{
	if(!((joy_number < num_of_djoys) && (joy_number >= 0)))
		return(0);
	if(!((axis_index < djoys[joy_number].axes_arr_count) && (axis_index >= 0)))
		return(0);

	if(djoys[joy_number].axes[axis_index].axis_type == GUID_XAxis)
		return(djoys[joy_number].djoy_state.lX);
	if(djoys[joy_number].axes[axis_index].axis_type == GUID_YAxis)
		return(djoys[joy_number].djoy_state.lY);
	if(djoys[joy_number].axes[axis_index].axis_type == GUID_ZAxis)
		return(djoys[joy_number].djoy_state.lZ);
	if(djoys[joy_number].axes[axis_index].axis_type == GUID_RxAxis)
		return(djoys[joy_number].djoy_state.lRx);
	if(djoys[joy_number].axes[axis_index].axis_type == GUID_RyAxis)
		return(djoys[joy_number].djoy_state.lRy);
	if(djoys[joy_number].axes[axis_index].axis_type == GUID_RzAxis)
		return(djoys[joy_number].djoy_state.lRz);

	if(djoys[joy_number].axes[axis_index].axis_type == GUID_Slider)
	{
		int slider_index = LOWORD(djoys[joy_number].axes[axis_index].axis_spec);
		return(djoys[joy_number].djoy_state.rglSlider[slider_index]);
	}
	return(0);
}
//***********************************************************************************************
extern "C" BOOL Is_Button_Pressed(int joy_number,int button_index)
{
	if(!((joy_number < num_of_djoys) && (joy_number >= 0)))
		return(FALSE);
	if(!((button_index < djoys[joy_number].buttons_arr_count) && (button_index >= 0)))
		return(FALSE);
	return(djoys[joy_number].djoy_state.rgbButtons[button_index]>>7);
}
//***********************************************************************************************
extern "C" DWORD GET_POV_Value(int joy_number,int pov_index)
{
	if(!((joy_number < num_of_djoys) && (joy_number >= 0)))
		return(-2);
	if(!((pov_index < djoys[joy_number].povs_arr_count ) && (pov_index >= 0)))
		return(-2);
	return(djoys[joy_number].djoy_state.rgdwPOV[pov_index]);
}
//***********************************************************************************************
extern "C" TCHAR *GET_Djoy_Name(int djoy_number)
{
	if(!((djoy_number < num_of_djoys) && (djoy_number >= 0)))
		return(NULL);
	return(djoys[djoy_number].device_name);
}
//***********************************************************************************************
extern "C" TCHAR *GET_Object_Name(int djoy_number,int object_type,int index)
{
	if(!((djoy_number < num_of_djoys) && (djoy_number >= 0)))
		return(NULL);          

	switch(object_type)
	{
	case OBJECT_AXIS:
		if(!((index < djoys[djoy_number].axes_arr_count) && (index >= 0)))
			return(NULL);
		return(djoys[djoy_number].axes[index].axis_name);
		break;
	case OBJECT_BUTTON:
		if(!((index < djoys[djoy_number].buttons_arr_count) && (index >= 0)))
			return(NULL);
		return(djoys[djoy_number].buttons[index].button_name);
		break;
	case OBJECT_POV:
		if(!((index < djoys[djoy_number].povs_arr_count ) && (index >= 0)))
			return(NULL);
		return(djoys[djoy_number].povs[index].pov_name);
		break;
	}
	return(NULL);
}
//***********************************************************************************************
extern "C" DWORD DI_axis_value(djoy_info *dj,int index)
{
	int i;
	for(i=0;i<num_of_djoys;i++)
	{
		if(djoys[i].device_id == dj->device_id)
			return(GET_Axis_Value(i,index));
	}
	return(0);
}
//***********************************************************************************************
extern "C" BYTE  DI_button_value(djoy_info *dj,int index)
{
	int i;
	for(i=0;i<num_of_djoys;i++)
	{
		if(djoys[i].device_id == dj->device_id)
			return(Is_Button_Pressed(i,index));
	}
	return(0);

}
//***********************************************************************************************
extern "C" DWORD DI_pov_value(djoy_info *dj,int index)
{
	int i;
	for(i=0;i<num_of_djoys;i++)
	{
		if(djoys[i].device_id == dj->device_id)
			return(GET_POV_Value(i,index));
	}
	return(0);
}
//***********************************************************************************************

typedef struct _inp_evnt_t
{
   char *event_name;
   long event_code;
}inp_evnt_t;

typedef struct _inp_mapping_t
{
   long djoy_number;
   long button_number;
   long input_event;
   long press_count;
}inp_mapping_t;

#include <vector>
using namespace std;

vector<inp_mapping_t > input_vec;

const int input_events_count=18;

inp_evnt_t events[input_events_count] = 
{
   {"ON/OFF button",INPUT_ONOFF},
   {"Brightness more knob",INPUT_BTNMORE},
   {"Brightness less knob",INPUT_BTNLESS},
   {"Right Outer Knob to the right",INPUT_ROUTERPLUS},
   {"Right Outer Knob to the left",INPUT_ROUTERMINUS},
   {"Right Inner Knob to the right",INPUT_RINNERPLUS},
   {"Right Inner Knob to the left",INPUT_RINNERMINUS},
   {"ENT button",INPUT_ENTER},
   {"Right Inner Knob to the right",INPUT_LINNERPLUS},
   {"Right Inner Knob to the left",INPUT_LINNERMINUS},
   {"D-> button",INPUT_DTO},
   {"Right CRSR button",INPUT_RCURSOR},
   {"Left Outer Knob to the left",INPUT_LOUTERMINUS},
   {"Left Outer Knob to the right",INPUT_LOUTERPLUS},
   {"PULL SCAN Knob",INPUT_PULLSCAN},
   {"CLR button",INPUT_CLR},
   {"Left CRSR button",INPUT_LCURSOR},
   {"MSG button",INPUT_MSG},
   {"AP HDG Mode button",INPUT_APON}
};

void load_di_config(void);

char *event2str(long event)
{
	switch(event)
	{
    case INPUT_ONOFF:
	   return("INPUT_ONOFF");
	case INPUT_BTNMORE:
	   return("INPUT_BTNMORE");
	case INPUT_BTNLESS:
	   return("INPUT_BTNLESS");
	case INPUT_ROUTERPLUS:
	   return("INPUT_ROUTERPLUS");
	case INPUT_ROUTERMINUS:
	   return("INPUT_ROUTERMINUS");
	case INPUT_RINNERPLUS:
	   return("INPUT_RINNERPLUS");
	case INPUT_RINNERMINUS:
	   return("INPUT_RINNERMINUS");
	case INPUT_ENTER:
	   return("INPUT_ENTER");
	case INPUT_LINNERPLUS:
	   return("INPUT_LINNERPLUS");
	case INPUT_LINNERMINUS:
	   return("INPUT_LINNERMINUS");
	case INPUT_DTO:
	   return("INPUT_DTO");
	case INPUT_RCURSOR:
	   return("INPUT_RCURSOR");
	case INPUT_LOUTERMINUS:
	   return("INPUT_LOUTERMINUS");
	case INPUT_LOUTERPLUS:
	   return("INPUT_LOUTERPLUS");
	case INPUT_PULLSCAN:
	   return("INPUT_PULLSCAN");
	case INPUT_CLR:
	   return("INPUT_CLR");
	case INPUT_LCURSOR:
	   return("INPUT_LCURSOR");
	case INPUT_MSG:
	   return("INPUT_MSG");
	case INPUT_APON:
		return("INPUT_APON");
	}
	return("");
}
long str2event(char *event_str)
{
	if(!stricmp(event_str,"INPUT_ONOFF"))
		return INPUT_ONOFF;
	if(!stricmp(event_str,"INPUT_BTNMORE"))
		return INPUT_BTNMORE;
	if(!stricmp(event_str,"INPUT_BTNLESS"))
		return INPUT_BTNLESS;
	if(!stricmp(event_str,"INPUT_ROUTERPLUS"))
		return INPUT_ROUTERPLUS;
	if(!stricmp(event_str,"INPUT_ROUTERMINUS"))
		return INPUT_ROUTERMINUS;
	if(!stricmp(event_str,"INPUT_RINNERPLUS"))
		return INPUT_RINNERPLUS;
	if(!stricmp(event_str,"INPUT_RINNERMINUS"))
		return INPUT_RINNERMINUS;
	if(!stricmp(event_str,"INPUT_ENTER"))
		return INPUT_ENTER;
	if(!stricmp(event_str,"INPUT_LINNERPLUS"))
		return INPUT_LINNERPLUS;
	if(!stricmp(event_str,"INPUT_LINNERMINUS"))
		return INPUT_LINNERMINUS;
	if(!stricmp(event_str,"INPUT_DTO"))
		return INPUT_DTO;
	if(!stricmp(event_str,"INPUT_RCURSOR"))
		return INPUT_RCURSOR;
	if(!stricmp(event_str,"INPUT_LOUTERMINUS"))
		return INPUT_LOUTERMINUS;
	if(!stricmp(event_str,"INPUT_LOUTERPLUS"))
		return INPUT_LOUTERPLUS;
	if(!stricmp(event_str,"INPUT_PULLSCAN"))
		return INPUT_PULLSCAN;
	if(!stricmp(event_str,"INPUT_CLR"))
		return INPUT_CLR;
	if(!stricmp(event_str,"INPUT_LCURSOR"))
	   return INPUT_LCURSOR;
	if(!stricmp(event_str,"INPUT_MSG"))
	   return INPUT_MSG;
	if(!stricmp(event_str,"INPUT_APON"))
		return INPUT_APON;
	return(-1);
}

class cDialog : public CDialog
{
public:
	BOOL OnInitDialog()
	{
		BOOL ret = CDialog::OnInitDialog();

		if(ret == FALSE) return(ret);

       
		CListBox *lb = (CListBox *)GetDlgItem(IDC_LIST2);
		CListBox *lbb = (CListBox *)GetDlgItem(IDC_LIST3);
		CListBox *lbie = (CListBox *)GetDlgItem(IDC_LIST4);
        lbb->ResetContent();
        int dj_num = GET_Djoy_count();
		for(int i=0;i<dj_num;i++)
		   lb->AddString(GET_Djoy_Name(i));
		lb->SetCurSel(0);    
		OnLbnSelchangeList2();

		lbie->ResetContent();
		for(int i=0;i<input_events_count;i++)
			lbie->AddString(events[i].event_name);
		SetWindowText("KLN90B Input Config");
        
        //CEdit *ed = (CEdit *)GetDlgItem(IDC_EDIT1);
		((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText("");
		SetTimer(0x01020304,200,NULL);
		return(TRUE);
	}
	cDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnLbnSelchangeList4();
	afx_msg void OnLbnDblclkList4();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnSetfocusEdit1();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
};
cDialog::cDialog() : CDialog(IDD_DIALOG1)
{

}

void show_input_dlg(void)
{
    cDialog mdlg;
	mdlg.DoModal();
}
BEGIN_MESSAGE_MAP(cDialog, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST2, OnLbnSelchangeList2)
	ON_LBN_DBLCLK(IDC_LIST4, OnLbnDblclkList4)
	ON_LBN_SELCHANGE(IDC_LIST4, OnLbnSelchangeList4)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_SETFOCUS(IDC_EDIT1, OnEnSetfocusEdit1)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void cDialog::OnLbnSelchangeList2()
{
  CListBox *lb = (CListBox *)GetDlgItem(IDC_LIST2);
  int djoy_number = lb->GetCurSel();
  CListBox *lbb = (CListBox *)GetDlgItem(IDC_LIST3);
  lbb->ResetContent();
  djoy_info *dj_info = get_djoy_state(djoy_number);
  if(!dj_info) return;

  for(int i=0;i<DI_count_of_buttons(dj_info);i++)
  {
     char button_name[MAX_PATH];
	 sprintf(button_name,"%d. (%s)",i+1,GET_Object_Name(djoy_number,OBJECT_BUTTON,i));
	 lbb->AddString(button_name);
  }
}
void cDialog::OnLbnDblclkList4()
{
	CListBox *lb = (CListBox *)GetDlgItem(IDC_LIST2);
	int djoy_number = lb->GetCurSel();
	CListBox *lbb = (CListBox *)GetDlgItem(IDC_LIST3);
	int but_number = lbb->GetCurSel();
	CListBox *lbi = (CListBox *)GetDlgItem(IDC_LIST4);
	int inp_number = lbi->GetCurSel();

	TCHAR *dj_name = GET_Djoy_Name(djoy_number);
	TCHAR *but_name = GET_Object_Name(djoy_number,OBJECT_BUTTON,but_number);
	if(!dj_name || !but_name) return;
	if(inp_number<0 || inp_number>= input_events_count) return; 
    char status_line[MAX_PATH];
	sprintf(status_line,"%s.%d=%s",GET_Djoy_Name(djoy_number),but_number,
		event2str(events[inp_number].event_code));
	((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText(status_line);

	char str_attr[MAX_PATH];
	sprintf(str_attr,"%s.%d",GET_Djoy_Name(djoy_number),but_number);
	K_save_string_param("DI_CONFIG",str_attr,event2str(events[inp_number].event_code));
	load_di_config();
	OnLbnSelchangeList4();
}
void cDialog::OnLbnSelchangeList4()
{
	CListBox *lbi = (CListBox *)GetDlgItem(IDC_LIST4);
	int inp_number = lbi->GetCurSel();
	long event = events[inp_number].event_code;
    char status_line[MAX_PATH];
	sprintf(status_line,"No mapping for '%s'",events[inp_number].event_name);

	for(vector<inp_mapping_t >::iterator it=input_vec.begin();it!=input_vec.end();it++)
	{
		if((*it).input_event == event)
		{
		   sprintf(status_line,"Press button [%s] on [%s] for '%s'",
		                       GET_Object_Name((*it).djoy_number,
							                   OBJECT_BUTTON,
											  (*it).button_number
											  ),
		                      GET_Djoy_Name((*it).djoy_number),
							  events[inp_number].event_name
				  );
		   break;
		}
	}
	((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText(status_line);
}
void cDialog::OnBnClickedButton1()
{
	CListBox *lb = (CListBox *)GetDlgItem(IDC_LIST2);
	int djoy_number = lb->GetCurSel();
	CListBox *lbb = (CListBox *)GetDlgItem(IDC_LIST3);
	int but_number = lbb->GetCurSel();

	TCHAR *dj_name = GET_Djoy_Name(djoy_number);
	TCHAR *but_name = GET_Object_Name(djoy_number,OBJECT_BUTTON,but_number);
	if(!dj_name || !but_name) return;

	char status_line[MAX_PATH];
	sprintf(status_line,"%s.%d=%s",GET_Djoy_Name(djoy_number),but_number,
		"NULL");
	((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText(status_line);
	char str_attr[MAX_PATH];
    sprintf(str_attr,"%s.%d",GET_Djoy_Name(djoy_number),but_number);
	K_save_string_param("DI_CONFIG",str_attr,"NULL");
	load_di_config();
	OnLbnSelchangeList4();
}

void load_di_config(void)
{
   input_vec.clear();
   inp_mapping_t temp_map;
   for(int dj=0;dj<GET_Djoy_count();dj++)
   {
      djoy_info *dji = get_djoy_state(dj);
	  for(int but=0;but<DI_count_of_buttons(dji);but++)
	  {
	     char par_attr[MAX_PATH];
		 char par_value[MAX_PATH];

		 sprintf(par_attr,"%s.%d",GET_Djoy_Name(dj),but);
		 BOOL ret = K_load_string_param("DI_CONFIG",par_attr,par_value);
		 if(!ret || !stricmp(par_value,"NULL")) continue;
		 long input_event = str2event(par_value);
		 if(input_event == -1) continue;
         temp_map.djoy_number=dj;
		 temp_map.button_number=but;
		 temp_map.input_event=input_event;
		 temp_map.press_count=0;
		 input_vec.push_back(temp_map);
	   }   
   }
}

void input_config_init(void)
{
   load_di_config();
}
void input_config_clear(void)
{
   input_vec.clear();
}

void check_for_buttons(void)
{
   for(vector<inp_mapping_t >::iterator it=input_vec.begin();it!=input_vec.end();it++)
   {
      inp_mapping_t temp_map = *it;
      djoy_info *cur_dj = get_djoy_state(temp_map.djoy_number);
	  if(Is_Button_Pressed(temp_map.djoy_number,temp_map.button_number))
	  {
         if(!temp_map.press_count)
		 {
		    do_input_event(temp_map.input_event);
			(*it).press_count++;
		 }
	  }
	  else
         (*it).press_count=0;
   }
}
void cDialog::OnEnSetfocusEdit1()
{
   ((CEdit *)GetDlgItem(IDC_EDIT1))->SendMessage(WM_KILLFOCUS);
}
void cDialog::OnTimer(UINT nIDEvent)
{
	CListBox *lbi = (CListBox *)GetDlgItem(IDC_LIST4);
    int sel = lbi->GetCurSel();
	char str[MAX_PATH];
    BOOL button_pressed = FALSE;

	for(int dj=0;dj<GET_Djoy_count();dj++)
	{
		djoy_info *dji=get_djoy_state(dj);
        for(int but=0;but<DI_count_of_buttons(dji);but++)
		{
		   if(Is_Button_Pressed(dj,but))
		   {
		      sprintf(str,"Button [%s] on [%s] pressed",
		      GET_Object_Name(dj,OBJECT_BUTTON,but),
		      GET_Djoy_Name(dj));
              ((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText(str);
		      button_pressed=TRUE;
		      break;
		   }
		}
		if(button_pressed) break;
	}
	if(!button_pressed)
	{
	   if(sel>=0) OnLbnSelchangeList4();
	   else ((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText("");
	}
	CDialog::OnTimer(nIDEvent);
}
void cDialog::OnClose()
{
	KillTimer(0x01020304);
	CDialog::OnClose();
}

#endif

