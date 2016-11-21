/*
+------------------------------------------------------------------------------
| Description: io.c
| This file handles all input and output functionality
|
+------------------------------------------------------------------------------
| Copyright: This software is the property of
|
|                   Edwards Systems Technology
|                   6411 Parkland Drive
|                   Sarasota, Fl 34243
|                   (941) 739-4200
|
|                   Copyright (c) 2001 Edwards Systems Technology
|
|                   The use, duplication, or disclosure of this material in any
|                   form without permission from Edwards Systems Technology is
|                   strictly forbidden.
+------------------------------------------------------------------------------
|  Provide the date, name of person performing the maintenance, and
|  a short description of the change.  Identify the area of the file
|  that has been modified
|
|  Date      Name             Description of change
| ========  ===============  ==================================================
| 03/03/2016 R. Hempel       [QIT-46403798] - Added debouncing for the
|                            overcurrent detection state machine - we need
|                            to see at least 2 good overcurrent restorals
|                            before restarting the state machine to avoid
|                            the problem of extending the overcurrent fault
|                            detection past 600 msec.
| 02/17/2016 R. Hempel       [QIT-46403798] - Added acFailing so that we can
|                            ignore the apparent NAC overcurrent indication.
|                            When AC fails, the NAC supervision circuit presents
|                            the same data as a true NAC overcurrent fault - we
|                            use acFailing to ignore the data during the AC fail
|                            debounce time.
| 10/27/2010 J. Castellanos  Changed the A/D values for the brownout because the 
|                            panel (FSP 3 zones) was browning out above 103 
|                            Vac with no load, and with a 4 Amp load and NACs 
|                            active it was coming in and out of brownout at 
|                            99.0 Vac (when we detect brownout and change to 
|                            batteries the load goes to the battery and the 
|                            voltage from the transformer increases, we have 
|                            to use a restored value that is greater than that 
|                            voltage increase). Also, we have to consider the 
|                            case of the Battery Charger, when this is on 
|                            (for 5 seconds) the voltage drops because the 
|                            charger uses current to charge the battery. 
| 2/26/2007  C. McGill       In Process_Specials(), modified to refresh peripherals
|                            only if constant power has been removed/restored. Eliminates
|                            "conflict" with walk test wherein resettable power is
|                            removed/restored, then peripherals are updated prematurely.
|                            Removed edit marks.
| 11/27/2006 C. McGill       In debounce_nac_levels(), corrected to debounce NAC
|                            overcurrent for 3/5 zone panel. NACs incorrectly
|                            going into overcurrent when active and system
|                            transferred to battery (ACFAIL). Updated A/D tables.
| 11/1/2006  C. McGill       Updated A/D tables based on UL compatibility testing.
| 5/11/2006  C. McGill       Modified BROWNOUT_DEBOUNCE from 15 to 20. Corrects
|                            PTR 1508, where 5 zone panel would transfer to battery
|                            when -2400V transient applied. In button_handler(),
|                            corrected to reset any_button_ctr in the absence of a
|                            button press. Corrects PTR 1508, where 3 zone panel would
|                            disable IDC zone when +2400V transient was applied repeatedly.
| 11/28/05  C. McGill        In debounce_nac_levels(), modified NAC Class A control
|                            to match hardware modifications.
| 11/17/05  C. McGill        In debounce_idc_levels(), modified to add A/D values
|                            when configured Class A. Corrects situation where + side
|                            of the odd IDC was open and detector was activated, but
|                            even IDC did not activate as the current flow was split
|                            between IDCs. Updated batteryLevels tables for
|                            even IDCs. Were tripping at half the level of odd IDCs.
|                            In ProcessSpecials(), added debounce of groundfault restoration.
|                            Prevents repeated activation/restoration of ground faults.
|                            PTR 1494.
| 11/14/05  C. McGill        In BATTERY_SUPERVISE_CHECK case in ProcessSpecials(),
|                            modified not to declare charger error if battery is fully
|                            charged. Special case for charging in low temp (0C) condition
|                            where charge is extremely slow to fall off making it difficult
|                            to see voltage differential for charger test and leading to false
|                            charger faults. Updated batteryLevels tables for proper 2-wire
|                            smoke operation (PTR 1474). Updated GF values in batteryLevels[]
|                            to detect GF on neg leg of IDC/NAC and to detect when on battery.
|                            PTR 1492 - 1493.
| 10/24/05  C. McGill        In ProcessSpecials(), updated AC brownout/restore values.
| 10/18/05  C. McGill        Adjusted Aux OC values in batteryLevels[] to prevent
|                            false OC during battery charge. PTR 1471. In ProcessSpecials(),
|                            modified to debounce AC brownout detection, PTR 1470. In
|                            ProcessSpecials() modified to debounce battery charger fail/restore,
|                            PTR 1468.
| 8/15/2005  C. McGill       Modified to use new define MAX_LATCH_HW in output_refresh().
|                            Corrected setting of 10zone class A NACs.
| 8/11/2005  C. McGill       Removed test code.
| 8/10/2005  C. McGill       Added new variable to hold average IDC AD during charging
|                            phase. Modified clean me functions to check charging average
|                            when panel in charge mode. Added softare latch 9 for 10 zone
|                            panel.
| 8/9/2005   C. McGill       Corrected size of CleanMe variables. Modified AC Power
|                            LED to follow ac trouble.
| 04/17/03  C. McGill        In debounce_idc_levels() modified to set new variable
|                             debounceCalled to signal when function has been
|                             called. In output_refresh() modified to put panel
|                             in trouble state while debounceCalled is not set
|                             and panel not in program state. This provides
|                             startup trouble state for panel until inputs
|                             operational. PTR #1142. Reversed History w/most
|                             recent change first. Removed old edit marks.
| 09/03/02  C. McGill        In output_refresh(), modified button press and
|                             enter entry buzzer handling to prevent buzzer
|                             lockup if buzzer sounding when program mode exited.
|                             PTR #891.
| 08/19/02  J. Monczynski    In ProcessSpecials(), turned charger off before
|                             checking if battery needs to be charged.
| 08/16/02  J. Monczynski    Added new function DetectChargerPeaks() for generic
|                             charger supervsion under all conditions.
| 07/26/02  C. McGill        In debounce_idc_levels(), corrected to reset state
|                             to start condition when system is reset.
| 07/14/02  C. McGill        In output_refresh(), corrected buzzer pattern limit
|                             check. PTR #675, #678, #693, #699.
| 06/30/02  C. McGill        In ProcessSpecials(), modified to continue to (try to)
|                             charge through a battery placement fault. Batteries
|                             may be present, but below 19.4V. PTR #666.
| 06/12/02  C. McGill        In output_refresh(), added 3-3-3 buzzer pattern for
|                             alarm state.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   io.c  $
|     $Date:   Nov 02 2010 10:39:28  $
|  $Modtime:   Nov 02 2010 10:37:18  $
| $Revision:   1.187  $
|  $Archive:   \\BRAFLAPP02SECGE\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\io.c-arc  $
|   $Author:   Julio Castellanos  $
+------------------------------------------------------------------------------
*/

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include "config.h"
#include <pic18.h>
#include "defines.h"
#include "main.h"
#include "externs.h"
#include "io.h"

#define BATT_TEST                       0
#define CHAR_COMPARE                    0
#define BATT_CHARGE                     0

/* Turn one of these on at a time to enable debugging with the spare ouput at:
 *
 *     latch6.latch6_output_bit.unused
 */
#define AC_FAIL_TEST_PIN                0
#define NAC_SHORT_TEST_PIN              0

#define AUX_CHECK_STATE_START           0
#define AUX_CHECK_STATE_AUX_CONSTANT    1
#define AUX_CHECK_STATE_AUX_RESETTABLE  2

#define OC_RESTORE_DEBOUNCE             2
#define BROWNOUT_DEBOUNCE               20
#define STABLE_FAIL_DEBOUNCE            1
#define NOISE_DIFFERENTIAL              1

#define GF_COUNT                        200

extern void DogWatch(void);
//======= Local (to module) prototypes =================================
static void button_pressed(void);
//======================================================================

//============ Local (to module) variables ===================
static unsigned char any_ad4_button_ctr = 0;
static unsigned char any_button_ctr = 0;
static unsigned char lamp_test_ctr = 0;
static unsigned char remote_disconnect_ctr = 0;
static unsigned char walk_test_ctr = 0;
static unsigned char programming_ctr = 0;
static unsigned char button_active_flag = 0;
static near unsigned char idc_ctr;
static near unsigned char e2_data;
static near unsigned char nac_ctr;
//============================================================

//=== Global Variable Declarations (extern statement in file: "externs.h") ========
near union BUTTON_BIT_BYTE button;
#if (PANEL_10_ZONE)
union BUTTON_BIT_BYTE2 button2;
#endif
volatile unsigned char button_valid_timer;    //volatile because it is dec'd in interrupt
unsigned int an4_buttons_ad_int_value;
unsigned int batt_ad_int_value;
unsigned int aux_power_ad_int_value;
unsigned int ground_ad_int_value;
unsigned int ac_ad_int_value;
union IDC_LEVEL_BIT_BYTE idc_level[MAX_IDC]; //current voltage level (between thresholds) of each idc zone
union NAC_LEVEL_BIT_BYTE nac_level[MAX_NAC]; //current voltage level (between thesholds) of each NAC
unsigned int idc_ad_int_value[MAX_IDC];
unsigned int nac_ad_int_value[MAX_NAC];
#if (PANEL_10_ZONE)
unsigned int nacOverCurrent[MAX_NAC];
unsigned int transformer2 = 0;
#endif

unsigned char idc_x_debounce_ctr[MAX_IDC];
unsigned char nac_x_debounce_ctr[MAX_NAC];
unsigned char nac_x_oc_restore_ctr[MAX_NAC];

unsigned char cleanMePulseDetect[MAX_IDC];
unsigned char cleanMePulseValid[MAX_IDC];
unsigned int  cleanMeAverageNormal[MAX_IDC];
unsigned int  cleanMeAverageCharging[MAX_IDC];
unsigned char cleanMeInitiationTimer[MAX_IDC];
unsigned char cleanMeValidationTimer[MAX_IDC];
unsigned char cleanMeRestorationTimer[MAX_IDC];

unsigned char auxTimerOC = NOT_RUNNING;
unsigned char powerLossDebounce = NOT_RUNNING;
unsigned char powerRestoreDebounce = NOT_RUNNING;

near unsigned int idc_thresh[4];        //trip thresholds change for battery lowering during A.C. fail, etc.
near unsigned int idc_thresh_odd[4];    //trip thresholds change for battery lowering during A.C. fail, etc.
near unsigned int nac_thresh[6];        //trip thresholds change for battery lowering during A.C. fail, etc.
unsigned int ground_fault[2];
static unsigned int auxPwrOC;

/******************************************************************************/
/* Global Variable Declarations (extern statement in file: "externs.h")       */
/******************************************************************************/
volatile unsigned char pulse_120spm_timer; //LED flash rate timer (volatile because it is also used in interrupt)
volatile unsigned char pulse_trouble_timer; //pulse trouble LED timer (volatile because it is dec'd in interrupt)
volatile unsigned char pulse_2x20_timer;    //2x20 spm timer (volatile because it is dec'd in interrupt)

bit buzzer_on_flag;

bit buzzer_blip_flag;
//unsigned char buzzer_blip_flag; //fix compiler bug

bit buzzer_enter_flag;
bit trouble_LED_flag;
bit pulse_2x20_flag;
bit RRM_pulse_2x20_flag;
bit alarm_pulse_2x20_flag;
bit buzzer_lockout_flag;

static unsigned char batteryKill = 0;
static unsigned char acFailing = 0;

union LATCH1_BIT_BYTE latch1;
union LATCH2_BIT_BYTE latch2;
union LATCH3_BIT_BYTE latch3;
union LATCH4_BIT_BYTE latch4;
union LATCH5_BIT_BYTE latch5;
union LATCH6_BIT_BYTE latch6;
#if (PANEL_10_ZONE)
union LATCH7_BIT_BYTE latch7;
union LATCH8_BIT_BYTE latch8;
union LATCH9_BIT_BYTE latch9;
#endif

union LATCH1_PULSE_BIT_BYTE latch1_pulse;
union LATCH2_PULSE_BIT_BYTE latch2_pulse;
union LATCH4_PULSE_BIT_BYTE latch4_pulse;
union LATCH5_PULSE_BIT_BYTE latch5_pulse;
union LATCH6_PULSE_BIT_BYTE latch6_pulse;
#if (PANEL_10_ZONE)
union LATCH7_PULSE_BIT_BYTE latch7_pulse;
union LATCH8_PULSE_BIT_BYTE latch8_pulse;
union LATCH9_PULSE_BIT_BYTE latch9_pulse;
#endif

volatile unsigned char battery_check_timer;
unsigned char voltageIndex;
unsigned char debounceCalled = 0;

/**************************************************************************
 ** FUNCTION: load_thresh_levels                                         **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the threshold levels according to the battery  **
 ** voltage. This occurrs only if no AC power. If AC power, 23.4v values **
 ** are used.                                                            **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  updated threshold values.                                  **
 **                                                                      **
 **************************************************************************/
