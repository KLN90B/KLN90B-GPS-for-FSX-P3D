// gauges.h
// Copyright (c) 2003 Microsoft Corporation.  All rights reserved.
//

#ifndef GAUGE_H__
#define GAUGE_H__

#include "windows.h"

#pragma pack(4)

// Useful macros for INL files
#define CONCATENATE1(A,B) A##B
#define CONCATENATE(A,B) CONCATENATE1(A,B)
#define CONCATENATE31(A,B,C) A##B##C
#define CONCATENATE3(A,B,C) CONCATENATE31(A,B,C)
#define C_GAUGE(A) CONCATENATE(A , GAUGE_ITEM)
#define C_PLIST(A) CONCATENATE(plist_ , A)

#define LENGTHOF(array) (sizeof(array)/sizeof(array[0]))

#ifndef EXTERN_C
#ifdef __cplusplus
    #define EXTERN_C    extern "C"
#else
    #define EXTERN_C    extern
#endif
#endif


#ifndef DECLSPEC_EXPORT
#define DECLSPEC_EXPORT __declspec(dllexport)
#endif

// Flightsim calling convention
#define FSAPI   __stdcall

//
// Common Flight Simulator Constant Definitions
//

// parameter direction tags
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef VOID
#define VOID    void
#endif

// Logical True/False Constants
#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

// Pointer Constant
#ifndef NULL
#define NULL    ((void *)0)         // empty handle
#endif

// Misc Constants
#define NULLPTR NULL                // empty pointer
#define CR      0x0d                // ASCII carriage return
#define LF      0x0a                // ASCII line feed
#define FLAGS0  0                   // no flags set in a FLAGSn type variable
#define FLAGS1  -1                  // all flags set in a FLAGSn type variable

// Bit Flag Constants
#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

// integer data types limits
#define UINT8_MIN   0
#define UINT8_MAX   255
#define SINT8_MIN   -128
#define SINT8_MAX    127
#define UINT16_MIN  0
#define UINT16_MAX  65535
#define SINT16_MIN  -32768
#define SINT16_MAX  32767
#define UINT32_MIN  0
#define UINT32_MAX  4294967295
#define SINT32_MIN  ((signed int) BIT31)
#define SINT32_MAX  2147483647

// fractional data types limits
#define UFRAC8_MIN  0
#define UFRAC8_MAX  255
#define SFRAC8_MIN  -128
#define SFRAC8_MAX  127
#define UFRAC16_MIN 0
#define UFRAC16_MAX 65535
#define SFRAC16_MIN -32768
#define SFRAC16_MAX 32767
#define UFRAC32_MIN 0
#define UFRAC32_MAX 4294967295
#define SFRAC32_MIN -2147483648
#define SFRAC32_MAX 2147483647

// flag values for engine_control_select variable
#define ENGINE_CONTROL_SELECT_NONE  0       // no engines
#define ENGINE_CONTROL_SELECT_1     BIT0    // engine 1
#define ENGINE_CONTROL_SELECT_2     BIT1    // engine 2
#define ENGINE_CONTROL_SELECT_3     BIT2    // engine 3
#define ENGINE_CONTROL_SELECT_4     BIT3    // engine 4

// realism_systems values
#define REALISM_SYSTEMS_ELEV_TRIM   BIT1    // enable elevator trim realism (ratchet effect)
#define REALISM_SYSTEMS_GYRO_DRIFT  BIT2    // enable gyro drift
#define REALISM_SYSTEMS_LIGHT_BURN  BIT3    // enable light burnout if on too long during day
#define REALISM_SYSTEMS_FAST_THROT  BIT4    // enable fast throttle kill engine
#define REALISM_SYSTEMS_INS_LIGHTS  BIT5    // enable instrument light needed at night to see panel
#define REALISM_SYSTEMS_BARO_DRIFT  BIT6    // enable barometric pressure drift


// defines for window layer numbers & shared buffer numbers
#define LOWEST_LAYER        0x0000
#define VIEW_LAYER          0x2000
#define MINICONTROL_LAYER   0x3000
#define AUX_VIEW_LAYER      0x3F00
#define PANEL_LAYER         0x4000
#define MESSAGE_LAYER       0x5000
#define MENU_LAYER          0x6000
#define HIGHEST_LAYER       0xffff


// constants for the range of the UNIPIX coordinate system
#define UNIPIX_MAX_HEIGHT   6144    // max height of unipix region
#define UNIPIX_MAX_WIDTH    8192    // max width of unipix region


// vor1_tf_flag and vor2_tf_flag
#define VOR_TF_FLAG_OFF         0
#define VOR_TF_FLAG_TO          1
#define VOR_TF_FLAG_FROM        2

// Back course flags for VOR gauges
#define BC_FLAG_BACKCOURSE_AVAILABLE    BIT0
#define BC_FLAG_LOCALIZER_TUNED_IN      BIT1
#define BC_FLAG_ON_BACKCOURSE           BIT2
#define BC_FLAG_STATION_ACTIVE          BIT7

// sim_speed constants
#define SIM_SPEED_QUARTER   0x0040
#define SIM_SPEED_HALF      0x0080
#define SIM_SPEED_1X        0x0100
#define SIM_SPEED_2X        0x0200
#define SIM_SPEED_4X        0x0400
#define SIM_SPEED_8X        0x0800
#define SIM_SPEED_16X       0x1000
#define SIM_SPEED_32X       0x2000
#define SIM_SPEED_64X       0x4000
#define SIM_SPEED_128X      0x8000

#define SIM_SPEED_MIN       SIM_SPEED_QUARTER
#define SIM_SPEED_MAX       SIM_SPEED_128X

// controls limits
#define SPOILERS_POS_OFF            0       // spoilers desired position {0=retracted..16k=fully extended}
#define SPOILERS_POS_FULL           16383
#define FLAPS_POS_OFF               0       // 0=up, 16383 = full
#define FLAPS_POS_FULL              16383
#define GEAR_POS_UP                 0       // 0 = up, 16K = dn
#define GEAR_POS_DOWN               16383

// defines for VOR_INFO.CODE field
#define VOR_CODE_IS_LOCALIZER           BIT7    // bit7 = 0= VOR  1= Localizer
#define VOR_CODE_GLIDESLOPE             BIT6    // bit6 = 1= Glideslope Available
#define VOR_CODE_BACKCOURSE_UNAVAIL     BIT5    // bit5 = 1= no localizer backcourse
#define VOR_CODE_DME_AT_GLIDE_SLOPE     BIT4    // bit4 = 1= DME transmitter at Glide Slope Transmitter
#define VOR_CODE_NAV_UNAVAILABLE        BIT3    // bit3 = 1= no nav signal available
#define VOR_CODE_VOICE_AVAILABLE        BIT2    // bit2 = Voice Available
#define VOR_CODE_TACAN                  BIT1    // bit1 = TACAN
#define VOR_CODE_DME_AVAILABLE          BIT0    // bit0 = DME

// defines for engine structure starter variable, note:  magnetos for recip. engines / starters for jet engines
#define ENG_STARTER_MAGNETO_OFF     0   // the following values for reciprocating engines
#define ENG_STARTER_MAGNETO_RIGHT   1
#define ENG_STARTER_MAGNETO_LEFT    2
#define ENG_STARTER_MAGNETO_BOTH    3
#define ENG_STARTER_MAGNETO_START   4
#define ENG_STARTER_STARTER_OFF     0   // the following values for jet engines
#define ENG_STARTER_STARTER_START   1
#define ENG_STARTER_STARTER_GEN     2


#define GST_UNUSED  0x0000
#define GST_ACTIVE  0x0001
#define GST_GEAR    0x0002
#define GST_SCRAPE  0x0004

#define GSN_CENTER_GEAR         0
#define GSN_LEFT_GEAR           1
#define GSN_RIGHT_GEAR          2
#define GSN_AUX_GEAR            3
#define GSN_RIGHT_WING_SCRAPE   4
#define GSN_LEFT_WING_SCRAPE    5
#define GSN_FUSELAGE_SCRAPE     6
#define GSN_XTAIL_SCRAPE        7
#define GSN_AUX1_SCRAPE         8
#define GSN_AUX2_SCRAPE         9

#define TANK_MAIN_RIGHT     0
#define TANK_MAIN_LEFT      1
#define TANK_AUX_RIGHT      2
#define TANK_AUX_LEFT       3
#define TANK_TIP_RIGHT      4
#define TANK_TIP_LEFT       5
#define TANK_CENTER         6
#define TANK_CENTER2        7
#define TANK_CENTER3        8
#define TANK_EXTERNAL1      9
#define TANK_EXTERNAL2      10

// gear types
#define GEAR_TYPE_FIXED         0
#define GEAR_TYPE_RETRACTABLE   1
#define GEAR_TYPE_SKIDS         2
#define GEAR_TYPE_FLOATS        3
#define GEAR_TYPE_SKIS          4


// time_of_day constants
#define TIME_OF_DAY_DAY     BIT0
#define TIME_OF_DAY_DAWN    BIT1
#define TIME_OF_DAY_DUSK    BIT1
#define TIME_OF_DAY_NIGHT   BIT2

// surface_type constants
typedef enum SURFACE_TYPE
{
    SURFACE_TYPE_CONCRETE       = 0,    // concrete
    SURFACE_TYPE_GRASS          = 1,    // soft, bumpy ground (landable)
    SURFACE_TYPE_WATER          = 2,    // water
    SURFACE_TYPE_GRASS_BUMPY    = 3,    // very bumpy grass & mud (crashable)
    SURFACE_TYPE_ASPHALT        = 4,    // asphalt
    SURFACE_TYPE_SHORT_GRASS    = 5,    // short grass
    SURFACE_TYPE_LONG_GRASS     = 6,    // long grass
    SURFACE_TYPE_HARD_TURF      = 7,    // hard turf
    SURFACE_TYPE_SNOW           = 8,
    SURFACE_TYPE_ICE            = 9,
    SURFACE_TYPE_URBAN          = 10,
    SURFACE_TYPE_FOREST         = 11,
    SURFACE_TYPE_DIRT           = 12,   // added for dirt runways
    SURFACE_TYPE_CORAL          = 13,   // added for coral runways
    SURFACE_TYPE_GRAVEL         = 14,   // added for gravel runways
    SURFACE_TYPE_OIL_TREATED    = 15,   // added for oil treated (tar&chip) runways
    SURFACE_TYPE_STEEL_MATS     = 16,   // added for steel mats (steel mesh) temporary runways
    SURFACE_TYPE_BITUMINUS      = 17,
    SURFACE_TYPE_BRICK          = 18,
    SURFACE_TYPE_MACADAM        = 19,
    SURFACE_TYPE_PLANKS         = 20,
    SURFACE_TYPE_SAND           = 21,
    SURFACE_TYPE_SHALE          = 22,
    SURFACE_TYPE_TARMAC         = 23,
    SURFACE_TYPE_WRIGHT_FLYER_TRACK = 24,

    SURFACE_TYPE_UNKNOWN        = 254,  // valid but unknown surface type
    SURFACE_TYPE_UNDEFINED      = 255   // invalid surface type
}
SURFACE_TYPE, *PSURFACE_TYPE;

typedef enum SURFACE_CONDITION
{
    SURFACE_CONDITION_NORMAL    = 0,    // default for the given SURFACE_TYPE
    SURFACE_CONDITION_WET,
    SURFACE_CONDITION_ICY,
    SURFACE_CONDITION_SNOW,             // this is for snow on a non-snow SURFACE_TYPE
    SURFACE_CONDITION_MAX       // this surface condition should always be last
}
SURFACE_CONDITION;


#define FS_REG_BASE     "SOFTWARE\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"

enum TOOLTIP_UNITS_TYPE {
    TOOLTIP_UNITS_TYPE_DEFAULT,
    TOOLTIP_UNITS_TYPE_METRIC,
    TOOLTIP_UNITS_TYPE_US,
};


// Panel quick help strings
#define HELPID_MIN                              0x0000a000

#define HELPID_CESS                             (HELPID_MIN + 0x0000)
#define HELPID_CESS_CARB_HEAT                   (HELPID_CESS + 0x000f)
#define HELPID_CESS_FUEL_PUMP_TOGGLE            (HELPID_CESS + 0x003c)
#define HELPID_CESS_PRIMER_SWITCH               (HELPID_CESS + 0x003D)
#define HELPID_LEAR                             (HELPID_MIN + 0x0100)
#define HELPID_LEAR_AOA                         (HELPID_LEAR + 0x0004)
#define HELPID_LEAR_COM_RADIO                   (HELPID_LEAR + 0x0027)
#define HELPID_LEAR_NAV1                        (HELPID_LEAR + 0x0028)
#define HELPID_LEAR_NAV2                        (HELPID_LEAR + 0x0029)
#define HELPID_LEAR_MACH                        (HELPID_LEAR + 0x0038)
#define HELPID_LEAR_VOR2_NEEDLE_TOGGLE          (HELPID_LEAR + 0x003d)
#define HELPID_LEAR_ADF_NEEDLE_TOGGLE           (HELPID_LEAR + 0x00de)
#define HELPID_LEAR_MACH_INDICATOR              (HELPID_LEAR + 0x00e1)
#define HELPID_LEAR_FUEL_STANDBY                (HELPID_LEAR + 0x00E5)
#define HELPID_SOPWITH                          (HELPID_MIN + 0x0200)
#define HELPID_SOPWITH_OILP                     (HELPID_SOPWITH + 0x0005)
#define HELPID_SOPWITH_MAG                      (HELPID_SOPWITH + 0x0006)
#define HELPID_SCHWEIZER                        (HELPID_MIN + 0x0300)
#define HELPID_SCHWEIZER_ASI                    (HELPID_SCHWEIZER + 0x0002)
#define HELPID_SCHWEIZER_VSI                    (HELPID_SCHWEIZER + 0x0006)
#define HELPID_B737                             (HELPID_MIN + 0x0400)
#define HELPID_B737_OIL_QUANT                   (HELPID_B737 + 0x001a)
#define HELPID_B737_HYDRAULIC                   (HELPID_B737 + 0x001b)
#define HELPID_B737_TOTAL_AIR_TEMP              (HELPID_B737 + 0x0031)
#define HELPID_B737_VIBRATION                   (HELPID_B737 + 0x0033)
#define HELPID_B737_HYDRAULIC_QTY               (HELPID_B737 + 0x0034)
#define HELPID_B737_AUTOPILOT_IAS_MACH_SELECT   (HELPID_B737 + 0x0037)
#define HELPID_B737_TO_GA                       (HELPID_B737 + 0x0038)
#define HELPID_B737_AUTOTHROTTLE                (HELPID_B737 + 0x0039)
#define HELPID_B737_REFERENCE_N1                (HELPID_B737 + 0x003a)
#define HELPID_B737_N1_SET                      (HELPID_B737 + 0x003b)
#define HELPID_B737_TAKEOFF_POWER_ACTIVE        (HELPID_B737 + 0x0044)
#define HELPID_B737_AT_LIM                      (HELPID_B737 + 0x0046)
#define HELPID_B737_FLAPS_EXT_LIGHT             (HELPID_B737 + 0x0048)
#define HELPID_B737_FLAPS_TRANSIT_LIGHT         (HELPID_B737 + 0x0049)
#define HELPID_B737_OIL_PRESSURE_LIGHT          (HELPID_B737 + 0x004A)
#define HELPID_B737_REVERSER_UNLOCKED           (HELPID_B737 + 0x004B)
#define HELPID_B737_START_VALVE_LIGHT           (HELPID_B737 + 0x004C)
#define HELPID_B737_START_LEVERS                (HELPID_B737 + 0x0051)
#define HELPID_EXTRA                            (HELPID_MIN + 0x0500)
#define HELPID_EXTR_SMOKE                       (HELPID_EXTRA + 0x0013)
#define HELPID_EXTR_ACRO_TANK                   (HELPID_EXTRA + 0x0014)
#define HELPID_EXTR_MAIN_TANK                   (HELPID_EXTRA + 0x0015)
#define HELPID_EXTR_G_METER                     (HELPID_EXTRA + 0x001c)
#define HELPID_EXTR_G_METER_RESET               (HELPID_EXTRA + 0x0027)
#define HELPID_EXTR_PEDAL_SWITCH_LEFT           (HELPID_EXTRA + 0x002D)
#define HELPID_EXTR_PEDAL_SWITCH_RIGHT          (HELPID_EXTRA + 0x002E)
#define HELPID_EXTR_LOW_VOLT                    (HELPID_EXTRA + 0x002F)
#define HELPID_BELL                             (HELPID_MIN + 0x0600)
#define HELPID_BELL_XMISSION_PSI                (HELPID_BELL + 0x0002)
#define HELPID_BELL_XMISSION_TEMP               (HELPID_BELL + 0x0003)
#define HELPID_BELL_GEN_LOAD                    (HELPID_BELL + 0x0005)
#define HELPID_BELL_TURBINE_TEMP                (HELPID_BELL + 0x0008)
#define HELPID_BELL_GAS_PRODUCER                (HELPID_BELL + 0x0009)
#define HELPID_BELL_ROTOR_TURBINE               (HELPID_BELL + 0x000c)
#define HELPID_BELL_CYCLIC_FA                   (HELPID_BELL + 0x0016)
#define HELPID_BELL_CYCLIC_LR                   (HELPID_BELL + 0x0017)
#define HELPID_BELL_ANTI_TORQUE_ROTOR           (HELPID_BELL + 0x0018)
#define HELPID_BELL_FUEL_VALVE                  (HELPID_BELL + 0x0019)
#define HELPID_BELL_GOVERNOR                    (HELPID_BELL + 0x001A)
#define HELPID_BELL_THROTTLE                    (HELPID_BELL + 0x001B)
#define HELPID_LSO                              (HELPID_MIN + 0x0780)
#define HELPID_LSO_WINDOW                       (HELPID_LSO + 0x00)
#define HELPID_CONCORDE                         (HELPID_MIN +  0x0A00)
#define HELPID_CONCORDE_ACCELEROMETER           (HELPID_CONCORDE + 0x0000)
#define HELPID_CONCORDE_ANGLE_OF_ATTACK         (HELPID_CONCORDE + 0x0001)
#define HELPID_CONCORDE_SPEED_SELECT            (HELPID_CONCORDE + 0x0002)
#define HELPID_CONCORDE_AIRSPEED_MODE_SWITCH    (HELPID_CONCORDE + 0x0003)
#define HELPID_CONCORDE_DISCREPANCY_LIGHT       (HELPID_CONCORDE + 0x0004)
#define HELPID_CONCORDE_MACHMETER               (HELPID_CONCORDE + 0x0005)
#define HELPID_CONCORDE_ALTIMETER_MODE_SWITCH   (HELPID_CONCORDE + 0x0006)
#define HELPID_CONCORDE_ATTITUDE_TEST           (HELPID_CONCORDE + 0x0007)
#define HELPID_CONCORDE_CHECK_ATT               (HELPID_CONCORDE + 0x0008)
#define HELPID_CONCORDE_ATTITUDE_DH             (HELPID_CONCORDE + 0x0009)
#define HELPID_CONCORDE_PITCH_INDEX_KNOB        (HELPID_CONCORDE + 0x000A)
#define HELPID_CONCORDE_HSI_FAIL                (HELPID_CONCORDE + 0x000B)
#define HELPID_CONCORDE_HSI_SWITCH              (HELPID_CONCORDE + 0x000C)
#define HELPID_CONCORDE_HSI_MILES               (HELPID_CONCORDE + 0x000D)
#define HELPID_CONCORDE_HSI_SPEED               (HELPID_CONCORDE + 0x000E)
#define HELPID_CONCORDE_SIDESLIP                (HELPID_CONCORDE + 0x000F)
#define HELPID_CONCORDE_DME_DISTANCE            (HELPID_CONCORDE + 0x0010)
#define HELPID_CONCORDE_DME_SPEED               (HELPID_CONCORDE + 0x0011)
#define HELPID_CONCORDE_DME_TIME                (HELPID_CONCORDE + 0x0012)
#define HELPID_CONCORDE_DME_SWITCH              (HELPID_CONCORDE + 0x0013)
#define HELPID_CONCORDE_RADIO_ALTIMETER         (HELPID_CONCORDE + 0x0014)
#define HELPID_CONCORDE_RADIO_ALTIMETER_TEST    (HELPID_CONCORDE + 0x0015)
#define HELPID_CONCORDE_RADIO_ALTIMETER_KNOB    (HELPID_CONCORDE + 0x0016)
#define HELPID_CONCORDE_TOTAL_FUEL              (HELPID_CONCORDE + 0x0017)
#define HELPID_CONCORDE_CENTER_GRAVITY          (HELPID_CONCORDE + 0x0018)
#define HELPID_CONCORDE_CENTER_GRAVITY_LIGHT    (HELPID_CONCORDE + 0x0019)
#define HELPID_CONCORDE_TEMPERATURE_TOTAL       (HELPID_CONCORDE + 0x001A)
#define HELPID_CONCORDE_TEMPERATURE_STATIC      (HELPID_CONCORDE + 0x001B)
#define HELPID_CONCORDE_TEMPERATURE_ISA         (HELPID_CONCORDE + 0x001C)
#define HELPID_CONCORDE_GMT                     (HELPID_CONCORDE + 0x001D)
#define HELPID_CONCORDE_TIMER                   (HELPID_CONCORDE + 0x001E)
#define HELPID_CONCORDE_CLOCK_MODE_SWITCH       (HELPID_CONCORDE + 0x001F)
#define HELPID_CONCORDE_CLOCK_START_STOP        (HELPID_CONCORDE + 0x0020)
#define HELPID_CONCORDE_CLOCK_FAST_SLOW         (HELPID_CONCORDE + 0x0021)
#define HELPID_CONCORDE_ENGINE_MANAGE_LIGHTS    (HELPID_CONCORDE + 0x0022)
#define HELPID_CONCORDE_ENGINE_MANAGE_GO        (HELPID_CONCORDE + 0x0023)
#define HELPID_CONCORDE_ENGINE_MANAGE_FAILURE   (HELPID_CONCORDE + 0x0024)
#define HELPID_CONCORDE_ENGINE_MANAGE_REVERSE   (HELPID_CONCORDE + 0x0025)
#define HELPID_CONCORDE_ENGINE_FUEL             (HELPID_CONCORDE + 0x0026)
#define HELPID_CONCORDE_ENGINE_FUEL_SETTING_KNOB (HELPID_CONCORDE + 0x0027)
#define HELPID_CONCORDE_ENGINE_EGT              (HELPID_CONCORDE + 0x0028)
#define HELPID_CONCORDE_ENGINE_AREA             (HELPID_CONCORDE + 0x0029)
#define HELPID_CONCORDE_ENGINE_FAILURE_LIGHT    (HELPID_CONCORDE + 0x002A)
#define HELPID_CONCORDE_ENGINE_AFTERBURNER_LIGHT (HELPID_CONCORDE + 0x002B)
#define HELPID_CONCORDE_VISOR_NOSE_LEVER        (HELPID_CONCORDE + 0x002C)
#define HELPID_CONCORDE_VISOR_INDICATOR         (HELPID_CONCORDE + 0x002D)
#define HELPID_CONCORDE_NOSE_INDICATOR          (HELPID_CONCORDE + 0x002E)
#define HELPID_CONCORDE_LEFT_GEAR_UNLOCKED      (HELPID_CONCORDE + 0x002F)
#define HELPID_CONCORDE_RIGHT_GEAR_UNLOCKED     (HELPID_CONCORDE + 0x0030)
#define HELPID_CONCORDE_UP_GEAR_UNLOCKED        (HELPID_CONCORDE + 0x0031)
#define HELPID_CONCORDE_GEAR_TRANSIT            (HELPID_CONCORDE + 0x0032)
#define HELPID_CONCORDE_LEFT_GEAR_DOWN          (HELPID_CONCORDE + 0x0033)
#define HELPID_CONCORDE_NOSE_GEAR_DOWN          (HELPID_CONCORDE + 0x0034)
#define HELPID_CONCORDE_TAIL_GEAR_DOWN          (HELPID_CONCORDE + 0x0035)
#define HELPID_CONCORDE_RIGHT_GEAR_DOWN         (HELPID_CONCORDE + 0x0036)
#define HELPID_CONCORDE_CONTROL_SURFACE         (HELPID_CONCORDE + 0x0037)
#define HELPID_CONCORDE_CONTROL_SURFACE_RESET   (HELPID_CONCORDE + 0x0038)
#define HELPID_CONCORDE_CONTROL_SURFACE_TEST    (HELPID_CONCORDE + 0x0039)
#define HELPID_CONCORDE_CONTROL_SURFACE_LIGHTS  (HELPID_CONCORDE + 0x003A)
#define HELPID_CONCORDE_TURB_MODE               (HELPID_CONCORDE + 0x003B)
#define HELPID_CONCORDE_MAX_CLIMB               (HELPID_CONCORDE + 0x003C)
#define HELPID_CONCORDE_MAX_CRUISE              (HELPID_CONCORDE + 0x003D)
#define HELPID_CONCORDE_MASTER_WARNING          (HELPID_CONCORDE + 0x003E)
#define HELPID_CONCORDE_FUEL_QUANTITY           (HELPID_CONCORDE + 0x003F)
#define HELPID_CONCORDE_LOW_FUEL_LIGHT          (HELPID_CONCORDE + 0x0040)
#define HELPID_CONCORDE_FUEL_CROSSFEED          (HELPID_CONCORDE + 0x0041)
#define HELPID_CONCORDE_FUEL_PUMP               (HELPID_CONCORDE + 0x0042)
#define HELPID_CONCORDE_MASTER_SWITCH           (HELPID_CONCORDE + 0x0043)
#define HELPID_CONCORDE_ENGINE_START            (HELPID_CONCORDE + 0x0044)
#define HELPID_CONCORDE_GENERATOR               (HELPID_CONCORDE + 0x0045)
#define HELPID_CONCORDE_THROTTLES               (HELPID_CONCORDE + 0x0046)
#define HELPID_CONCORDE_REHEAT                  (HELPID_CONCORDE + 0x0047)
#define HELPID_CONCORDE_THROTTLE_FAILURE        (HELPID_CONCORDE + 0x0048)
#define HELPID_CONCORDE_BRAKE                   (HELPID_CONCORDE + 0x0049)
#define HELPID_CONCORDE_WARNING_CG              (HELPID_CONCORDE + 0x004A)
#define HELPID_CONCORDE_WARNING_ELECTRICAL      (HELPID_CONCORDE + 0x004B)
#define HELPID_CONCORDE_WARNING_ENGINE          (HELPID_CONCORDE + 0x004C)
#define HELPID_CONCORDE_WARNING_FUEL            (HELPID_CONCORDE + 0x004D)
#define HELPID_CONCORDE_WARNING_ICE             (HELPID_CONCORDE + 0x004E)
#define HELPID_CONCORDE_WARNING_AIR             (HELPID_CONCORDE + 0x004F)
#define HELPID_CONCORDE_WARNING_TRIM            (HELPID_CONCORDE + 0x0050)
#define HELPID_CONCORDE_WARNING_INTAKE          (HELPID_CONCORDE + 0x0051)
#define HELPID_CONCORDE_WARNING_HYDRAULIC       (HELPID_CONCORDE + 0x0052)
#define HELPID_CONCORDE_WARNING_SMOKE           (HELPID_CONCORDE + 0x0053)
#define HELPID_CONCORDE_RADIO_ADF               (HELPID_CONCORDE + 0x0054)
#define HELPID_CONCORDE_RADIO_ADF_SET           (HELPID_CONCORDE + 0x0055)
#define HELPID_CONCORDE_RADIO_ADF_FREQUENCY     (HELPID_CONCORDE + 0x0056)
#define HELPID_CONCORDE_RADIO_ADF_IDENT         (HELPID_CONCORDE + 0x0057)
#define HELPID_CONCORDE_RADIO_ADF_TEST          (HELPID_CONCORDE + 0x0058)
#define HELPID_CONCORDE_RADIO_COMM1             (HELPID_CONCORDE + 0x0059)
#define HELPID_CONCORDE_RADIO_COMM2             (HELPID_CONCORDE + 0x005A)
#define HELPID_CONCORDE_RADIO_COMM_FREQUENCY    (HELPID_CONCORDE + 0x005B)
#define HELPID_CONCORDE_RADIO_COMM_FREQUENCY_STBY (HELPID_CONCORDE + 0x005C)
#define HELPID_CONCORDE_RADIO_COMM_SET          (HELPID_CONCORDE + 0x005D)
#define HELPID_CONCORDE_RADIO_COMM_SET_STBY     (HELPID_CONCORDE + 0x005E)
#define HELPID_CONCORDE_RADIO_COMM_LIGHT        (HELPID_CONCORDE + 0x005F)
#define HELPID_CONCORDE_RADIO_COMM_SWITCH       (HELPID_CONCORDE + 0x0060)
#define HELPID_CONCORDE_RADIO_TRANSPONDER       (HELPID_CONCORDE + 0x0061)
#define HELPID_CONCORDE_RADIO_TRANSPONDER_ID    (HELPID_CONCORDE + 0x0062)
#define HELPID_CONCORDE_RADIO_TRANSPONDER_SET_ID (HELPID_CONCORDE + 0x0063)
#define HELPID_CONCORDE_RADIO_TRANSPONDER_MODE  (HELPID_CONCORDE + 0x0064)
#define HELPID_CONCORDE_RADIO_TRANSPONDER_POSITION (HELPID_CONCORDE + 0x0065)
#define HELPID_CONCORDE_RADIO_LIGHTS            (HELPID_CONCORDE + 0x0066)
#define HELPID_CONCORDE_BATTERY_SWITCH          (HELPID_CONCORDE + 0x0067)
#define HELPID_CONCORDE_AVIONICS_SWITCH         (HELPID_CONCORDE + 0x0068)
#define HELPID_CONCORDE_THROTTLES_REVERSE       (HELPID_CONCORDE + 0x0069)
#define HELPID_CONCORDE_VOR_BEARING             (HELPID_CONCORDE + 0x006A)
#define HELPID_CONCORDE_LIGHT_SWITCH_ANTI_COLLISION (HELPID_CONCORDE + 0x006B)
#define HELPID_777                              (HELPID_MIN +  0x0C00)
#define HELPID_777_AUTOPILOT                    (HELPID_777 + 0x0000)
#define HELPID_777_ENGINE_AUTOSTART             (HELPID_777 + 0x0001)
#define HELPID_MOONEY                           (HELPID_MIN +  0x0C80)
#define HELPID_MOONEY_STANDBY_VAC_SWITCH        (HELPID_MOONEY + 0x0000)
#define HELPID_MOONEY_PROP_DEICE                (HELPID_MOONEY + 0x0001)
#define HELPID_MOONEY_SPEED_BRAKE               (HELPID_MOONEY + 0x0002)
#define HELPID_KINGAIR                          (HELPID_MIN +  0x0D00)
#define HELPID_KINGAIR_AUTOFEATHER_LIGHT        (HELPID_KINGAIR + 0x0000)
#define HELPID_KINGAIR_AUTOFEATHER_SWITCH       (HELPID_KINGAIR + 0x0001)
#define HELPID_KINGAIR_PROP_SYNC_SWITCH         (HELPID_KINGAIR + 0x0002)
#define HELPID_KINGAIR_PROP_SYNCHROSCOPE        (HELPID_KINGAIR + 0x0003)
#define HELPID_KINGAIR_PROP_LEVERS              (HELPID_KINGAIR + 0x0004)
#define HELPID_KINGAIR_CONDITION_LEVERS         (HELPID_KINGAIR + 0x0005)
#define HELPID_CFS                              (HELPID_MIN +  0x0800)
#define HELPID_SPITFIRE_ENGINE_TEMP             (HELPID_CFS + 0)
#define HELPID_SPITFIRE_GEAR                    (HELPID_CFS + 1)
#define HELPID_SPITFIRE_OIL_TEMP                (HELPID_CFS + 2)
#define HELPID_SPITFIRE_STARTER                 (HELPID_CFS + 3)
#define HELPID_SPITFIRE_MAGNETO                 (HELPID_CFS + 4)
#define HELPID_SPITFIRE_OIL_PRESSURE            (HELPID_CFS + 5)
#define HELPID_SPITFIRE_FLAPS                   (HELPID_CFS + 6)
#define HELPID_SPITFIRE_CLOCK                   (HELPID_CFS + 8)
#define HELPID_SPITFIRE_AIRSPEED                (HELPID_CFS + 9)
#define HELPID_SPITFIRE_ATTITUDE                (HELPID_CFS + 10)
#define HELPID_SPITFIRE_VERTICAL_SPEED          (HELPID_CFS + 11)
#define HELPID_SPITFIRE_ALTIMETER               (HELPID_CFS + 12)
#define HELPID_SPITFIRE_DG                      (HELPID_CFS + 13)
#define HELPID_SPITFIRE_TURN_BANK               (HELPID_CFS + 14)
#define HELPID_SPITFIRE_WARNING_LIGHT           (HELPID_CFS + 15)
#define HELPID_SPITFIRE_BOOST_PRESSURE          (HELPID_CFS + 16)
#define HELPID_SPITFIRE_FUEL_SELECTOR           (HELPID_CFS + 17)
#define HELPID_SPITFIRE_TACHOMETER              (HELPID_CFS + 18)
#define HELPID_SPITFIRE_FUEL                    (HELPID_CFS + 19)
#define HELPID_SPITFIRE_THROTTLE                (HELPID_CFS + 20)
#define HELPID_SPITFIRE_MIXTURE                 (HELPID_CFS + 21)
#define HELPID_SPITFIRE_PROP                    (HELPID_CFS + 22)
#define HELPID_FW190A_GEAR                      (HELPID_CFS + 50)
#define HELPID_FW190A_CLOCK                     (HELPID_CFS + 51)
#define HELPID_FW190A_OIL_TEMP                  (HELPID_CFS + 52)
#define HELPID_FW190A_ATTITUDE                  (HELPID_CFS + 53)
#define HELPID_FW190A_DG                        (HELPID_CFS + 54)
#define HELPID_FW190A_OIL_PRESSURE              (HELPID_CFS + 55)
#define HELPID_FW190A_FUEL                      (HELPID_CFS + 56)
#define HELPID_FW190A_WARNING_LIGHT             (HELPID_CFS + 57)
#define HELPID_FW190A_STARTER                   (HELPID_CFS + 58)
#define HELPID_FW190A_FLAPS                     (HELPID_CFS + 59)
#define HELPID_FW190A_AIRSPEED                  (HELPID_CFS + 60)
#define HELPID_FW190A_ALTIMETER                 (HELPID_CFS + 61)
#define HELPID_FW190A_VERTICAL_SPEED            (HELPID_CFS + 62)
#define HELPID_FW190A_TACHOMETER                (HELPID_CFS + 63)
#define HELPID_FW190A_BOOST_PRESSURE            (HELPID_CFS + 64)
#define HELPID_FW190A_FUEL_SELECTOR             (HELPID_CFS + 65)
#define HELPID_FW190A_THROTTLE                  (HELPID_CFS + 66)
#define HELPID_FW190A_MIXTURE                   (HELPID_CFS + 67)
#define HELPID_FW190A_PROP                      (HELPID_CFS + 68)
#define HELPID_P51_FLAPS                        (HELPID_CFS + 100)
#define HELPID_P51_MAGNETO                      (HELPID_CFS + 101)
#define HELPID_P51_GEAR                         (HELPID_CFS + 102)
#define HELPID_P51_STARTER                      (HELPID_CFS + 103)
#define HELPID_P51_CLOCK                        (HELPID_CFS + 104)
#define HELPID_P51_ENGINE_TEMP                  (HELPID_CFS + 106)
#define HELPID_P51_AIRSPEED                     (HELPID_CFS + 107)
#define HELPID_P51_DG                           (HELPID_CFS + 108)
#define HELPID_P51_ATTITUDE                     (HELPID_CFS + 109)
#define HELPID_P51_MANIFOLD_PRESSURE            (HELPID_CFS + 110)
#define HELPID_P51_ALTIMETER                    (HELPID_CFS + 111)
#define HELPID_P51_TURN_BANK                    (HELPID_CFS + 112)
#define HELPID_P51_VERTICAL_SPEED               (HELPID_CFS + 113)
#define HELPID_P51_TACHOMETER                   (HELPID_CFS + 114)
#define HELPID_P51_FUEL_SELECTOR                (HELPID_CFS + 115)
#define HELPID_P51_FUEL                         (HELPID_CFS + 116)
#define HELPID_P51_ENGINE                       (HELPID_CFS + 117)
#define HELPID_P51_THROTTLE                     (HELPID_CFS + 118)
#define HELPID_P51_MIXTURE                      (HELPID_CFS + 119)
#define HELPID_P51_PROP                         (HELPID_CFS + 120)
#define HELPID_BF109_STARTER                    (HELPID_CFS + 150)
#define HELPID_BF109_MAGNETO                    (HELPID_CFS + 151)
#define HELPID_BF109_GEAR                       (HELPID_CFS + 152)
#define HELPID_BF109_FLAPS                      (HELPID_CFS + 153)
#define HELPID_BF109_CLOCK                      (HELPID_CFS + 154)
#define HELPID_BF109_GUNSIGHT                   (HELPID_CFS + 155)
#define HELPID_BF109_ALTIMETER                  (HELPID_CFS + 156)
#define HELPID_BF109_DG                         (HELPID_CFS + 157)
#define HELPID_BF109_TACHOMETER                 (HELPID_CFS + 158)
#define HELPID_BF109_AIRSPEED                   (HELPID_CFS + 159)
#define HELPID_BF109_ENGINE_TEMP                (HELPID_CFS + 160)
#define HELPID_BF109_TURN_BANK                  (HELPID_CFS + 161)
#define HELPID_BF109_WARNING_LIGHT              (HELPID_CFS + 162)
#define HELPID_BF109_BOOST_PRESSURE             (HELPID_CFS + 163)
#define HELPID_BF109_OIL_TEMP                   (HELPID_CFS + 164)
#define HELPID_BF109_FUEL                       (HELPID_CFS + 165)
#define HELPID_BF109_FUEL_OIL_PRESSURE          (HELPID_CFS + 166)
#define HELPID_BF109_THROTTLE                   (HELPID_CFS + 167)
#define HELPID_BF109_MIXTURE                    (HELPID_CFS + 168)
#define HELPID_BF109_PROP                       (HELPID_CFS + 169)
#define HELPID_HURRICANE_GEAR                   (HELPID_CFS + 200)
#define HELPID_HURRICANE_FUEL_SELECTOR          (HELPID_CFS + 201)
#define HELPID_HURRICANE_FUEL                   (HELPID_CFS + 202)
#define HELPID_HURRICANE_ENGINE_TEMP            (HELPID_CFS + 203)
#define HELPID_HURRICANE_STARTER                (HELPID_CFS + 204)
#define HELPID_HURRICANE_MAGNETO                (HELPID_CFS + 205)
#define HELPID_HURRICANE_WARNING_LIGHT          (HELPID_CFS + 206)
#define HELPID_HURRICANE_CLOCK                  (HELPID_CFS + 207)
#define HELPID_HURRICANE_AIRSPEED               (HELPID_CFS + 208)
#define HELPID_HURRICANE_ATTITUDE               (HELPID_CFS + 209)
#define HELPID_HURRICANE_VERTICAL_SPEED         (HELPID_CFS + 210)
#define HELPID_HURRICANE_ALTIMETER              (HELPID_CFS + 211)
#define HELPID_HURRICANE_DG                     (HELPID_CFS + 212)
#define HELPID_HURRICANE_TURN_BANK              (HELPID_CFS + 213)
#define HELPID_HURRICANE_TACHOMETER             (HELPID_CFS + 214)
#define HELPID_HURRICANE_OIL_PRESSURE           (HELPID_CFS + 215)
#define HELPID_HURRICANE_BOOST_PRESSURE         (HELPID_CFS + 216)
#define HELPID_HURRICANE_OIL_TEMP               (HELPID_CFS + 217)
#define HELPID_HURRICANE_FLAPS                  (HELPID_CFS + 218)
#define HELPID_HURRICANE_THROTTLE               (HELPID_CFS + 219)
#define HELPID_HURRICANE_MIXTURE                (HELPID_CFS + 220)
#define HELPID_HURRICANE_PROP                   (HELPID_CFS + 221)
#define HELPID_P47_FUEL_SELECTOR                (HELPID_CFS + 250)
#define HELPID_P47_FUEL                         (HELPID_CFS + 251)
#define HELPID_P47_WARNING_LIGHT                (HELPID_CFS + 252)
#define HELPID_P47_CLOCK                        (HELPID_CFS + 253)
#define HELPID_P47_AIRSPEED                     (HELPID_CFS + 254)
#define HELPID_P47_TURN_BANK                    (HELPID_CFS + 255)
#define HELPID_P47_ATTITUDE                     (HELPID_CFS + 256)
#define HELPID_P47_TACHOMETER                   (HELPID_CFS + 257)
#define HELPID_P47_ENGINE                       (HELPID_CFS + 258)
#define HELPID_P47_FLAPS_GEAR                   (HELPID_CFS + 259)
#define HELPID_P47_STARTER                      (HELPID_CFS + 260)
#define HELPID_P47_MAGNETO                      (HELPID_CFS + 261)
#define HELPID_P47_ALTIMETER                    (HELPID_CFS + 262)
#define HELPID_P47_DG                           (HELPID_CFS + 263)
#define HELPID_P47_VERTICAL_SPEED               (HELPID_CFS + 264)
#define HELPID_P47_MANIFOLD_PRESSURE            (HELPID_CFS + 265)
#define HELPID_P47_ENGINE_TEMP                  (HELPID_CFS + 266)
#define HELPID_P47_THROTTLE                     (HELPID_CFS + 267)
#define HELPID_P47_MIXTURE                      (HELPID_CFS + 268)
#define HELPID_P47_PROP                         (HELPID_CFS + 269)
#define HELPID_WILDCAT_PROP                     (HELPID_CFS + 270)
#define HELPID_WILDCAT_FUEL_SELECTOR            (HELPID_CFS + 271)

#define HELPID_RADIO                            (HELPID_MIN + 0x0700)
#define HELPID_RADIO_NAV1                       (HELPID_RADIO + 0x0000)
#define HELPID_RADIO_COMM1                      (HELPID_RADIO + 0x0001)
#define HELPID_RADIO_NAV2                       (HELPID_RADIO + 0x0002)
#define HELPID_RADIO_COMM2                      (HELPID_RADIO + 0x0003)
#define HELPID_RADIO_DME                        (HELPID_RADIO + 0x0004)
#define HELPID_RADIO_DME_KT                     (HELPID_RADIO + 0x0005)
#define HELPID_RADIO_DME_NM                     (HELPID_RADIO + 0x0006)
#define HELPID_RADIO_DME_MIN                    (HELPID_RADIO + 0x0007)
#define HELPID_RADIO_DME_N1_N2_SELECT           (HELPID_RADIO + 0x0008)
#define HELPID_RADIO_ADF_RADIO                  (HELPID_RADIO + 0x0009)
#define HELPID_RADIO_TRANSPONDER                (HELPID_RADIO + 0x000a)
#define HELPID_RADIO_RADIOS_OPEN                (HELPID_RADIO + 0x000b)
#define HELPID_RADIO_AP_RADIO                   (HELPID_RADIO + 0x000c)
#define HELPID_RADIO_SWITCH_STANDBY_FREQUENCY   (HELPID_RADIO + 0x000d)
#define HELPID_RADIO_COMM_SELECT                (HELPID_RADIO + 0x00e)
#define HELPID_RADIO_COMM_FREQ                  (HELPID_RADIO + 0x000f)
#define HELPID_RADIO_COMM1_SELECT               (HELPID_RADIO + 0x0010)
#define HELPID_RADIO_COMM1_FREQ                 (HELPID_RADIO + 0x0011)
#define HELPID_RADIO_COMM2_SELECT               (HELPID_RADIO + 0x0012)
#define HELPID_RADIO_COMM2_FREQ                 (HELPID_RADIO + 0x0013)
#define HELPID_RADIO_NAV1_SELECT                (HELPID_RADIO + 0x0014)
#define HELPID_RADIO_NAV1_FREQ                  (HELPID_RADIO + 0x0015)
#define HELPID_RADIO_NAV2_SELECT                (HELPID_RADIO + 0x0016)
#define HELPID_RADIO_NAV2_FREQ                  (HELPID_RADIO + 0x0017)
#define HELPID_RADIO_ADF                        (HELPID_RADIO + 0x0018)
#define HELPID_RADIO_AUTOPILOT                  (HELPID_RADIO + 0x0019)
#define HELPID_RADIO_OPEN_STACK                 (HELPID_RADIO + 0x001a)
#define HELPID_RADIO_DME_IND                    (HELPID_RADIO + 0x001B)
#define HELPID_GPS_MODE                         (HELPID_RADIO + 0x001C)
#define HELPID_GPS_DIRECT                       (HELPID_RADIO + 0x001D)
#define HELPID_GPS_MENU                         (HELPID_RADIO + 0x001E)
#define HELPID_GPS_ARROWS                       (HELPID_RADIO + 0x001F)
#define HELPID_GPS_ENTER                        (HELPID_RADIO + 0x0020)
#define HELPID_GPS_CANCEL                       (HELPID_RADIO + 0x0021)
#define HELPID_RADIO_AUDIO                      (HELPID_RADIO + 0x0022)
#define HELPID_AUTOPILOT                        (HELPID_MIN +  0x0D80)
#define HELPID_AUTOPILOT_WING_LEVELER           (HELPID_AUTOPILOT + 0x0000)
#define HELPID_AUTOPILOT_IAS_HOLD               (HELPID_AUTOPILOT + 0x0001)
#define HELPID_AUTOPILOT_BC_HOLD                (HELPID_AUTOPILOT + 0x0002)
#define HELPID_AUTOPILOT_APP_HOLD               (HELPID_AUTOPILOT + 0x0003)
#define HELPID_AUTOPILOT_YAW_DAMP_HOLD          (HELPID_AUTOPILOT + 0x0004)
#define HELPID_AUTOPILOT_NAV_HOLD               (HELPID_AUTOPILOT + 0x0005)
#define HELPID_AUTOPILOT_NAV_INDICATOR          (HELPID_AUTOPILOT + 0x0006)
#define HELPID_AUTOPILOT_HEADING_HOLD           (HELPID_AUTOPILOT + 0x0007)
#define HELPID_AUTOPILOT_HEADING_INDICATOR      (HELPID_AUTOPILOT + 0x0008)
#define HELPID_AUTOPILOT_IAS_ACQUIRE_HOLD       (HELPID_AUTOPILOT + 0x0009)
#define HELPID_AUTOPILOT_VSI_HOLD               (HELPID_AUTOPILOT + 0x000A)
#define HELPID_AUTOPILOT_VSI_INDICATOR          (HELPID_AUTOPILOT + 0x000B)
#define HELPID_AUTOPILOT_ALTITUDE_HOLD          (HELPID_AUTOPILOT + 0x000C)
#define HELPID_AUTOPILOT_ALTITUDE_INDICATOR     (HELPID_AUTOPILOT + 0x000D)
#define HELPID_AUTOPILOT_SPEED_HOLD             (HELPID_AUTOPILOT + 0x000E)
#define HELPID_AUTOPILOT_SPEED_INDICATOR        (HELPID_AUTOPILOT + 0x000F)
#define HELPID_AUTOPILOT_FLIGHT_DIRECTOR_SWITCH (HELPID_AUTOPILOT + 0x0010)
#define HELPID_AUTOPILOT_AUTOTHROTTLE           (HELPID_AUTOPILOT + 0x0011)
#define HELPID_AUTOPILOT_MACH_HOLD              (HELPID_AUTOPILOT + 0x0012)
#define HELPID_AUTOPILOT_MACH_INDICATOR         (HELPID_AUTOPILOT + 0x0013)
#define HELPID_AUTOPILOT_VSI_WHEEL              (HELPID_AUTOPILOT + 0x0014)

#define HELPID_GAUGE                            (HELPID_MIN +  0x0B00)
#define HELPID_GAUGE_ADF                        (HELPID_GAUGE + 0x0000)
#define HELPID_GAUGE_ADF_SET                    (HELPID_GAUGE + 0x0001)
#define HELPID_GAUGE_AIRSPEED                   (HELPID_GAUGE + 0x0002)
#define HELPID_GAUGE_AIRSPEED_STRIP             (HELPID_GAUGE + 0x0003)
#define HELPID_GAUGE_ALTERNATOR_SWITCH          (HELPID_GAUGE + 0x0004)
#define HELPID_GAUGE_ALTERNATOR_SWITCH_LEFT     (HELPID_GAUGE + 0x0005)
#define HELPID_GAUGE_ALTERNATOR_SWITCH_RIGHT    (HELPID_GAUGE + 0x0006)
#define HELPID_GAUGE_ALTIMETER                  (HELPID_GAUGE + 0x0007)
#define HELPID_GAUGE_ALTIMETER_SET              (HELPID_GAUGE + 0x0008)
#define HELPID_GAUGE_ALTIMETER_STRIP            (HELPID_GAUGE + 0x0009)
#define HELPID_GAUGE_ALTITUDE_ALERTER           (HELPID_GAUGE + 0x000A)
#define HELPID_GAUGE_AMMETER                    (HELPID_GAUGE + 0x000B)
#define HELPID_GAUGE_ANNUNCIATOR                (HELPID_GAUGE + 0x000C)
#define HELPID_GAUGE_ATTITUDE_INDICATOR         (HELPID_GAUGE + 0x000D)
#define HELPID_GAUGE_AUTOPILOT_MASTER           (HELPID_GAUGE + 0x000E)
#define HELPID_GAUGE_AVIONICS_SWITCH            (HELPID_GAUGE + 0x000F)
#define HELPID_GAUGE_BACKUP_AIRSPEED            (HELPID_GAUGE + 0x0010)
#define HELPID_GAUGE_BACKUP_ALTIMETER           (HELPID_GAUGE + 0x0011)
#define HELPID_GAUGE_BACKUP_ATTITUDE            (HELPID_GAUGE + 0x0012)
#define HELPID_GAUGE_BATTERY_SWITCH             (HELPID_GAUGE + 0x0013)
#define HELPID_GAUGE_BOOST_PUMP_SWITCH          (HELPID_GAUGE + 0x0014)
#define HELPID_GAUGE_CHT                        (HELPID_GAUGE + 0x0015)
#define HELPID_GAUGE_CLOCK                      (HELPID_GAUGE + 0x0016)
#define HELPID_GAUGE_COWL_FLAPS                 (HELPID_GAUGE + 0x0017)
#define HELPID_GAUGE_COWL_FLAPS_INDICATOR       (HELPID_GAUGE + 0x0018)
#define HELPID_GAUGE_COWL_FLAPS_SWITCH          (HELPID_GAUGE + 0x0019)
#define HELPID_GAUGE_DEICE_SWITCH               (HELPID_GAUGE + 0x001A)
#define HELPID_GAUGE_DME1                       (HELPID_GAUGE + 0x001B)
#define HELPID_GAUGE_DME2                       (HELPID_GAUGE + 0x001C)
#define HELPID_GAUGE_EGT                        (HELPID_GAUGE + 0x001D)
#define HELPID_GAUGE_EICAS                      (HELPID_GAUGE + 0x001E)
#define HELPID_GAUGE_ELAPSED_TIME               (HELPID_GAUGE + 0x001F)
#define HELPID_GAUGE_ENGINE_CONTROLS            (HELPID_GAUGE + 0x0020)
#define HELPID_GAUGE_ENGINE_N1                  (HELPID_GAUGE + 0x0021)
#define HELPID_GAUGE_ENGINE_N2                  (HELPID_GAUGE + 0x0022)
#define HELPID_GAUGE_ENGINE_TEMPERATURE         (HELPID_GAUGE + 0x0023)
#define HELPID_GAUGE_FLAP_INDICATOR             (HELPID_GAUGE + 0x0024)
#define HELPID_GAUGE_FLAPS_SWITCH               (HELPID_GAUGE + 0x0025)
#define HELPID_GAUGE_FUEL                       (HELPID_GAUGE + 0x0026)
#define HELPID_GAUGE_FUEL_CUTOFF                (HELPID_GAUGE + 0x0027)
#define HELPID_GAUGE_FUEL_FLOW                  (HELPID_GAUGE + 0x0028)
#define HELPID_GAUGE_FUEL_LEFT                  (HELPID_GAUGE + 0x0029)
#define HELPID_GAUGE_FUEL_RIGHT                 (HELPID_GAUGE + 0x002A)
#define HELPID_GAUGE_FUEL_LOW_LIGHT             (HELPID_GAUGE + 0x002B)
#define HELPID_GAUGE_FUEL_PRESSURE              (HELPID_GAUGE + 0x002C)
#define HELPID_GAUGE_FUEL_SELECTOR              (HELPID_GAUGE + 0x002D)
#define HELPID_GAUGE_FUEL_XFEED                 (HELPID_GAUGE + 0x002E)
#define HELPID_GAUGE_GAS_GENERATOR_RPM          (HELPID_GAUGE + 0x002F)
#define HELPID_GAUGE_GEAR_LEVER                 (HELPID_GAUGE + 0x0030)
#define HELPID_GAUGE_GEAR_LIGHT                 (HELPID_GAUGE + 0x0031)
#define HELPID_GAUGE_GLIDE_SLOPE                (HELPID_GAUGE + 0x0032)
#define HELPID_GAUGE_HEADING_BUG_ADJUST         (HELPID_GAUGE + 0x0033)
#define HELPID_GAUGE_HEADING_INDICATOR          (HELPID_GAUGE + 0x0034)
#define HELPID_GAUGE_HEADING_INDICATOR_ADJUST   (HELPID_GAUGE + 0x0035)
#define HELPID_GAUGE_HSI                        (HELPID_GAUGE + 0x0036)
#define HELPID_GAUGE_HSI_HUD                    (HELPID_GAUGE + 0x0037)
#define HELPID_GAUGE_HSI_OBS1                   (HELPID_GAUGE + 0x0038)
#define HELPID_GAUGE_IOAT                       (HELPID_GAUGE + 0x0039)
#define HELPID_GAUGE_KOHLSMAN_KNOB              (HELPID_GAUGE + 0x003A)
#define HELPID_GAUGE_LIGHT_AP_INDICATOR         (HELPID_GAUGE + 0x003B)
#define HELPID_GAUGE_LIGHT_MASTER_WARNING       (HELPID_GAUGE + 0x003C)
#define HELPID_GAUGE_LIGHT_SWITCH_BEACON        (HELPID_GAUGE + 0x003D)
#define HELPID_GAUGE_LIGHT_SWITCH_LANDING       (HELPID_GAUGE + 0x003E)
#define HELPID_GAUGE_LIGHT_SWITCH_NAV           (HELPID_GAUGE + 0x003F)
#define HELPID_GAUGE_LIGHT_SWITCH_PANEL         (HELPID_GAUGE + 0x0040)
#define HELPID_GAUGE_LIGHT_SWITCH_RECOGNITION   (HELPID_GAUGE + 0x0041)
#define HELPID_GAUGE_LIGHT_SWITCH_STROBE        (HELPID_GAUGE + 0x0042)
#define HELPID_GAUGE_LIGHT_SWITCH_TAXI          (HELPID_GAUGE + 0x0043)
#define HELPID_GAUGE_LOCALIZER_INDEX            (HELPID_GAUGE + 0x0044)
#define HELPID_GAUGE_MAGNETO_SWITCH             (HELPID_GAUGE + 0x0045)
#define HELPID_GAUGE_MANIFOLD_PRESSURE          (HELPID_GAUGE + 0x0046)
#define HELPID_GAUGE_MARKER_OUTER               (HELPID_GAUGE + 0x0047)
#define HELPID_GAUGE_MARKER_MIDDLE              (HELPID_GAUGE + 0x0048)
#define HELPID_GAUGE_MARKER_INNER               (HELPID_GAUGE + 0x0049)
#define HELPID_GAUGE_MASTER_SWITCH              (HELPID_GAUGE + 0x004A)
#define HELPID_GAUGE_MASTER_ALT_BAT             (HELPID_GAUGE + 0x004B)
#define HELPID_GAUGE_MASTER_CAUTION_LIGHT       (HELPID_GAUGE + 0x004C)
#define HELPID_GAUGE_NAV_GPS_SWITCH             (HELPID_GAUGE + 0x004D)
#define HELPID_GAUGE_OIL_PRESSURE               (HELPID_GAUGE + 0x004E)
#define HELPID_GAUGE_OIL_TEMPERATURE            (HELPID_GAUGE + 0x004F)
#define HELPID_GAUGE_OMI_LIGHTS                 (HELPID_GAUGE + 0x0050)
#define HELPID_GAUGE_PARKING_BRAKE              (HELPID_GAUGE + 0x0051)
#define HELPID_GAUGE_PFD                        (HELPID_GAUGE + 0x0052)
#define HELPID_GAUGE_PITCH_TRIM                 (HELPID_GAUGE + 0x0053)
#define HELPID_GAUGE_PITCH_TRIM_INDICATOR       (HELPID_GAUGE + 0x0054)
#define HELPID_GAUGE_PITCH_TRIM_KNOB            (HELPID_GAUGE + 0x0055)
#define HELPID_GAUGE_PITCH_TRIM_SWITCH          (HELPID_GAUGE + 0x0056)
#define HELPID_GAUGE_PITOT_HEAT_SWITCH          (HELPID_GAUGE + 0x0057)
#define HELPID_GAUGE_PROP_RPM                   (HELPID_GAUGE + 0x0058)
#define HELPID_GAUGE_PROP_SYNC                  (HELPID_GAUGE + 0x0059)
#define HELPID_GAUGE_RMI                        (HELPID_GAUGE + 0x005A)
#define HELPID_GAUGE_RMI_DME                    (HELPID_GAUGE + 0x005B)
#define HELPID_GAUGE_ROLL_TRIM                  (HELPID_GAUGE + 0x005C)
#define HELPID_GAUGE_ROLL_TRIM_INDICATOR        (HELPID_GAUGE + 0x005D)
#define HELPID_GAUGE_ROLL_TRIM_KNOB             (HELPID_GAUGE + 0x005E)
#define HELPID_GAUGE_ROLL_TRIM_SWITCH           (HELPID_GAUGE + 0x005F)
#define HELPID_GAUGE_SIM_RATE                   (HELPID_GAUGE + 0x0060)
#define HELPID_GAUGE_SLIP_SKID                  (HELPID_GAUGE + 0x0061)
#define HELPID_GAUGE_SPOILER                    (HELPID_GAUGE + 0x0062)
#define HELPID_GAUGE_STARTER_LEFT               (HELPID_GAUGE + 0x0063)
#define HELPID_GAUGE_STARTER_RIGHT              (HELPID_GAUGE + 0x0064)
#define HELPID_GAUGE_STARTER_SWITCH             (HELPID_GAUGE + 0x0065)
#define HELPID_GAUGE_SUCTION                    (HELPID_GAUGE + 0x0066)
#define HELPID_GAUGE_SWITCH_FUEL_PUMP           (HELPID_GAUGE + 0x0067)
#define HELPID_GAUGE_TACHOMETER                 (HELPID_GAUGE + 0x0068)
#define HELPID_GAUGE_TAS_CALIBRATION            (HELPID_GAUGE + 0x0069)
#define HELPID_GAUGE_THROTTLE_PROP              (HELPID_GAUGE + 0x006A)
#define HELPID_GAUGE_THROTTLE_MIXTURE           (HELPID_GAUGE + 0x006B)
#define HELPID_GAUGE_THROTTLE_THROTTLE          (HELPID_GAUGE + 0x006C)
#define HELPID_GAUGE_THROTTLE_QUADRANT          (HELPID_GAUGE + 0x006D)
#define HELPID_GAUGE_THRUST_LEVERS              (HELPID_GAUGE + 0x006E)
#define HELPID_GAUGE_TIT                        (HELPID_GAUGE + 0x006F)
#define HELPID_GAUGE_TORQUE                     (HELPID_GAUGE + 0x0070)
#define HELPID_GAUGE_TURN_COORDINATOR           (HELPID_GAUGE + 0x0071)
#define HELPID_GAUGE_VOR1                       (HELPID_GAUGE + 0x0072)
#define HELPID_GAUGE_VOR1_OBS                   (HELPID_GAUGE + 0x0073)
#define HELPID_GAUGE_VOR2                       (HELPID_GAUGE + 0x0074)
#define HELPID_GAUGE_VOR2_OBS2                  (HELPID_GAUGE + 0x0075)
#define HELPID_GAUGE_VSI                        (HELPID_GAUGE + 0x0076)
#define HELPID_GAUGE_WHISKEY_COMPASS            (HELPID_GAUGE + 0x0077)
#define HELPID_GAUGE_YAW_TRIM                   (HELPID_GAUGE + 0x0078)
#define HELPID_GAUGE_YAW_TRIM_INDICATOR         (HELPID_GAUGE + 0x0079)
#define HELPID_GAUGE_YAW_TRIM_KNOB              (HELPID_GAUGE + 0x007A)
#define HELPID_GAUGE_YAW_TRIM_SWITCH            (HELPID_GAUGE + 0x007B)
#define HELPID_GAUGE_AUTOBRAKE_SWITCH           (HELPID_GAUGE + 0x007C)
#define HELPID_GAUGE_FLAP_LEVER                 (HELPID_GAUGE + 0x007D)
#define HELPID_GAUGE_FLAP_LEVER_SOLO            (HELPID_GAUGE + 0x007E)
#define HELPID_GAUGE_GENERATOR_SWITCH           (HELPID_GAUGE + 0x007F)
#define HELPID_GAUGE_GENERATOR_LEFT             (HELPID_GAUGE + 0x0080)
#define HELPID_GAUGE_GENERATOR_RIGHT            (HELPID_GAUGE + 0x0081)
#define HELPID_GAUGE_LIGHT_SWITCH_WING          (HELPID_GAUGE + 0x0082)
#define HELPID_GAUGE_LIGHT_SWITCH_LOGO          (HELPID_GAUGE + 0x0083)
#define HELPID_GAUGE_TURN_BANK                  (HELPID_GAUGE + 0x0084)
#define HELPID_GAUGE_VSI_KNOB                   (HELPID_GAUGE + 0x0085)
#define HELPID_GAUGE_TOGGLE_RADIO_WINDOW        (HELPID_GAUGE + 0x0086)
#define HELPID_GAUGE_TOGGLE_GPS_WINDOW          (HELPID_GAUGE + 0x0087)
#define HELPID_GAUGE_TOGGLE_THROTTLE_WINDOW     (HELPID_GAUGE + 0x0088)
#define HELPID_GAUGE_TOGGLE_COMPASS_WINDOW      (HELPID_GAUGE + 0x0089)
#define HELPID_GAUGE_TOGGLE_FUEL_WINDOW         (HELPID_GAUGE + 0x008A)
#define HELPID_GAUGE_TOGGLE_COLLECTIVE_WINDOW   (HELPID_GAUGE + 0x008B)
#define HELPID_GAUGE_HYDRAULIC_PRESSURE         (HELPID_GAUGE + 0x008C)
#define HELPID_GAUGE_COOLANT_TEMPERATURE        (HELPID_GAUGE + 0x008D)
#define HELPID_GAUGE_STALL_LIGHT                (HELPID_GAUGE + 0x008E)
#define HELPID_GAUGE_FLIGHT_MAP                 (HELPID_GAUGE + 0x008F)
#define HELPID_GAUGE_VOR2_OBS                   (HELPID_GAUGE + 0x0090)
#define HELPID_GAUGE_THROTTLE_EMERGENCY_THROTTLE (HELPID_GAUGE + 0x0091)
#define HELPID_GAUGE_WATER_RUDDER_LEVER         (HELPID_GAUGE + 0x0092)
#define HELPID_GAUGE_TOGGLE_CHECKLIST_WINDOW    (HELPID_GAUGE + 0x0093)
#define HELPID_GAUGE_TOGGLE_ATC_WINDOW          (HELPID_GAUGE + 0x0094)
#define HELPID_GAUGE_TOGGLE_OVERHEAD_WINDOW     (HELPID_GAUGE + 0x0095)
#define HELPID_GAUGE_TOGGLE_CLOCK_WINDOW        (HELPID_GAUGE + 0x0096)
#define HELPID_GAUGE_SPEED_BRAKE                (HELPID_GAUGE + 0x0097)
#define HELPID_GAUGE_BOOT_ICING_SWITCH          (HELPID_GAUGE + 0x0098)
#define HELPID_GAUGE_ANNUNCIATOR_AFT_DOOR       (HELPID_GAUGE + 0x0099)
#define HELPID_GAUGE_ANNUNCIATOR_GEAR_UP        (HELPID_GAUGE + 0x009A)
#define HELPID_GAUGE_ANNUNCIATOR_LEFT_ALTERNATOR (HELPID_GAUGE + 0x009B)
#define HELPID_GAUGE_ANNUNCIATOR_RIGHT_ALTERNATOR (HELPID_GAUGE + 0x009C)
#define HELPID_GAUGE_ANNUNCIATOR_STARTER        (HELPID_GAUGE + 0x009D)
#define HELPID_GAUGE_REF_ALT_MARKER_BUG         (HELPID_GAUGE + 0x009E)
#define HELPID_GAUGE_TOGGLE_FLOOR_WINDOW        (HELPID_GAUGE + 0x009F)
#define HELPID_GAUGE_RADIO_ALTIMETER            (HELPID_CONCORDE_RADIO_ALTIMETER)
#define HELPID_GAUGE_RADIO_ALTIMETER_KNOB       (HELPID_CONCORDE_RADIO_ALTIMETER_KNOB)
#define HELPID_GAUGE_CONDITION_LEVERS           (HELPID_KINGAIR_CONDITION_LEVERS)
#define HELPID_GAUGE_PROP_LEVERS                (HELPID_KINGAIR_PROP_LEVERS)
#define HELPID_GAUGE_DISPLAY_IFR_WINDOW         (HELPID_GAUGE + 0x00A0)
#define HELPID_GAUGE_DISPLAY_LANDING_WINDOW     (HELPID_GAUGE + 0x00A1)
#define HELPID_GAUGE_DISPLAY_VFR_WINDOW         (HELPID_GAUGE + 0x00A2)
#define HELPID_GAUGE_CLOCK_MODE_SELECT          (HELPID_GAUGE + 0x00A3)
#define HELPID_GAUGE_CLOCK_RESET                (HELPID_GAUGE + 0x00A4)
#define HELPID_GAUGE_CLOSE_POPUP_WINDOW         (HELPID_GAUGE + 0x00A5)


#define TOOLTIPTEXT_BASE                            0x3000

#define TOOLTIPTEXT_AIRSPEED_KILOS                  (TOOLTIPTEXT_BASE + 0x0001)
#define TOOLTIPTEXT_AIRSPEED_KNOTS                  (TOOLTIPTEXT_BASE + 0x0002)
#define TOOLTIPTEXT_AIRSPEED_MILES                  (TOOLTIPTEXT_BASE + 0x0003)
#define TOOLTIPTEXT_RADIO_ALTIMETER_METERS          (TOOLTIPTEXT_BASE + 0x0008)
#define TOOLTIPTEXT_RADIO_ALTIMETER_FEET            (TOOLTIPTEXT_BASE + 0x0009)
#define TOOLTIPTEXT_RADIO_ALTIMETER_DH_METERS       (TOOLTIPTEXT_BASE + 0x000A)
#define TOOLTIPTEXT_RADIO_ALTIMETER_DH_FEET         (TOOLTIPTEXT_BASE + 0x000B)
#define TOOLTIPTEXT_RADIO_ALTIMETER_DH_LIGHT        (TOOLTIPTEXT_BASE + 0x000C)
#define TOOLTIPTEXT_FLAP_INDICATOR_POSITION         (TOOLTIPTEXT_BASE + 0x0010)
#define TOOLTIPTEXT_FLAP_LEVER_DEGREES              (TOOLTIPTEXT_BASE + 0x0011)
#define TOOLTIPTEXT_FLAP_INDICATOR_DEGREES          (TOOLTIPTEXT_BASE + 0x0012)
#define TOOLTIPTEXT_FLAPS_SWITCH                    (TOOLTIPTEXT_BASE + 0x0013)
#define TOOLTIPTEXT_CLOCK                           (TOOLTIPTEXT_BASE + 0x0018)
#define TOOLTIPTEXT_CLOCK_SIMRATE                   (TOOLTIPTEXT_BASE + 0x0019)
#define TOOLTIPTEXT_WATER_RUDDER_HANDLE             (TOOLTIPTEXT_BASE + 0x0020)
#define TOOLTIPTEXT_LIGHT_ENGINE_FIRE               (TOOLTIPTEXT_BASE + 0x0030)
#define TOOLTIPTEXT_LIGHT_OIL_PRESSURE_LOW          (TOOLTIPTEXT_BASE + 0x0031)
#define TOOLTIPTEXT_LIGHT_GENERATOR_OFF             (TOOLTIPTEXT_BASE + 0x0032)
#define TOOLTIPTEXT_LIGHT_EMERGENCY_POWER_LEVER_ON  (TOOLTIPTEXT_BASE + 0x0033)
#define TOOLTIPTEXT_LIGHT_AUXILIARY_FUEL_PUMP_ON    (TOOLTIPTEXT_BASE + 0x0034)
#define TOOLTIPTEXT_LIGHT_FUEL_PRESSURE_LOW         (TOOLTIPTEXT_BASE + 0x0035)
#define TOOLTIPTEXT_LIGHT_STARTER_ON                (TOOLTIPTEXT_BASE + 0x0036)
#define TOOLTIPTEXT_LIGHT_IGNITION_ON               (TOOLTIPTEXT_BASE + 0x0037)
#define TOOLTIPTEXT_LIGHT_VOLTAGE_LOW               (TOOLTIPTEXT_BASE + 0x0038)
#define TOOLTIPTEXT_LIGHT_VACUUM_LOW                (TOOLTIPTEXT_BASE + 0x0039)
#define TOOLTIPTEXT_LIGHT_RESERVOIR_FUEL_LOW        (TOOLTIPTEXT_BASE + 0x003A)
#define TOOLTIPTEXT_LIGHT_LEFT_TANK_FUEL_LOW        (TOOLTIPTEXT_BASE + 0x003B)
#define TOOLTIPTEXT_LIGHT_RIGHT_TANK_FUEL_LOW       (TOOLTIPTEXT_BASE + 0x003C)
#define TOOLTIPTEXT_LIGHT_STANDBY_POWER_ON          (TOOLTIPTEXT_BASE + 0x003D)
#define TOOLTIPTEXT_LIGHT_DEICE_ON                  (TOOLTIPTEXT_BASE + 0x003E)
#define TOOLTIPTEXT_LIGHT_FUEL_SELECTOR_NOT_ALL     (TOOLTIPTEXT_BASE + 0x003F)
#define TOOLTIPTEXT_ALTIMETER_METERS                (TOOLTIPTEXT_BASE + 0x0050)
#define TOOLTIPTEXT_ALTIMETER_FEET                  (TOOLTIPTEXT_BASE + 0x0051)
#define TOOLTIPTEXT_ALTIMETER_FEET_METERS_SPECIAL   (TOOLTIPTEXT_BASE + 0x0052)
#define TOOLTIPTEXT_ALTIMETER_KOHLSMAN_INHG         (TOOLTIPTEXT_BASE + 0x0053)
#define TOOLTIPTEXT_ALTIMETER_KOHLSMAN_MBAR         (TOOLTIPTEXT_BASE + 0x0054)
#define TOOLTIPTEXT_AUTOPILOT_MASTER                (TOOLTIPTEXT_BASE + 0x0060)
#define TOOLTIPTEXT_AUTOPILOT_FLIGHT_DIRECTOR_SWITCH (TOOLTIPTEXT_BASE + 0x0061)
#define TOOLTIPTEXT_AUTOPILOT_WING_LEVELER          (TOOLTIPTEXT_BASE + 0x0062)
#define TOOLTIPTEXT_AUTOPILOT_IAS_HOLD              (TOOLTIPTEXT_BASE + 0x0063)
#define TOOLTIPTEXT_AUTOPILOT_BC_HOLD               (TOOLTIPTEXT_BASE + 0x0064)
#define TOOLTIPTEXT_AUTOPILOT_APP_HOLD              (TOOLTIPTEXT_BASE + 0x0065)
#define TOOLTIPTEXT_AUTOPILOT_YAW_DAMP_HOLD         (TOOLTIPTEXT_BASE + 0x0066)
#define TOOLTIPTEXT_AUTOPILOT_NAV_HOLD              (TOOLTIPTEXT_BASE + 0x0067)
#define TOOLTIPTEXT_AUTOPILOT_HEADING_HOLD          (TOOLTIPTEXT_BASE + 0x0068)
#define TOOLTIPTEXT_AUTOPILOT_HEADING_INDICATOR     (TOOLTIPTEXT_BASE + 0x0069)
#define TOOLTIPTEXT_AUTOPILOT_VSI_HOLD              (TOOLTIPTEXT_BASE + 0x006A)
#define TOOLTIPTEXT_AUTOPILOT_VSI_INDICATOR_FTMIN   (TOOLTIPTEXT_BASE + 0x006B)
#define TOOLTIPTEXT_AUTOPILOT_VSI_INDICATOR_MS      (TOOLTIPTEXT_BASE + 0x006C)
#define TOOLTIPTEXT_AUTOPILOT_ALTITUDE_HOLD         (TOOLTIPTEXT_BASE + 0x006D)
#define TOOLTIPTEXT_AUTOPILOT_ALTITUDE_INDICATOR_FEET (TOOLTIPTEXT_BASE + 0x006E)
#define TOOLTIPTEXT_AUTOPILOT_ALTITUDE_INDICATOR_METERS (TOOLTIPTEXT_BASE + 0x006F)
#define TOOLTIPTEXT_AUTOPILOT_SPEED_HOLD            (TOOLTIPTEXT_BASE + 0x0070)
#define TOOLTIPTEXT_AUTOPILOT_SPEED_INDICATOR_KNOTS (TOOLTIPTEXT_BASE + 0x0071)
#define TOOLTIPTEXT_AUTOPILOT_SPEED_INDICATOR_KILOS (TOOLTIPTEXT_BASE + 0x0072)
#define TOOLTIPTEXT_AUTOPILOT_AUTOTHROTTLE          (TOOLTIPTEXT_BASE + 0x0073)
#define TOOLTIPTEXT_AUTOPILOT_MACH_HOLD             (TOOLTIPTEXT_BASE + 0x0074)
#define TOOLTIPTEXT_AUTOPILOT_MACH_INDICATOR        (TOOLTIPTEXT_BASE + 0x0075)
#define TOOLTIPTEXT_SUCTION_MBAR                    (TOOLTIPTEXT_BASE + 0x0080)
#define TOOLTIPTEXT_SUCTION_INHG                    (TOOLTIPTEXT_BASE + 0x0081)
#define TOOLTIPTEXT_TURN_COORDINATOR_RATE           (TOOLTIPTEXT_BASE + 0x0090)
#define TOOLTIPTEXT_ATTITUDE_INDICATOR_BANK_PITCH   (TOOLTIPTEXT_BASE + 0x0092)
#define TOOLTIPTEXT_HSI_HEADING_COURSE              (TOOLTIPTEXT_BASE + 0x0094)
#define TOOLTIPTEXT_VSI_METERS_PER_SEC              (TOOLTIPTEXT_BASE + 0x0098)
#define TOOLTIPTEXT_VSI_FEET_PER_MIN                (TOOLTIPTEXT_BASE + 0x0099)
#define TOOLTIPTEXT_MANIFOLD_PRESSURE_INHG          (TOOLTIPTEXT_BASE + 0x00A0)
#define TOOLTIPTEXT_ENG1_MANIFOLD_PRESSURE_INHG     (TOOLTIPTEXT_BASE + 0x00A1)
#define TOOLTIPTEXT_ENG2_MANIFOLD_PRESSURE_INHG     (TOOLTIPTEXT_BASE + 0x00A2)
#define TOOLTIPTEXT_ENG3_MANIFOLD_PRESSURE_INHG     (TOOLTIPTEXT_BASE + 0x00A3)
#define TOOLTIPTEXT_ENG4_MANIFOLD_PRESSURE_INHG     (TOOLTIPTEXT_BASE + 0x00A4)
#define TOOLTIPTEXT_MANIFOLD_PRESSURE_PASCAL        (TOOLTIPTEXT_BASE + 0x00A8)
#define TOOLTIPTEXT_ENG1_MANIFOLD_PRESSURE_PASCAL   (TOOLTIPTEXT_BASE + 0x00A9)
#define TOOLTIPTEXT_ENG2_MANIFOLD_PRESSURE_PASCAL   (TOOLTIPTEXT_BASE + 0x00AA)
#define TOOLTIPTEXT_ENG3_MANIFOLD_PRESSURE_PASCAL   (TOOLTIPTEXT_BASE + 0x00AB)
#define TOOLTIPTEXT_ENG4_MANIFOLD_PRESSURE_PASCAL   (TOOLTIPTEXT_BASE + 0x00AC)
#define TOOLTIPTEXT_PROP_RPM                        (TOOLTIPTEXT_BASE + 0x00B0)
#define TOOLTIPTEXT_ENG1_PROP_RPM                   (TOOLTIPTEXT_BASE + 0x00B1)
#define TOOLTIPTEXT_ENG2_PROP_RPM                   (TOOLTIPTEXT_BASE + 0x00B2)
#define TOOLTIPTEXT_ENG3_PROP_RPM                   (TOOLTIPTEXT_BASE + 0x00B3)
#define TOOLTIPTEXT_ENG4_PROP_RPM                   (TOOLTIPTEXT_BASE + 0x00B4)
#define TOOLTIPTEXT_PROP_SYNCHROSCOPE               (TOOLTIPTEXT_BASE + 0x00B8)
#define TOOLTIPTEXT_FUEL_FLOW_GALLON_HOUR           (TOOLTIPTEXT_BASE + 0x00C0)
#define TOOLTIPTEXT_ENG1_FUEL_FLOW_GALLON_HOUR      (TOOLTIPTEXT_BASE + 0x00C1)
#define TOOLTIPTEXT_ENG2_FUEL_FLOW_GALLON_HOUR      (TOOLTIPTEXT_BASE + 0x00C2)
#define TOOLTIPTEXT_ENG3_FUEL_FLOW_GALLON_HOUR      (TOOLTIPTEXT_BASE + 0x00C3)
#define TOOLTIPTEXT_ENG4_FUEL_FLOW_GALLON_HOUR      (TOOLTIPTEXT_BASE + 0x00C4)
#define TOOLTIPTEXT_FUEL_FLOW_LITER_HOUR            (TOOLTIPTEXT_BASE + 0x00C5)
#define TOOLTIPTEXT_ENG1_FUEL_FLOW_LITER_HOUR       (TOOLTIPTEXT_BASE + 0x00C6)
#define TOOLTIPTEXT_ENG2_FUEL_FLOW_LITER_HOUR       (TOOLTIPTEXT_BASE + 0x00C7)
#define TOOLTIPTEXT_ENG3_FUEL_FLOW_LITER_HOUR       (TOOLTIPTEXT_BASE + 0x00C8)
#define TOOLTIPTEXT_ENG4_FUEL_FLOW_LITER_HOUR       (TOOLTIPTEXT_BASE + 0x00C9)
#define TOOLTIPTEXT_FUEL_FLOW_POUND_HOUR            (TOOLTIPTEXT_BASE + 0x00CA)
#define TOOLTIPTEXT_ENG1_FUEL_FLOW_POUND_HOUR       (TOOLTIPTEXT_BASE + 0x00CB)
#define TOOLTIPTEXT_ENG2_FUEL_FLOW_POUND_HOUR       (TOOLTIPTEXT_BASE + 0x00CC)
#define TOOLTIPTEXT_ENG3_FUEL_FLOW_POUND_HOUR       (TOOLTIPTEXT_BASE + 0x00CD)
#define TOOLTIPTEXT_ENG4_FUEL_FLOW_POUND_HOUR       (TOOLTIPTEXT_BASE + 0x00CE)
#define TOOLTIPTEXT_CHT_CELSIUS                     (TOOLTIPTEXT_BASE + 0x00D0)
#define TOOLTIPTEXT_ENG1_CHT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00D1)
#define TOOLTIPTEXT_ENG2_CHT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00D2)
#define TOOLTIPTEXT_ENG3_CHT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00D3)
#define TOOLTIPTEXT_ENG4_CHT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00D4)
#define TOOLTIPTEXT_CHT_FARENHEIT                   (TOOLTIPTEXT_BASE + 0x00D8)
#define TOOLTIPTEXT_ENG1_CHT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00D9)
#define TOOLTIPTEXT_ENG2_CHT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00DA)
#define TOOLTIPTEXT_ENG3_CHT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00DB)
#define TOOLTIPTEXT_ENG4_CHT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00DC)
#define TOOLTIPTEXT_EGT_CELSIUS                     (TOOLTIPTEXT_BASE + 0x00E0)
#define TOOLTIPTEXT_ENG1_EGT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00E1)
#define TOOLTIPTEXT_ENG2_EGT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00E2)
#define TOOLTIPTEXT_ENG3_EGT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00E3)
#define TOOLTIPTEXT_ENG4_EGT_CELSIUS                (TOOLTIPTEXT_BASE + 0x00E4)
#define TOOLTIPTEXT_EGT_FARENHEIT                   (TOOLTIPTEXT_BASE + 0x00E8)
#define TOOLTIPTEXT_ENG1_EGT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00E9)
#define TOOLTIPTEXT_ENG2_EGT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00EA)
#define TOOLTIPTEXT_ENG3_EGT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00EB)
#define TOOLTIPTEXT_ENG4_EGT_FARENHEIT              (TOOLTIPTEXT_BASE + 0x00EC)
#define TOOLTIPTEXT_OIL_TEMPERATURE_CELSIUS         (TOOLTIPTEXT_BASE + 0x00F0)
#define TOOLTIPTEXT_ENG1_OIL_TEMPERATURE_CELSIUS    (TOOLTIPTEXT_BASE + 0x00F1) 
#define TOOLTIPTEXT_ENG2_OIL_TEMPERATURE_CELSIUS    (TOOLTIPTEXT_BASE + 0x00F2) 
#define TOOLTIPTEXT_ENG3_OIL_TEMPERATURE_CELSIUS    (TOOLTIPTEXT_BASE + 0x00F3) 
#define TOOLTIPTEXT_ENG4_OIL_TEMPERATURE_CELSIUS    (TOOLTIPTEXT_BASE + 0x00F4) 
#define TOOLTIPTEXT_OIL_TEMPERATURE_FARENHEIT       (TOOLTIPTEXT_BASE + 0x00F8)
#define TOOLTIPTEXT_ENG1_OIL_TEMPERATURE_FARENHEIT  (TOOLTIPTEXT_BASE + 0x00F9)
#define TOOLTIPTEXT_ENG2_OIL_TEMPERATURE_FARENHEIT  (TOOLTIPTEXT_BASE + 0x00FA)  
#define TOOLTIPTEXT_ENG3_OIL_TEMPERATURE_FARENHEIT  (TOOLTIPTEXT_BASE + 0x00FB)  
#define TOOLTIPTEXT_ENG4_OIL_TEMPERATURE_FARENHEIT  (TOOLTIPTEXT_BASE + 0x00FC)  
#define TOOLTIPTEXT_OIL_PRESSURE_PSI                (TOOLTIPTEXT_BASE + 0x0100)
#define TOOLTIPTEXT_ENG1_OIL_PRESSURE_PSI           (TOOLTIPTEXT_BASE + 0x0101)
#define TOOLTIPTEXT_ENG2_OIL_PRESSURE_PSI           (TOOLTIPTEXT_BASE + 0x0102)
#define TOOLTIPTEXT_ENG3_OIL_PRESSURE_PSI           (TOOLTIPTEXT_BASE + 0x0103)
#define TOOLTIPTEXT_ENG4_OIL_PRESSURE_PSI           (TOOLTIPTEXT_BASE + 0x0104)
#define TOOLTIPTEXT_OIL_PRESSURE_KGFSQCM            (TOOLTIPTEXT_BASE + 0x0108)
#define TOOLTIPTEXT_ENG1_OIL_PRESSURE_KGFSQCM       (TOOLTIPTEXT_BASE + 0x0109)
#define TOOLTIPTEXT_ENG2_OIL_PRESSURE_KGFSQCM       (TOOLTIPTEXT_BASE + 0x010A)
#define TOOLTIPTEXT_ENG3_OIL_PRESSURE_KGFSQCM       (TOOLTIPTEXT_BASE + 0x010B)
#define TOOLTIPTEXT_ENG4_OIL_PRESSURE_KGFSQCM       (TOOLTIPTEXT_BASE + 0x010C)
#define TOOLTIPTEXT_LEFT_FUEL_PERCENT               (TOOLTIPTEXT_BASE + 0x0110)
#define TOOLTIPTEXT_RIGHT_FUEL_PERCENT              (TOOLTIPTEXT_BASE + 0x0111)
#define TOOLTIPTEXT_LEFT_FUEL_LBS                   (TOOLTIPTEXT_BASE + 0x0112)
#define TOOLTIPTEXT_RIGHT_FUEL_LBS                  (TOOLTIPTEXT_BASE + 0x0113)
#define TOOLTIPTEXT_LEFT_FUEL_KGS                   (TOOLTIPTEXT_BASE + 0x0114)
#define TOOLTIPTEXT_RIGHT_FUEL_KGS                  (TOOLTIPTEXT_BASE + 0x0115)
#define TOOLTIPTEXT_LEFT_FUEL_GALLONS               (TOOLTIPTEXT_BASE + 0x0116)
#define TOOLTIPTEXT_RIGHT_FUEL_GALLONS              (TOOLTIPTEXT_BASE + 0x0117)
#define TOOLTIPTEXT_LEFT_FUEL_LITERS                (TOOLTIPTEXT_BASE + 0x0118)
#define TOOLTIPTEXT_RIGHT_FUEL_LITERS               (TOOLTIPTEXT_BASE + 0x0119)
#define TOOLTIPTEXT_MARKER_OUTER                    (TOOLTIPTEXT_BASE + 0x0120)
#define TOOLTIPTEXT_MARKER_MIDDLE                   (TOOLTIPTEXT_BASE + 0x0121)
#define TOOLTIPTEXT_MARKER_INNER                    (TOOLTIPTEXT_BASE + 0x0122)
#define TOOLTIPTEXT_PITCH_TRIM                      (TOOLTIPTEXT_BASE + 0x0128)
#define TOOLTIPTEXT_ROLL_TRIM                       (TOOLTIPTEXT_BASE + 0x0129)
#define TOOLTIPTEXT_YAW_TRIM                        (TOOLTIPTEXT_BASE + 0x012A)
#define TOOLTIPTEXT_YAW_TRIM_INDICATOR              (TOOLTIPTEXT_BASE + 0x012B)
#define TOOLTIPTEXT_YAW_TRIM_KNOB                   (TOOLTIPTEXT_BASE + 0x012C)
#define TOOLTIPTEXT_GEAR_LIGHT                      (TOOLTIPTEXT_BASE + 0x0130)
#define TOOLTIPTEXT_GEAR_LEVER                      (TOOLTIPTEXT_BASE + 0x0131)
#define TOOLTIPTEXT_AMMETER                         (TOOLTIPTEXT_BASE + 0x0140)
#define TOOLTIPTEXT_AUDIO_PANEL_TRANSMIT_RECEIVE    (TOOLTIPTEXT_BASE + 0x0150)
#define TOOLTIPTEXT_COMM1_FREQ_ACTIVE_STANDBY       (TOOLTIPTEXT_BASE + 0x0160)
#define TOOLTIPTEXT_COMM2_FREQ_ACTIVE_STANDBY       (TOOLTIPTEXT_BASE + 0x0161)
#define TOOLTIPTEXT_NAV1_FREQ_ACTIVE_STANDBY        (TOOLTIPTEXT_BASE + 0x0168)
#define TOOLTIPTEXT_NAV2_FREQ_ACTIVE_STANDBY        (TOOLTIPTEXT_BASE + 0x0169)
#define TOOLTIPTEXT_ADF_FREQ                        (TOOLTIPTEXT_BASE + 0x0170)
#define TOOLTIPTEXT_DME_NAV_NM_KT                   (TOOLTIPTEXT_BASE + 0x0178)
#define TOOLTIPTEXT_DME_NAV_KM_MS                   (TOOLTIPTEXT_BASE + 0x0179)
#define TOOLTIPTEXT_DME_NAV_NM_KT_MIN               (TOOLTIPTEXT_BASE + 0x017A)
#define TOOLTIPTEXT_DME_NAV_KM_MS_MIN               (TOOLTIPTEXT_BASE + 0x017B)
#define TOOLTIPTEXT_DME_N1_N2_SELECT                (TOOLTIPTEXT_BASE + 0x017C)
#define TOOLTIPTEXT_TRANSPONDER_ID                  (TOOLTIPTEXT_BASE + 0x0180)
#define TOOLTIPTEXT_THROTTLE_MIXTURE_PERCENT        (TOOLTIPTEXT_BASE + 0x0190)
#define TOOLTIPTEXT_THROTTLE_ENG1_MIXTURE_PERCENT   (TOOLTIPTEXT_BASE + 0x0191)
#define TOOLTIPTEXT_THROTTLE_ENG2_MIXTURE_PERCENT   (TOOLTIPTEXT_BASE + 0x0192)
#define TOOLTIPTEXT_THROTTLE_ENG3_MIXTURE_PERCENT   (TOOLTIPTEXT_BASE + 0x0193)
#define TOOLTIPTEXT_THROTTLE_ENG4_MIXTURE_PERCENT   (TOOLTIPTEXT_BASE + 0x0194)
#define TOOLTIPTEXT_THROTTLE_PROPELLER_PERCENT      (TOOLTIPTEXT_BASE + 0x0198)
#define TOOLTIPTEXT_THROTTLE_ENG1_PROPELLER_PERCENT (TOOLTIPTEXT_BASE + 0x0199)
#define TOOLTIPTEXT_THROTTLE_ENG2_PROPELLER_PERCENT (TOOLTIPTEXT_BASE + 0x019A)
#define TOOLTIPTEXT_THROTTLE_ENG3_PROPELLER_PERCENT (TOOLTIPTEXT_BASE + 0x019B)
#define TOOLTIPTEXT_THROTTLE_ENG4_PROPELLER_PERCENT (TOOLTIPTEXT_BASE + 0x019C)
#define TOOLTIPTEXT_THROTTLE_THROTTLE_PERCENT       (TOOLTIPTEXT_BASE + 0x01A0)
#define TOOLTIPTEXT_THROTTLE_ENG1_THROTTLE_PERCENT  (TOOLTIPTEXT_BASE + 0x01A1)
#define TOOLTIPTEXT_THROTTLE_ENG2_THROTTLE_PERCENT  (TOOLTIPTEXT_BASE + 0x01A2)
#define TOOLTIPTEXT_THROTTLE_ENG3_THROTTLE_PERCENT  (TOOLTIPTEXT_BASE + 0x01A3)
#define TOOLTIPTEXT_THROTTLE_ENG4_THROTTLE_PERCENT  (TOOLTIPTEXT_BASE + 0x01A4)
#define TOOLTIPTEXT_COWL_FLAPS_PERCENT              (TOOLTIPTEXT_BASE + 0x01A8)
#define TOOLTIPTEXT_ENG1_COWL_FLAPS_PERCENT         (TOOLTIPTEXT_BASE + 0x01A9)
#define TOOLTIPTEXT_ENG2_COWL_FLAPS_PERCENT         (TOOLTIPTEXT_BASE + 0x01AA)
#define TOOLTIPTEXT_ENG3_COWL_FLAPS_PERCENT         (TOOLTIPTEXT_BASE + 0x01AB)
#define TOOLTIPTEXT_ENG4_COWL_FLAPS_PERCENT         (TOOLTIPTEXT_BASE + 0x01AC)
#define TOOLTIPTEXT_FUEL_SELECTOR                   (TOOLTIPTEXT_BASE + 0x01B0)
#define TOOLTIPTEXT_LEFT_FUEL_SELECTOR              (TOOLTIPTEXT_BASE + 0x01B1)
#define TOOLTIPTEXT_RIGHT_FUEL_SELECTOR             (TOOLTIPTEXT_BASE + 0x01B2)
#define TOOLTIPTEXT_WHISKEY_COMPASS                 (TOOLTIPTEXT_BASE + 0x01B8)
#define TOOLTIPTEXT_DEICE_SWITCH                    (TOOLTIPTEXT_BASE + 0x01BC)
#define TOOLTIPTEXT_PITOT_HEAT_SWITCH               (TOOLTIPTEXT_BASE + 0x01BD)
#define TOOLTIPTEXT_TORQUE_PERCENT                  (TOOLTIPTEXT_BASE + 0x01C0)
#define TOOLTIPTEXT_ENG1_TORQUE_PERCENT             (TOOLTIPTEXT_BASE + 0x01C1)
#define TOOLTIPTEXT_ENG2_TORQUE_PERCENT             (TOOLTIPTEXT_BASE + 0x01C2)
#define TOOLTIPTEXT_ENG3_TORQUE_PERCENT             (TOOLTIPTEXT_BASE + 0x01C3)
#define TOOLTIPTEXT_ENG4_TORQUE_PERCENT             (TOOLTIPTEXT_BASE + 0x01C4)
#define TOOLTIPTEXT_TORQUE_FT_LBS                   (TOOLTIPTEXT_BASE + 0x01C5)
#define TOOLTIPTEXT_ENG1_TORQUE_FT_LBS              (TOOLTIPTEXT_BASE + 0x01C6)
#define TOOLTIPTEXT_ENG2_TORQUE_FT_LBS              (TOOLTIPTEXT_BASE + 0x01C7)
#define TOOLTIPTEXT_ENG3_TORQUE_FT_LBS              (TOOLTIPTEXT_BASE + 0x01C8)
#define TOOLTIPTEXT_ENG4_TORQUE_FT_LBS              (TOOLTIPTEXT_BASE + 0x01C9)
#define TOOLTIPTEXT_TORQUE_NEWTON                   (TOOLTIPTEXT_BASE + 0x01CA)
#define TOOLTIPTEXT_ENG1_TORQUE_NEWTON              (TOOLTIPTEXT_BASE + 0x01CB)
#define TOOLTIPTEXT_ENG2_TORQUE_NEWTON              (TOOLTIPTEXT_BASE + 0x01CC)
#define TOOLTIPTEXT_ENG3_TORQUE_NEWTON              (TOOLTIPTEXT_BASE + 0x01CD)
#define TOOLTIPTEXT_ENG4_TORQUE_NEWTON              (TOOLTIPTEXT_BASE + 0x01CE)
#define TOOLTIPTEXT_ITT_RANKINE                     (TOOLTIPTEXT_BASE + 0x01D0)
#define TOOLTIPTEXT_ENG1_ITT_RANKINE                (TOOLTIPTEXT_BASE + 0x01D1)
#define TOOLTIPTEXT_ENG2_ITT_RANKINE                (TOOLTIPTEXT_BASE + 0x01D2)
#define TOOLTIPTEXT_ENG3_ITT_RANKINE                (TOOLTIPTEXT_BASE + 0x01D3)
#define TOOLTIPTEXT_ENG4_ITT_RANKINE                (TOOLTIPTEXT_BASE + 0x01D4)
#define TOOLTIPTEXT_ITT_CELSIUS                     (TOOLTIPTEXT_BASE + 0x01D5)
#define TOOLTIPTEXT_ENG1_ITT_CELSIUS                (TOOLTIPTEXT_BASE + 0x01D6)
#define TOOLTIPTEXT_ENG2_ITT_CELSIUS                (TOOLTIPTEXT_BASE + 0x01D7)
#define TOOLTIPTEXT_ENG3_ITT_CELSIUS                (TOOLTIPTEXT_BASE + 0x01D8)
#define TOOLTIPTEXT_ENG4_ITT_CELSIUS                (TOOLTIPTEXT_BASE + 0x01D9)
#define TOOLTIPTEXT_GAS_GENERATOR_RPM_PERCENT       (TOOLTIPTEXT_BASE + 0x01E0)
#define TOOLTIPTEXT_ENG1_GAS_GENERATOR_RPM_PERCENT  (TOOLTIPTEXT_BASE + 0x01E1)
#define TOOLTIPTEXT_ENG2_GAS_GENERATOR_RPM_PERCENT  (TOOLTIPTEXT_BASE + 0x01E2)
#define TOOLTIPTEXT_ENG3_GAS_GENERATOR_RPM_PERCENT  (TOOLTIPTEXT_BASE + 0x01E3)
#define TOOLTIPTEXT_ENG4_GAS_GENERATOR_RPM_PERCENT  (TOOLTIPTEXT_BASE + 0x01E4)
#define TOOLTIPTEXT_THROTTLE_FUEL_CONDITION_PERCENT (TOOLTIPTEXT_BASE + 0x01E8)
#define TOOLTIPTEXT_ENG1_THROTTLE_FUEL_CONDITION_PERCENT (TOOLTIPTEXT_BASE + 0x01E9)
#define TOOLTIPTEXT_ENG2_THROTTLE_FUEL_CONDITION_PERCENT (TOOLTIPTEXT_BASE + 0x01EA)
#define TOOLTIPTEXT_ENG3_THROTTLE_FUEL_CONDITION_PERCENT (TOOLTIPTEXT_BASE + 0x01EB)
#define TOOLTIPTEXT_ENG4_THROTTLE_FUEL_CONDITION_PERCENT (TOOLTIPTEXT_BASE + 0x01EC)
#define TOOLTIPTEXT_PARKING_BRAKE                   (TOOLTIPTEXT_BASE + 0x01F0)
#define TOOLTIPTEXT_FUEL_CUTOFF                     (TOOLTIPTEXT_BASE + 0x01F8)
#define TOOLTIPTEXT_ENG1_FUEL_CUTOFF                (TOOLTIPTEXT_BASE + 0x01F9)
#define TOOLTIPTEXT_ENG2_FUEL_CUTOFF                (TOOLTIPTEXT_BASE + 0x01FA)
#define TOOLTIPTEXT_ENG3_FUEL_CUTOFF                (TOOLTIPTEXT_BASE + 0x01FB)
#define TOOLTIPTEXT_ENG4_FUEL_CUTOFF                (TOOLTIPTEXT_BASE + 0x01FC)
#define TOOLTIPTEXT_SPEED_BRAKE                     (TOOLTIPTEXT_BASE + 0x0200)
#define TOOLTIPTEXT_HEADING_INDICATOR_HEADING       (TOOLTIPTEXT_BASE + 0x0208)
#define TOOLTIPTEXT_TACHOMETER_RPM                  (TOOLTIPTEXT_BASE + 0x0210)
#define TOOLTIPTEXT_ENG1_TACHOMETER_RPM             (TOOLTIPTEXT_BASE + 0x0211)
#define TOOLTIPTEXT_ENG2_TACHOMETER_RPM             (TOOLTIPTEXT_BASE + 0x0212)
#define TOOLTIPTEXT_ENG3_TACHOMETER_RPM             (TOOLTIPTEXT_BASE + 0x0213)
#define TOOLTIPTEXT_ENG4_TACHOMETER_RPM             (TOOLTIPTEXT_BASE + 0x0214)
#define TOOLTIPTEXT_ON                              (TOOLTIPTEXT_BASE + 0x0220)
#define TOOLTIPTEXT_OFF                             (TOOLTIPTEXT_BASE + 0x0221)
#define TOOLTIPTEXT_AMBIENT_TEMPERATURE_CELSIUS     (TOOLTIPTEXT_BASE + 0x0224)
#define TOOLTIPTEXT_AMBIENT_TEMPERATURE_FARENHEIT   (TOOLTIPTEXT_BASE + 0x0225)
#define TOOLTIPTEXT_ENG1_CARB_TEMPERATURE_CELSIUS   (TOOLTIPTEXT_BASE + 0x0228)
#define TOOLTIPTEXT_ENG2_CARB_TEMPERATURE_CELSIUS   (TOOLTIPTEXT_BASE + 0x0229)
#define TOOLTIPTEXT_CABIN_DOORS                     (TOOLTIPTEXT_BASE + 0x0230)
#define TOOLTIPTEXT_DC3_FUEL_GAUGE                  (TOOLTIPTEXT_BASE + 0x0231)
#define TOOLTIPTEXT_PROP_FEATHER_SWITCH             (TOOLTIPTEXT_BASE + 0x0232)
#define TOOLTIPTEXT_PROP_FEATHER_SWITCH_1           (TOOLTIPTEXT_BASE + 0x0233)
#define TOOLTIPTEXT_PROP_FEATHER_SWITCH_2           (TOOLTIPTEXT_BASE + 0x0234)
#define TOOLTIPTEXT_PROP_FEATHER_SWITCH_3           (TOOLTIPTEXT_BASE + 0x0235)
#define TOOLTIPTEXT_PROP_FEATHER_SWITCH_4           (TOOLTIPTEXT_BASE + 0x0236)
#define TOOLTIPTEXT_MASTER_IGNITION_SWITCH          (TOOLTIPTEXT_BASE + 0x0238)
#define TOOLTIPTEXT_HYDRAULIC_PRESSURE              (TOOLTIPTEXT_BASE + 0x0240)
#define TOOLTIPTEXT_GEAR_HYDRAULIC_PRESSURE         (TOOLTIPTEXT_BASE + 0x0241)
#define TOOLTIPTEXT_COMM1_FREQ                      (TOOLTIPTEXT_BASE + 0x0242)
#define TOOLTIPTEXT_COMM2_FREQ                      (TOOLTIPTEXT_BASE + 0x0243)
#define TOOLTIPTEXT_NAV1_FREQ                       (TOOLTIPTEXT_BASE + 0x0244)
#define TOOLTIPTEXT_NAV2_FREQ                       (TOOLTIPTEXT_BASE + 0x0245)
#define TOOLTIPTEXT_ADF1_FREQ                       (TOOLTIPTEXT_BASE + 0x0246)
#define TOOLTIPTEXT_ADF2_FREQ                       (TOOLTIPTEXT_BASE + 0x0247)
#define TOOLTIPTEXT_ADF1_BAND_SELECTOR              (TOOLTIPTEXT_BASE + 0x0248)
#define TOOLTIPTEXT_ADF2_BAND_SELECTOR              (TOOLTIPTEXT_BASE + 0x0249)
#define TOOLTIPTEXT_ADF1_AUDIO_SELECTOR             (TOOLTIPTEXT_BASE + 0x024A)
#define TOOLTIPTEXT_ADF2_AUDIO_SELECTOR             (TOOLTIPTEXT_BASE + 0x024B)
#define TOOLTIPTEXT_NAV1_AUDIO_SELECTOR             (TOOLTIPTEXT_BASE + 0x024C)
#define TOOLTIPTEXT_NAV2_AUDIO_SELECTOR             (TOOLTIPTEXT_BASE + 0x024D)
#define TOOLTIPTEXT_COM1_AUDIO_SELECTOR             (TOOLTIPTEXT_BASE + 0x024E)
#define TOOLTIPTEXT_COM2_AUDIO_SELECTOR             (TOOLTIPTEXT_BASE + 0x024F)
#define TOOLTIPTEXT_MARKER_AUDIO_SELECTOR           (TOOLTIPTEXT_BASE + 0x0250)
#define TOOLTIPTEXT_ADF_SELECTOR                    (TOOLTIPTEXT_BASE + 0x0251)
#define TOOLTIPTEXT_NAV_SELECTOR                    (TOOLTIPTEXT_BASE + 0x0252)
#define TOOLTIPTEXT_DC3_FLAP_LEVER_EXTEND           (TOOLTIPTEXT_BASE + 0x0253)
#define TOOLTIPTEXT_DC3_FLAP_LEVER_RETRACT          (TOOLTIPTEXT_BASE + 0x0254)
#define TOOLTIPTEXT_TAILWHEEL_LOCK                  (TOOLTIPTEXT_BASE + 0x0255)
#define TOOLTIPTEXT_FORD_TRIMOTOR_FUEL_GAUGE        (TOOLTIPTEXT_BASE + 0x0260)
#define TOOLTIPTEXT_FORD_TRIMOTOR_AMMETER           (TOOLTIPTEXT_BASE + 0x0261)
#define TOOLTIPTEXT_AUTOPILOT_PITCH_REF             (TOOLTIPTEXT_BASE + 0x0264)
#define TOOLTIPTEXT_CENTER_FUEL_PERCENT             (TOOLTIPTEXT_BASE + 0x0265)
#define TOOLTIPTEXT_FUEL_SELECTOR_1                 (TOOLTIPTEXT_BASE + 0x0268)
#define TOOLTIPTEXT_FUEL_SELECTOR_2                 (TOOLTIPTEXT_BASE + 0x0269)
#define TOOLTIPTEXT_FUEL_SELECTOR_3                 (TOOLTIPTEXT_BASE + 0x026A)
#define TOOLTIPTEXT_FUEL_SELECTOR_4                 (TOOLTIPTEXT_BASE + 0x026B)
#define TOOLTIPTEXT_RADIATOR_TEMPERATURE_CELSIUS    (TOOLTIPTEXT_BASE + 0x0270)
#define TOOLTIPTEXT_RADIATOR_TEMPERATURE_FARENHEIT  (TOOLTIPTEXT_BASE + 0x0271)
#define TOOLTIPTEXT_PITCH_TRIM_PCT                  (TOOLTIPTEXT_BASE + 0x0278)
#define TOOLTIPTEXT_INCLINOMETER_BALL               (TOOLTIPTEXT_BASE + 0x0279)
#define TOOLTIPTEXT_MAGNETO_SWITCH                  (TOOLTIPTEXT_BASE + 0x0280)
#define TOOLTIPTEXT_STARTER_SWITCH                  (TOOLTIPTEXT_BASE + 0x0281)
#define TOOLTIPTEXT_TURN_INDICATOR_RATE             (TOOLTIPTEXT_BASE + 0x0282)
#define TOOLTIPTEXT_ENG1_CARB_HEAT                  (TOOLTIPTEXT_BASE + 0x0291)
#define TOOLTIPTEXT_ENG2_CARB_HEAT                  (TOOLTIPTEXT_BASE + 0x0292)
#define TOOLTIPTEXT_ENG3_CARB_HEAT                  (TOOLTIPTEXT_BASE + 0x0293)
#define TOOLTIPTEXT_ENG4_CARB_HEAT                  (TOOLTIPTEXT_BASE + 0x0294)
#define TOOLTIPTEXT_ENG1_MAGNETO_SWITCH             (TOOLTIPTEXT_BASE + 0x02A0)
#define TOOLTIPTEXT_ENG2_MAGNETO_SWITCH             (TOOLTIPTEXT_BASE + 0x02A1)
#define TOOLTIPTEXT_ENG3_MAGNETO_SWITCH             (TOOLTIPTEXT_BASE + 0x02A2)
#define TOOLTIPTEXT_ENG4_MAGNETO_SWITCH             (TOOLTIPTEXT_BASE + 0x02A3)
#define TOOLTIPTEXT_ENG1_PRIMER_SWITCH              (TOOLTIPTEXT_BASE + 0x02A8)
#define TOOLTIPTEXT_ENG2_PRIMER_SWITCH              (TOOLTIPTEXT_BASE + 0x02A9)
#define TOOLTIPTEXT_ENG3_PRIMER_SWITCH              (TOOLTIPTEXT_BASE + 0x02AA)
#define TOOLTIPTEXT_ENG4_PRIMER_SWITCH              (TOOLTIPTEXT_BASE + 0x02AB)
#define TOOLTIPTEXT_ENG1_STARTER_SWITCH             (TOOLTIPTEXT_BASE + 0x02B1)
#define TOOLTIPTEXT_ENG2_STARTER_SWITCH             (TOOLTIPTEXT_BASE + 0x02B2)
#define TOOLTIPTEXT_ENG3_STARTER_SWITCH             (TOOLTIPTEXT_BASE + 0x02B3)
#define TOOLTIPTEXT_ENG4_STARTER_SWITCH             (TOOLTIPTEXT_BASE + 0x02B4)
#define TOOLTIPTEXT_TOGGLE_ELECTRICAL_WINDOW        (TOOLTIPTEXT_BASE + 0x02C0)
#define TOOLTIPTEXT_TOGGLE_ENGINE_WINDOW            (TOOLTIPTEXT_BASE + 0x02C1)
#define TOOLTIPTEXT_TOGGLE_BACK_WINDOW              (TOOLTIPTEXT_BASE + 0x02C2)
#define TOOLTIPTEXT_TOGGLE_IGNITION_CONTROL_WINDOW  (TOOLTIPTEXT_BASE + 0x02C3)
#define TOOLTIPTEXT_TOGGLE_ENGINE_CONTROL_WINDOW    (TOOLTIPTEXT_BASE + 0x02C4)
#define TOOLTIPTEXT_TOGGLE_ENGINE_INSTRUMENTS_WINDOW (TOOLTIPTEXT_BASE + 0x02C5)
#define TOOLTIPTEXT_FORD_TRIMOTOR_PRIMER_SELECT     (TOOLTIPTEXT_BASE + 0x02C8)
#define TOOLTIPTEXT_FORD_TRIMOTOR_PRIMER_PULL       (TOOLTIPTEXT_BASE + 0x02C9)
#define TOOLTIPTEXT_TOGGLE_PIPER_CUB_STARTER_WINDOW (TOOLTIPTEXT_BASE + 0x02CC)
#define TOOLTIPTEXT_DC3_RMI_CARD                    (TOOLTIPTEXT_BASE + 0x02D0)
#define TOOLTIPTEXT_THROTTLE_ENG1_PROPELLER         (TOOLTIPTEXT_BASE + 0x02D8)
#define TOOLTIPTEXT_THROTTLE_ENG2_PROPELLER         (TOOLTIPTEXT_BASE + 0x02D9)
#define TOOLTIPTEXT_THROTTLE_ENG3_PROPELLER         (TOOLTIPTEXT_BASE + 0x02DA)
#define TOOLTIPTEXT_THROTTLE_ENG4_PROPELLER         (TOOLTIPTEXT_BASE + 0x02DB)
#define TOOLTIPTEXT_BATTERY_SWITCH                  (TOOLTIPTEXT_BASE + 0x02E0)
#define TOOLTIPTEXT_FUEL_PUMP                       (TOOLTIPTEXT_BASE + 0x02E8)
#define TOOLTIPTEXT_ENG1_FUEL_PUMP                  (TOOLTIPTEXT_BASE + 0x02E9)
#define TOOLTIPTEXT_ENG2_FUEL_PUMP                  (TOOLTIPTEXT_BASE + 0x02EA)
#define TOOLTIPTEXT_ENG3_FUEL_PUMP                  (TOOLTIPTEXT_BASE + 0x02EB)
#define TOOLTIPTEXT_ENG4_FUEL_PUMP                  (TOOLTIPTEXT_BASE + 0x02EC)
#define TOOLTIPTEXT_ENG1_GENERATOR_SWITCH           (TOOLTIPTEXT_BASE + 0x02F1)
#define TOOLTIPTEXT_ENG2_GENERATOR_SWITCH           (TOOLTIPTEXT_BASE + 0x02F2)
#define TOOLTIPTEXT_ENG3_GENERATOR_SWITCH           (TOOLTIPTEXT_BASE + 0x02F3)
#define TOOLTIPTEXT_ENG4_GENERATOR_SWITCH           (TOOLTIPTEXT_BASE + 0x02F4)
#define TOOLTIPTEXT_LIGHT_SWITCH_WING               (TOOLTIPTEXT_BASE + 0x0300)
#define TOOLTIPTEXT_LIGHT_SWITCH_NAV                (TOOLTIPTEXT_BASE + 0x0301)
#define TOOLTIPTEXT_LIGHT_SWITCH_LANDING            (TOOLTIPTEXT_BASE + 0x0302)
#define TOOLTIPTEXT_LIGHT_SWITCH_PANEL              (TOOLTIPTEXT_BASE + 0x0304)
#define TOOLTIPTEXT_LIGHT_SWITCH_STROBE             (TOOLTIPTEXT_BASE + 0x0305)
#define TOOLTIPTEXT_LIGHT_SWITCH_TAXI               (TOOLTIPTEXT_BASE + 0x0306)
#define TOOLTIPTEXT_LIGHT_SWITCH_BEACON             (TOOLTIPTEXT_BASE + 0x0307)
#define TOOLTIPTEXT_PROP_DEICE_SWITCH               (TOOLTIPTEXT_BASE + 0x0310)
#define TOOLTIPTEXT_AVIONICS_SWITCH                 (TOOLTIPTEXT_BASE + 0x0311)
#define TOOLTIPTEXT_SPIRIT_TOGGLE_ECU_WINDOW        (TOOLTIPTEXT_BASE + 0x0320)
#define TOOLTIPTEXT_SPIRIT_TOGGLE_EIC_WINDOW        (TOOLTIPTEXT_BASE + 0x0321)
#define TOOLTIPTEXT_SPIRIT_INCLINOMETER             (TOOLTIPTEXT_BASE + 0x0322)
#define TOOLTIPTEXT_SPIRIT_PITCH_INDICATOR          (TOOLTIPTEXT_BASE + 0x0323)
#define TOOLTIPTEXT_SPIRIT_PERISCOPE                (TOOLTIPTEXT_BASE + 0x0324)
#define TOOLTIPTEXT_SPIRIT_FUEL_PRESSURE            (TOOLTIPTEXT_BASE + 0x0325)
#define TOOLTIPTEXT_SPIRIT_PIONEER_COMPASS          (TOOLTIPTEXT_BASE + 0x0326)
#define TOOLTIPTEXT_VIMY_FUEL_PRESSURE              (TOOLTIPTEXT_BASE + 0x0328)
#define TOOLTIPTEXT_VIMY_FUEL_PUMP_PORT             (TOOLTIPTEXT_BASE + 0x0329)
#define TOOLTIPTEXT_VIMY_FUEL_PUMP_STARBOARD        (TOOLTIPTEXT_BASE + 0x032A)
#define TOOLTIPTEXT_VIMY_FUEL_VALVE_PORT            (TOOLTIPTEXT_BASE + 0x032B)
#define TOOLTIPTEXT_VIMY_FUEL_VALVE_STARBOARD       (TOOLTIPTEXT_BASE + 0x032C)
#define TOOLTIPTEXT_ALTERNATOR_SWITCH               (TOOLTIPTEXT_BASE + 0x0330)
#define TOOLTIPTEXT_VOLTMETER                       (TOOLTIPTEXT_BASE + 0x0331)
#define TOOLTIPTEXT_HEADING_INDICATOR_HEADING_2     (TOOLTIPTEXT_BASE + 0x0332)
#define TOOLTIPTEXT_ADF1_BEARING_TO_STATION         (TOOLTIPTEXT_BASE + 0x0333)
#define TOOLTIPTEXT_ADF_WINDOW                      (TOOLTIPTEXT_BASE + 0x0334)
#define TOOLTIPTEXT_FUEL_TRANSFER_PUMP              (TOOLTIPTEXT_BASE + 0x0335)
#define TOOLTIPTEXT_VEGA_FUEL_SELECTOR              (TOOLTIPTEXT_BASE + 0x0336)
#define TOOLTIPTEXT_AUTOPILOT_HEADING_REF           (TOOLTIPTEXT_BASE + 0x0337)
#define TOOLTIPTEXT_GENERATOR_SWITCH                (TOOLTIPTEXT_BASE + 0x0338)
#define TOOLTIPTEXT_FUEL_PRESSURE                   (TOOLTIPTEXT_BASE + 0x0339)
#define TOOLTIPTEXT_AIRSPEED_MPH                    (TOOLTIPTEXT_BASE + 0x033A)
#define TOOLTIPTEXT_WRIGHT_FLYER_DISTANCE           (TOOLTIPTEXT_BASE + 0x033B)
#define TOOLTIPTEXT_WRIGHT_FLYER_ELAPSED_TIME       (TOOLTIPTEXT_BASE + 0x033C)
#define TOOLTIPTEXT_ATTITUDE_GYRO_CAGE              (TOOLTIPTEXT_BASE + 0x033D)
#define TOOLTIPTEXT_ATTITUDE_GYRO_CAGE_2            (TOOLTIPTEXT_BASE + 0x033E)
#define TOOLTIPTEXT_ENGINE_PERCENT_RPM              (TOOLTIPTEXT_BASE + 0x033F)
#define TOOLTIPTEXT_ROTOR_PERCENT_RPM               (TOOLTIPTEXT_BASE + 0x0340)
#define TOOLTIPTEXT_TOGGLE_PEDESTAL_WINDOW          (TOOLTIPTEXT_BASE + 0x0341)
#define TOOLTIPTEXT_CARB_TEMPERATURE_CELSIUS        (TOOLTIPTEXT_BASE + 0x0342)
#define TOOLTIPTEXT_CARB_HEAT                       (TOOLTIPTEXT_BASE + 0x0343)
#define TOOLTIPTEXT_R22_MAIN_FUEL_PERCENT           (TOOLTIPTEXT_BASE + 0x0344)
#define TOOLTIPTEXT_R22_AUX_FUEL_PERCENT            (TOOLTIPTEXT_BASE + 0x0345)
#define TOOLTIPTEXT_R22_RT_TRIM                     (TOOLTIPTEXT_BASE + 0x0346)
#define TOOLTIPTEXT_R22_THROTTLE_PERCENT            (TOOLTIPTEXT_BASE + 0x0347)
#define TOOLTIPTEXT_ROTOR_GOVERNOR                  (TOOLTIPTEXT_BASE + 0x0348)
#define TOOLTIPTEXT_ROTOR_GOVERNOR_SWITCH           (TOOLTIPTEXT_BASE + 0x0349)
#define TOOLTIPTEXT_ROTOR_BRAKE                     (TOOLTIPTEXT_BASE + 0x034A)
#define TOOLTIPTEXT_ROTOR_BRAKE_HANDLE              (TOOLTIPTEXT_BASE + 0x034B)
#define TOOLTIPTEXT_ROTOR_CLUTCH                    (TOOLTIPTEXT_BASE + 0x034C)
#define TOOLTIPTEXT_ROTOR_CLUTCH_SWITCH             (TOOLTIPTEXT_BASE + 0x034D)
#define TOOLTIPTEXT_FUEL_VALVE                      (TOOLTIPTEXT_BASE + 0x034E)
#define TOOLTIPTEXT_TOGGLE_FLAPS_WINDOW             (TOOLTIPTEXT_BASE + 0x034F)
#define TOOLTIPTEXT_MANIFOLD_PRESSURE_PSI           (TOOLTIPTEXT_BASE + 0x0350)
#define TOOLTIPTEXT_ENG1_MANIFOLD_PRESSURE_PSI      (TOOLTIPTEXT_BASE + 0x0351)
#define TOOLTIPTEXT_ENG2_MANIFOLD_PRESSURE_PSI      (TOOLTIPTEXT_BASE + 0x0352)
#define TOOLTIPTEXT_ENG3_MANIFOLD_PRESSURE_PSI      (TOOLTIPTEXT_BASE + 0x0353)
#define TOOLTIPTEXT_ENG4_MANIFOLD_PRESSURE_PSI      (TOOLTIPTEXT_BASE + 0x0354)
#define TOOLTIPTEXT_BATTERY_LOAD                    (TOOLTIPTEXT_BASE + 0x0355)
#define TOOLTIPTEXT_SIDE_SLIP                       (TOOLTIPTEXT_BASE + 0x0356)
#define TOOLTIPTEXT_COMET_LEFT_FUEL_SELECTOR        (TOOLTIPTEXT_BASE + 0x0357)
#define TOOLTIPTEXT_COMET_RIGHT_FUEL_SELECTOR       (TOOLTIPTEXT_BASE + 0x0358)
#define TOOLTIPTEXT_COMET_CROSSFEED                 (TOOLTIPTEXT_BASE + 0x0359)
#define TOOLTIPTEXT_COMET_FRONT_TANK_FUEL_QTY       (TOOLTIPTEXT_BASE + 0x035A)
#define TOOLTIPTEXT_COMET_CENTER_TANK_FUEL_QTY      (TOOLTIPTEXT_BASE + 0x035B)
#define TOOLTIPTEXT_DC3_ENG1_FUEL_PRESSURE_LOW      (TOOLTIPTEXT_BASE + 0x035C)
#define TOOLTIPTEXT_DC3_ENG2_FUEL_PRESSURE_LOW      (TOOLTIPTEXT_BASE + 0x035D)
#define TOOLTIPTEXT_DC3_ENG1_OIL_PRESSURE_LOW       (TOOLTIPTEXT_BASE + 0x035E)
#define TOOLTIPTEXT_DC3_ENG2_OIL_PRESSURE_LOW       (TOOLTIPTEXT_BASE + 0x035F)
#define TOOLTIPTEXT_DC3_VOLTAGE_LOW                 (TOOLTIPTEXT_BASE + 0x0360)
#define TOOLTIPTEXT_DC3_VACUUM_LOW                  (TOOLTIPTEXT_BASE + 0x0361)
#define TOOLTIPTEXT_FORD_TRIMOTOR_FUEL_GAUGE_SELECTOR (TOOLTIPTEXT_BASE + 0x0362)
#define TOOLTIPTEXT_DC3_FUEL_GAUGE_SELECTOR         (TOOLTIPTEXT_BASE + 0x0363)
#define TOOLTIPTEXT_DC3_RMI1_SELECTOR               (TOOLTIPTEXT_BASE + 0x0364)
#define TOOLTIPTEXT_DC3_RMI2_SELECTOR               (TOOLTIPTEXT_BASE + 0x0365)
#define TOOLTIPTEXT_DC3_ADF_SIGNAL_STRENGTH         (TOOLTIPTEXT_BASE + 0x0366)
#define TOOLTIPTEXT_LANDING_GEAR_LATCH              (TOOLTIPTEXT_BASE + 0x0367)
#define TOOLTIPTEXT_WRIGHT_FLYER_ANEMOMETER         (TOOLTIPTEXT_BASE + 0x0368)
#define TOOLTIPTEXT_ENG1_RADIATOR_SHUTTER_PERCENT   (TOOLTIPTEXT_BASE + 0x0369)
#define TOOLTIPTEXT_ENG2_RADIATOR_SHUTTER_PERCENT   (TOOLTIPTEXT_BASE + 0x036A)
#define TOOLTIPTEXT_ENG1_GENERATOR_AMPS             (TOOLTIPTEXT_BASE + 0x036B)
#define TOOLTIPTEXT_ENG2_GENERATOR_AMPS             (TOOLTIPTEXT_BASE + 0x036C)
#define TOOLTIPTEXT_TOGGLE_OTHER_WINDOW             (TOOLTIPTEXT_BASE + 0x036D)
#define TOOLTIPTEXT_COLLECTIVE                      (TOOLTIPTEXT_BASE + 0x0370)
#define TOOLTIPTEXT_INERTIAL_SEPERATOR              (TOOLTIPTEXT_BASE + 0x0378)
#define TOOLTIPTEXT_NAV_GPS_SWITCH                  (TOOLTIPTEXT_BASE + 0x0380)
#define TOOLTIPTEXT_MASTER_SWITCH                   (TOOLTIPTEXT_BASE + 0x0384)
#define TOOLTIPTEXT_ENG1_FUEL_PRESSURE              (TOOLTIPTEXT_BASE + 0x0390)
#define TOOLTIPTEXT_ENG2_FUEL_PRESSURE              (TOOLTIPTEXT_BASE + 0x0391)
#define TOOLTIPTEXT_ENG3_FUEL_PRESSURE              (TOOLTIPTEXT_BASE + 0x0392)
#define TOOLTIPTEXT_ENG4_FUEL_PRESSURE              (TOOLTIPTEXT_BASE + 0x0393)
#define TOOLTIPTEXT_AUTOFEATHER_SWITCH              (TOOLTIPTEXT_BASE + 0x0398)
#define TOOLTIPTEXT_GPS_NRST_BUTTON                 (TOOLTIPTEXT_BASE + 0x03A0)
#define TOOLTIPTEXT_GPS_OBS_BUTTON                  (TOOLTIPTEXT_BASE + 0x03A1)
#define TOOLTIPTEXT_GPS_MSG_BUTTON                  (TOOLTIPTEXT_BASE + 0x03A2)
#define TOOLTIPTEXT_GPS_FPL_BUTTON                  (TOOLTIPTEXT_BASE + 0x03A3)
#define TOOLTIPTEXT_GPS_VNAV_BUTTON                 (TOOLTIPTEXT_BASE + 0x03A4)
#define TOOLTIPTEXT_GPS_PROC_BUTTON                 (TOOLTIPTEXT_BASE + 0x03A5)
#define TOOLTIPTEXT_GPS_CRSR_BUTTON                 (TOOLTIPTEXT_BASE + 0x03A6)
#define TOOLTIPTEXT_GPS_GROUP_KNOB                  (TOOLTIPTEXT_BASE + 0x03A7)
#define TOOLTIPTEXT_GPS_GROUP_KNOB_RIGHT            (TOOLTIPTEXT_BASE + 0x03A8)
#define TOOLTIPTEXT_GPS_GROUP_KNOB_LEFT             (TOOLTIPTEXT_BASE + 0x03A9)
#define TOOLTIPTEXT_GPS_PAGE_KNOB                   (TOOLTIPTEXT_BASE + 0x03AA)
#define TOOLTIPTEXT_GPS_PAGE_KNOB_RIGHT             (TOOLTIPTEXT_BASE + 0x03AB)
#define TOOLTIPTEXT_GPS_PAGE_KNOB_LEFT              (TOOLTIPTEXT_BASE + 0x03AC)
#define TOOLTIPTEXT_GPS_RANGE_UP                    (TOOLTIPTEXT_BASE + 0x03AE)
#define TOOLTIPTEXT_GPS_RANGE_DOWN                  (TOOLTIPTEXT_BASE + 0x03AF)
#define TOOLTIPTEXT_GPS_DIRECT_TO_BUTTON            (TOOLTIPTEXT_BASE + 0x03B0)
#define TOOLTIPTEXT_GPS_MENU_BUTTON                 (TOOLTIPTEXT_BASE + 0x03B1)
#define TOOLTIPTEXT_GPS_CLR_BUTTON                  (TOOLTIPTEXT_BASE + 0x03B2)
#define TOOLTIPTEXT_GPS_ENTER_BUTTON                (TOOLTIPTEXT_BASE + 0x03B3)
#define TOOLTIPTEXT_GPS_POWER_BUTTON                (TOOLTIPTEXT_BASE + 0x03B4)
#define TOOLTIPTEXT_GPS_TERR_BUTTON                 (TOOLTIPTEXT_BASE + 0x03B5)
#define TOOLTIPTEXT_GPS_QUIT_BUTTON                 (TOOLTIPTEXT_BASE + 0x03B6)
#define TOOLTIPTEXT_GPS_IN_BUTTON                   (TOOLTIPTEXT_BASE + 0x03B7)
#define TOOLTIPTEXT_GPS_OUT_BUTTON                  (TOOLTIPTEXT_BASE + 0x03B8)
#define TOOLTIPTEXT_GPS_PAD_UP                      (TOOLTIPTEXT_BASE + 0x03B9)
#define TOOLTIPTEXT_GPS_PAD_DOWN                    (TOOLTIPTEXT_BASE + 0x03BA)
#define TOOLTIPTEXT_GPS_PAD_LEFT                    (TOOLTIPTEXT_BASE + 0x03BB)
#define TOOLTIPTEXT_GPS_PAD_RIGHT                   (TOOLTIPTEXT_BASE + 0x03BC)
#define TOOLTIPTEXT_GPS_ROUTE_BUTTON                (TOOLTIPTEXT_BASE + 0x03BD)
#define TOOLTIPTEXT_GPS_DEFAULT_NAV_PAGE            (TOOLTIPTEXT_BASE + 0x03BE)
#define TOOLTIPTEXT_GPS_MAP_PAGE                    (TOOLTIPTEXT_BASE + 0x03BF)
#define TOOLTIPTEXT_GPS_AIRPORT_LOCATION_PAGE       (TOOLTIPTEXT_BASE + 0x03C0)
#define TOOLTIPTEXT_GPS_AIRPORT_RUNWAY_PAGE         (TOOLTIPTEXT_BASE + 0x03C1)
#define TOOLTIPTEXT_GPS_AIRPORT_FREQUENCY_PAGE      (TOOLTIPTEXT_BASE + 0x03C2)
#define TOOLTIPTEXT_GPS_AIRPORT_APPROACH_PAGE       (TOOLTIPTEXT_BASE + 0x03C3)
#define TOOLTIPTEXT_GPS_INTERSECTION_PAGE           (TOOLTIPTEXT_BASE + 0x03C4)
#define TOOLTIPTEXT_GPS_NDB_PAGE                    (TOOLTIPTEXT_BASE + 0x03C5)
#define TOOLTIPTEXT_GPS_VOR_PAGE                    (TOOLTIPTEXT_BASE + 0x03C6)
#define TOOLTIPTEXT_GPS_NEAREST_AIRPORT_PAGE        (TOOLTIPTEXT_BASE + 0x03C7)
#define TOOLTIPTEXT_GPS_NEAREST_INTERSECTION_PAGE   (TOOLTIPTEXT_BASE + 0x03C8)
#define TOOLTIPTEXT_GPS_NEAREST_NDB_PAGE            (TOOLTIPTEXT_BASE + 0x03C9)
#define TOOLTIPTEXT_GPS_NEAREST_VOR_PAGE            (TOOLTIPTEXT_BASE + 0x03CA)
#define TOOLTIPTEXT_GPS_NEAREST_AIRSPACE_PAGE       (TOOLTIPTEXT_BASE + 0x03CB)
#define TOOLTIPTEXT_GPS_FLIGHT_PLAN_PAGE            (TOOLTIPTEXT_BASE + 0x03CC)
#define TOOLTIPTEXT_GPS_PROCEDURES_PAGE             (TOOLTIPTEXT_BASE + 0x03CD)
#define TOOLTIPTEXT_LOW_ROTOR_RPM_ANNUNCIATOR       (TOOLTIPTEXT_BASE + 0x03CE)
#define TOOLTIPTEXT_ALTERNATOR_ANNUNCIATOR          (TOOLTIPTEXT_BASE + 0x03CF)
#define TOOLTIPTEXT_LOW_OIL_PRESSURE_ANNUNCIATOR    (TOOLTIPTEXT_BASE + 0x03D0)
#define TOOLTIPTEXT_LOW_FUEL_LEVEL_ANNUNCIATOR      (TOOLTIPTEXT_BASE + 0x03D1)
#define TOOLTIPTEXT_STARTER_ANNUNCIATOR             (TOOLTIPTEXT_BASE + 0x03D2)
#define TOOLTIPTEXT_MIXTURE_CUTOFF                  (TOOLTIPTEXT_BASE + 0x03D3)
#define TOOLTIPTEXT_ENG1_MIXTURE_CUTOFF             (TOOLTIPTEXT_BASE + 0x03D4)
#define TOOLTIPTEXT_ENG2_MIXTURE_CUTOFF             (TOOLTIPTEXT_BASE + 0x03D5)
#define TOOLTIPTEXT_ENG3_MIXTURE_CUTOFF             (TOOLTIPTEXT_BASE + 0x03D6)
#define TOOLTIPTEXT_ENG4_MIXTURE_CUTOFF             (TOOLTIPTEXT_BASE + 0x03D7)
#define TOOLTIPTEXT_THROTTLE_WRIGHT                 (TOOLTIPTEXT_BASE + 0x03D8)
#define TOOLTIPTEXT_FLAPS_ROCKER                    (TOOLTIPTEXT_BASE + 0x03D9)
#define TOOLTIPTEXT_DC3_LEFT_FUEL_SELECTOR          (TOOLTIPTEXT_BASE + 0x03E0)
#define TOOLTIPTEXT_DC3_RIGHT_FUEL_SELECTOR         (TOOLTIPTEXT_BASE + 0x03E1)
#define TOOLTIPTEXT_LIGHT_SWITCH_RECOGNITION        (TOOLTIPTEXT_BASE + 0x03E2)
#define TOOLTIPTEXT_AUTOBRAKE_SWITCH                (TOOLTIPTEXT_BASE + 0x03E3)
#define TOOLTIPTEXT_EMERGENCY_THROTTLE_PERCENT      (TOOLTIPTEXT_BASE + 0x03E4)
#define TOOLTIPTEXT_BACKUP_AIRSPEED_KNOTS           (TOOLTIPTEXT_BASE + 0x03E5)
#define TOOLTIPTEXT_BACKUP_AIRSPEED_KILOS           (TOOLTIPTEXT_BASE + 0x03E6)
#define TOOLTIPTEXT_BACKUP_ALTIMETER_METERS         (TOOLTIPTEXT_BASE + 0x03E7)
#define TOOLTIPTEXT_BACKUP_ALTIMETER_FEET           (TOOLTIPTEXT_BASE + 0x03E8)
#define TOOLTIPTEXT_BACKUP_ATTITUDE_INDICATOR_BANK_PITCH    (TOOLTIPTEXT_BASE + 0x03E9)
#define TOOLTIPTEXT_ALTERNATE_STATIC                (TOOLTIPTEXT_BASE + 0x03EA)
#define TOOLTIPTEXT_XPNDR_SET_CODE_1200             (TOOLTIPTEXT_BASE + 0x03EB)
#define TOOLTIPTEXT_XPNDR_STORE_STBY_CODE           (TOOLTIPTEXT_BASE + 0x03EC)
#define TOOLTIPTEXT_XPNDR_RECALL_STBY_CODE          (TOOLTIPTEXT_BASE + 0x03ED)
#define TOOLTIPTEXT_XPNDR_IDENT                     (TOOLTIPTEXT_BASE + 0x03EE)
#define TOOLTIPTEXT_XPNDR_REPLY                     (TOOLTIPTEXT_BASE + 0x03EF)
#define TOOLTIPTEXT_XPNDR_REPORTED_FLT_LVL          (TOOLTIPTEXT_BASE + 0x03F0)
#define TOOLTIPTEXT_XPNDR_REPORTED_METERS           (TOOLTIPTEXT_BASE + 0x03F1)
#define TOOLTIPTEXT_NAV_DISPLAY                     (TOOLTIPTEXT_BASE + 0x03F2)
#define TOOLTIPTEXT_NAV_DISPLAY_POWER               (TOOLTIPTEXT_BASE + 0x03F3)
#define TOOLTIPTEXT_NAV_DISPLAY_GO                  (TOOLTIPTEXT_BASE + 0x03F4)
#define TOOLTIPTEXT_NAV_DISPLAY_LEFT                (TOOLTIPTEXT_BASE + 0x03F5)
#define TOOLTIPTEXT_NAV_DISPLAY_UP                  (TOOLTIPTEXT_BASE + 0x03F6)
#define TOOLTIPTEXT_NAV_DISPLAY_DOWN                (TOOLTIPTEXT_BASE + 0x03F7)
#define TOOLTIPTEXT_NAV_DISPLAY_RIGHT               (TOOLTIPTEXT_BASE + 0x03F8)
#define TOOLTIPTEXT_CANOPY                          (TOOLTIPTEXT_BASE + 0x03F9)
#define TOOLTIPTEXT_MULTI_ENG_RPM                   (TOOLTIPTEXT_BASE + 0x03FA)
#define TOOLTIPTEXT_MULTI_ENG_MANIFOLD_PRESSURE_INHG        (TOOLTIPTEXT_BASE + 0x03FB)
#define TOOLTIPTEXT_MULTI_ENG_MANIFOLD_PRESSURE_PASCAL      (TOOLTIPTEXT_BASE + 0x03FC)
#define TOOLTIPTEXT_VHF_NAV1_COURSE                (TOOLTIPTEXT_BASE + 0x03FD)
#define TOOLTIPTEXT_BEAVER_RMI                     (TOOLTIPTEXT_BASE + 0x03FE)
#define TOOLTIPTEXT_BEAVER_FUEL_SELECTOR           (TOOLTIPTEXT_BASE + 0x03FF)
#define TOOLTIPTEXT_LOW_FUEL_PRESSURE_ANNUNCIATOR  (TOOLTIPTEXT_BASE + 0x0400)
#define TOOLTIPTEXT_BEAVER_FRONT_FUEL_GALLONS      (TOOLTIPTEXT_BASE + 0x0401)
#define TOOLTIPTEXT_BEAVER_CENTER_FUEL_GALLONS     (TOOLTIPTEXT_BASE + 0x0402)
#define TOOLTIPTEXT_BEAVER_REAR_FUEL_GALLONS       (TOOLTIPTEXT_BASE + 0x0403)
#define TOOLTIPTEXT_BEAVER_FRONT_FUEL_LITERS       (TOOLTIPTEXT_BASE + 0x0404)
#define TOOLTIPTEXT_BEAVER_CENTER_FUEL_LITERS      (TOOLTIPTEXT_BASE + 0x0405)
#define TOOLTIPTEXT_BEAVER_REAR_FUEL_LITERS        (TOOLTIPTEXT_BASE + 0x0406)
#define TOOLTIPTEXT_FUEL_PRESSURE_KGFSQCM          (TOOLTIPTEXT_BASE + 0x0407)
#define TOOLTIPTEXT_ENG_PRIMER                     (TOOLTIPTEXT_BASE + 0x0408)
#define TOOLTIPTEXT_BEAVER_FLAP_SELECTOR_EXTEND    (TOOLTIPTEXT_BASE + 0x0409)
#define TOOLTIPTEXT_BEAVER_FLAP_SELECTOR_RETRACT   (TOOLTIPTEXT_BASE + 0x040A)
#define TOOLTIPTEXT_BEAVER_FLAP_PUMP               (TOOLTIPTEXT_BASE + 0x040B)
#define TOOLTIPTEXT_ENGINE_ALTERNATE_AIR           (TOOLTIPTEXT_BASE + 0x040C)
#define TOOLTIPTEXT_MAGNETO_SWITCH_OFF             (TOOLTIPTEXT_BASE + 0x040D)
#define TOOLTIPTEXT_MAGNETO_SWITCH_LEFT            (TOOLTIPTEXT_BASE + 0x040E)
#define TOOLTIPTEXT_MAGNETO_SWITCH_RIGHT           (TOOLTIPTEXT_BASE + 0x040F)
#define TOOLTIPTEXT_MAGNETO_SWITCH_BOTH            (TOOLTIPTEXT_BASE + 0x0410)
#define TOOLTIPTEXT_STALL_WARNING                  (TOOLTIPTEXT_BASE + 0x0411)
#define TOOLTIPTEXT_VHF_NAV2_COURSE                (TOOLTIPTEXT_BASE + 0x0412)
#define TOOLTIPTEXT_LEFT_FUEL_WITH_AUX_TANK_GALLONS        (TOOLTIPTEXT_BASE + 0x0413)
#define TOOLTIPTEXT_RIGHT_FUEL_WITH_AUX_TANK_GALLONS       (TOOLTIPTEXT_BASE + 0x0414)
#define TOOLTIPTEXT_LEFT_FUEL_WITH_AUX_TANK_LITERS         (TOOLTIPTEXT_BASE + 0x0415)
#define TOOLTIPTEXT_RIGHT_FUEL_WITH_AUX_TANK_LITERS        (TOOLTIPTEXT_BASE + 0x0416)
#define TOOLTIPTEXT_WATER_BALLAST_DRAIN_VALVE      (TOOLTIPTEXT_BASE + 0x0417)
#define TOOLTIPTEXT_GENERATOR_WARNING_LIGHT        (TOOLTIPTEXT_BASE + 0x0418)
#define TOOLTIPTEXT_OIL_PRESSURE_WARNING_LIGHT     (TOOLTIPTEXT_BASE + 0x0419)
#define TOOLTIPTEXT_FUEL_PRESSURE_WARNING_LIGHT    (TOOLTIPTEXT_BASE + 0x041A)
#define TOOLTIPTEXT_ENGINE_FIRE_WARNING_LIGHT      (TOOLTIPTEXT_BASE + 0x041B)
#define TOOLTIPTEXT_ENG1_FUEL_PRESSURE_KGFSQCM     (TOOLTIPTEXT_BASE + 0x041C)
#define TOOLTIPTEXT_ENG2_FUEL_PRESSURE_KGFSQCM     (TOOLTIPTEXT_BASE + 0x041D)
#define TOOLTIPTEXT_ENG3_FUEL_PRESSURE_KGFSQCM     (TOOLTIPTEXT_BASE + 0x041E)
#define TOOLTIPTEXT_ENG4_FUEL_PRESSURE_KGFSQCM     (TOOLTIPTEXT_BASE + 0x041F)
#define TOOLTIPTEXT_GEAR_SWITCH                    (TOOLTIPTEXT_BASE + 0x0420)
#define TOOLTIPTEXT_CYCLE_NEXT_TARGET              (TOOLTIPTEXT_BASE + 0x0421)
#define TOOLTIPTEXT_CYCLE_PREVIOUS_TARGET          (TOOLTIPTEXT_BASE + 0x0422)
#define TOOLTIPTEXT_DISTANCE_TO_TARGET             (TOOLTIPTEXT_BASE + 0x0423)
#define TOOLTIPTEXT_TARGET_ALTITUDE                (TOOLTIPTEXT_BASE + 0x0424)
#define TOOLTIPTEXT_GAUGE_FIRE_PROTECTION           (TOOLTIPTEXT_BASE + 0x0425)
#define TOOLTIPTEXT_ENG1_FIRE_SWITCH                (TOOLTIPTEXT_BASE + 0x0426)
#define TOOLTIPTEXT_ENG2_FIRE_SWITCH                (TOOLTIPTEXT_BASE + 0x0427)
#define TOOLTIPTEXT_ENG3_FIRE_SWITCH                (TOOLTIPTEXT_BASE + 0x0428)
#define TOOLTIPTEXT_ENG4_FIRE_SWITCH                (TOOLTIPTEXT_BASE + 0x0429)
#define TOOLTIPTEXT_APU_FIRE_SWITCH                 (TOOLTIPTEXT_BASE + 0x042A)
#define TOOLTIPTEXT_APU_SWITCH                      (TOOLTIPTEXT_BASE + 0x042B)
#define TOOLTIPTEXT_APU_FAULT_LIGHT                 (TOOLTIPTEXT_BASE + 0x042C)
#define TOOLTIPTEXT_APU_ONOFF_LIGHT                 (TOOLTIPTEXT_BASE + 0x042D)
#define TOOLTIPTEXT_GAUGE_RADIO_CONTROL_HEAD        (TOOLTIPTEXT_BASE + 0x042E)
#define TOOLTIPTEXT_RADIO_COM1_SELECT_SWITCH        (TOOLTIPTEXT_BASE + 0x042F)
#define TOOLTIPTEXT_RADIO_COM1_SWAP_SWITCH          (TOOLTIPTEXT_BASE + 0x0430)
#define TOOLTIPTEXT_RADIO_COM2_SELECT_SWITCH        (TOOLTIPTEXT_BASE + 0x0431)
#define TOOLTIPTEXT_RADIO_COM2_SWAP_SWITCH          (TOOLTIPTEXT_BASE + 0x0432)
#define TOOLTIPTEXT_RADIO_NAV1_SELECT_SWITCH        (TOOLTIPTEXT_BASE + 0x0433)
#define TOOLTIPTEXT_RADIO_NAV1_SWAP_SWITCH          (TOOLTIPTEXT_BASE + 0x0434)
#define TOOLTIPTEXT_RADIO_NAV2_SELECT_SWITCH        (TOOLTIPTEXT_BASE + 0x0435)
#define TOOLTIPTEXT_RADIO_NAV2_SWAP_SWITCH          (TOOLTIPTEXT_BASE + 0x0436)
#define TOOLTIPTEXT_RADIO_ADF1_SELECT_SWITCH        (TOOLTIPTEXT_BASE + 0x0437)
#define TOOLTIPTEXT_RADIO_ADF2_SELECT_SWITCH        (TOOLTIPTEXT_BASE + 0x0438)
#define TOOLTIPTEXT_RADIO_XPDR_SELECT_SWITCH        (TOOLTIPTEXT_BASE + 0x0439)
#define TOOLTIPTEXT_RADIO_TUNER_KNOB                (TOOLTIPTEXT_BASE + 0x043A)
#define TOOLTIPTEXT_GAUGE_MFD                       (TOOLTIPTEXT_BASE + 0x043B)
#define TOOLTIPTEXT_SEATBELTS_SWITCH                (TOOLTIPTEXT_BASE + 0x043C)
#define TOOLTIPTEXT_NOSMOKING_SWITCH                (TOOLTIPTEXT_BASE + 0x043D)
#define TOOLTIPTEXT_ENG1_IGNITION_SWITCH            (TOOLTIPTEXT_BASE + 0x043E)
#define TOOLTIPTEXT_ENG2_IGNITION_SWITCH            (TOOLTIPTEXT_BASE + 0x043F)
#define TOOLTIPTEXT_GAUGE_TRIM                      (TOOLTIPTEXT_BASE + 0x0440)
#define TOOLTIPTEXT_GAUGE_BACKUP_VORADF             (TOOLTIPTEXT_BASE + 0x0441)
#define TOOLTIPTEXT_GAUGE_BACKUP_ALTASI             (TOOLTIPTEXT_BASE + 0x0442)
#define TOOLTIPTEXT_GAUGE_ECU                       (TOOLTIPTEXT_BASE + 0x0443)
#define TOOLTIPTEXT_GAUGE_EFIS_CONTROL_HEAD         (TOOLTIPTEXT_BASE + 0x0444)
#define TOOLTIPTEXT_GAUGE_OVERHEAD_PANEL            (TOOLTIPTEXT_BASE + 0x0445)
#define TOOLTIPTEXT_LEFT_LDG_LIGHT_RETRACT_SWITCH   (TOOLTIPTEXT_BASE + 0x0446)
#define TOOLTIPTEXT_RIGHT_LDG_LIGHT_RETRACT_SWITCH  (TOOLTIPTEXT_BASE + 0x0447)
#define TOOLTIPTEXT_ENG_ANTI_ICE_SWITCH             (TOOLTIPTEXT_BASE + 0x0448)
#define TOOLTIPTEXT_B737_LE_DEVICES_TEST_SWITCH     (TOOLTIPTEXT_BASE + 0x0449)
#define TOOLTIPTEXT_B737_MASTER_STARTER_ENG1        (TOOLTIPTEXT_BASE + 0x044A)
#define TOOLTIPTEXT_B737_MASTER_STARTER_ENG2        (TOOLTIPTEXT_BASE + 0x044B)
#define TOOLTIPTEXT_B737_DC_VOLTS_SWITCH            (TOOLTIPTEXT_BASE + 0x0450)
#define TOOLTIPTEXT_B737_GEN1_SWITCH                (TOOLTIPTEXT_BASE + 0x0451)
#define TOOLTIPTEXT_B737_GEN2_SWITCH                (TOOLTIPTEXT_BASE + 0x0452)
#define TOOLTIPTEXT_B737_APU1_SWITCH                (TOOLTIPTEXT_BASE + 0x0453)
#define TOOLTIPTEXT_B737_APU2_SWITCH                (TOOLTIPTEXT_BASE + 0x0454)
#define TOOLTIPTEXT_B737_EEC1_SWITCH                (TOOLTIPTEXT_BASE + 0x0455)
#define TOOLTIPTEXT_B737_EEC2_SWITCH                (TOOLTIPTEXT_BASE + 0x0456)
#define TOOLTIPTEXT_B737_HYD_ENG1_SWITCH            (TOOLTIPTEXT_BASE + 0x0457)
#define TOOLTIPTEXT_B737_HYD_ENG2_SWITCH            (TOOLTIPTEXT_BASE + 0x0458)
#define TOOLTIPTEXT_B737_HYD_ELEC_SWITCH            (TOOLTIPTEXT_BASE + 0x0459)
#define TOOLTIPTEXT_B737_ENG1_OFF_FLT_SWITCH        (TOOLTIPTEXT_BASE + 0x045A)
#define TOOLTIPTEXT_B737_ENG2_OFF_FLT_SWITCH        (TOOLTIPTEXT_BASE + 0x045B)
#define TOOLTIPTEXT_B737_ENG_START_SWITCH           (TOOLTIPTEXT_BASE + 0x045C)
#define TOOLTIPTEXT_TEMP_PLACEHOLDER_SYSTEM         (TOOLTIPTEXT_BASE + 0x045D)
#define TOOLTIPTEXT_B737_N1_KNOB                    (TOOLTIPTEXT_BASE + 0x045E)
#define TOOLTIPTEXT_C208_INERTIAL_SEPARATOR         (TOOLTIPTEXT_BASE + 0x045F)
#define TOOLTIPTEXT_C208_VOLT_AMMETER               (TOOLTIPTEXT_BASE + 0x0460)
#define TOOLTIPTEXT_XMISSION_PRESSURE_PSI           (TOOLTIPTEXT_BASE + 0x0461)
#define TOOLTIPTEXT_XMISSION_PRESSURE_KGFSQCM       (TOOLTIPTEXT_BASE + 0x0462)
#define TOOLTIPTEXT_XMISSION_TEMP_F                 (TOOLTIPTEXT_BASE + 0x0463)
#define TOOLTIPTEXT_XMISSION_TEMP_C                 (TOOLTIPTEXT_BASE + 0x0464)
#define TOOLTIPTEXT_FUEL_QUANTITY_GAL               (TOOLTIPTEXT_BASE + 0x0465)
#define TOOLTIPTEXT_FUEL_QUANTITY_L                 (TOOLTIPTEXT_BASE + 0x0466)
#define TOOLTIPTEXT_ENG_ELECTRICAL_LOAD             (TOOLTIPTEXT_BASE + 0x0467)
#define TOOLTIPTEXT_TURBINE_TEMPERATURE_F           (TOOLTIPTEXT_BASE + 0x0468)
#define TOOLTIPTEXT_TURBINE_TEMPERATURE_C           (TOOLTIPTEXT_BASE + 0x0469)
#define TOOLTIPTEXT_TOTAL_AIR_TEMPERATURE_CELSIUS   (TOOLTIPTEXT_BASE + 0x046A)
#define TOOLTIPTEXT_G_FORCE                         (TOOLTIPTEXT_BASE + 0x046B)
#define TOOLTIPTEXT_AUTOPILOT_ENGAGE                (TOOLTIPTEXT_BASE + 0x046C)
#define TOOLTIPTEXT_AUTOPILOT_DISCONNECT            (TOOLTIPTEXT_BASE + 0x046D)
#define TOOLTIPTEXT_TURB_ENG_IGNITION_SWITCH        (TOOLTIPTEXT_BASE + 0x046E)
#define TOOLTIPTEXT_EXTRA_FUEL_GAUGE_QUANTITY      (TOOLTIPTEXT_BASE + 0x046F)
#define TOOLTIPTEXT_DME_AUDIO_SELECTOR             (TOOLTIPTEXT_BASE + 0x0470)
#define TOOLTIPTEXT_A321_ACTIVE_FREQUENCY          (TOOLTIPTEXT_BASE + 0x0471)
#define TOOLTIPTEXT_A321_STBY_FREQUENCY            (TOOLTIPTEXT_BASE + 0x0472)
#define TOOLTIPTEXT_AUTO_LAND                      (TOOLTIPTEXT_BASE + 0x0473)
#define TOOLTIPTEXT_MASTER_WARN                    (TOOLTIPTEXT_BASE + 0x0474)
#define TOOLTIPTEXT_A321_PFD_CHRONO                (TOOLTIPTEXT_BASE + 0x0475)
#define TOOLTIPTEXT_TOGA                           (TOOLTIPTEXT_BASE + 0x0476)
#define TOOLTIPTEXT_A321_MASTER_STARTER_ENG1       (TOOLTIPTEXT_BASE + 0x0477)
#define TOOLTIPTEXT_A321_MASTER_STARTER_ENG2       (TOOLTIPTEXT_BASE + 0x0478)
#define TOOLTIPTEXT_A321_IGNITION_MODE             (TOOLTIPTEXT_BASE + 0x0479)
#define TOOLTIPTEXT_A321_INERTIAL_DATA_KNOB        (TOOLTIPTEXT_BASE + 0x047A)
#define TOOLTIPTEXT_A321_INERTIAL_DATA_SWITCH      (TOOLTIPTEXT_BASE + 0x047B)
#define TOOLTIPTEXT_A321_STD_BARO_PULL             (TOOLTIPTEXT_BASE + 0x047C)
#define TOOLTIPTEXT_SET_STD_BARO                   (TOOLTIPTEXT_BASE + 0x047D)
#define TOOLTIPTEXT_EMERGENCY_GEAR_HANDLE          (TOOLTIPTEXT_BASE + 0x047E)
#define TOOLTIPTEXT_GAUGE_EFIS                     (TOOLTIPTEXT_BASE + 0x047F)
#define TOOLTIPTEXT_GAUGE_ECAMS                    (TOOLTIPTEXT_BASE + 0x0480)
#define TOOLTIPTEXT_BACKUP_PFD                     (TOOLTIPTEXT_BASE + 0x0481)
#define TOOLTIPTEXT_CRJ_FLAP_LEVER                 (TOOLTIPTEXT_BASE + 0x0482)
#define TOOLTIPTEXT_A321_ELAC1                     (TOOLTIPTEXT_BASE + 0x0483)
#define TOOLTIPTEXT_A321_ELAC2                     (TOOLTIPTEXT_BASE + 0x0484)
#define TOOLTIPTEXT_A321_SEC1                      (TOOLTIPTEXT_BASE + 0x0485)
#define TOOLTIPTEXT_A321_SEC2                      (TOOLTIPTEXT_BASE + 0x0486)
#define TOOLTIPTEXT_A321_SEC3                      (TOOLTIPTEXT_BASE + 0x0487)
#define TOOLTIPTEXT_A321_FAC1                      (TOOLTIPTEXT_BASE + 0x0488)
#define TOOLTIPTEXT_A321_FAC2                      (TOOLTIPTEXT_BASE + 0x0489)
#define TOOLTIPTEXT_APU_VOLTS                      (TOOLTIPTEXT_BASE + 0x048A)
#define TOOLTIPTEXT_TURB_ENG2_IGNITION_SWITCH      (TOOLTIPTEXT_BASE + 0x048B)
#define TOOLTIPTEXT_LEAR45_POINTER1_SOURCE_SELECT_SWITCH  (TOOLTIPTEXT_BASE + 0x048C)
#define TOOLTIPTEXT_LEAR45_POINTER2_SOURCE_SELECT_SWITCH  (TOOLTIPTEXT_BASE + 0x048D)
#define TOOLTIPTEXT_CRJ_HPA_IN_BUTTON               (TOOLTIPTEXT_BASE + 0x048E)
#define TOOLTIPTEXT_CRJ_BRG1_BUTTON                 (TOOLTIPTEXT_BASE + 0x048F)
#define TOOLTIPTEXT_CRJ_BRG2_BUTTON                 (TOOLTIPTEXT_BASE + 0x0490)
#define TOOLTIPTEXT_CRJ_MFD_FORMAT                  (TOOLTIPTEXT_BASE + 0x0491)
#define TOOLTIPTEXT_CRJ_MAP_RANGE                   (TOOLTIPTEXT_BASE + 0x0492)
#define TOOLTIPTEXT_CRJ_HALF_BANK                   (TOOLTIPTEXT_BASE + 0x0493)
#define TOOLTIPTEXT_DISPLAY_CONTROL_PANEL           (TOOLTIPTEXT_BASE + 0x0494)
#define TOOLTIPTEXT_CRJ_BLEED_AIR_SOURCE            (TOOLTIPTEXT_BASE + 0x0495)
#define TOOLTIPTEXT_COURSE_SELECT                   (TOOLTIPTEXT_BASE + 0x0496)
#define TOOLTIPTEXT_COM_RECEIVE_ALL                 (TOOLTIPTEXT_BASE + 0x0497)
#define TOOLTIPTEXT_AUTOPILOT_MODE_ANNUNCIATOR      (TOOLTIPTEXT_BASE + 0x0498)
#define TOOLTIPTEXT_RMI_POINTER1                    (TOOLTIPTEXT_BASE + 0x0499)
#define TOOLTIPTEXT_RMI_POINTER2                    (TOOLTIPTEXT_BASE + 0x049A)
#define TOOLTIPTEXT_FIRE_WARNING_LIGHT              (TOOLTIPTEXT_BASE + 0x049B)
#define TOOLTIPTEXT_B737_EFIS_DISPLAY_MODE          (TOOLTIPTEXT_BASE + 0x049C)
#define TOOLTIPTEXT_B737_EFIS_DISPLAY_SCALE         (TOOLTIPTEXT_BASE + 0x049D)
#define TOOLTIPTEXT_B737_MFD_CENTERED               (TOOLTIPTEXT_BASE + 0x049E)
#define TOOLTIPTEXT_B737_POINTER1                   (TOOLTIPTEXT_BASE + 0x049F)
#define TOOLTIPTEXT_B737_POINTER2                   (TOOLTIPTEXT_BASE + 0x04A0)
#define TOOLTIPTEXT_B737_EFIS_SHOW_WAYPOINTS        (TOOLTIPTEXT_BASE + 0x04A1)
#define TOOLTIPTEXT_B737_EFIS_SHOW_AIRPORTS         (TOOLTIPTEXT_BASE + 0x04A2)
#define TOOLTIPTEXT_B737_EFIS_SHOW_NDB              (TOOLTIPTEXT_BASE + 0x04A3)
#define TOOLTIPTEXT_B737_EFIS_SHOW_VOR              (TOOLTIPTEXT_BASE + 0x04A4)
#define TOOLTIPTEXT_B737_AUTOPILOT_BANK_LIMIT       (TOOLTIPTEXT_BASE + 0x04A5)
#define TOOLTIPTEXT_NAV_FREQUENCY_SWAP              (TOOLTIPTEXT_BASE + 0x04A6)
#define TOOLTIPTEXT_COM_FREQUENCY_SWAP              (TOOLTIPTEXT_BASE + 0x04A7)
#define TOOLTIPTEXT_ALTITUDE_BUG_ADJUST             (TOOLTIPTEXT_BASE + 0x04A8)
#define TOOLTIPTEXT_COURSE_ADJUST                   (TOOLTIPTEXT_BASE + 0x04A9)
#define TOOLTIPTEXT_G1000_SOFTKEYS                  (TOOLTIPTEXT_BASE + 0x04AA)
#define TOOLTIPTEXT_VERTICAL_SPEED_KTS              (TOOLTIPTEXT_BASE + 0x04AB)
#define TOOLTIPTEXT_TOW_RELEASE                     (TOOLTIPTEXT_BASE + 0x04AC)
#define TOOLTIPTEXT_DG808S_VARIO_SWITCH             (TOOLTIPTEXT_BASE + 0x04AD)
#define TOOLTIPTEXT_DG808S_GYRO_SWITCH              (TOOLTIPTEXT_BASE + 0x04AE)
#define TOOLTIPTEXT_DG808S_VARIOMETER               (TOOLTIPTEXT_BASE + 0x04AF)
#define TOOLTIPTEXT_ANTISKID                        (TOOLTIPTEXT_BASE + 0x04B0)
#define TOOLTIPTEXT_B737_BACKUP_VOR_POINTER1        (TOOLTIPTEXT_BASE + 0x04B1)
#define TOOLTIPTEXT_B737_BACKUP_VOR_POINTER2        (TOOLTIPTEXT_BASE + 0x04B2)
#define TOOLTIPTEXT_B737_CLOCK_CHR                  (TOOLTIPTEXT_BASE + 0x04B3)
#define TOOLTIPTEXT_B737_CLOCK_DAT                  (TOOLTIPTEXT_BASE + 0x04B4)
#define TOOLTIPTEXT_B737_CLOCK_RUN_HLD              (TOOLTIPTEXT_BASE + 0x04B5)
#define TOOLTIPTEXT_B737_CLOCK_RST                  (TOOLTIPTEXT_BASE + 0x04B6)
#define TOOLTIPTEXT_B737_BACKUP_ATTITUDE_ILS_KNOB   (TOOLTIPTEXT_BASE + 0x04B7)
#define TOOLTIPTEXT_B737_BACKUP_ATTITUDE_CAL_KNOB   (TOOLTIPTEXT_BASE + 0x04B8)


#define TOOLTIPTEXT_MANIFOLD_PRESSURE_KPASCAL           (TOOLTIPTEXT_BASE + 0x04B9)   
#define TOOLTIPTEXT_ENG1_MANIFOLD_PRESSURE_KPASCAL      (TOOLTIPTEXT_BASE + 0x04BA)
#define TOOLTIPTEXT_ENG2_MANIFOLD_PRESSURE_KPASCAL      (TOOLTIPTEXT_BASE + 0x04BB)
#define TOOLTIPTEXT_ENG3_MANIFOLD_PRESSURE_KPASCAL      (TOOLTIPTEXT_BASE + 0x04BC)
#define TOOLTIPTEXT_ENG4_MANIFOLD_PRESSURE_KPASCAL      (TOOLTIPTEXT_BASE + 0x04BD)
#define TOOLTIPTEXT_MULTI_ENG_MANIFOLD_PRESSURE_KPASCAL (TOOLTIPTEXT_BASE + 0x04BE)
#define TOOLTIPTEXT_ENGINE_HEALTH                   (TOOLTIPTEXT_BASE + 0x04BF)        
#define TOOLTIPTEXT_ANTIDETONATION_FLUID_LEVEL      (TOOLTIPTEXT_BASE + 0x04C0)
#define TOOLTIPTEXT_NITROUS_FLUID_LEVEL             (TOOLTIPTEXT_BASE + 0x04C1)
#define HELPID_CURRENT_LAP		                    (TOOLTIPTEXT_BASE + 0x04C2)
#define HELPID_LAST_LAP			                    (TOOLTIPTEXT_BASE + 0x04C3) 
#define HELPID_AHEAD_BY	                            (TOOLTIPTEXT_BASE + 0x04C4)
#define HELPID_BEHIND_BY	                        (TOOLTIPTEXT_BASE + 0x04C5)
#define HELPID_TOTAL_PENALTIES	                    (TOOLTIPTEXT_BASE + 0x04C6)
#define HELPID_RACER_PLACE                          (TOOLTIPTEXT_BASE + 0x04C7)
#define HELPID_RACER_LAP                            (TOOLTIPTEXT_BASE + 0x04C8)
#define HELPID_TOTAL_TIME                           (TOOLTIPTEXT_BASE + 0x04C9)
#define TOOLTIPTEXT_RACE_FLAG_DISQUALIFIED          (TOOLTIPTEXT_BASE + 0x04CA)
#define TOOLTIPTEXT_RACE_FLAG_LAST_LAP              (TOOLTIPTEXT_BASE + 0x04CB)
#define TOOLTIPTEXT_RACE_FLAG_RACE_COMPLETE         (TOOLTIPTEXT_BASE + 0x04CC)
#define TOOLTIPTEXT_RACE_FLAG_RACING                (TOOLTIPTEXT_BASE + 0x04CD)
#define TOOLTIPTEXT_BATTERY_VOLTAGE                 (TOOLTIPTEXT_BASE + 0x04CE)

#define TOOLTIPTEXT_HUD							    (TOOLTIPTEXT_BASE + 0x04CF)
#define TOOLTIPTEXT_AOA_INDEXER_LIGHTS			    (TOOLTIPTEXT_BASE + 0x04D0)
#define TOOLTIPTEXT_DDI 						    (TOOLTIPTEXT_BASE + 0x04D1)   
#define TOOLTIPTEXT_BRIGHTNESS					    (TOOLTIPTEXT_BASE + 0x04D2)
#define TOOLTIPTEXT_CONTRAST                        (TOOLTIPTEXT_BASE + 0x04D3)
#define TOOLTIPTEXT_DISPLAY_PRESETS	                (TOOLTIPTEXT_BASE + 0x04D4)
#define TOOLTIPTEXT_MANUAL_TUNE					    (TOOLTIPTEXT_BASE + 0x04D5)
#define TOOLTIPTEXT_NOZZLE_POSITION                 (TOOLTIPTEXT_BASE + 0x04D6)
#define TOOLTIPTEXT_TOTAL_FUEL_POUNDS               (TOOLTIPTEXT_BASE + 0x04D7)
#define TOOLTIPTEXT_FUEL_QUANTITY_SELECTOR		    (TOOLTIPTEXT_BASE + 0x04D8)
#define TOOLTIPTEXT_MAP_MODE                        (TOOLTIPTEXT_BASE + 0x04D9)
#define TOOLTIPTEXT_LAUNCHBAR_SWITCH                (TOOLTIPTEXT_BASE + 0x04DA)
#define TOOLTIPTEXT_PRESSURIZATION_CABIN_ALTITUDE   (TOOLTIPTEXT_BASE + 0x04DB)
#define TOOLTIPTEXT_TAILHOOK_HANDLE                 (TOOLTIPTEXT_BASE + 0x04DC)
#define TOOLTIPTEXT_FOLDING_WING_HANDLE			    (TOOLTIPTEXT_BASE + 0x04DD)

#define TOOLTIPTEXT_STEER_ANGLE	                    (TOOLTIPTEXT_BASE + 0x04DE)					
#define TOOLTIPTEXT_ANNUNCIATOR_CHIP_DETECTED       (TOOLTIPTEXT_BASE + 0x04DF)
#define TOOLTIPTEXT_ALTITUDE_MODE                   (TOOLTIPTEXT_BASE + 0x04E0) 					
#define TOOLTIPTEXT_HSI_PAGE_SELECT					(TOOLTIPTEXT_BASE + 0x04E1)
#define TOOLTIPTEXT_MAP_PAGE_SELECT					(TOOLTIPTEXT_BASE + 0x04E2)
#define TOOLTIPTEXT_MAP_RANGE_INC					(TOOLTIPTEXT_BASE + 0x04E3)
#define TOOLTIPTEXT_MAP_RANGE_DEC					(TOOLTIPTEXT_BASE + 0x04E4)
#define TOOLTIPTEXT_PRIMARY_PAGE_SELECT             (TOOLTIPTEXT_BASE + 0x04E5)
#define TOOLTIPTEXT_SECONDARY_PAGE_SELECT           (TOOLTIPTEXT_BASE + 0x04E6)
#define TOOLTIPTEXT_FUEL_PAGE_SELECT                (TOOLTIPTEXT_BASE + 0x04E7)
#define TOOLTIPTEXT_ELECTRICAL_PAGE_SELECT          (TOOLTIPTEXT_BASE + 0x04E8)
#define TOOLTIPTEXT_SWITCH_MUTE_RADIOS              (TOOLTIPTEXT_BASE + 0x04E9)
#define TOOLTIPTEXT_SELECT_MAGNETIC_OR_TRUE_HEADING (TOOLTIPTEXT_BASE + 0x04EA)
#define TOOLTIPTEXT_HOIST_RAISE_LOWER				(TOOLTIPTEXT_BASE + 0x04EB)
#define TOOLTIPTEXT_RAMP_RAISE_LOWER				(TOOLTIPTEXT_BASE + 0x04EC)
#define TOOLTIPTEXT_SLING_PICKUP_RELEASE			(TOOLTIPTEXT_BASE + 0x04ED)
#define TOOLTIPTEXT_TUNE_COM_FRACTION				(TOOLTIPTEXT_BASE + 0x04EF)
#define TOOLTIPTEXT_TUNE_COM_WHOLE					(TOOLTIPTEXT_BASE + 0x04F0) 
#define TOOLTIPTEXT_TUNE_COM_1_2					(TOOLTIPTEXT_BASE + 0x04F1)
#define TOOLTIPTEXT_TURB_ENG3_IGNITION_SWITCH       (TOOLTIPTEXT_BASE + 0x04F2)
#define TOOLTIPTEXT_FIRE_BOTTLE_DISCHARGED_1        (TOOLTIPTEXT_BASE + 0x04F3)
#define TOOLTIPTEXT_FIRE_BOTTLE_DISCHARGED_2        (TOOLTIPTEXT_BASE + 0x04F4)
#define TOOLTIPTEXT_FIRE_BOTTLE_DISCHARGED_3        (TOOLTIPTEXT_BASE + 0x04F5)
#define TOOLTIPTEXT_FIRE_BOTTLE_DISCHARGED_4        (TOOLTIPTEXT_BASE + 0x04F6)
#define TOOLTIPTEXT_APU_GENERATOR_SWITCH			(TOOLTIPTEXT_BASE + 0x04F7)
#define TOOLTIPTEXT_EXIT_DOOR_OPEN_1				(TOOLTIPTEXT_BASE + 0x04F8)
#define TOOLTIPTEXT_EXIT_DOOR_OPEN_2				(TOOLTIPTEXT_BASE + 0x04F9)
#define TOOLTIPTEXT_ALTIMETER_KOHLSMAN_INHG_OR_MBAR (TOOLTIPTEXT_BASE + 0x04FA)
#define TOOLTIPTEXT_RADIO_ALTIMETER_DH_FEET_OR_METERS (TOOLTIPTEXT_BASE + 0x04FB) 


//
// Key Accelerator Event IDs
//

#define KEY_ID_MIN                      0x00010000

#define KEY_NULL                        (KEY_ID_MIN + 0)
#define KEY_DEMO_STOP                   (KEY_ID_MIN + 1)
#define KEY_REPLAY_STOP                 (KEY_ID_MIN + 1)
#define KEY_SELECT_1                    (KEY_ID_MIN + 2)
#define KEY_SELECT_2                    (KEY_ID_MIN + 3)
#define KEY_SELECT_3                    (KEY_ID_MIN + 4)
#define KEY_SELECT_4                    (KEY_ID_MIN + 5)
#define KEY_DEMO_RECORD_1_SEC           (KEY_ID_MIN + 7)
#define KEY_DEMO_RECORD_5_SEC           (KEY_ID_MIN + 8)
#define KEY_MINUS                       (KEY_ID_MIN + 12)
#define KEY_PLUS                        (KEY_ID_MIN + 13)
#define KEY_ZOOM_1X                     (KEY_ID_MIN + 14)
#define KEY_PANEL_SELECT_1              (KEY_ID_MIN + 15)
#define KEY_SOUND_TOGGLE                (KEY_ID_MIN + 16)
#define KEY_FULL_WINDOW_TOGGLE          (KEY_ID_MIN + 17)
#define KEY_ENGINE                      (KEY_ID_MIN + 18)
#define KEY_SIM_RATE                    (KEY_ID_MIN + 19)
#define KEY_XPNDR                       (KEY_ID_MIN + 20)
#define KEY_SLEW_TOGGLE                 (KEY_ID_MIN + 21)
#define KEY_EGT                         (KEY_ID_MIN + 22)
#define KEY_SMOKE_TOGGLE                (KEY_ID_MIN + 23)
#define KEY_STROBES_TOGGLE              (KEY_ID_MIN + 24)
#define KEY_PAUSE_TOGGLE                (KEY_ID_MIN + 25)
#define KEY_REFRESH_SCENERY             (KEY_ID_MIN + 26)
#define KEY_ATC                         (KEY_ID_MIN + 28)
#define KEY_ADF                         (KEY_ID_MIN + 30)
#define KEY_VIEW_MODE                   (KEY_ID_MIN + 31)
#define KEY_HEADING_GYRO_SET            (KEY_ID_MIN + 32)
#define KEY_DME                         (KEY_ID_MIN + 33)
#define KEY_GEAR_TOGGLE                 (KEY_ID_MIN + 34)
#define KEY_ANTI_ICE_TOGGLE             (KEY_ID_MIN + 35)
#define KEY_JET_STARTER                 (KEY_ID_MIN + 36)
#define KEY_JOYSTICK_CALIBRATE          (KEY_ID_MIN + 37)
#define KEY_ALL_LIGHTS_TOGGLE           (KEY_ID_MIN + 38)
#define KEY_SITUATION_SAVE              (KEY_ID_MIN + 39)
#define KEY_VIEW_WINDOW_TO_FRONT        (KEY_ID_MIN + 40)
#define KEY_DEMO_RECORD_STOP            (KEY_ID_MIN + 43)
#define KEY_ANALYSIS_MANEUVER_STOP      (KEY_ID_MIN + 43)
#define KEY_AP_MASTER                   (KEY_ID_MIN + 44)
#define KEY_FREQUENCY_SWAP              (KEY_ID_MIN + 45)
#define KEY_COM_RADIO                   (KEY_ID_MIN + 46)
#define KEY_VOR_OBS                     (KEY_ID_MIN + 47)
#define KEY_BAROMETRIC                  (KEY_ID_MIN + 48)
#define KEY_NAV_RADIO                   (KEY_ID_MIN + 49)
#define KEY_MAGNETO                     (KEY_ID_MIN + 50)
#define KEY_DEMO_RECORD_MESSAGE         (KEY_ID_MIN + 51)
#define KEY_BRAKES                      (KEY_ID_MIN + 52)
#define KEY_SPOILERS_TOGGLE             (KEY_ID_MIN + 53)
#define KEY_SITUATION_RESET             (KEY_ID_MIN + 55)
#define KEY_FLAPS_UP                    (KEY_ID_MIN + 59)
#define KEY_THROTTLE_FULL               (KEY_ID_MIN + 60)
#define KEY_FLAPS_1                     (KEY_ID_MIN + 61)
#define KEY_THROTTLE_INCR               (KEY_ID_MIN + 62)
#define KEY_FLAPS_2                     (KEY_ID_MIN + 63)
#define KEY_THROTTLE_INCR_SMALL         (KEY_ID_MIN + 64)
#define KEY_FLAPS_3                     (KEY_ID_MIN + 65)
#define KEY_THROTTLE_DECR               (KEY_ID_MIN + 66)
#define KEY_FLAPS_4                     (KEY_ID_MIN + 67)
#define KEY_FLAPS_DOWN                  (KEY_ID_MIN + 67)
#define KEY_THROTTLE_CUT                (KEY_ID_MIN + 68)
#define KEY_VIEW                        (KEY_ID_MIN + 70)
#define KEY_ELEV_TRIM_DN                (KEY_ID_MIN + 71)
#define KEY_ELEV_DOWN                   (KEY_ID_MIN + 72)
#define KEY_INCREASE_THROTTLE           (KEY_ID_MIN + 73)
#define KEY_AILERONS_LEFT               (KEY_ID_MIN + 75)
#define KEY_CENTER_AILER_RUDDER         (KEY_ID_MIN + 76)
#define KEY_AILERONS_RIGHT              (KEY_ID_MIN + 77)
#define KEY_ELEV_TRIM_UP                (KEY_ID_MIN + 79)
#define KEY_ELEV_UP                     (KEY_ID_MIN + 80)
#define KEY_DECREASE_THROTTLE           (KEY_ID_MIN + 81)
#define KEY_MOUSE_AS_YOKE_TOGGLE        (KEY_ID_MIN + 83)
#define KEY_SLEW_ALTIT_UP_FAST          (KEY_ID_MIN + 84)
#define KEY_SLEW_ALTIT_UP_SLOW          (KEY_ID_MIN + 85)
#define KEY_SLEW_ALTIT_FREEZE           (KEY_ID_MIN + 86)
#define KEY_SLEW_ALTIT_DN_SLOW          (KEY_ID_MIN + 87)
#define KEY_SLEW_ALTIT_DN_FAST          (KEY_ID_MIN + 88)
#define KEY_SLEW_ALTIT_PLUS             (KEY_ID_MIN + 89)
#define KEY_SLEW_ALTIT_MINUS            (KEY_ID_MIN + 90)
#define KEY_SLEW_PITCH_DN_FAST          (KEY_ID_MIN + 91)
#define KEY_SLEW_PITCH_DN_SLOW          (KEY_ID_MIN + 92)
#define KEY_SLEW_PITCH_FREEZE           (KEY_ID_MIN + 93)
#define KEY_SLEW_PITCH_UP_SLOW          (KEY_ID_MIN + 94)
#define KEY_SLEW_PITCH_UP_FAST          (KEY_ID_MIN + 95)
#define KEY_SLEW_PITCH_PLUS             (KEY_ID_MIN + 96)
#define KEY_SLEW_PITCH_MINUS            (KEY_ID_MIN + 97)
#define KEY_MAGNETO_DECR                (KEY_ID_MIN + 98)
#define KEY_MAGNETO_INCR                (KEY_ID_MIN + 99)
#define KEY_COM_RADIO_WHOLE_DEC         (KEY_ID_MIN + 100)
#define KEY_COM_RADIO_WHOLE_INC         (KEY_ID_MIN + 101)
#define KEY_COM_RADIO_FRACT_DEC         (KEY_ID_MIN + 102)
#define KEY_COM_RADIO_FRACT_INC         (KEY_ID_MIN + 103)
#define KEY_NAV1_RADIO_WHOLE_DEC        (KEY_ID_MIN + 104)
#define KEY_NAV1_RADIO_WHOLE_INC        (KEY_ID_MIN + 105)
#define KEY_NAV1_RADIO_FRACT_DEC        (KEY_ID_MIN + 106)
#define KEY_NAV1_RADIO_FRACT_INC        (KEY_ID_MIN + 107)
#define KEY_NAV2_RADIO_WHOLE_DEC        (KEY_ID_MIN + 108)
#define KEY_NAV2_RADIO_WHOLE_INC        (KEY_ID_MIN + 109)
#define KEY_NAV2_RADIO_FRACT_DEC        (KEY_ID_MIN + 110)
#define KEY_NAV2_RADIO_FRACT_INC        (KEY_ID_MIN + 111)
#define KEY_ADF_100_INC                 (KEY_ID_MIN + 112)
#define KEY_ADF_10_INC                  (KEY_ID_MIN + 113)
#define KEY_ADF_1_INC                   (KEY_ID_MIN + 114)
#define KEY_XPNDR_1000_INC              (KEY_ID_MIN + 115)
#define KEY_XPNDR_100_INC               (KEY_ID_MIN + 116)
#define KEY_XPNDR_10_INC                (KEY_ID_MIN + 117)
#define KEY_XPNDR_1_INC                 (KEY_ID_MIN + 118)
#define KEY_ZOOM_IN                     (KEY_ID_MIN + 119)
#define KEY_ZOOM_OUT                    (KEY_ID_MIN + 120)
#define KEY_CLOCK_HOURS_DEC             (KEY_ID_MIN + 121)
#define KEY_CLOCK_HOURS_INC             (KEY_ID_MIN + 122)
#define KEY_CLOCK_MINUTES_DEC           (KEY_ID_MIN + 123)
#define KEY_CLOCK_MINUTES_INC           (KEY_ID_MIN + 124)
#define KEY_CLOCK_SECONDS_ZERO          (KEY_ID_MIN + 125)
#define KEY_VOR1_OBI_DEC                (KEY_ID_MIN + 126)
#define KEY_VOR1_OBI_INC                (KEY_ID_MIN + 127)
#define KEY_VOR2_OBI_DEC                (KEY_ID_MIN + 128)
#define KEY_VOR2_OBI_INC                (KEY_ID_MIN + 129)
#define KEY_ADF_100_DEC                 (KEY_ID_MIN + 130)
#define KEY_ADF_10_DEC                  (KEY_ID_MIN + 131)
#define KEY_ADF_1_DEC                   (KEY_ID_MIN + 132)
#define KEY_AP_MASTER_ALT               (KEY_ID_MIN + 133)
#define KEY_MAP_ZOOM_FINE_IN            (KEY_ID_MIN + 134)
#define KEY_PAN_LEFT                    (KEY_ID_MIN + 135)
#define KEY_PAN_RIGHT                   (KEY_ID_MIN + 136)
#define KEY_MAP_ZOOM_FINE_OUT           (KEY_ID_MIN + 137)
#define KEY_VIEW_FORWARD                (KEY_ID_MIN + 138)
#define KEY_VIEW_FORWARD_RIGHT          (KEY_ID_MIN + 139)
#define KEY_VIEW_RIGHT                  (KEY_ID_MIN + 140)
#define KEY_VIEW_REAR_RIGHT             (KEY_ID_MIN + 141)
#define KEY_VIEW_REAR                   (KEY_ID_MIN + 142)
#define KEY_VIEW_REAR_LEFT              (KEY_ID_MIN + 143)
#define KEY_VIEW_LEFT                   (KEY_ID_MIN + 144)
#define KEY_VIEW_FORWARD_LEFT           (KEY_ID_MIN + 145)
#define KEY_VIEW_DOWN                   (KEY_ID_MIN + 146)
#define KEY_ELEVATOR_DOWN               (KEY_ID_MIN + 147)
#define KEY_ELEVATOR_UP                 (KEY_ID_MIN + 148)
#define KEY_AILERON_LEFT                (KEY_ID_MIN + 149)
#define KEY_AILERON_CENTER              (KEY_ID_MIN + 150)
#define KEY_AILERON_RIGHT               (KEY_ID_MIN + 151)
#define KEY_RUDDER_LEFT                 (KEY_ID_MIN + 152)
#define KEY_RUDDER_CENTER               (KEY_ID_MIN + 153)
#define KEY_RUDDER_RIGHT                (KEY_ID_MIN + 154)
#define KEY_VIEW1_MODE_SET              (KEY_ID_MIN + 155)
#define KEY_SOUND_SET                   (KEY_ID_MIN + 156)
#define KEY_VIEW1_DIRECTION_SET         (KEY_ID_MIN + 157)
#define KEY_ELEVATOR_SET                (KEY_ID_MIN + 158)
#define KEY_AILERON_SET                 (KEY_ID_MIN + 159)
#define KEY_RUDDER_SET                  (KEY_ID_MIN + 160)
#define KEY_THROTTLE_SET                (KEY_ID_MIN + 161)
#define KEY_FLAPS_SET                   (KEY_ID_MIN + 162)
#define KEY_GEAR_SET                    (KEY_ID_MIN + 164)
#define KEY_VIEW1_ZOOM_SET              (KEY_ID_MIN + 165)
#define KEY_AXIS_IND_SET                (KEY_ID_MIN + 166)
#define KEY_ELEVATOR_TRIM_SET           (KEY_ID_MIN + 170)
#define KEY_COM_RADIO_SET               (KEY_ID_MIN + 171)
#define KEY_NAV1_RADIO_SET              (KEY_ID_MIN + 172)
#define KEY_NAV2_RADIO_SET              (KEY_ID_MIN + 173)
#define KEY_VIEW2_MODE_SET              (KEY_ID_MIN + 174)
#define KEY_VIEW2_DIRECTION_SET         (KEY_ID_MIN + 175)
#define KEY_VIEW2_ZOOM_SET              (KEY_ID_MIN + 176)
#define KEY_MAP_ZOOM_SET                (KEY_ID_MIN + 177)
#define KEY_ADF_SET                     (KEY_ID_MIN + 178)
#define KEY_XPNDR_SET                   (KEY_ID_MIN + 179)
#define KEY_VOR1_SET                    (KEY_ID_MIN + 180)
#define KEY_VOR2_SET                    (KEY_ID_MIN + 181)
#define KEY_ZOOM_MINUS                  (KEY_ID_MIN + 182)
#define KEY_ZOOM_PLUS                   (KEY_ID_MIN + 183)
#define KEY_BRAKES_LEFT                 (KEY_ID_MIN + 184)
#define KEY_BRAKES_RIGHT                (KEY_ID_MIN + 185)
#define KEY_AP_ATT_HOLD                 (KEY_ID_MIN + 186)
#define KEY_AP_LOC_HOLD                 (KEY_ID_MIN + 187)
#define KEY_AP_APR_HOLD                 (KEY_ID_MIN + 188)
#define KEY_AP_HDG_HOLD                 (KEY_ID_MIN + 189)
#define KEY_AP_ALT_HOLD                 (KEY_ID_MIN + 190)
#define KEY_AP_WING_LEVELER             (KEY_ID_MIN + 191)
#define KEY_AP_BC_HOLD                  (KEY_ID_MIN + 192)
#define KEY_AP_NAV1_HOLD                (KEY_ID_MIN + 193)
#define KEY_SLEW_OFF                    (KEY_ID_MIN + 194)
#define KEY_SLEW_ON                     (KEY_ID_MIN + 195)
#define KEY_EXIT                        (KEY_ID_MIN + 196)
#define KEY_ABORT                       (KEY_ID_MIN + 197)
#define KEY_PAN_UP                      (KEY_ID_MIN + 198)
#define KEY_PAN_DOWN                    (KEY_ID_MIN + 199)
#define KEY_READOUTS_SLEW               (KEY_ID_MIN + 200)
#define KEY_READOUTS_FLIGHT             (KEY_ID_MIN + 201)
#define KEY_SLEW_BANK_MINUS             (KEY_ID_MIN + 202)
#define KEY_SLEW_AHEAD_PLUS             (KEY_ID_MIN + 203)
#define KEY_SLEW_BANK_PLUS              (KEY_ID_MIN + 204)
#define KEY_SLEW_LEFT                   (KEY_ID_MIN + 205)
#define KEY_SLEW_FREEZE                 (KEY_ID_MIN + 206)
#define KEY_SLEW_RIGHT                  (KEY_ID_MIN + 207)
#define KEY_SLEW_HEADING_MINUS          (KEY_ID_MIN + 208)
#define KEY_SLEW_AHEAD_MINUS            (KEY_ID_MIN + 209)
#define KEY_SLEW_HEADING_PLUS           (KEY_ID_MIN + 210)
#define KEY_PANEL_SELECT_2              (KEY_ID_MIN + 211)
#define KEY_PANEL_TOGGLE                (KEY_ID_MIN + 212)
#define KEY_VIEW_MODE_REV               (KEY_ID_MIN + 213)
#define KEY_PANEL_LIGHTS_TOGGLE         (KEY_ID_MIN + 214)
#define KEY_LANDING_LIGHTS_TOGGLE       (KEY_ID_MIN + 215)
#define KEY_PARKING_BRAKES              (KEY_ID_MIN + 216)
#define KEY_ZOOM_IN_FINE                (KEY_ID_MIN + 218)
#define KEY_ZOOM_OUT_FINE               (KEY_ID_MIN + 219)
#define KEY_MINUS_SHIFT                 (KEY_ID_MIN + 220)
#define KEY_PLUS_SHIFT                  (KEY_ID_MIN + 221)
#define KEY_FLAPS_INCR                  (KEY_ID_MIN + 222)
#define KEY_FLAPS_DECR                  (KEY_ID_MIN + 223)
#define KEY_FLAPS_DETENTS_SET           (KEY_ID_MIN + 224)
#define KEY_AXIS_ELEVATOR_SET           (KEY_ID_MIN + 226)
#define KEY_AXIS_AILERONS_SET           (KEY_ID_MIN + 227)
#define KEY_AXIS_RUDDER_SET             (KEY_ID_MIN + 228)
#define KEY_AXIS_THROTTLE_SET           (KEY_ID_MIN + 229)
#define KEY_AXIS_ELEV_TRIM_SET          (KEY_ID_MIN + 230)
#define KEY_PROP_PITCH_SET              (KEY_ID_MIN + 231)
#define KEY_PROP_PITCH_LO               (KEY_ID_MIN + 232)
#define KEY_PROP_PITCH_INCR             (KEY_ID_MIN + 233)
#define KEY_PROP_PITCH_INCR_SMALL       (KEY_ID_MIN + 234)
#define KEY_PROP_PITCH_DECR             (KEY_ID_MIN + 235)
#define KEY_PROP_PITCH_HI               (KEY_ID_MIN + 236)
#define KEY_MIXTURE_SET                 (KEY_ID_MIN + 237)
#define KEY_MIXTURE_RICH                (KEY_ID_MIN + 238)
#define KEY_MIXTURE_INCR                (KEY_ID_MIN + 239)
#define KEY_MIXTURE_INCR_SMALL          (KEY_ID_MIN + 240)
#define KEY_MIXTURE_DECR                (KEY_ID_MIN + 241)
#define KEY_MIXTURE_LEAN                (KEY_ID_MIN + 242)
#define KEY_SCRIPT_EVENT_1              (KEY_ID_MIN + 246)
#define KEY_SCRIPT_EVENT_2              (KEY_ID_MIN + 247)
#define KEY_SPOILERS_SET                (KEY_ID_MIN + 250)
#define KEY_DME1_TOGGLE                 (KEY_ID_MIN + 251)
#define KEY_DME2_TOGGLE                 (KEY_ID_MIN + 252)
#define KEY_SIM_RATE_INCR               (KEY_ID_MIN + 253)
#define KEY_SIM_RATE_DECR               (KEY_ID_MIN + 254)
#define KEY_AUTOPILOT_OFF               (KEY_ID_MIN + 255)
#define KEY_AUTOPILOT_ON                (KEY_ID_MIN + 256)
#define KEY_YAW_DAMPER_TOGGLE           (KEY_ID_MIN + 257)
#define KEY_PAUSE_ON                    (KEY_ID_MIN + 258)
#define KEY_PAUSE_OFF                   (KEY_ID_MIN + 259)
#define KEY_SLEW_RESET                  (KEY_ID_MIN + 260)
#define KEY_AP_PANEL_HEADING_HOLD       (KEY_ID_MIN + 262)
#define KEY_AP_PANEL_ALTITUDE_HOLD      (KEY_ID_MIN + 263)
#define KEY_CHVPP_LEFT_HAT_UP           (KEY_ID_MIN + 264)
#define KEY_CHVPP_LEFT_HAT_DOWN         (KEY_ID_MIN + 265)
#define KEY_CHVPP_AP_ALT_WING           (KEY_ID_MIN + 266)
#define KEY_CENTER_NT361_CHECK          (KEY_ID_MIN + 267)
#define KEY_AP_ATT_HOLD_ON              (KEY_ID_MIN + 268)
#define KEY_AP_LOC_HOLD_ON              (KEY_ID_MIN + 269)
#define KEY_AP_APR_HOLD_ON              (KEY_ID_MIN + 270)
#define KEY_AP_HDG_HOLD_ON              (KEY_ID_MIN + 271)
#define KEY_AP_ALT_HOLD_ON              (KEY_ID_MIN + 272)
#define KEY_AP_WING_LEVELER_ON          (KEY_ID_MIN + 273)
#define KEY_AP_BC_HOLD_ON               (KEY_ID_MIN + 274)
#define KEY_AP_NAV1_HOLD_ON             (KEY_ID_MIN + 275)
#define KEY_AP_ATT_HOLD_OFF             (KEY_ID_MIN + 276)
#define KEY_AP_LOC_HOLD_OFF             (KEY_ID_MIN + 277)
#define KEY_AP_APR_HOLD_OFF             (KEY_ID_MIN + 278)
#define KEY_AP_HDG_HOLD_OFF             (KEY_ID_MIN + 279)
#define KEY_AP_ALT_HOLD_OFF             (KEY_ID_MIN + 280)
#define KEY_AP_WING_LEVELER_OFF         (KEY_ID_MIN + 281)
#define KEY_AP_BC_HOLD_OFF              (KEY_ID_MIN + 282)
#define KEY_AP_NAV1_HOLD_OFF            (KEY_ID_MIN + 283)
#define KEY_THROTTLE1_SET               (KEY_ID_MIN + 284)
#define KEY_THROTTLE2_SET               (KEY_ID_MIN + 285)
#define KEY_THROTTLE3_SET               (KEY_ID_MIN + 286)
#define KEY_THROTTLE4_SET               (KEY_ID_MIN + 287)
#define KEY_CLOSE_VIEW                  (KEY_ID_MIN + 288)
#define KEY_NEW_VIEW                    (KEY_ID_MIN + 289)
#define KEY_NEW_MAP                     (KEY_ID_MIN + 290)
#define KEY_NEXT_VIEW                   (KEY_ID_MIN + 291)
#define KEY_PREV_VIEW                   (KEY_ID_MIN + 292)
#define KEY_RADIO_VOR1_IDENT_DISABLE    (KEY_ID_MIN + 296)
#define KEY_RADIO_VOR2_IDENT_DISABLE    (KEY_ID_MIN + 297)
#define KEY_RADIO_DME1_IDENT_DISABLE    (KEY_ID_MIN + 298)
#define KEY_RADIO_DME2_IDENT_DISABLE    (KEY_ID_MIN + 299)
#define KEY_RADIO_ADF_IDENT_DISABLE     (KEY_ID_MIN + 300)
#define KEY_RADIO_VOR1_IDENT_ENABLE     (KEY_ID_MIN + 301)
#define KEY_RADIO_VOR2_IDENT_ENABLE     (KEY_ID_MIN + 302)
#define KEY_RADIO_DME1_IDENT_ENABLE     (KEY_ID_MIN + 303)
#define KEY_RADIO_DME2_IDENT_ENABLE     (KEY_ID_MIN + 304)
#define KEY_RADIO_ADF_IDENT_ENABLE      (KEY_ID_MIN + 305)
#define KEY_RADIO_VOR1_IDENT_TOGGLE     (KEY_ID_MIN + 306)
#define KEY_RADIO_VOR2_IDENT_TOGGLE     (KEY_ID_MIN + 307)
#define KEY_RADIO_DME1_IDENT_TOGGLE     (KEY_ID_MIN + 308)
#define KEY_RADIO_DME2_IDENT_TOGGLE     (KEY_ID_MIN + 309)
#define KEY_RADIO_ADF_IDENT_TOGGLE      (KEY_ID_MIN + 310)
#define KEY_RADIO_VOR1_IDENT_SET        (KEY_ID_MIN + 311)
#define KEY_RADIO_VOR2_IDENT_SET        (KEY_ID_MIN + 312)
#define KEY_RADIO_DME1_IDENT_SET        (KEY_ID_MIN + 313)
#define KEY_RADIO_DME2_IDENT_SET        (KEY_ID_MIN + 314)
#define KEY_RADIO_ADF_IDENT_SET         (KEY_ID_MIN + 315)
#define KEY_GEAR_PUMP                   (KEY_ID_MIN + 316)
#define KEY_SPOILERS_ARM_TOGGLE         (KEY_ID_MIN + 317)
#define KEY_PAN_LEFT_UP                 (KEY_ID_MIN + 318)
#define KEY_PAN_LEFT_DOWN               (KEY_ID_MIN + 319)
#define KEY_PAN_RIGHT_UP                (KEY_ID_MIN + 320)
#define KEY_PAN_RIGHT_DOWN              (KEY_ID_MIN + 321)
#define KEY_PITOT_HEAT_TOGGLE           (KEY_ID_MIN + 322)
#define KEY_AP_AIRSPEED_HOLD            (KEY_ID_MIN + 323)
#define KEY_AUTO_THROTTLE_ARM           (KEY_ID_MIN + 324)
#define KEY_AUTO_THROTTLE_TO_GA         (KEY_ID_MIN + 325)
#define KEY_LANDING_LIGHT_UP            (KEY_ID_MIN + 326)
#define KEY_LANDING_LIGHT_DOWN          (KEY_ID_MIN + 327)
#define KEY_LANDING_LIGHT_LEFT          (KEY_ID_MIN + 328)
#define KEY_LANDING_LIGHT_RIGHT         (KEY_ID_MIN + 329)
#define KEY_LANDING_LIGHT_HOME          (KEY_ID_MIN + 330)
#define KEY_AXIS_SLEW_AHEAD_SET         (KEY_ID_MIN + 331)
#define KEY_AXIS_SLEW_SIDEWAYS_SET      (KEY_ID_MIN + 332)
#define KEY_AXIS_SLEW_HEADING_SET       (KEY_ID_MIN + 333)
#define KEY_AXIS_SLEW_ALT_SET           (KEY_ID_MIN + 334)
#define KEY_AXIS_SLEW_BANK_SET          (KEY_ID_MIN + 335)
#define KEY_AXIS_SLEW_PITCH_SET         (KEY_ID_MIN + 336)
#define KEY_PAN_TILT_LEFT               (KEY_ID_MIN + 337)
#define KEY_PAN_TILT_RIGHT              (KEY_ID_MIN + 338)
#define KEY_PAN_RESET                   (KEY_ID_MIN + 339)
#define KEY_KNEEBOARD                   (KEY_ID_MIN + 340)
#define KEY_GYRO_DRIFT_INC              (KEY_ID_MIN + 341)
#define KEY_GYRO_DRIFT_DEC              (KEY_ID_MIN + 342)
#define KEY_HEADING_BUG_INC             (KEY_ID_MIN + 343)
#define KEY_HEADING_BUG_DEC             (KEY_ID_MIN + 344)
#define KEY_ADF_CARD_INC                (KEY_ID_MIN + 345)
#define KEY_ADF_CARD_DEC                (KEY_ID_MIN + 346)
#define KEY_KOHLSMAN_INC                (KEY_ID_MIN + 347)
#define KEY_KOHLSMAN_DEC                (KEY_ID_MIN + 348)
#define KEY_TRUE_AIRSPEED_CALIBRATE_INC (KEY_ID_MIN + 349)
#define KEY_TRUE_AIRSPEED_CALIBRATE_DEC (KEY_ID_MIN + 350)
#define KEY_CROSS_FEED_OFF              (KEY_ID_MIN + 351)
#define KEY_CROSS_FEED_LEFT_TO_RIGHT    (KEY_ID_MIN + 352)
#define KEY_CROSS_FEED_RIGHT_TO_LEFT    (KEY_ID_MIN + 353)
#define KEY_AP_PANEL_VS_HOLD            (KEY_ID_MIN + 354)
#define KEY_AP_PANEL_SPEED_HOLD         (KEY_ID_MIN + 355)
#define KEY_AP_ALT_VAR_INC              (KEY_ID_MIN + 356)
#define KEY_AP_ALT_VAR_DEC              (KEY_ID_MIN + 357)
#define KEY_AP_VS_VAR_INC               (KEY_ID_MIN + 358)
#define KEY_AP_VS_VAR_DEC               (KEY_ID_MIN + 359)
#define KEY_AP_SPD_VAR_INC              (KEY_ID_MIN + 360)
#define KEY_AP_SPD_VAR_DEC              (KEY_ID_MIN + 361)
#define KEY_AP_N1_REF_INC               (KEY_ID_MIN + 362)
#define KEY_AP_N1_REF_DEC               (KEY_ID_MIN + 363)
#define KEY_AP_N1_REF_SET               (KEY_ID_MIN + 364)
#define KEY_MULTIPLAYER_TRANSFER_CONTROL (KEY_ID_MIN + 365)
#define KEY_MULTIPLAYER_PLAYER_CYCLE    (KEY_ID_MIN + 366)
#define KEY_MULTIPLAYER_PLAYER_FOLLOW   (KEY_ID_MIN + 367)
#define KEY_MULTIPLAYER_CHAT            (KEY_ID_MIN + 368)
#define KEY_MULTIPLAYER_ACTIVATE_CHAT   (KEY_ID_MIN + 369)
#define KEY_PANEL_1                     (KEY_ID_MIN + 370)
#define KEY_PANEL_2                     (KEY_ID_MIN + 371)
#define KEY_PANEL_3                     (KEY_ID_MIN + 372)
#define KEY_PANEL_4                     (KEY_ID_MIN + 373)
#define KEY_PANEL_5                     (KEY_ID_MIN + 374)
#define KEY_PANEL_6                     (KEY_ID_MIN + 375)
#define KEY_PANEL_7                     (KEY_ID_MIN + 376)
#define KEY_PANEL_8                     (KEY_ID_MIN + 377)
#define KEY_PANEL_9                     (KEY_ID_MIN + 378)
#define KEY_AP_PANEL_MACH_HOLD          (KEY_ID_MIN + 379)
#define KEY_AP_MACH_VAR_INC             (KEY_ID_MIN + 380)
#define KEY_AP_MACH_VAR_DEC             (KEY_ID_MIN + 381)
#define KEY_AP_MACH_HOLD                (KEY_ID_MIN + 382)
#define KEY_MIXTURE1_SET                (KEY_ID_MIN + 383)
#define KEY_MIXTURE2_SET                (KEY_ID_MIN + 384)
#define KEY_MIXTURE3_SET                (KEY_ID_MIN + 385)
#define KEY_MIXTURE4_SET                (KEY_ID_MIN + 386)
#define KEY_PROP_PITCH1_SET             (KEY_ID_MIN + 387)
#define KEY_PROP_PITCH2_SET             (KEY_ID_MIN + 388)
#define KEY_PROP_PITCH3_SET             (KEY_ID_MIN + 389)
#define KEY_PROP_PITCH4_SET             (KEY_ID_MIN + 390)
#define KEY_MAGNETO1_OFF                (KEY_ID_MIN + 391)
#define KEY_MAGNETO1_RIGHT              (KEY_ID_MIN + 392)
#define KEY_MAGNETO1_LEFT               (KEY_ID_MIN + 393)
#define KEY_MAGNETO1_BOTH               (KEY_ID_MIN + 394)
#define KEY_MAGNETO1_START              (KEY_ID_MIN + 395)
#define KEY_STARTER1_SET                (KEY_ID_MIN + 396)
#define KEY_MAGNETO2_OFF                (KEY_ID_MIN + 397)
#define KEY_MAGNETO2_RIGHT              (KEY_ID_MIN + 398)
#define KEY_MAGNETO2_LEFT               (KEY_ID_MIN + 399)
#define KEY_MAGNETO2_BOTH               (KEY_ID_MIN + 400)
#define KEY_MAGNETO2_START              (KEY_ID_MIN + 401)
#define KEY_STARTER2_SET                (KEY_ID_MIN + 402)
#define KEY_MAGNETO3_OFF                (KEY_ID_MIN + 403)
#define KEY_MAGNETO3_RIGHT              (KEY_ID_MIN + 404)
#define KEY_MAGNETO3_LEFT               (KEY_ID_MIN + 405)
#define KEY_MAGNETO3_BOTH               (KEY_ID_MIN + 406)
#define KEY_MAGNETO3_START              (KEY_ID_MIN + 407)
#define KEY_STARTER3_SET                (KEY_ID_MIN + 408)
#define KEY_MAGNETO4_OFF                (KEY_ID_MIN + 409)
#define KEY_MAGNETO4_RIGHT              (KEY_ID_MIN + 410)
#define KEY_MAGNETO4_LEFT               (KEY_ID_MIN + 411)
#define KEY_MAGNETO4_BOTH               (KEY_ID_MIN + 412)
#define KEY_MAGNETO4_START              (KEY_ID_MIN + 413)
#define KEY_STARTER4_SET                (KEY_ID_MIN + 414)
#define KEY_AUTOCOORD_TOGGLE            (KEY_ID_MIN + 415)
#define KEY_AUTOCOORD_OFF               (KEY_ID_MIN + 416)
#define KEY_AUTOCOORD_ON                (KEY_ID_MIN + 417)
#define KEY_AUTOCOORD_SET               (KEY_ID_MIN + 418)
#define KEY_FUEL_SELECTOR_OFF           (KEY_ID_MIN + 419)
#define KEY_FUEL_SELECTOR_ALL           (KEY_ID_MIN + 420)
#define KEY_FUEL_SELECTOR_LEFT          (KEY_ID_MIN + 421)
#define KEY_FUEL_SELECTOR_RIGHT         (KEY_ID_MIN + 422)
#define KEY_FUEL_SELECTOR_LEFT_AUX      (KEY_ID_MIN + 423)
#define KEY_FUEL_SELECTOR_RIGHT_AUX     (KEY_ID_MIN + 424)
#define KEY_FUEL_SELECTOR_CENTER        (KEY_ID_MIN + 425)
#define KEY_FUEL_SELECTOR_SET           (KEY_ID_MIN + 426)
#define KEY_THROTTLE1_FULL              (KEY_ID_MIN + 427)
#define KEY_THROTTLE1_INCR              (KEY_ID_MIN + 428)
#define KEY_THROTTLE1_INCR_SMALL        (KEY_ID_MIN + 429)
#define KEY_THROTTLE1_DECR              (KEY_ID_MIN + 430)
#define KEY_THROTTLE1_CUT               (KEY_ID_MIN + 431)
#define KEY_THROTTLE2_FULL              (KEY_ID_MIN + 432)
#define KEY_THROTTLE2_INCR              (KEY_ID_MIN + 433)
#define KEY_THROTTLE2_INCR_SMALL        (KEY_ID_MIN + 434)
#define KEY_THROTTLE2_DECR              (KEY_ID_MIN + 435)
#define KEY_THROTTLE2_CUT               (KEY_ID_MIN + 436)
#define KEY_THROTTLE3_FULL              (KEY_ID_MIN + 437)
#define KEY_THROTTLE3_INCR              (KEY_ID_MIN + 438)
#define KEY_THROTTLE3_INCR_SMALL        (KEY_ID_MIN + 439)
#define KEY_THROTTLE3_DECR              (KEY_ID_MIN + 440)
#define KEY_THROTTLE3_CUT               (KEY_ID_MIN + 441)
#define KEY_THROTTLE4_FULL              (KEY_ID_MIN + 442)
#define KEY_THROTTLE4_INCR              (KEY_ID_MIN + 443)
#define KEY_THROTTLE4_INCR_SMALL        (KEY_ID_MIN + 444)
#define KEY_THROTTLE4_DECR              (KEY_ID_MIN + 445)
#define KEY_THROTTLE4_CUT               (KEY_ID_MIN + 446)
#define KEY_MIXTURE1_RICH               (KEY_ID_MIN + 447)
#define KEY_MIXTURE1_INCR               (KEY_ID_MIN + 448)
#define KEY_MIXTURE1_INCR_SMALL         (KEY_ID_MIN + 449)
#define KEY_MIXTURE1_DECR               (KEY_ID_MIN + 450)
#define KEY_MIXTURE1_LEAN               (KEY_ID_MIN + 451)
#define KEY_MIXTURE2_RICH               (KEY_ID_MIN + 452)
#define KEY_MIXTURE2_INCR               (KEY_ID_MIN + 453)
#define KEY_MIXTURE2_INCR_SMALL         (KEY_ID_MIN + 454)
#define KEY_MIXTURE2_DECR               (KEY_ID_MIN + 455)
#define KEY_MIXTURE2_LEAN               (KEY_ID_MIN + 456)
#define KEY_MIXTURE3_RICH               (KEY_ID_MIN + 457)
#define KEY_MIXTURE3_INCR               (KEY_ID_MIN + 458)
#define KEY_MIXTURE3_INCR_SMALL         (KEY_ID_MIN + 459)
#define KEY_MIXTURE3_DECR               (KEY_ID_MIN + 460)
#define KEY_MIXTURE3_LEAN               (KEY_ID_MIN + 461)
#define KEY_MIXTURE4_RICH               (KEY_ID_MIN + 462)
#define KEY_MIXTURE4_INCR               (KEY_ID_MIN + 463)
#define KEY_MIXTURE4_INCR_SMALL         (KEY_ID_MIN + 464)
#define KEY_MIXTURE4_DECR               (KEY_ID_MIN + 465)
#define KEY_MIXTURE4_LEAN               (KEY_ID_MIN + 466)
#define KEY_PROP_PITCH1_LO              (KEY_ID_MIN + 467)
#define KEY_PROP_PITCH1_INCR            (KEY_ID_MIN + 468)
#define KEY_PROP_PITCH1_INCR_SMALL      (KEY_ID_MIN + 469)
#define KEY_PROP_PITCH1_DECR            (KEY_ID_MIN + 470)
#define KEY_PROP_PITCH1_HI              (KEY_ID_MIN + 471)
#define KEY_PROP_PITCH2_LO              (KEY_ID_MIN + 472)
#define KEY_PROP_PITCH2_INCR            (KEY_ID_MIN + 473)
#define KEY_PROP_PITCH2_INCR_SMALL      (KEY_ID_MIN + 474)
#define KEY_PROP_PITCH2_DECR            (KEY_ID_MIN + 475)
#define KEY_PROP_PITCH2_HI              (KEY_ID_MIN + 476)
#define KEY_PROP_PITCH3_LO              (KEY_ID_MIN + 477)
#define KEY_PROP_PITCH3_INCR            (KEY_ID_MIN + 478)
#define KEY_PROP_PITCH3_INCR_SMALL      (KEY_ID_MIN + 479)
#define KEY_PROP_PITCH3_DECR            (KEY_ID_MIN + 480)
#define KEY_PROP_PITCH3_HI              (KEY_ID_MIN + 481)
#define KEY_PROP_PITCH4_LO              (KEY_ID_MIN + 482)
#define KEY_PROP_PITCH4_INCR            (KEY_ID_MIN + 483)
#define KEY_PROP_PITCH4_INCR_SMALL      (KEY_ID_MIN + 484)
#define KEY_PROP_PITCH4_DECR            (KEY_ID_MIN + 485)
#define KEY_PROP_PITCH4_HI              (KEY_ID_MIN + 486)
#define KEY_MAGNETO_OFF                 (KEY_ID_MIN + 487)
#define KEY_STARTER_OFF                 (KEY_ID_MIN + 487)
#define KEY_MAGNETO_RIGHT               (KEY_ID_MIN + 488)
#define KEY_STARTER_START               (KEY_ID_MIN + 488)
#define KEY_MAGNETO_LEFT                (KEY_ID_MIN + 489)
#define KEY_STARTER_GEN                 (KEY_ID_MIN + 489)
#define KEY_MAGNETO_BOTH                (KEY_ID_MIN + 490)
#define KEY_MAGNETO_START               (KEY_ID_MIN + 491)
#define KEY_STARTER_SET                 (KEY_ID_MIN + 492)
#define KEY_ANTI_ICE_ON                 (KEY_ID_MIN + 493)
#define KEY_ANTI_ICE_OFF                (KEY_ID_MIN + 494)
#define KEY_ANTI_ICE_SET                (KEY_ID_MIN + 495)
#define KEY_EGT_INC                     (KEY_ID_MIN + 496)
#define KEY_EGT_DEC                     (KEY_ID_MIN + 497)
#define KEY_EGT_SET                     (KEY_ID_MIN + 498)
#define KEY_AP_ALT_VAR_SET_METRIC       (KEY_ID_MIN + 499)
#define KEY_AP_VS_VAR_SET_ENGLISH       (KEY_ID_MIN + 500)
#define KEY_AP_SPD_VAR_SET              (KEY_ID_MIN + 501)
#define KEY_AP_MACH_VAR_SET             (KEY_ID_MIN + 502)
#define KEY_ADF_CARD_SET                (KEY_ID_MIN + 503)
#define KEY_KOHLSMAN_SET                (KEY_ID_MIN + 504)
#define KEY_SIM_RATE_SET                (KEY_ID_MIN + 505)
#define KEY_HEADING_BUG_SET             (KEY_ID_MIN + 506)
#define KEY_TRUE_AIRSPEED_CAL_SET       (KEY_ID_MIN + 507)
#define KEY_CLOCK_HOURS_SET             (KEY_ID_MIN + 508)
#define KEY_CLOCK_MINUTES_SET           (KEY_ID_MIN + 509)
#define KEY_GYRO_DRIFT_SET              (KEY_ID_MIN + 510)
#define KEY_ADF_EXTENDED_SET            (KEY_ID_MIN + 511)
#define KEY_SLEW_SET                    (KEY_ID_MIN + 512)
#define KEY_SMOKE_ON                    (KEY_ID_MIN + 513)
#define KEY_SMOKE_OFF                   (KEY_ID_MIN + 514)
#define KEY_SMOKE_SET                   (KEY_ID_MIN + 515)
#define KEY_STROBES_ON                  (KEY_ID_MIN + 516)
#define KEY_STROBES_OFF                 (KEY_ID_MIN + 517)
#define KEY_STROBES_SET                 (KEY_ID_MIN + 518)
#define KEY_PAUSE_SET                   (KEY_ID_MIN + 519)
#define KEY_PANEL_LIGHTS_ON             (KEY_ID_MIN + 520)
#define KEY_PANEL_LIGHTS_OFF            (KEY_ID_MIN + 521)
#define KEY_PANEL_LIGHTS_SET            (KEY_ID_MIN + 522)
#define KEY_LANDING_LIGHTS_ON           (KEY_ID_MIN + 523)
#define KEY_LANDING_LIGHTS_OFF          (KEY_ID_MIN + 524)
#define KEY_LANDING_LIGHTS_SET          (KEY_ID_MIN + 525)
#define KEY_SOUND_ON                    (KEY_ID_MIN + 526)
#define KEY_SOUND_OFF                   (KEY_ID_MIN + 527)
#define KEY_SPOILERS_ON                 (KEY_ID_MIN + 528)
#define KEY_SPOILERS_OFF                (KEY_ID_MIN + 529)
#define KEY_SPOILERS_ARM_ON             (KEY_ID_MIN + 530)
#define KEY_SPOILERS_ARM_OFF            (KEY_ID_MIN + 531)
#define KEY_SPOILERS_ARM_SET            (KEY_ID_MIN + 532)
#define KEY_YAW_DAMPER_ON               (KEY_ID_MIN + 533)
#define KEY_YAW_DAMPER_OFF              (KEY_ID_MIN + 534)
#define KEY_YAW_DAMPER_SET              (KEY_ID_MIN + 535)
#define KEY_PITOT_HEAT_ON               (KEY_ID_MIN + 536)
#define KEY_PITOT_HEAT_OFF              (KEY_ID_MIN + 537)
#define KEY_PITOT_HEAT_SET              (KEY_ID_MIN + 538)
#define KEY_ZULU_HOURS_SET              (KEY_ID_MIN + 539)
#define KEY_ZULU_MINUTES_SET            (KEY_ID_MIN + 540)
#define KEY_ZULU_DAY_SET                (KEY_ID_MIN + 541)
#define KEY_ZULU_YEAR_SET               (KEY_ID_MIN + 542)
#define KEY_GEAR_UP                     (KEY_ID_MIN + 543)
#define KEY_GEAR_DOWN                   (KEY_ID_MIN + 544)
#define KEY_EGT1_INC                    (KEY_ID_MIN + 545)
#define KEY_EGT1_DEC                    (KEY_ID_MIN + 546)
#define KEY_EGT1_SET                    (KEY_ID_MIN + 547)
#define KEY_EGT2_INC                    (KEY_ID_MIN + 548)
#define KEY_EGT2_DEC                    (KEY_ID_MIN + 549)
#define KEY_EGT2_SET                    (KEY_ID_MIN + 550)
#define KEY_EGT3_INC                    (KEY_ID_MIN + 551)
#define KEY_EGT3_DEC                    (KEY_ID_MIN + 552)
#define KEY_EGT3_SET                    (KEY_ID_MIN + 553)
#define KEY_EGT4_INC                    (KEY_ID_MIN + 554)
#define KEY_EGT4_DEC                    (KEY_ID_MIN + 555)
#define KEY_EGT4_SET                    (KEY_ID_MIN + 556)
#define KEY_AP_AIRSPEED_ON              (KEY_ID_MIN + 557)
#define KEY_AP_AIRSPEED_OFF             (KEY_ID_MIN + 558)
#define KEY_AP_AIRSPEED_SET             (KEY_ID_MIN + 559)
#define KEY_AP_MACH_ON                  (KEY_ID_MIN + 560)
#define KEY_AP_MACH_OFF                 (KEY_ID_MIN + 561)
#define KEY_AP_MACH_SET                 (KEY_ID_MIN + 562)
#define KEY_AP_VS_HOLD                  (KEY_ID_MIN + 563)
#define KEY_AP_VS_ON                    (KEY_ID_MIN + 564)
#define KEY_AP_VS_OFF                   (KEY_ID_MIN + 565)
#define KEY_AP_VS_SET                   (KEY_ID_MIN + 566)
#define KEY_AP_PANEL_ALTITUDE_ON        (KEY_ID_MIN + 567)
#define KEY_AP_PANEL_ALTITUDE_OFF       (KEY_ID_MIN + 568)
#define KEY_AP_PANEL_ALTITUDE_SET       (KEY_ID_MIN + 569)
#define KEY_AP_PANEL_HEADING_ON         (KEY_ID_MIN + 570)
#define KEY_AP_PANEL_HEADING_OFF        (KEY_ID_MIN + 571)
#define KEY_AP_PANEL_HEADING_SET        (KEY_ID_MIN + 572)
#define KEY_AP_PANEL_MACH_ON            (KEY_ID_MIN + 573)
#define KEY_AP_PANEL_MACH_OFF           (KEY_ID_MIN + 574)
#define KEY_AP_PANEL_MACH_SET           (KEY_ID_MIN + 575)
#define KEY_AP_PANEL_SPEED_ON           (KEY_ID_MIN + 576)
#define KEY_AP_PANEL_SPEED_OFF          (KEY_ID_MIN + 577)
#define KEY_AP_PANEL_SPEED_SET          (KEY_ID_MIN + 578)
#define KEY_AP_PANEL_VS_ON              (KEY_ID_MIN + 579)
#define KEY_AP_PANEL_VS_OFF             (KEY_ID_MIN + 580)
#define KEY_AP_PANEL_VS_SET             (KEY_ID_MIN + 581)
#define KEY_SEE_OWN_AC_TOGGLE           (KEY_ID_MIN + 582)
#define KEY_SEE_OWN_AC_ON               (KEY_ID_MIN + 583)
#define KEY_SEE_OWN_AC_OFF              (KEY_ID_MIN + 584)
#define KEY_SEE_OWN_AC_SET              (KEY_ID_MIN + 585)
#define KEY_ADF_LOWRANGE_SET            (KEY_ID_MIN + 586)
#define KEY_ADF_HIGHRANGE_SET           (KEY_ID_MIN + 587)
#define KEY_AP_ALT_VAR_SET_ENGLISH      (KEY_ID_MIN + 588)
#define KEY_AP_VS_VAR_SET_METRIC        (KEY_ID_MIN + 589)
#define KEY_MAGNETO1_DECR               (KEY_ID_MIN + 590)
#define KEY_MAGNETO1_INCR               (KEY_ID_MIN + 591)
#define KEY_MAGNETO2_DECR               (KEY_ID_MIN + 592)
#define KEY_MAGNETO2_INCR               (KEY_ID_MIN + 593)
#define KEY_MAGNETO3_DECR               (KEY_ID_MIN + 594)
#define KEY_MAGNETO3_INCR               (KEY_ID_MIN + 595)
#define KEY_MAGNETO4_DECR               (KEY_ID_MIN + 596)
#define KEY_MAGNETO4_INCR               (KEY_ID_MIN + 597)
#define KEY_GUNSIGHT_SEL                (KEY_ID_MIN + 599)
#define KEY_GUNSIGHT_TOGGLE             (KEY_ID_MIN + 600)
#define KEY_VIEW_FORWARD_UP             (KEY_ID_MIN + 601)
#define KEY_VIEW_FORWARD_RIGHT_UP       (KEY_ID_MIN + 602)
#define KEY_VIEW_RIGHT_UP               (KEY_ID_MIN + 603)
#define KEY_VIEW_REAR_RIGHT_UP          (KEY_ID_MIN + 604)
#define KEY_VIEW_REAR_UP                (KEY_ID_MIN + 605)
#define KEY_VIEW_REAR_LEFT_UP           (KEY_ID_MIN + 606)
#define KEY_VIEW_LEFT_UP                (KEY_ID_MIN + 607)
#define KEY_VIEW_FORWARD_LEFT_UP        (KEY_ID_MIN + 608)
#define KEY_VIEW_UP                     (KEY_ID_MIN + 609)
#define KEY_SKIP_ACTION                 (KEY_ID_MIN + 610)          // Warp to next action waypoint
#define KEY_VIEW_RESET                  (KEY_ID_MIN + 611)          // reset views
#define KEY_MAP_ORIENTATION_SET         (KEY_ID_MIN + 612)
#define KEY_WINDOW_TITLES_SET           (KEY_ID_MIN + 613)
#define KEY_TEXT_SCROLL_SET             (KEY_ID_MIN + 614)
#define KEY_VIEW_ALWAYS_PAN_UP          (KEY_ID_MIN + 615)
#define KEY_VIEW_ALWAYS_PAN_DOWN        (KEY_ID_MIN + 616)
#define KEY_NEXT_SUB_VIEW               (KEY_ID_MIN + 617)
#define KEY_PREV_SUB_VIEW               (KEY_ID_MIN + 618)
#define KEY_FIRE_ALL_GUNS               (KEY_ID_MIN + 620)
#define KEY_FIRE_PRIMARY_GUNS           (KEY_ID_MIN + 621)
#define KEY_FIRE_SECONDARY_GUNS         (KEY_ID_MIN + 622)
#define KEY_COWLFLAP1_SET               (KEY_ID_MIN + 626)
#define KEY_COWLFLAP2_SET               (KEY_ID_MIN + 627)
#define KEY_COWLFLAP3_SET               (KEY_ID_MIN + 628)
#define KEY_COWLFLAP4_SET               (KEY_ID_MIN + 629)
#define KEY_VIEW_TRACK_PAN_TOGGLE       (KEY_ID_MIN + 630)
#define KEY_VIEW_PREVIOUS_TOGGLE        (KEY_ID_MIN + 631)
#define KEY_VIEW_CAMERA_SELECT_STARTING (KEY_ID_MIN + 632)
#define KEY_TOGGLE_RADAR                (KEY_ID_MIN + 634)
#define KEY_ATC_MENU_1                  (KEY_ID_MIN + 636)
#define KEY_ATC_MENU_2                  (KEY_ID_MIN + 637)
#define KEY_ATC_MENU_3                  (KEY_ID_MIN + 638)
#define KEY_ATC_MENU_4                  (KEY_ID_MIN + 639)
#define KEY_ATC_MENU_5                  (KEY_ID_MIN + 640)
#define KEY_ATC_MENU_6                  (KEY_ID_MIN + 641)
#define KEY_ATC_MENU_7                  (KEY_ID_MIN + 642)
#define KEY_ATC_MENU_8                  (KEY_ID_MIN + 643)
#define KEY_ATC_MENU_9                  (KEY_ID_MIN + 644)
#define KEY_ATC_MENU_0                  (KEY_ID_MIN + 645)
#define KEY_VIEW_AUX_00                 (KEY_ID_MIN + 646)
#define KEY_VIEW_AUX_01                 (KEY_ID_MIN + 647)
#define KEY_VIEW_AUX_02                 (KEY_ID_MIN + 648)
#define KEY_VIEW_AUX_03                 (KEY_ID_MIN + 649)
#define KEY_VIEW_AUX_04                 (KEY_ID_MIN + 650)
#define KEY_VIEW_AUX_05                 (KEY_ID_MIN + 651)
#define KEY_INVOKE_HELP                 (KEY_ID_MIN + 659)
#define KEY_SELECT_NEXT_TARGET          (KEY_ID_MIN + 660)
#define KEY_TOGGLE_PADLOCK              (KEY_ID_MIN + 661)
#define KEY_UNLOCK_TARGET               (KEY_ID_MIN + 662)
#define KEY_TOGGLE_AIRCRAFT_LABELS      (KEY_ID_MIN + 663)
#define KEY_TOGGLE_DAMAGE_TEXT          (KEY_ID_MIN + 664)
#define KEY_TOGGLE_ENEMY_INDICATOR      (KEY_ID_MIN + 665)
#define KEY_WAR_EMERGENCY_POWER         (KEY_ID_MIN + 666)
#define KEY_BAIL_OUT                    (KEY_ID_MIN + 667)
#define KEY_TOGGLE_RADIO                (KEY_ID_MIN + 668)
#define KEY_KEYBOARD_OVERLAY            (KEY_ID_MIN + 669)
#define KEY_HUD_UNITS                   (KEY_ID_MIN + 673)
#define KEY_HUD_COLOR                   (KEY_ID_MIN + 674)
#define KEY_LETTERBOX                   (KEY_ID_MIN + 687)
#define KEY_ENGINE_AUTO_START           (KEY_ID_MIN + 688)
#define KEY_THROTTLE_10                 (KEY_ID_MIN + 689)
#define KEY_THROTTLE_20                 (KEY_ID_MIN + 690)
#define KEY_THROTTLE_30                 (KEY_ID_MIN + 691)
#define KEY_THROTTLE_40                 (KEY_ID_MIN + 692)
#define KEY_THROTTLE_50                 (KEY_ID_MIN + 693)
#define KEY_THROTTLE_60                 (KEY_ID_MIN + 694)
#define KEY_THROTTLE_70                 (KEY_ID_MIN + 695)
#define KEY_THROTTLE_80                 (KEY_ID_MIN + 696)
#define KEY_THROTTLE_90                 (KEY_ID_MIN + 697)
#define KEY_FORCE_END                   (KEY_ID_MIN + 698)
#define KEY_FUEL_PUMP                   (KEY_ID_MIN + 701)
#define KEY_ENGINE_PRIMER               (KEY_ID_MIN + 702)
#define KEY_TOGGLE_BEACON_LIGHTS        (KEY_ID_MIN + 703)
#define KEY_TOGGLE_TAXI_LIGHTS          (KEY_ID_MIN + 704)
#define KEY_TOGGLE_MASTER_BATTERY       (KEY_ID_MIN + 705)
#define KEY_TOGGLE_MASTER_ALTERNATOR    (KEY_ID_MIN + 706)
#define KEY_INC_COWL_FLAPS              (KEY_ID_MIN + 707)
#define KEY_DEC_COWL_FLAPS              (KEY_ID_MIN + 708)
#define KEY_OVERLAYMENU                 (KEY_ID_MIN + 731)
#define KEY_USERINTERRUPT               (KEY_ID_MIN + 732)
#define KEY_SELECT_PREV_TARGET          (KEY_ID_MIN + 733)
#define KEY_STOP_PRIMARY_GUNS           (KEY_ID_MIN + 734)
#define KEY_STOP_SECONDARY_GUNS         (KEY_ID_MIN + 735)
#define KEY_STOP_ALL_GUNS               (KEY_ID_MIN + 736)
#define KEY_SP_MULTIPLAYER_SCORE_DISPLAY (KEY_ID_MIN + 737)
#define KEY_AILERON_TRIM_LEFT           (KEY_ID_MIN + 740)
#define KEY_AILERON_TRIM_RIGHT          (KEY_ID_MIN + 741)
#define KEY_RUDDER_TRIM_LEFT            (KEY_ID_MIN + 742)
#define KEY_RUDDER_TRIM_RIGHT           (KEY_ID_MIN + 743)
#define KEY_RADIO_COMMNAV1_TEST_TOGGLE  (KEY_ID_MIN + 744)
#define KEY_RADIO_COMMNAV2_TEST_TOGGLE  (KEY_ID_MIN + 745)
#define KEY_RADIO_COMM1_AUTOSWITCH_TOGGLE (KEY_ID_MIN + 746)
#define KEY_RADIO_NAV1_AUTOSWITCH_TOGGLE (KEY_ID_MIN + 747)
#define KEY_RADIO_COMM2_AUTOSWITCH_TOGGLE (KEY_ID_MIN + 748)
#define KEY_RADIO_NAV2_AUTOSWITCH_TOGGLE (KEY_ID_MIN + 749)
#define KEY_DME_TOGGLE                  (KEY_ID_MIN + 750)
#define KEY_TOGGLE_PROP_SYNC            (KEY_ID_MIN + 751)
#define KEY_TOGGLE_FLIGHT_DIRECTOR      (KEY_ID_MIN + 752)
#define KEY_SYNC_FLIGHT_DIRECTOR_PITCH  (KEY_ID_MIN + 753)
#define KEY_TOGGLE_ELECTRIC_VACUUM_PUMP (KEY_ID_MIN + 754)
#define KEY_AXIS_PROPELLER_SET          (KEY_ID_MIN + 755)
#define KEY_AXIS_MIXTURE_SET            (KEY_ID_MIN + 756)
#define KEY_TOGGLE_AVIONICS_MASTER      (KEY_ID_MIN + 757)
#define KEY_INC_CONCORDE_NOSE_VISOR     (KEY_ID_MIN + 758)
#define KEY_DEC_CONCORDE_NOSE_VISOR     (KEY_ID_MIN + 759)
#define KEY_TOGGLE_AFTERBURNER          (KEY_ID_MIN + 760)
#define KEY_TOGGLE_ARM_AUTOFEATHER      (KEY_ID_MIN + 761)
#define KEY_INC_AUTOBRAKE_CONTROL       (KEY_ID_MIN + 762)
#define KEY_DEC_AUTOBRAKE_CONTROL       (KEY_ID_MIN + 763)
#define KEY_TOGGLE_STARTER1             (KEY_ID_MIN + 764)
#define KEY_TOGGLE_STARTER2             (KEY_ID_MIN + 765)
#define KEY_TOGGLE_STARTER3             (KEY_ID_MIN + 766)
#define KEY_TOGGLE_STARTER4             (KEY_ID_MIN + 767)
#define KEY_TOGGLE_ALL_STARTERS         (KEY_ID_MIN + 768)
#define KEY_TOGGLE_VACUUM_FAILURE       (KEY_ID_MIN + 769)
#define KEY_TOGGLE_ELECTRICAL_FAILURE   (KEY_ID_MIN + 770)
#define KEY_TOGGLE_PITOT_BLOCKAGE       (KEY_ID_MIN + 771)
#define KEY_TOGGLE_STATIC_PORT_BLOCKAGE (KEY_ID_MIN + 772)
#define KEY_TOGGLE_HYDRAULIC_FAILURE    (KEY_ID_MIN + 773)
#define KEY_TOGGLE_TOTAL_BRAKE_FAILURE  (KEY_ID_MIN + 774)
#define KEY_TOGGLE_LEFT_BRAKE_FAILURE   (KEY_ID_MIN + 775)
#define KEY_TOGGLE_RIGHT_BRAKE_FAILURE  (KEY_ID_MIN + 776)
#define KEY_TOGGLE_ENGINE1_FAILURE      (KEY_ID_MIN + 777)
#define KEY_TOGGLE_ENGINE2_FAILURE      (KEY_ID_MIN + 778)
#define KEY_TOGGLE_ENGINE3_FAILURE      (KEY_ID_MIN + 779)
#define KEY_TOGGLE_ENGINE4_FAILURE      (KEY_ID_MIN + 780)
#define KEY_TOGGLE_ALTERNATE_STATIC     (KEY_ID_MIN + 781)
#define KEY_ATTITUDE_BARS_POSITION_INC  (KEY_ID_MIN + 782)
#define KEY_ATTITUDE_BARS_POSITION_DEC  (KEY_ID_MIN + 783)
#define KEY_TOGGLE_RAD_INS_SWITCH       (KEY_ID_MIN + 784)
#define KEY_DECISION_HEIGHT_INC         (KEY_ID_MIN + 785)
#define KEY_DECISION_HEIGHT_DEC         (KEY_ID_MIN + 786)
#define KEY_LOW_HIGHT_WARNING_SET       (KEY_ID_MIN + 787)
#define KEY_LOW_HIGHT_WARNING_GAUGE_WILL_SET (KEY_ID_MIN + 788)
#define KEY_SET_FUEL_TRANSFER_FORWARD   (KEY_ID_MIN + 789)
#define KEY_SET_FUEL_TRANSFER_AFT       (KEY_ID_MIN + 790)
#define KEY_SET_FUEL_TRANSFER_AUTO      (KEY_ID_MIN + 791)
#define KEY_SET_FUEL_TRANSFER_OFF       (KEY_ID_MIN + 792)
#define KEY_INC_COWL_FLAPS1             (KEY_ID_MIN + 793)
#define KEY_DEC_COWL_FLAPS1             (KEY_ID_MIN + 794)
#define KEY_INC_COWL_FLAPS2             (KEY_ID_MIN + 795)
#define KEY_DEC_COWL_FLAPS2             (KEY_ID_MIN + 796)
#define KEY_INC_COWL_FLAPS3             (KEY_ID_MIN + 797)
#define KEY_DEC_COWL_FLAPS3             (KEY_ID_MIN + 798)
#define KEY_INC_COWL_FLAPS4             (KEY_ID_MIN + 799)
#define KEY_DEC_COWL_FLAPS4             (KEY_ID_MIN + 800)
#define KEY_TOGGLE_STRUCTURAL_DEICE     (KEY_ID_MIN + 801)
#define KEY_TOGGLE_PROPELLER_DEICE      (KEY_ID_MIN + 802)
#define KEY_TOGGLE_ELECT_FUEL_PUMP      (KEY_ID_MIN + 803)
#define KEY_TOGGLE_ELECT_FUEL_PUMP1     (KEY_ID_MIN + 804)
#define KEY_TOGGLE_ELECT_FUEL_PUMP2     (KEY_ID_MIN + 805)
#define KEY_TOGGLE_ELECT_FUEL_PUMP3     (KEY_ID_MIN + 806)
#define KEY_TOGGLE_ELECT_FUEL_PUMP4     (KEY_ID_MIN + 807)
#define KEY_TOGGLE_PRIMER               (KEY_ID_MIN + 808)
#define KEY_TOGGLE_PRIMER1              (KEY_ID_MIN + 809)
#define KEY_TOGGLE_PRIMER2              (KEY_ID_MIN + 810)
#define KEY_TOGGLE_PRIMER3              (KEY_ID_MIN + 811)
#define KEY_TOGGLE_PRIMER4              (KEY_ID_MIN + 812)
#define KEY_ENGINE_FUELFLOW_BUG_POSITION1 (KEY_ID_MIN + 813)
#define KEY_ENGINE_FUELFLOW_BUG_POSITION2 (KEY_ID_MIN + 814)
#define KEY_ENGINE_FUELFLOW_BUG_POSITION3 (KEY_ID_MIN + 815)
#define KEY_ENGINE_FUELFLOW_BUG_POSITION4 (KEY_ID_MIN + 816)
#define KEY_AUTOPILOT_AIRSPEED_HOLD_CURRENT (KEY_ID_MIN + 817)
#define KEY_AUTOPILOT_AIRSPEED_ACQUIRE  (KEY_ID_MIN + 818)
#define KEY_AUTOPILOT_PANEL_AIRSPEED_SET (KEY_ID_MIN + 819)
#define KEY_AUTOPILOT_MACH_HOLD_CURRENT (KEY_ID_MIN + 820)
#define KEY_AUTOPILOT_PANEL_MAX_SPEED   (KEY_ID_MIN + 821)
#define KEY_AUTOPILOT_PANEL_CRUISE_SPEED (KEY_ID_MIN + 822)
#define KEY_TOGGLE_AFTERBURNER1         (KEY_ID_MIN + 823)
#define KEY_TOGGLE_AFTERBURNER2         (KEY_ID_MIN + 824)
#define KEY_TOGGLE_AFTERBURNER3         (KEY_ID_MIN + 825)
#define KEY_TOGGLE_AFTERBURNER4         (KEY_ID_MIN + 826)
#define KEY_TOGGLE_ALTERNATOR1          (KEY_ID_MIN + 827)
#define KEY_TOGGLE_ALTERNATOR2          (KEY_ID_MIN + 828)
#define KEY_TOGGLE_ALTERNATOR3          (KEY_ID_MIN + 829)
#define KEY_TOGGLE_ALTERNATOR4          (KEY_ID_MIN + 830)
#define KEY_VOR1_OBI_FAST_DEC           (KEY_ID_MIN + 831)
#define KEY_VOR1_OBI_FAST_INC           (KEY_ID_MIN + 832)
#define KEY_VOR2_OBI_FAST_DEC           (KEY_ID_MIN + 833)
#define KEY_VOR2_OBI_FAST_INC           (KEY_ID_MIN + 834)
#define KEY_COM_STBY_RADIO_SET          (KEY_ID_MIN + 835)
#define KEY_COM_STBY_RADIO_SWITCH_TO    (KEY_ID_MIN + 836)
#define KEY_COM_RADIO_SWAP              KEY_COM_STBY_RADIO_SWITCH_TO
#define KEY_TOGGLE_ATTITUDE_CAGE        (KEY_ID_MIN + 837)
#define KEY_TOGGLE_MASTER_BATTERY_ALTERNATOR (KEY_ID_MIN + 838)
#define KEY_TOGGLE_GPS_DRIVES_NAV1      (KEY_ID_MIN + 839)
#define KEY_TOGGLE_LOGO_LIGHTS          (KEY_ID_MIN + 840)
#define KEY_TOGGLE_RECOGNITION_LIGHTS   (KEY_ID_MIN + 841)
#define KEY_TOGGLE_WING_LIGHTS          (KEY_ID_MIN + 842)
#define KEY_TOGGLE_NAV_LIGHTS           (KEY_ID_MIN + 843)
#define KEY_HELI_BEEP_INCREASE          (KEY_ID_MIN + 844)
#define KEY_HELI_BEEP_DECREASE          (KEY_ID_MIN + 845)
#define KEY_AXIS_SPOILER_SET            (KEY_ID_MIN + 846)
#define KEY_CONCORDE_NOSE_VISOR_FULL_EXT (KEY_ID_MIN + 847)
#define KEY_CONCORDE_NOSE_VISOR_FULL_RET (KEY_ID_MIN + 848)
#define KEY_LOD_ZOOM_IN                 (KEY_ID_MIN + 849)
#define KEY_LOD_ZOOM_OUT                (KEY_ID_MIN + 850)
#define KEY_AXIS_LEFT_BRAKE_SET         (KEY_ID_MIN + 851)
#define KEY_AXIS_RIGHT_BRAKE_SET        (KEY_ID_MIN + 852)
#define KEY_TOGGLE_AIRCRAFT_EXIT        (KEY_ID_MIN + 853)
#define KEY_TOGGLE_WING_FOLD            (KEY_ID_MIN + 854)
#define KEY_TOGGLE_TAIL_HOOK_HANDLE     (KEY_ID_MIN + 855)
#define KEY_RELEASE_DROP_TANK_ALL       (KEY_ID_MIN + 856)
#define KEY_RELEASE_DROP_TANK_1         (KEY_ID_MIN + 857)
#define KEY_RELEASE_DROP_TANK_2         (KEY_ID_MIN + 858)
#define KEY_MAGNETO_SET                 (KEY_ID_MIN + 863)
#define KEY_MAGNETO1_SET                (KEY_ID_MIN + 864)
#define KEY_MAGNETO2_SET                (KEY_ID_MIN + 865)
#define KEY_MAGNETO3_SET                (KEY_ID_MIN + 866)
#define KEY_MAGNETO4_SET                (KEY_ID_MIN + 867)
#define KEY_PANEL_HUD_NEXT              (KEY_ID_MIN + 868)
#define KEY_TOOLTIP_UNITS_SET           (KEY_ID_MIN + 869)
#define KEY_TOOLTIP_UNITS_TOGGLE        (KEY_ID_MIN + 870)
#define KEY_PANEL_HUD_PREVIOUS          (KEY_ID_MIN + 871)
#define KEY_VIEW_SNAP_PANEL             (KEY_ID_MIN + 877)
#define KEY_VIEW_SNAP_PANEL_RESET       (KEY_ID_MIN + 878)
#define KEY_PAN_RESET_COCKPIT           (KEY_ID_MIN + 879)
#define KEY_PAN_VIEW                    (KEY_ID_MIN + 880)
#define KEY_SNAP_VIEW                   (KEY_ID_MIN + 881)
#define KEY_AXIS_THROTTLE1_SET          (KEY_ID_MIN + 884)
#define KEY_AXIS_PROPELLER1_SET         (KEY_ID_MIN + 885)
#define KEY_AXIS_MIXTURE1_SET           (KEY_ID_MIN + 886)
#define KEY_AXIS_THROTTLE2_SET          (KEY_ID_MIN + 887)
#define KEY_AXIS_PROPELLER2_SET         (KEY_ID_MIN + 888)
#define KEY_AXIS_MIXTURE2_SET           (KEY_ID_MIN + 889)
#define KEY_AXIS_THROTTLE3_SET          (KEY_ID_MIN + 890)
#define KEY_AXIS_PROPELLER3_SET         (KEY_ID_MIN + 891)
#define KEY_AXIS_MIXTURE3_SET           (KEY_ID_MIN + 892)
#define KEY_AXIS_THROTTLE4_SET          (KEY_ID_MIN + 893)
#define KEY_AXIS_PROPELLER4_SET         (KEY_ID_MIN + 894)
#define KEY_AXIS_MIXTURE4_SET           (KEY_ID_MIN + 895)
#define KEY_FLIGHT_MAP                  (KEY_ID_MIN + 896)
#define KEY_LABEL_COLOR_CYCLE           (KEY_ID_MIN + 897)
#define KEY_COM_RADIO_FRACT_DEC_CARRY   (KEY_ID_MIN + 898)
#define KEY_COM_RADIO_FRACT_INC_CARRY   (KEY_ID_MIN + 899)
#define KEY_COM2_RADIO_WHOLE_DEC        (KEY_ID_MIN + 900)
#define KEY_COM2_RADIO_WHOLE_INC        (KEY_ID_MIN + 901)
#define KEY_COM2_RADIO_FRACT_DEC        (KEY_ID_MIN + 902)
#define KEY_COM2_RADIO_FRACT_DEC_CARRY  (KEY_ID_MIN + 903)
#define KEY_COM2_RADIO_FRACT_INC        (KEY_ID_MIN + 904)
#define KEY_COM2_RADIO_FRACT_INC_CARRY  (KEY_ID_MIN + 905)
#define KEY_COM2_RADIO_SET              (KEY_ID_MIN + 906)
#define KEY_COM2_STBY_RADIO_SET         (KEY_ID_MIN + 907)
#define KEY_COM2_RADIO_SWAP             (KEY_ID_MIN + 908)
#define KEY_NAV1_RADIO_FRACT_DEC_CARRY  (KEY_ID_MIN + 909)
#define KEY_NAV1_RADIO_FRACT_INC_CARRY  (KEY_ID_MIN + 910)
#define KEY_NAV1_STBY_SET               (KEY_ID_MIN + 911)
#define KEY_NAV1_RADIO_SWAP             (KEY_ID_MIN + 912)
#define KEY_NAV2_RADIO_FRACT_DEC_CARRY  (KEY_ID_MIN + 913)
#define KEY_NAV2_RADIO_FRACT_INC_CARRY  (KEY_ID_MIN + 914)
#define KEY_NAV2_STBY_SET               (KEY_ID_MIN + 915)
#define KEY_NAV2_RADIO_SWAP             (KEY_ID_MIN + 916)
#define KEY_ADF1_RADIO_TENTHS_DEC       (KEY_ID_MIN + 917)
#define KEY_ADF1_RADIO_TENTHS_INC       (KEY_ID_MIN + 918)
#define KEY_XPNDR_1000_DEC              (KEY_ID_MIN + 919)
#define KEY_XPNDR_100_DEC               (KEY_ID_MIN + 920)
#define KEY_XPNDR_10_DEC                (KEY_ID_MIN + 921)
#define KEY_XPNDR_1_DEC                 (KEY_ID_MIN + 922)
#define KEY_XPNDR_DEC_CARRY             (KEY_ID_MIN + 923)
#define KEY_XPNDR_INC_CARRY             (KEY_ID_MIN + 924)
#define KEY_ADF_FRACT_DEC_CARRY         (KEY_ID_MIN + 925)
#define KEY_ADF_FRACT_INC_CARRY         (KEY_ID_MIN + 926)
#define KEY_COM1_TRANSMIT_SELECT        (KEY_ID_MIN + 927)
#define KEY_COM2_TRANSMIT_SELECT        (KEY_ID_MIN + 928)
#define KEY_COM_RECEIVE_ALL_TOGGLE      (KEY_ID_MIN + 929)
#define KEY_COM_RECEIVE_ALL_SET         (KEY_ID_MIN + 930)
#define KEY_MARKER_SOUND_TOGGLE         (KEY_ID_MIN + 941)
#define KEY_MARKER_SOUND_SET            (KEY_ID_MIN + 942)
#define KEY_ADF_COMPLETE_SET            (KEY_ID_MIN + 943)
#define KEY_ADF_OUTSIDE_SOURCE          (KEY_ID_MIN + 944)
#define KEY_ADF_NEEDLE_SET              (KEY_ID_MIN + 945)
#define KEY_TOGGLE_WATER_RUDDER         (KEY_ID_MIN + 946)
#define KEY_PUSHBACK_SET                (KEY_ID_MIN + 947)
#define KEY_ANTI_ICE_TOGGLE_ENG1        (KEY_ID_MIN + 948)
#define KEY_ANTI_ICE_TOGGLE_ENG2        (KEY_ID_MIN + 949)
#define KEY_ANTI_ICE_TOGGLE_ENG3        (KEY_ID_MIN + 950)
#define KEY_ANTI_ICE_TOGGLE_ENG4        (KEY_ID_MIN + 951)
#define KEY_ANTI_ICE_SET_ENG1           (KEY_ID_MIN + 952)
#define KEY_ANTI_ICE_SET_ENG2           (KEY_ID_MIN + 953)
#define KEY_ANTI_ICE_SET_ENG3           (KEY_ID_MIN + 954)
#define KEY_ANTI_ICE_SET_ENG4           (KEY_ID_MIN + 955)
#define KEY_RELOAD_PANELS               (KEY_ID_MIN + 956)
#define KEY_TOGGLE_FUEL_VALVE_ALL       (KEY_ID_MIN + 957)
#define KEY_TOGGLE_FUEL_VALVE_ENG1      (KEY_ID_MIN + 958)
#define KEY_TOGGLE_FUEL_VALVE_ENG2      (KEY_ID_MIN + 959)
#define KEY_TOGGLE_FUEL_VALVE_ENG3      (KEY_ID_MIN + 960)
#define KEY_TOGGLE_FUEL_VALVE_ENG4      (KEY_ID_MIN + 961)
#define KEY_TUG_HEADING                 (KEY_ID_MIN + 962)
#define KEY_TUG_SPEED                   (KEY_ID_MIN + 963)
#define KEY_CHASE_VIEW_NEXT             (KEY_ID_MIN + 964)
#define KEY_CHASE_VIEW_PREV             (KEY_ID_MIN + 965)
#define KEY_AP_NAV_SELECT_SET           (KEY_ID_MIN + 966)
#define KEY_AXIS_PAN_PITCH              (KEY_ID_MIN + 967)
#define KEY_AXIS_PAN_HEADING            (KEY_ID_MIN + 968)
#define KEY_AXIS_PAN_TILT               (KEY_ID_MIN + 969)
#define KEY_PANEL_ID_TOGGLE             (KEY_ID_MIN + 970)
#define KEY_PANEL_ID_OPEN               (KEY_ID_MIN + 971)
#define KEY_PANEL_ID_CLOSE              (KEY_ID_MIN + 972)
#define KEY_HEADING_BUG_SELECT          (KEY_ID_MIN + 973)
#define KEY_ALTITUDE_BUG_SELECT         (KEY_ID_MIN + 974)
#define KEY_VSI_BUG_SELECT              (KEY_ID_MIN + 975)
#define KEY_CONTROL_RELOAD_USER_AIRCRAFT (KEY_ID_MIN + 976)
#define KEY_ATC_MENU_OPEN               (KEY_ID_MIN + 977)
#define KEY_ATC_MENU_CLOSE              (KEY_ID_MIN + 978)
#define KEY_CHASE_VIEW_TOGGLE           (KEY_ID_MIN + 979)
#define KEY_FUEL_SELECTOR_2_OFF         (KEY_ID_MIN + 980)
#define KEY_FUEL_SELECTOR_2_ALL         (KEY_ID_MIN + 981)
#define KEY_FUEL_SELECTOR_2_LEFT        (KEY_ID_MIN + 982)
#define KEY_FUEL_SELECTOR_2_RIGHT       (KEY_ID_MIN + 983)
#define KEY_FUEL_SELECTOR_2_LEFT_AUX    (KEY_ID_MIN + 984)
#define KEY_FUEL_SELECTOR_2_RIGHT_AUX   (KEY_ID_MIN + 985)
#define KEY_FUEL_SELECTOR_2_CENTER      (KEY_ID_MIN + 986)
#define KEY_FUEL_SELECTOR_2_SET         (KEY_ID_MIN + 987)
#define KEY_EYEPOINT_UP                 (KEY_ID_MIN + 988)
#define KEY_EYEPOINT_DOWN               (KEY_ID_MIN + 989)
#define KEY_EYEPOINT_RIGHT              (KEY_ID_MIN + 990)
#define KEY_EYEPOINT_LEFT               (KEY_ID_MIN + 991)
#define KEY_EYEPOINT_FORWARD            (KEY_ID_MIN + 992)
#define KEY_EYEPOINT_BACK               (KEY_ID_MIN + 993)
#define KEY_EYEPOINT_RESET              (KEY_ID_MIN + 994)
#define KEY_ENGINE_AUTO_SHUTDOWN        (KEY_ID_MIN + 995)
#define KEY_AIRSPEED_BUG_SELECT         (KEY_ID_MIN + 996)
#define KEY_TUG_DISABLE                 (KEY_ID_MIN + 997)
#define KEY_AXIS_FLAPS_SET              (KEY_ID_MIN + 998)
#define KEY_TOGGLE_MASTER_IGNITION_SWITCH (KEY_ID_MIN + 999)
#define KEY_TOGGLE_FEATHER_SWITCHES     (KEY_ID_MIN + 1000)
#define KEY_TOGGLE_FEATHER_SWITCH_1     (KEY_ID_MIN + 1001)
#define KEY_TOGGLE_FEATHER_SWITCH_2     (KEY_ID_MIN + 1002)
#define KEY_TOGGLE_FEATHER_SWITCH_3     (KEY_ID_MIN + 1003)
#define KEY_TOGGLE_FEATHER_SWITCH_4     (KEY_ID_MIN + 1004)
#define KEY_TOGGLE_TAILWHEEL_LOCK       (KEY_ID_MIN + 1005)
#define KEY_ADF_WHOLE_INC               (KEY_ID_MIN + 1006)
#define KEY_ADF_WHOLE_DEC               (KEY_ID_MIN + 1007)
#define KEY_ADF2_100_INC                (KEY_ID_MIN + 1008)
#define KEY_ADF2_10_INC                 (KEY_ID_MIN + 1009)
#define KEY_ADF2_1_INC                  (KEY_ID_MIN + 1010)
#define KEY_ADF2_RADIO_TENTHS_INC       (KEY_ID_MIN + 1011)
#define KEY_ADF2_100_DEC                (KEY_ID_MIN + 1012)
#define KEY_ADF2_10_DEC                 (KEY_ID_MIN + 1013)
#define KEY_ADF2_1_DEC                  (KEY_ID_MIN + 1014)
#define KEY_ADF2_RADIO_TENTHS_DEC       (KEY_ID_MIN + 1015)
#define KEY_ADF2_WHOLE_INC              (KEY_ID_MIN + 1016)
#define KEY_ADF2_WHOLE_DEC              (KEY_ID_MIN + 1017)
#define KEY_ADF2_FRACT_INC_CARRY        (KEY_ID_MIN + 1018)
#define KEY_ADF2_FRACT_DEC_CARRY        (KEY_ID_MIN + 1019)
#define KEY_ADF2_COMPLETE_SET           (KEY_ID_MIN + 1020)
#define KEY_RADIO_ADF2_IDENT_DISABLE    (KEY_ID_MIN + 1021)
#define KEY_RADIO_ADF2_IDENT_ENABLE     (KEY_ID_MIN + 1022)
#define KEY_RADIO_ADF2_IDENT_TOGGLE     (KEY_ID_MIN + 1023)
#define KEY_RADIO_ADF2_IDENT_SET        (KEY_ID_MIN + 1024)
#define KEY_FUEL_SELECTOR_3_OFF         (KEY_ID_MIN + 1025)
#define KEY_FUEL_SELECTOR_3_ALL         (KEY_ID_MIN + 1026)
#define KEY_FUEL_SELECTOR_3_LEFT        (KEY_ID_MIN + 1027)
#define KEY_FUEL_SELECTOR_3_RIGHT       (KEY_ID_MIN + 1028)
#define KEY_FUEL_SELECTOR_3_LEFT_AUX    (KEY_ID_MIN + 1029)
#define KEY_FUEL_SELECTOR_3_RIGHT_AUX   (KEY_ID_MIN + 1030)
#define KEY_FUEL_SELECTOR_3_CENTER      (KEY_ID_MIN + 1031)
#define KEY_FUEL_SELECTOR_3_SET         (KEY_ID_MIN + 1032)
#define KEY_FUEL_SELECTOR_4_OFF         (KEY_ID_MIN + 1033)
#define KEY_FUEL_SELECTOR_4_ALL         (KEY_ID_MIN + 1034)
#define KEY_FUEL_SELECTOR_4_LEFT        (KEY_ID_MIN + 1035)
#define KEY_FUEL_SELECTOR_4_RIGHT       (KEY_ID_MIN + 1036)
#define KEY_FUEL_SELECTOR_4_LEFT_AUX    (KEY_ID_MIN + 1037)
#define KEY_FUEL_SELECTOR_4_RIGHT_AUX   (KEY_ID_MIN + 1038)
#define KEY_FUEL_SELECTOR_4_CENTER      (KEY_ID_MIN + 1039)
#define KEY_FUEL_SELECTOR_4_SET         (KEY_ID_MIN + 1040)
#define KEY_INDUCTOR_COMPASS_REF_INC    (KEY_ID_MIN + 1041)
#define KEY_INDUCTOR_COMPASS_REF_DEC    (KEY_ID_MIN + 1042)
#define KEY_TOGGLE_CABIN_LIGHTS         (KEY_ID_MIN + 1043)
#define KEY_RESET_G_FORCE_INDICATOR     (KEY_ID_MIN + 1044)
#define KEY_RESET_MAX_RPM_INDICATOR     (KEY_ID_MIN + 1045)
#define KEY_MANUAL_FUEL_TRANSFER        (KEY_ID_MIN + 1046)
#define KEY_AP_PITCH_REF_INC_UP         (KEY_ID_MIN + 1047)
#define KEY_AP_PITCH_REF_INC_DN         (KEY_ID_MIN + 1048)
#define KEY_AP_PITCH_REF_SELECT         (KEY_ID_MIN + 1049)
#define KEY_SIM_RESET                   (KEY_ID_MIN + 1050)
#define KEY_ROTOR_BRAKE                 (KEY_ID_MIN + 1051)
#define KEY_ROTOR_CLUTCH_SWITCH_TOGGLE  (KEY_ID_MIN + 1052)
#define KEY_ROTOR_CLUTCH_SWITCH_SET     (KEY_ID_MIN + 1053)
#define KEY_ROTOR_GOV_SWITCH_TOGGLE     (KEY_ID_MIN + 1054)
#define KEY_ROTOR_GOV_SWITCH_SET        (KEY_ID_MIN + 1055)
#define KEY_ROTOR_LATERAL_TRIM_INC      (KEY_ID_MIN + 1056)
#define KEY_ROTOR_LATERAL_TRIM_DEC      (KEY_ID_MIN + 1057)
#define KEY_ROTOR_LATERAL_TRIM_SET      (KEY_ID_MIN + 1058)
#define KEY_CROSS_FEED_OPEN             (KEY_ID_MIN + 1059)
#define KEY_CROSS_FEED_TOGGLE           (KEY_ID_MIN + 1060)
#define KEY_VIRTUAL_COPILOT_TOGGLE      (KEY_ID_MIN + 1061)
#define KEY_VIRTUAL_COPILOT_SET         (KEY_ID_MIN + 1062)
#define KEY_VIRTUAL_COPILOT_ACTION      (KEY_ID_MIN + 1063)
#define KEY_MIXTURE_SET_BEST            (KEY_ID_MIN + 1064)
#define KEY_ADD_FUEL_QUANTITY           (KEY_ID_MIN + 1065)
#define KEY_GPS_POWER_BUTTON            (KEY_ID_MIN + 1066)
#define KEY_GPS_NEAREST_BUTTON          (KEY_ID_MIN + 1068)
#define KEY_GPS_OBS_BUTTON              (KEY_ID_MIN + 1069)
#define KEY_GPS_MSG_BUTTON              (KEY_ID_MIN + 1070)
#define KEY_GPS_MSG_BUTTON_DOWN         (KEY_ID_MIN + 1071)
#define KEY_GPS_MSG_BUTTON_UP           (KEY_ID_MIN + 1072)
#define KEY_GPS_FLIGHTPLAN_BUTTON       (KEY_ID_MIN + 1073)
#define KEY_GPS_VNAV_BUTTON             (KEY_ID_MIN + 1074)
#define KEY_GPS_TERRAIN_BUTTON          (KEY_ID_MIN + 1075)
#define KEY_GPS_PROCEDURE_BUTTON        (KEY_ID_MIN + 1076)
#define KEY_GPS_SETUP_BUTTON            (KEY_ID_MIN + 1077)
#define KEY_GPS_ACTIVATE_BUTTON         (KEY_ID_MIN + 1078)
#define KEY_GPS_ZOOMIN_BUTTON           (KEY_ID_MIN + 1079)
#define KEY_GPS_ZOOMOUT_BUTTON          (KEY_ID_MIN + 1080)
#define KEY_GPS_DIRECTTO_BUTTON         (KEY_ID_MIN + 1081)
#define KEY_GPS_MENU_BUTTON             (KEY_ID_MIN + 1082)
#define KEY_GPS_CLEAR_BUTTON            (KEY_ID_MIN + 1083)
#define KEY_GPS_CLEAR_ALL_BUTTON        (KEY_ID_MIN + 1084)
#define KEY_GPS_CLEAR_BUTTON_DOWN       (KEY_ID_MIN + 1085)
#define KEY_GPS_CLEAR_BUTTON_UP         (KEY_ID_MIN + 1086)
#define KEY_GPS_ENTER_BUTTON            (KEY_ID_MIN + 1087)
#define KEY_GPS_CURSOR_BUTTON           (KEY_ID_MIN + 1088)
#define KEY_GPS_GROUP_KNOB_INC          (KEY_ID_MIN + 1089)
#define KEY_GPS_GROUP_KNOB_DEC          (KEY_ID_MIN + 1090)
#define KEY_GPS_PAGE_KNOB_INC           (KEY_ID_MIN + 1091)
#define KEY_GPS_PAGE_KNOB_DEC           (KEY_ID_MIN + 1092)
#define KEY_GPS_BUTTON1                 (KEY_ID_MIN + 1093)
#define KEY_GPS_BUTTON2                 (KEY_ID_MIN + 1094)
#define KEY_GPS_BUTTON3                 (KEY_ID_MIN + 1095)
#define KEY_GPS_BUTTON4                 (KEY_ID_MIN + 1096)
#define KEY_GPS_BUTTON5                 (KEY_ID_MIN + 1097)
#define KEY_THROTTLE_DECR_SMALL         (KEY_ID_MIN + 1098)
#define KEY_THROTTLE1_DECR_SMALL        (KEY_ID_MIN + 1099)
#define KEY_THROTTLE2_DECR_SMALL        (KEY_ID_MIN + 1100)
#define KEY_THROTTLE3_DECR_SMALL        (KEY_ID_MIN + 1101)
#define KEY_THROTTLE4_DECR_SMALL        (KEY_ID_MIN + 1102)
#define KEY_PROP_PITCH_DECR_SMALL       (KEY_ID_MIN + 1103)
#define KEY_PROP_PITCH1_DECR_SMALL      (KEY_ID_MIN + 1104)
#define KEY_PROP_PITCH2_DECR_SMALL      (KEY_ID_MIN + 1105)
#define KEY_PROP_PITCH3_DECR_SMALL      (KEY_ID_MIN + 1106)
#define KEY_PROP_PITCH4_DECR_SMALL      (KEY_ID_MIN + 1107)
#define KEY_MIXTURE_DECR_SMALL          (KEY_ID_MIN + 1108)
#define KEY_MIXTURE1_DECR_SMALL         (KEY_ID_MIN + 1109)
#define KEY_MIXTURE2_DECR_SMALL         (KEY_ID_MIN + 1110)
#define KEY_MIXTURE3_DECR_SMALL         (KEY_ID_MIN + 1111)
#define KEY_MIXTURE4_DECR_SMALL         (KEY_ID_MIN + 1112)
#define KEY_REPAIR_AND_REFUEL           (KEY_ID_MIN + 1113)
#define KEY_DME_SELECT                  (KEY_ID_MIN + 1114)
#define KEY_FUEL_DUMP_TOGGLE            (KEY_ID_MIN + 1115)
#define KEY_HORN_TRIGGER                (KEY_ID_MIN + 1116)
#define KEY_VIEW_COCKPIT_FORWARD        (KEY_ID_MIN + 1117)
#define KEY_VIEW_VIRTUAL_COCKPIT_FORWARD (KEY_ID_MIN + 1118)
#define KEY_ADVENTURE_ACTION            (KEY_ID_MIN + 1119)
#define KEY_REQUEST_FUEL                (KEY_ID_MIN + 1121)
#define KEY_RELEASE_DROPPABLE_OBJECTS   (KEY_ID_MIN + 1122)
#define KEY_VIEW_PANEL_ALPHA_SET        (KEY_ID_MIN + 1123)
#define KEY_VIEW_PANEL_ALPHA_SELECT     (KEY_ID_MIN + 1124)
#define KEY_VIEW_PANEL_ALPHA_INC        (KEY_ID_MIN + 1125)
#define KEY_VIEW_PANEL_ALPHA_DEC        (KEY_ID_MIN + 1126)
#define KEY_VIEW_LINKING_SET            (KEY_ID_MIN + 1127)
#define KEY_VIEW_LINKING_TOGGLE         (KEY_ID_MIN + 1128)
#define KEY_RADIO_SELECTED_DME_IDENT_ENABLE   (KEY_ID_MIN + 1129)
#define KEY_RADIO_SELECTED_DME_IDENT_DISABLE  (KEY_ID_MIN + 1130)
#define KEY_RADIO_SELECTED_DME_IDENT_SET      (KEY_ID_MIN + 1131)
#define KEY_RADIO_SELECTED_DME_IDENT_TOGGLE   (KEY_ID_MIN + 1132)
#define KEY_FUEL_SELECTOR_LEFT_MAIN     (KEY_ID_MIN + 1133)
#define KEY_FUEL_SELECTOR_2_LEFT_MAIN   (KEY_ID_MIN + 1134)
#define KEY_FUEL_SELECTOR_3_LEFT_MAIN   (KEY_ID_MIN + 1135)
#define KEY_FUEL_SELECTOR_4_LEFT_MAIN   (KEY_ID_MIN + 1136)
#define KEY_FUEL_SELECTOR_RIGHT_MAIN    (KEY_ID_MIN + 1137)
#define KEY_FUEL_SELECTOR_2_RIGHT_MAIN  (KEY_ID_MIN + 1138)
#define KEY_FUEL_SELECTOR_3_RIGHT_MAIN  (KEY_ID_MIN + 1139)
#define KEY_FUEL_SELECTOR_4_RIGHT_MAIN  (KEY_ID_MIN + 1140)
//(KEY_ID_MIN + 1141)
//(KEY_ID_MIN + 1142)
//(KEY_ID_MIN + 1143)
//(KEY_ID_MIN + 1144)
//(KEY_ID_MIN + 1145)
//(KEY_ID_MIN + 1146)
#define KEY_GAUGE_KEYSTROKE                 (KEY_ID_MIN + 1147)
#define KEY_MULTIPLAYER_VOICE_CAPTURE_START (KEY_ID_MIN + 1148)
#define KEY_MULTIPLAYER_VOICE_CAPTURE_STOP  (KEY_ID_MIN + 1149)
#define KEY_SIMUI_WINDOW_HIDESHOW           (KEY_ID_MIN + 1150)
#define KEY_TOGGLE_VARIOMETER_SWITCH        (KEY_ID_MIN + 1151)
#define KEY_TOGGLE_TURN_INDICATOR_SWITCH    (KEY_ID_MIN + 1152)
#define KEY_WINDOW_TITLES_TOGGLE            (KEY_ID_MIN + 1153)
#define KEY_AXIS_INDICATOR_CYCLE            (KEY_ID_MIN + 1154)
#define KEY_MAP_ORIENTATION_CYCLE           (KEY_ID_MIN + 1155)
#define KEY_POINT_OF_INTEREST_TOGGLE_POINTER (KEY_ID_MIN + 1156)
#define KEY_POINT_OF_INTEREST_CYCLE_PREVIOUS (KEY_ID_MIN + 1157)
#define KEY_POINT_OF_INTEREST_CYCLE_NEXT    (KEY_ID_MIN + 1158)
#define KEY_TOGGLE_JETWAY                   (KEY_ID_MIN + 1159)
#define KEY_RETRACT_FLOAT_SWITCH_DEC        (KEY_ID_MIN + 1160)
#define KEY_RETRACT_FLOAT_SWITCH_INC        (KEY_ID_MIN + 1161)
#define KEY_TOGGLE_WATER_BALLAST_VALVE      (KEY_ID_MIN + 1162)
#define KEY_VIEW_CHASE_DISTANCE_ADD         (KEY_ID_MIN + 1163)
#define KEY_VIEW_CHASE_DISTANCE_SUB         (KEY_ID_MIN + 1164)
#define KEY_AVIONICS_MASTER_SET             (KEY_ID_MIN + 1165)
#define KEY_EXTERNAL_SYSTEM_SET             (KEY_ID_MIN + 1166)
#define KEY_EXTERNAL_SYSTEM_TOGGLE          (KEY_ID_MIN + 1167)
#define KEY_APU_STARTER                     (KEY_ID_MIN + 1168)
#define KEY_APU_OFF_SWITCH                  (KEY_ID_MIN + 1169)
#define KEY_APU_GENERATOR_SWITCH_TOGGLE     (KEY_ID_MIN + 1170)
#define KEY_APU_GENERATOR_SWITCH_SET        (KEY_ID_MIN + 1171)
#define KEY_EXTINGUISH_ENGINE_FIRE          (KEY_ID_MIN + 1172)
#define KEY_AP_MAX_BANK_INC                 (KEY_ID_MIN + 1173)
#define KEY_AP_MAX_BANK_DEC                 (KEY_ID_MIN + 1174)
#define KEY_AP_N1_HOLD                      (KEY_ID_MIN + 1175)
#define KEY_HYDRAULIC_SWITCH_TOGGLE         (KEY_ID_MIN + 1176)
#define KEY_DECISION_ALTITUDE_MSL_INC       (KEY_ID_MIN + 1177)
#define KEY_DECISION_ALTITUDE_MSL_DEC       (KEY_ID_MIN + 1178)
#define KEY_BLEED_AIR_SOURCE_CONTROL_INC    (KEY_ID_MIN + 1179)
#define KEY_BLEED_AIR_SOURCE_CONTROL_DEC    (KEY_ID_MIN + 1180)
#define KEY_TURBINE_IGNITION_SWITCH_TOGGLE  (KEY_ID_MIN + 1181)
#define KEY_CABIN_NO_SMOKING_ALERT_SWITCH_TOGGLE (KEY_ID_MIN + 1182)
#define KEY_CABIN_SEATBELTS_ALERT_SWITCH_TOGGLE  (KEY_ID_MIN + 1183)
#define KEY_ANTISKID_BRAKES_TOGGLE          (KEY_ID_MIN + 1184)
#define KEY_GPWS_SWITCH_TOGGLE              (KEY_ID_MIN + 1185)
#define KEY_VIDEO_RECORD_TOGGLE             (KEY_ID_MIN + 1186)
#define KEY_SET_AUTOBRAKE_CONTROL           (KEY_ID_MIN + 1187)
#define KEY_TOGGLE_AIRPORT_NAME_DISPLAY     (KEY_ID_MIN + 1188)
#define KEY_TOGGLE_MASTER_STARTER_SWITCH    (KEY_ID_MIN + 1189)
#define KEY_GEAR_EMERGENCY_HANDLE_TOGGLE    (KEY_ID_MIN + 1190)
#define KEY_AILERON_TRIM_SET                (KEY_ID_MIN + 1195)
#define KEY_RUDDER_TRIM_SET                 (KEY_ID_MIN + 1196)
#define KEY_CAPTURE_SCREENSHOT              (KEY_ID_MIN + 1197)
#define KEY_MOUSE_LOOK_TOGGLE               (KEY_ID_MIN + 1198)
#define KEY_MULTIPLAYER_BROADCAST_VOICE_CAPTURE_START (KEY_ID_MIN + 1199)
#define KEY_MULTIPLAYER_BROADCAST_VOICE_CAPTURE_STOP  (KEY_ID_MIN + 1200)
#define KEY_FLY_BY_WIRE_ELAC_TOGGLE         (KEY_ID_MIN + 1201)
#define KEY_FLY_BY_WIRE_FAC_TOGGLE          (KEY_ID_MIN + 1202)
#define KEY_FLY_BY_WIRE_SEC_TOGGLE          (KEY_ID_MIN + 1203)
#define KEY_MANUAL_FUEL_PRESSURE_PUMP       (KEY_ID_MIN + 1204)
#define KEY_ADF1_RADIO_SWAP                 (KEY_ID_MIN + 1205)
#define KEY_ADF2_RADIO_SWAP                 (KEY_ID_MIN + 1206)
#define KEY_YAXIS_INVERT_TOGGLE             (KEY_ID_MIN + 1207)
#define KEY_LOW_HEIGHT_WARNING_SET            (KEY_ID_MIN + 1210)   //Spelling correction for KEY_LOW_HIGHT_WARNING_SET           
#define KEY_LOW_HEIGHT_WARNING_GAUGE_WILL_SET (KEY_ID_MIN + 1211)   //Spelling correction for KEY_LOW_HIGHT_WARNING_GAUGE_WILL_SET
#define KEY_G1000_PFD_ZOOMIN_BUTTON             (KEY_ID_MIN + 1212)
#define KEY_G1000_PFD_ZOOMOUT_BUTTON            (KEY_ID_MIN + 1213)
#define KEY_G1000_PFD_DIRECTTO_BUTTON           (KEY_ID_MIN + 1214)
#define KEY_G1000_PFD_MENU_BUTTON               (KEY_ID_MIN + 1215)
#define KEY_G1000_PFD_FLIGHTPLAN_BUTTON         (KEY_ID_MIN + 1216)
#define KEY_G1000_PFD_PROCEDURE_BUTTON          (KEY_ID_MIN + 1217)
#define KEY_G1000_PFD_CLEAR_BUTTON              (KEY_ID_MIN + 1218)
#define KEY_G1000_PFD_ENTER_BUTTON              (KEY_ID_MIN + 1219)
#define KEY_G1000_PFD_CURSOR_BUTTON             (KEY_ID_MIN + 1220)
#define KEY_G1000_PFD_GROUP_KNOB_INC            (KEY_ID_MIN + 1221)
#define KEY_G1000_PFD_GROUP_KNOB_DEC            (KEY_ID_MIN + 1222)
#define KEY_G1000_PFD_PAGE_KNOB_INC             (KEY_ID_MIN + 1223)
#define KEY_G1000_PFD_PAGE_KNOB_DEC             (KEY_ID_MIN + 1224)
#define KEY_G1000_PFD_SOFTKEY1                  (KEY_ID_MIN + 1225)
#define KEY_G1000_PFD_SOFTKEY2                  (KEY_ID_MIN + 1226)
#define KEY_G1000_PFD_SOFTKEY3                  (KEY_ID_MIN + 1227)
#define KEY_G1000_PFD_SOFTKEY4                  (KEY_ID_MIN + 1228)
#define KEY_G1000_PFD_SOFTKEY5                  (KEY_ID_MIN + 1229)
#define KEY_G1000_PFD_SOFTKEY6                  (KEY_ID_MIN + 1230)
#define KEY_G1000_PFD_SOFTKEY7                  (KEY_ID_MIN + 1231)
#define KEY_G1000_PFD_SOFTKEY8                  (KEY_ID_MIN + 1232)
#define KEY_G1000_PFD_SOFTKEY9                  (KEY_ID_MIN + 1233)
#define KEY_G1000_PFD_SOFTKEY10                 (KEY_ID_MIN + 1234)
#define KEY_G1000_PFD_SOFTKEY11                 (KEY_ID_MIN + 1235)
#define KEY_G1000_PFD_SOFTKEY12                 (KEY_ID_MIN + 1236)
#define KEY_G1000_MFD_ZOOMIN_BUTTON             (KEY_ID_MIN + 1240)
#define KEY_G1000_MFD_ZOOMOUT_BUTTON            (KEY_ID_MIN + 1241)
#define KEY_G1000_MFD_DIRECTTO_BUTTON           (KEY_ID_MIN + 1242)
#define KEY_G1000_MFD_MENU_BUTTON               (KEY_ID_MIN + 1243)
#define KEY_G1000_MFD_FLIGHTPLAN_BUTTON         (KEY_ID_MIN + 1244)
#define KEY_G1000_MFD_PROCEDURE_BUTTON          (KEY_ID_MIN + 1245)
#define KEY_G1000_MFD_CLEAR_BUTTON              (KEY_ID_MIN + 1246)
#define KEY_G1000_MFD_ENTER_BUTTON              (KEY_ID_MIN + 1247)
#define KEY_G1000_MFD_CURSOR_BUTTON             (KEY_ID_MIN + 1248)
#define KEY_G1000_MFD_GROUP_KNOB_INC            (KEY_ID_MIN + 1249)
#define KEY_G1000_MFD_GROUP_KNOB_DEC            (KEY_ID_MIN + 1250)
#define KEY_G1000_MFD_PAGE_KNOB_INC             (KEY_ID_MIN + 1251)
#define KEY_G1000_MFD_PAGE_KNOB_DEC             (KEY_ID_MIN + 1252)
#define KEY_G1000_MFD_SOFTKEY1                  (KEY_ID_MIN + 1253)
#define KEY_G1000_MFD_SOFTKEY2                  (KEY_ID_MIN + 1254)
#define KEY_G1000_MFD_SOFTKEY3                  (KEY_ID_MIN + 1255)
#define KEY_G1000_MFD_SOFTKEY4                  (KEY_ID_MIN + 1256)
#define KEY_G1000_MFD_SOFTKEY5                  (KEY_ID_MIN + 1257)
#define KEY_G1000_MFD_SOFTKEY6                  (KEY_ID_MIN + 1258)
#define KEY_G1000_MFD_SOFTKEY7                  (KEY_ID_MIN + 1259)
#define KEY_G1000_MFD_SOFTKEY8                  (KEY_ID_MIN + 1260)
#define KEY_G1000_MFD_SOFTKEY9                  (KEY_ID_MIN + 1261)
#define KEY_G1000_MFD_SOFTKEY10                 (KEY_ID_MIN + 1262)
#define KEY_G1000_MFD_SOFTKEY11                 (KEY_ID_MIN + 1263)
#define KEY_G1000_MFD_SOFTKEY12                 (KEY_ID_MIN + 1264)
#define KEY_TOW_PLANE_RELEASE                   (KEY_ID_MIN + 1270)
#define KEY_REQUEST_TOW_PLANE                   (KEY_ID_MIN + 1271)
#define KEY_STEERING_INC                        (KEY_ID_MIN + 1280)
#define KEY_STEERING_DEC                        (KEY_ID_MIN + 1281)
#define KEY_STEERING_SET                        (KEY_ID_MIN + 1282)
#define KEY_APU_EXTINGUISH_FIRE                 (KEY_ID_MIN + 1290)
#define KEY_FREEZE_LATITUDE_LONGITUE_TOGGLE     (KEY_ID_MIN + 1295)
#define KEY_FREEZE_LATITUDE_LONGITUE_SET        (KEY_ID_MIN + 1296)
#define KEY_FREEZE_ALTITUDE_TOGGLE              (KEY_ID_MIN + 1297)
#define KEY_FREEZE_ALTITUDE_SET                 (KEY_ID_MIN + 1298)
#define KEY_PRESSURIZATION_PRESSURE_ALT_INC     (KEY_ID_MIN + 1300)
#define KEY_PRESSURIZATION_PRESSURE_ALT_DEC     (KEY_ID_MIN + 1301)
#define KEY_PRESSURIZATION_CLIMB_RATE_INC       (KEY_ID_MIN + 1302)
#define KEY_PRESSURIZATION_CLIMB_RATE_DEC       (KEY_ID_MIN + 1303)
#define KEY_PRESSURIZATION_CLIMB_RATE_SET       (KEY_ID_MIN + 1304)
#define KEY_PRESSURIZATION_PRESSURE_DUMP_SWITCH (KEY_ID_MIN + 1305)
#define KEY_BAROMETRIC_STD_PRESSURE             (KEY_ID_MIN + 1310)
#define KEY_MULTIPLAYER_PAUSE_SESSION           (KEY_ID_MIN + 1311)
#define KEY_VIEW_CAMERA_SELECT_1                (KEY_ID_MIN + 1315)
#define KEY_VIEW_CAMERA_SELECT_2                (KEY_ID_MIN + 1316)
#define KEY_VIEW_CAMERA_SELECT_3                (KEY_ID_MIN + 1317)
#define KEY_VIEW_CAMERA_SELECT_4                (KEY_ID_MIN + 1318)
#define KEY_VIEW_CAMERA_SELECT_5                (KEY_ID_MIN + 1319)
#define KEY_VIEW_CAMERA_SELECT_6                (KEY_ID_MIN + 1320)
#define KEY_VIEW_CAMERA_SELECT_7                (KEY_ID_MIN + 1321)
#define KEY_VIEW_CAMERA_SELECT_8                (KEY_ID_MIN + 1322)
#define KEY_VIEW_CAMERA_SELECT_9                (KEY_ID_MIN + 1323)
#define KEY_VIEW_CAMERA_SELECT_0                (KEY_ID_MIN + 1324)

//Start of FSX-Xpack events                                                                 
#define KEY_SLING_PICKUP_RELEASE                (KEY_ID_MIN + 1325) 
#define KEY_HOIST_SWITCH_EXTEND                 (KEY_ID_MIN + 1326)
#define KEY_HOIST_SWITCH_RETRACT                (KEY_ID_MIN + 1327)
#define KEY_HOIST_SWITCH_SET                    (KEY_ID_MIN + 1328)           
#define KEY_HOIST_SWITCH_SELECT                 (KEY_ID_MIN + 1329)
#define KEY_HOIST_DEPLOY_TOGGLE                 (KEY_ID_MIN + 1330)           
#define KEY_HOIST_DEPLOY_SET                    (KEY_ID_MIN + 1331)           

#define KEY_TOGGLE_ANTIDETONATION_TANK_VALVE    (KEY_ID_MIN + 1335)
#define KEY_TOGGLE_NITROUS_TANK_VALVE           (KEY_ID_MIN + 1336)

#define KEY_TAKEOFF_ASSIST_ARM_TOGGLE           (KEY_ID_MIN + 1340)
#define KEY_TAKEOFF_ASSIST_ARM_SET              (KEY_ID_MIN + 1341)
#define KEY_TAKEOFF_ASSIST_FIRE                 (KEY_ID_MIN + 1342)
#define KEY_TOGGLE_LAUNCH_BAR_SWITCH            (KEY_ID_MIN + 1343)
#define KEY_SET_LAUNCH_BAR_SWITCH               (KEY_ID_MIN + 1344)
#define KEY_SET_TAIL_HOOK_HANDLE                (KEY_ID_MIN + 1345)
#define KEY_SET_WING_FOLD                       (KEY_ID_MIN + 1346)

#define KEY_TOGGLE_RACERESULTS_WINDOW           (KEY_ID_MIN + 1347)

#define KEY_BLEED_AIR_SOURCE_CONTROL_SET        (KEY_ID_MIN + 1350)
#define KEY_FUEL_DUMP_SWITCH_SET                (KEY_ID_MIN + 1351)

// Third parties can use custom events in this range to communicate between 3D VC's and 2D C++ gauges.
#define THIRD_PARTY_EVENT_ID_MIN        		0x00011000
#define THIRD_PARTY_EVENT_ID_MAX        		0x0001FFFF

//
// Common Flight Simulator Data Types
//

typedef void            *PVOID;
typedef PVOID           *PPVOID;

typedef unsigned    char    UINT8, *PUINT8, **PPUINT8;
typedef signed      char    SINT8, *PSINT8, **PPSINT8;
typedef unsigned    short   UINT16, *PUINT16, **PPUINT16;
typedef signed      short   SINT16, *PSINT16, **PPSINT16;
typedef signed      int     SINT32, *PSINT32, **PPSINT32;
typedef unsigned    char    VAR8, *PVAR8, **PPVAR8;
typedef unsigned    short   VAR16, *PVAR16, **PPVAR16;
typedef unsigned    int     VAR32, *PVAR32, **PPVAR32;

#ifndef _BASETSD_H_
typedef unsigned    int     UINT32, *PUINT32;
#endif
typedef unsigned    int     **PPUINT32;


#ifndef _BASETSD_H_ 
typedef unsigned __int64 UINT64, *PUINT64, **PPUINT64;
#endif

typedef signed __int64 SINT64, *PSINT64, **PPSINT64;

// 64-bit generic structure
typedef struct  VAR64
{
    VAR32   lo;
    VAR32   hi;
} VAR64, *PVAR64, **PPVAR64;

typedef struct  UIF64
{
    UINT32  f;
    UINT32  i;
} UIF64, *PUIF4, **PPUIF64;

typedef struct  SVAR64
{
    UINT32  lo;
    SINT32  hi;
} SVAR64, *PSVAR64, **PPSVAR64;

// boolean variables - hold TRUE/FALSE
typedef int     BOOL, *PBOOL, **PPBOOL;
typedef VAR8    BOOL8, *PBOOL8, **PPBOOL8;
typedef VAR16   BOOL16, *PBOOL16, **PPBOOL16;
typedef VAR32   BOOL32, *PBOOL32, **PPBOOL32;

// array of 1-bit boolean variables
typedef int     FLAGS, *PFLAGS, **PPFLAGS;
typedef VAR8    FLAGS8, *PFLAGS8, **PPFLAGS8;
typedef VAR16   FLAGS16, *PFLAGS16, **PPFLAGS16;
typedef VAR32   FLAGS32, *PFLAGS32, **PPFLAGS32;

// Enumeration variables - domain is subset of all possible values
typedef int     ENUM, *PENUM, **PPENUM;
typedef VAR8    ENUM8, *PENUM8, **PPENUM8;
typedef VAR16   ENUM16, *PENUM16, **PPENUM16;
typedef VAR32   ENUM32, *PENUM32, **PPENUM32;

// array index variables
typedef int     INDX, *PINDX, **PPINDX;
typedef VAR8    INDX8, *PINDX8, **PPINDX8;
typedef VAR16   INDX16, *PINDX16, **PPINDX16;
typedef VAR32   INDX32, *PINDX32, **PPINDX32;

// Identifier variables
typedef int     ID, *PID, **PPID;
typedef VAR8    ID8, *PID8, **PPID8;
typedef VAR16   ID16, *PID16, **PPID16;
typedef VAR32   ID32, *PID32, **PPID32;

// Error return variables
typedef VAR32   ERR, *PERR, **PPERR;

// Fractional variables
typedef UINT8   UFRAC8, *PUFRAC8, **PPUFRAC8;    // { 0..+1}
typedef SINT8   SFRAC8, *PSFRAC8, **PPSFRAC8;    // {-1..+1}
typedef UINT16  UFRAC16, *PUFRAC16, **PPUFRAC16; // { 0..+1}
typedef SINT16  SFRAC16, *PSFRAC16, **PPSFRAC16; // {-1..+1}
typedef UINT32  UFRAC32, *PUFRAC32, **PPUFRAC32; // { 0..+1}
typedef SINT32  SFRAC32, *PSFRAC32, **PPSFRAC32; // {-1..+1}

// Floating-point types
typedef double  FLOAT64, *PFLOAT64, **PPFLOAT64;
typedef float   FLOAT32, *PFLOAT32, **PPFLOAT32;

// Integer/Fraction structures both signed & unsigned
typedef UINT16  UIF16, *PUIF16, **PPUIF16;

typedef SINT16  SIF16, *PSIF16, **PPSIF16;

typedef UINT32  UIF32, *PUIF32, **PPUIF32;

typedef SINT32  SIF32, *PSIF32, **PPSIF32;

#ifdef _H2INC
typedef struct  UIF48
{
    UINT16  pad;
    UINT16  f;
    UINT32  i;
} UIF48, *PUIF48, **PPUIF48;

typedef struct  SIF48
{
    UINT16  pad;
    UINT16  f;
    SINT32  i;
} SIF48, *PSIF48, **PPSIF48;
#else
typedef union   UIF48
{
    struct
    {
        UINT16  pad;
        UINT16  f;
        UINT32  i;
    };

    UINT64      i64;

} UIF48, *PUIF48, **PPUIF48;

typedef union   SIF48
{
    struct
    {
        UINT16  pad;
        UINT16  f;
        SINT32  i;
    };

    SINT64      i64;

} SIF48, *PSIF48, **PPSIF48;
#endif


typedef SVAR64  SIF64, *PSIF64, **PPSIF64;

#if !defined(_WINNT_)               // NOT PORTABLE!
typedef char    CHAR;
typedef char    *PCHAR;             // single character
#endif
typedef PCHAR   *PPCHAR;            // pointer to pointer to char
typedef CHAR    STRING, *PSTRING, **PPSTRING;       // generic string of unknown length
typedef CHAR    STRINGZ, *PSTRINGZ, **PPSTRINGZ;    // generic string of unknown length, zero term
typedef const CHAR  *PCSTRINGZ, **PPCSTRINGZ;   // generic string of unknown length, zero term

typedef struct SUBSTRING
{
    PSTRINGZ    string;
    UINT32      leng;
}   SUBSTRING, *PSUBSTRING, **PPSUBSTRING;

typedef VAR16   BCD16, *PBCD16, **PPBCD16;      // 4-digit BCD number (0000-9999)
typedef VAR16   BCO16, *PBCO16, **PPBCO16;      // 4-digit Octal number (0000-7777) (in BCD)
typedef VAR32   BCD32, *PBCD32, **PPBCD32;      // 8-digit BCD number (00000000-99999999)
typedef VAR32   BCO32, *PBCO32, **PPBCO32;      // 8-digit Octal number (00000000-77777777) (in BCD)

// pseudodegree variables
typedef VAR8    ANGL8, *PANGL8, **PPANGL8;
typedef VAR16   ANGL16, *PANGL16, **PPANGL16;
typedef VAR32   ANGL32, *PANGL32, **PPANGL32;
#ifdef _H2INC
typedef struct  ANGL48
{
    UINT16  pad;
    UINT16  lo;
    UINT32  hi;
} ANGL48, *PANGL48, **PPANGL48;
#else
typedef union   ANGL48
{
    struct
    {
        UINT16  pad;
        UINT16  lo;
        UINT32  hi;
    };

    UINT64      i64;

} ANGL48, *PANGL48, **PPANGL48;
#endif

typedef ANGL48  SINT48, *PSINT48, **PPSINT48;

// radians
typedef FLOAT64 RADIANS;

// counter variables
typedef VAR8    CNTR8, *PCNTR8, **PPCNTR8;
typedef VAR16   CNTR16, *PCNTR16, **PPCNTR16;
typedef VAR32   CNTR32, *PCNTR32, **PPCNTR32;

// ring counter variables - modified by shifting instead of add/sub
typedef VAR8    RCNTR8, *PRCNTR8, **PPRCNTR8;
typedef VAR16   RCNTR16, *PRCNTR16, **PPRCNTR16;
typedef VAR32   RCNTR32, *PRCNTR32, **PPRCNTR32;

// Spare/Unused variables
typedef VAR8    SPARE8;
typedef VAR16   SPARE16;
typedef VAR32   SPARE32;

// version number data type
typedef struct  APP_VERSION
{
    UINT32  major;
    UINT32  minor;
    UINT32  build;
} APP_VERSION, *PAPP_VERSION, **PPAPP_VERSION;

// LatLon - used to store a position in 2D space
typedef struct
{
    SIF48       lat;
    ANGL48      lon;
} LATLON, *PLATLON, **PPLATLON;

// LatLon32 - used to store a position in 2D space (less accuracy)
typedef struct
{
    SINT32      lat;
    ANGL32      lon;
} LATLON32, *PLATLON32, **PPLATLON32;

// LatLonAlt - used to store a position in 3D space
typedef struct  LATLONALT
{
    SIF48       lat;
    ANGL48      lon;
    SIF48       alt;
} LATLONALT, *PLATLONALT, **PPLATLONALT;

typedef struct
{
    SINT32      lat;                // 32 bits of LAT (Meters)
    ANGL32      lon;                // 32 bits of LON
    SINT32      alt;                // 32 bits of ALT (Meters)
} LATLONALT32, *PLATLONALT32, **PPLATLONALT32;

typedef struct  _latlonalt_float32
{
    FLOAT32     lat;
    FLOAT32     lon;
    FLOAT32     alt;
} LATLONALT_FLOAT32, *PLATLONALT_FLOAT32, **PPLATLONALT_FLOAT32;

typedef struct _llf64               // llf64
{
    FLOAT64     lat;                // degrees (+/-90)
    FLOAT64     lon;                // degrees (+/-180)
} LLF64, *PLLF64, **PPLLF64;

typedef struct _llaf64              // llaf64
{
    FLOAT64     lat;                // degrees (+/-90)
    FLOAT64     lon;                // degrees (+/-180)
    FLOAT64     alt;                // meters (+/- a lot)
} LLAF64, *PLLAF64, **PPLLAF64;

// ZXY48 - used to mirror LatLonAlt with Lon as meters IIII.FF
typedef struct  ZXY48
{
    SIF48       z;
    SIF48       x;
    SIF48       y;
} ZXY48, *PZXY48, **PPZXY48;




// FLOAT64_VECTOR3 - <i,j,k> vector, hopefully normalized
typedef struct  _FLOAT64_VECTOR3
{
    FLOAT64     i ;
    FLOAT64     j ;
    FLOAT64     k ;
}   FLOAT64_VECTOR3, *PFLOAT64_VECTOR3 ;

// FLOAT64_VECTOR2 - <i,j> vector
typedef struct  _FLOAT64_VECTOR2
{
    FLOAT64     i ;
    FLOAT64     j ;
}   FLOAT64_VECTOR2, *PFLOAT64_VECTOR2 ;

// PBH32 & PBH16 - rotation parameters of an object

typedef struct  PBH32
{
    ANGL32  pitch;
    ANGL32  bank;
    ANGL32  heading;
} PBH32, *PPBH32, **PPPBH32;

typedef struct  PBH16
{
    ANGL16  pitch;
    ANGL16  bank;
    ANGL16  heading;
} PBH16, *PPBH16, **PPPBH16;


// LATLONBOX - a set of lat and lon values that defines a rectangular boundary
typedef struct LATLONBOX
{
    SIF48   top;
    ANGL48  left;
    SIF48   bottom;
    ANGL48  right;
} LATLONBOX, *PLATLONBOX, **PPLATLONBOX;

// LATLONRECT32 - a set of lat and lon values that defines a rectangular boundary
typedef struct LATLONRECT32
{
    SINT32  north;
    SINT32  south;
    ANGL32  east;
    ANGL32  west;
} LATLONRECT32, *PLATLONRECT32, **PPLATLONRECT32;

// LATLONRECT32 - a set of lat and lon values that defines a rectangular boundary
typedef struct LATLONRECTF64
{
    FLOAT64 north;
    FLOAT64 south;
    FLOAT64 east;
    FLOAT64 west;
} LATLONRECTF64, *PLATLONRECTF64, **PPLATLONRECTF64;

typedef struct LLAPBH
{
    LATLONALT   lla;
    PBH32       pbh;
}  LLAPBH, *PLLAPBH, **PPLLAPBH;

// LATLONALTPBH - Combination of LATLONALT & PBH32
typedef union LATLONALTPBH
{
    struct
    {
        SIF48       lat;
        ANGL48      lon;
        SIF48       alt;
        ANGL32      pitch;
        ANGL32      bank;
        ANGL32      heading;
    };

    struct
    {
        LATLONALT   lla;
        PBH32       pbh;
    };
} LATLONALTPBH, *PLATLONALTPBH, **PPLATLONALTPBH;

// LATLONALTTPO - Time, Position, Orientation using LatLonAlt for Position
typedef struct  LATLONALTTPO
{
    UINT32      time;

    union
    {
        struct
        {
            SIF48       lat;
            ANGL48      lon;
            SIF48       alt;
            ANGL32      pitch;
            ANGL32      bank;
            ANGL32      heading;
        };

        struct
        {
            LATLONALT   lla;
            PBH32       pbh;
        };

        LATLONALTPBH    llapbh;
    };
} LATLONALTTPO, *PLATLONALTTPO, **PPLATLONALTTPO;

// Data structures for BGL manipulation
typedef struct LLA2416
{
    char    stream[3+3+2];
} LLA2416, *PLLA2416, **PPLLA2416;

typedef struct LLA2424
{
    char    stream[3+3+3];
} LLA2424, *PLLA2424, **PPLLA2424;

// coordinates of a point in a cartesion system
typedef struct  XYZ16
{
    SINT16  x;
    SINT16  y;
    SINT16  z;
} XYZ16, *PXYZ16, **PPXYZ16;

typedef struct  XYZ32
{
    SINT32  x;
    SINT32  y;
    SINT32  z;
} XYZ32, *PXYZ32, **PPXYZ32;

typedef struct  _xyz_float32
{
    FLOAT32     x;
    FLOAT32     y;
    FLOAT32     z;
} XYZ_FLOAT32, *PXYZ_FLOAT32, **PPXYZ_FLOAT32;


// Same as above, but uses east, altit, north for field names
typedef struct  EAN32
{
    SINT32  east;
    SINT32  altit;
    SINT32  north;
} EAN32, *PEAN32, **PPEAN32;

// Coordinates of a point on the XZ plane

typedef struct  XZF32
{
    FLOAT32 x;
    FLOAT32 z;
} XZF32, *PXZF32, **PPXZF32;

typedef struct  XZ32
{
    SINT32  x;
    SINT32  z;
} XZ32, *PXZ32, **PPXZ32;

// same as above, but uses east, north for field names

typedef struct  EN32
{
    SINT32  east;
    SINT32  north;
} EN32, *PEN32, **PPEN32;

// ANGLSINCOS16 structure to hold data for an angle and it's trig functions
typedef struct  ANGLSINCOS16
{
    ANGL16  angle;
    SFRAC16 sine;
    SFRAC16 cosine;
} ANGLSINCOS16, *PANGLSINCOS16, **PPANGLSINCOS16;

// XYZPBH - defines the position of an object
typedef union   XYZPBH
{
    struct
    {
        SINT32  x;
        SINT32  y;
        SINT32  z;
        ANGL16  pitch;
        ANGL16  bank;
        ANGL16  heading;
    };

    struct
    {
        XYZ32   xyz;
        PBH16   pbh;
    };
} XYZPBH, *PXYZPBH, **PPXYZPBH;

// XYZTPO - same as above, but starts with a time-stamp field
// Notes: these structures will need to be reordered when
//  H2INc is fixed!
typedef struct  XYZTPO
{
    UINT32  time;

    union
    {
        struct
        {
            SINT32  x;
            SINT32  y;
            SINT32  z;
            ANGL16  pitch;
            ANGL16  bank;
            ANGL16  heading;
        };

        struct
        {
            XYZ32   xyz;
            PBH16   pbh;
        };

        XYZPBH  xyzpbh;
    };
} XYZTPO, *PXYZTPO, **PPXYZTPO;


// XYZF64_  - XYZ point(vector) in floating point
// XYZF64 - XYZ vector which includes XYZF64 as a part of union
// (needed to be changed together with XYZF64)

struct  XYZF64_
{
    FLOAT64 x;
    FLOAT64 y;
    FLOAT64 z;
};

//  XYZF64, *PXYZF64, **PPXYZF64;

typedef struct  XYZF64
{
    union
    {
        struct { FLOAT64   lon; FLOAT64 alt;      FLOAT64 lat; };
        struct { FLOAT64     x; FLOAT64  y;       FLOAT64   z; };
        struct { FLOAT64 pitch; FLOAT64 heading;  FLOAT64 bank;};
    };
}
XYZF64, *PXYZF64, **PPXYZF64,
POS3_FLOAT64,
VEL3_FLOAT64, *PVEL3_FLOAT64, **PPVEL3_FLOAT64,
ACC3_FLOAT64,
ROT3_FLOAT64,
ROV3_FLOAT64,
ROA3_FLOAT64;

//-----------------------------------------------


// XYF64 - XY point (vector in 2d space) in floating point
typedef struct XYF64                // xyf64
{
    FLOAT64 x;
    FLOAT64 y;
} XYF64, *PXYF64, **PPXYF64;


//----------------------------------------------------
// XYZF32 - XYZW point(vector) in floating point
typedef struct XYZF32 {
    FLOAT32 x;
    FLOAT32 y;
    FLOAT32 z;
    FLOAT32 w;
}   XYZF32, *PXYZF32;

#ifdef __cplusplus
struct CXYZF32:public XYZF32
{
    CXYZF32() { }
    CXYZF32(FLOAT32 _x, FLOAT32 _y, FLOAT32 _z, FLOAT32 _w=1.0f) {x = _x; y = _y; z = _z; w = _w;}
};
#endif
//---------------------------------------------------


typedef FLOAT64 ANGLF64;    // radians

// PBHF64 - Pitch/bank/heading in floating point radians
typedef struct PBHF64 {
    ANGLF64 pitch;
    ANGLF64 bank;
    ANGLF64 heading;
} PBHF64, *PPBHF64, **PPPBHF64;

// FLOAT32 matrix
typedef struct MATRIXF32
{
    union
    {
        struct { XYZF32 x,y,z,w;};
        struct
        {
            FLOAT32 m00, m01, m02, m03;
            FLOAT32 m10, m11, m12, m13;
            FLOAT32 m20, m21, m22, m23;
            FLOAT32 m30, m31, m32, m33;
        };
        struct
        {
            FLOAT32 _11, _12, _13, _14;
            FLOAT32 _21, _22, _23, _24;
            FLOAT32 _31, _32, _33, _34;
            FLOAT32 _41, _42, _43, _44;
        };
        FLOAT32 m[4][4];
        FLOAT32 mat[4][4];
    };
#ifdef __cplusplus
    FLOAT32& operator()(int iRow, int iColumn) { return m[iRow][iColumn]; }
    const FLOAT32& operator()(int iRow, int iColumn) const { return m[iRow][iColumn]; }
#endif
} MATRIXF32, *PMATRIXF32;

// map ROTMTRXF64 to a MATRIXF32
typedef MATRIXF32  ROTMTRXF64, *PROTMTRXF64;
// map QUATERN_ROTMAT to a MATRIXF32
typedef MATRIXF32  QUATERN_ROTMAT, *PQUATERN_ROTMAT;

//  2D data structures

typedef SINT32  UNIPIX, *PUNIPIX, **PPUNIPIX;   // universal pixel (X=0-8191    Y=0-6143}
typedef SINT32  PIXEL, *PPIXEL, **PPPIXEL;      // screen pixel
typedef VAR32  UNICOL, *PUNICOL, **PPUNICOL;    // universal color code

typedef struct  RGB15
{
    UINT16      blue:5;
    UINT16      green:5;
    UINT16      red:5;
    UINT16      alpha:1;
} RGB15, *PRGB15, **PPRGB15;

typedef struct  RGB16
{
    UINT16      blue:5;
    UINT16      green:6;
    UINT16      red:5;
} RGB16, *PRGB16, **PPRGB16;


typedef struct  RGB24
{
    UINT8       blue;
    UINT8       green;
    UINT8       red;
} RGB24, *PRGB24, **PPRGB24;

typedef struct  RGB32
{
    UINT8       blue;
    UINT8       green;
    UINT8       red;
    UINT8       junk;
} RGB32, *PRGB32, **PPRGB32;

typedef struct  RGBA
{
    UINT8       blue;
    UINT8       green;
    UINT8       red;
    UINT8       alpha;
} RGBA, *PRGBA, **PPRGBA;

typedef struct  RGB         // this is used by the 2d system and stuff
{                           // note that red and blue are backwards from
    UINT8       red;        // what windows expects
    UINT8       green;
    UINT8       blue;
} RGB, *PRGB, **PPRGB;

typedef struct  RGBAF
{
    FLOAT32     r;
    FLOAT32     g;
    FLOAT32     b;
    FLOAT32     a;
} RGBAF, *PRGBAF;

typedef struct  XY
{
    SINT32  x;
    SINT32  y;
} XY, *PXY, **PPXY;

typedef struct  UNIPOINT
{
    UNIPIX  x;
    UNIPIX  y;
} UNIPOINT, *PUNIPOINT, **PPUNIPOINT;

typedef struct  UNIBOX
{
    UNIPIX  x;
    UNIPIX  y;
    UNIPIX  size_x;
    UNIPIX  size_y;
} UNIBOX, *PUNIBOX, **PPUNIBOX;

typedef struct  UNIRECT
{
    UNIPIX  left;
    UNIPIX  top;
    UNIPIX  right;
    UNIPIX  bottom;
} UNIRECT, *PUNIRECT, **PPUNIRECT;

typedef struct  PIXPOINT
{
    PIXEL x;
    PIXEL y;
} PIXPOINT, *PPIXPOINT, **PPPIXPOINT;

typedef const PIXPOINT *PCPIXPOINT, **PPCPIXPOINT;

typedef struct  PIXBOX
{
    PIXEL x;
    PIXEL y;
    PIXEL size_x;
    PIXEL size_y;
} PIXBOX, *PPIXBOX, **PPPIXBOX;

typedef struct  PIXRECT
{
    PIXEL left;
    PIXEL top;
    PIXEL right;
    PIXEL bottom;
} PIXRECT, *PPIXRECT, **PPPIXRECT;


typedef struct  BOUNDSRECT
{
    SIF48   N;                  //  north boundary
    SIF48   S;                  //  south boundary
    ANGL48  E;                  // east boundary
    ANGL48  W;                  // west boundary

} BOUNDSRECT, *PBOUNDSRECT, **PPBOUNDSRECT;


typedef struct
{
    SINT32  north;              // north boundary
    SINT32  south;              // south boundary
    SINT32  east;               // east boundary
    SINT32  west;               // west boundary

} BOUNDSRECT32, *PBOUNDSRECT32, **PPBOUNDSRECT32;


// LLA_INFO - this struct contains a LATLONALT param and a user definable PVOID ptr
//  useful for an array of LATLONALTs that you also want to associate some additional data with
typedef struct  LLA_INFO
{
    union
    {
        struct
        {
            SIF48       lat;
            ANGL48      lon;
            SIF48       alt;
        };
        LATLONALT   lla;
    };
    PVOID               data_ptr;
} LLA_INFO, *PLLA_INFO, **PPLLA_INFO;


// enum constants for IMAGE.format
typedef enum IMG_FORMAT
{
    IMG_8_BIT_MONOCHROME = 0,
    IMG_8_BIT_INDEXED,
    IMG_15_BIT,             // 1555
    IMG_16_BIT,             // 565
    IMG_16A_BIT,            // 4444
    IMG_24_BIT,             // 888
    IMG_32_BIT,             // 888
    IMG_32A_BIT,            // 8888
    IMG_DXT1,               // DirectX Texture Compression DXT1
    IMG_DXT3,               // DirectX Texture Compression DXT3
    IMG_DUDV,               // Pertubation data
    IMG_MAX                 // keep this last
} IMG_FORMAT, *PIMG_FORMAT, **PPIMG_FORMAT;

// IMAGE - structure containing info pertaining to an offscreen image buffer
//         NOTE this structure should not change size compared to the FS98
//         version because gauges use this structure
typedef struct  IMAGE
{
    IMG_FORMAT  format:16;  // bit format for this image
    UINT32      flags:16;   // flags for this image
    PIXPOINT    dim;        // offscreen buffer size (viewable area)
    UINT32      pitch;      // offset to add to advance one scan line in image buffer
    RGBA*       palette;    // palette for IMG_8_BIT_INDEXED images (was rowtbl)
    UINT32      len;        // xxx
    PCHAR       image;      // pointer to image buffer
    PVOID       pdx;        // pointer to directx information
} IMAGE, *PIMAGE, **PPIMAGE;

typedef const IMAGE *PCIMAGE, **PPCIMAGE;

#define IMAGE_XY(p,x,y) ((BYTE *)(p)->image + ((p)->pitch * (y)) + (x))
#define IMAGE_PTR(p,y)  IMAGE_XY(p,0,y)

// View Dir enum for select_view
typedef enum    VIEW_DIR {
    VIEW_DIR_FORWARD,
    VIEW_DIR_FORWARD_RIGHT,
    VIEW_DIR_RIGHT,
    VIEW_DIR_REAR_RIGHT,
    VIEW_DIR_REAR,
    VIEW_DIR_REAR_LEFT,
    VIEW_DIR_LEFT,
    VIEW_DIR_FORWARD_LEFT,
    VIEW_DIR_DOWN,
    VIEW_DIR_FORWARD_UP,
    VIEW_DIR_FORWARD_RIGHT_UP,
    VIEW_DIR_RIGHT_UP,
    VIEW_DIR_REAR_RIGHT_UP,
    VIEW_DIR_REAR_UP,
    VIEW_DIR_REAR_LEFT_UP,
    VIEW_DIR_LEFT_UP,
    VIEW_DIR_FORWARD_LEFT_UP,
    VIEW_DIR_UP,
    VIEW_DIR_AUX_00,
    VIEW_DIR_AUX_01,
    VIEW_DIR_AUX_02,
    VIEW_DIR_AUX_03,
    VIEW_DIR_AUX_04,
    VIEW_DIR_AUX_05,
    VIEW_DIR_MAX
} VIEW_DIR, *PVIEW_DIR, **PPVIEW_DIR;


// BOUNDBOX - this structure is used extensively by the FrontEnd system, and by the symbolic map
typedef struct BOUNDBOX
{
    SIF32   N;      // northern latitude boundary
    SIF32   S;      // southern latitude boundary
    ANGL32  E;      // eastern longitude boundary
    ANGL32  W;      // western longitude boundary
} BOUNDBOX, *PBOUNDBOX, **PPBOUNDBOX;

typedef struct _engine_fuel_vars
{
    ENUM        tank_selector;          //what tanks are selected to draw from
    FLAGS       tanks_used;             //what tanks are actually being drawn from
    UINT32      number_of_tanks_used;   //# of tanks being drawn from
    FLOAT64     fuelflow_pph;           //lbs/hr
    BOOL32      fuel_available;         //True or False in selected tanks

} ENG_FUEL_VARS, *PENG_FUEL_VARS, **PPENG_FUEL_VARS;

typedef struct  ENG
{
    SINT16      throttle_lvr_pos;       // throttle position (0-16384=>0-100%throttle position)
    UINT16      propeller_lvr_pos;      // propeller lever position (0-16384=>0-100%prop setting)
    UINT16      mixture_lvr_pos;        // mixture position (0-16384=>0-100%mixture position)
    ENUM16      starter;                // starter switch position

    BOOL16      combustion;             // combustion flag (combustion = TRUE)
    UINT16      n1_rpm;                 // gas producer RPM (0-16384=>0-100%RPM)
    UINT16      n2_rpm;                 // power RPM (0-16384=>0-100%RPM)
    UINT16      ff_pph;                 // fuel flow (0-16384=>0-100%)PPH (normalized to Lear 3500 lbs thrust)
    UINT32      ff_pph_actual;          // fuel flow (0-16384=>0-100%)PPH
    UINT16      ff_pph_ssl;             // fuel flow (0-16384=>0-100%)PPH, Standard sea level conditions
    UINT16      torque;                 // torque produced by engine (FT*LBS)
    UINT16      absorbed_torque;        // torque absorbed by propeller (FT*LBS)
    SINT32      thrust_lbs;             // thrust (LBS)
    ANGL16      blade_angle_pdeg;       // propeller blade angle (PDEG)

    BOOL16      lf_mag;                 // left magneto status (activated = 1)
    BOOL16      rt_mag;                 // right magneto status (activated = 1)
    BOOL16      anti_ice;               // anti ice switch
    UINT16      epr_bug_pos;            // epr governed engine setting (0-16384=>0-100%epr setting)
    UINT16      egt_bug_pos;            // egt governed engine setting (0-16384=>0-100%egt setting)

    UINT16      oil_tmp;                // oil temperature (degC) * 16384
    UINT16      oil_prs;                // oil pressure (PSI) * 16384
    UINT16      epr;                    // engine pressure ratio (0-65536=>0-6.4)
    UINT16      egt_degC;               // exhaust gas temperature (degC)
    UINT16      manifold_pressure;      // manifold pressure (0-65536=>0-64)

    SINT16      x_pos;                  // lateral engine position (IN)
    SINT16      y_pos;                  // vertical engine position (IN)
    SINT16      z_pos;                  // longitudinal engine position (IN)

    UINT16      rpm_scaler;             // *16384 gives actual prop rpm in dx (= 65535 for jets)
    PVOID       ext_ptr;                // engine structure extension pointer

    UINT32      oil_quantity;           //oil quantity (0-16384 => 0-100%)
    UINT32      engine_vibration;       //oil quantity (0-16384 => 0-5)
    UINT32      hydraulic_pressure;     //hydraulic pressure (0-16384 => 0-4 psi)
    UINT32      hydraulic_quantity;     //hydraulic quantity (0-16384 => 0-100%)
    FLOAT64     induced_velocity;       //induced velocity from propwash or jet blast

    FLOAT64     cyl_head_temp_degF;     //cylinder head temperature (deg F)

    UINT32      tot_ind;                // TOT * 16384, deg C
    UINT32      torque_ind;             // TQ * 16384, %
    UINT32      Wfpress_ind;            // Fuel Pressure * 16384, psi
    UINT32      electric_load;          // electrical load * 16384, % max load
    UINT32      xmsn_pressure;          // xmsn pressure * 16384, psi
    UINT32      xmsn_temperature;       // transmission temp * 16384, deg C
    UINT32      rotor_rpm;              // main rotor speed * 16384, % max

    ENG_FUEL_VARS   fuel_vars;

} ENG, *PENG, **PPENG;


// mouse sensitivity structure
typedef struct  SENSEBLOCK
{
    PIXEL       left;
    PIXEL       top;
    PIXEL       right;
    PIXEL       bottom;
    PVOID       routine;
} SENSEBLOCK, *PSENSEBLOCK, **PPSENSEBLOCK;

typedef enum    XVOR_TYPE
{
    VOR_UNKNOWN         = 0,
    VOR_TERMINAL        = 1,
    VOR_LOW_ALTITUDE    = 2,
    VOR_LOW_ALT         = 2,
    VOR_HIGH_ALTITUDE   = 3,
    VOR_HIGH_ALT        = 3,
    VOR_ILS             = 4,
    VOR_VOT             = 5,
} XVOR_TYPE, *PXVOR_TYPE, **PPXVOR_TYPE;


typedef struct
{
    ENUM32      code;               // code value
    PSTRINGZ    text;               // text value
} LOOKUP_TABLE, *PLOOKUP_TABLE, **PPLOOKUP_TABLE;

typedef struct _xyz_minmax
{
    XYZF32 xyzMin;
    XYZF32 xyzMax;
} XYZ_MINMAX, *PXYZ_MINMAX, **PPXYZ_MINMAX;


typedef struct _min_max
{
    FLOAT32 fMin;
    FLOAT32 fMax;
} MIN_MAX, *PMIN_MAX, **PPMIN_MAX;


typedef struct _uv
{
    FLOAT32 u;
    FLOAT32 v;
} UV, *PUV, **PPUV;

typedef struct _uv_info
{
    UV uv[2];
} UV_INFO, *PUV_INFO, **PPUV_INFO;


// fuel_tank_selector enum values
typedef enum    FUEL_SELECT
{
    FUEL_TANK_SELECTOR_OFF = 0,
    FUEL_TANK_SELECTOR_ALL = 1,
    FUEL_TANK_SELECTOR_LEFT = 2,
    FUEL_TANK_SELECTOR_RIGHT = 3,
    FUEL_TANK_SELECTOR_LEFT_AUX = 4,
    FUEL_TANK_SELECTOR_RIGHT_AUX = 5,
    FUEL_TANK_SELECTOR_CENTER = 6,
    FUEL_TANK_SELECTOR_CENTER2 = 7,
    FUEL_TANK_SELECTOR_CENTER3 = 8,
    FUEL_TANK_SELECTOR_EXTERNAL1 = 9,
    FUEL_TANK_SELECTOR_EXTERNAL2 = 10,
    FUEL_TANK_SELECTOR_RIGHT_TIP = 11,
    FUEL_TANK_SELECTOR_LEFT_TIP = 12,
    FUEL_TANK_SELECTOR_CROSSFEED = 13,
    FUEL_TANK_SELECTOR_CROSSFEED_L2R = 14,
    FUEL_TANK_SELECTOR_CROSSFEED_R2L = 15,
    FUEL_TANK_SELECTOR_BOTH = 16,
    FUEL_TANK_SELECTOR_EXTERNAL_ALL = 17,
    FUEL_TANK_SELECTOR_ISOLATE = 18,

    FUEL_TANK_MAXIMUM
} FUEL_SELECT, *PFUEL_SELECT, **PPFUEL_SELECT;

// cross feed enum values
typedef enum    CROSS_FEED_SELECT
{
    CROSS_FEED_OFF = 0,
    CROSS_FEED_LEFT_TO_RIGHT,
    CROSS_FEED_RIGHT_TO_LEFT,

    CROSS_FEED_MAX
} CROSS_FEED, *PCROSS_FEED, **PPCROSS_FEED;

// prop type enum values
typedef enum PROP_TYPE
{
    PROP_TYPE_NONE,
    PROP_TYPE_FIXED,
    PROP_TYPE_MANUAL,
    PROP_TYPE_CHOICE
} PROP_TYPE;

// units enum values
typedef enum UNITS_OF_MEASURE
{
    ENGLISH_UNITS,
    METRIC_UNITS_ALT_FEET,
    METRIC_UNITS_ALT_METER
} UNITS_OF_MEASURE;

//
//  Panel System types and defines
//

//  Token list
//

typedef enum    GAUGE_TOKEN
{
    MODULE_VAR_NONE = 0,
    TICK18,                     //  inc'd at 18hz. used for general purpose timing
    GROUND_ALTITUDE,            //  ground level altitude (1/256 meter/unit)
    CLOCK_HOUR,                 //  local clock time, hours {0..23}
    CLOCK_MINUTE,               //  local clock time, minutes {0..59}
    CLOCK_SECOND,               //  local clock time, seconds {0..59}
    ZULU_HOUR,                  //  delta time from GMT
    ZULU_MINUTE,                //  delta time from GMT
    ZULU_DAY,                   //  GMT day of year (1 - 365/366)
    ZULU_YEAR,                  //  GMT year
    PANEL_LIGHTS,               //  TRUE=panel lighting on
    STROBE_LIGHTS,              //  TRUE=strobe lights on
    STROBE_FLASH,               //  TRUE=strobe flash on model & screen
    PITOT_HEAT,                 //  TRUE=pitot heat on
    LANDING_LIGHTS,             //  TRUE=landing lights on
    SMOKE_ENABLE,               //  TRUE=smoke system active
    ADF_FREQUENCY,              //  |
    ADF_500_HZ_TUNABLE,         //  |   ADF frequency
    ADF_EXTENDED_FREQUENCY,     //  |
    ADF_IDENTITY,
    ADF_SIGNAL_STRENGTH,
    ADF_MORSE_IDENT,
    ADF_CARD_RADIAL,            //  integer degrees 0-360
    COM_FREQUENCY,              //  BCD COM radio freq
    NAV1_FREQUENCY,             //  BCD NAV1 radio freq
    NAV1_VOR_RADIAL,
    NAV1_MORSE_IDENT,
    NAV2_FREQUENCY,             //  BCD NAV2 radio freq
    NAV2_VOR_RADIAL,
    NAV2_MORSE_IDENT,
    DME1_DISPLAY_TYPE,          //  FALSE=distance readout,  TRUE=speed in kts readout
    DME1_DISTANCE,              //  DME1 distance readout
    DME1_SPEED,                 //  DME1 speed readout
    DME1_MORSE_IDENT,
    DME2_DISPLAY_TYPE,          //  FALSE=distance readout,  TRUE=speed in kts readout
    DME2_DISTANCE,              //  DME2 distance readout
    DME2_SPEED,                 //  DME2 speed readout
    DME2_MORSE_IDENT,
    TRANSPONDER_CODE,           //  Octal Transponder code {0000 - 7777}
    ADF_NEEDLE,                 //  16 bit pseudodegrees
    VOR1_NEEDLE,                // +-127 correspond to +-10 if VOR or to +-2.5 if localiser.
    VOR1_GS_NEEDLE,             // +-120 correspond to +-0.7
    VOR1_BACK_COURSE_FLAGS,     //  Status of VOR1 (see BC_FLAG_ in FS6DEF.H)
    VOR1_TF_FLAG,
    VOR1_GS_FLAG,
    VOR1_OBI,
    VOR1_CODE,
    VOR1_IDENTITY,
    VOR1_MAG_VAR,
    VOR1_SIGNAL_STRENGTH,
    VOR2_NEEDLE,
    VOR2_BACK_COURSE_FLAGS,     //  Status of VOR2 (see BC_FLAG_ in FS6DEF.H)
    VOR2_TF_FLAG,
    VOR2_GS_FLAG,
    VOR2_OBI,
    VOR2_CODE,
    VOR2_IDENTITY,
    VOR2_MAG_VAR,
    VOR2_SIGNAL_STRENGTH,
    VOR2_BEARING_DEGREES,
    AIRCRAFT_ON_GROUND,         //  TRUE=aircraft is on the ground
    AIRSPEED,                   //  Airspeed - Indicated or True based on GLOBALP->indicated_aspd
    AIRSPEED_TRUE_CALIBRATE,    //  Degrees rotation of the tas calibration card
    BARBER_POLE_ASPD,           //  Movable Red-line airspeed  {1/128th knot/unit}
    MACH,                       //  mach, 64k=3.2 mach (would be better as UIF32)
    VERTICAL_SPEED,             //  Velocity Up, vertical speed {1/256th m/sec}
    MACH_MAX_OPERATE,
    STALL_WARNING,              //  TRUE=5 knots before stall
    OVERSPEED_WARNING,          //  TRUE=overspeed warning
    TURN_COORDINATOR_BALL_POS,  //  turn coordinator ball position {-127..0..127}
    DELTA_HEADING_RATE,
    PLANE_LATITUDE,             //  current position of our aircraft
    PLANE_LONGITUDE,            //  current position of our aircraft
    PLANE_ALTITUDE,             //  current position of our aircraft
    ALT_FROM_BAROMETRIC_PRESSURE,
    KOHLSMAN_SETTING_MB,        //  knob position millibars
    KOHLSMAN_SETTING_HG,        //  knob position inches of hg
    PLANE_PITCH_DEGREES,
    ATTITUDE_INDICATOR_PITCH_DEGREES,
    PLANE_BANK_DEGREES,
    ATTITUDE_INDICATOR_BANK_DEGREES,
    PLANE_HEADING_DEGREES_MAGNETIC,
    MAGNETIC_VAR,               //  magnetic north vs. true north angular error
    WHISKEY_COMPASS_DEGREES,    //  True degrees
    PLANE_HEADING_DEGREES_GYRO,
    GYRO_DRIFT_ERROR,
    PLANE_HEADING_DEGREES_TRUE,
    AIRCRAFT_AUTOPILOT_AVAILABLE,
    AIRCRAFT_FLAPS_AVAILABLE,
    AIRCRAFT_STALL_HORN_AVAILABLE,
    AIRCRAFT_ENGINE_MIXTURE_AVAILABLE,
    AIRCRAFT_CARB_HEAT_AVAILABLE,
    AIRCRAFT_SPOILER_AVAILABLE,
    AIRCRAFT_IS_TAIL_DRAGGER,
    AIRCRAFT_STROBES_AVAILABLE,
    AIRCRAFT_PROP_TYPE_AVAILABLE,   //  PROP_TYPE_FIXED, PROP_TYPE_MANUAL, PROP_TYPE_AUTOMATIC
    AIRCRAFT_TOE_BRAKES_AVAILABLE,
    AIRCRAFT_NAV1_AVAILABLE,
    AIRCRAFT_NAV2_AVAILABLE,
    AIRCRAFT_NAV1_OBS_AVAILABLE,
    AIRCRAFT_NAV2_OBS_AVAILABLE,
    AIRCRAFT_VOR2_GAUGE_AVAILABLE,
    AIRCRAFT_GYRO_DRIFT_AVAILABLE,  // fek: may not be used for bug #2131 but making other changes...build build build
    AUTOPILOT_ACTIVE,           //  autopilot master on/off
    AUTOPILOT_WING_LEVELER,     //  wing leveler on/off
    AUTOPILOT_NAV1_LOCK,        //  nav 1 lock on/off
    AUTOPILOT_HEADING_LOCK,     //  heading lock on/off
    AUTOPILOT_HEADING_LOCK_DIR, //  heading lock direction
    AUTOPILOT_ALTITUDE_LOCK,    //  altitude lock on/off
    AUTOPILOT_ALTITUDE_LOCK_VAR,//  altitude lock altitude in appropriate units
    AUTOPILOT_ATTITUDE_HOLD,    //  attitude hold
    AUTOPILOT_GLIDESLOPE_HOLD,  //  glideslope hold
    AUTOPILOT_APROACH_HOLD,     //  approach hold
    AUTOPILOT_BACKCOURSE_HOLD,  //  backcourse hold
    AUTOPILOT_YAW_DAMPER,       //  yaw damper
    AUTOPILOT_AIRSPEED_HOLD,    //  airspeed hold
    AUTOPILOT_AIRSPEED_HOLD_VAR,//  airspeed hold var
    AUTOPILOT_MACH_HOLD,        //  airspeed hold
    AUTOPILOT_MACH_HOLD_VAR,    //  airspeed hold var
    AUTOPILOT_VERTICAL_HOLD,    //  vertical hold
    AUTOPILOT_VERTICAL_HOLD_VAR,//  vertical speed hold var in appropriate units
    AUTOPILOT_ALTITUDE_MANUALLY_TUNABLE,
    AUTOPILOT_HEADING_MANUALLY_TUNABLE,
    AUTOPILOT_AUTO_THROTTLE_ARM,
    AUTOPILOT_TAKEOFF_POWER_ACTIVE,
    ENGINE_RPM_HOLD,
    ENGINE_CONTROL_SELECT,              //  which engine is being controlled
    ENGINE1,
    ENGINE1_THROTTLE_LEVER_POS, //  throttle position           (16k = 100% throttle position)
    ENGINE1_PROPELLER_LEVER_POS,    //  propeller lever position    (16k = 100% prop setting)
    ENGINE1_MIXTURE_LEVER_POS,      //  mixture position            (16k = 100% mixture position)
    ENGINE1_COMBUSTION,         //  combustion flag             (combustion = TRUE)
    ENGINE1_STARTER_SWITCH_POS, //  starter switch position
    ENGINE1_MAGNETO_LEFT,           //  left magneto status         (activated = 1)
    ENGINE1_MAGNETO_RIGHT,          //  right magneto status        (activated = 1)
    ENGINE1_N1_RPM,             //  gas producer (RPM)          (16k = 100% RPM)
    ENGINE1_N2_RPM,             //  power (RPM)                 (16k = 100% RPM)
    ENGINE1_FF_GPH,
    ENGINE1_FF_PPH,             //  fuel flow PPH
    ENGINE1_FF_PPH_SSL,         //  fuel flow (PPH)             (16k = 100%, Standard sea level conditions)
    ENGINE1_TORQUE,             //  torque produced ( FT*LBS )
    ENGINE1_ANTI_ICE_SWITCH,        //  anti ice switch
    ENGINE1_PRESSURE_RATIO,     //  engine pressure ratio       (65k = 6.4)
    ENGINE1_PRESSURE_BUG_POS,       //  epr governed engine setting (16k = 100% epr setting)
    ENGINE1_EGT,                    //  exhaust gas temp (degC)     (16k = 860 deg)
    ENGINE1_EGT_BUG_POS,            //  egt governed engine setting (16k = 100% egt setting)
    ENGINE1_CHT,                    //  cylinder head temperature
    ENGINE1_OIL_TEMPERATURE,        //  oil temperature (degC)      (16k = 140 deg)
    ENGINE1_OIL_PRESSURE,           //  oil pressure (PSI)          (16k = 55 PSI)
    ENGINE1_OIL_QUANTITY,           //  oil quantity                (16k = 100%)
    ENGINE1_HYDRAULIC_PRESSURE, //  hydraulic pressure (PSI)    (16k = 4 psi)
    ENGINE1_HYDRAULIC_QUANTITY, //  hydraulic quantity          (16k = 100%)
    ENGINE1_MANIFOLD_PRESSURE,      //  manifold pressure           (65k = 64)
    ENGINE1_ENGINE_VIBRATION,       //  oil quantity                (16k = 5)
    ENGINE1_RPM_SCALER,         //  *16384 gives actual prop rpm in dx (= 65535 for jets)
    ENGINE1_TURBINE_TEMPERATURE,    //  (DegC * 16384)
    ENGINE1_TORQUE_PERCENT,         //  (FT/LBS * 16384)
    ENGINE1_FUEL_PRESSURE,              //  (PSI * 16384)
    ENGINE1_ELECTRICAL_LOAD,            //  (Max Load * 16384)
    ENGINE1_TRANSMISSION_PRESSURE,      //  (PSI * 16384)
    ENGINE1_TRANSMISSION_TEMPERATURE,   //  (DegC * 16384)
    ENGINE1_ROTOR_RPM,                  //  (Max 8 16384)
    ENGINE2,
    ENGINE2_THROTTLE_LEVER_POS, //  throttle position (0-16384=>0-100%throttle position)
    ENGINE2_PROPELLER_LEVER_POS,//  propeller lever position (0-16384=>0-100%prop setting)
    ENGINE2_MIXTURE_LEVER_POS,  //  mixture position (0-16384=>0-100%mixture position)
    ENGINE2_COMBUSTION,         //  combustion flag (combustion = TRUE)
    ENGINE2_STARTER_SWITCH_POS, //  starter switch position
    ENGINE2_MAGNETO_LEFT,       //  left magneto status (activated = 1)
    ENGINE2_MAGNETO_RIGHT,      //  right magneto status (activated = 1)
    ENGINE2_N1_RPM,             //  gas producer RPM (0-16384=>0-100%RPM)
    ENGINE2_N2_RPM,             //  power RPM (0-16384=>0-100%RPM)
    ENGINE2_FF_GPH,
    ENGINE2_FF_PPH,             //  PPH
    ENGINE2_FF_PPH_SSL,         //  fuel flow (0-16384=>0-100%)PPH, Standard sea level conditions
    ENGINE2_TORQUE,             //  torque produced ( FT*LBS )
    ENGINE2_ANTI_ICE_SWITCH,    //  anti ice switch
    ENGINE2_PRESSURE_RATIO,     //  engine pressure ratio (0-65536=>0-6.4)
    ENGINE2_PRESSURE_BUG_POS,   //  epr governed engine setting (0-16384=>0-100%epr setting)
    ENGINE2_EGT,                //  exhaust gas temperature (degC)
    ENGINE2_EGT_BUG_POS,        //  egt governed engine setting (0-16384=>0-100%egt setting)
    ENGINE2_CHT,                //  cylinder head temperature
    ENGINE2_OIL_TEMPERATURE,    //  oil temperature (degC)
    ENGINE2_OIL_PRESSURE,       //  oil pressure (PSI)
    ENGINE2_OIL_QUANTITY,       //  oil quantity (0-16384 => 0-100%)
    ENGINE2_HYDRAULIC_PRESSURE, //  hydraulic pressure (0-16384 => 0-4 psi)
    ENGINE2_HYDRAULIC_QUANTITY, //  hydraulic quantity (0-16384 => 0-100%)
    ENGINE2_MANIFOLD_PRESSURE,  //  manifold pressure (0-65536=>0-64)
    ENGINE2_ENGINE_VIBRATION,   //  oil quantity (0-16384 => 0-5)
    ENGINE2_RPM_SCALER,         //  *16384 gives actual prop rpm in dx (= 65535 for jets)
    ENGINE2_TURBINE_TEMPERATURE,        //  (DegC * 16384)
    ENGINE2_TORQUE_PERCENT,             //  (FT/LBS * 16384)
    ENGINE2_FUEL_PRESSURE,              //  (PSI * 16384)
    ENGINE2_ELECTRICAL_LOAD,            //  (Max Load * 16384)
    ENGINE2_TRANSMISSION_PRESSURE,      //  (PSI * 16384)
    ENGINE2_TRANSMISSION_TEMPERATURE,   //  (DegC * 16384)
    ENGINE2_ROTOR_RPM,                  //  (Max 8 16384)
    ENGINE3,
    ENGINE3_THROTTLE_LEVER_POS, //  throttle position (0-16384=>0-100%throttle position)
    ENGINE3_PROPELLER_LEVER_POS,//  propeller lever position (0-16384=>0-100%prop setting)
    ENGINE3_MIXTURE_LEVER_POS,  //  mixture position (0-16384=>0-100%mixture position)
    ENGINE3_COMBUSTION,         //  combustion flag (combustion = TRUE)
    ENGINE3_STARTER_SWITCH_POS, //  starter switch position
    ENGINE3_MAGNETO_LEFT,       //  left magneto status (activated = 1)
    ENGINE3_MAGNETO_RIGHT,      //  right magneto status (activated = 1)
    ENGINE3_N1_RPM,             //  gas producer RPM (0-16384=>0-100%RPM)
    ENGINE3_N2_RPM,             //  power RPM (0-16384=>0-100%RPM)
    ENGINE3_FF_GPH,
    ENGINE3_FF_PPH,             //  fuel flow PPH
    ENGINE3_FF_PPH_SSL,         //  fuel flow (0-16384=>0-100%)PPH, Standard sea level conditions
    ENGINE3_TORQUE,             //  torque produced ( FT*LBS )
    ENGINE3_ANTI_ICE_SWITCH,    //  anti ice switch
    ENGINE3_PRESSURE_RATIO,     //  engine pressure ratio (0-65536=>0-6.4)
    ENGINE3_PRESSURE_BUG_POS,   //  epr governed engine setting (0-16384=>0-100%epr setting)
    ENGINE3_EGT,                //  exhaust gas temperature (degC)
    ENGINE3_EGT_BUG_POS,        //  egt governed engine setting (0-16384=>0-100%egt setting)
    ENGINE3_CHT,                //  cylinder head temperature
    ENGINE3_OIL_TEMPERATURE,    //  oil temperature (degC)
    ENGINE3_OIL_PRESSURE,       //  oil pressure (PSI)
    ENGINE3_OIL_QUANTITY,       //  oil quantity (0-16384 => 0-100%)
    ENGINE3_HYDRAULIC_PRESSURE, //  hydraulic pressure (0-16384 => 0-4 psi)
    ENGINE3_HYDRAULIC_QUANTITY, //  hydraulic quantity (0-16384 => 0-100%)
    ENGINE3_MANIFOLD_PRESSURE,  //  manifold pressure (0-65536=>0-64)
    ENGINE3_ENGINE_VIBRATION,   //  oil quantity (0-16384 => 0-5)
    ENGINE3_RPM_SCALER,         //  *16384 gives actual prop rpm in dx (= 65535 for jets)
    ENGINE3_TURBINE_TEMPERATURE,        //  (DegC * 16384)
    ENGINE3_TORQUE_PERCENT,             //  (FT/LBS * 16384)
    ENGINE3_FUEL_PRESSURE,              //  (PSI * 16384)
    ENGINE3_ELECTRICAL_LOAD,            //  (Max Load * 16384)
    ENGINE3_TRANSMISSION_PRESSURE,      //  (PSI * 16384)
    ENGINE3_TRANSMISSION_TEMPERATURE,   //  (DegC * 16384)
    ENGINE3_ROTOR_RPM,                  //  (Max 8 16384)
    ENGINE4,
    ENGINE4_THROTTLE_LEVER_POS, //  throttle position (0-16384=>0-100%throttle position)
    ENGINE4_PROPELLER_LEVER_POS,//  propeller lever position (0-16384=>0-100%prop setting)
    ENGINE4_MIXTURE_LEVER_POS,  //  mixture position (0-16384=>0-100%mixture position)
    ENGINE4_COMBUSTION,         //  combustion flag (combustion = TRUE)
    ENGINE4_STARTER_SWITCH_POS, //  starter switch position
    ENGINE4_MAGNETO_LEFT,       //  left magneto status (activated = 1)
    ENGINE4_MAGNETO_RIGHT,      //  right magneto status (activated = 1)
    ENGINE4_N1_RPM,             //  gas producer RPM (0-16384=>0-100%RPM)
    ENGINE4_N2_RPM,             //  power RPM (0-16384=>0-100%RPM)
    ENGINE4_FF_GPH,
    ENGINE4_FF_PPH,             //  fuel flow PPH
    ENGINE4_FF_PPH_SSL,         //  fuel flow (0-16384=>0-100%)PPH, Standard sea level conditions
    ENGINE4_TORQUE,             //  torque produced ( FT*LBS )
    ENGINE4_ANTI_ICE_SWITCH,    //  anti ice switch
    ENGINE4_PRESSURE_RATIO,     //  engine pressure ratio (0-65536=>0-6.4)
    ENGINE4_PRESSURE_BUG_POS,   //  epr governed engine setting (0-16384=>0-100%epr setting)
    ENGINE4_EGT,                //  exhaust gas temperature (degC)
    ENGINE4_EGT_BUG_POS,        //  egt governed engine setting (0-16384=>0-100%egt setting)
    ENGINE4_CHT,                //  cylinder head temperature
    ENGINE4_OIL_TEMPERATURE,    //  oil temperature (degC)
    ENGINE4_OIL_PRESSURE,       //  oil pressure (PSI)
    ENGINE4_OIL_QUANTITY,       //  oil quantity (0-16384 => 0-100%)
    ENGINE4_HYDRAULIC_PRESSURE, //  hydraulic pressure (0-16384 => 0-4 psi)
    ENGINE4_HYDRAULIC_QUANTITY, //  hydraulic quantity (0-16384 => 0-100%)
    ENGINE4_MANIFOLD_PRESSURE,  //  manifold pressure (0-65536=>0-64)
    ENGINE4_ENGINE_VIBRATION,   //  oil quantity (0-16384 => 0-5)
    ENGINE4_RPM_SCALER,         //  *16384 gives actual prop rpm in dx (= 65535 for jets)
    ENGINE4_TURBINE_TEMPERATURE,        //  (DegC * 16384)
    ENGINE4_TORQUE_PERCENT,             //  (FT/LBS * 16384)
    ENGINE4_FUEL_PRESSURE,              //  (PSI * 16384)
    ENGINE4_ELECTRICAL_LOAD,            //  (Max Load * 16384)
    ENGINE4_TRANSMISSION_PRESSURE,      //  (PSI * 16384)
    ENGINE4_TRANSMISSION_TEMPERATURE,   //  (DegC * 16384)
    ENGINE4_ROTOR_RPM,                  //  (Max 8 16384)
    NUMBER_OF_ENGINES,                  //  1=single 2=twin ...
    PROP_ADVANCE_SELECTION,             //  0=fixed 1=optimal 2=manual
    THROTTLE_LOWER_LIMIT,               //  set by sim for thrust reversers
    OIL_QUANTITY_TOTAL,                 //  0..7fff (8 qt.)
    SYSTEMS_AVAILABLE,                  //  OBSOLETE!
    INSTRUMENTS_AVAILABLE,              //  OBSOLETE
    PP_ADF,                             //  partial panel variables
    PP_AIR,
    PP_ALTIM,
    PP_ATTITUDE,
    PP_COM,
    PP_COMPASS,
    PP_ELECT,                           // there is no electricity in panels circuit
    PP_ENGINE,
    PP_FUEL_IND,
    PP_HEADING,
    PP_VERT_VEL,
    PP_XPNDR,
    PP_NAV,
    PP_PITOT,
    PP_TURN_COORD,
    PP_VACUUM,
    FUEL_TANK_CENTER_LEVEL,
    FUEL_TANK_CENTER_CAPACITY,
    FUEL_TANK_LEFT_MAIN_LEVEL,
    FUEL_TANK_LEFT_MAIN_CAPACITY,
    FUEL_TANK_LEFT_AUX_LEVEL,
    FUEL_TANK_LEFT_AUX_CAPACITY,
    FUEL_TANK_LEFT_TIP_LEVEL,
    FUEL_TANK_LEFT_TIP_CAPACITY,
    FUEL_TANK_RIGHT_MAIN_LEVEL,
    FUEL_TANK_RIGHT_MAIN_CAPACITY,
    FUEL_TANK_RIGHT_AUX_LEVEL,
    FUEL_TANK_RIGHT_AUX_CAPACITY,
    FUEL_TANK_RIGHT_TIP_LEVEL,
    FUEL_TANK_RIGHT_TIP_CAPACITY,
    FUEL_QUANTITY_RIGHT,        //  Total Gallons
    FUEL_QUANTITY_LEFT,         //  Total Gallons
    FUEL_QUANTITY_CENTER,       //  Total Gallons
    FUEL_WEGHT_PER_GALLON,      //  in pounds I.F  (6 lbs/gal = 1536)
    FUEL_TANK_SELECTOR,         //  see FUEL_TANK_SELECTOR_ enum values
    CROSS_FEED_SELECTOR,        //  OFF, LEFT_TO_RIGHT, or RIGHT_TO_LEFT
    OBSOLETE_YOKE_POS_Y,        // use ELEVATOR_DEFLECTION
    OBSOLETE_YOKE_IND_Y,        // use ELEVATOR_DEFLECTION
    OBSOLETE_YOKE_POS_X,        // use AILERON_DEFLECTION
    OBSOLETE_YOKE_IND_X,        // use AILERON_DEFLECTION
    OBSOLETE_AILERON_POS,       // use AILERON_DEFLECTION
    OBSOLETE_RUDDER_PEDAL_POS,  // use RUDDER_DEFLECTION
    OBSOLETE_RUDDER_PEDAL_IND,  // use RUDDER_DEFLECTION
    OBSOLETE_RUDDER_POS,        // use RUDDER_DEFLECTION
    OBSOLETE_ELEVATOR_POS,      // use ELEVATOR_DEFLECTION
    OBSOLETE_ELEVATOR_TRIM_POS, // use ELEVATOR_TRIM
    OBSOLETE_ELEVATOR_TRIM_IND, // use ELEVATOR_TRIM
    BRAKES_LEFT_POS,            //  0=off 32k=full brakes
    BRAKES_RIGHT_POS,           //  0=off 32k=full brakes
    BRAKE_IND,                  //  0=off 16k=full brakes
    PARKING_BRAKE_POS,          //  0=off 32k=full  parking brakes
    SPOILERS_ARMED,
    SPOILERS_HANDLE_POS,
    SPOILERS_POS_LEFT,
    SPOILERS_POS_RIGHT,
    FLAPS_HANDLE_POS,
    FLAPS_POS_LEFT,
    FLAPS_POS_RIGHT,
    GEAR_HANDLE_POS,
    GEAR_POS_NOSE,
    GEAR_POS_LEFT,
    GEAR_POS_RIGHT,
    DISPLAY_UNITS,              //  english vs. metric system
    SIM_SPEED,                  //  time dilation factor for sim equations
    ZOOM_FACTOR,                //  zoom value of current window in real units
    REALISM_SENSE,
    REALISM_SYSTEMS,
    RELIABILITY_SENSE,          //  0 = unreliable - 100 = totally reliable
    AMBIENT_TEMP_DEGREES_C,     //
    AMBIENT_PRES_MBAR,          //  Millibars * 16
    AMBIENT_WIND_VEL,           //
    AMBIENT_WIND_DIR,           //
    AMBIENT_WINDE,              //  MPS x(east) wind component
    AMBIENT_WINDN,              //  MPS z(up) wind component
    AMBIENT_WINDA,              //  MPS y(north) wind component
    BAROMETRIC_PRESSURE,        //  barometric pressure experienced
    ANGLE_OF_ATTACK_INDICATOR,
    TOTAL_AIR_TEMP,
    G_FORCE,
    CHARGING_AMPS,              //  charging system amps - 1 amp=65535
    SUCTION_PRESSURE,           //  in hg
    MARKER_BEACON_STATE,        //  0=no beacon, 1=outer, 2=middle, 3=inner
    CURRENT_DME,                //  DMECUR_DME1 or DMECUR_DME2
    PANEL_STATE_FLAGS,
    GET_GUN_AMMO,
    GET_CANNON_AMMO,
    GET_ROCKET_AMMO,
    GET_BOMB_AMMO,
    ENGINE_PRIMER,              // 0.0 -> 1.0 == in -> out
    COWL_FLAPS,                 // 0.0 -> 1.0 == closed -> open
    MASTER_ALTERNATOR,          // 0.0 -> 1.0 == off -> on
    MASTER_BATTERY,             // 0.0 -> 1.0 == off -> on
    CARB_HEAT,                  // 0.0 -> 1.0 == off -> on
    FLIGHT_TIME,                // in 1/10 of hours
    FUEL_PUMP,                  // 0.0 -> 1.0 == off -> on
    BEACON_LIGHTS,              // 0.0 -> 1.0 == off -> on
    TAXI_LIGHTS,                // 0.0 -> 1.0 == off -> on
    RADIO_STACK_VISIBLE,        // 0.0 -> 1.0 == off -> on
    HORIZON_BAR_OFFSET,         // -10 deg to + 10 deg
    CURRENT_DME_DISTANCE,
    CURRENT_DME_SPEED,
    CURRENT_DME_MIN,
    WARNING_FUEL,
    WARNING_FUEL_L,
    WARNING_FUEL_R,
    WARNING_VACUUM,
    WARNING_VACUUM_L,
    WARNING_VACUUM_R,
    WARNING_OIL_PSI,
    WARNING_VOLTAGE,
    FUEL_TANK_CENTER2_LEVEL,
    FUEL_TANK_CENTER2_CAPACITY,
    FUEL_QUANTITY_CENTER2,                       // Total Gallons
    TOTAL_FUEL_QUANTITY_GALLONS,
    SELECTED_FUEL_QUANTITY_GALLONS,
    SELECTED_FUEL_QUANTITY_LITERS,
    SELECTED_FUEL_QUANTITY_PERCENT,
    RECIP_ENGINE1_ENGINE_RPM,
    RECIP_ENGINE1_MANIFOLD_PRESSURE,
    RECIP_ENGINE1_THROTTLE_LEVER_POS,
    RECIP_ENGINE1_MIXTURE_LEVER_POS,
    RECIP_ENGINE1_PROP_LEVER_POS,
    RECIP_ENGINE1_COWL_FLAP_POS,                //0 = closed, 1 = open
    RECIP_ENGINE1_CARB_HEAT_POS,                //0 = off, 1 = on
    RECIP_ENGINE1_ALTERNATE_AIR_POS,            //0 = closed, 1 = off
    RECIP_ENGINE1_COOLANT_RESERVOIR_PCT,
    RECIP_ENGINE1_LEFT_MAG,
    RECIP_ENGINE1_RIGHT_MAG,
    RECIP_ENGINE1_STARTER,
    RECIP_ENGINE1_BRAKE_POWER,                  //power delivered to the shaft
    RECIP_ENGINE1_SHAFT_TORQUE,                 //torque delivered to the shaft
    RECIP_ENGINE1_PROP_TORQUE,                  //torque required by prop
    RECIP_ENGINE1_STARTER_TORQUE,               //torque applied by starter
    RECIP_ENGINE1_COMBUSTION,                   //combustion flag
    RECIP_ENGINE1_TURBOCHARGER_FAILED,
    RECIP_ENGINE1_EMERGENCY_BOOST_ACTIVE,       //like WEP in the P51
    RECIP_ENGINE1_EMERGENCY_BOOST_ELAPSED_TIME, //how long emergency has been active
    RECIP_ENGINE1_WASTEGATE_POS,                //position of wastegate controller 1=closed (full boost), 0=open (no boost)
    RECIP_ENGINE1_PROP_BETA,                    //blade angle (relative to plane of rotation)
    RECIP_ENGINE1_THRUST,                       //lbs
    RECIP_ENGINE1_EGT_DEGR,                     //EGT degrees Rankine
    RECIP_ENGINE1_TIT_DEGR,                     //Turbine Inlet Temperature degrees Rankine
    RECIP_ENGINE1_CHT_DEGR,                     //CHT degrees Rankine
    RECIP_ENGINE1_OIL_TEMP_DEGR,                //Oil Temperature degrees Rankine
    RECIP_ENGINE1_RADIATOR_DEGR,                //Radiator temperature
    RECIP_ENGINE1_OIL_PRESS_PSF,                //Oil Pressure lbs/sqft
    RECIP_ENGINE1_OIL_LEAK_PCT,                 //Percent Oil leaked
    RECIP_ENGINE1_FUEL_AVAILABLE,               //Fuel available to engine?
    RECIP_ENGINE1_FUELFLOW_PPH,                 //Fuel flow in lbs/hr
    RECIP_ENGINE1_TANK_SELECTOR,                //Fuel tank selector
    RECIP_ENGINE1_TANKS_USED,                   //Fuel tanks used
    RECIP_ENGINE1_NUMBER_OF_TANKS_USED,         //Number of fuel tanks used
    RECIP_ENGINE1_FUEL_PRESSURE_PSF,            //Fuel pressure in lbs/sqft
    RECIP_ENGINE1_COMBUSTION_SOUND_PCT,         //Percent of Sound (0 - 1)
    RECIP_ENGINE1_INDUCED_VELOCITY,             //fps, velocity induced by prop
    RECIP_ENGINE1_DAMAGE_PERCENT,               //0-100%
    RECIP_ENGINE2_ENGINE_RPM,
    RECIP_ENGINE2_MANIFOLD_PRESSURE,
    RECIP_ENGINE2_THROTTLE_LEVER_POS,
    RECIP_ENGINE2_MIXTURE_LEVER_POS,
    RECIP_ENGINE2_PROP_LEVER_POS,
    RECIP_ENGINE2_COWL_FLAP_POS,                //0 = closed, 1 = open
    RECIP_ENGINE2_CARB_HEAT_POS,                //0 = off, 1 = on
    RECIP_ENGINE2_ALTERNATE_AIR_POS,            //0 = closed, 1 = off
    RECIP_ENGINE2_COOLANT_RESERVOIR_PCT,
    RECIP_ENGINE2_LEFT_MAG,
    RECIP_ENGINE2_RIGHT_MAG,
    RECIP_ENGINE2_STARTER,
    RECIP_ENGINE2_BRAKE_POWER,                  //power delivered to the shaft
    RECIP_ENGINE2_SHAFT_TORQUE,                 //torque delivered to the shaft
    RECIP_ENGINE2_PROP_TORQUE,                  //torque required by prop
    RECIP_ENGINE2_STARTER_TORQUE,               //torque applied by starter
    RECIP_ENGINE2_COMBUSTION,                   //combustion flag
    RECIP_ENGINE2_TURBOCHARGER_FAILED,
    RECIP_ENGINE2_EMERGENCY_BOOST_ACTIVE,       //like WEP in the P51
    RECIP_ENGINE2_EMERGENCY_BOOST_ELAPSED_TIME, //how long emergency has been active
    RECIP_ENGINE2_WASTEGATE_POS,                //position of wastegate controller 1=closed (full boost), 0=open (no boost)
    RECIP_ENGINE2_PROP_BETA,                    //blade angle (relative to plane of rotation)
    RECIP_ENGINE2_THRUST,                       //lbs
    RECIP_ENGINE2_EGT_DEGR,                     //EGT degrees Rankine
    RECIP_ENGINE2_TIT_DEGR,                     //Turbine Inlet Temperature degrees Rankine
    RECIP_ENGINE2_CHT_DEGR,                     //CHT degrees Rankine
    RECIP_ENGINE2_OIL_TEMP_DEGR,                //Oil Temperature degrees Rankine
    RECIP_ENGINE2_RADIATOR_DEGR,                //Radiator temperature
    RECIP_ENGINE2_OIL_PRESS_PSF,                //Oil Pressure lbs/sqft
    RECIP_ENGINE2_OIL_LEAK_PCT,                 //Percent Oil leaked
    RECIP_ENGINE2_FUEL_AVAILABLE,               //Fuel available to engine?
    RECIP_ENGINE2_FUELFLOW_PPH,                 //Fuel flow in lbs/hr
    RECIP_ENGINE2_TANK_SELECTOR,                //Fuel tank selector
    RECIP_ENGINE2_TANKS_USED,                   //Fuel tanks used
    RECIP_ENGINE2_NUMBER_OF_TANKS_USED,         //Number of fuel tanks used
    RECIP_ENGINE2_FUEL_PRESSURE_PSF,            //Fuel pressure in lbs/sqft
    RECIP_ENGINE2_COMBUSTION_SOUND_PCT,         //Percent of Sound (0 - 1)
    RECIP_ENGINE2_INDUCED_VELOCITY,             //fps, velocity induced by prop
    RECIP_ENGINE2_DAMAGE_PERCENT,               //0-100%
    RECIP_ENGINE3_ENGINE_RPM,
    RECIP_ENGINE3_MANIFOLD_PRESSURE,
    RECIP_ENGINE3_THROTTLE_LEVER_POS,
    RECIP_ENGINE3_MIXTURE_LEVER_POS,
    RECIP_ENGINE3_PROP_LEVER_POS,
    RECIP_ENGINE3_COWL_FLAP_POS,                //0 = closed, 1 = open
    RECIP_ENGINE3_CARB_HEAT_POS,                //0 = off, 1 = on
    RECIP_ENGINE3_ALTERNATE_AIR_POS,            //0 = closed, 1 = off
    RECIP_ENGINE3_COOLANT_RESERVOIR_PCT,
    RECIP_ENGINE3_LEFT_MAG,
    RECIP_ENGINE3_RIGHT_MAG,
    RECIP_ENGINE3_STARTER,
    RECIP_ENGINE3_BRAKE_POWER,                  //power delivered to the shaft
    RECIP_ENGINE3_SHAFT_TORQUE,                 //torque delivered to the shaft
    RECIP_ENGINE3_PROP_TORQUE,                  //torque required by prop
    RECIP_ENGINE3_STARTER_TORQUE,               //torque applied by starter
    RECIP_ENGINE3_COMBUSTION,                   //combustion flag
    RECIP_ENGINE3_TURBOCHARGER_FAILED,
    RECIP_ENGINE3_EMERGENCY_BOOST_ACTIVE,       //like WEP in the P51
    RECIP_ENGINE3_EMERGENCY_BOOST_ELAPSED_TIME, //how long emergency has been active
    RECIP_ENGINE3_WASTEGATE_POS,                //position of wastegate controller 1=closed (full boost), 0=open (no boost)
    RECIP_ENGINE3_PROP_BETA,                    //blade angle (relative to plane of rotation)
    RECIP_ENGINE3_THRUST,                       //lbs
    RECIP_ENGINE3_EGT_DEGR,                     //EGT degrees Rankine
    RECIP_ENGINE3_TIT_DEGR,                     //Turbine Inlet Temperature degrees Rankine
    RECIP_ENGINE3_CHT_DEGR,                     //CHT degrees Rankine
    RECIP_ENGINE3_OIL_TEMP_DEGR,                //Oil Temperature degrees Rankine
    RECIP_ENGINE3_RADIATOR_DEGR,                //Radiator temperature
    RECIP_ENGINE3_OIL_PRESS_PSF,                //Oil Pressure lbs/sqft
    RECIP_ENGINE3_OIL_LEAK_PCT,                 //Percent Oil leaked
    RECIP_ENGINE3_FUEL_AVAILABLE,               //Fuel available to engine?
    RECIP_ENGINE3_FUELFLOW_PPH,                 //Fuel flow in lbs/hr
    RECIP_ENGINE3_TANK_SELECTOR,                //Fuel tank selector
    RECIP_ENGINE3_TANKS_USED,                   //Fuel tanks used
    RECIP_ENGINE3_NUMBER_OF_TANKS_USED,         //Number of fuel tanks used
    RECIP_ENGINE3_FUEL_PRESSURE_PSF,            //Fuel pressure in lbs/sqft
    RECIP_ENGINE3_COMBUSTION_SOUND_PCT,         //Percent of Sound (0 - 1)
    RECIP_ENGINE3_INDUCED_VELOCITY,             //fps, velocity induced by prop
    RECIP_ENGINE3_DAMAGE_PERCENT,               //0-100%
    RECIP_ENGINE4_ENGINE_RPM,
    RECIP_ENGINE4_MANIFOLD_PRESSURE,
    RECIP_ENGINE4_THROTTLE_LEVER_POS,
    RECIP_ENGINE4_MIXTURE_LEVER_POS,
    RECIP_ENGINE4_PROP_LEVER_POS,
    RECIP_ENGINE4_COWL_FLAP_POS,                //0 = closed, 1 = open
    RECIP_ENGINE4_CARB_HEAT_POS,                //0 = off, 1 = on
    RECIP_ENGINE4_ALTERNATE_AIR_POS,            //0 = closed, 1 = off
    RECIP_ENGINE4_COOLANT_RESERVOIR_PCT,
    RECIP_ENGINE4_LEFT_MAG,
    RECIP_ENGINE4_RIGHT_MAG,
    RECIP_ENGINE4_STARTER,
    RECIP_ENGINE4_BRAKE_POWER,                  //power delivered to the shaft
    RECIP_ENGINE4_SHAFT_TORQUE,                 //torque delivered to the shaft
    RECIP_ENGINE4_PROP_TORQUE,                  //torque required by prop
    RECIP_ENGINE4_STARTER_TORQUE,               //torque applied by starter
    RECIP_ENGINE4_COMBUSTION,                   //combustion flag
    RECIP_ENGINE4_TURBOCHARGER_FAILED,
    RECIP_ENGINE4_EMERGENCY_BOOST_ACTIVE,       //like WEP in the P51
    RECIP_ENGINE4_EMERGENCY_BOOST_ELAPSED_TIME, //how long emergency has been active
    RECIP_ENGINE4_WASTEGATE_POS,                //position of wastegate controller 1=closed (full boost), 0=open (no boost)
    RECIP_ENGINE4_PROP_BETA,                    //blade angle (relative to plane of rotation)
    RECIP_ENGINE4_THRUST,                       //lbs
    RECIP_ENGINE4_EGT_DEGR,                     //EGT degrees Rankine
    RECIP_ENGINE4_TIT_DEGR,                     //Turbine Inlet Temperature degrees Rankine
    RECIP_ENGINE4_CHT_DEGR,                     //CHT degrees Rankine
    RECIP_ENGINE4_OIL_TEMP_DEGR,                //Oil Temperature degrees Rankine
    RECIP_ENGINE4_RADIATOR_DEGR,                //Radiator temperature
    RECIP_ENGINE4_OIL_PRESS_PSF,                //Oil Pressure lbs/sqft
    RECIP_ENGINE4_OIL_LEAK_PCT,                 //Percent Oil leaked
    RECIP_ENGINE4_FUEL_AVAILABLE,               //Fuel available to engine?
    RECIP_ENGINE4_FUELFLOW_PPH,                 //Fuel flow in lbs/hr
    RECIP_ENGINE4_TANK_SELECTOR,                //Fuel tank selector
    RECIP_ENGINE4_TANKS_USED,                   //Fuel tanks used
    RECIP_ENGINE4_NUMBER_OF_TANKS_USED,         //Number of fuel tanks used
    RECIP_ENGINE4_FUEL_PRESSURE_PSF,            //Fuel pressure in lbs/sqft
    RECIP_ENGINE4_COMBUSTION_SOUND_PCT,         //Percent of Sound (0 - 1)
    RECIP_ENGINE4_INDUCED_VELOCITY,             //fps, velocity induced by prop
    RECIP_ENGINE4_DAMAGE_PERCENT,               //0-100%
    FUEL_QUANTITY_CENTER3,                      //Total Gallons
    SCORE_STRING_QUICK_COMBAT,
    SCORE_STRING_MULTIPLAYER,
    IS_MULTIPLAYER_TEAM_COMBAT,
    VOR1_BEARING_DEGREES,
    FORWARD_GROUND_VELOCITY,                    // in ft/sec
    SIDE_GROUND_VELOCITY,                       // in ft/sec
    VERTICAL_GROUND_VELOCITY,                   // in ft/sec
    NORTH_VELOCITY,                             // in ft/sec
    EAST_VELOCITY,                              // in ft/sec
    VERTICAL_VELOCITY,                          // in ft/sec
    CLOCK_TOTAL_SECONDS,                //  local clock time, seconds since midnight
    GENERAL_ENGINE1_THROTTLE_LEVER_POS,
    GENERAL_ENGINE1_PROPELLER_LEVER_POS,
    GENERAL_ENGINE1_MIXTURE_LEVER_POS ,
    GENERAL_ENGINE1_OIL_TEMP,
    GENERAL_ENGINE1_OIL_PRES,
    GENERAL_ENGINE1_STARTER,
    GENERAL_ENGINE1_FAILURE,
    GENERAL_ENGINE1_GENERATOR_SWITCH,
    GENERAL_ENGINE1_GENERATOR_ACTIVE,
    GENERAL_ENGINE1_EGT,
    GENERAL_ENGINE2_THROTTLE_LEVER_POS,
    GENERAL_ENGINE2_PROPELLER_LEVER_POS,
    GENERAL_ENGINE2_MIXTURE_LEVER_POS ,
    GENERAL_ENGINE2_OIL_TEMP,
    GENERAL_ENGINE2_OIL_PRES,
    GENERAL_ENGINE2_STARTER ,
    GENERAL_ENGINE2_FAILURE,
    GENERAL_ENGINE2_GENERATOR_SWITCH,
    GENERAL_ENGINE2_GENERATOR_ACTIVE,
    GENERAL_ENGINE2_EGT,
    GENERAL_ENGINE3_THROTTLE_LEVER_POS,
    GENERAL_ENGINE3_PROPELLER_LEVER_POS,
    GENERAL_ENGINE3_MIXTURE_LEVER_POS ,
    GENERAL_ENGINE3_OIL_TEMP,
    GENERAL_ENGINE3_OIL_PRES,
    GENERAL_ENGINE3_STARTER ,
    GENERAL_ENGINE3_FAILURE,
    GENERAL_ENGINE3_GENERATOR_SWITCH,
    GENERAL_ENGINE3_GENERATOR_ACTIVE,
    GENERAL_ENGINE3_EGT,
    GENERAL_ENGINE4_THROTTLE_LEVER_POS,
    GENERAL_ENGINE4_PROPELLER_LEVER_POS,
    GENERAL_ENGINE4_MIXTURE_LEVER_POS ,
    GENERAL_ENGINE4_OIL_TEMP,
    GENERAL_ENGINE4_OIL_PRES,
    GENERAL_ENGINE4_STARTER ,
    GENERAL_ENGINE4_FAILURE,
    GENERAL_ENGINE4_GENERATOR_SWITCH,
    GENERAL_ENGINE4_GENERATOR_ACTIVE,
    GENERAL_ENGINE4_EGT,
    NAV1_COMMNAV_TEST,
    NAV2_COMMNAV_TEST,
    COM1_AUTOSWITCH_ON,
    NAV1_AUTOSWITCH_ON,
    COM2_AUTOSWITCH_ON,
    NAV2_AUTOSWITCH_ON,
    ELAPSED_SECONDS,
    G_FORCE_MAXIMUM,
    G_FORCE_MINIMUM,
    ENGINE_WORKING_TIME,
    ENGINE1_MAX_RPM,
    ENGINE2_MAX_RPM,
    ENGINE3_MAX_RPM,
    ENGINE4_MAX_RPM,
    ATTITUDE_BARS_POSITION,
    RAD_INS_SWITCH,     // used in concorde only
    RADIO_HEIGHT,           // meters
    DECISION_HEIGHT,        // meters
    LOW_HEIGHT_WARNING,     // boolean
    COM_STBY_FREQUENCY,     // BCD; same as COM_FREQUENCY
    COM_RADIO_25_KHZ_TUNABLE, // Boolean
    NAV_LIGHTS,
    LOGO_LIGHTS,
    WING_LIGHTS,
    RECOGNITION_LIGHTS,
    RADIO_MORSE_IDENT_STATES,
    TURB_ENGINE_1_N1,
    TURB_ENGINE_1_N2,
    TURB_ENGINE_1_CORRECTED_N1,
    TURB_ENGINE_1_CORRECTED_N2,
    TURB_ENGINE_1_CORRECTED_FF,
    TURB_ENGINE_1_PCT_MAX_TORQUE,
    TURB_ENGINE_1_EPR,
    TURB_ENGINE_1_ITT,
    TURB_ENGINE_1_AFTERBURNER,
    TURB_ENGINE_1_JET_THRUST,
    TURB_ENGINE_1_BLEEDAIRPSI,
    TURB_ENGINE_1_TANK_SELECTOR,
    TURB_ENGINE_1_TANKS_USED,
    TURB_ENGINE_1_NUMBER_TANKS,
    TURB_ENGINE_1_FUELFLOW_PPH,
    TURB_ENGINE_1_FUEL_AVAILABLE,
    TURB_ENGINE_1_PCT_AREA,
    TURB_ENGINE_1_PCT_REVERSER,
    TURB_ENGINE_1_VIBRATION,
    TURB_ENGINE_2_N1,
    TURB_ENGINE_2_N2,
    TURB_ENGINE_2_CORRECTED_N1,
    TURB_ENGINE_2_CORRECTED_N2,
    TURB_ENGINE_2_CORRECTED_FF,
    TURB_ENGINE_2_PCT_MAX_TORQUE,
    TURB_ENGINE_2_EPR,
    TURB_ENGINE_2_ITT,
    TURB_ENGINE_2_AFTERBURNER,
    TURB_ENGINE_2_JET_THRUST,
    TURB_ENGINE_2_BLEEDAIRPSI,
    TURB_ENGINE_2_TANK_SELECTOR,
    TURB_ENGINE_2_TANKS_USED,
    TURB_ENGINE_2_NUMBER_TANKS,
    TURB_ENGINE_2_FUELFLOW_PPH,
    TURB_ENGINE_2_FUEL_AVAILABLE,
    TURB_ENGINE_2_PCT_AREA,
    TURB_ENGINE_2_PCT_REVERSER,
    TURB_ENGINE_2_VIBRATION,
    TURB_ENGINE_3_N1,
    TURB_ENGINE_3_N2,
    TURB_ENGINE_3_CORRECTED_N1,
    TURB_ENGINE_3_CORRECTED_N2,
    TURB_ENGINE_3_CORRECTED_FF,
    TURB_ENGINE_3_PCT_MAX_TORQUE,
    TURB_ENGINE_3_EPR,
    TURB_ENGINE_3_ITT,
    TURB_ENGINE_3_AFTERBURNER,
    TURB_ENGINE_3_JET_THRUST,
    TURB_ENGINE_3_BLEEDAIRPSI,
    TURB_ENGINE_3_TANK_SELECTOR,
    TURB_ENGINE_3_TANKS_USED,
    TURB_ENGINE_3_NUMBER_TANKS,
    TURB_ENGINE_3_FUELFLOW_PPH,
    TURB_ENGINE_3_FUEL_AVAILABLE,
    TURB_ENGINE_3_PCT_AREA,
    TURB_ENGINE_3_PCT_REVERSER,
    TURB_ENGINE_3_VIBRATION,
    TURB_ENGINE_4_N1,
    TURB_ENGINE_4_N2,
    TURB_ENGINE_4_CORRECTED_N1,
    TURB_ENGINE_4_CORRECTED_N2,
    TURB_ENGINE_4_CORRECTED_FF,
    TURB_ENGINE_4_PCT_MAX_TORQUE,
    TURB_ENGINE_4_EPR,
    TURB_ENGINE_4_ITT,
    TURB_ENGINE_4_AFTERBURNER,
    TURB_ENGINE_4_JET_THRUST,
    TURB_ENGINE_4_BLEEDAIRPSI,
    TURB_ENGINE_4_TANK_SELECTOR,
    TURB_ENGINE_4_TANKS_USED,
    TURB_ENGINE_4_NUMBER_TANKS,
    TURB_ENGINE_4_FUELFLOW_PPH,
    TURB_ENGINE_4_FUEL_AVAILABLE,
    TURB_ENGINE_4_PCT_AREA,
    TURB_ENGINE_4_PCT_REVERSER,
    TURB_ENGINE_4_VIBRATION,
    PROPELLER_1_RPM,                        // Propeller 1
    PROPELLER_1_PCT_MAX_RPM,
    PROPELLER_1_THRUST,
    PROPELLER_1_BETA,
    PROPELLER_1_FEATHERING_INHIBIT,
    PROPELLER_1_FEATHERED,
    PROPELLER_1_SYNC_DELTA_LEVER,
    PROPELLER_1_AUTOFEATHER_ARMED,
    PROPELLER_2_RPM,                        // Propeller 2
    PROPELLER_2_PCT_MAX_RPM,
    PROPELLER_2_THRUST,
    PROPELLER_2_BETA,
    PROPELLER_2_FEATHERING_INHIBIT,
    PROPELLER_2_FEATHERED,
    PROPELLER_2_SYNC_DELTA_LEVER,
    PROPELLER_2_AUTOFEATHER_ARMED,
    PROPELLER_3_RPM,                        // Propeller 3
    PROPELLER_3_PCT_MAX_RPM,
    PROPELLER_3_THRUST,
    PROPELLER_3_BETA,
    PROPELLER_3_FEATHERING_INHIBIT,
    PROPELLER_3_FEATHERED,
    PROPELLER_3_SYNC_DELTA_LEVER,
    PROPELLER_3_AUTOFEATHER_ARMED,
    PROPELLER_4_RPM,                        // Propeller 4
    PROPELLER_4_PCT_MAX_RPM,
    PROPELLER_4_THRUST,
    PROPELLER_4_BETA,
    PROPELLER_4_FEATHERING_INHIBIT,
    PROPELLER_4_FEATHERED,
    PROPELLER_4_SYNC_DELTA_LEVER,
    PROPELLER_4_AUTOFEATHER_ARMED,
    HYDRAULICS1_PRESSURE_PSF,
    HYDRAULICS1_RESERVOIR_PCT,
    HYDRAULICS2_PRESSURE_PSF,
    HYDRAULICS2_RESERVOIR_PCT,
    HYDRAULICS3_PRESSURE_PSF,
    HYDRAULICS3_RESERVOIR_PCT,
    HYDRAULICS4_PRESSURE_PSF,
    HYDRAULICS4_RESERVOIR_PCT,
    AVIONICS_MASTER_SWITCH,
    PANEL_AUTOFEATHER_SWITCH,               // Panel States
    PANEL_ANTIICE_SWITCH,
    PANEL_AUTOBRAKE_SWITCH,
    DAY_OF_WEEK,            // 0-6
    DAY_OF_MONTH,           // 1-31
    CURRENT_MONTH,          // 1-12
    CURRENT_YEAR,           // 1-????
    TIME_ZONE_OFFSET,       // in minutes, positive west of GMT
    DAYLIGHT_SAVING_ACTIVE, // 0 or 1
    TOTAL_LOAD_AMPS,
    BATTERY_LOAD,
    BATTERY_VOLTAGE,
    MAIN_BUS_VOLTAGE,
    MAIN_BUS_AMPS,
    AVIONICS_BUS_VOLTAGE,
    AVIONICS_BUS_AMPS,
    HOT_BATTERY_BUS_VOLTAGE,
    HOT_BATTERY_BUS_AMPS,
    BATTERY_BUS_VOLTAGE,
    BATTERY_BUS_AMPS,
    GENERATOR_ALTERNATOR_1_BUS_VOLTAGE,
    GENERATOR_ALTERNATOR_1_BUS_AMPS,
    GENERATOR_ALTERNATOR_2_BUS_VOLTAGE,
    GENERATOR_ALTERNATOR_2_BUS_AMPS,
    GENERATOR_ALTERNATOR_3_BUS_VOLTAGE,
    GENERATOR_ALTERNATOR_3_BUS_AMPS,
    GENERATOR_ALTERNATOR_4_BUS_VOLTAGE,
    GENERATOR_ALTERNATOR_4_BUS_AMPS,
    GENERAL_PANEL_CIRCUIT_ON,
    FLAP_MOTOR_CIRCUIT_ON,
    GEAR_MOTOR_CIRCUIT_ON,
    AUTOPILOT_CIRCUIT_ON,
    AVIONICS_CIRCUIT_ON,
    PITOT_HEAT_CIRCUIT_ON,
    PROP_SYNC_CIRCUIT_ON,
    AUTO_FEATHER_CIRCUIT_ON,
    AUTO_BRAKES_CIRCUIT_ON,
    STANDBY_VACUUM_CIRCUIT_ON,
    MARKER_BEACON_CIRCUIT_ON,
    GEAR_WARNING_CIRCUIT_ON,
    HYDRAULIC_PUMP_CIRCUIT_ON,
    ELEVATOR_DEFLECTION,    //  radians
    ELEVATOR_TRIM,          //  radians
    AILERON_DEFLECTION,     //  radians
    AILERON_TRIM,           //  radians
    RUDDER_DEFLECTION,      //  radians
    RUDDER_TRIM,            //  radians
    PROPSYNC_ACTIVE,
    INCIDENCE_ALPHA,
    INCIDENCE_BETA,
    TOTAL_FUEL_QUANTITY_LBS,
    FLIGHT_DIRECTOR_ACTIVE,
    FLIGHT_DIRECTOR_PITCH,      // degrees
    FLIGHT_DIRECTOR_BANK,       // degrees
    CG_PERCENT,
    CG_AFT_LIMIT,
    CG_FWD_LIMIT,
    CG_MAX_MACH,
    CG_MIN_MACH,
    STANDARD_ATM_TEMPERATURE,   // standard atmosphere temperature at aircraft's altitude; celsius
    ENGINE_1_FUELFLOW_BUG_POSITION,     // lbs per hour
    ENGINE_2_FUELFLOW_BUG_POSITION,
    ENGINE_3_FUELFLOW_BUG_POSITION,
    ENGINE_4_FUELFLOW_BUG_POSITION,
    CONCORDE_VISOR_NOSE_HANDLE, // 0 - Visor Up, Nose Up, 1 - Visor Dn, Nose Up, 2 - Visor Dn, Nose 5 deg, 3 - Visor Dn, Nose 12.5 deg
    CONCORDE_VISOR_POS_PCT,     // 0=Up, 1.0 Extended/Down
    CONCORDE_NOSE_ANGLE,        // degrees; 0 - up
    GEAR_POS_TAIL,
    ELEVON_1_DEFLECTION,        // The elevon indices start at 1 at the far left and increase to the right.
    ELEVON_2_DEFLECTION,        // radians
    ELEVON_3_DEFLECTION,
    ELEVON_4_DEFLECTION,
    ELEVON_5_DEFLECTION,
    ELEVON_6_DEFLECTION,
    ELEVON_7_DEFLECTION,
    ELEVON_8_DEFLECTION,
    PANEL_AUTOPILOT_SPEED_SETTING,      // Knots
    AUTOPILOT_AIRSPEED_ACQUISITION,     // boolean
    AUTOPILOT_AIRSPEED_HOLD_CURRENT,    // boolean
    AUTOPILOT_MAX_SPEED,                // boolean
    AUTOPILOT_CRUISE_SPEED,             // boolean
    BARBER_POLE_MACH,                   // real mach
    SELECTED_FUEL_TRANSFER_MODE,        // -1 off, 0 auto, 1 forward, 2 aft
    HYDRAULIC_SYSTEM_INTEGRITY,         // 0.0 - complete failure; 1.0 - everything is ok
    ATTITUDE_CAGE_BUTTON,
    VOR1_NEEDLE_RADIO,                  // +-127 correspond to +-10 if VOR or to +-2.5 if localiser. Driven always by VOR
    GPS_POSITION_LAT,
    GPS_POSITION_LON,
    GPS_POSITION_ALT,
    GPS_MAGVAR,                         // magnetic deviation (radians)
    GPS_FLAGS,                          // GPS_FLAG_ACTIVE_FLIGHT_PLAN = 0x00002    there is an active flight plan
                                        // GPS_FLAG_ACTIVE_WAY_POINT   = 0x00004    there is an active way point
                                        // GPS_FLAG_ARRIVED            = 0x00008    last waypoint reached
                                        // GPS_FLAG_DIRECTTO_FLIGHTPLAN= 0x00020    manual flight plan
    GPS_GROUND_SPEED,                   // meters per second
    GPS_GROUND_HEADING,                 // radians (0 - 2PI); true heading, yaw
    GPS_GROUND_TRACK,                   // radians (0 - 2PI); heading + magvar, magnetic yaw
    GPS_WP_DISTANCE,                    // meters to next wp
    GPS_WP_BEARING,                     // heading (radians) to next wp + magvar
    GPS_WP_CROSS_TRK,                   // cross_trk (meters)
    GPS_WP_REQ_HDG,                     // required heading, magnetic, radians (0 - 2PI), from prev wp to next wp
    GPS_WP_STRING,                      // name of next waypoint
    GPS_WP_NEXT_LAT,
    GPS_WP_NEXT_LON,
    GPS_WP_NEXT_ALT,
    GPS_WP_PREV_LAT,
    GPS_WP_PREV_LON,
    GPS_WP_PREV_ALT,
    GPS_WP_TRUE_BEARING,                // true bearing, degrees
    GPS_WP_TRUE_REQ_HDG,                // true required heading, degrees
    GPS_DRIVES_NAV1,
    HSI_VERTICAL_NEEDLE,                // -1.0 .. 1.0 corresponds to -10deg .. 10deg if VOR or to -2.5deg .. +2.5deg if localiser +5NM .. -5NM  if GPS
    HSI_HORIZONTAL_NEEDLE,              // -1.0 .. 1.0 corresponds to -0.7deg .. +0.7deg
    HSI_VERTICAL_VALID,                 // 0 - Nav flag on
    HSI_HORIZONTAL_VALID,               // 0 - GS flag on
    HSI_TF_FLAG,                        // -1.0 - from. 0.0 - none, 1.0 - to
    HSI_OBI_NEEDLE,                     // 0 .. 360
    HSI_DESIRED_HEADING_NEEDLE,         // 0 .. 360
    HSI_BEARING,                        // 0 .. 360
    HSI_BEARING_VALID,                  // 1 - true
    HSI_SIGNAL_LOCALISER,               // 1 - true
    HSI_SPEED,                          // knots
    HSI_DISTANCE,                       // NM
    HSI_STATION_NAME,                   // string
    GEAR_POS_AUX,                       //      Aux gear    0 - 16k
    TOOLTIP_UNITS,                      // enum TOOLTIP_UNITS_TYPE
    LSO_TEXTURE_ID,                     // current LSO_Texture_Id in AI_PLAYER.mw
    LSO_TEXT,                           // text
    LEFT_FOLDING_WING_PERCENT,          // 0 - not folded; 1 - folded
    RIGHT_FOLDING_WING_PERCENT,         // 0 - not folded; 1 - folded
    MAIN_EXIT_OPEN,                     // 0 - not open; 1 - open
    TAILHOOK_POSITION,                    // 0 - retracted; 1 - extended
    COM2_FREQUENCY,                     // BCD same as COM_FREQUENCY
    COM2_STBY_FREQUENCY,                // BCD; same as COM_FREQUENCY
    NAV1_STBY_FREQUENCY,                // BCD NAV1 radio freq; same as NAV1_FREQUENCY
    NAV2_STBY_FREQUENCY,                // BCD NAV2 radio freq; same as NAV2_FREQUENCY
    COM1_TRANSMIT_SELECT,               // 1.0 - yes
    COM2_TRANSMIT_SELECT,               // 1.0 - yes
    COM_BOTH_RECEIVE,                   // 1.0 - yes
    NAV1_SOUND_TOGGLE,
    NAV2_SOUND_TOGGLE,
    DME_SOUND_TOGGLE,
    ADF_SOUND_TOGGLE,
    MARKER_SOUND_TOGGLE,
    VOR2_GS_NEEDLE,
    GENERAL_ENGINE1_ANTI_ICE_POS,       //0 - off 1 - on
    GENERAL_ENGINE2_ANTI_ICE_POS,       //0 - off 1 - on
    GENERAL_ENGINE3_ANTI_ICE_POS,       //0 - off 1 - on
    GENERAL_ENGINE4_ANTI_ICE_POS,       //0 - off 1 - on
    ENGINE1_ON_FIRE,                    // 0 - no, 1 - yes
    ENGINE2_ON_FIRE,
    ENGINE3_ON_FIRE,
    ENGINE4_ON_FIRE,
    ENGINE1_FUEL_PUMP_ON,               // 0 - no, 1 - yes
    ENGINE2_FUEL_PUMP_ON,               // 0 - no, 1 - yes
    ENGINE3_FUEL_PUMP_ON,               // 0 - no, 1 - yes
    ENGINE4_FUEL_PUMP_ON,               // 0 - no, 1 - yes
    GENERAL_ENGINE1_FUEL_VALVE,         //1 = Open, 0 = Closed
    GENERAL_ENGINE2_FUEL_VALVE,         //1 = Open, 0 = Closed
    GENERAL_ENGINE3_FUEL_VALVE,         //1 = Open, 0 = Closed
    GENERAL_ENGINE4_FUEL_VALVE,         //1 = Open, 0 = Closed
    GENERAL_ENGINE1_FUEL_PRESSURE,
    GENERAL_ENGINE2_FUEL_PRESSURE,
    GENERAL_ENGINE3_FUEL_PRESSURE,
    GENERAL_ENGINE4_FUEL_PRESSURE,
    WATER_RUDDER_ON,                    // 0 - up, 1 - down
    PROP_DEICE_SWITCH,                  // 0 = off, 1 = on
    STRUCTURAL_DEICE_SWITCH,            // 0 = off, 1 = on 
    FUEL_TANK_SELECTOR_2,               //  see FUEL_TANK_SELECTOR_ enum values
    TRAILING_EDGE_FLAPS0_LEFT_ANGLE,    // Angle of primary trailing edge flaps
    TRAILING_EDGE_FLAPS0_RIGHT_ANGLE,   // Angle of primary trailing edge flaps


    ALTERNATE_STATIC_AIR_OPEN,                  // Alternate static air source

    AILERON_TRIM_PCT,                           // percent -1 to 1
    RUDDER_TRIM_PCT,                            // percent -1 to 1

    LIGHT_NAV_ON,
    LIGHT_BEACON_ON,
    LIGHT_LANDING_ON,
    LIGHT_TAXI_ON,
    LIGHT_STROBE_ON,
    LIGHT_PANEL_ON,
    LIGHT_RECOGNITION_ON,
    LIGHT_WING_ON,
    LIGHT_LOGO_ON,
    LIGHT_CABIN_ON,

    TOTAL_VELOCITY,                      
    TOTAL_WORLD_VELOCITY,                
    
    VARIOMETER_RATE,                     
    VARIOMETER_SWITCH,                   
    
    RELATIVE_WIND_VELOCITY_BODY_X,       
    RELATIVE_WIND_VELOCITY_BODY_Y,       
    RELATIVE_WIND_VELOCITY_BODY_Z,       
    ACCELERATION_WORLD_X,                
    ACCELERATION_WORLD_Y,                
    ACCELERATION_WORLD_Z,                
    ACCELERATION_BODY_X,                 
    ACCELERATION_BODY_Y,                 
    ACCELERATION_BODY_Z,                 
    
    PLANE_ALT_ABOVE_GROUND,              
    MAGNETIC_COMPASS,                    
    TURN_INDICATOR_RATE,                 
    TURN_INDICATOR_SWITCH,               
    BRAKE_DEPENDENT_HYDRAULIC_PRESSURE,  
    
    FLY_BY_WIRE_ELAC_SWITCH,             
    FLY_BY_WIRE_FAC_SWITCH,              
    FLY_BY_WIRE_SEC_SWITCH,              
    FLY_BY_WIRE_ELAC_FAILED,             
    FLY_BY_WIRE_FAC_FAILED,              
    FLY_BY_WIRE_SEC_FAILED,              
    
    GEAR_EMERGENCY_HANDLE_POSITION,      
    GEAR_WARNING,                        
    TAILWHEEL_LOCK_ON,                   
    ANTISKID_BRAKES_ACTIVE,              
    RETRACT_FLOAT_SWITCH,                
    STEER_INPUT_CONTROL,                 
    
    STALL_ALPHA,                         
    EXIT_OPEN,                           
    DECISION_ALTITUDE_MSL,               
    WATER_BALLAST_VALVE,                 
    AUTOPILOT_RPM_HOLD,                  
    AUTOPILOT_PITCH_HOLD_REF,            
    AUTOPILOT_MAX_BANK,                  
    PROP_FEATHER_SWITCH,                 
    MASTER_IGNITION_SWITCH,              
    BLEED_AIR_SOURCE_CONTROL,            
    STRUCTURAL_ICE_PCT,                  
    
    ROTOR_BRAKE_HANDLE_POS,              
    ROTOR_BRAKE_ACTIVE,                  
    ROTOR_CLUTCH_SWITCH_POS,             
    ROTOR_CLUTCH_ACTIVE,                 
    ROTOR_GOV_SWITCH_POS,                
    ROTOR_GOV_ACTIVE,                    
    ROTOR_LATERAL_TRIM_PCT,              
    DISK_PITCH_ANGLE,                    
    DISK_BANK_ANGLE,                     
    DISK_PITCH_PCT,                      
    DISK_BANK_PCT,                       
    
    TOW_RELEASE_HANDLE,                  
    APU_PCT_RPM,                         
    APU_PCT_STARTER,                     
    APU_VOLTS,                           
    APU_GENERATOR_SWITCH,                
    APU_GENERATOR_ACTIVE,                
    APU_ON_FIRE_DETECTED,                
    
    PRESSURIZATION_CABIN_ALTITUDE,       
    PRESSURIZATION_CABIN_ALTITUDE_GOAL,  
    PRESSURIZATION_CABIN_ALTITUDE_RATE,  
    PRESSURIZATION_PRESSURE_DIFFERENTIAL,
    PRESSURIZATION_DUMP_SWITCH,          
    
    FIRE_BOTTLE_SWITCH,                  
    FIRE_BOTTLE_DISCHARGED,              
    
    CABIN_NO_SMOKING_ALERT_SWITCH,       
    CABIN_SEATBELTS_ALERT_SWITCH,        
    
    GPWS_WARNING,                        
    GPWS_SYSTEM_ACTIVE,                  
    
    IS_LATITUDE_LONGITUDE_FREEZE_ON,     
    IS_ALTITUDE_FREEZE_ON,               

    NOSEWHEEL_LOCK_ON,
    PAYLOAD_STATION_NUM_SIMOBJECTS,
    FOLDING_WING_HANDLE_POSITION,
    TAILHOOK_HANDLE,
    LAUNCHBAR_POSITION,
    LAUNCHBAR_SWITCH,
    LAUNCHBAR_HELD_EXTENDED,
    AUTOPILOT_PITCH_HOLD,
    NUMBER_OF_CATAPULTS,
    HOLDBACK_BAR_INSTALLED,
    BLAST_SHIELD_POSITION,
    CATAPULT_STROKE_POSITION,
    RECIP_ENG_DETONATING,
    RECIP_ENG_CYLINDER_HEALTH,
    RECIP_ENG_NUM_CYLINDERS,
    RECIP_ENG_NUM_CYLINDERS_FAILED,
    RECIP_ENG_ANTIDETONATION_TANK_VALVE,
    RECIP_ENG_ANTIDETONATION_TANK_QUANTITY,
    RECIP_ENG_ANTIDETONATION_TANK_MAX_QUANTITY,
    RECIP_ENG_NITROUS_TANK_VALVE,
    RECIP_ENG_NITROUS_TANK_QUANTITY,
    RECIP_ENG_NITROUS_TANK_MAX_QUANTITY,
    TURB_ENG_AFTERBURNER_STAGE_ACTIVE,
    TURB_ENG_AFTERBURNER_PCT_ACTIVE,
    FUEL_DUMP_SWITCH,
    FUEL_DUMP_ACTIVE,
    COLLECTIVE_POSITION,

    IS_ATTITUDE_FREEZE_ON,

    NUM_SLING_CABLES,
    SLING_OBJECT_ATTACHED,
    SLING_CABLE_BROKEN,
    SLING_CABLE_EXTENDED_LENGTH,
    SLING_ACTIVE_PAYLOAD_STATION,
    SLING_HOIST_PERCENT_DEPLOYED,
    SLING_HOIST_SWITCH,
    SLING_HOOK_IN_PICKUP_MODE,

    CABLE_CAUGHT_BY_TAILHOOK,
                      
    C_GAUGE_TOKEN                       // Always the last

} GAUGE_TOKEN;

    

typedef enum    VAR_TYPE {
    VAR_TYPE_NONE,

    TYPE_BOOL8,
    TYPE_UINT8,
    TYPE_SINT8,
    TYPE_FLAGS8,
    TYPE_ENUM8,

    TYPE_BOOL16,
    TYPE_ANGL16,
    TYPE_UINT16,
    TYPE_SINT16,
    TYPE_UIF16,
    TYPE_SIF16,
    TYPE_FLAGS16,
    TYPE_ENUM16,
    TYPE_BCD16,
    TYPE_BCO16,
    TYPE_VAR16,

    TYPE_BOOL32,
    TYPE_ANGL32,
    TYPE_UINT32,
    TYPE_SINT32,
    TYPE_UIF32,
    TYPE_SIF32,
    TYPE_FLAGS32,
    TYPE_ENUM32,
    TYPE_VAR32,

    TYPE_ANGL48,
    TYPE_SINT48,
    TYPE_UIF48,
    TYPE_SIF48,

    TYPE_UINT64,
    TYPE_SINT64,
    TYPE_SIF64,
    TYPE_FLOAT64,

    TYPE_BOOL,
    TYPE_FLAGS,
    TYPE_ENUM,

    TYPE_VOID,
    TYPE_PVOID,

    TYPE_PUINT32,
    TYPE_PSINT32,
    TYPE_PFLOAT64,

    VAR_TYPE_MAX
} VAR_TYPE;
    
typedef union   UNIVERSAL_VAR {
    FLOAT64 n;              // any number
    BOOL    b;              // any boolean
    ENUM    e;              // any enumerated value
    FLAGS   f;              // any flags field
    PVOID   p;              // any pointer
    VAR32   d;              // any binary coded decimal
    VAR32   o;              // any binary coded octal
} UNIVERSAL_VAR, *PUNIVERSAL_VAR, **PPUNIVERSAL_VAR;

typedef struct  MODULE_VAR {
    GAUGE_TOKEN             id;
    PVOID                   var_ptr;
    VAR_TYPE                var_type;
    UNIVERSAL_VAR           var_value;
    UNIVERSAL_VAR           var_old;                // value of global var last iteration
} MODULE_VAR, *PMODULE_VAR, **PPMODULE_VAR;
    
    
    
// Panel identifiers
#define IDENT_MAIN_PANEL                        0
#define IDENT_MAIN_PANEL_ALT1                    1
#define IDENT_MAIN_PANEL_ALT2                    2
#define IDENT_MAIN_PANEL_ALT3                    3
#define IDENT_MAIN_PANEL_ALT4                    4
#define IDENT_MAIN_PANEL_ALT5                    5
#define IDENT_MAIN_PANEL_ALT6                    6
#define IDENT_MAIN_PANEL_ALT7                    7
#define IDENT_MAIN_PANEL_ALT8                    8
#define IDENT_MAIN_PANEL_ALT9                    9
#define IDENT_THROTTLE_PANEL                    10
#define IDENT_RADIO_STACK_PANEL                 50
#define IDENT_COMPASS_PANEL                     75
#define IDENT_MINI_CONTROLS_PANEL               100
#define IDENT_ANNUNCIATOR_PANEL                 125
#define IDENT_ANNUNCIATOR2_PANEL                150
#define IDENT_IFR_MAIN_PANEL                    175
#define IDENT_COLLECTIVE_PANEL                  200
#define IDENT_GPS_PANEL                         225
#define IDENT_OVERHEAD_PANEL                    250
#define IDENT_PARKING_BRAKE_PANEL               251
#define IDENT_FLAPS_TRIM_PANEL                  252
#define IDENT_FUEL_PANEL                        253
#define IDENT_ELECTRICAL_PANEL                  254
#define IDENT_TRIM_PANEL                        255
#define IDENT_LANDING_GEAR_PANEL                256
#define IDENT_MISC_POPUP_1                      260
#define IDENT_MISC_POPUP_2                      261
#define IDENT_MISC_POPUP_3                      262
#define IDENT_MISC_POPUP_4                      263
#define IDENT_MISC_POPUP_5                      264
#define IDENT_MISC_POPUP_6                      265
#define IDENT_MISC_POPUP_7                      266
#define IDENT_MISC_POPUP_8                      267
#define IDENT_MISC_POPUP_9                      268
#define IDENT_MISC_POPUP_10                     269


#define IDENT_USER                              10000
#define IDENT_USER_MAX                          19999

#define MIN_HUD_IDENT                           20000
#define IDENT_AIRCRAFT_HUD_3D_PANEL             20000
#define IDENT_HELICOPTER_HUD_3D_PANEL           20001
#define IDENT_CONTROLS_HUD_3D_PANEL             20002
#define IDENT_TEXT_HUD_3D_PANEL                 20003
#define IDENT_DAMAGE_HUD_3D_PANEL               20004
#define IDENT_SCORE_HUD_3D_PANEL                20005
#define MAX_HUD_IDENT                           20006

#define IDENT_LSO_WINDOW                        22000
#define IDENT_POI_WINDOW                        22001
#define IDENT_TIMER_WINDOW                      22002
#define IDENT_DROPPABLE_OBJ_WINDOW              22003


#define IDENT_INVALID                           4294967295
    
// HUD settings
#define HUD_ON                          0x1
#define HUD_METRIC                      0x2
    
    
// PANEL STATES
//
#define PS_MAIN_PANEL_VISIBLE           BIT1
#define PS_MAIN_PANEL_COMPOSED          BIT2
#define PS_PADLOCK_SERVER               BIT3            // not used yet
#define PS_PADLOCK_CLIENT               BIT4            // not used yet
#define PS_HELICOPTER_HUD_VISIBLE       BIT5
#define PS_AIRCRAFT_HUD_VISIBLE         BIT6
#define PS_AIRCRAFT_HUD_ON              BIT7
#define PS_RADIO_STACK_VISIBLE          BIT8
#define PS_HUD_METRIC_OUTPUT            BIT9
#define PS_MAIN_PANEL_ON                BIT10
#define PS_CONTROLS_HUD_VISIBLE         BIT11
#define PS_RADIO_STACK_AUTOPOPED        BIT12
#define PS_MAIN_PANEL_UNDOCKED          BIT13
#define PS_AIRCRAFT_HUD_EXIST           BIT14
#define PS_HELICOPTER_HUD_EXIST         BIT15
#define PS_HUD_EXIST                    (PS_AIRCRAFT_HUD_EXIST|PS_HELICOPTER_HUD_EXIST)
#define PS_PANELS_LOAD_VISIBLE          BIT16
#define PS_HUD_LOAD_VISIBLE             BIT17
#define PS_PANEL_VISIBLE                BIT18
#define PS_HUD_PANEL_VISIBLE            BIT19
#define PS_READONLY                      (~(PS_PADLOCK_SERVER|PS_PADLOCK_CLIENT|PS_HUD_METRIC_OUTPUT))

#define PS_SET                                          0xFFFFFFFF
#define PS_CLEAR                                        0x00000000
    
#define GAUGE_FLAG_NORMAL               0
#define GAUGE_FLAG_HIDDEN               0x1
#define GAUGE_FLAG_BLINKING             0x2
#define GAUGE_FLAG_GRAYED               0x4
#define GAUGE_FLAG_HILIGHTED            0x8

#define UNITS_UNKNOWN                   (-1)
#define UNITS_STRING                    (-2)
    
class IGaugeCDrawableCreateParameters
{
public:
    enum PARAMETER_TYPE {
        PARAMETER_TYPE_NONE = 0,
        PARAMETER_TYPE_BOOL = 1,        
        PARAMETER_TYPE_FLOAT = 2,
        PARAMETER_TYPE_INT = 3,
        PARAMETER_TYPE_STRING = 4,
    };

    virtual bool GetParameter (const char* szParName, const char** pszValue) const = 0;
    virtual bool GetParameter (const char* szParName, FLOAT64* pdValue) const = 0;
    virtual bool GetParameter (const char* szParName, SINT32* piValue) const = 0;
    virtual bool GetParameter (const char* szParName, bool* pbValue) const = 0;
    virtual bool SetParameterId (const char* szParName, SINT32 id, PARAMETER_TYPE type) const = 0;
};

class IGaugeCDrawableDrawParameters 
{
public:
    virtual bool GetParameter (SINT32 id, const char** pszValue) const = 0;
    virtual bool GetParameter (SINT32 id, FLOAT64* pdValue) const = 0;
    virtual bool GetParameter (SINT32 id, SINT32* piValue) const = 0;
    virtual bool GetParameter (SINT32 id, bool* pbValue) const = 0;
    virtual FLOAT64 GetScaleX () const = 0;
    virtual FLOAT64 GetScaleY () const = 0;
    virtual const PIXRECT* GetScreenRectangle () const = 0;         // return pointer to a rect relative to the flight sim window left top corner or NULL if undocked or virtual cockpit
};

class IGaugeCDrawable
{
public:
    virtual ULONG AddRef () = 0;
    virtual ULONG Release () = 0;
    virtual FLAGS32 GetFlags () = 0;
    virtual void Update () = 0;
    virtual void Show (bool on) = 0;
    virtual bool Draw (IGaugeCDrawableDrawParameters* pParameters, PIXPOINT size, HDC hdc, PIMAGE pImage) = 0;
    virtual bool SetupDraw (PIXPOINT size, HDC hdc, PIMAGE pImage) = 0;
    virtual bool GetDraw (IGaugeCDrawableDrawParameters* pParameters)  = 0;

    // flags returened by GetFlags
    enum {
        TAKES_DC = 0x1,             // Draw() method should be called with a valid hdc
        TAKES_PIMAGE = 0x2,         // Draw() method should be called with a pImage
        NOT_RESIZABLE = 0x4,        // can not accept size different than in XML file
        DRAWS_ALPHA = 0x8,          // wants to use alpha channel
        NO_TRANSPARENCY = 0x10,     // image is solid rectangle
        MASK_TRANSPARENCY = 0x20,   // use alpha channel to mask image
        DOUBLE_BUFFER = 0x40        // will draw in background, use double buffering
    };
};

// forward class declaration
class IPanelCCallback;

class IGaugeCCallback {
public:
    virtual ULONG AddRef () = 0;
    virtual ULONG Release () = 0;
    virtual IGaugeCCallback* QueryInterface (PCSTRINGZ pszInterface) = 0;
    virtual void Update () = 0;
    virtual bool GetPropertyValue (SINT32 id, FLOAT64* pValue) = 0;
    virtual bool GetPropertyValue (SINT32 id, PCSTRINGZ* pszValue) = 0;
    virtual bool SetPropertyValue (SINT32 id, FLOAT64 value) = 0;
    virtual bool SetPropertyValue (SINT32 id, PCSTRINGZ szValue) = 0;
    virtual IGaugeCDrawable* CreateGaugeCDrawable (SINT32 id, const IGaugeCDrawableCreateParameters* pParameters) = 0;
};

#define ISERIALIZABLE_GAUGECCALLBACK_NAME  "ISerializableGaugeCCallback"


class NetOut;
class NetIn;

class ISerializableGaugeCCallback: public IGaugeCCallback {
public:
    virtual bool Serialize(NetOut& netout) = 0;
    virtual bool Deserialize(NetIn& netin) = 0;
};

class IAircraftCCallback {
public:
    virtual ULONG AddRef () = 0;
    virtual ULONG Release () = 0;
    virtual IAircraftCCallback* QueryInterface (PCSTRINGZ pszInterface) = 0;
    virtual IGaugeCCallback* CreateGaugeCCallback () = 0;
    virtual void Update () = 0;
};

class IPanelCCallback {
public:
    virtual ULONG AddRef () = 0;
    virtual ULONG Release () = 0;
    virtual IPanelCCallback* QueryInterface (PCSTRINGZ pszInterface) = 0;
    virtual UINT32 GetVersion () = 0;
    virtual IAircraftCCallback*  CreateAircraftCCallback (UINT32 ContainerID) = 0;
    virtual bool  ConvertStringToProperty (PCSTRINGZ keyword, SINT32* pID) = 0;
    virtual bool  ConvertPropertyToString (SINT32 id, PPCSTRINGZ pKeyword) = 0;
    virtual bool  GetPropertyUnits (SINT32 id, ENUM* pEnum) = 0;            // UNITS_UNKNOWN if no units, UNITS_STRING if string
};

#define IFSX_PANELCCALLBACK_NAME "IFSXPanelCCallback"

class IFSXPanelCCallback: public IPanelCCallback 
{
public:
    virtual void Clear() = 0; // clear all internal state and internal list of state variables (as GPS does)
};

#define DECLARE_PANEL_CALLBACK_REFCOUNT(CLASSNAME)  \
    private:                                        \
        ULONG   m_RefCount;                         \
    public:                                         \
        ULONG AddRef ();                            \
        ULONG Release ();                           \
        
#define DEFINE_PANEL_CALLBACK_REFCOUNT(CLASSNAME)   \
    ULONG CLASSNAME::AddRef ()                      \
    {                                               \
        return ++m_RefCount;                        \
    }                                               \
    ULONG CLASSNAME::Release ()                     \
    {                                               \
        ULONG result = --m_RefCount;                \
        if (result < 1)                             \
            delete this;                            \
        return result;                              \
    }

#define INIT_PANEL_CALLBACK_REFCOUNT(CLASSNAME)     \
    m_RefCount = 1;

typedef void (*GAUGE_KEY_EVENT_HANDLER) (ID32 event, UINT32 evdata, PVOID userdata);

typedef enum {
        PANEL_TYPE_PLAIN,               //  Radio Stack
        PANEL_TYPE_HUD,                 //  Aircraft HUD
        PANEL_TYPE_SPECIAL,             //      LSO
        PANEL_TYPE_TEXTURE,             //  Virtual Cockpit Texture
        PANEL_TYPE_MISSION,             //  Context-specific (i.e. displayed during certain missions)

} PANEL_TYPE;
    
    
typedef FLOAT64 FSAPI FN_FLOAT64      ( FLOAT64       val );
typedef               FN_FLOAT64      *PFN_FLOAT64;
    
typedef FLOAT64 FSAPI MODULE_VAR_CB   ( PMODULE_VAR   val );
typedef               MODULE_VAR_CB   *PMODULE_VAR_CB;
    
    
// Seqsel Selection stuff
#define SELECT_NONE                             0
#define SELECT_1                                1
#define SELECT_ZOOM                             2
#define SELECT_MAGNETO                          3
#define SELECT_COM_WHOLE                        4
#define SELECT_COM_FRACTION                     5
#define SELECT_NAV1_WHOLE                       6
#define SELECT_NAV1_FRACTION                    7
#define SELECT_NAV2_WHOLE                       8
#define SELECT_NAV2_FRACTION                    9
#define SELECT_XPNDR_1000                       10
#define SELECT_XPNDR_0100                       11
#define SELECT_XPNDR_0010                       12
#define SELECT_XPNDR_0001                       13
#define SELECT_VOR1                             14
#define SELECT_VOR2                             15
#define SELECT_ENGINE                           16
#define SELECT_DME1                             17
#define SELECT_DME2                             18
#define SELECT_ADF_100                          19
#define SELECT_ADF_010                          20
#define SELECT_ADF_001                          21
#define SELECT_EGT_BUG                          22
#define SELECT_SIM_RATE                         23
#define SELECT_CLOCK_HOURS                      24
#define SELECT_CLOCK_MINUTES                    25
#define SELECT_CLOCK_SECONDS                    26
#define SELECT_COM2_WHOLE                       27
#define SELECT_COM2_FRACTION                    28
#define SELECT_ADF_TENTHS                       29
    
// used for the image_flags field
#define         IMAGE_USE_TRANSPARENCY                  BIT0    // these don't change dynamically and are set
#define         IMAGE_USE_ERASE                         BIT1
#define         IMAGE_USE_BRIGHT                        BIT2
#define         IMAGE_ERASE_ON_FAILURE                  BIT3    // not used now
#define         IMAGE_NO_STATIC_BLENDING                BIT4    // not used now
#define         IMAGE_CREATE_DIBSECTION                 BIT5
#define         IMAGE_BILINEAR_GRAY                     BIT6
#define         IMAGE_BILINEAR_COLOR                    BIT7
#define         IMAGE_PRESERVE_COLOR_IN_HUD             BIT8
#define         IMAGE_CONTAINS_NO_MASK                  BIT9

#define         IMAGE_SPRITE_FORCE_TRANS                BIT10   // not used now
#define         IMAGE_BLT_MASK_ONLY                     BIT11

#define         IMAGE_CONTAINS_MASK                     BIT12   // Image contains mask bits
#define         IMAGE_USE_ALPHA                         BIT13   // Image contains alpha channel
#define         IMAGE_USE_LUMINOUS                      BIT14   // Image is bright when the interior light is on
#define         IMAGE_USE_LUMINOUS_PARTIAL              BIT15       // Parts of image are lit by interior light (alpha channel)
#define         IMAGE_ERASE_ALWAYS                      BIT16
    
#define         IMAGE_HIDDEN_TREE                       BIT25
#define         IMAGE_NO_STRETCH                        BIT27   // these change dynamically
#define         IMAGE_HUD_COLOR_MAP                     BIT28
#define         IMAGE_NO_TRANSLATION                    BIT29
#define         IMAGE_HIDDEN                            BIT30
#define         IMAGE_ON_SCREEN                         BIT31


#define         GET_IMAGE_HIDDEN(element)                       (element->image_flags & IMAGE_HIDDEN)
#define         SHOW_IMAGE(element)                             (element->image_flags &= ~IMAGE_HIDDEN)
#define         HIDE_IMAGE(element)                             (element->image_flags |= IMAGE_HIDDEN)

#define         GET_IMAGE_HIDDEN_TREE(element)                  (element->image_flags & IMAGE_HIDDEN_TREE)
#define         SHOW_IMAGE_TREE(element)                        (element->image_flags &= ~IMAGE_HIDDEN_TREE)
#define         HIDE_IMAGE_TREE(element)                        (element->image_flags |= IMAGE_HIDDEN_TREE)

#define         GET_USE_TRANSPARENCY( element )                 (element->image_flags & IMAGE_USE_TRANSPARENCY)
#define         GET_USE_ERASE( element )                        (element->image_flags & IMAGE_USE_ERASE)
#define         GET_USE_BRIGHT( element )                       (element->image_flags & IMAGE_USE_BRIGHT)
#define         GET_ERASE_ON_FAILURE( element )                 (element->image_flags & IMAGE_ERASE_ON_FAILURE)

#define         GET_ON_SCREEN( element )                        (element->image_flags & IMAGE_ON_SCREEN)
#define         SET_ON_SCREEN( element )                        (element->image_flags |= IMAGE_ON_SCREEN)
#define         SET_OFF_SCREEN( element )                       (element->image_flags &= ~IMAGE_ON_SCREEN)


#define         GET_FAILED( element )                           (element->image_flags & GAUGE_FAILURE)
#define         SET_GAUGE_FAILED( element)                      (element->image_flags |= GAUGE_FAILURE)
#define         SET_GAUGE_NOT_FAILED( element)                  (element->image_flags &= ~GAUGE_FAILURE)

#define         GET_IMAGE_ERASE_ALWAYS( element )               (element->image_flags &   IMAGE_ERASE_ALWAYS)

    
typedef enum ELEMENT_TYPE_ENUM {
        ELEMENT_TYPE_STATIC_IMAGE,
        ELEMENT_TYPE_NEEDLE,
        ELEMENT_TYPE_STRING,
        ELEMENT_TYPE_SLIDER,
        ELEMENT_TYPE_ICON,
        ELEMENT_TYPE_MOVING_IMAGE,
        ELEMENT_TYPE_SPRITE,
}
ELEMENT_TYPE_ENUM;
    
    
#define ASI_ALT_MANUALLY_TUNABLE                BIT0
#define ASI_HEADING_MANUALLY_TUNABLE            BIT1
    
    
typedef struct NONLINEARITY
{
        PIXPOINT        pt;
        FLOAT64         value;
        FLOAT64         degrees;
}
NONLINEARITY, *PNONLINEARITY, **PPNONLINEARITY;

typedef enum    FAILURE_KEY
{
        FAIL_NONE = 0,

        OLD_FAIL_SYSTEM_ELECTRICAL,         // obsolete: dont use it in new gauges; use FAIL_SYSTEM_ELECTRICAL_PANELS
        FAIL_SYSTEM_ENGINE,
        FAIL_SYSTEM_PITOT_STATIC,
        FAIL_SYSTEM_VACUUM,

        FAIL_GAUGE_ADF,
        FAIL_GAUGE_AIRSPEED,
        FAIL_GAUGE_ALTIMETER,
        FAIL_GAUGE_ATTITUDE,
        FAIL_GAUGE_COMMUNICATIONS,              // Used for COM1
        FAIL_GAUGE_FUEL_INDICATORS,
        FAIL_GAUGE_GYRO_HEADING,
        FAIL_GAUGE_MAGNETIC_COMPASS,
        OLD_FAIL_GAUGE_NAVIGATION,              // OBSOLETE: DON'T USE
        FAIL_GAUGE_NAVIGATION_VOR1,             // only vor1
        FAIL_GAUGE_NAVIGATION_VOR2,             // only vor2
        OLD_FAIL_GAUGE_NAVIGATION_BOTH,         // OBSOLETE: DON'T USE
        FAIL_GAUGE_TRANSPONDER,
        FAIL_GAUGE_TURN_COORDINATOR,
        FAIL_GAUGE_VERTICAL_SPEED,

        FAIL_SYSTEM_ELECTRICAL_PANELS,
        FAIL_SYSTEM_ELECTRICAL_AVIONICS,

        FAIL_GAUGE_COMMUNICATIONS_COM2,        // Used for COM2


        FAIL_KEY_MAX
}
FAILURE_KEY, *PFAILURE_KEY, **PPFAILURE_KEY;

typedef enum    FAILURE_ACTION
{
        FAIL_ACTION_NONE = 0,
        FAIL_ACTION_FREEZE,
        FAIL_ACTION_ZERO,
        FAIL_ACTION_NO_DRAW,
        FAIL_ACTION_COVER,

        FAIL_ACTION_MAX
}
FAILURE_ACTION, *PFAILURE_ACTION, **PPFAILURE_ACTION;
    
typedef struct  FAILURE_RECORD
{
        FAILURE_KEY             key;
        FAILURE_ACTION          action;
        PVOID                   reserved;
}
FAILURE_RECORD, *PFAILURE_RECORD, **PPFAILURE_RECORD;
    
typedef struct  IMAGE_SET
{
        PIMAGE  final;
        PIMAGE  source;
        PIMAGE  reserved1;
        PIMAGE  reserved2;
} IMAGE_SET, *PIMAGE_SET, **PPIMAGE_SET;

   
#define PANEL_SERVICE_PRE_QUERY                         0
#define PANEL_SERVICE_POST_QUERY                        1
#define PANEL_SERVICE_PRE_INSTALL                       2       // extra_data = resource_handle
#define PANEL_SERVICE_POST_INSTALL                      3       // extra_data = resource_handle
#define PANEL_SERVICE_PRE_INITIALIZE                    4
#define PANEL_SERVICE_POST_INITIALIZE                   5
#define PANEL_SERVICE_PRE_UPDATE                        6
#define PANEL_SERVICE_POST_UPDATE                       7
#define PANEL_SERVICE_PRE_GENERATE                      8       // extra_data = phase
#define PANEL_SERVICE_POST_GENERATE                     9       // extra_data = phase
#define PANEL_SERVICE_PRE_DRAW                          10
#define PANEL_SERVICE_POST_DRAW                         11
#define PANEL_SERVICE_PRE_KILL                          12
#define PANEL_SERVICE_POST_KILL                         13
#define PANEL_SERVICE_CONNECT_TO_WINDOW                 14      // extra_data = PANEL_WND
#define PANEL_SERVICE_DISCONNECT                        15      // extra_data = PANEL_WND
#define PANEL_SERVICE_PANEL_OPEN                        16
#define PANEL_SERVICE_PANEL_CLOSE                       17
    
    
#define GAUGE_HEADER_VERSION_FS610                      610    // FS 6.10 gauges
#define GAUGE_HEADER_VERSION_FSNEW                      625    // OLD style gauges compiled in the FS70 TREE
#define GAUGE_HEADER_VERSION_CFS1                       650    // CFS1 Gauges
#define GAUGE_HEADER_VERSION_FS700                      700    // new FS7 gauges built after CFS1 is finalized
#define GAUGE_HEADER_VERSION_CFS2                       750    // gauges built after FS2000 shipped and before CFS2 shipped
#define GAUGE_HEADER_VERSION_FS800                      800    // FS2002 gauges 
#define GAUGE_HEADER_VERSION_FS900                      900    // FS2004 gauges
#define GAUGE_HEADER_VERSION_FS1000                     1000   // FSX gauges


typedef struct GAUGEHDR *PGAUGEHDR, **PPGAUGEHDR;
typedef struct ELEMENT_HEADER *PELEMENT_HEADER, **PPELEMENT_HEADER;
typedef struct MOUSERECT *PMOUSERECT, **PPMOUSERECT;

//
// gauge interface routines used by the panels system
//
//
// FS6.1, not used by FS7.0 gauges
typedef UINT32 GENERATE_PHASE;

typedef void FSAPI QUERY_ROUTINE( void );
typedef            QUERY_ROUTINE           *PQUERY_ROUTINE;
    
typedef void FSAPI INSTALL_ROUTINE( PVOID resource_file_handle );
typedef            INSTALL_ROUTINE         *PINSTALL_ROUTINE;

typedef void FSAPI INITIALIZE_ROUTINE( void );
typedef            INITIALIZE_ROUTINE       *PINITIALIZE_ROUTINE;

typedef void FSAPI UPDATE_ROUTINE( void );
typedef            UPDATE_ROUTINE          *PUPDATE_ROUTINE;
    
typedef void FSAPI GENERATE_ROUTINE( UINT32 phase );
typedef            GENERATE_ROUTINE        *PGENERATE_ROUTINE;

typedef void FSAPI DRAW_ROUTINE( void );
typedef            DRAW_ROUTINE            *PDRAW_ROUTINE;

typedef void FSAPI KILL_ROUTINE( void );
typedef            KILL_ROUTINE            *PKILL_ROUTINE;

typedef void FSAPI GAUGE_CALLBACK( PGAUGEHDR pgauge, SINT32 service_id, UINT32 extra_data );
typedef            GAUGE_CALLBACK  *PGAUGE_CALLBACK;

typedef void    SERIALIZE_SIZE_CALLBACK( PGAUGEHDR gauge_header, UINT32* size);
typedef         SERIALIZE_SIZE_CALLBACK *PSERIALIZE_SIZE_CALLBACK;

typedef void    SERIALIZE_CALLBACK( PGAUGEHDR gauge_header, BYTE* pBuf);
typedef         SERIALIZE_CALLBACK *PSERIALIZE_CALLBACK;

// return true if successful
typedef bool    DESERIALIZE_CALLBACK( PGAUGEHDR gauge_header, BYTE* pBuf );
typedef         DESERIALIZE_CALLBACK *PDESERIALIZE_CALLBACK;

typedef void    EVENT_SIZE_CALLBACK( PGAUGEHDR gauge_header, UINT32* size );
typedef         EVENT_SIZE_CALLBACK *PEVENT_SIZE_CALLBACK;

// return true if successful
typedef bool    PROCESS_EVENT_CALLBACK( PGAUGEHDR gauge_header, BYTE* pBuf );
typedef         PROCESS_EVENT_CALLBACK *PPROCESS_EVENT_CALLBACK;

typedef struct  GAUGEHDR
{
    UINT32                  gauge_header_version;
    char                    *gauge_name;         
    PPELEMENT_HEADER        elements_list;               
    PQUERY_ROUTINE          query_routine;               
    PINSTALL_ROUTINE        install_routine;             
    PINITIALIZE_ROUTINE     initialize_routine;          
    PUPDATE_ROUTINE         update_routine;              
    PGENERATE_ROUTINE       generate_routine;            
    PDRAW_ROUTINE           draw_routine;                
    PKILL_ROUTINE           kill_routine;                
    PVOID                   reserved1;
    UINT32                  size_x_mm;           
    UINT32                  size_y_mm;           
    FLOAT32                 x_adjust;            
    FLOAT32                 y_adjust;            
    PVOID                   reserved2;              
    PVOID                   reserved3;   
    PIXPOINT                position;
    PVOID                   reserved4;             
    PMOUSERECT              mouse_rect;          
    PGAUGE_CALLBACK         gauge_callback;              
    UINT32                  user_data;           
    char*                   parameters;          
    char*                   usage;               
    SINT32                  reserved5;
    PVOID                   reserved6;
    PIXPOINT                size;                           
    FLOAT64                 user_area[10];
    FLAGS32                 flags;
    PVOID                   reserved7;
    GUID                         guid;                    
    UINT32                       key_id;                  
    PSERIALIZE_SIZE_CALLBACK     serialize_size_callback; 
    PSERIALIZE_CALLBACK          serialize_callback;      
    PDESERIALIZE_CALLBACK        deserialize_callback;    
    PEVENT_SIZE_CALLBACK         event_size_callback;     
    PPROCESS_EVENT_CALLBACK      process_event_callback;
    PIXPOINT                     original_position;
}
GAUGEHDR;
    
    
#define ELEMENT_INFO                                        \
    ELEMENT_TYPE_ENUM               element_type;           \
    ID                              resource_id;            \
    PIXPOINT                        position;               \
    PIXPOINT                        previous_position;      \
    PIXPOINT                        ofs;                    \
    PGAUGEHDR                       gauge_header;           \
    struct ELEMENT_HEADER           *previous_element;      \
    struct ELEMENT_HEADER           **next_element;         \
    PFAILURE_RECORD                 failure_systems;        \
    FLAGS                           image_flags;            \
    FLAGS                           aircraft_special_instrumentation;\
    FLAGS                           reserved;
    
    
typedef struct ELEMENT_HEADER
{
        ELEMENT_INFO
}
ELEMENT_HEADER;
    
    
#define HEADER union        \
{                           \
    struct                  \
    {                       \
        ELEMENT_INFO        \
    };                      \
    ELEMENT_HEADER header;  \
}
    
    
typedef struct ELEMENT_STATIC_IMAGE *PELEMENT_STATIC_IMAGE,     **PPELEMENT_STATIC_IMAGE;
typedef struct ELEMENT_NEEDLE       *PELEMENT_NEEDLE,           **PPELEMENT_NEEDLE;
typedef struct ELEMENT_STRING           *PELEMENT_STRING,               **PPELEMENT_STRING;
typedef struct ELEMENT_SLIDER           *PELEMENT_SLIDER,               **PPELEMENT_SLIDER;
typedef struct ELEMENT_ICON                     *PELEMENT_ICON,                 **PPELEMENT_ICON;
typedef struct ELEMENT_MOVING_IMAGE     *PELEMENT_MOVING_IMAGE, **PPELEMENT_MOVING_IMAGE;
typedef struct ELEMENT_SPRITE           *PELEMENT_SPRITE,               **PPELEMENT_SPRITE;

typedef FLOAT64 FSAPI NEEDLE_UPDATE_CALLBACK(PELEMENT_NEEDLE pelement);
typedef               NEEDLE_UPDATE_CALLBACK                  *PNEEDLE_UPDATE_CALLBACK;

typedef FLOAT64 FSAPI STRING_UPDATE_CALLBACK(PELEMENT_STRING pelement);
typedef               STRING_UPDATE_CALLBACK                  *PSTRING_UPDATE_CALLBACK;

typedef FLOAT64 FSAPI SLIDER_UPDATE_CALLBACK(PELEMENT_SLIDER pelement);
typedef               SLIDER_UPDATE_CALLBACK                  *PSLIDER_UPDATE_CALLBACK;

typedef FLOAT64 FSAPI ICON_UPDATE_CALLBACK(PELEMENT_ICON pelement);
typedef               ICON_UPDATE_CALLBACK                    *PICON_UPDATE_CALLBACK;

typedef FLOAT64 FSAPI MOVING_IMAGE_UPDATE_CALLBACK(PELEMENT_MOVING_IMAGE pelement);
typedef               MOVING_IMAGE_UPDATE_CALLBACK    *PMOVING_IMAGE_UPDATE_CALLBACK;

typedef FLOAT64 FSAPI SPRITE_UPDATE_CALLBACK(PELEMENT_SPRITE pelement);
typedef               SPRITE_UPDATE_CALLBACK                  *PSPRITE_UPDATE_CALLBACK;
    


typedef struct ELEMENT_STATIC_IMAGE
{
    HEADER;
    IMAGE_SET               image_data;
    PIMAGE                  save_buffer;
    HDC                     hdc;                                    // HDC for Win32 API DrawText
    HBITMAP                 hbmp;                                   // DibSection for Win32 API DrawText
    HBITMAP                 reserved1;
    PIMAGE                  reserved2;
}
ELEMENT_STATIC_IMAGE, *PELEMENT_STATIC_IMAGE, **PPELEMENT_STATIC_IMAGE;
    
#define MAKE_STATIC(    NAME,                                                       \
                        RES_ID,                                                     \
                        NEXT_LIST,                                                  \
                        FAILURE,                                                    \
                        DRAW_FLAGS,                                                 \
                        ASI_FLAGS,                                                  \
                        POSITION_X, POSITION_Y )                                    \
                                                                                    \
    ELEMENT_STATIC_IMAGE        NAME    =                                           \
    {                                                                               \
        ELEMENT_TYPE_STATIC_IMAGE,                                                  \
        RES_ID,                                                                     \
        POSITION_X, POSITION_Y,                                                     \
        {0,0},                                                                      \
        {0,0},                                                                      \
        &GAUGEHDR_VAR_NAME,                                                         \
        NULL,                                                                       \
        (PPELEMENT_HEADER)(NEXT_LIST),                                              \
        (PFAILURE_RECORD)(FAILURE),                                                 \
        DRAW_FLAGS,                                                                 \
        ASI_FLAGS,                                                                  \
        0                                                                           \
    };
    
    
    
typedef struct ELEMENT_NEEDLE
{
    HEADER;
    MODULE_VAR              source_var;             
    PMODULE_VAR_CB          source_var_cb;                                                              
    PFN_FLOAT64             display_value_fn;       
    PFN_FLOAT64             angle_fn;               
    PFN_FLOAT64             lag_fn;                 
    PIXPOINT                reference;              
    PNONLINEARITY           nonlinearity_table;             
    UINT32                  nonlinearity_table_size;
    SINT32                  max_degrees_per_tick;                                               
    FLOAT64                 readout_goal;                                                       
    FLOAT64                 theta_goal;                                                         
    FLOAT64                 theta;                                                              
    PIXPOINT                reference_old;                                                      
    PIXPOINT                o0, o1, o2, o3;                                                     
    IMAGE_SET               needle_data;                                                        
    PIMAGE                  texture_data;           
    UINT32                  texture_handle;                                                     
    PIMAGE                  save_buffer;            
    PIXPOINT                save_position;
    PNEEDLE_UPDATE_CALLBACK update_cb;
    PIXPOINT                save_size;
}
ELEMENT_NEEDLE, *PELEMENT_NEEDLE, **PPELEMENT_NEEDLE;

#define MAKE_NEEDLE(    NAME,                                                       \
                        RES_ID,                                                     \
                        NEXT_LIST,                                                  \
                        FAILURE,                                                    \
                        DRAW_FLAGS,                                                 \
                        ASI_FLAGS,                                                  \
                        BKND_POSITION_X, BKND_POSITION_Y,                           \
                        NDL_POSITION_X, NDL_POSITION_Y,                             \
                        SOURCE_VAR, CALLBACK,                                       \
                        NONLINEARITY_TABLE,                                         \
                        MAX_DEG_PER_SEC )                                           \
                                                                                    \
    ELEMENT_NEEDLE              NAME    =                                           \
    {                                                                               \
        ELEMENT_TYPE_NEEDLE,                                                        \
        RES_ID,                                                                     \
        BKND_POSITION_X, BKND_POSITION_Y,                                           \
        {0,0},                                                                      \
        NDL_POSITION_X, NDL_POSITION_Y,                                             \
        &GAUGEHDR_VAR_NAME,                                                         \
        NULL,                                                                       \
        (PPELEMENT_HEADER)(NEXT_LIST),                                              \
        (PFAILURE_RECORD)(FAILURE),                                                 \
        DRAW_FLAGS,                                                                 \
        ASI_FLAGS,                                                                  \
        0,                                                                          \
                                                                                    \
        {SOURCE_VAR},                                                               \
        NULL,                                                                       \
        NULL,                                                                       \
        NULL,                                                                       \
        NULL,                                                                       \
        0, 0,                                                                       \
        NONLINEARITY_TABLE,                                                         \
        sizeof( NONLINEARITY_TABLE )/sizeof( NONLINEARITY ),                        \
        MAX_DEG_PER_SEC,                                                            \
        0.0,                                                                        \
        0.0,                                                                        \
        0.0,                                                                        \
        {0,0},                                                                      \
        {0,0},{0,0},{0,0},{0,0},                                                    \
        {0},                                                                        \
        NULL,                                                                       \
        0,                                                                          \
        0,                                                                          \
        {0,0},                                                                      \
        CALLBACK,                                                                   \
        {0,0},                                                                      \
    };

    
    
typedef struct SEQ_REC
{
    SINT32 seq_id;
    SINT32 sel_str;
    SINT32 sel_end;
}
SEQ_REC, *PSEQ_REC, **PPSEQ_REC;

#define STR_UNSEL(ELEMENT)                      ((ELEMENT)->sel_end = -1, (ELEMENT)->sel_str = -1)
#define STR_SEL(ELEMENT, STR, END)              ((ELEMENT)->sel_end = (END), (ELEMENT)->sel_str = (STR))
#define IS_STR_SEL(ELEMENT)                     ((ELEMENT)->sel_end != -1 && (ELEMENT)->sel_str != -1)
    
typedef struct ELEMENT_STRING
{
    HEADER;                                     
    MODULE_VAR              source_var[3];  
    FLOAT64                 save_var[3];    
    PMODULE_VAR_CB          source_var_cb[3];       
    PFN_FLOAT64             source_fn;      
    PCHAR                   string;         
    PCHAR                   string_old;     
    PIXPOINT                img_size;       
    PIMAGE                  string_data;    
    PIMAGE                  save_buffer;                                                
    COLORREF                fg_color;       
    COLORREF                bg_color;       
    COLORREF                hilite_color;   
    HDC                     hdc;    
    HFONT                   hfont;          
    HFONT                   reserved1;      
    HBITMAP                 hbmp;           
    HBITMAP                 reserved2;       
    HBRUSH                  hbrush;         
    HBRUSH                  reserved3;     
    char                    font_name[32];  
    SINT32                  font_weight;    
    UINT8                   font_charset;   
    UINT32                  draw_text_flags;
    UINT16                  max_string_length;
    UINT16                  char_width;       
    SINT16                  sel_str;          
    SINT16                  sel_end;          
    PSEQ_REC                seq;              
    SINT32                  save_global_seq;
    SINT32                  char_height;
    PSTRING_UPDATE_CALLBACK update_cb;
    BOOL                    use_user_color;
    COLORREF                user_color;
}
ELEMENT_STRING, *PELEMENT_STRING, **PPELEMENT_STRING;
    
#define MAKE_STRING(    NAME,                                                       \
                        NEXT_LIST,                                                  \
                        FAILURE,                                                    \
                        DRAW_FLAGS,                                                 \
                        ASI_FLAGS,                                                  \
                        POSITION_X, POSITION_Y,                                     \
                        SIZE_X, SIZE_Y,                                             \
                        NUM_CHARS,                                                  \
                        SOURCE_VAR_1,                                               \
                        SOURCE_VAR_2,                                               \
                        SOURCE_VAR_3,                                               \
                        FORECOLOR,                                                  \
                        BACKCOLOR,                                                  \
                        HILITECOLOR,                                                \
                        FONT_NAME,                                                  \
                        FONT_WEIGHT,                                                \
                        FONT_CHARSET,                                               \
                        FONT_SIZE,                                                  \
                        DRAW_TEXT_FLAGS,                                            \
                        HILITE_LIST,                                                \
                        CALLBACK)                                                   \
                                                                                    \
    ELEMENT_STRING              NAME    =                                           \
    {                                                                               \
        ELEMENT_TYPE_STRING,                                                        \
        -1,                                                                         \
        POSITION_X, POSITION_Y,                                                     \
        {0,0},                                                                      \
        {0,0},                                                                      \
        &GAUGEHDR_VAR_NAME,                                                         \
        NULL,                                                                       \
        (PPELEMENT_HEADER)(NEXT_LIST),                                              \
        (PFAILURE_RECORD)(FAILURE),                                                 \
        DRAW_FLAGS,                                                                 \
        ASI_FLAGS,                                                                  \
        0,                                                                          \
                                                                                    \
        {{SOURCE_VAR_1}, {SOURCE_VAR_2}, {SOURCE_VAR_3}},                           \
        {0, 0, 0},                                                                  \
        {NULL, NULL, NULL},                                                         \
        NULL,                                                                       \
        NULL,                                                                       \
        NULL,                                                                       \
        SIZE_X, SIZE_Y,                                                             \
        NULL,                                                                       \
        NULL,                                                                       \
        FORECOLOR,                                                                  \
        BACKCOLOR,                                                                  \
        HILITECOLOR,                                                                \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        FONT_NAME,                                                                  \
        FONT_WEIGHT,                                                                \
        FONT_CHARSET,                                                               \
        DRAW_TEXT_FLAGS,                                                            \
        NUM_CHARS,                                                                  \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        HILITE_LIST,                                                                \
        0,                                                                          \
        FONT_SIZE,                                                                  \
        CALLBACK,                                                                   \
        FALSE,                                                                      \
        0,                                                                          \
    };
    
    
typedef struct ELEMENT_SLIDER
{
    HEADER;                              
    MODULE_VAR              source_var_x;
    PMODULE_VAR_CB          source_var_x_cb;     
    FLOAT64                 scale_x;     
    PFN_FLOAT64             lag_fn_x;                                             
    MODULE_VAR              source_var_y;
    PMODULE_VAR_CB          source_var_y_cb;     
    FLOAT64                 scale_y;     
    PFN_FLOAT64             lag_fn_y;    
    IMAGE_SET               slider_data; 
    PIMAGE                  save_buffer;
    PSLIDER_UPDATE_CALLBACK update_x_cb;
    PSLIDER_UPDATE_CALLBACK update_y_cb;
    FLOAT64 previous_x;
    FLOAT64 previous_y;
}
ELEMENT_SLIDER, *PELEMENT_SLIDER, **PPELEMENT_SLIDER;
    
#define MAKE_SLIDER(    NAME,                                                       \
                        RES_ID,                                                     \
                        NEXT_LIST,                                                  \
                        FAILURE,                                                    \
                        DRAW_FLAGS,                                                 \
                        ASI_FLAGS,                                                  \
                        POSITION_X, POSITION_Y,                                     \
                        SOURCE_VAR_X, CALLBACK_X, SCALE_X,                          \
                        SOURCE_VAR_Y, CALLBACK_Y, SCALE_Y )                         \
                                                                                    \
    ELEMENT_SLIDER              NAME    =                                           \
    {                                                                               \
        ELEMENT_TYPE_SLIDER,                                                        \
        RES_ID,                                                                     \
        POSITION_X, POSITION_Y,                                                     \
        {0,0},                                                                      \
        {0,0},                                                                      \
        &GAUGEHDR_VAR_NAME,                                                         \
        NULL,                                                                       \
        (PPELEMENT_HEADER)(NEXT_LIST),                                              \
        (PFAILURE_RECORD)(FAILURE),                                                 \
        DRAW_FLAGS,                                                                 \
        ASI_FLAGS,                                                                  \
        0,                                                                          \
                                                                                    \
        {SOURCE_VAR_X},                                                             \
        NULL,                                                                       \
        SCALE_X,                                                                    \
        NULL,                                                                       \
        {SOURCE_VAR_Y},                                                             \
        NULL,                                                                       \
        SCALE_Y,                                                                    \
        NULL,                                                                       \
        {0},                                                                        \
        NULL,                                                                       \
        CALLBACK_X,                                                                 \
        CALLBACK_Y                                                                  \
    };
    
    
    
// Icon Switch Types
#define ICON_SWITCH_TYPE_SET_CUR_ICON                   0
#define ICON_SWITCH_TYPE_SET_CUR_USING_RANGE            1
#define ICON_SWITCH_TYPE_STEP_TO                        3
#define ICON_SWITCH_TYPE_STEP_TO_USING_RANGE            4
    

typedef struct ELEMENT_ICON
{
    HEADER;                                
    MODULE_VAR              source_var;    
    PMODULE_VAR_CB          source_var_cb;         
    SINT32                  switch_type;   
    SINT32                  switch_count;  
    FLOAT64                 scale;         
    SINT32                  offset;        
    SINT32                  num_icons;     
    SINT32                  cur_icon;      
    SINT32                  dst_icon;      
    PIMAGE_SET              picon_data;    
    PIMAGE                  save_buffer;
    PICON_UPDATE_CALLBACK   update_cb;
    ID                      reserved1;
}
ELEMENT_ICON, *PELEMENT_ICON, **PPELEMENT_ICON;
    
#define MAKE_ICON(      NAME,                                                       \
                        RES_ID,                                                     \
                        NEXT_LIST,                                                  \
                        FAILURE,                                                    \
                        DRAW_FLAGS,                                                 \
                        ASI_FLAGS,                                                  \
                        POSITION_X, POSITION_Y,                                     \
                        SOURCE_VAR, CALLBACK,                                       \
                        SWITCH_TYPE,                                                \
                        NUM_ICONS,                                                  \
                        SCALE,                                                      \
                        OFFSET )                                                    \
                                                                                    \
    ELEMENT_ICON                NAME    =                                           \
    {                                                                               \
        ELEMENT_TYPE_ICON,                                                          \
        RES_ID,                                                                     \
        POSITION_X, POSITION_Y,                                                     \
        {0,0},                                                                      \
        {0,0},                                                                      \
        &GAUGEHDR_VAR_NAME,                                                         \
        NULL,                                                                       \
        (PPELEMENT_HEADER)(NEXT_LIST),                                              \
        (PFAILURE_RECORD)(FAILURE),                                                 \
        DRAW_FLAGS,                                                                 \
        ASI_FLAGS,                                                                  \
        0,                                                                          \
                                                                                    \
        {SOURCE_VAR},                                                               \
        NULL,                                                                       \
        SWITCH_TYPE,                                                                \
        0,                                                                          \
        SCALE,                                                                      \
        OFFSET,                                                                     \
        NUM_ICONS,                                                                  \
        0,                                                                          \
        0,                                                                          \
        NULL,                                                                       \
        NULL,                                                                       \
        CALLBACK,                                                                   \
    };
    
    
    
typedef struct ELEMENT_MOVING_IMAGE
{
        HEADER;                                   
        MODULE_VAR          source_var_x;     
        PMODULE_VAR_CB      source_var_x_cb;          
        FLOAT64             min_x;            
        FLOAT64             max_x;            
        PFN_FLOAT64         lag_x;            
        MODULE_VAR          source_var_y;     
        PMODULE_VAR_CB      source_var_y_cb;          
        FLOAT64             min_y;            
        FLOAT64             max_y;            
        PFN_FLOAT64         lag_y;            
        FLOAT64             scale_x;          
        FLOAT64             scale_y;          
        FLOAT64             offset_x;         
        FLOAT64             offset_y;         
        PIXPOINT            save_position;    
        IMAGE_SET           image_data;       
        IMAGE_SET           local_mask_data;  
        PIMAGE              mask_data;        
        PIMAGE              save_buffer;
        PMOVING_IMAGE_UPDATE_CALLBACK update_x_cb;
        PMOVING_IMAGE_UPDATE_CALLBACK update_y_cb;
        FLOAT64             previous_x;
        FLOAT64             previous_y;
}
ELEMENT_MOVING_IMAGE, *PELEMENT_MOVING_IMAGE, **PPELEMENT_MOVING_IMAGE;
    
#define MAKE_MOVING(    NAME,                                                       \
                        RES_ID,                                                     \
                        NEXT_LIST,                                                  \
                        FAILURE,                                                    \
                        DRAW_FLAGS,                                                 \
                        ASI_FLAGS,                                                  \
                        POSITION_X, POSITION_Y,                                     \
                        SOURCE_VAR_X, CALLBACK_X,                                   \
                        MIN_X, MAX_X,                                               \
                        SOURCE_VAR_Y, CALLBACK_Y,                                   \
                        MIN_Y, MAX_Y )                                              \
                                                                                    \
    ELEMENT_MOVING_IMAGE        NAME    =                                           \
    {                                                                               \
        ELEMENT_TYPE_MOVING_IMAGE,                                                  \
        RES_ID,                                                                     \
        POSITION_X, POSITION_Y,                                                     \
        {0,0},                                                                      \
        {0,0},                                                                      \
        &GAUGEHDR_VAR_NAME,                                                         \
        NULL,                                                                       \
        (PPELEMENT_HEADER)(NEXT_LIST),                                              \
        (PFAILURE_RECORD)(FAILURE),                                                 \
        DRAW_FLAGS,                                                                 \
        ASI_FLAGS,                                                                  \
        0,                                                                          \
                                                                                    \
        {SOURCE_VAR_X},                                                             \
        NULL,                                                                       \
        MIN_X,                                                                      \
        MAX_X,                                                                      \
        NULL,                                                                       \
        {SOURCE_VAR_Y},                                                             \
        NULL,                                                                       \
        MIN_Y,                                                                      \
        MAX_Y,                                                                      \
        NULL,                                                                       \
        0.0,                                                                        \
        0.0,                                                                        \
        0.0,                                                                        \
        0.0,                                                                        \
        {0,0},                                                                      \
        {0},                                                                        \
        {0},                                                                        \
        NULL,                                                                       \
        NULL,                                                                       \
        CALLBACK_X,                                                                 \
        CALLBACK_Y                                                                  \
    };
    
    
    
typedef struct ELEMENT_SPRITE
{
    HEADER;
    MODULE_VAR              source_var_x;      
    PMODULE_VAR_CB          source_var_x_cb;           
    FLOAT64                 scale_x;           
    MODULE_VAR              source_var_y;      
    PMODULE_VAR_CB          source_var_y_cb;           
    FLOAT64                 scale_y;           
    MODULE_VAR              source_var_0;      
    PMODULE_VAR_CB          source_var_0_cb;           
    FLOAT64                 scale_0;           
    FLOAT32                 texture_scale_x; 
    FLOAT32                 texture_scale_y; 
    PIXPOINT                reference;         
    PIXPOINT                reference_old;     
    PIXPOINT                o0, o1, o2, o3;    
    PIXPOINT                os0, os1, os2, os3;
    IMAGE_SET               sprite_data;       
    PIMAGE                  texture_data;      
    UINT32                  texture_handle;    
    IMAGE_SET               local_mask_data;   
    PIMAGE                  mask_data;         
    PIMAGE                  save_buffer;
    PSPRITE_UPDATE_CALLBACK update_x_cb;
    PSPRITE_UPDATE_CALLBACK update_y_cb;
    PSPRITE_UPDATE_CALLBACK update_0_cb;
    FLOAT64                 previous_x;
    FLOAT64                 previous_y;
    FLOAT64                 previous_0;
}
ELEMENT_SPRITE, *PELEMENT_SPRITE, **PPELEMENT_SPRITE;

#define MAKE_SPRITE(    NAME,                                                       \
                        RES_ID,                                                     \
                        NEXT_LIST,                                                  \
                        FAILURE,                                                    \
                        DRAW_FLAGS,                                                 \
                        ASI_FLAGS,                                                  \
                        BKND_POSITION_X, BKND_POSITION_Y,                           \
                        TEXTURE_CENTER_X, TEXTURE_CENTER_Y,                         \
                        TEXTURE_SCALE_X, TEXTURE_SCALE_Y,                           \
                        SOURCE_VAR_X, CALLBACK_X, SCALE_X,                          \
                        SOURCE_VAR_Y, CALLBACK_Y, SCALE_Y,                          \
                        SOURCE_VAR_0, CALLBACK_0, SCALE_0 )                         \
                                                                                    \
    ELEMENT_SPRITE              NAME    =                                           \
    {                                                                               \
        ELEMENT_TYPE_SPRITE,                                                        \
        RES_ID,                                                                     \
        {BKND_POSITION_X, BKND_POSITION_Y},                                         \
        {0,0},                                                                      \
        {TEXTURE_CENTER_X, TEXTURE_CENTER_Y},                                       \
        &GAUGEHDR_VAR_NAME,                                                         \
        NULL,                                                                       \
        (PPELEMENT_HEADER)(NEXT_LIST),                                              \
        (PFAILURE_RECORD)(FAILURE),                                                 \
        DRAW_FLAGS,                                                                 \
        ASI_FLAGS,                                                                  \
        0,                                                                          \
                                                                                    \
        {SOURCE_VAR_X},                                                             \
        NULL,                                                                       \
        SCALE_X,                                                                    \
        {SOURCE_VAR_Y},                                                             \
        NULL,                                                                       \
        SCALE_Y,                                                                    \
        {SOURCE_VAR_0},                                                             \
        NULL,                                                                       \
        SCALE_0,                                                                    \
        (FLOAT32)TEXTURE_SCALE_X, (FLOAT32)TEXTURE_SCALE_Y,                         \
        {0,0},                                                                      \
        {0,0},                                                                      \
        {0,0},{0,0},{0,0},{0,0},                                                    \
        {0,0},{0,0},{0,0},{0,0},                                                    \
        {0},                                                                        \
        NULL,                                                                       \
        0,                                                                          \
        {0},                                                                        \
        NULL,                                                                       \
        NULL,                                                                       \
        CALLBACK_X,                                                                 \
        CALLBACK_Y,                                                                 \
        CALLBACK_0                                                                  \
    };



// defines for mouse sense rect stuff
typedef enum
{
    MOUSE_RECT_EOL,
    MOUSE_RECT_PARENT,
    MOUSE_RECT_CHILD,
    MOUSE_RECT_END_PARENT,
    MOUSE_RECT_USER,
} MOUSE_RECT_TYPE;

typedef enum
{
    CURSOR_NONE = -1,
    CURSOR_NORMAL = 0,
    CURSOR_UPARROW,
    CURSOR_DOWNARROW,
    CURSOR_LEFTARROW,
    CURSOR_RIGHTARROW,
    CURSOR_HAND,
    CURSOR_CROSSHAIR,
    CURSOR_GRAB,
} CURSOR_TYPE;
    

typedef struct  MOUSECALLBACK
{
    PIXPOINT    relative_point;
    PVOID       user_data;
    PMOUSERECT  mouse;
    PIXPOINT    screen_point;       // window client coordinates of the click
    PVOID       reserved;
} MOUSECALLBACK, *PMOUSECALLBACK, **PPMOUSECALLBACK;

// arg1 is actually pointer to MOUSECALLBACK, but for compatibility is defined here as a pointer to PIXPOINT
typedef BOOL FSAPI  MOUSE_FUNCTION( PPIXPOINT arg1, FLAGS32 mouse_flags );      
typedef             MOUSE_FUNCTION      *PMOUSE_FUNCTION;

#define GAUGEHDR_FOR_MOUSE_CALLBACK(PIXPOINT) ((PGAUGEHDR)(((PMOUSECALLBACK)(PIXPOINT))->user_data))

typedef struct  MOUSERECT
{
    MOUSE_RECT_TYPE rect_type;          // type of mouse rectangle (parent, child, eol)
    PIXBOX          relative_box;       // relative sense rectangle (relative to parameter to register function)
    CURSOR_TYPE     cursor;             // cursor to display when over this window
    ID              help_id;            // pop-up help id
    FLAGS           mouse_flags;        // types of mouse activities to look for
    ID              event_id;           // event to generate if mouse_flags is satisfied (implies simple mouse_flags)
    PMOUSE_FUNCTION mouse_function;     // function to call if mouse_flag is satisfied
    PVOID           api_data;           // data for FS6API's use
} MOUSERECT;


#define HELP_NONE           0

// Mouse rectangle macros

#define MOUSE_RIGHTSINGLE   BIT31
#define MOUSE_MIDDLESINGLE  BIT30
#define MOUSE_LEFTSINGLE    BIT29
#define MOUSE_RIGHTDOUBLE   BIT28
#define MOUSE_MIDDLEDOUBLE  BIT27
#define MOUSE_LEFTDOUBLE    BIT26
#define MOUSE_RIGHTDRAG     BIT25
#define MOUSE_MIDDLEDRAG    BIT24
#define MOUSE_LEFTDRAG      BIT23
#define MOUSE_MOVE          BIT22
#define MOUSE_DOWN_REPEAT   BIT21
#define MOUSE_RIGHTRELEASE  BIT19
#define MOUSE_MIDDLERELEASE BIT18
#define MOUSE_LEFTRELEASE   BIT17
#define MOUSE_WHEEL_FLIP    BIT16   // invert direction of mouse wheel
#define MOUSE_WHEEL_SKIP    BIT15   // look at next 2 rect for mouse wheel commands
#define MOUSE_WHEEL_UP      BIT14
#define MOUSE_WHEEL_DOWN    BIT13
#define MOUSE_MOVE_REPEAT   BIT12   // if used with MOUSE_*DRAG generates MOVE_MOVE every so often
#define MOUSE_LEAVE         BIT11   // to be used with MOUSE_DOWN_REPEAT
#define MOUSE_GETALL        BIT10   // used to get all mouse messages happend over the rect
#define MOUSE_LEFTALL       (MOUSE_LEFTSINGLE | MOUSE_LEFTDRAG | MOUSE_LEFTRELEASE)
#define MOUSE_RIGHTALL      (MOUSE_RIGHTSINGLE | MOUSE_RIGHTDRAG | MOUSE_RIGHTRELEASE)
#define MOUSE_MIDDLEALL     (MOUSE_MIDDLESINGLE | MOUSE_MIDDLEDRAG | MOUSE_MIDDLERELEASE)
#define MOUSE_WHEEL         (MOUSE_WHEEL_DOWN|MOUSE_WHEEL_UP)

#define MOUSE_NONE          0

#define MOUSE_MAXIMUM_TOOLTIP_TEXT 80

#define MOUSE_PARENT_BEGIN( x, y, w, h, helpid )                        {MOUSE_RECT_PARENT, {x, y, w, h}, CURSOR_NONE, helpid, MOUSE_NONE, 0, NULL, NULL},

#define MOUSE_PARENT_END                                                {MOUSE_RECT_END_PARENT, {0, 0, 0, 0}, CURSOR_NONE, HELP_NONE, MOUSE_NONE, 0, NULL, NULL},

#define MOUSE_PARENT( x, y, w, h, helpid )                              MOUSE_PARENT_BEGIN( x, y, w, h, helpid )                    \
                                                                        MOUSE_PARENT_END

#define MOUSE_BEGIN( name, helpid, x, y )                               MOUSERECT   name[]  = {                                     \
                                                                        MOUSE_PARENT_BEGIN( x, y, 0, 0, helpid )

#define MOUSE_END                                                       MOUSE_PARENT_END                                            \
                                                                        {MOUSE_RECT_EOL, {0, 0, 0, 0}, CURSOR_NONE, HELP_NONE, MOUSE_NONE, 0, NULL, NULL}};

#define MOUSE_CHILD_EVENT( x, y, w, h, cursor, mouse_flags, event_id )  {MOUSE_RECT_CHILD, {x, y, w, h}, cursor, HELP_NONE, mouse_flags, event_id, NULL, NULL},
#define MOUSE_CHILD_FUNCT( x, y, w, h, cursor, mouse_flags, function )  {MOUSE_RECT_CHILD, {x, y, w, h}, cursor, HELP_NONE, mouse_flags, 0, function, NULL},




    
// Dynamic tooltips' stuff

#define MOUSE_TOOLTIP_TEXT_MACRO(HELPID, HELPID_METRIC, HELPID_US, STRING, ARGS, NARG)  \
                            {MOUSE_RECT_USER, {0, 0, (HELPID_METRIC), (HELPID_US)}, CURSOR_NONE, (HELPID), MOUSE_NONE, (ID)(NARG), (BOOL (FSAPI *)(PPIXPOINT , FLAGS32))(PVOID)(STRING), (ARGS)}
#define MOUSE_TOOLTIP_TEXT(HELPID, HELPID_METRIC, HELPID_US, STRING, ARGS, NARG)    MOUSE_TOOLTIP_TEXT_MACRO(HELPID, HELPID_METRIC, HELPID_US, STRING, ARGS, NARG),
#define MOUSE_TOOLTIP_TEXT_3ID_NARG(HELPID, HELPID_METRIC, HELPID_US, ARGS, NARG)   MOUSE_TOOLTIP_TEXT (HELPID, HELPID_METRIC, HELPID_US, NULL, ARGS, NARG)
#define MOUSE_TOOLTIP_TEXT_ID_NARG(HELPID, ARGS, NARG)                              MOUSE_TOOLTIP_TEXT_3ID_NARG ((HELPID), HELP_NONE, HELP_NONE, (ARGS), (NARG))
#define MOUSE_TOOLTIP_TEXT_STRING_NARG(STRING, ARGS, NARG)                          MOUSE_TOOLTIP_TEXT (HELP_NONE, HELP_NONE, HELP_NONE, STRING, ARGS, NARG)
#define MOUSE_TOOLTIP_TEXT_ID(HELPID, ARGS)                                         MOUSE_TOOLTIP_TEXT_ID_NARG ((HELPID), (ARGS), sizeof(ARGS)/sizeof(MOUSE_ARG))
#define MOUSE_TOOLTIP_TEXT_STRING(STRING, ARGS)                                     MOUSE_TOOLTIP_TEXT_STRING_NARG ((STRING), (ARGS), sizeof(ARGS)/sizeof(MOUSE_ARG))
#define MOUSE_TOOLTIP_STRING(STRING)                                                MOUSE_TOOLTIP_TEXT_STRING_NARG ((STRING), NULL, 0)
#define MOUSE_TOOLTIP_ID(HELPID)                                                    MOUSE_TOOLTIP_TEXT_ID_NARG ((HELPID), NULL, 0)
#define MOUSE_TOOLTIP_3ID(HELPID, HELPID_METRIC, HELPID_US)                         MOUSE_TOOLTIP_TEXT_3ID_NARG ((HELPID), (HELPID_METRIC), (HELPID_US), NULL, 0)
#define MOUSE_TOOLTIP_STANDARD_STRING(STRING)                                       MOUSE_TOOLTIP_TEXT_STRING (STRING, 0)

#define MAKE_MOUSE_TOOLTIP_STANDARD_STRING(PARAMETER, UNITS)                        "@" ## #PARAMETER ## "@" ## #UNITS
#define MOUSE_TOOLTIP_STANDARD(PARAMETER, UNITS)                                    MOUSE_TOOLTIP_STANDARD_STRING (MAKE_MOUSE_TOOLTIP_STANDARD_STRING (PARAMETER, UNITS))
    
    
typedef struct {
        FLOAT64 source;
        FLOAT64 value;
} MOUSE_ARG_NUMERIC_MAP;
typedef struct {
        SINT32 source;
        ID value;
} MOUSE_ARG_ID_MAP;
typedef struct {
        SINT32 source;
        PSTRINGZ value;
} MOUSE_ARG_STRING_MAP;

typedef struct {
        MODULE_VAR      source_var;
        FLOAT64         scale;
        MOUSE_ARG_NUMERIC_MAP *numeric_table;
        UINT            numeric_table_size;
        MOUSE_ARG_ID_MAP *id_table;
        UINT            id_table_size;
        MOUSE_ARG_STRING_MAP *string_table;
        UINT            string_table_size;
        FLOAT64         (FSAPI *numeric_callback) (FLOAT64 number, ID id, PCSTRINGZ string, MODULE_VAR *source_var, PGAUGEHDR gauge);
        ID              (FSAPI *id_callback) (FLOAT64 number, ID id, PCSTRINGZ string, MODULE_VAR *source_var, PGAUGEHDR gauge);
        PCSTRINGZ       (FSAPI *string_callback) (FLOAT64 number, ID id, PCSTRINGZ string, MODULE_VAR *source_var, PGAUGEHDR gauge);
} MOUSE_ARG;

#define MOUSE_TOOLTIP_ARGS(NAME)        MOUSE_ARG (NAME)[] = {
#define MOUSE_TOOLTIP_ARG(VAR, SCALE, NUMERIC_TABLE, ID_TABLE, STRING_TABLE, NUMERIC_CALLBACK, ID_CALLBACK, STRING_CALLBACK) \
                {{(VAR)}, (SCALE), \
                 (NUMERIC_TABLE), sizeof(NUMERIC_TABLE)/sizeof(MOUSE_ARG_NUMERIC_MAP), \
                 (ID_TABLE), sizeof(ID_TABLE)/sizeof(MOUSE_ARG_ID_MAP), \
                 (STRING_TABLE), sizeof(STRING_TABLE)/sizeof(MOUSE_ARG_STRING_MAP), \
                 (NUMERIC_CALLBACK), (ID_CALLBACK), (STRING_CALLBACK)},
#define MOUSE_TOOLTIP_ARG_NUMBER(VAR, SCALE) MOUSE_TOOLTIP_ARG((VAR), (SCALE), NULL, NULL, NULL, NULL, NULL, NULL)
#define MOUSE_TOOLTIP_ARG_HELP_ID(VAR, ID_TABLE) MOUSE_TOOLTIP_ARG((VAR), 1, NULL, (ID_TABLE), NULL, NULL, NULL, NULL)
#define MOUSE_TOOLTIP_ARG_NUMBER_FUNCT(VAR, FUNCT) MOUSE_TOOLTIP_ARG((VAR), 1, NULL, NULL, NULL, (FUNCT), NULL, NULL)
#define MOUSE_TOOLTIP_ARG_HELP_ID_FUNCT(VAR, FUNCT) MOUSE_TOOLTIP_ARG((VAR), 1, NULL, NULL, NULL, NULL, (FUNCT), NULL)
#define MOUSE_TOOLTIP_ARGS_END          };
    

#define FS9LINK_VERSION     0x0900


#define FSLINKAGE       ID          ModuleID;                       \
                        void        (FSAPI *ModuleInit)(void);      \
                        void        (FSAPI *ModuleDeinit)(void);    \
                        FLAGS32     ModuleFlags;                    \
                        UINT32      ModulePriority;                 \
                        UINT32      ModuleVersion;

// Module Export Structure
typedef struct  PANELS
{
    FSLINKAGE
    void        (FSAPI *reserved1) (void);  
    void        (FSAPI *reserved2) (void);  
    void        (FSAPI *reserved3) (void);  
    void        (FSAPI *reserved4) (void);  
    void        (FSAPI *reserved5) (void);  
    BOOL        (FSAPI *is_panel_window_visible_ident) (UINT32 panel_id);
    ENUM        (FSAPI *tooltip_units_getset) (int action, ENUM type);  // action < 0 - toggle(ignore 'type'), = 0 get(ignore 'type'), > 0 set(use 'type')
    void        (FSAPI *reserved7) (void);  
    void        (FSAPI *reserved8) (void);  
    void        (FSAPI *reserved9) (void);  
    void        (FSAPI *reserved10) (void); 
    void        (FSAPI *reserved11) (void); 
    void        (FSAPI *reserved12) (void); 
    void        (FSAPI *reserved13) (void); 
    void        (FSAPI *reserved14) (void); 
    void        (FSAPI *reserved15) (void); 
    void        (FSAPI *reserved16) (void); 
    void        (FSAPI *reserved17) (void); 
    void        (FSAPI *element_list_query) ( PELEMENT_HEADER element );    
    void        (FSAPI *element_list_install) ( PELEMENT_HEADER element, PVOID resource_file_handle );  
    void        (FSAPI *element_list_initialize) ( PELEMENT_HEADER element );   
    void        (FSAPI *element_list_update) ( PELEMENT_HEADER element );   
    void        (FSAPI *element_list_generate) ( PELEMENT_HEADER element, GENERATE_PHASE phase );   
    void        (FSAPI *element_list_plot) ( PELEMENT_HEADER element ); 
    void        (FSAPI *element_list_erase) ( PELEMENT_HEADER element );    
    void        (FSAPI *element_list_kill) ( PELEMENT_HEADER element ); 
    void        (FSAPI *mouse_list_install) ( PMOUSERECT rect, PGAUGEHDR gauge_header, PPIXPOINT size );    
    void        (FSAPI *mouse_list_register) ( PMOUSERECT rect, PGAUGEHDR gauge_header );   
    void        (FSAPI *mouse_list_unregister) ( PMOUSERECT rect, PGAUGEHDR gauge_header ); 
    BOOL        (FSAPI *panel_window_toggle) (UINT32 panel_id); 
    ERR         (FSAPI *trigger_key_event) ( ID32 event_id, UINT32 value ); 
    void        (FSAPI *register_var_by_name) ( PVOID var, VAR_TYPE var_type, PSTRINGZ name );  
    void        (FSAPI *initialize_var) ( PMODULE_VAR module_var ); 
    void        (FSAPI *initialize_var_by_name) ( PMODULE_VAR module_var, PSTRINGZ name );  
    void        (FSAPI *lookup_var) ( PMODULE_VAR module_var ); 
    void        (FSAPI *unregister_var_by_name) ( PSTRINGZ name );  
    void        (FSAPI *unregister_all_named_vars) (void);  
    void        (FSAPI *reserved18) (void); 
    void        (FSAPI *reserved19) (void); 
    BOOL        (FSAPI *panel_window_close_ident) (UINT32 panel_id);    
    BOOL        (FSAPI *panel_window_open_ident) (UINT32 panel_id); 
    void        (FSAPI *panel_window_toggle_hud_color) (void);  
    void        (FSAPI *panel_window_toggle_hud_units) (void);  
    void        (FSAPI *radio_stack_popup) (void);  
    void        (FSAPI *radio_stack_autoclose) (void);  
    ID          (FSAPI *check_named_variable) (PCSTRINGZ name);
    ID          (FSAPI *register_named_variable) (PCSTRINGZ name);
    FLOAT64     (FSAPI *get_named_variable_value) (ID id);
    FLOAT64     (FSAPI *get_named_variable_typed_value) (ID id, ENUM units);
    void        (FSAPI *set_named_variable_value) (ID id, FLOAT64 value);
    void        (FSAPI *set_named_variable_typed_value) (ID id, FLOAT64 value, ENUM units);
    void        (FSAPI *reserved26) (void); 
    void        (FSAPI *reserved27) (void); 
    PCSTRINGZ   (FSAPI *get_name_of_named_variable) (ID id);
    void        (FSAPI *reserved29) (void); 
    PCSTRINGZ   (FSAPI *panel_resource_string_get) (ID32 id);   
    BOOL        (FSAPI *panel_window_toggle_menu_id) (ID32 menu_id);    
    void        (FSAPI *reserved30) (void); 
    void        (FSAPI *reserved31) (void); 
    void        (FSAPI *element_use_color) (PELEMENT_HEADER element, BOOL override, UINT32 color);  // color actually is  COLORREF
    void        (FSAPI *set_gauge_flags) (PCSTRINGZ name, FLAGS32 newflags);        // newlags is GAUGE_FLAG_NORMAL, GAUGE_FLAG_HIDDEN... 
    FLAGS32     (FSAPI *get_gauge_flags) (PCSTRINGZ name);
    BOOL        (FSAPI *gauge_calculator_code_precompile) (PCSTRINGZ* pCompiled, UINT32* pCompiledSize, PCSTRINGZ source);
    BOOL        (FSAPI *execute_calculator_code) (PCSTRINGZ code, FLOAT64* fvalue, SINT32* ivalue, PCSTRINGZ* svalue);
    BOOL        (FSAPI *format_calculator_string) (PSTRINGZ result, UINT32 resultsize, PCSTRINGZ format);
    BOOL        (FSAPI *reserved32) (void);
    BOOL        (FSAPI *reserved33) (void);
    ENUM        (FSAPI *get_units_enum) (PCSTRINGZ unitname);
    ENUM        (FSAPI *get_aircraft_var_enum) (PCSTRINGZ simvar);
    FLOAT64     (FSAPI *aircraft_varget) (ENUM simvar, ENUM units, SINT32 index);
    BOOL        (FSAPI *panel_register_c_callback) (PCSTRINGZ name, IPanelCCallback* pcallback);
    IPanelCCallback* (FSAPI *panel_get_registered_c_callback) (PCSTRINGZ name);
    IAircraftCCallback* (FSAPI *panel_get_aircraft_c_callback) (PCSTRINGZ name);
    void        (FSAPI *send_key_event) (ID32 event_id, UINT32 value);
    void        (FSAPI *register_key_event_handler) (GAUGE_KEY_EVENT_HANDLER handler, PVOID userdata);
    void        (FSAPI *unregister_key_event_handler) (GAUGE_KEY_EVENT_HANDLER handler, PVOID userdata);
    void        (FSAPI *reserved34) (void);
    void        (FSAPI *reserved35) (void);
    bool        (FSAPI *process_shared_event_out)(PGAUGEHDR gauge_header, BYTE* pBuf, UINT32 nSize);
    bool        (FSAPI *is_master)();
} PANELS, *PPANELS, **PPPANELS;

#define element_list_query  (ImportTable.PANELSentry.fnptr->element_list_query)
#define element_list_install    (ImportTable.PANELSentry.fnptr->element_list_install)
#define element_list_initialize (ImportTable.PANELSentry.fnptr->element_list_initialize)
#define element_list_update (ImportTable.PANELSentry.fnptr->element_list_update)
#define element_list_generate   (ImportTable.PANELSentry.fnptr->element_list_generate)
#define element_list_plot   (ImportTable.PANELSentry.fnptr->element_list_plot)
#define element_list_erase  (ImportTable.PANELSentry.fnptr->element_list_erase)
#define element_list_kill   (ImportTable.PANELSentry.fnptr->element_list_kill)
#define mouse_list_install  (ImportTable.PANELSentry.fnptr->mouse_list_install)
#define mouse_list_register (ImportTable.PANELSentry.fnptr->mouse_list_register)
#define mouse_list_unregister   (ImportTable.PANELSentry.fnptr->mouse_list_unregister)
#define panel_window_toggle (ImportTable.PANELSentry.fnptr->panel_window_toggle)
#define trigger_key_event   (ImportTable.PANELSentry.fnptr->trigger_key_event)
#define register_var_by_name    (ImportTable.PANELSentry.fnptr->register_var_by_name)
#define initialize_var  (ImportTable.PANELSentry.fnptr->initialize_var)
#define initialize_var_by_name  (ImportTable.PANELSentry.fnptr->initialize_var_by_name)
#define lookup_var  (ImportTable.PANELSentry.fnptr->lookup_var)
#define unregister_var_by_name  (ImportTable.PANELSentry.fnptr->unregister_var_by_name)
#define unregister_all_named_vars   (ImportTable.PANELSentry.fnptr->unregister_all_named_vars)
#define panel_window_close_ident    (ImportTable.PANELSentry.fnptr->panel_window_close_ident)
#define panel_window_open_ident (ImportTable.PANELSentry.fnptr->panel_window_open_ident)
#define is_panel_window_visible_ident   (ImportTable.PANELSentry.fnptr->is_panel_window_visible_ident)
#define panel_window_toggle_hud_color   (ImportTable.PANELSentry.fnptr->panel_window_toggle_hud_color)
#define panel_window_toggle_hud_units   (ImportTable.PANELSentry.fnptr->panel_window_toggle_hud_units)
#define radio_stack_popup   (ImportTable.PANELSentry.fnptr->radio_stack_popup)
#define radio_stack_autoclose   (ImportTable.PANELSentry.fnptr->radio_stack_autoclose)
#define panel_resource_string_get   (ImportTable.PANELSentry.fnptr->panel_resource_string_get)
#define panel_window_toggle_menu_id (ImportTable.PANELSentry.fnptr->panel_window_toggle_menu_id)
#define element_use_color   (ImportTable.PANELSentry.fnptr->element_use_color)
#define check_named_variable    (ImportTable.PANELSentry.fnptr->check_named_variable)
#define register_named_variable (ImportTable.PANELSentry.fnptr->register_named_variable)
#define get_named_variable_value    (ImportTable.PANELSentry.fnptr->get_named_variable_value)
#define get_named_variable_typed_value  (ImportTable.PANELSentry.fnptr->get_named_variable_typed_value)
#define set_named_variable_value    (ImportTable.PANELSentry.fnptr->set_named_variable_value)
#define set_named_variable_typed_value  (ImportTable.PANELSentry.fnptr->set_named_variable_typed_value)
#define get_name_of_named_variable  (ImportTable.PANELSentry.fnptr->get_name_of_named_variable)
#define set_gauge_flags (ImportTable.PANELSentry.fnptr->set_gauge_flags)
#define get_gauge_flags (ImportTable.PANELSentry.fnptr->get_gauge_flags)
#define gauge_calculator_code_precompile    (ImportTable.PANELSentry.fnptr->gauge_calculator_code_precompile)
#define execute_calculator_code (ImportTable.PANELSentry.fnptr->execute_calculator_code)
#define format_calculator_string    (ImportTable.PANELSentry.fnptr->format_calculator_string)
#define get_units_enum  (ImportTable.PANELSentry.fnptr->get_units_enum)
#define get_aircraft_var_enum   (ImportTable.PANELSentry.fnptr->get_aircraft_var_enum)
#define aircraft_varget (ImportTable.PANELSentry.fnptr->aircraft_varget)
#define panel_register_c_callback   (ImportTable.PANELSentry.fnptr->panel_register_c_callback)
#define panel_get_registered_c_callback (ImportTable.PANELSentry.fnptr->panel_get_registered_c_callback)
#define panel_get_aircraft_c_callback   (ImportTable.PANELSentry.fnptr->panel_get_aircraft_c_callback)
#define send_key_event  (ImportTable.PANELSentry.fnptr->send_key_event)
#define register_key_event_handler  (ImportTable.PANELSentry.fnptr->register_key_event_handler)
#define unregister_key_event_handler    (ImportTable.PANELSentry.fnptr->unregister_key_event_handler)
#define process_shared_event_out    ( ImportTable.PANELSentry.fnptr->process_shared_event_out)
#define is_master ( ImportTable.PANELSentry.fnptr->is_master)
#define FSVersion               (ImportTable.PANELSentry.fnptr->ModuleVersion)


// This is the gauge's import table definition
typedef struct GAUGESIMPORT
{
    struct
    {
        ID          fnID;       
        PPANELS     fnptr;
    }PANELSentry;

    struct
    {
        ID          fnID;
        PVOID       fnptr;
    }nullentry;
} GAUGESIMPORT, *PGAUGESIMPORT;

EXTERN_C DECLSPEC_EXPORT GAUGESIMPORT ImportTable;

#if defined(_MSC_VER) 
#pragma warning( push )
#pragma warning( disable:4200 )
#endif

// This is the module's export table definition
typedef struct  GAUGESLINKAGE
{
    FSLINKAGE
    PGAUGEHDR       gauge_header_ptr[];
} GAUGESLINKAGE, *PGAUGESLINKAGE, **PPGAUGESLINKAGE;

#if defined(_MSC_VER) 
#pragma warning( pop )
#endif

EXTERN_C DECLSPEC_EXPORT    GAUGESLINKAGE   Linkage;



// Old gauge functions
static void FSAPI query_routine_610( void )
{
    element_list_query( *Linkage.gauge_header_ptr[0]->elements_list );
}

static void FSAPI install_routine_610( PVOID resource_file_handle )
{
    element_list_install( *Linkage.gauge_header_ptr[0]->elements_list, resource_file_handle );  
}

static void FSAPI initialize_routine_610( void )
{
    element_list_initialize( *Linkage.gauge_header_ptr[0]->elements_list );
}

static void FSAPI update_routine_610( void )
{
    element_list_update(*Linkage.gauge_header_ptr[0]->elements_list);
}

static void FSAPI generate_routine_610( GENERATE_PHASE phase )
{
    element_list_generate( *Linkage.gauge_header_ptr[0]->elements_list, phase );
}

static void FSAPI draw_routine_610( void )
{
    element_list_plot( *Linkage.gauge_header_ptr[0]->elements_list );
}

static void FSAPI kill_routine_610( void )
{
    element_list_kill( *Linkage.gauge_header_ptr[0]->elements_list );
}


//-------------------------------------------------------------------------------
// GAUGE_TABLE_BEGIN
// GAUGE_TABLE_ENTRY
// GAUGE_TABLE_END
//
//  These macros build up the export table for the module by inserting gauge names
//  into the list of gauges
//-------------------------------------------------------------------------------
#define GAUGE_TABLE_BEGIN()                                     \
        extern  GAUGEHDR    gauge_header;                       \
        void FSAPI  module_init(void){}                         \
        void FSAPI  module_deinit(void){}                       \
        BOOL WINAPI DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved) \
        {                                                       \
            return TRUE;                                        \
        }                                                       \
                                                                \
                                                                \
        /* This is the module's import table.   */              \
        GAUGESIMPORT    ImportTable =                           \
        {                                                       \
            { 0x0000000F, (PPANELS)NULL },                      \
            { 0x00000000, NULL }                                \
        };                                                      \
                                                                \
                                                                \
        /* This is the module's export table.   */              \
        GAUGESLINKAGE   Linkage =                               \
        {                                                       \
            0x00000013,                                         \
            module_init,                                        \
            module_deinit,                                      \
            0,                                                  \
            0,                                                  \
            FS9LINK_VERSION, {

#define GAUGE_TABLE_ENTRY(pgauge_header)        (pgauge_header),

#define GAUGE_TABLE_END()                       0 }};

#define GAUGE_HEADER_FS1000(            \
                gaugehdr_var_name,      \
                default_size_mm,        \
                gauge_name,             \
                element_list,           \
                pmouse_rect,            \
                pgauge_callback,        \
                user_data,              \
                usage,                  \
                guid,                   \
                serialize_size_callback,\
                serialize_callback,     \
    			deserialize_callback,   \
    			event_size_callback,    \
    			process_event_callback) \
    GAUGEHDR gaugehdr_var_name =        \
    {                                   \
        GAUGE_HEADER_VERSION_FS1000,    \
        gauge_name,                     \
        element_list,                   \
        query_routine_610,              \
        install_routine_610,            \
        initialize_routine_610,         \
        update_routine_610,             \
        generate_routine_610,           \
        draw_routine_610,               \
        kill_routine_610,               \
        NULL,                           \
        default_size_mm,                \
        0,                              \
        0.0,                            \
        0.0,                            \
        NULL,                           \
        NULL,                           \
        {0, 0},                         \
        NULL,                           \
        pmouse_rect,                    \
        pgauge_callback,                \
        user_data,                      \
        NULL,                           \
        usage,                          \
        0,                              \
        NULL,                           \
        {0, 0},                         \
        {0.0},                          \
        0,                              \
        NULL,                           \
        guid,                           \
        0,                              \
        serialize_size_callback,        \
        serialize_callback,             \
	    deserialize_callback,           \
	    event_size_callback,            \
	    process_event_callback,         \
    };
    
#define GAUGE_HEADER_FS900(             \
                gaugehdr_var_name,      \
                default_size_mm,        \
                gauge_name,             \
                element_list,           \
                pmouse_rect,            \
                pgauge_callback,        \
                user_data,              \
                usage)                  \
    GAUGEHDR gaugehdr_var_name =        \
    {                                   \
        GAUGE_HEADER_VERSION_FS900,      \
        gauge_name,                     \
        element_list,                   \
        query_routine_610,              \
        install_routine_610,            \
        initialize_routine_610,         \
        update_routine_610,             \
        generate_routine_610,           \
        draw_routine_610,               \
        kill_routine_610,               \
        NULL,                           \
        default_size_mm,                \
        0,                              \
        0.0,                            \
        0.0,                            \
        NULL,                           \
        NULL,                           \
        {0, 0},                         \
        NULL,                           \
        pmouse_rect,                    \
        pgauge_callback,                \
        user_data,                      \
        NULL,                           \
        usage,                          \
        0,                              \
        NULL,                           \
        {0, 0},                         \
        {0.0},                          \
        0,                              \
        NULL,                           \
    };


#define GAUGE_HEADER_FS800(             \
                gaugehdr_var_name,      \
                default_size_mm,        \
                gauge_name,             \
                element_list,           \
                pmouse_rect,            \
                pgauge_callback,        \
                user_data,              \
                usage)                  \
    GAUGEHDR gaugehdr_var_name =        \
    {                                   \
        GAUGE_HEADER_VERSION_FS800,     \
        gauge_name,                     \
        element_list,                   \
        query_routine_610,              \
        install_routine_610,            \
        initialize_routine_610,         \
        update_routine_610,             \
        generate_routine_610,           \
        draw_routine_610,               \
        kill_routine_610,               \
        NULL,                           \
        default_size_mm,                \
        0,                              \
        0.0,                            \
        0.0,                            \
        NULL,                           \
        NULL,                           \
        {0, 0},                         \
        NULL,                           \
        pmouse_rect,                    \
        pgauge_callback,                \
        user_data,                      \
        NULL,                           \
        usage,                          \
        0,                              \
        NULL,                           \
        {0, 0},                         \
        {0.0},                          \
    };

#define GAUGE_HEADER_FS700(             \
                default_size_mm,        \
                gauge_name,             \
                element_list,           \
                pmouse_rect,            \
                pgauge_callback,        \
                user_data,              \
                parameters,             \
                usage)                  \
        GAUGE_HEADER_FS800(             \
                GAUGEHDR_VAR_NAME,      \
                default_size_mm,        \
                gauge_name,             \
                element_list,           \
                pmouse_rect,            \
                pgauge_callback,        \
                user_data,              \
                usage)

   

#pragma pack()

#endif