//=================================================================================
#if 0
const unsigned int batteryLevels[BATTERY_MAX_VOLTAGES][BATTERY_MAX_ENTRIES] =
{
 /* Battery  |---------Odd-IDC----------|---------------NAC---------------|ground fault| Aux */
 /*  A/D,    open, WF/SUP, v alm, nv alm, B-open, B-short, A-open, A-short, GF-L,GF-H,   OC, */
     458,      77,    202,   288,    576,     211,    502,     117,    326,  253, 716,  400,  /* 19.4 v */
     482,      80,    212,   304,    582,     222,    528,     123,    343,  265, 757,  432,  /* 20.4 v */
     506,      83,    223,   320,    611,     233,    554,     129,    360,  277, 798,  456,  /* 21.4 v */
     529,      85,    221,   316,    564,     232,    519,     128,    328,  283, 839,  479,  /* 22.4 v */
     553,      88,    231,   330,    549,     243,    542,     134,    342,  302, 880,  503,  /* 23.4 v */
     577,      91,    241,   344,    615,     253,    566,     140,    357,  314, 900,  526,  /* 24.4 v */
     600,      94,    251,   358,    640,     264,    589,     145,    371,  326, 920,  550,  /* 25.4 v */
     624,      97,    261,   372,    665,     274,    612,     151,    386,  337, 942,  574,  /* 26.4 v */
     648,     100,    271,   386,    690,     284,    635,     157,    401,  350, 978,  596   /* 27.4 v */
};

const unsigned int batteryLevels2[BATTERY_MAX_VOLTAGES][BATTERY2_MAX_ENTRIES] =
{
 /*|---------Even-IDC---------|--------NAC-------*/
 /*open, WF/SUP, v alm, nv alm, P-open, P-short, */
    77,    202,   288,    576,     211,    502,  /* 19.4 v */
    80,    212,   304,    582,     222,    528,  /* 20.4 v */
    83,    223,   320,    611,     233,    554,  /* 21.4 v */
    84,    217,   310,    554,     232,    519,  /* 22.4 v */
    86,    227,   324,    579,     145,    423,  /* 23.4 v */
    89,    237,   337,    604,     152,    441,  /* 24.4 v */
    92,    246,   351,    628,     158,    459,  /* 25.4 v */
    95,    256,   365,    653,     164,    477,  /* 26.4 v */
    98,    266,   379,    677,     170,    495   /* 27.4 v */
};
#endif
const unsigned int batteryLevels[BATTERY_MAX_VOLTAGES][BATTERY_MAX_ENTRIES] =
{
 /* Battery  |---------Odd-IDC----------|---------------NAC---------------|ground fault| Aux */
 /*  A/D,    open, WF/SUP, v alm, nv alm, B-open, B-short, A-open, A-short, GF-L,GF-H,   OC, */
     458,      69,    187,   268,    493,     202,    450,     110,    283,  244, 547,  400,  /* 19.4 v */
     482,      73,    197,   282,    519,     212,    473,     116,    298,  257, 575,  432,  /* 20.4 v */
     506,      76,    207,   296,    544,     222,    496,     122,    313,  270, 604,  456,  /* 21.4 v */
     529,      80,    217,   310,    569,     232,    519,     128,    328,  283, 632,  479,  /* 22.4 v */
     553,      84,    227,   324,    595,     243,    542,     134,    342,  302, 660,  503,  /* 23.4 v */
     577,      87,    237,   337,    620,     253,    566,     140,    357,  314, 688,  526,  /* 24.4 v */
     600,      90,    246,   351,    645,     264,    589,     145,    371,  326, 716,  550,  /* 25.4 v */
     624,      94,    256,   365,    671,     274,    612,     151,    386,  337, 745,  574,  /* 26.4 v */
     648,      97,    266,   379,    696,     284,    635,     157,    401,  350, 773,  596   /* 27.4 v */
};

const unsigned int batteryLevels2[BATTERY_MAX_VOLTAGES][BATTERY2_MAX_ENTRIES] =
{
 /*|---------Even-IDC---------|--------NAC-------*/
 /*open, WF/SUP, v alm, nv alm, P-open, P-short, */
     66,    147,   210,    406,     121,    350,  /* 19.4 v */
     69,    154,   221,    427,     127,    368,  /* 20.4 v */
     72,    162,   231,    447,     133,    389,  /* 21.4 v */
     75,    169,   242,    468,     139,    404,  /* 22.4 v */
     77,    176,   252,    489,     145,    423,  /* 23.4 v */
     80,    183,   262,    509,     152,    441,  /* 24.4 v */
     83,    190,   273,    530,     158,    459,  /* 25.4 v */
     85,    197,   283,    551,     164,    477,  /* 26.4 v */
     88,    205,   294,    572,     170,    495   /* 27.4 v */
};
#if (PANEL_3_5_ZONE)
unsigned char batteryLevel = V23_4;
#endif
//=================================================================================
void load_thresh_levels(void)
{
    unsigned char indexVoltage, done;

    /* if AC power, use 23.4v values */
    voltageIndex = V23_4;
#if (PANEL_3_5_ZONE)
//     batteryLevel = V26_4;
     batteryLevel = V27_4;
#endif

    /* if charger on, use battery level. */
    if ( latch2.latch2_output_bit.charger )
    {
        done = 0;

//        voltageIndex = V26_4;
        voltageIndex = V27_4;

        /* search through table for current voltage values */
        while ( !done )
        {
            /* if voltage in range OR end of table */
            if ( batt_ad_int_value >= batteryLevels[voltageIndex][BATTERY_VOLTAGE] || voltageIndex == V19_4 )
            {
                /* table match */
                done = 1;
            }
            else
            {
                voltageIndex--;
            }
        }

#if (PANEL_3_5_ZONE)
        batteryLevel = voltageIndex;
#endif
    }

    /* load table values into threshold tables */
    for (indexVoltage = 0; indexVoltage < 4; indexVoltage++ )
    {
        /* set up IDC voltage thresholds */
        idc_thresh[indexVoltage] = batteryLevels2[voltageIndex][indexVoltage+BATTERY2_IDC_OPEN_ODD];
        idc_thresh_odd[indexVoltage] = batteryLevels[voltageIndex][indexVoltage+BATTERY_IDC_OPEN];

        /* set up NAC voltage thresholds */
        nac_thresh[indexVoltage] = batteryLevels[voltageIndex][indexVoltage+BATTERY_NAC_B_OPEN];

        /* set up ground fault voltage thresholds */
        if ( indexVoltage < 2 )
        {
            ground_fault[indexVoltage] = batteryLevels[voltageIndex][indexVoltage+BATTERY_GF_LOW];
        }
    }
    auxPwrOC = batteryLevels[voltageIndex][BATTERY_AUX_OC];

    // load new parallel settings
    nac_thresh[4] = batteryLevels2[voltageIndex][BATTERY2_NAC_P_OPEN];
    nac_thresh[5] = batteryLevels2[voltageIndex][BATTERY2_NAC_P_OPEN];
}

/*
+------------------------------------------------------------------------------
| Purpose:
|	Perform A/D conversion. PIC processor provides a 10 bit A/D conversion in
|	registers ADRESH and ADRESL. These registers will be manipulated into an
|	integer value and returned.
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
| 	-10 bit A/D result is manipulated into an integer variable "ad_int" and returned.
|		This integer is returned in little endian format (low byte,high byte).
|	-When selecting a new channel, for conversion, PIC requires an aquisition delay.
|
|	CALLS: None
|
+------------------------------------------------------------------------------
| Parameters:
|	ad_channel:
|		PIC channel for conversion. Range 0 - 4.
|
+------------------------------------------------------------------------------
| Return Value:
|	unsigned int:
|		little endian integer value of requested channel A/D conversion.
|
+------------------------------------------------------------------------------
*/
unsigned int ad_conversion(unsigned char ad_channel)
{
    unsigned int ad_int;
    unsigned int temp_int;
    static unsigned char last_ad_channel_selected = 0xff;

#if (PROC_18F252)
    ADCON0  = 0b01000000;           //A/D cov clock = Fosc/8
    ADCON0 |= (ad_channel << 3);    //select A/D channel
    ADCON0 |= 0b00000001;           //A/D on
    ADCON1  = 0b10000000;           //A/D result right justified, all analog inputs for PORTA
#else
    ADCON1  = 0b00000111;           //All analog inputs for PORTA, Vss, Vdd
    ADCON0  = (ad_channel << 2);    //select A/D channel
    ADCON2  = 0b10111001;           //A/D cov clock = Fosc/8, Tad 20, A/D result right justified
    ADCON0 |= 0b00000001;           //A/D on
#endif

    temp_int = 0;

    //if new A/D channel selected, must delay acquisition time
    if (last_ad_channel_selected != ad_channel)
    {
        for (temp_int = 0; temp_int < 15; temp_int++)
        {
            //dummy write to waste time
            ad_int = 1;
        }
    }
    last_ad_channel_selected = ad_channel;

    //clear flag, PIC sets it when conversion completed
    ADIF = 0;

    //start conversion
#if (PROC_18F252)
    ADCON0 |= 0b0000100;
#else
    ADCON0 |= 0b0000010;
#endif

    //wait here till conversion complete
    while (!ADIF);

    //put 10 bit A/D result value into an integer variable (little endian format)
    ad_int = ADRESL;
    temp_int = ADRESH;
    temp_int = temp_int << 8;
    ad_int += temp_int;

    return(ad_int);
}


/*
+------------------------------------------------------------------------------
| Purpose:
|	Requests A/D conversion of all voltages from IDC, NAC and button inputs and places
|	results into their repective variables.
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|	-Called from MAIN EXECUTIVE LOOP (main.c) regardless of what state (ALARM, PROGRAM,etc)
|	system is in.
|	-Places integer value, returned from "ad_conversion()" into the following:
|		an4_buttons_ad_int_value
|       idc_ad_int_value[]
|       nac_ad_int_value[]
|	-Interrupt is temporarily disabled when manipulating PORTB because interrupt also changes
|	PORTB.
|
|	CALLS: ad_conversion()
+------------------------------------------------------------------------------
| Parameters: None
|
+------------------------------------------------------------------------------
| Return Value: None
|
+------------------------------------------------------------------------------
*/
void scan_analog_inputs(void)
{
    unsigned char addrMux;   /* pin address of mux */
#if ( PANEL_10_ZONE )
    unsigned int muxValue;
    unsigned char numMux;    /* current mux being read */
#endif

    //voltage presented to PIC channel AN4 varies according to which button is pressed.
    an4_buttons_ad_int_value = ad_conversion(4);

#if (PANEL_3_5_ZONE)
	batt_ad_int_value = ad_conversion(1);
#endif

    //Disable interrupt while manipulating PORTB. Interrupt contains
    //a modification to PORTB for driving NAC GENESIS SYNCH signals.
    PEIE = 0;

#if ( PANEL_10_ZONE )
    /* loop through available mux's */
    for (numMux = MUX_1; numMux < MAX_MUX; numMux++)
    {
        //This loop increments the mux address and requests A/D conversion for these voltage inputs.
        for (addrMux = 0; addrMux < NUM_MUX_INPUTS; addrMux++)
        {
            //clear lower 3 bits (address inputs to mux)
            PORTB &= 0b11111000;
            PORTB |= addrMux;

            /* get the mux address value and store */
            muxValue = ad_conversion( numMux );

            /* get IDC/NAC information according to MUX */
            switch ( numMux )
            {
                /* get IDC 1-5 information */
                case MUX_1:

                    /* IDC 1-5 in address 0-4 */
                    if ( addrMux < MUX_ADDRESS_5 )
                    {
                        /* if not verifying an alarm */
                        if ( !latch4.latch4_output_bit.smoke_reset )
                        {
                            idc_ad_int_value[addrMux] = muxValue;
                        }
                    }
                    /* non-IDC information */
                    else
                    {
                        switch (addrMux)
                        {
                            case MUX_ADDRESS_5:
                                /* aux power */
                                aux_power_ad_int_value = muxValue;
                                break;

                            case MUX_ADDRESS_6:
                                /* ground fault */
                                ground_ad_int_value = muxValue;
                                break;

                            case MUX_ADDRESS_7:
                                /* AC power */
                                ac_ad_int_value = muxValue;
                                break;
                        }
                    }
                    break;

                /* get IDC 6-10 information */
                case MUX_2:

                    /* IDC 6-10 in address 0-4 */
                    if ( addrMux < MUX_ADDRESS_5 )
                    {
                        /* if not verifying an alarm */
                        if ( !latch4.latch4_output_bit.smoke_reset )
                        {
                            idc_ad_int_value[IDC_6+addrMux] = muxValue;
                        }
                    }
                    /* non-IDC information */
                    else
                    {
                        switch (addrMux)
                        {
                            case MUX_ADDRESS_5:
                                /* extra transformer */
                                transformer2 = muxValue;
                                break;

                            case MUX_ADDRESS_6:

                                /* NAC 3 OC */
                                nacOverCurrent[NAC_3] = muxValue;
                                break;

                            case MUX_ADDRESS_7:
                                /* NAC 4 OC */
                                nacOverCurrent[NAC_4] = muxValue;
                                break;
                        }
                    }
                    break;

                /* get NAC 1-4 information */
                case MUX_3:

                    switch (addrMux)
                    {
                        /* NAC 1+2 on address 0 and 1 */
                        case MUX_ADDRESS_0:
                        case MUX_ADDRESS_1:
                            nac_ad_int_value[addrMux] = muxValue;
                            break;

                        /* MUX_ADDRESS_2 not used */

                        /* NAC 3+4 on address 3 and 4 */
                        case MUX_ADDRESS_3:
                        case MUX_ADDRESS_4:
                            nac_ad_int_value[addrMux-1] = muxValue;
                            break;

                        case MUX_ADDRESS_5:
                            batt_ad_int_value = muxValue;
                            break;

                        case MUX_ADDRESS_6:
                            /* NAC 1 OC */
                            nacOverCurrent[NAC_1] = muxValue;
                            break;

                        case MUX_ADDRESS_7:
                            /* NAC 2 OC */
                            nacOverCurrent[NAC_2] = muxValue;
                            break;
                    }
                    break;
            }
        }
    }
#elif ( PANEL_3_5_ZONE )
    //on 5 zone panel, IDC 4 & 5 voltage inputs are presented to AN3 & AN4 A/D channels respectively
    /* if not verifying an alarm */
    if ( !latch4.latch4_output_bit.smoke_reset )
    {
        idc_ad_int_value[IDC_4] = ad_conversion(3);
        idc_ad_int_value[IDC_5] = ad_conversion(2);
    }

    //PIC A/D channel AN0 has the multiplexed inputs for IDC1,2,3 and NAC1,2. This loop increments
    //the mux address and requests A/D conversion for these voltage inputs.
    for ( addrMux = 0; addrMux < NUM_MUX_INPUTS; addrMux++ )
    {
        //clear lower 3 bits (address inputs to mux)
        PORTB &= 0b11111000;
        PORTB |= addrMux;

        switch( addrMux )
        {
            case MUX_ADDRESS_0:
                /* if not verifying an alarm */
                if ( !latch4.latch4_output_bit.smoke_reset )
                {
                    idc_ad_int_value[IDC_1] = ad_conversion(0);
                }
                break;

            case MUX_ADDRESS_1:
                /* if not verifying an alarm */
                if ( !latch4.latch4_output_bit.smoke_reset )
                {
                    idc_ad_int_value[IDC_2] = ad_conversion(0);
                }
                break;

            case MUX_ADDRESS_2:
                /* if not verifying an alarm */
                if ( !latch4.latch4_output_bit.smoke_reset )
                {
                    idc_ad_int_value[IDC_3] = ad_conversion(0);
                }
                break;

            case MUX_ADDRESS_3:
                nac_ad_int_value[NAC_1] = ad_conversion(0);
                break;

            case MUX_ADDRESS_4:
                nac_ad_int_value[NAC_2] = ad_conversion(0);
                break;

            case MUX_ADDRESS_5:
                aux_power_ad_int_value = ad_conversion(0);
                break;

            case MUX_ADDRESS_6:
                ground_ad_int_value = ad_conversion(0);
                break;

            case MUX_ADDRESS_7:
                ac_ad_int_value = ad_conversion(0);
                break;
        }
    }
#endif
    load_thresh_levels();

    //re-enable interrupt
    PEIE = 1;

}

/*
+------------------------------------------------------------------------------
| Purpose:
|	Takes A/D integer voltage values (from "scan_analog_inputs()") and compares to
|	integer thresholds. If within certain ranges (SUPERVISORY, OPEN, etc), these
|	voltages are then debounced. If, after the debounce time, they are still valid,
|	the appropriate "level" flags are set.
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|	-Called from MAIN EXECUTIVE LOOP (main.c) regardless of what state (ALARM, PROGRAM,etc)
|	system is in.
|   -IDC level bit flags set (each IDC has a corresponding array element in "idc_level[]":
|		open
|       normal
|       supv (when programmed as waterflow/supervisory "combo")
|       verified
|       non_ver  (NON-VERIFIED, WATERFLOW(programmed combo or not),SUPERVISORY (when not
|                     programmed combo), MONITOR
|
|	CALLS: None
|
+------------------------------------------------------------------------------
| Parameters: None
|
+------------------------------------------------------------------------------
| Return Value: None
|
+------------------------------------------------------------------------------
*/
void debounce_idc_levels(void)
{
    unsigned int idcValue, idc_open, idc_sup, idc_ver, idc_non;
    static unsigned char db_idc_x_state[MAX_IDC];
    unsigned int tempCleanMeAverage;
    unsigned int cleanMeLowerThreshhold;
    unsigned int cleanMeUpperThreshhold;

    // panel remains in troubles state until inputs debounced
    debounceCalled = 1;

    //---------------- check each idc voltage and set flags accordingly ---------------------
    for (idc_ctr = IDC_1; idc_ctr < MAX_IDC; idc_ctr++)
    {
        /* if in reset, clear IDC status */
        if ( system_state.system_state_bit.reset )
        {
            idc_level[idc_ctr].idc_level_byte  = 0;
            db_idc_x_state[idc_ctr]            = IDC_START;
            cleanMePulseDetect[idc_ctr]        = 0;
            cleanMePulseValid[idc_ctr]         = 0;
            cleanMeAverageNormal[idc_ctr]      = 0;
            cleanMeAverageCharging[idc_ctr]    = 0;
            cleanMeInitiationTimer[idc_ctr]    = 0;
            cleanMeValidationTimer[idc_ctr]    = 0;
            cleanMeRestorationTimer[idc_ctr]   = 0;
            continue;
        }
#if (PANEL_3_5_ZONE)
        if ( !five_zone_panel && idc_ctr > IDC_3 )
        {
            continue;
        }
#endif
        /* get current IDC's status */
        idcValue = idc_ad_int_value[idc_ctr];

        /* This if statement checks for even IDCs as IDC_1 is defined as 0 */
        if ( idc_ctr & 0x01 )
        {
            idc_open = idc_thresh[0];
            idc_sup  = idc_thresh[1];
            idc_ver  = idc_thresh[2];
            idc_non  = idc_thresh[3];
        }
        else
        {
            idc_open = idc_thresh_odd[0];
            idc_sup  = idc_thresh_odd[1];
            idc_ver  = idc_thresh_odd[2];
            idc_non  = idc_thresh_odd[3];

            /* use Class A A/D values additive */
            if ( ext_eeprom_read( eeprom_idc_location[idc_ctr] + EEPROM_IDC_OPTIONS_OFFSET ) & CLASS_A )
            {
                idcValue += idc_ad_int_value[idcCompanion(idc_ctr)];
            }
        }

        /* look for potential clean me pulses, if so configured.... */
        if ( ext_eeprom_read( eeprom_idc_location[idc_ctr] + EEPROM_IDC_OPTIONS_OFFSET ) & IDC_CLEAN_ME_ENABLED )
        {
            if ( latch2.latch2_output_bit.charger )
            {
                tempCleanMeAverage = cleanMeAverageCharging[idc_ctr];
            }
            else
            {
                tempCleanMeAverage = cleanMeAverageNormal[idc_ctr];
            }

            if ( tempCleanMeAverage )
            {
                cleanMeLowerThreshhold = tempCleanMeAverage + CLEAN_ME_MIN_THRESHOLD;
                cleanMeUpperThreshhold = tempCleanMeAverage + CLEAN_ME_MAX_THRESHOLD;

                if ( idcValue >= cleanMeLowerThreshhold && idcValue <= cleanMeUpperThreshhold )
                {
                    // Value above average normal detected, may be clean me pulse...
                    cleanMePulseDetect[idc_ctr]++;
                }
            }
        }

        switch (db_idc_x_state[idc_ctr])
        {
            case IDC_START:

                /* setup debounce time for IDC */
                idc_x_debounce_ctr[idc_ctr] = IDC_DEBOUNCE;

                //check current IDC zone for NON-VERIFIED condition
                if ( idcValue > idc_non && idcValue <= IDC_DISABLED )
                {
                    db_idc_x_state[idc_ctr] = IDC_NONVERIFIED_ALARM;
                }
                //check current IDC zone for VERIFIED ALARM condition
                else if ( idcValue > idc_ver && idcValue <= idc_non )
                {
                    db_idc_x_state[idc_ctr] = IDC_VERIFIED_ALARM;
                }
                //check current IDC zone for SUPERVISORY condition
                else if ( idcValue > idc_sup && idcValue <= idc_ver )
                {
                    db_idc_x_state[idc_ctr] = IDC_SUPERVISORY;
                }
                //check current IDC zone for OPEN condition
                else if ( idcValue <= idc_open )
                {
                    db_idc_x_state[idc_ctr] = IDC_OPEN;
                }
                //check current IDC zone for NORMAL condition
                else if ( idcValue > idc_open && idcValue <= idc_sup )
                {
                    db_idc_x_state[idc_ctr] = IDC_NORMAL;
                }
                break;

            //debounce current IDC zone for OPEN condition
            case IDC_OPEN:

                if (idcValue <= idc_open)
                {
                    /* if done debouncing */
                    if (!idc_x_debounce_ctr[idc_ctr])
                    {
                        //clear all bit flags and set idc_level[idc_ctr].idc_level_bit.open to 1
                        idc_level[idc_ctr].idc_level_byte = 0b00000001;
                        db_idc_x_state[idc_ctr] = IDC_START;
                    }
                }
                else
                {
                    db_idc_x_state[idc_ctr] = IDC_START;
                }
                break;

            //debounce current IDC zone for NORMAL condition
            case IDC_NORMAL:

                if ( idcValue > idc_open && idcValue <= idc_sup )
                {
                    /* if done debouncing */
                    if (!idc_x_debounce_ctr[idc_ctr])
                    {
                        //clear all bit flags and set idc_level[idc_ctr].idc_level_bit.normal to 1
                        idc_level[idc_ctr].idc_level_byte = 0b00000010;
                        db_idc_x_state[idc_ctr] = IDC_START;

                        //check for valid clean me pulse, if so configured
                        if ( ext_eeprom_read( eeprom_idc_location[idc_ctr] + EEPROM_IDC_OPTIONS_OFFSET ) & IDC_CLEAN_ME_ENABLED )
                        {
                            if ( latch2.latch2_output_bit.charger )
                            {
                                tempCleanMeAverage = cleanMeAverageCharging[idc_ctr];
                            }
                            else
                            {
                                tempCleanMeAverage = cleanMeAverageNormal[idc_ctr];
                            }

                            if ( tempCleanMeAverage )
                            {
                                // Average in current sample...
                                tempCleanMeAverage = ( tempCleanMeAverage + idcValue ) / 2;
                            }
                            else
                            {
                                // No samples taken yet, start averaging...
                                tempCleanMeAverage = idcValue;
                            }

                            if ( latch2.latch2_output_bit.charger )
                            {
                                cleanMeAverageCharging[idc_ctr] = tempCleanMeAverage;
                            }
                            else
                            {
                                cleanMeAverageNormal[idc_ctr] = tempCleanMeAverage;
                            }

                            if ( cleanMePulseDetect[idc_ctr] > CLEAN_ME_PULSE_DETECT_COUNT &&
                                 cleanMePulseValid[idc_ctr]  < CLEAN_ME_PULSE_VALID_COUNT_MAX )
                            {
                                cleanMePulseValid[idc_ctr]++;
                                if ( !idc_status3[idc_ctr].idc_status_bit3.cleanMeSequence )
                                {
                                    if ( cleanMePulseValid[idc_ctr] == 1 )
                                    {
                                        cleanMeInitiationTimer[idc_ctr] = CLEAN_ME_INITIATE_TIME;
                                    }
                                    else if ( cleanMePulseValid[idc_ctr] >= CLEAN_ME_VERIFY_TRIP_MIN &&
                                              cleanMePulseValid[idc_ctr] < CLEAN_ME_VERIFY_TRIP_MAX )
                                    {
                                        idc_status3[idc_ctr].idc_status_bit3.cleanMeSequence = 1;
                                        cleanMePulseValid[idc_ctr] = 0;
                                    }

                                }
                            }
                            if ( !cleanMeInitiationTimer[idc_ctr] )
                            {
                                cleanMePulseValid[idc_ctr] = 0;
                            }
                            cleanMePulseDetect[idc_ctr] = 0;
                        }
                    }
                }
                else
                {
                    db_idc_x_state[idc_ctr] = IDC_START;
                }
                break;

            //debounce current IDC zone for SUPERVISORY condition
            case IDC_SUPERVISORY:

                if ( idcValue > idc_sup && idcValue <= idc_ver )
                {
                    /* if done debouncing */
                    if (!idc_x_debounce_ctr[idc_ctr])
                    {
                        //clear all bit flags and set idc_level[idc_ctr].idc_level_bit.supv to 1
                        idc_level[idc_ctr].idc_level_byte = 0b00000100;
                        db_idc_x_state[idc_ctr] = IDC_START;
                    }
                }
                else
                {
                    db_idc_x_state[idc_ctr] = IDC_START;
                }
                break;

            //debounce current IDC zone for VERIFIED ALARM condition
            case IDC_VERIFIED_ALARM:

                /* value in range for verified alarm routine */
                if ( idcValue > idc_ver && idcValue <= idc_non )
                {
                    /* if done debouncing */
                    if (!idc_x_debounce_ctr[idc_ctr])
                    {
                        //clear all bit flags and set idc_level[idc_ctr].idc_level_bit.verified to 1
                        idc_level[idc_ctr].idc_level_byte = 0b00001000;
                        db_idc_x_state[idc_ctr] = IDC_START;
                    }
                }
                else
                {
                    db_idc_x_state[idc_ctr] = IDC_START;
                }
                break;

            //debounce current IDC zone for NON-VERIFIED condition
            case IDC_NONVERIFIED_ALARM:

                if ( idcValue > idc_non && idcValue <= IDC_DISABLED )
                {
                    /* if done debouncing */
                    if (!idc_x_debounce_ctr[idc_ctr])
                    {
                        //clear all bit flags and set idc_level[idc_ctr].idc_level_bit.non_ver to 1
                        idc_level[idc_ctr].idc_level_byte = 0b00010000;
                        db_idc_x_state[idc_ctr] = IDC_START;
                    }
                }
                else
                {
                    db_idc_x_state[idc_ctr] = IDC_START;
                }
                break;
        }
    }
}

/*
+------------------------------------------------------------------------------
| Purpose:
|	Takes A/D integer voltage values (from "scan_analog_inputs()") and compares to
|	integer thresholds. If within certain ranges (SHORTED, OPEN, etc), these
|	voltages are then debounced. If, after the debounce time, they are still valid,
|	the appropriate "level" flags are set.
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|	-Called from MAIN EXECUTIVE LOOP (main.c) regardless of what state (ALARM, PROGRAM,etc)
|	system is in.
|   -NAC level bit flags set (each NAC has a corresponding array element in "nac_level[]":
|       open
|       normal
|       shorted (only appicable when NAC off)
|		over_current (only applicable when NAC on)
|
|	CALLS: None
|
+------------------------------------------------------------------------------
| Parameters: None
|
+------------------------------------------------------------------------------
| Return Value: None
|
+------------------------------------------------------------------------------
*/
void debounce_nac_levels(void)
{
    unsigned int nacValue, nac_open, nac_short;
#if (PANEL_10_ZONE)
    unsigned int nacOC;
#endif
    unsigned char nac_tmp;
    static near unsigned char debounce_nac_x_state[MAX_NAC];

    for (nac_ctr = NAC_1; nac_ctr < MAX_NAC; nac_ctr++)
    {
        /* if in reset, clear NAC status */
        if ( system_state.system_state_bit.reset )
        {
            nac_level[nac_ctr].nac_level_byte = 0;
            debounce_nac_x_state[nac_ctr] = NAC_START;
            continue;
        }

        /* update temproary variables for comparision */
        nacValue = nac_ad_int_value[nac_ctr];
#if (PANEL_10_ZONE)
        nacOC = nacOverCurrent[nac_ctr];
#endif

		//see if current NAC is programmed CLASS A
        e2_data = ext_eeprom_read( eeprom_nac_location[nac_ctr] );

        /* get this NAC's current threshold values, depending on wiring class */
        if (e2_data & CLASS_A)
		{
#if (PANEL_3_5_ZONE)
            latch6.latch6_output_bit.nac_class_a = 0;
#elif (PANEL_10_ZONE)
            if ( nac_ctr < NAC_3 )
            {
                latch9.latch9_output_bit.nac_classA_1st = 1;
            }
            else
            {
                latch9.latch9_output_bit.nac_classA_2nd = 1;
            }
#endif
            //load CLASS A thresholds
            nac_open = nac_thresh[2];
            nac_short = nac_thresh[3];
		}
		else
		{
#if (PANEL_3_5_ZONE)
            latch6.latch6_output_bit.nac_class_a = 1;
#elif (PANEL_10_ZONE)
            if ( nac_ctr < NAC_3 )
            {
                latch9.latch9_output_bit.nac_classA_1st = 0;
            }
            else
            {
                latch9.latch9_output_bit.nac_classA_2nd = 0;
            }
#endif
            if ( e2_data & PARALLEL )
            {
                //load Parallel thresholds
                nac_open = nac_thresh[4];
                nac_short = nac_thresh[5];
            }
            else
            {
                //load CLASS B thresholds
                nac_open = nac_thresh[0];
                nac_short = nac_thresh[1];
            }
		}

        switch (debounce_nac_x_state[nac_ctr])
        {
            case NAC_START:
                /* set up debounce time for NAC*/
                nac_x_debounce_ctr[nac_ctr] = NAC_DEBOUNCE;

#if (PANEL_10_ZONE)
                /* NAC is ON */
                if ( nac_ctr < 2 && (latch3.latch3_output_byte & (1 << (nac_ctr+1)))
                        || nac_ctr >= 2 && (latch7.latch7_output_byte & (1 << (3-nac_ctr) )) )
                {
                    /* check for over current */
                    if ( !(ignoreNacOC & (0x01 << nac_ctr) ) && nacOC < NAC_OC_VALUE )
                    {
                        debounce_nac_x_state[nac_ctr] = NAC_OVERCURRENT;
                    }
                    /* check if currently selected NAC problem (looks like OPEN on active NAC) */
                    else if (nacValue <= nac_open)
                    {
                        debounce_nac_x_state[nac_ctr] = NAC_OPEN;
                        /* set up debounce time for NAC*/
                        nac_x_debounce_ctr[nac_ctr] = SEC_2;
                    }
                    /* check if currently selected NAC normal */
                    else
                    {
                        debounce_nac_x_state[nac_ctr] = NAC_NORMAL;
                    }
                }
#elif (PANEL_3_5_ZONE)
                /* NAC is ON */
                if ( latch3.latch3_output_byte & (1 << (nac_ctr+1)) )
                {
                    if ( !(ignoreNacOC & (0x01 << nac_ctr) ) && nacValue < NAC_OC_VALUE )
                    {
                        debounce_nac_x_state[nac_ctr] = NAC_OVERCURRENT;
                        nac_x_oc_restore_ctr[nac_ctr] = OC_RESTORE_DEBOUNCE;
#if (NAC_SHORT_TEST_PIN)
                        if (NAC_2 == nac_ctr)
                            latch6.latch6_output_bit.unused = 1;
                    }
                    else
                    {
                        if (NAC_2 == nac_ctr)
                            latch6.latch6_output_bit.unused = 0;
#endif
                    }
                }
#endif
                /* NAC is OFF */
                else
                {
#if (NAC_SHORT_TEST_PIN)
                    if (NAC_2 == nac_ctr)
                        latch6.latch6_output_bit.unused = 0;
#endif
                    //check if currently selected NAC OPEN
                    if (nacValue <= nac_open)
                    {
                        debounce_nac_x_state[nac_ctr] = NAC_OPEN;
                        /* set up debounce time for NAC, long for an OPEN */
                        nac_x_debounce_ctr[nac_ctr] = SEC_2;
                    }
                    //check if currently selected NAC shorted
                    else if (nacValue < NAC_DISABLED && nacValue > nac_short)
                    {
                        debounce_nac_x_state[nac_ctr] = NAC_SHORTED;
                    }
                    //check if currently selected NAC normal
                    else
                    {
                        debounce_nac_x_state[nac_ctr] = NAC_NORMAL;
                    }
                }
                break;

            //debounce current NAC for OPEN condition
            case NAC_OPEN:

                /* if NAC still reading open */
                if (nacValue < nac_open)
                {
                    /* if done debouncing */
                    if ( !nac_x_debounce_ctr[nac_ctr] )
                    {
                        nac_level[nac_ctr].nac_level_bit.open = 1;
                        debounce_nac_x_state[nac_ctr] = NAC_START;
                    }
                }
                /* NAC is not reading open, restart*/
                else
                {
                    debounce_nac_x_state[nac_ctr] = NAC_START;
                }
                break;

            //debounce current NAC for NORMAL condition
            case NAC_NORMAL:

                /* if NAC reading normal */
                if ( nacValue > nac_open && nacValue < nac_short )
                {
                    /* if done debouncing */
                    if ( !nac_x_debounce_ctr[nac_ctr] )
                    {
                        //clear all bit flags
                        nac_level[nac_ctr].nac_level_byte = 0;
                        nac_level[nac_ctr].nac_level_bit.normal = 1;
                        debounce_nac_x_state[nac_ctr] = NAC_START;
                    }
                }
                /* NAC is not reading normal, restart*/
                else
                {
                    debounce_nac_x_state[nac_ctr] = NAC_START;
                }
                break;

            //debounce current NAC for SHORTED condition
            case NAC_SHORTED:

                /* if NAC readin disabled */
                if ( nacValue < NAC_DISABLED && nacValue > nac_short )
                {
                    /* if done debouncing */
                    if ( !nac_x_debounce_ctr[nac_ctr] )
                    {
                        nac_level[nac_ctr].nac_level_bit.shorted = 1;
                        debounce_nac_x_state[nac_ctr] = NAC_START;
                    }
                }
                /* NAC is not reading disabled, restart*/
                else
                {
                    debounce_nac_x_state[nac_ctr] = NAC_START;
                }
                break;

#if (PANEL_10_ZONE)
            //debounce current NAC for OVER CURRENT condition (only if current NAC is on)
            case NAC_OVERCURRENT:

                /* if NOT ignoring NAC input and reading OC */
                if ( !(ignoreNacOC & (0x01 << nac_ctr) ) && nacOC < NAC_OC_VALUE )
                {
                    /* if done debouncing */
                    if ( !nac_x_debounce_ctr[nac_ctr] )
                    {
                        nac_level[nac_ctr].nac_level_bit.over_current = 1;
                        debounce_nac_x_state[nac_ctr] = NAC_START;

                        /* Check for Class A */
                        if ( e2_data & CLASS_A )
                        {
                            /* figure out even or odd nac */
                            nac_tmp = (nac_ctr & 0x01) ? (nac_ctr - 1) : (nac_ctr + 1);
                            /* set appropriate OC */
                            nac_level[nac_tmp].nac_level_bit.over_current = 1;
                            debounce_nac_x_state[nac_tmp] = NAC_START;
                        }
                    }
                }
                /* Not reading OC or supposed to ignore NAC reading*/
                else
                {
                    debounce_nac_x_state[nac_ctr] = NAC_START;
                }
				break;

#elif (PANEL_3_5_ZONE)
            //debounce current NAC for OVER CURRENT condition (only if current NAC is on)
            case NAC_OVERCURRENT:

                /* if NOT ignoring NAC input and reading OC */
                if ( !(ignoreNacOC & (0x01 << nac_ctr) ) && nacValue < NAC_OC_VALUE )
                {
                    nac_x_oc_restore_ctr[nac_ctr] = OC_RESTORE_DEBOUNCE;

                    /* if done debouncing */
                    if ( !nac_x_debounce_ctr[nac_ctr] )
                    {
                        /* if running on battery, then valid OC */
                        if ( latch2.latch2_output_bit.main_battery )
                        {
                            nac_level[nac_ctr].nac_level_bit.over_current = 1;
                            debounce_nac_x_state[nac_ctr] = NAC_START;

                            /* Check for Class A */
                            if ( e2_data & CLASS_A )
                            {
                                /* figure out even or odd nac */
                                nac_tmp = (nac_ctr & 0x01) ? (nac_ctr - 1) : (nac_ctr + 1);
                                /* set appropriate OC */
                                nac_level[nac_tmp].nac_level_bit.over_current = 1;
                                debounce_nac_x_state[nac_tmp] = NAC_START;
                            }
                        }
                        /* if not on battery, maybe AC loss */
                        else
                        {
                            nac_status[nac_ctr].nac_status_bit.OC_wait = 1;
                            debounce_nac_x_state[nac_ctr] = NAC_OVERCURRENT_WAIT;
                            nac_x_debounce_ctr[nac_ctr] = MS500;

                            /* Check for Class A */
                            if ( e2_data & CLASS_A )
                            {
                                /* figure out even or odd nac */
                                nac_tmp = (nac_ctr & 0x01) ? (nac_ctr - 1) : (nac_ctr + 1);
                                nac_status[nac_tmp].nac_status_bit.OC_wait = 1;
                                debounce_nac_x_state[nac_tmp] = NAC_OVERCURRENT_WAIT;
                                nac_x_debounce_ctr[nac_tmp] = MS500;
                            }
                        }
                    }
                }
                /* Not reading OC or supposed to ignore NAC reading*/
                else
                {
                    /* We get here if we are ignoring OC readings or if
                     * the OC reading was good. In either case, not NOT go
                     * back to NAC_START unless the nac_x_oc_restore_ctr[]
                     * has hit zero - we wnat to ensure at least
                     * OC_RESTORE_DEBOUNCE readings in a row were good before
                     * declaring the overcurrent resolved.
                     */
                    if (nac_x_oc_restore_ctr[nac_ctr]) {
                        nac_x_oc_restore_ctr[nac_ctr]--;
                    } else {
#if (NAC_SHORT_TEST_PIN)
                        if (NAC_2 == nac_ctr)
                            latch6.latch6_output_bit.unused = 0;
#endif
                        debounce_nac_x_state[nac_ctr] = NAC_START;
                    }
                }
                break;

            case NAC_OVERCURRENT_WAIT:

                /* if NOT ignoring NAC input and reading OC */
                if ( !(ignoreNacOC & (0x01 << nac_ctr) ) && nacValue < NAC_OC_VALUE )
                {
                    nac_x_oc_restore_ctr[nac_ctr] = OC_RESTORE_DEBOUNCE;

                    /* if done waiting */
                    if ( !nac_x_debounce_ctr[nac_ctr] )
                    {
                        /* if we did switch to battery, AC loss, restart state machine
                         * 
                         * Note: acFailing is set as soon as the AC voltage dips below
                         *       the brownout threshold - for the case of the NAC
                         *       overcurrent test, we will ignore the fault if we
                         *       have switched to battery OR if we are thinking about
                         *       switching to battery - that's what acFailing is
                         *       telling us.
                         *
                         * Once we have actually switched to battery, the NAC fault
                         * is properly caught in the NAC_OVERCURRENT case.
                         */
                        if ( latch2.latch2_output_bit.main_battery || acFailing )
                        {
                            debounce_nac_x_state[nac_ctr] = NAC_START;
                            nac_status[nac_ctr].nac_status_bit.OC_wait = 0;
    
                            /* Check for Class A */
                            if ( e2_data & CLASS_A )
                            {
                                /* figure out even or odd nac */
                                nac_tmp = (nac_ctr & 0x01) ? (nac_ctr - 1) : (nac_ctr + 1);
                                nac_status[nac_tmp].nac_status_bit.OC_wait = 0;
                                debounce_nac_x_state[nac_tmp] = NAC_START;
                            }
                        }
                        /* if we didn't switch to battery, valid OC */
                        else
                        {
                            nac_level[nac_ctr].nac_level_bit.over_current = 1;
                            debounce_nac_x_state[nac_ctr] = NAC_START;
    
                            /* Check for Class A */
                            if ( e2_data & CLASS_A )
                            {
                                /* figure out even or odd nac */
                                nac_tmp = (nac_ctr & 0x01) ? (nac_ctr - 1) : (nac_ctr + 1);
                                /* set appropriate OC */
                                nac_level[nac_tmp].nac_level_bit.over_current = 1;
                                debounce_nac_x_state[nac_tmp] = NAC_START;
                            }
                        }
                    }
                }
                /* Not reading OC or supposed to ignore NAC reading*/
                else
                {
                    /* We get here if we are ignoring OC readings or if
                     * the OC reading was good. In either case, not NOT go
                     * back to NAC_START unless the nac_x_oc_restore_ctr[]
                     * has hit zero - we wnat to ensure at least
                     * OC_RESTORE_DEBOUNCE readings in a row were good before
                     * declaring the overcurrent resolved.
                     */

                    if (nac_x_oc_restore_ctr[nac_ctr]) {
                        nac_x_oc_restore_ctr[nac_ctr]--;
                    } else {
#if (NAC_SHORT_TEST_PIN)
                        if (NAC_2 == nac_ctr)
                            latch6.latch6_output_bit.unused = 0;
#endif
                        debounce_nac_x_state[nac_ctr] = NAC_START;
                    }
                }
                break;
#endif
        }
    }
}

/*
+------------------------------------------------------------------------------
| Purpose:
|	Takes A/D integer voltage values (from "scan_analog_inputs()") and compares to
|	integer thresholds. If within certain ranges, corresponding to which button
|	is pressed, these voltages are then debounced. If, after the debounce time,
|	they are still valid, the appropriate "button pressed" flags are set. The flags
|	are cleared, elswhere in the program, when the button press is acknowledged. If
|	not acknowledged, the flag expires after "button valid timer" expires (dec'd to
|	zero in interrupt).
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|	-Called from MAIN EXECUTIVE LOOP (main.c) regardless of what state (ALARM, PROGRAM,etc)
|	system is in.
|	-Button pressed flags set:
|       remote_disconnect
|       lamp_test (combination of remote diconnect and walk test pressed simultaneously)
|       walk_test
|       trouble_silence
|       alarm_sil_drill
|       reset
|       program (this is a suitcase jumper as opposed to a button)
|       idc_1_disable
|       idc_2_disable
|       idc_3_disable
|       idc_4_disable
|       idc_5_disable
|       nac_1_disable
|       nac_2_disable
|	-Debounce times for remote diconnect and walk test are much longer than for lamp
|	test (the two pressed simultaneously) so that if one of the two is pressed slightly
|	before the other, when attempting the simultaneous press, they won't be acted on
|	prematurely and appear as a single button press.
|
|	CALLS: button_pressed()
|
+------------------------------------------------------------------------------
| Parameters: None
|
+------------------------------------------------------------------------------
| Return Value: None
|
+------------------------------------------------------------------------------
*/
void button_handler(void)
{
    unsigned char index, idcButton = BUTTON_NOT_PRESSED, nacButton = BUTTON_NOT_PRESSED, e2_data;
    register unsigned int tempIdcNac;
    static bit button_pressed_flag;

    /* if system is not currently in alarm */
    if ( !latch1.latch1_output_bit.com_alarm || system_state.system_state_bit.program )
    {
        /* if programming jumper IN and not in programming mode */
        if ( !RA4 && !system_state.system_state_bit.program )
        {
            /* is program jumper debounced */
            if ( programming_ctr > BUTTON_DEBOUNCE )
            {
                /* reset counter */
                programming_ctr = 0;

                //clear all flags
                system_state.system_state_byte = 0;

                //set flag enabling PROGRAM state
                system_state.system_state_bit.program = 1;
            }
            /* not debounced */
            else
            {
                programming_ctr++;
            }
        }
        /* programming jumper out and in programming mode */
        else if ( RA4 && system_state.system_state_bit.program )
        {
            /* is program jumper debounced */
            if ( programming_ctr > BUTTON_DEBOUNCE )
            {
                /* reset counter */
                programming_ctr = 0;

                //set flag disabling PROGRAM state
                system_state.system_state_bit.program = 0;

                //call program() one more time to clean up programmin mode
                programCleanUp = 1;
            }
            /* not debounced */
            else
            {
                programming_ctr++;
            }
        }
    }

    /* loop though available DISABLE buttons and look for a pressed one */
    for ( index = IDC_1; index < MAX_IDC; index++ )
    {
#if (PANEL_3_5_ZONE)
        /* if not 5 zone, do not check zones 4 and 5 */
        if ( !five_zone_panel && index > IDC_3 )
        {
            continue;
        }
#endif

        /* Use temp variable for comparison - compiler bug */
        tempIdcNac = idc_ad_int_value[index];

        /* check for IDC Disable button pressed */
        if ( tempIdcNac > IDC_DISABLED )
        {
            idcButton = index;
            break;
        }
        if ( index < MAX_NAC )
        {
            /* Use temp variable for comparison - compiler bug */
            tempIdcNac = nac_ad_int_value[index];

            /* check for NAC Disable button pressed */
            if ( tempIdcNac > NAC_DISABLED )
            {
                nacButton = index;
                break;
            }
        }
    }

    /* flag if any buttons pressed */
    if ( an4_buttons_ad_int_value <= IDC_DISABLED || idcButton != BUTTON_NOT_PRESSED || nacButton != BUTTON_NOT_PRESSED )
    {
        button_pressed_flag = 1;
    }
    else
    {
        button_pressed_flag = 0;
    }

    /* if button still active and still pressed, */
    /* ignore until released.                    */
    if ( button_active_flag && button_pressed_flag )
    {
        return;
    }

    //has timer expired that controls the "life" of button flags
    if ( !button_valid_timer )
    {
        //if so, reset all button flags
        button.button_int = 0;
#if (PANEL_10_ZONE)
        button2.button_int2 = 0;
#endif
        button_active_flag = 0;
    }

    //check for NO BUTTON pressed
    if ( !button_pressed_flag )
    {
        //if so, clear all debounce counters
        any_ad4_button_ctr = 0;
        any_button_ctr = 0;
        lamp_test_ctr = 0;
        remote_disconnect_ctr = 0;
        walk_test_ctr = 0;
    }

    //check for RESET button PRESSED
    if ( an4_buttons_ad_int_value <= 81 )
    {
        if ( ++any_ad4_button_ctr > BUTTON_DEBOUNCE )
        {
            button.button_bit.reset = 1;
            button_pressed();
        }
    }
    //check for LAMP TEST buttons (remote dis & walk test) pressed
    else if ( an4_buttons_ad_int_value >= 82 && an4_buttons_ad_int_value < 188 )
    {
        if ( ++lamp_test_ctr > BUTTON_DEBOUNCE )
        {
            button.button_bit.lamp_test = 1;
            button_pressed();
        }
    }
    //check for REMOTE DISCONNECT button pressed
    else if ( an4_buttons_ad_int_value >= 188 && an4_buttons_ad_int_value < 311 )
    {
        if ( ++remote_disconnect_ctr > BUTTON_DEBOUNCE )
        {
                if (!( ext_eeprom_read(EEPROM_DISABLE_LOCKOUT) & (REMOTE_DISCONNECT_LOCKOUT << DISABLE_LOCKOUT_SHIFT) )
                        || system_state.system_state_bit.program )
                {
                    button.button_bit.remote_disconnect = 1;
                }
                else
                {
                    buzzer_lockout_flag = 1;
                }
                button_pressed();
        }
    }
    //check for WALK TEST button pressed
    else if ( an4_buttons_ad_int_value >= 311 && an4_buttons_ad_int_value < 510 )
    {
        if ( ++walk_test_ctr > BUTTON_DEBOUNCE )
        {
            button.button_bit.walk_test = 1;
            button_pressed();
        }
    }
    //check for ALARM SILENCE/DRILL button pressed
    else if ( an4_buttons_ad_int_value >= 510 && an4_buttons_ad_int_value < 713 )
    {
        /* if in alarm, sup or program mode, use short debounce time */
        if ( system_state.system_state_bit.program || (latch1.latch1_output_byte & 0b00010100) )
        {
            if ( ++any_ad4_button_ctr > BUTTON_DEBOUNCE )
            {
                button.button_bit.alarm_sil_drill = 1;
                button_pressed();
            }
        }
        /* otherwise, for entering drill mode, use long debounce time */
        else
        {
            if ( ++any_ad4_button_ctr > DRILL_BUTTON_DEBOUNCE )
            {
                button.button_bit.alarm_sil_drill = 1;
                system_state.system_state_bit.drill = 1;
                button_pressed();
            }
        }
    }
    //check for TROUBLE SILENCE button pressed
    else if ( an4_buttons_ad_int_value >= 713 && an4_buttons_ad_int_value < 922 )
    {
        if ( ++any_ad4_button_ctr > BUTTON_DEBOUNCE )
        {
            button.button_bit.trouble_silence = 1;
            button_pressed();
        }
    }
    //check for IDC DISABLE buttons pressed
    else if ( idcButton != BUTTON_NOT_PRESSED )
    {
        if ( ++any_button_ctr > BUTTON_DEBOUNCE )
        {
            /* if not in programming mode */
            if (!system_state.system_state_bit.program)
            {
                if (!( ext_eeprom_read(EEPROM_DISABLE_LOCKOUT) & (IDC_LOCKOUT << DISABLE_LOCKOUT_SHIFT) ))
                {
                    //see if current IDC is programmed CLASS A
                    e2_data = ext_eeprom_read( eeprom_idc_location[idcButton] + EEPROM_IDC_OPTIONS_OFFSET );

                    /* if class A make sure NACs disabled in pairs */
                    if ( ( e2_data & CLASS_A ) && ( idcButton & 0x01 ) )
                    {
                        /* if even IDC, treat like previous odd IDC was pressed */
                        idcButton--;
                    }

                    //toggle flag
                    idc_status[idcButton].idc_status_bit.disabled = !idc_status[idcButton].idc_status_bit.disabled;

                    /* if class A make sure IDCs disabled in pairs */
                    if ( e2_data & CLASS_A )
                    {
                        idc_status[idcButton+1].idc_status_bit.disabled = idc_status[idcButton].idc_status_bit.disabled;
                    }
                }
                else
                {
                    buzzer_lockout_flag = 1;
                }
            }
#if (PANEL_10_ZONE)
            /* set respective disable bit for IDCs 1-9 */
            if ( idcButton < IDC_10 )
            {
                button.button_int |= ( 0x0001 << (idcButton+IDC_BIT_OFFSET) );
            }
            /* IDC 10, is first bit, in a different structure */
            else
            {
                button2.button_int2 |= 0x0001;
            }
#elif (PANEL_3_5_ZONE)
            button.button_int |= ( 0x0001 << ( idcButton+IDC_BIT_OFFSET ) );
#endif
            button_pressed();
        }
    }
    //check for NAC DISABLE buttons pressed
    else if ( nacButton != BUTTON_NOT_PRESSED )
    {
        if ( ++any_button_ctr > BUTTON_DEBOUNCE )
        {
            /* if not in programming mode */
            if ( !system_state.system_state_bit.program )
            {
                //see if current NAC is programmed CLASS A
                e2_data = ext_eeprom_read( eeprom_nac_location[nacButton] );

                if (( e2_data & NAC_TYPE_MASK ) != CITY_TIE
                        && !( ext_eeprom_read(EEPROM_DISABLE_LOCKOUT) & (NAC_LOCKOUT << DISABLE_LOCKOUT_SHIFT) ))
                {
                    /* if class A make sure NACs disabled in pairs */
                    if ( ( e2_data & CLASS_A ) && ( nacButton & 0x01 ) )
                    {
                        /* if even NAC, treat like previous odd NAC was pressed */
                        nacButton--;
                    }
                    //toggle flag
                    nac_status[nacButton].nac_status_bit.disabled = !nac_status[nacButton].nac_status_bit.disabled;
                    /* if class A make sure NACs disabled in pairs */
                    if ( e2_data & CLASS_A )
                    {
                        nac_status[nacButton+1].nac_status_bit.disabled = nac_status[nacButton].nac_status_bit.disabled;
                    }
                }
                else
                {
                    buzzer_lockout_flag = 1;
                }
            }
#if (PANEL_10_ZONE)
            /* bits 1-4 of button.button_int2 are NAC_1-NAC_4, respectively */
            button2.button_int2 |= (0x0001 << (nacButton+NAC_BIT_OFFSET));
#elif (PANEL_3_5_ZONE)
            button.button_int |= (0x0001 << (nacButton+NAC_BIT_OFFSET));
#endif
            button_pressed();
        }
    }
}

/*
+------------------------------------------------------------------------------
| Purpose:
|	Resets button pressed debounce counters and starts button pressed "lifetime"
|	timer ("button_valid_timer").
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
| 	-Function is local to this module only.
|	-Called by "button_handler()".
|	-"button_valid_timer" controls lifetime of button press. It is loaded with
|	a value that is decremented to 0 (dec'd in interrupt). "button_handler()"
|	checks to see if this timer reaches zero before any button pressed flag
|	is cleared elsewhere in the program (flag cleared acts as an acknowledgement).
|	If the timer reaches zero then the lifetime of the button has been reached and
|	the flag is cleared. This prevents any "old" button presses from being acted upon
|	inappropriately.
|	-Sets flag that requests a tactile buzzer "blip" (actual blip done in "outputs.c")
|	-Clears debounce counters that are used by "button_handler()".
|
|	CALLS: None
|
+------------------------------------------------------------------------------
| Parameters: None
|
+------------------------------------------------------------------------------
| Return Value: None
|
+------------------------------------------------------------------------------
*/
static void button_pressed(void)
{
    button_active_flag = 1;
    any_ad4_button_ctr = 0;
    any_button_ctr = 0;
    lamp_test_ctr = 0;
    remote_disconnect_ctr = 0;
    walk_test_ctr = 0;
    button_valid_timer = 50;

    //check if buzzer is currently inactive
    if (!latch2.latch2_output_bit.buzzer)
    {
        //only allow tactile blip if buzzer currently inactive
        buzzer_blip_flag = !buzzer_lockout_flag;
    }

    /* this is for programming mode */
    programmingSecs = 90;
}

/* OUTPUTS */

/**************************************************************************
 ** FUNCTION: output_refresh                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the output latched according to the way latch  **
 ** unions are set. Also special buzzer handling occurs here.            **
 **                                                                      **
 ** PARAMETERS:  none.                                                   **
 **                                                                      **
 ** RETURNS:  none.                                                      **
 **                                                                      **
 **************************************************************************/
void output_refresh(void)
{
    unsigned char outputLatchBit, latchAddress;
    unsigned char latchBit;
    unsigned char buzzerMaxCount;
    unsigned int  buzzerStates;
    static unsigned int buzzerBlipCounter;
    static unsigned char state_2x20;
    static unsigned char buzzerCount;
    static unsigned char flashMask = 0;
    static bit disableWasOn = 0;
    static bit cleanMeWasActive = 0;

#if 1
#if ( BUZZER_TEST )
    /* for 8 hours, turn buzzer on 5 mins, buzzer off 5 mins */
    if ( buzzerPhaseMins )
    {
        /* if 5 mins up, toggle buzzer output */
        if ( !buzzerToggleSecs )
        {
            latch2.latch2_output_bit.buzzer = !latch2.latch2_output_bit.buzzer;
            /* reset timer */
            buzzerToggleSecs = 300;
        }
    }
    /* after 8 hours, turn buzzer on */
    else
    {
        latch2.latch2_output_bit.buzzer = 1;
    }
#else
    /* if the buzzer needs to be turned ON */
    if (buzzer_on_flag)
    {
        /* Alarm and Supervisory override any other buzzer set */
        if ( latch1.latch1_output_bit.com_alarm || latch1.latch1_output_bit.com_sup )
        {
            latch2_pulse.latch2_pulse_bit.buzzer = 1;
        }
        /* if in program or drill mode, buzzer follows buzzer_on_flag */
        else if ( system_state.system_state_bit.program || (drillTemporal && !latch1_pulse.latch1_pulse_bit.com_trbl) )
        {
            latch2.latch2_output_bit.buzzer = 1;
        }
        /* in any other mode, buzzer is in 20SPM blip mode. */
        else
        {
            latch2_pulse.latch2_pulse_bit.buzzer = 1;
        }
    }
    /* Buzzer not needed, turn OFF */
    else
    {
        latch2_pulse.latch2_pulse_bit.buzzer=0;
        latch2.latch2_output_bit.buzzer = 0;
    }

    // if startup condition and not programming, maintain trouble state
    if (!debounceCalled && !system_state.system_state_bit.program && !system_state.system_state_bit.trouble)
    {
        //turn COMMON TROUBLE LED on steady
        trouble_LED_flag = 1;
        //activate trouble relay
		latch4.latch4_output_bit.trouble_relay = 1;
        //activate steady buzzer
        latch2.latch2_output_bit.buzzer = 1;
        buzzer_on_flag = 1;
    }

    /* handle a buzzer blip (tactile beep) */
    if ( buzzer_blip_flag || buzzer_enter_flag || buzzer_lockout_flag )
    {
        if ( buzzer_enter_flag )
        {
            buzzer_blip_flag = 0;
            buzzer_lockout_flag = 0;
        }

        latch2.latch2_output_bit.buzzer = 1;
        buzzerBlipCounter++;

        if (  ((buzzerBlipCounter >= 20 ) && buzzer_blip_flag    )
           || ((buzzerBlipCounter >= 50 ) && buzzer_lockout_flag )
           || ((buzzerBlipCounter >= 100) && buzzer_enter_flag   ))
        {
            buzzerBlipCounter = 0;
            buzzer_blip_flag = 0;
            buzzer_enter_flag = 0;
            buzzer_lockout_flag = 0;
            latch2.latch2_output_bit.buzzer = 0;
        }
        else if ( buzzer_lockout_flag )
        {
           latch2.latch2_output_bit.buzzer = ( buzzerBlipCounter / 5 ) & 1;
        }
    }
#endif


    /* If not in programming mode or in reset */
    if ( !(system_state.system_state_byte & 0b00001100) && !button.button_bit.lamp_test )
    {
        /* decide what to do with trouble LED */
        latch1.latch1_output_bit.com_trbl = 0;
        if ( trouble_LED_flag )
        {
            latch1.latch1_output_bit.com_trbl = 1;
        }
    }

    /* if flash rate time has timed out */
    if (!pulse_120spm_timer)
    {
        //restart flash rate timer (dec'd in interrupt)
        pulse_120spm_timer = 25;
        flashMask = ~flashMask;
#if 1
        for (latchBit = 0; latchBit < MAX_BITS; latchBit++)
        {
            outputLatchBit = 1 << latchBit;
            if (latch1_pulse.latch1_pulse_byte & outputLatchBit)
            {
                latch1.latch1_output_byte ^= outputLatchBit;
            }
            if ( latch2_pulse.latch2_pulse_byte & outputLatchBit )
            {
                /* buzzer output bit */
                if ( latchBit == 7 && !system_state.system_state_bit.program )
                {
                    buzzerMaxCount = 16;
                    if ( latch1.latch1_output_bit.com_alarm )
                    {
                        buzzerStates   = 0xCCC0;
                    }
                    else if ( latch1.latch1_output_bit.com_sup )
                    {
                        buzzerStates   = 0xAAAA;
                    }
                    else
                    {
                        buzzerStates   = 0x0800;
                        buzzerMaxCount = 12;
                    }

                    if ( buzzerStates & ( 1 << buzzerCount ) )
                    {
                        latch2.latch2_output_byte |= outputLatchBit;
                    }
                    else
                    {
                        latch2.latch2_output_byte &= ~outputLatchBit;
                    }
                    buzzerCount++;
                    if ( buzzerCount >= buzzerMaxCount )
                    {
                        buzzerCount = 0;
                    }
                }
                /* all other outputs */
                else
                {
                    latch2.latch2_output_byte ^=  outputLatchBit;
                }
            }
            if (latch4_pulse.latch4_pulse_byte & outputLatchBit)
            {
                latch4.latch4_output_byte ^= outputLatchBit;
            }
            if (latch5_pulse.latch5_pulse_byte & outputLatchBit)
            {
                latch5.latch5_output_byte ^= outputLatchBit;
            }
            if (latch6_pulse.latch6_pulse_byte & outputLatchBit)
            {
                latch6.latch6_output_byte ^= outputLatchBit;
            }
#if (PANEL_10_ZONE)
            if (latch7_pulse.latch7_pulse_byte & outputLatchBit)
            {
                latch7.latch7_output_byte ^= outputLatchBit;
            }
            if (latch8_pulse.latch8_pulse_byte & outputLatchBit)
            {
                latch8.latch8_output_byte ^= outputLatchBit;
            }
            if (latch9_pulse.latch9_pulse_byte & outputLatchBit)
            {
                latch9.latch9_output_byte ^= outputLatchBit;
            }
#endif
        }
#else
        for (latchBit = 0; latchBit < MAX_BITS; latchBit++)
        {
            outputLatchBit = 1 << latchBit;
            if (latch1_pulse.latch1_pulse_byte & outputLatchBit)
            {
                latch1.latch1_output_byte &= ~outputLatchBit;
                latch1.latch1_output_byte |= (outputLatchBit&flashMask);
            }
            if ( latch2_pulse.latch2_pulse_byte & outputLatchBit )
            {
                /* buzzer output bit */
                if ( latchBit == 7 && !system_state.system_state_bit.program )
                {
                    buzzerMaxCount = 16;
                    if ( latch1.latch1_output_bit.com_alarm )
                    {
                        buzzerStates   = 0xCCC0;
                    }
                    else if ( latch1.latch1_output_bit.com_sup )
                    {
                        buzzerStates   = 0xAAAA;
                    }
                    else
                    {
                        buzzerStates   = 0x0800;
                        buzzerMaxCount = 12;
                    }

                    if ( buzzerStates & ( 1 << buzzerCount ) )
                    {
                        latch2.latch2_output_byte |= outputLatchBit;
                    }
                    else
                    {
                        latch2.latch2_output_byte &= ~outputLatchBit;
                    }
                    buzzerCount++;
                    if ( buzzerCount >= buzzerMaxCount )
                    {
                        buzzerCount = 0;
                    }
                }
                /* all other outputs */
                else
                {
                    latch2.latch2_output_byte ^=  outputLatchBit;
                }
            }
            if (latch4_pulse.latch4_pulse_byte & outputLatchBit)
            {
                latch4.latch4_output_byte &= ~outputLatchBit;
                latch4.latch4_output_byte |= (outputLatchBit&flashMask);
            }
            if (latch5_pulse.latch5_pulse_byte & outputLatchBit)
            {
                latch5.latch5_output_byte &= ~outputLatchBit;
                latch5.latch5_output_byte |= (outputLatchBit&flashMask);
            }
            if (latch6_pulse.latch6_pulse_byte & outputLatchBit)
            {
                latch6.latch6_output_byte &= ~outputLatchBit;
                latch6.latch6_output_byte |= (outputLatchBit&flashMask);
            }
#if (PANEL_10_ZONE)
            if (latch7_pulse.latch7_pulse_byte & outputLatchBit)
            {
                latch7.latch7_output_byte &= ~outputLatchBit;
                latch7.latch7_output_byte |= (outputLatchBit&flashMask);
            }
            if (latch8_pulse.latch8_pulse_byte & outputLatchBit)
            {
                latch8.latch8_output_byte &= ~outputLatchBit;
                latch8.latch8_output_byte |= (outputLatchBit&flashMask);
            }
            if (latch9_pulse.latch9_pulse_byte & outputLatchBit)
            {
                latch9.latch9_output_byte &= ~outputLatchBit;
                latch9.latch9_output_byte |= (outputLatchBit&flashMask);
            }
#endif
        }
#endif
    }

    /* if timer (2 X 20 spm) has timed out */
    if ( !pulse_2x20_timer )
    {
        /* flag that disable need to be turned ON */
        if ( pulse_2x20_flag || RRM_pulse_2x20_flag )
        {
            disableWasOn = 1;
        }
        else
        {
            if ( disableWasOn )
            {
                disableWasOn = 0;
                latch1.latch1_output_bit.com_disable = 0;
            }
        }

        /* flag that service detector needs to be turned ON */
        if ( alarm_pulse_2x20_flag )
        {
            cleanMeWasActive = 1;
        }
        else
        {
            if ( cleanMeWasActive )
            {
                cleanMeWasActive = 0;
#if (PANEL_3_5_ZONE)
                latch6.latch6_output_bit.com_service_detector = 0;
#elif (PANEL_10_ZONE)
                latch9.latch9_output_bit.com_service_detector = 0;
#endif
            }
        }

        if ( pulse_2x20_flag || RRM_pulse_2x20_flag || alarm_pulse_2x20_flag )
        {
            //restart timer (2 X 20 spm)
            pulse_2x20_timer = 50;

            /* if in alarm, DO NOT pulse disable LAMP */
            if ( latch1.latch1_output_bit.com_alarm )
            {
                if ( disableWasOn )
                {
                    latch1.latch1_output_bit.com_disable = 1;
                }
                if ( cleanMeWasActive )
                {
#if (PANEL_3_5_ZONE)
                    latch6.latch6_output_bit.com_service_detector = 1;
#elif (PANEL_10_ZONE)
                    latch9.latch9_output_bit.com_service_detector = 1;
#endif
                }
            }
            else
            {
                state_2x20++;
                switch (state_2x20)
                {
                    case 1:
                        if ( disableWasOn )
                        {
                            latch1.latch1_output_bit.com_disable = 1;
                        }
                        if ( cleanMeWasActive )
                        {
#if (PANEL_3_5_ZONE)
                            latch6.latch6_output_bit.com_service_detector = 1;
#elif (PANEL_10_ZONE)
                            latch9.latch9_output_bit.com_service_detector = 1;
#endif
                        }
                        break;

                    case 2:
                    case 3:
                    case 4:
                        if ( disableWasOn )
                        {
                            latch1.latch1_output_bit.com_disable = !latch1.latch1_output_bit.com_disable;
                        }
                        if ( cleanMeWasActive )
                        {
#if (PANEL_3_5_ZONE)
                            latch6.latch6_output_bit.com_service_detector = !latch6.latch6_output_bit.com_service_detector;
#elif (PANEL_10_ZONE)
                            latch9.latch9_output_bit.com_service_detector = !latch9.latch9_output_bit.com_service_detector;
#endif
                        }
                        break;

                    case 5:
                    case 6:
                    case 7:
                        break;

                    case 8:
                        state_2x20 = 0;
                        break;
                }
            }
        }
    }
#endif

    // Disable interrupt while updating latch outputs. Interrupt contains a modification
    // to PORTB when driving NAC SYNCH signals. Can't have PORTB modified when updating NAC SYNCH
    PEIE = 0;

    /* External loop will loop through all latch bits        */
    /* Internal loop will loop through all available latches */
    for ( outputLatchBit = 0; outputLatchBit < MAX_BITS; outputLatchBit++ )
    {
        /* clear lower 3 bits of PORTB (address inputs to latch) */
        PORTB &= CLEAR_OUTPUT_LATCH;
        PORTB |= outputLatchBit;

        /* create bit mask flag */
        latchBit = 1 << outputLatchBit;

        /* Loop through each latch for each bit */
        for ( latchAddress = 0; latchAddress < MAX_LATCH_HW; latchAddress++ )
        {
            /* default value to output is 0 */
            DATA_SET = 0;

            /* process each latch seperately */
            switch ( latchAddress )
            {
                /* Latch 1 controls:   */
                case LATCH_1 :

                    /* if data 1, set data latch 1 */
                    if (latch1.latch1_output_byte & latchBit)
                    {
                        DATA_SET = 1;
                    }

                    //check if COM_TRBL,POWER,BAT BO (bits 1, 3 and 5) are currently being set
                    if ( latchBit & 0b00101010 )
                    {
                        //if so, invert value because for these 1=off and 0=on
                        DATA_SET = !DATA_SET;
                    }
#if (PANEL_3_5_ZONE)
                    /* strobe the latch */
                    LATCH1_STROBE = 1;
                    LATCH1_STROBE = 0;
#endif
                    break;

                case LATCH_2 :

                    if (latch2.latch2_output_byte & latchBit)
                    {
                        DATA_SET = 1;
                    }

                    //check if BUZZER (bit 7) is currently being set
                    if ( latchBit & 0b10000000 )
                    {
                        //if so, invert value because 1=off and 0=on
                        DATA_SET = !DATA_SET;
                    }
#if (PANEL_3_5_ZONE)
                    /* strobe the latch */
                    LATCH2_STROBE = 1;
                    LATCH2_STROBE = 0;
#endif
                    break;

                case LATCH_3 :

                    if (latch3.latch3_output_byte & latchBit)
                    {
                        DATA_SET = 1;
                    }
#if (PANEL_3_5_ZONE)
                    /* strobe the latch */
                    LATCH3_STROBE = 1;
                    LATCH3_STROBE = 0;
#endif
                    break;

                case LATCH_4 :

                    if (latch4.latch4_output_byte & latchBit)
                    {
                        DATA_SET = 1;
                    }
                    //check if TROUBLE RELAY or SMOKE RESET (bits 0 and 7) are currently being set
                    if ( latchBit & 0b10000001 )
                    {
                        //if so, invert value because relay is normally powered ON in normal condition
                        DATA_SET = !DATA_SET;
                    }
#if (PANEL_3_5_ZONE)
                    /* strobe the latch */
                    LATCH4_STROBE = 1;
                    LATCH4_STROBE = 0;
#endif
                    break;

                case LATCH_5 :

                    if (latch5.latch5_output_byte & latchBit)
                    {
                        DATA_SET = 1;
                    }
#if (PANEL_3_5_ZONE)
                    /* strobe the latch */
                    LATCH5_STROBE = 1;
                    LATCH5_STROBE = 0;
#endif
                    break;

                case LATCH_6 :
                    if ( latch6.latch6_output_byte & latchBit )
                    {
                        DATA_SET = 1;
                    }
#if (PANEL_3_5_ZONE)
                    /* strobe the latch */
                    RB6 = 1;
                    RB6 = 0;
#endif
                    break;
#if (PANEL_10_ZONE)
                case LATCH_7 :
                    if (latch7.latch7_output_byte & latchBit)
                    {
                        DATA_SET = 1;
                    }
                    break;

                case LATCH_8 :
                    if (latch8.latch8_output_byte & latchBit)
                    {
                        DATA_SET = 1;
                    }
                    break;
#endif
            }

#if (PANEL_10_ZONE)
            /* select latch's address */
            PORTB &= CLEAR_ADDRESS_LATCH;
            PORTB |= (latchAddress << 4);

            /* make data sure ready for the strobe */
            ADDRESS_DATA_SET = 1;

            /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
            ADDRESS_LATCH_STROBE = 1;
            ADDRESS_LATCH_STROBE = 0;

            /* clear clear the data */
            ADDRESS_DATA_SET = 0;

            /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
            ADDRESS_LATCH_STROBE = 1;
            ADDRESS_LATCH_STROBE = 0;
#endif
        }
    }

#if (PANEL_10_ZONE)
    // Process "Latch 9" special. Tis only a software construct....
    NAC1_2_CLASS_A          = !latch9.latch9_output_bit.nac_classA_1st;
    NAC3_4_CLASS_A          = !latch9.latch9_output_bit.nac_classA_2nd;
    COMMON_WATERFLOW        =  latch9.latch9_output_bit.com_waterflow;
    COMMON_SERVICE_DETECTOR =  latch9.latch9_output_bit.com_service_detector;
    AUX_POWER_CONSTANT      =  latch9.latch9_output_bit.aux_power_constant;
    REMOTE_DISCONNECT       =  latch9.latch9_output_bit.rem_disc;
#endif

    // re-enable interrupt
    PEIE = 1;
}

/**************************************************************************
 ** FUNCTION: ProcessSpecials                                            **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles all processing of special functionality. This  **
 ** includes battery charging, AC loss, ground fault, aux power, etc.    **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ProcessSpecials(void)
{
    static near unsigned char batteryState = BATTERY_SUPERVISE_OFF;
    static unsigned char enableBatterySupevision = 0;
    static unsigned char groundFaultCount = 0, groundFaultRestoreCount = 0;
    static unsigned char noBatteryCount = 0;
    static unsigned int  temp_batt_ad_int_value = 5;
    static unsigned char clearSynchTmr = 0;
    static unsigned char auxOCDebounce = 0;
    static unsigned char auxCheckState = AUX_CHECK_STATE_START;
    static unsigned char batteryFail = 0, diff = 1, stableCount = 0, brownCount = 0;

    unsigned int brownOutVoltage, acRestored;

#if (BATT_SHED_CODE)
    unsigned char index;
#endif
#if (CHAR_COMPARE)
    unsigned char batt_charH, batt_charL;
    unsigned char temp_charH, temp_charL;
#endif

//#if (BATT_TEST)
#if (0)
    static unsigned char hundreds, tens, ones, delim = '-', count=20;
    static unsigned int batt_value = 0;

    /* see if DACT programmed as installed (DACT is recognized and logged in E2 when entering PROGRAM MODE) */
    if ( !(system_state.system_state_byte & 0b00001100) )
    {
        if ( dactPresent && (!auxTimerOC || auxTimerOC == NOT_RUNNING) )
        {
            auxTimerOC = 1;

    //        batt_value = idc_ad_int_value[IDC_1];
    //        batt_value = nac_ad_int_value[NAC_1];
    //        batt_value = temp_batt_ad_int_value;
    //        batt_value = aux_power_ad_int_value;
    //        batt_value = ac_ad_int_value;
    //        batt_value = cleanMePulseValid[IDC_5];
            hundreds = batt_value / 100;
            batt_value -= (hundreds * 100);
            tens = batt_value / 10;
            ones = batt_value - (tens * 10);

            /* update array with command and data for displaying battery a/d */
            var_dact_data[0] = 10;
            var_dact_data[1] = DACT_LCD_Text;
            var_dact_data[2] = delim;
            var_dact_data[3] = hundreds + '0';
            var_dact_data[4] = tens + '0';
            var_dact_data[5] = ones + '0';

    //        batt_value = idc_ad_int_value[IDC_2];
    //        batt_value = nac_ad_int_value[NAC_2];
    //        batt_value = batt_ad_int_value;
    //        batt_value = aux_power_ad_int_value;
    //        batt_value = cleanMePulseDetect[IDC_5];
    //        batt_value = cleanMeValidationTimer[IDC_5];
    //        batt_value = cleanMeAverageNormal[IDC_5];
            hundreds = batt_value / 100;
            batt_value -= (hundreds * 100);
            tens = batt_value / 10;
            ones = batt_value - (tens * 10);

            /* update array with command and data for displahing battery a/d */
            var_dact_data[6] = ',';
            var_dact_data[7] = hundreds + '0';
            var_dact_data[8] = tens + '0';
            var_dact_data[9] = ones + '0';

            /* toggle delimiter every 20 "displays", so you know when new info is being displayed */
//            count--;
//            if ( !count )
//            {
                if ( delim == '-' )
                {
                    delim = '*';
                }
                else
                {
                    delim = '-';
                }
//                count=20;
//            }

            /* send information to DACT */
            dialer_comm(WRITE_TO_DIALER, var_dact_data);
        }
    }
#endif



    /*********************************************************************/
    /* AC Power                                                          */
    /*********************************************************************/
//EOC JCC 10/27/2010: The requirement here is that the NAC voltage has to
// be greater than 20.4 Vrms ( = sqrt(DC Voltage ^ 2 + AC Voltage ^ 2)).
// 20.4 Vrms is what the customers use to calculate their wire lenghts and
// the number of devices they can put on a NAC, see technical manual on 
// Appendix A: System Calculations. 
// brownOutVoltage A/D values correspond to different outlet Vac, and
// it is dependent on the load of the system. acRestored corresponds to 
// 99 Vac because when we detect a brownout, we switch to the batteries and
// the transformer is not loaded anymore, the batteries are the ones that 
// are loaded.

	brownOutVoltage = 515; 	// Depends on the load of the system.
    acRestored = 590; 		// 99 Vac.

	//If the charger is on, the current consumption increases and the voltage
	// decreases, so we have to adjust the A/D values accordantly.
	if (latch2.latch2_output_bit.charger)
	{
		brownOutVoltage -= 25;
	}
//EOC JCC 10/27/2010
    /* If not in programming mode or in reset */
    if ( !(system_state.system_state_byte & 0b00001100) )
    {
        /* main_battery is ON, no charger supervision */
        if ( latch2.latch2_output_bit.main_battery )
        {
            //turn AC POWER LED off
            latch1.latch1_output_bit.ac_power = 0;

            /* if power loss not debounced yet */
            if ( !trouble.trouble_bit.ac_loss && ac_ad_int_value < acRestored )
            {
                /* start timer if not running */
                if ( powerLossDebounce == NOT_RUNNING )
                {
                    powerLossDebounce = 6;
                }
                /* upon timeout, set trouble bit */
                else if ( !powerLossDebounce )
                {
                    //set AC LOSS TROUBLE flag
                    localACLoss = 1;
                    trouble.trouble_bit.ac_loss = localACLoss | remoteACLoss;
                    /* stop timer */
                    powerLossDebounce = NOT_RUNNING;
                }
            }
        }
        /* main battery NOT turned on */
        else
        {
            //turn AC POWER LED on
            latch1.latch1_output_bit.ac_power = !trouble.trouble_bit.ac_loss;
        }
    }

    //is AC power present?
    if ( ac_ad_int_value > acRestored )
    {

#if (AC_FAIL_TEST_PIN)
        latch6.latch6_output_bit.unused = 0;
#endif
        acFailing  = 0;

        /* turn on logic battery backup with AC power */
        latch1.latch1_output_bit.logic_battery_back_up = 0;
        batteryKill = 0;
        brownCount = 0;

        /* if power restore not debounced yet (battery still ON) */
        if ( latch2.latch2_output_bit.main_battery )
        {
            /* start timer if not running */
            if ( powerRestoreDebounce == NOT_RUNNING )
            {
                powerRestoreDebounce = 6;
            }
            /* upon timeout, turn off battery */
            else if ( !powerRestoreDebounce )
            {
                //turn battery off
                latch2.latch2_output_bit.main_battery = 0;
                /* stop timer */
                powerRestoreDebounce = NOT_RUNNING;
            }
            return;
        }

        /* stop timers */
        powerLossDebounce = NOT_RUNNING;
        powerRestoreDebounce = NOT_RUNNING;

        clearSynchTmr = 0;

        //clear AC LOSS TROUBLE flag
        localACLoss = 0;
        trouble.trouble_bit.ac_loss = localACLoss | remoteACLoss;

#if (BATT_SHED_CODE)
        /* if NACs were active, tell state machine it can turn on NACs */
        for ( index = NAC_1; index < MAX_NAC; index++ )
        {
            nac_status[index].nac_status_bit.battery_low = 0;
        }
#endif

#if (PANEL_10_ZONE)
        /*********************************************************************/
        /* Second transformer                                                */
        /*********************************************************************/

        /* transformer present and not working */
        if ( transformer2 < 482 )
        {
            /* bad transformer #2 */
            trouble.trouble_bit.transformer2 = 1;
        }
        /* transformer present and working or not present */
        else
        {
            trouble.trouble_bit.transformer2 = 0;
        }
#endif
    }
    //is AC input in BROWNOUT or AC LOSS condition
    else if ( ac_ad_int_value < brownOutVoltage)
	{
                brownCount++;

#if (AC_FAIL_TEST_PIN)
                latch6.latch6_output_bit.unused = 1;
#endif
                acFailing = 1;

		if (brownCount > BROWNOUT_DEBOUNCE )
    	{
	        /* latch brownout debounce counter */
	        brownCount = BROWNOUT_DEBOUNCE;
	
	        /* turn battery on (battery supplies power) */
	        latch2.latch2_output_bit.main_battery = 1;
	
	        /* turn off logic battery backup without AC power */
	        latch1.latch1_output_bit.logic_battery_back_up = 1;
	
	        //is battery really low (or missing) ?
	        if ( batt_ad_int_value <= BATTERY_REALLY_LOW || batteryKill )
	        {
	            //turn off battery power (battery killer)
	            latch2.latch2_output_bit.main_battery = 0;
	            batteryKill = 1;
	        }
	
	        /* if Genesis devices are connected, resend synch pulse */
	        if ( !clearSynchTmr )
	        {
	            reSynchGen = 1;
	            clearSynchTmr = 1;
	        }
	
#if (BATT_SHED_CODE)
	        //is battery sufficiently low to shed loads
	        if ( batt_ad_int_value <= BATTERY_SHED_LOADS )
	        {
	            /* aux power is turned off */
	            latch3.latch3_output_bit.aux_power = 0;
	
	            /* make sure it doesn't get turned on again */
	            auxTimerOC = NOT_RUNNING;
	
	            /* if NAC active, tell state machine to turn off NACs */
	            for ( index=NAC_1; index < MAX_NAC; index++ )
	            {
	                nac_status[index].nac_status_bit.battery_low = 1;
	            }
	        }
#endif
		}	

    }
    else
    {
        /* This is the case between brownout and AC restoral - if we're
         * in this state then brownCount gets reset to 0 which essentally
         * keeps restarting the 200 msec brownout delay to AC transfer.
         *
         * This will effectively STOP the transfer to battery as long as
         * the voltage remains above the brownout level but below the 
         * restoral level. The NAC overcurrent detection will still work 
         * because we have not flipped over to battery operation and we
         * have cleared the acFailing status.
         */

        powerRestoreDebounce = NOT_RUNNING;
        brownCount = 0;

#if (AC_FAIL_TEST_PIN)
        latch6.latch6_output_bit.unused = 0;
#endif
        acFailing = 0;
    }

    /*********************************************************************/
    /* Battery                                                           */
    /*********************************************************************/

    /* update threshold values according to battery voltage */
    /* in case main battery has been turned ON              */
    load_thresh_levels();

    // Have we supervised charger at least once?
    if ( enableBatterySupevision )
    {
        /* is battery missing or fuse blown? */
        if ( batt_ad_int_value < BATTERY_REALLY_LOW )
        {
            /* signal debounced? */
            noBatteryCount++;
            if ( noBatteryCount > 50 )
            {
                /* set battery not present trouble */
                trouble.trouble_bit.batt_placement = 1;
            }
        }
        else
        {
            // Make missing battery restore to BATTERY_LOW_RESTORE level
            // before clearing.
            if ( !trouble.trouble_bit.battery )
            {
                /* clear no battery trouble */
                trouble.trouble_bit.batt_placement = 0;

                /* clear counts */
                noBatteryCount = 0;
            }
        }

        /* only check for battery low if charger OFF */
        if ( !latch2.latch2_output_bit.charger )
        {
            /* is battery moderately low? (below 21.4 v) */
            if ( batt_ad_int_value < BATTERY_LOW_SET )
            {
                //set BATTERY TROUBLE flag
                trouble.trouble_bit.battery = 1;
            }
            /* has battery recharged enough? (above 22.4 v) */
            else if ( batt_ad_int_value > BATTERY_LOW_RESTORE )
            {
                //clear BATTERY TROUBLE flag
                trouble.trouble_bit.battery = 0;
            }
        }
    }
    else
    {
        if ( batteryState == BATTERY_SUPERVISE_CHECK )
        {
            enableBatterySupevision = 1;
        }
    }

    /*********************************************************************/
    /* Battery Charging and Charger supervision                          */
    /*********************************************************************/

    /* main_battery is OFF AND if some AC power present */
    if ( !latch2.latch2_output_bit.main_battery && !(trouble.trouble_byte & 0b00010000)
    /* AND not in programming mode or reset */
        && !(system_state.system_state_byte & 0b00001100)
    /* AND NACs are NOT active */
#if (PANEL_10_ZONE)
        && !( (latch3.latch3_output_byte & NAC_1_2) || (latch7.latch7_output_byte & NAC_3_4) ) )
#elif (PANEL_3_5_ZONE)
        && !( latch3.latch3_output_byte & NAC_1_2 ) )
#endif
    {

        /* handle current battery status */
        switch ( batteryState )
        {
            /* Supervise charger by turning off charger for 70 seconds */
            case BATTERY_SUPERVISE_OFF :
                /* turn battery charger off */
                /* if noise differential has not been reduced, normal check */
                if ( diff )
                {
                    latch2.latch2_output_bit.charger = 0;
                }

                /* wait 25 seconds for charge to bleed off */
                battery_check_timer = 25;

                /* got to next state */
                batteryState = BATTERY_SUPERVISE_ON;
                break;

            /* Supervise charger by turning on charger for 10 seconds and then checking value */
            case BATTERY_SUPERVISE_ON :
 #if (BATT_CHARGE)
                latch4.latch4_output_bit.idc1_sup = !latch4.latch4_output_bit.idc1_sup;
                latch4.latch4_output_bit.idc2_sup = 0;
                latch4.latch4_output_bit.idc2_trbl = 0;
 #endif
                /* if timeout */
                if ( !battery_check_timer )
                {
                    /* save current battery value for comparison, if noise differential has not been reduced */
                    if ( diff )
                    {
                        temp_batt_ad_int_value = batt_ad_int_value;
                    }

                    /* turn battery charger on */
                    latch2.latch2_output_bit.charger = 1;

                    /* turn charger on for 5 seconds and check A/D value */
                    battery_check_timer = 5;

                    /* got to next state */
                    batteryState = BATTERY_SUPERVISE_CHECK;
                }
                break;

            case BATTERY_SUPERVISE_CHECK :
 #if (BATT_CHARGE)
                latch4.latch4_output_bit.idc2_sup = !latch4.latch4_output_bit.idc2_sup;
                latch4.latch4_output_bit.idc1_sup = 0;
 #endif
                /* if timeout */
                if ( !battery_check_timer )
                {
                    /* turn battery charger off to get "good" reading of battery voltage */
                    if ( diff )
                    {
                        latch2.latch2_output_bit.charger = 0;
                    }

                    /* if temp value (charger OFF) is less than current value (charger ON), battery is charging */
//                    if ( ( temp_batt_ad_int_value + diff ) < batt_ad_int_value )
                    if ( ( ( temp_batt_ad_int_value + diff ) < batt_ad_int_value ) || ( batt_ad_int_value >= BATTERY_CHARGED ) )
                    {
                        /* clear trouble, if in normal check, or if in reduced differential check, must be stable */
                        if ( diff || ++stableCount > STABLE_FAIL_DEBOUNCE )
                        {
                            trouble.trouble_bit.charger = 0;
                            batteryFail = 0;
                            stableCount = 0;
                            diff        = NOISE_DIFFERENTIAL;
                        }
                    }
                    /* battery is NOT charging */
                    else
                    {
                        /* reduce noise differential and look for continued failure */
                        if ( diff )
                        {
                            diff = 0;
                            stableCount = 0;
                            batteryFail = 0;
                        }
                        else
                        {
                            if ( ++batteryFail > STABLE_FAIL_DEBOUNCE )
                            {
                                /* set trouble */
                                trouble.trouble_bit.charger = 1;
                                stableCount = 0;
                                batteryFail = 0;
                                diff        = NOISE_DIFFERENTIAL;
                            }
                        }
                    }
                    /* Start charging process (in case battery discharged a little) */
                    batteryState = BATTERY_CHARGE;

                    /* check for A/D value DROP in 10 seconds */
                    battery_check_timer = 10;
                }
                break;

            /* Battery below charged voltage, charger on for 80 secs */
            case BATTERY_CHARGE :
                /* if timeout */
                if ( !battery_check_timer )
                {
                    /* save current battery value for comparison, if noise differential has not been reduced */
                    if ( diff )
                    {
                        temp_batt_ad_int_value = batt_ad_int_value;
                    }

                    /* if battery not fully charged (less than BATTERY_CHARGED volts) */
                    if ( batt_ad_int_value < BATTERY_CHARGED )
                    {
                        /* turn battery charger on */
                        latch2.latch2_output_bit.charger = 1;

                        /* turn on charger and check A/D value in 75 seconds */
                        battery_check_timer = 75;

                        /* go to next state */
                        batteryState = BATTERY_CHARGE_OFF;
                    }
                    /* if battery fully charged, restart process */
                    else
                    {
                        batteryState = BATTERY_SUPERVISE_OFF;
                    }
                }
                break;

            /* After 75 second charge turn off charger for 5 seconds */
            case BATTERY_CHARGE_OFF :
 #if (BATT_CHARGE)
                latch4.latch4_output_bit.idc1_trbl = !latch4.latch4_output_bit.idc1_trbl;
                latch4.latch4_output_bit.idc2_trbl = 0;
 #endif
                /* if timeout */
                if ( !battery_check_timer )
                {
                    /* turn battery charger off to get "good" reading of battery voltage */
                    if ( diff )
                    {
                        latch2.latch2_output_bit.charger = 0;
                    }

                    /* if temp value (charger OFF) is less than current value (charger ON), battery is charging */
                    if ( ( temp_batt_ad_int_value + diff ) < batt_ad_int_value )
                    {
                        /* clear trouble, if in normal check, or if in reduced differential check, must be stable */
                        if ( diff || ++stableCount > STABLE_FAIL_DEBOUNCE )
                        {
                            trouble.trouble_bit.charger = 0;
                            batteryFail = 0;
                            stableCount = 0;
                            diff        = NOISE_DIFFERENTIAL;
                        }
                    }
                    /* battery is NOT charging */
                    else
                    {
                        /* reduce noise differential and look for continued failure */
                        if ( diff )
                        {
                            diff = 0;
                            stableCount = 0;
                            batteryFail = 0;
                        }
                        else
                        {
                            if ( ++batteryFail > STABLE_FAIL_DEBOUNCE )
                            {
                                /* set trouble */
                                trouble.trouble_bit.charger = 1;
                                stableCount = 0;
                                batteryFail = 0;
                                diff        = NOISE_DIFFERENTIAL;
                            }
                        }
                    }

                    /* go back to charging state */
                    batteryState = BATTERY_CHARGE;

                    /* check for A/D value DROP in 10 seconds */
                    battery_check_timer = 10;
                }
                break;
        }
    }
    /* if can't supervise/charge battery, turn off charger */
    else
    {
        /* make sure battery charger off */
        latch2.latch2_output_bit.charger = 0;

        /* Restart battery charging state machine, if can't charge */
        batteryState = BATTERY_SUPERVISE_OFF;
    }

    /*********************************************************************/
    /* Aux Power                                                         */
    /*********************************************************************/

    /* if aux power not ON */
    if ( !latch3.latch3_output_bit.aux_power
#if (PANEL_3_5_ZONE)
            || !latch6.latch6_output_bit.aux_power_constant
#elif (PANEL_10_ZONE)
            || !latch9.latch9_output_bit.aux_power_constant
#endif
       )
    {
        /* if OC timer not running, and no Aux power, turn on in 2 seconds */
        if ( auxTimerOC == NOT_RUNNING )
        {
            /* in seconds */
            auxTimerOC = 2;
        }
        /* if OC timer timeout, turn on aux power */
        else if ( !auxTimerOC )
        {
            auxTimerOC = NOT_RUNNING;
// BOC CSM 2/26/2007
#if (PANEL_3_5_ZONE)
            if ( !latch6.latch6_output_bit.aux_power_constant )
#elif (PANEL_10_ZONE)
            if ( !latch9.latch9_output_bit.aux_power_constant )
#endif
            {
                /* refresh peripheral status */
                ClearPeripheralStatus(REFRESH);
            }
// EOC CSM 2/26/2007

            //turn AUX POWER on
            latch3.latch3_output_bit.aux_power = 1;
#if (PANEL_3_5_ZONE)
            latch6.latch6_output_bit.aux_power_constant = 1;
#elif (PANEL_10_ZONE)
            latch9.latch9_output_bit.aux_power_constant = 1;
#endif
            //clear aux OC trouble
            trouble2.trouble_bit2.auxOC = 0;

// BOC CSM 2/26/2007
//            /* refresh peripheral status */
//            ClearPeripheralStatus(REFRESH);
// EOC CSM 2/26/2007

            /* reset aux check state */
            auxCheckState = AUX_CHECK_STATE_START;
            auxOCDebounce = 0;
        }
    }
    /* check for overcurrent on Aux Power */
    if ( ( latch3.latch3_output_bit.aux_power
#if (PANEL_3_5_ZONE)
            || latch6.latch6_output_bit.aux_power_constant
#elif (PANEL_10_ZONE)
            || latch9.latch9_output_bit.aux_power_constant
#endif
         )
         && aux_power_ad_int_value < auxPwrOC )
    {
        /* if OC debounced (in main loop counts), turn off aux power */
#if (PANEL_10_ZONE)
        /* 10 zone main loop ~10ms = 150ms debounce*/
        if ( auxOCDebounce > 15 )
#elif (PANEL_3_5_ZONE)
        /* 3 zone main loop ~3ms = 90ms */
        /* 5 zone main loop ~5ms = 150ms */
        if ( auxOCDebounce > 30 )
#endif
        {
            //annunciate trouble
            trouble2.trouble_bit2.auxOC = 1;
            /* default to check in 60 seconds */
            auxTimerOC = 60;
            switch ( auxCheckState )
            {
                case AUX_CHECK_STATE_START:
                    //turn resettable AUX POWER off
                    latch3.latch3_output_bit.aux_power = 0;
                    //check constant AUX
                    auxCheckState = AUX_CHECK_STATE_AUX_CONSTANT;
                    break;
                case AUX_CHECK_STATE_AUX_CONSTANT:
                    //we turned off resettable aux and left constant aux on and we're back in trouble.
                    //we know constant aux is in trouble. let's see if resettable aux in trouble too!!
                    //turn constant AUX POWER off
#if (PANEL_3_5_ZONE)
                    latch6.latch6_output_bit.aux_power_constant = 0;
#elif (PANEL_10_ZONE)
                    latch9.latch9_output_bit.aux_power_constant = 0;
#endif
                    //turn resettable AUX POWER on
                    latch3.latch3_output_bit.aux_power = 1;
                    //check resettable AUX
                    auxCheckState = AUX_CHECK_STATE_AUX_RESETTABLE;
                    break;
                case AUX_CHECK_STATE_AUX_RESETTABLE:
                    //we turned on resettable aux and turned constant aux off and we're back in trouble.
                    //we now know resettable aux is in trouble too!!.
                    //turn resettable AUX POWER off
                    latch3.latch3_output_bit.aux_power = 0;
                    //done checking aux, look for restoral
                    break;
            }
            auxOCDebounce = 0;
        }
        else
        {
            /* increment debounce timer */
            auxOCDebounce++;
        }
    }
    /* aux_power_ad_int_value >= 425 */
    else
    {
        /* reset aux check state */
        auxCheckState = AUX_CHECK_STATE_START;
        auxOCDebounce = 0;
    }

    /*********************************************************************/
    /* Ground Fault                                                      */
    /*********************************************************************/

    /* is ground fault value out of normal range */
    if ( ground_ad_int_value < ground_fault[LOW] || ground_ad_int_value > ground_fault[HIGH] )
    {
        groundFaultCount++;
        if ( groundFaultCount > GF_COUNT )
        {
            groundFaultCount = GF_COUNT;
            groundFaultRestoreCount = 0;
            trouble.trouble_bit.gnd_fault = 1;
        }
    }
    else
    {
        groundFaultRestoreCount++;
        if ( groundFaultRestoreCount > GF_COUNT )
        {
            groundFaultCount = 0;
            groundFaultRestoreCount = GF_COUNT;
            trouble.trouble_bit.gnd_fault = 0;
        }
    }
}
