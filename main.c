/*
+------------------------------------------------------------------------------
| Description:
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
| 11/21/2016 M. Desai        Version 4.00.00
| 03/10/2016 R. Hempel       Version 3.12.00
| 03/04/2016 R. Hempel       Version 3.11.54
| 02/17/2016 R. Hempel       Version 3.11.51
| 11/22/2010 J. Castellanos  Version 3.10.00.
| 10/27/2010 J. Castellanos  Fixed brownout issue. Version 3.00.01.
| 6/18/2007  C. McGill       Promoted version 02.10.15 to 03.00.00 (3.0) for release.
| 2/26/2007  C. McGill       In ProcessWalkTest(), modified to debounce zone restore
|                            in support of walk test restore of Class A open state
|                            wherein to auto restore the zone, have to clear the trouble
|                            and restore the relay. If the open has not been fixed, it will
|                            cycle back to trouble state. PTR1149.
|                            Version 2.10.15.
| 11/27/2006 C. McGill       Version 2.10.14. Modified to send service detector
|                            and remote disconnect status.
| 11/1/2006  C. McGill       Version 2.10.13.
| 05/11/06  C. McGill        Version 2.10.12.
| 12/15/05  C. McGill        Version 2.10.11.
|                            In UpdatePeripheralState(), corrected to translate
|                            IDC status bits to peripheral status bits. Was sending
|                            IDC status direct and the bit for waterflow was being
|                            translated to trouble when sent to peripherals. PTR 1504.
| 12/08/05  C. McGill        Version 2.10.10.
|                            Added OMCoderClearCode(). Modifications to support
|                            clearing of coder entry for restoration of non-latching
|                            IDC. PTR 1503.
| 11/28/05  C. McGill        Version 2.10.09
| 11/17/05  C. McGill        Version 2.10.08
| 11/16/05  C. McGill        Version 2.10.07
| 11/14/05  C. McGill        Version 2.10.06
| 11/14/05  C. McGill        In ClearPeripheralStatus(), added SP_MONITOR as this
|                            state is now transmitted to the RA10(C).
| 10/24/05  C. McGill        Version 2.10.05
| 10/18/05   C. McGill       Removed change marks, commented-out code.
|                            Version 2.10.04
| 9/26/2005  C. McGill       Version 2.10.03
| 8/24/2005  C. McGill       Version 2.10.02
| 8/15/2005  C. McGill       Version 2.10.01
| 8/11/2005  C. McGill       Added clean me timers to interrupt.
| 8/10/2005  C. McGill       Modified overlay test code to include 10 zone LEDs.
|                            added initialization of software latch 9 for 10 zone.
| 8/9/2005   C. McGill       Modifications for non-latching IDCs. Modified TRISC
|                            initialization for PIC18F2525.
| 09/15/03  C. McGill        Promoted version 02.00.02 to 02.10.00 for release.
| 06/04/03  C. McGill        Version 02.00.02.
|                             In tc_int(), modified use of nacSynchSecs to provide
|                             for Genesis synch of 119.4s. Current method does not
|                             provide proper synch and results in visible
|                             "double flash" with new multi-candela horn/strobes.
| 04/16/03  C. McGill        Version 02.00.01.
|                             Reversed History w/most recent change first. Removed
|                             old edit marks.
| 09/03/02  C. McGill        In tc_int() and UpdateDact(), added processing
|                             of new global comboDisableDelaySecs[]. Used in
|                             processing combo zone disable after supv side
|                             activation. PTR #893. In main(), corrected to
|                             initialize dactPresent from EEPROM.
| 08/30/02  C. McGill        Promoted to version 02.00.00
| 08/29/02  C. McGill        Added global dactPresent for use in determining
|                            whether to attempt dact comm and to reduce i2c
|                            traffic.
| 08/28/02  C. McGill        Version 01.00.10.
| 08/23/02  C. McGill        Version 01.00.09.
| 08/17/02  C. McGill        In main(), moved call to ProcessWalkTest() to before
|                             call to input_tripped_handler(). PTR #860.
|                             Version 01.00.08.
| 08/16/02  C. McGill        In ProcessWalkTest(), modified to prevent deselecting
|                             zone while zone in walktest reset. PTR #854.
| 08/14/02  C. McGill        In ProcessWalkTest(), modified to check ALL zones
|                             for active state before performing walk test setup
|                             on the zones. PTR #850.
| 08/10/02  C. McGill        In UpdatePeripheralState(), modified to ensure previous
|                             state sent to all peripherals before accepting new
|                             new state change. In quick state transitions, change
|                             would be accepted before all peripherals were updated
|                             resulting in lower addressed peripherals not receiving
|                             new state change. PTR #768. Version 01.00.07.
| 08/04/02  C. McGill        In ProcessWalkTest(), modified to include trouble
|                             and monitor to active point check ( walktest exclusion ).
|                             This makes the operation consistent, thus requiring
|                             a "green" panel to start walktest. Ideally, walktest
|                             would exclude any pre-disabled zones, but the current
|                             function is so pervasive in the code, it could not
|                             be released by 8/15/02. PTR #804.
| 07/30/02  C. McGill        In UpdateDACT(), modified to force system buzzer resound
|                             on report of DACT trouble. DACT sends indication on
|                             new trouble activation. PTR #779. In UpdateDACT(),
|                             corrected to exclude only IDC that is in walktest
|                             from DACT reporting during walktest. PTR #784.
|                             Version 1.00.06.
| 07/15/02  C. McGill        In UpdateDACT(), corrected to not send a newly enabled
|                             active point during the enable grace period.
| 07/14/02  C. McGill        Replaced watchdog process with new function WatchDog();
|                             Version 1.00.05.
| 06/30/02  C. McGill        Version 1.00.04.
| 06/16/02  C. McGill        In UpdateDACT(), modified to send panel (trouble)
|                             silence event regardless if panel in trouble state.
|                             Now panel silence functionality. In main(), allowed
|                             activation of buzzer_on_flag to invoke trouble_handler()
|                             to process if alarm or sup.
| 06/12/02  C. McGill        Removed use of internal watchdog circuit. Overkill
|                            to use both internal and external watchdog.
|                            In ClearPeripheralStatus(), modified to refresh
|                             all system level basetypes: Alarm, Sup & Trouble.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   main.c  $
|     $Date:   Nov 22 2010 09:16:34  $
|  $Modtime:   Nov 22 2010 09:15:20  $
| $Revision:   1.208  $
|  $Archive:   \\BRAFLAPP02SECGE\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\main.c-arc  $
|   $Author:   Julio Castellanos  $
+------------------------------------------------------------------------------
*/

#include "config.h"
#include <pic18.h>
#include <string.h>
#include "defines.h"
#include "main.h"
#include "usart_m.h"
#include "externs.h"
#include "i2c_mst.h"
#include "io.h"
#include "tripped.h"
#include "progrm.h"

#if (PANEL_10_ZONE)
#define WALK_TEST_FLASH_RATE    2
#elif (PANEL_3_5_ZONE)
#define WALK_TEST_FLASH_RATE    5
#endif

/*************************************************************************/
/*** CONFIGURATION FUSE SETTINGS FOR PIC18C252                           */
/*************************************************************************/
#if (!PROC_18F252)
__CONFIG(1, UNPROTECT & IESODIS & XT);
__CONFIG(2, PWRTEN & BORV21 & BORDIS & WDTDIS);
__CONFIG(3, CCP2RC1 & 0xFDFF);   // OxFDFF disables Port B as analog inputs and sets as digital I/O. Thus matching F252. Tricky!!!!
__CONFIG(4, STVREN & XINSTDIS & LVPDIS);
#else
__CONFIG(1, UNPROTECT & OSCSDIS & XT);
__CONFIG(2, PWRTEN & BORV25 & BORDIS & WDTDIS);
__CONFIG(3, CCP2RC1);
__CONFIG(4, STVREN & LVPDIS);
#endif

/* code revision format 04.00.00 */
__IDLOC(00040000);
const unsigned char rev_number[] = { 0x04, 0x00, 0x00 };

//=== Global Variable Declarations (extern statement in file: "externs.h") ========
#if (PANEL_3_5_ZONE)
unsigned char five_zone_panel = 0;
#endif

volatile bit continuous_nac1 = 0;
volatile bit continuous_nac2 = 0;
volatile bit temporal_nac1 = 0;
volatile bit temporal_nac2 = 0;
volatile bit coder_nac1 = 0;
volatile bit coder_nac2 = 0;
volatile bit genesis_synch_nac1 = 0;
volatile bit genesis_synch_nac2 = 0;
volatile bit genesis_silence_nac1 = 0;
volatile bit genesis_silence_nac2 = 0;
volatile unsigned char genesis_turn_on_nac1;
volatile unsigned char genesis_turn_on_nac2;
volatile unsigned char ignoreNacOC = 0;

#if (PANEL_10_ZONE)
volatile bit continuous_nac3 = 0;
volatile bit continuous_nac4 = 0;
volatile bit temporal_nac3 = 0;
volatile bit temporal_nac4 = 0;
volatile bit coder_nac3 = 0;
volatile bit coder_nac4 = 0;
volatile bit genesis_synch_nac3 = 0;
volatile bit genesis_synch_nac4 = 0;
volatile bit genesis_silence_nac3 = 0;
volatile bit genesis_silence_nac4 = 0;
volatile unsigned char genesis_turn_on_nac3;
volatile unsigned char genesis_turn_on_nac4;

volatile bit nac3_synch_status;
volatile bit nac4_synch_status;
#endif

union SYSTEM_STATE_BIT_BYTE system_state;
union PERIPHERAL_STATE_BIT_BYTE peripheral_state;
union PERIPHERAL_STATE2_BIT_BYTE peripheral_state2;
unsigned char previousStatus;
unsigned char previousStatus2;
unsigned char currentStatus;
unsigned char currentStatus2;
unsigned char UpdateLocked;
unsigned char UpdateRequired;
unsigned char refreshStatus = 0;
unsigned char dactPresent = 0;
unsigned char rrmCommonAlarm = 0;
static unsigned char nacSynchSecs = 0;
volatile unsigned char reSynchGen = 0;
volatile unsigned char powerUpSecs;
volatile unsigned char walkTestInactiveMins;
volatile unsigned int  idcWalkTestTmr[MAX_IDC];
unsigned char idcWalkTestCnt[MAX_IDC];
unsigned char idcWalkTestState[MAX_IDC];

#if ( BUZZER_TEST )
unsigned int buzzerToggleSecs = 300;
unsigned int buzzerPhaseMins = 480;
#endif

void UpdatePeripheralState(void);
void ProcessWalkTest(void);
void ExitWalkTestMode(void);
void UpdateDACT(void);
void DogWatch(void);

__EEPROM_DATA ( 0x13, 0x40, 0x01, 0x01, 0x0f, 0x00, 0x00, 0x01 );
__EEPROM_DATA ( 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00, 0x01 );
__EEPROM_DATA ( 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00, 0x01 );
__EEPROM_DATA ( 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00, 0x01 );
__EEPROM_DATA ( 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00, 0x01 );
__EEPROM_DATA ( 0x0f, 0x00, 0x00, 0x83, 0x83, 0x83, 0x83, 0x00 );
__EEPROM_DATA ( 0x00, 0x13, 0x40, 0x01, 0x01, 0x0f, 0x00, 0x00 );
__EEPROM_DATA ( 0x01, 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00 );
__EEPROM_DATA ( 0x01, 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00 );
__EEPROM_DATA ( 0x01, 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00 );
__EEPROM_DATA ( 0x01, 0x0f, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00 );
__EEPROM_DATA ( 0x01, 0x0f, 0x00, 0x00, 0x83, 0x83, 0x83, 0x83 );
__EEPROM_DATA ( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 );

//=================================================================================

//=== Local (to module) variable declarations =============
//=========================================================
#define TRUE                        1
#define FALSE                       0
#define TIMER_CODER_DIGIT           50          /*  .5s on and off      */
#define TIMER_CODER_INTERDIGIT      150         /* 1.5s between digits  */
#define TIMER_CODER_INTERROUND      600         /* 6.0s between round   */
#define TIMER_CODER_MAXDIGITS       4           /* 4 digits of code     */
#define TIMER_CODER_MAXROUNDS       4           /* 4 rounds of coding   */
#if (PANEL_10_ZONE)
#define TIMER_CODER_MAXCODES        10          /* queue depth 10 codes */
#else
#define TIMER_CODER_MAXCODES        5           /* queue depth 5 codes  */
#endif

unsigned char omTimersCoderDigitCount = 0;
unsigned char omTimersCoderDigitPos   = 0;
unsigned char omTimersCoderRoundCount = 0;
unsigned int  omTimersCoderPulseCount = 0;
unsigned int  omTimersCoderPulseTimer = 0;

/* flag to denote whether nac(s) should be on or off */
unsigned char omCoder = 0;

/* coder queue */
unsigned char omCoderQueue[TIMER_CODER_MAXCODES][TIMER_CODER_MAXDIGITS];
unsigned char omCoderIdcActive[TIMER_CODER_MAXCODES];
unsigned char omCoderQueueReadIndex  = 0;
unsigned char omCoderQueueWriteIndex = 0;

/* flag to denote final inter-round delay */
unsigned char omCoderFinalRoundDelay = 0;

//========= Local (to module) prototypes =============================
static void initializations(void);
static unsigned char OMCoderNextCode( void );

//====================================================================

/**************************************************************************
 ** FUNCTION: Main                                                       **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This is the starting point of the panel. Contains main loop.         **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
main()
{
    unsigned char DACTPollTimer;

    initializations();
    init_usart();

    //
    // Check for DACT Presence
    //
#if (DACT_CODE)
    if ( ext_eeprom_read(EEPROM_DACT_INSTALLED) & DACT_PRESENT )
    {
        dactPresent = 1;
    }
#endif
    //
    // Check Matrix RRM Mode
    //
    if ( ext_eeprom_read(EEPROM_RRM_COMMON_MODE) & RRM_COMMON_ALARM_MODE )
    {
        rrmCommonAlarm = 1;
    }
    //
    // Update Class A IDC Addresses
    //
    UpdatePeriphClassAIDCAddress();
    //
    // Update Non-latching IDCs
    //
    UpdateNonLatchingIDCs();

    //MAIN EXECUTIVE LOOP
    while (1)
    {
        //toggle of external WATCHDOG
        DogWatch();

        if ( !system_state.system_state_bit.reset )
        {
            button_handler();
        }

        //pre-set alarm and trouble_silence flags to 0
        system_state.system_state_byte &= 0b10111110;

        /* if we are currently programming the panel, ignore any tripped inputs */
        if ( programCleanUp || system_state.system_state_bit.program )
        {
            /* "program()" monitors for jumper removal (returns 1 when jumper removed) */
            if ( program() == PRGM_FINISHED )
            {
                /* clear flag enabling PROGRAM state AND put system into reset mode */
                system_state.system_state_byte = 0b00000100;
            }
        }
        /* check for reset button press and/or state and WHILE alarm silence inhibit timer NOT active */
        else if ( (button.button_bit.reset || system_state.system_state_bit.reset) && (!alarm_sil_inhibit_timer) )
        {
            /* process a reset */
            ProcessReset();
        }
        /* we are not programming the panel, not doing a lamp test, normal operation */
        else if ( !button.button_bit.lamp_test )
        {
            /* check for drill state */
            if ( system_state.system_state_bit.drill )
            {
                /* handle drill state */
                ProcessDrill();
            }

            //see if any SYSTEM TROUBLE bits are set
            if ( button.button_bit.trouble_silence
                 || buzzer_on_flag
                 || trouble.trouble_byte != oldTrouble
                 || trouble2.trouble_byte2 != oldTrouble2
                 || dact_trouble.dact_trouble_byte != oldDactTroubs )
            {
                system_state.system_state_bit.trouble = 1;
            }

            /* handle walk test button press and Walk Test mode */
            if ( button.button_bit.walk_test || system_state.system_state_bit.walk_test )
            {
                ProcessWalkTest();
            }

            /* handle any IDC status change */
            input_tripped_handler();

            /* handle any NAC status change */
            nac_input_handler();

            /* if system in trouble and trouble cleared    */
            /* (trouble_handler() returns 1 when finished) */
            if ( system_state.system_state_bit.trouble && trouble_handler() )
            {
                //clear flag enabling TROUBLE HANDLER
                system_state.system_state_bit.trouble = 0;
            }

            //look for TROUBLE SILENCE button press
            if ( button.button_bit.trouble_silence )
            {
                //clear flag
                button.button_bit.trouble_silence = 0;
                peripheral_state.peripheral_state_bit.trouble_silence = 1;

                /* annunciate trouble again in 24 hours */
                troubleSilenceMinutes = TROUBLE_SILENCE_MINUTES;
            }

            /* check for remote disconnect button press */
            if ( button.button_bit.remote_disconnect )
            {
                /* toggle trouble flag */
                trouble2.trouble_bit2.remoteDisconnect = !trouble2.trouble_bit2.remoteDisconnect;

#if (DACT_CODE)
                /* see if DACT programmed as installed (DACT is recognized and logged in E2 when entering PROGRAM MODE) */
                if ( dactPresent )
                {
                    /* if no trouble, DACT has been re-enabled */
                    if ( !trouble2.trouble_bit2.remoteDisconnect )
                    {
                        dialer_comm(WRITE_TO_DIALER, enable_DACT);
                    }
                    /* else if trouble, DACT has been disabled */
                    else
                    {
                        dialer_comm(WRITE_TO_DIALER, disable_DACT);
                    }
                }
#endif

                /* acknowledge button press */
                button.button_bit.remote_disconnect = 0;
            }
        }

        normal_handler();

        output_refresh();
        scan_analog_inputs();

        /* if not currently powering up */
        if ( !powerUpSecs )
        {
#if (DACT_CODE)
            /* see if DACT programmed as installed (DACT is recognized and logged in E2 when entering PROGRAM MODE) */
            if ( dactPresent )
            {
                if(!++DACTPollTimer)    //delay to slow down rate of polls sent to DACT
                {
                    /* Supervise DACT and Send any DELTA STATUS CHANGES to DACT */
                    UpdateDACT();
                }
            }
#endif

            /* process IDCs */
            debounce_idc_levels();
            process_idc_level_status();

            /* process NACs */
            debounce_nac_levels();
            process_nac_level_status();

            /* do not supervise/update peripherals during Reset, programming mode or lamp test */
            if ( !system_state.system_state_bit.reset && !programCleanUp && !system_state.system_state_bit.program )
            {
                /* poll peripherals for presence */
                supervise_peripherals();
                /* if system status change, notify peripherals */
                UpdatePeripheralState();
            }
        }

        /* Check and process aux power, battery, ground fault, etc. */
        ProcessSpecials();

        /* handle any USART communication */
        usart_comm();

    }//end while(1)
}//END main()

/**************************************************************************
 ** FUNCTION: Interrupt                                                  **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This interrupt updates all timers and controls Genesis devices.      **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
#if 0
unsigned long endBuzzerCount  = 0,
      endChargerCount = 0,
      endAuxCount     = 0,
      endNacCount     = 0,
      psSysTicks      = 0,
      psClockTime     = 0;

unsigned char endBuzzerState   = 0,
        endChargerState  = 0,
        endAuxState      = 0,
        endNacState      = 0;
#endif
void interrupt tc_int(void)
{
    volatile static unsigned char tic_10ms_ctr = 100;
    volatile static unsigned char tic_sec_ctr = 60;
    unsigned char idc_ptr;
	volatile static unsigned int synch_sil_state_ctr;

    CCP1IF = 0; //clear interrupt flag that got us here

    //_ We are here every 10ms -
#if 0
    // NAC test 60 cycles per minute for 250000 cycles
    if ( endNacCount <= 250000 )
    {
        latch3.latch3_output_bit.nac1 = 1;
        latch3.latch3_output_bit.nac2 = 1;
        // 60s/60cycles = 1s/cycle => .5s on/off. 1s = 960 ticks => .5s = 480 ticks
        if ( ( psSysTicks % 50 ) == 0 )
        {
            if ( !endNacState )
            {
                // Increment on charger activation
                endNacCount++;
            }
            endNacState = !endNacState;
            //select NAC address on all latch
            PORTB &= CLEAR_OUTPUT_LATCH;
            //set data, for bit 0 (NAC1 SYNCH)
            DATA_SET = endNacState;
            latch6.latch6_output_bit.nac1_sync = endNacState;
            RB6 = 1;
            RB6 = 0;
            //select NAC address on all latch
            PORTB &= CLEAR_OUTPUT_LATCH;
            PORTB |= 0b00000001;
            //set data, for bit 1 (NAC2 SYNCH)
            DATA_SET = endNacState;
            latch6.latch6_output_bit.nac2_sync = endNacState;
            RB6 = 1;
            RB6 = 0;
        }
    }
    else
    {
        // Cycles over, turn off NACs
        latch3.latch3_output_bit.nac1 = 0;
        latch3.latch3_output_bit.nac2 = 0;
    }
    psSysTicks++;
    if ( ( psSysTicks % 100 ) == 0 )
    {
        // Buzzer test cycle 5m on/off for 8 hours then steady for 72 hours (80h*60m/h*60s/m = 288000s)
        if ( psClockTime <= (unsigned long)( 288000 ) )
        {
            if ( ( psClockTime % 300 ) == 0 )
            {
                if ( !endBuzzerState && endBuzzerCount < 49 )
                {
                    // Increment on buzzer activation...
                    endBuzzerCount++;
                }
                // Buzzer must cycle for 8 hours ( cycle 5m on/off = 10m.  60m/10m = 6 cyles per hour. 6 cycle * 8 hours = 48 cycles )
                // then on steady for 72 hours
                if ( endBuzzerCount < 49 )
                {
                    endBuzzerState = !endBuzzerState;
                }
                else
                {
                    endBuzzerState = 1;
                }
                latch2.latch2_output_bit.buzzer = endBuzzerState;
            }
        }
        else
        {
            // 72 hour period over....turn off buzzer
            latch2.latch2_output_bit.buzzer = 0;
        }

        // Battery Charger test cycle 5s on/off for 500 cycles
        if ( endChargerCount <= 500 )
        {
            if ( ( psClockTime % 5 ) == 0 )
            {
                if ( !endChargerState )
                {
                    // Increment on charger activation
                    endChargerCount++;
                }
                endChargerState = !endChargerState;
                latch2.latch2_output_bit.charger = endChargerState;
            }
        }

        // Aux Power test 15 cycles per minute for 6000 cycles
        if ( endAuxCount <= 6000 )
        {
            // 60s/15cycles = 4s/cycle => 2s on/off
            if ( ( psClockTime % 2 ) == 0 )
            {
                if ( !endAuxState )
                {
                    // Increment on aux activation
                    endAuxCount++;
                }
                endAuxState = !endAuxState;
                latch3.latch3_output_bit.aux_power = endAuxState;
            }
        }
        output_refresh();
        psClockTime++;
    }
#else
#if (PANEL_10_ZONE)
    /* select latch's address, latch 6 for nac 1 and 2 */
    /* nac's 3 and 4 are direct pins                   */
    PORTB &= CLEAR_ADDRESS_LATCH;
    PORTB |= (LATCH_6 << 4);
#endif

    /* no NAC synch pulse during walk test */
    if ( system_state.system_state_bit.walk_test )
    {
        genesis_synch_nac1 = 0;
        genesis_synch_nac2 = 0;
#if (PANEL_10_ZONE)
        genesis_synch_nac3 = 0;
        genesis_synch_nac4 = 0;
#endif
        synch_sil_state_ctr = 0;
    }

    //incremented every 10mS. Therfore each case below equates, in
    //   real time, to the case number times 10mS
    synch_sil_state_ctr++;

    /* if forced Genesis resynch */
    if ( reSynchGen )
    {
        /* decrement the synch counter */
        reSynchGen--;

        /* the synch may be delayed, so only do it when it becomes 0 */
        if ( !reSynchGen )
        {
            /* reset state counter */
            synch_sil_state_ctr = 0;
            /* synch Genesis right now */
            nacSynchSecs = 0;
        }
    }

    switch ( synch_sil_state_ctr )
    {
        //NAC output off (start of SYNCH signal)
        case 1:

            /* if Genesis devices need to be turned ON again, clear nacSynchSecs */
#if (PANEL_10_ZONE)
            if ( genesis_turn_on_nac1 || genesis_turn_on_nac2 || genesis_turn_on_nac3 || genesis_turn_on_nac4 )
#elif (PANEL_3_5_ZONE)
            if ( genesis_turn_on_nac1 || genesis_turn_on_nac2 )
#endif
            {
                /* synch right now ( if turn on nac flags set ) */
                nacSynchSecs = 0;

                /* if time to resound Genesis NAC, increment to next state */
                if ( genesis_turn_on_nac1 )
                {
                    genesis_turn_on_nac1++;
                }
                if ( genesis_turn_on_nac2 )
                {
                    genesis_turn_on_nac2++;
                }
#if (PANEL_10_ZONE)
                if ( genesis_turn_on_nac3 )
                {
                    genesis_turn_on_nac3++;
                }
                if ( genesis_turn_on_nac4 )
                {
                    genesis_turn_on_nac4++;
                }
#endif
            }

            /* delay NAC synch signal */
            if ( !nacSynchSecs )
            {
                if (genesis_synch_nac1)
                {
                    //select NAC address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;

                    //set data, for bit 0 (NAC1 SYNCH)
                    DATA_SET = 0;
                    latch6.latch6_output_bit.nac1_sync = 0;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }

                if (genesis_synch_nac2)
                {
                    //select NAC address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;
                    PORTB |= 0b00000001;

                    //set data, for bit 1 (NAC2 SYNCH)
                    DATA_SET = 0;
                    latch6.latch6_output_bit.nac2_sync = 0;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }
            }

            if (continuous_nac1 || temporal_nac1)
            {
                //select NAC address on all latch
                PORTB &= CLEAR_OUTPUT_LATCH;

                //set data, for bit 0 (NAC1 SYNCH)
                DATA_SET = 1;
                latch6.latch6_output_bit.nac1_sync = 1;

#if (PANEL_10_ZONE)
                /* strobe ON */
                ADDRESS_DATA_SET = 1;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;

                /* strobe OFF */
                ADDRESS_DATA_SET = 0;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;
#else
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if (continuous_nac2 || temporal_nac2)
            {
                PORTB &= CLEAR_OUTPUT_LATCH;

                //select NAC address on all latch
                PORTB |= 0b00000001;

                //set data, for bit 1 (NAC2 SYNCH)
                DATA_SET = 1;
                latch6.latch6_output_bit.nac2_sync = 1;

#if (PANEL_10_ZONE)
                /* strobe ON */
                ADDRESS_DATA_SET = 1;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;

                /* strobe OFF */
                ADDRESS_DATA_SET = 0;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;
#else
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if ( drillTemporal )
            {
                buzzer_on_flag = 1;
            }

#if (PANEL_10_ZONE)
            /* delay NAC synch signal */
            if ( !nacSynchSecs )
            {
                if (genesis_synch_nac3)
                {
                    //set data, for NAC3 SYNCH
                    NAC3_SYNC = 0;
                    /* flag input scanner to ignore NAC OC when synching for Genesis */
                    nac3_synch_status = 0;
                }

                if (genesis_synch_nac4)
                {
                    //set data, for NAC4 SYNCH
                    NAC4_SYNC = 0;
                    /* flag input scanner to ignore NAC OC when synching for Genesis */
                    nac4_synch_status = 0;
                }
            }

            if (continuous_nac3 || temporal_nac3)
            {
                //set data, for NAC3 SYNCH
                NAC3_SYNC = 1;
                /* flag input scanner NOT to ignore NAC OC when for continuous or temporal */
                nac3_synch_status = 1;
            }

            if (continuous_nac4 || temporal_nac4)
            {
                //set data, for NAC4 SYNCH
                NAC4_SYNC = 1;
                /* flag input scanner NOT to ignore NAC OC when for continuous or temporal */
                nac4_synch_status = 1;
            }
#endif
            break;

        case 2:

            if ( genesis_turn_on_nac1 == 2 )
            {
                //select NAC address on all latch
                PORTB &= CLEAR_OUTPUT_LATCH;

                //set data, for bit 0 (NAC1 SYNCH)
                DATA_SET = 1;
                latch6.latch6_output_bit.nac1_sync = 1;

#if (PANEL_10_ZONE)
                /* strobe ON */
                ADDRESS_DATA_SET = 1;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;

                /* strobe OFF */
                ADDRESS_DATA_SET = 0;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;
#else
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
                genesis_turn_on_nac1 = 0;
            }

            if ( genesis_turn_on_nac2 == 2 )
            {
                //select NAC address on all latch
                PORTB &= CLEAR_OUTPUT_LATCH;
                PORTB |= 0b00000001;

                //set data, for bit 1 (NAC2 SYNCH)
                DATA_SET = 1;
                latch6.latch6_output_bit.nac2_sync = 1;

#if (PANEL_10_ZONE)
                /* strobe ON */
                ADDRESS_DATA_SET = 1;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;

                /* strobe OFF */
                ADDRESS_DATA_SET = 0;

                /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                ADDRESS_LATCH_STROBE = 1;
                ADDRESS_LATCH_STROBE = 0;
#else
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
                genesis_turn_on_nac2 = 0;
            }

#if (PANEL_10_ZONE)
            if ( genesis_turn_on_nac3 == 2 )
            {
                genesis_turn_on_nac3 = 0;
                //set data, for NAC3 SYNCH
                NAC3_SYNC = 1;

                /* flag input scanner NOT to ignore NAC OC when for turn on */
                nac3_synch_status = 1;
            }

            if ( genesis_turn_on_nac4 == 2 )
            {
                genesis_turn_on_nac4 = 0;
                //set data, for NAC4 SYNCH
                NAC4_SYNC = 1;

                /* flag input scanner NOT to ignore NAC OC when for turn on */
                nac4_synch_status = 1;
            }
#endif
            break;

        //NAC output back on (end of SYNCH signal)
        case 11:

            /* delay NAC synch signal */
            if ( !nacSynchSecs )
            {
                if (genesis_synch_nac1)
                {
                    //select nac address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;

                    //set data, for NAC1 SYNCH
                    DATA_SET = 1;
                    latch6.latch6_output_bit.nac1_sync = 1;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }

                if (genesis_synch_nac2)
                {
                    //select nac address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;
                    PORTB |= 0b00000001;

                    //set data, for NAC2 SYNCH
                    DATA_SET = 1;
                    latch6.latch6_output_bit.nac2_sync = 1;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }

#if (PANEL_10_ZONE)
                if (genesis_synch_nac3)
                {
                    //set data, for NAC3 SYNCH
                    NAC3_SYNC = 1;

                    /* flag input scanner to NOT ignore NAC OC when not synching for Genesis */
                    nac3_synch_status = 1;
                }

                if (genesis_synch_nac4)
                {
                    //set data, for NAC4 SYNCH
                    NAC4_SYNC = 1;

                    /* flag input scanner to NOT ignore NAC OC when not synching for Genesis */
                    nac4_synch_status = 1;
                }
#endif
            }
            break;

        //NAC output off (start of silence sequence)
        case 21:

            /* delay NAC synch signal */
            if ( !nacSynchSecs )
            {
                if (genesis_silence_nac1)
                {
                    //select nac address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;

                    //set data, for NAC1 SYNCH
                    DATA_SET = 0;
                    latch6.latch6_output_bit.nac1_sync = 0;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }

                if (genesis_silence_nac2)
                {
                    //select nac address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;
                    PORTB |= 0b00000001;

                    //set data, for NAC2 SYNCH
                    DATA_SET = 0;
                    latch6.latch6_output_bit.nac2_sync = 0;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }

#if (PANEL_10_ZONE)
                if (genesis_silence_nac3)
                {
                    //set data, for NAC3 SYNCH
                    NAC3_SYNC = 0;
                    /* flag input scanner to ignore NAC OC when silencing Genesis */
                    nac3_synch_status = 0;
                }

                if (genesis_silence_nac4)
                {
                    //set data, for NAC4 SYNCH
                    NAC4_SYNC = 0;
                    /* flag input scanner to ignore NAC OC when silencing Genesis */
                    nac4_synch_status = 0;
                }
#endif
            }
            break;

        //NAC output on
        case 22:
        //NAC output on (end of silence sequence)
        case 24:

            //select nac address on all latch
            PORTB &= CLEAR_OUTPUT_LATCH;

            //set data, for NAC1 SYNCH
            DATA_SET = 1;
            latch6.latch6_output_bit.nac1_sync = 1;

#if (PANEL_10_ZONE)
            /* strobe ON */
            ADDRESS_DATA_SET = 1;

            /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
            ADDRESS_LATCH_STROBE = 1;
            ADDRESS_LATCH_STROBE = 0;

            /* strobe OFF */
            ADDRESS_DATA_SET = 0;

            /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
            ADDRESS_LATCH_STROBE = 1;
            ADDRESS_LATCH_STROBE = 0;
#else
            //clock the data out of latch6
            RB6 = 1;
            RB6 = 0;
#endif

            //select nac address on all latch
            PORTB &= CLEAR_OUTPUT_LATCH;
            PORTB |= 0b00000001;

            //set data, for NAC2 SYNCH
            DATA_SET = 1;
            latch6.latch6_output_bit.nac2_sync = 1;

#if (PANEL_10_ZONE)
            /* strobe ON */
            ADDRESS_DATA_SET = 1;

            /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
            ADDRESS_LATCH_STROBE = 1;
            ADDRESS_LATCH_STROBE = 0;

            /* strobe OFF */
            ADDRESS_DATA_SET = 0;

            /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
            ADDRESS_LATCH_STROBE = 1;
            ADDRESS_LATCH_STROBE = 0;
#else
            //clock the data out of latch6
            RB6 = 1;
            RB6 = 0;
#endif

#if (PANEL_10_ZONE)
            //set data, for NAC3 SYNCH
            NAC3_SYNC = 1;
            nac3_synch_status = 1;

            //set data, for NAC4 SYNCH
            NAC4_SYNC = 1;
            nac4_synch_status = 1;
#endif

            break;

        //NAC output off
        case 23:

            /* delay NAC synch signal */
            if ( !nacSynchSecs )
            {
                if (genesis_silence_nac1)
                {
                    //select nac address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;

                    //set data, for NAC1 SYNCH
                    DATA_SET = 0;
                    latch6.latch6_output_bit.nac1_sync = 0;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }

                if (genesis_silence_nac2)
                {
                    //select nac address on all latch
                    PORTB &= CLEAR_OUTPUT_LATCH;
                    PORTB |= 0b00000001;

                    //set data, for NAC2 SYNCH
                    DATA_SET = 0;
                    latch6.latch6_output_bit.nac2_sync = 0;

#if (PANEL_10_ZONE)
                    /* strobe ON */
                    ADDRESS_DATA_SET = 1;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;

                    /* strobe OFF */
                    ADDRESS_DATA_SET = 0;

                    /* strobe the address latch (write data (ADDRESS_DATA_SET) to pin) */
                    ADDRESS_LATCH_STROBE = 1;
                    ADDRESS_LATCH_STROBE = 0;
#else
                    //clock the data out of latch6
                    RB6 = 1;
                    RB6 = 0;
#endif
                }

#if (PANEL_10_ZONE)
                if (genesis_silence_nac3)
                {
                    //set data, for NAC3 SYNCH
                    NAC3_SYNC = 0;
                    /* flag input scanner to ignore NAC OC when silencing Genesis */
                    nac3_synch_status = 0;
                }

                if (genesis_silence_nac4)
                {
                    //set data, for NAC4 SYNCH
                    NAC4_SYNC = 0;
                    /* flag input scanner to ignore NAC OC when silencing Genesis */
                    nac4_synch_status = 0;
                }
#endif
            }
            break;

        case 51:
        case 151:
        case 251:

            if (temporal_nac1)
            {
#if (PANEL_3_5_ZONE)
                //deactivate NAC1 synch
                //select NAC address on all latch
                PORTB &= CLEAR_OUTPUT_LATCH;

                //set data, for bit 0 (NAC1 SYNCH)
                DATA_SET = 0;
#endif
                latch6.latch6_output_bit.nac1_sync = 0;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if (temporal_nac2)
            {
#if (PANEL_3_5_ZONE)
                PORTB &= CLEAR_OUTPUT_LATCH;

                //select NAC address on all latch
                PORTB |= 0b00000001;

                //set data, for bit 1 (NAC2 SYNCH)
                DATA_SET = 0;
#endif

                //deactivate NAC2 synch
                latch6.latch6_output_bit.nac2_sync = 0;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if ( drillTemporal )
            {
                buzzer_on_flag = 0;
            }

#if (PANEL_10_ZONE)
            if (temporal_nac3)
            {
                //set data, for NAC3 SYNCH
                NAC3_SYNC = 0;
                /* flag input scanner to ignore NAC OC when temporal */
                nac3_synch_status = 0;
            }

            if (temporal_nac4)
            {
                //set data, for NAC3 SYNCH
                NAC4_SYNC = 0;
                /* flag input scanner to ignore NAC OC when temporal */
                nac4_synch_status = 0;
            }
#endif
            break;

        case 101:
        case 201:

            if (temporal_nac1)
            {
#if (PANEL_3_5_ZONE)
                //deactivate NAC1 synch
                //select NAC address on all latch
                PORTB &= CLEAR_OUTPUT_LATCH;

                //set data, for bit 0 (NAC1 SYNCH)
                DATA_SET = 1;
#endif
                //activate NAC1 synch
                latch6.latch6_output_bit.nac1_sync = 1;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if (temporal_nac2)
            {
#if (PANEL_3_5_ZONE)
                PORTB &= CLEAR_OUTPUT_LATCH;

                //select NAC address on all latch
                PORTB |= 0b00000001;

                //set data, for bit 1 (NAC2 SYNCH)
                DATA_SET = 1;
#endif

                //activate NAC2 synch
                latch6.latch6_output_bit.nac2_sync = 1;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if ( drillTemporal )
            {
                buzzer_on_flag = 1;
            }

#if (PANEL_10_ZONE)
            if (temporal_nac3)
            {
                //set data, for NAC3 SYNCH
                NAC3_SYNC = 1;
                /* flag input scanner to NOT ignore NAC OC when temporal */
                nac3_synch_status = 1;
            }

            if (temporal_nac4)
            {
                //set data, for NAC3 SYNCH
                NAC4_SYNC = 1;
                /* flag input scanner to NOT ignore NAC OC when temporal */
                nac4_synch_status = 1;
            }
#endif
            break;

        case 398:
            //
            // Alternate length of cycle such that 30 cycles closely matches 119.4s.  Measured time shows ~6ms
            // skew per 3.98s cycle. By alternating length, inserting additional 10ms (by panel tick) every other
            // cycle for total of 150ms. Gives an overall synch time of ~119.38s.  Close enough for synch.
            //
            if (( nacSynchSecs % 2 ) == 0 )
            {
                break;
            }
        case 399:
            synch_sil_state_ctr = 0;
            //
            // synch Genesis strobes every 30 cycles ( 30 * 3.98s = 119.4s ). Matches CC1S and does
            // not interfere with G1M
            //
            if ( !nacSynchSecs )
            {
                nacSynchSecs = 30;
            }

            nacSynchSecs--;
            break;

    }//end switch statement

#if (CODER_CODE)
    //handle CODER
    if (  coder_nac1
       || coder_nac2
#if (PANEL_10_ZONE)
       || coder_nac3
       || coder_nac4
#endif
       )
    {
        /* Check for non-latching alarm */
        if ( !omCoderIdcActive[omCoderQueueReadIndex] )
        {
            /* Reset coder timer variables */
            omTimersCoderDigitCount = 0;
            omTimersCoderDigitPos   = 0;
            omTimersCoderRoundCount = 0;
            omTimersCoderPulseCount = 0;
            omTimersCoderPulseTimer = 0;
            omCoder                 = 0;

            /* Free current code */
            omCoderQueue[omCoderQueueReadIndex][0] = 0;
            if ( ++omCoderQueueReadIndex >= TIMER_CODER_MAXCODES )
            {
                omCoderQueueReadIndex = 0;
            }

            /* Reset flag to indicate final inter-round delay */
            omCoderFinalRoundDelay = FALSE;
        }

        if ( omTimersCoderRoundCount || omCoderFinalRoundDelay == TRUE )
        {
            /* Completed on or off pulse */
            if ( ++omTimersCoderPulseCount == omTimersCoderPulseTimer )
            {
                /* reset pulse counter */
                omTimersCoderPulseCount = 0;
                /* toggle coder output flag */
                omCoder = !omCoder;
                /* pulse complete? */
                if ( !omCoder )
                {
                    /* completed pulses for current coder digit? */
                    if ( ++omTimersCoderDigitCount == omCoderQueue[omCoderQueueReadIndex][omTimersCoderDigitPos] )
                    {
                        /* reset digit counter */
                        omTimersCoderDigitCount = 0;
                        /* advance past 0 digits (implies shorted code) */
                        while ( ++omTimersCoderDigitPos < TIMER_CODER_MAXDIGITS )
                        {
                            if ( omCoderQueue[omCoderQueueReadIndex][omTimersCoderDigitPos] )
                            {
                                break;
                            }
                        }
                        /* completed code ?*/
                        if ( omTimersCoderDigitPos == TIMER_CODER_MAXDIGITS )
                        {
                            /* reset digit position counter */
                            omTimersCoderDigitPos = 0;
                            /* more rounds ? */
                            if ( --omTimersCoderRoundCount == 0 )
                            {
                                omCoderFinalRoundDelay = TRUE;
                            }
                            omTimersCoderPulseTimer = TIMER_CODER_INTERROUND;
                        }
                        else
                        {
                            omTimersCoderPulseTimer = TIMER_CODER_INTERDIGIT;
                        }
                    }
                }
                else
                {
                    omTimersCoderPulseTimer = TIMER_CODER_DIGIT;
                    if ( omCoderFinalRoundDelay == TRUE )
                    {
                        omCoderFinalRoundDelay = FALSE;
                        omCoder = 0;

                        /* Free current code */
                        omCoderQueue[omCoderQueueReadIndex][0]  = 0;
                        omCoderIdcActive[omCoderQueueReadIndex] = 0;
                        if ( ++omCoderQueueReadIndex >= TIMER_CODER_MAXCODES )
                        {
                            omCoderQueueReadIndex = 0;
                        }
                    }
                }
            }
        }

        if ( !omTimersCoderRoundCount && omCoderFinalRoundDelay == FALSE )
        {
            OMCoderNextCode();

            if (coder_nac1)
            {
#if (PANEL_3_5_ZONE)
                //deactivate NAC1 synch
                //select NAC address on all latch
                PORTB &= CLEAR_OUTPUT_LATCH;

                //set data, for bit 0 (NAC1 SYNCH)
                DATA_SET = 0;
#endif
                latch6.latch6_output_bit.nac1_sync = 0;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if (coder_nac2)
            {
#if (PANEL_3_5_ZONE)
                PORTB &= CLEAR_OUTPUT_LATCH;

                //select NAC address on all latch
                PORTB |= 0b00000001;

                //set data, for bit 1 (NAC2 SYNCH)
                DATA_SET = 0;
#endif

                //deactivate NAC2 synch
                latch6.latch6_output_bit.nac2_sync = 0;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

#if (PANEL_10_ZONE)
            if (coder_nac3)
            {
                //set data, for NAC3 SYNCH
                NAC3_SYNC = 0;
                /* flag input scanner to ignore NAC OC when temporal */
                nac3_synch_status = 0;
            }

            if (coder_nac4)
            {
                //set data, for NAC3 SYNCH
                NAC4_SYNC = 0;
                /* flag input scanner to ignore NAC OC when temporal */
                nac4_synch_status = 0;
            }
#endif
        }
        else
        {
            if (coder_nac1)
            {
#if (PANEL_3_5_ZONE)
                //set NAC1 synch
                //select NAC address on all latch
                PORTB &= CLEAR_OUTPUT_LATCH;

                //set data, for bit 0 (NAC1 SYNCH)
                DATA_SET = omCoder;
#endif
                //activate NAC1 synch
                latch6.latch6_output_bit.nac1_sync = omCoder;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

            if (coder_nac2)
            {
#if (PANEL_3_5_ZONE)
                PORTB &= CLEAR_OUTPUT_LATCH;

                //select NAC address on all latch
                PORTB |= 0b00000001;

                //set data, for bit 1 (NAC2 SYNCH)
                DATA_SET = omCoder;
#endif

                //activate NAC2 synch
                latch6.latch6_output_bit.nac2_sync = omCoder;

#if (PANEL_3_5_ZONE)
                //clock the data out of latch6
                RB6 = 1;
                RB6 = 0;
#endif
            }

#if (PANEL_10_ZONE)
            if (coder_nac3)
            {
                //set data, for NAC3 SYNCH
                NAC3_SYNC = omCoder;
                /* flag input scanner to NOT ignore NAC OC when temporal */
                nac3_synch_status = omCoder;
            }

            if (coder_nac4)
            {
                //set data, for NAC3 SYNCH
                NAC4_SYNC = omCoder;
                /* flag input scanner to NOT ignore NAC OC when temporal */
                nac4_synch_status = omCoder;
            }
#endif
        }
    }
#endif

    /* tell debounce_nac_levels whether or not to ignore NAC OC  */
    /* based on status of sycnh output ( synch OFF = ignore NAC )*/
    ignoreNacOC = 0;
    if ( !latch6.latch6_output_bit.nac1_sync )
    {
        ignoreNacOC |= (0x01 << NAC_1);
    }

    if ( !latch6.latch6_output_bit.nac2_sync )
    {
        ignoreNacOC |= (0x01 << NAC_2);
    }

#if (PANEL_10_ZONE)
    if ( !nac3_synch_status )
    {
        ignoreNacOC |= (0x01 << NAC_3);
    }

    if ( !nac4_synch_status )
    {
        ignoreNacOC |= (0x01 << NAC_4);
    }
#endif

    if (exit_timer)
    {
        exit_timer--;
    }

    if (pulse_120spm_timer)
    {
        pulse_120spm_timer--;
    }

    if (button_valid_timer)
    {
        button_valid_timer--;
    }

    if (pulse_2x20_timer)
    {
        pulse_2x20_timer--;
    }

    if (usart_exit_timer)
    {
        usart_exit_timer--;
    }

    if ( status_to_periph_exit_timer )
    {
        status_to_periph_exit_timer--;
        if ( !status_to_periph_exit_timer )
        {
            ClearCurrentUsartCommand();
        }
    }

	if (i2c_wait_timer)
	{
		i2c_wait_timer--;
	}

    /* check debounce counters for IDCs */
    for (idc_ptr = IDC_1; idc_ptr < MAX_IDC; idc_ptr++)
    {
        if (idc_x_debounce_ctr[idc_ptr])
        {
            idc_x_debounce_ctr[idc_ptr]--;
        }

        if (idcWalkTestTmr[idc_ptr])
        {
            idcWalkTestTmr[idc_ptr]--;
        }

        /* check debounce counters for NACs */
        if (idc_ptr < MAX_NAC && nac_x_debounce_ctr[idc_ptr])
        {
            nac_x_debounce_ctr[idc_ptr]--;
        }
    }

    tic_10ms_ctr--;
    if ( !tic_10ms_ctr )
    {
        //- We are here every 1 sec -

        //restart with count that equates to 1 sec
        tic_10ms_ctr = 100;
        if ( battery_check_timer )
        {
            battery_check_timer--;
        }

        if ( auxTimerOC && auxTimerOC != NOT_RUNNING )
        {
            auxTimerOC--;
        }

        if ( powerLossDebounce && powerLossDebounce != NOT_RUNNING )
        {
            powerLossDebounce--;
        }

        if ( powerRestoreDebounce && powerRestoreDebounce != NOT_RUNNING )
        {
            powerRestoreDebounce--;
        }

        if ( resetTimerSeconds )
        {
            resetTimerSeconds--;
        }

        if ( powerUpSecs )
        {
            powerUpSecs--;
        }

        if ( programmingSecs )
        {
            programmingSecs--;
        }

        if ( idcAlarmVerifySecs )
        {
            idcAlarmVerifySecs--;
        }

        for (idc_ptr = IDC_1; idc_ptr < MAX_IDC; idc_ptr++)
        {
            if (idc_x_timer[idc_ptr])
            {
                idc_x_timer[idc_ptr]--;
            }

            if ( idcDisableDelaySecs[idc_ptr] )
            {
                idcDisableDelaySecs[idc_ptr]--;
            }

            if ( comboDisableDelaySecs[idc_ptr] )
            {
                comboDisableDelaySecs[idc_ptr]--;
            }

            if ( cleanMeInitiationTimer[idc_ptr] )
            {
                cleanMeInitiationTimer[idc_ptr]--;
            }

            if ( cleanMeValidationTimer[idc_ptr] )
            {
                cleanMeValidationTimer[idc_ptr]--;
            }
        }

        if (alarm_sil_inhibit_timer)
        {
            alarm_sil_inhibit_timer--;
        }

#if ( BUZZER_TEST )
        if ( buzzerToggleSecs )
        {
            buzzerToggleSecs--;
        }
#endif

        if (auto_sig_sil_timer != NOT_RUNNING_INT && auto_sig_sil_timer)
        {
            auto_sig_sil_timer--;
        }

        if (ac_delay_timer != NOT_RUNNING_INT && ac_delay_timer)
        {
            ac_delay_timer--;
        }

        tic_sec_ctr--;

        if ( !tic_sec_ctr )
        {
            //- We are here every 1 minute -

#if ( BUZZER_TEST )
            if ( buzzerPhaseMins )
            {
                buzzerPhaseMins--;
            }
#endif

            //restart with count that equates to 1 minute
            tic_sec_ctr = 60;

            /* check minute counters for IDCs */
            for (idc_ptr = IDC_1; idc_ptr < MAX_IDC; idc_ptr++)
            {
                if ( cleanMeRestorationTimer[idc_ptr] )
                {
                    cleanMeRestorationTimer[idc_ptr]--;
                }
            }

            /* update walk test inactivity timer */
            if ( walkTestInactiveMins )
            {
                walkTestInactiveMins--;
            }

            if ( troubleSilenceMinutes && troubleSilenceMinutes != NOT_RUNNING_INT )
            {
                troubleSilenceMinutes--;

                /* if buzzer silenced and 24 hours passed, re-annunciate trouble */
                if ( !troubleSilenceMinutes )
                {
                    /* if trouble LED pulsing, turn buzzer back on */
                    if ( trouble_LED_flag )
                    {
                        buzzer_on_flag = 1;
                    }

                    /* let buzzer sound */
                    buzzerSilenced = 0;

                    /* turn on RSI's buzzer again */
                    peripheral_state.peripheral_state_bit.trouble = 0;

                    /* stop timer */
                    troubleSilenceMinutes = NOT_RUNNING_INT;
                }
            } /* if troubleSilenceMinutes*/
        }/* if (tic_sec_ctr == 0) */
    }/* if (tic_10ms_ctr == 0) */
#endif
} /* end of tc_int */


/**************************************************************************
 ** FUNCTION: initializations                                            **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function initializes the panel interrput, comm and outputs.     **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void initializations(void)
{
    TMR1CS = 0; /* Timer1 internal clock driven (Fosc/4) */

    T1CON = 0b00000001;
    CCP1CON = 0b00001011;
    CCPR1H = 0x27;
    CCPR1L = 0x10;
    CCP1IE = 1;
    PEIE = 1;
    GIE = 1;

    /* turn off low voltage detect circuit */
    LVDCON = 0;

#if (PANEL_10_ZONE)
    TRISA = 0b11110111;   //all bits inputs except RA3
#else
    TRISA = 0xff;   //all bits inputs
#endif
    TRISB = 0x00;   /* all bits outputs */
#if (!PROC_18F252)
    TRISC = 0b10111000; //RC7(RX) input, RC5 (I2C BUSY), SCL(RC3) & SDA(RC4) configured as inputs, rest outputs
#else
    TRISC = 0b11111000; //RC7(RX),RC6(TX) input (EUSART requires), RC5 (I2C BUSY), SCL(RC3) & SDA(RC4) configured as inputs, rest outputs
#endif
#if (PANEL_10_ZONE)
    TRISD = 0b11000000; //RD5 (Rem_Disc), RD4 (Const Aux), RD3 (Svc Det), RD2 (Com WF), RD1 (NAC 3/4 Class A), RD0 (NAC 1/2 Class A) all outputs
#endif

#if (PANEL_3_5_ZONE)
    //if both RA2 & RA3 are at 5 volts, we're a 3 zone panel
    if ( ad_conversion(2) < 922 && ad_conversion(3) < 922 )
    {
        five_zone_panel = 0xff;    //since RA2(an2) is less than 5 volts...we must be a 5 zone
    }
#endif

    latch1.latch1_output_byte = 0b00000010;
    latch2.latch2_output_byte = 0;
#if ( BUZZER_TEST )
    latch2.latch2_output_bit.buzzer = 1;
#endif
    /* aux power ON */
    latch3.latch3_output_byte = 0;
    latch4.latch4_output_byte = 0;
    latch5.latch5_output_byte = 0;
    latch6.latch6_output_byte = 0;
#if (PANEL_10_ZONE)
    latch7.latch7_output_byte = 0;
    latch8.latch8_output_byte = 0;
    latch9.latch9_output_byte = 0;
#endif

    buzzer_blip_flag = 0;
    buzzer_lockout_flag = 0;

    load_thresh_levels();

    /* clear system state byte */
    system_state.system_state_byte = 0;
    ClearPeripheralStatus(ALL);

    /* reset all button flags */
    button.button_int = 0;

#if (PANEL_10_ZONE)
    button2.button_int2 = 0;
#endif

    /* delay some processing on powerup */
    powerUpSecs = 3;

    /* initialize coder variables */
    OMCoderReset();
}

/**************************************************************************
 ** FUNCTION: ClearPeripheralStatus                                      **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function clears the status variables associated with the        **
 ** peripherals.                                                         **
 **                                                                      **
 ** PARAMETERS: updated peripheral_state structure                       **
 **             function - how much of status needs to be updated        **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ClearPeripheralStatus( unsigned char function )
{
    previousStatus = 0;
    /* not if WALKTEST_REFRESH, update ALL peripherals with current status */
    if ( function & REFRESH )
    {
        refreshStatus = 1;
    }
    /* REFRESH_CMN, update ALL peripherals with new system activation */
    else if ( function & REFRESH_CMN )
    {
        previousStatus &= ~( SP_ALARM | SP_SUPERVISORY | SP_TROUBLE | SP_MONITOR );
        buzzer_on_flag  = 1;
    }
    /* function == all - clear all status variables */
    else if ( function == ALL )
    {
        previousStatus = 0xff;
        currentStatus = 0;
        peripheral_state.peripheral_state_byte = 0;
    }
}

/**************************************************************************
 ** FUNCTION: UpdatePeripheralState                                      **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function will update peripherals using the SYSTEM_STATE_CHANGE  **
 ** to send common type (RSI,RRM) status.                                **
 **                                                                      **
 ** PARAMETERS: updated peripheral_state structure                       **
 **                                                                      **
 ** RETURNS:  peripheral communication status                            **
 **                                                                      **
 **************************************************************************/
void UpdatePeripheralState(void)
{
    static unsigned char currentIDC = IDC_1;
           unsigned char currentIDCStatus;
           union IDC_STATUS_BIT_BYTE tempIDCStatus;

    /* check previous status against current status (excluding lamp_test and trouble_silence */
    if ( UpdateRequired )
    {

#if (USART_TEST)
        latch1.latch1_output_bit.nac1_trbl = 1;
        latch1.latch1_output_bit.nac2_trbl = 0;
        latch8.latch8_output_bit.nac3_trbl = 0;
        latch8.latch8_output_bit.nac4_trbl = 0;
#endif

        /* send new status to peripherals (keep calling function till comm completed) */
        if ( status_to_periph(SYS_STAT_CHANGE, 0, currentStatus, currentStatus2) == FINISHED )
        {
            /* when done, save new status (except for lamp_test and trouble_silence */
            previousStatus = currentStatus;
            previousStatus2 = currentStatus2;
            UpdateLocked = 0;
        }
        else
        {
            UpdateLocked = 1;
        }
    }
    /* check to see if IDC status needs to be refreshed */
    else if ( refreshStatus )
    {
        /* send new status to peripherals (keep calling function till comm completed) */
        //if ( status_to_periph(IDC_STAT_CHANGE, currentIDC, idc_status[currentIDC].idc_status_byte, 0 ) == FINISHED )
        tempIDCStatus = idc_status[currentIDC];
        currentIDCStatus = tempIDCStatus.idc_status_byte & 0x07;
        if ( tempIDCStatus.idc_status_bit.waterflow ) currentIDCStatus |= SP_ALARM;
        if ( tempIDCStatus.idc_status_bit.trouble   ) currentIDCStatus |= SP_TROUBLE;
        if ( tempIDCStatus.idc_status_bit.disabled  ) currentIDCStatus |= SP_IDC_DISABLE;
        if ( status_to_periph(IDC_STAT_CHANGE, currentIDC, currentIDCStatus, 0 ) == FINISHED )
        {
            /* if sent to all peripherals, clear refresh flag */
            if ( ++currentIDC == MAX_IDC )
            {
                refreshStatus = 0;
                currentIDC = IDC_1;
            }
        }
    }
    /* if buzzer is to be silenced remotely */
    else if ( peripheral_state.peripheral_state_bit.trouble_silence )
    {

#if (USART_TEST)
        latch1.latch1_output_bit.nac1_trbl = 0;
        latch1.latch1_output_bit.nac2_trbl = 1;
        latch8.latch8_output_bit.nac3_trbl = 0;
        latch8.latch8_output_bit.nac4_trbl = 0;
#endif

        /* send command to silence buzzer (keep calling function till comm completed) */
        if ( status_to_periph(TROUBLE_SILENCE, 0, 0, 0) == FINISHED )
        {
            /* when done, clear flag */
            peripheral_state.peripheral_state_bit.trouble_silence = 0;
        }
    }
    /* check if RSI put peripherals in lamp test */
    else if ( peripheral_state.peripheral_state_bit.lamp_test )
    {
#if (USART_TEST)
        latch1.latch1_output_bit.nac1_trbl = 0;
        latch1.latch1_output_bit.nac2_trbl = 0;
        latch8.latch8_output_bit.nac3_trbl = 1;
        latch8.latch8_output_bit.nac4_trbl = 0;
#endif
        /* send lamp test command to peripherals until done  */
        if ( status_to_periph(LAMP_TEST, 0, 0, 0) == FINISHED )
        {
            /* clear flag so polling routine can set it again */
            peripheral_state.peripheral_state_bit.lamp_test = 0;

            /* clear command so polling can be done */
            ClearCurrentUsartCommand();
        }
    }
    /* update peripheral status */
    if ( !button.button_bit.lamp_test && !UpdateLocked )
    {

#if (USART_TEST)
        latch1.latch1_output_bit.nac1_trbl = 0;
        latch1.latch1_output_bit.nac2_trbl = 0;
        latch8.latch8_output_bit.nac3_trbl = 0;
        latch8.latch8_output_bit.nac4_trbl = 1;
#endif

        /* check for alarm */
        peripheral_state.peripheral_state_bit.alarm = latch1.latch1_output_bit.com_alarm;

        /* check for supervisory */
        peripheral_state.peripheral_state_bit.supervisory = latch1.latch1_output_bit.com_sup;

        /* check for trouble */
        peripheral_state.peripheral_state_bit.trouble = latch1.latch1_output_bit.com_trbl;

        /* check for ground fault */
        peripheral_state.peripheral_state_bit.ground_fault = trouble.trouble_bit.gnd_fault;

        /* check for AC power */
        peripheral_state.peripheral_state_bit.ac_loss = trouble.trouble_bit.ac_loss;

        /* check for reset */
        peripheral_state2.peripheral_state_bit.reset = system_state.system_state_bit.reset;

        /* check for signal silence */
        peripheral_state2.peripheral_state_bit.signal_silence = latch2.latch2_output_bit.silenced;

        /* check for panel silence */
        peripheral_state2.peripheral_state_bit.panel_silence = buzzerSilenced;

        /* update currentStatus with the current peripheral state */
        currentStatus  = ( peripheral_state.peripheral_state_byte & 0b00111111 );
        currentStatus2 = ( peripheral_state2.peripheral_state_byte & 0b00000111 );
        UpdateRequired = 0;
        if ( currentStatus != previousStatus || currentStatus2 != previousStatus2 )
        {
            UpdateRequired = 1;
        }
    }
}


/**************************************************************************
 ** FUNCTION: ExitWalkTestMode                                           **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles clean up and exit from walk test mode.         **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ExitWalkTestMode(void)
{
    unsigned char indexIDC;

    /* clear walk test variables */
    system_state.system_state_byte &= 0b01011111;
    latch2.latch2_output_bit.walk_test = 0;
    latch2_pulse.latch2_pulse_bit.walk_test = 0;
    trouble2.trouble_bit2.walk_test = 0;

    /* turn on IDCs */
    latch4.latch4_output_bit.smoke_reset = 0;

    /* disable inhibit aux power */
    auxTimerOC = 0;
    powerUpSecs = 0;

    /* clear walk_test bit for all zones */
    for ( indexIDC = IDC_1; indexIDC < MAX_IDC; indexIDC++ )
    {
        idcWalkTestState[indexIDC] = WALK_TEST_RESET_START;
        if ( idc_status2[indexIDC].idc_status_bit2.walk_test )
        {
            /* clear any status variables used by Walk Test */
            idc_status[indexIDC].idc_status_byte = 0;
            idc_status2[indexIDC].idc_status_byte2 = 0;
            light_or_extinguish_idc_trouble_leds(EXTINGUISH, indexIDC);
            idcWalkTestTmr[indexIDC] = 0;
            idcWalkTestCnt[indexIDC] = 0;
        }
    }
}

/**************************************************************************
 ** FUNCTION: ProcessWalkTest                                            **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles all functionality of a walk test.              **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ProcessWalkTest(void)
{
#if ( !BUZZER_TEST )
    unsigned char indexIDC;
    unsigned char walkTestReset;
    static unsigned int  resetZoneSecs;
    static unsigned char resetAuxSecs;
    static unsigned char flash = 0;
    static unsigned char flashCount = WALK_TEST_FLASH_RATE;

    /*-----------------------------------*/
    /* check if walk test button pressed */
    /*-----------------------------------*/
    if ( button.button_bit.walk_test )
    {
        /* acknowledge button press */
        button.button_bit.walk_test = 0;

        /* make sure walk test LED ON */
        latch2.latch2_output_bit.walk_test = 1;

        /* set inactivity timer to 30 minutes */
        walkTestInactiveMins = WALK_TEST_INACTIVITY;

        //clear flag in case IDCs were already disabled.
        pulse_2x20_flag = 0;
        latch1.latch1_output_bit.com_disable = 0;

        /* first time in routine, enter walk test mode */
        if ( !system_state.system_state_bit.walk_test )
        {
            /* Enter walk test mode */
            system_state.system_state_bit.walk_test = 1;
            /* pulse Walk Test LED */
            latch2_pulse.latch2_pulse_bit.walk_test = 1;
        }
        /* walk test mode activated, want silent walk test */
        else if ( !system_state.system_state_bit.silent_walk_test )
        {
            /* Enter silent walk test mode */
            system_state.system_state_bit.silent_walk_test = 1;
            /* do not pulse Walk Test LED in silent walk test mode */
            latch2_pulse.latch2_pulse_bit.walk_test = 0;
        }
        /* exit walk test mode */
        else
        {
            ExitWalkTestMode();
        }
    }

    /* slow flashing down */
    flashCount--;
    if ( !flashCount )
    {
        /* use this flag for flashing LEDs */
        flash = !flash;
        flashCount = WALK_TEST_FLASH_RATE;
    }

    /*--------------------------------------------------*/
    /* Processing done only if system in walk test mode */
    /*--------------------------------------------------*/
    if ( system_state.system_state_bit.walk_test )
    {
        /* make sure system is in trouble */
        trouble2.trouble_bit2.walk_test = 1;

        /* loop through all zones */
        for ( indexIDC = IDC_1; indexIDC < MAX_IDC; indexIDC++ )
        {
            /* if a zone, not in walk test mode, goes active, exit walk test mode */
            if ( (idc_status[indexIDC].idc_status_byte & 0b00011111) && !idc_status2[indexIDC].idc_status_bit2.walk_test )
            {
                /* clean up walk test mode */
                ExitWalkTestMode();
                return;
            }
        }

        /* loop through all zones */
        for ( indexIDC = IDC_1; indexIDC < MAX_IDC; indexIDC++ )
        {

            /************************************/
            /* if zone's disable button pressed */
            /************************************/
            /* check if walk test zone is resetting  */
            walkTestReset = idc_status2[indexIDC].idc_status_bit2.walk_test_reset;
            if ( idc_status[indexIDC].idc_status_bit.disabled )
            {
                /* reset inactivity timer */
                walkTestInactiveMins = WALK_TEST_INACTIVITY;

                /* clear disable button */
                idc_status[indexIDC].idc_status_bit.disabled = 0;

                if ( !walkTestReset )
                {
                    /* if current zone in walk test */
                    if ( idc_status2[indexIDC].idc_status_bit2.walk_test )
                    {
                        /* make it not in walk test */
                        idc_status2[indexIDC].idc_status_bit2.walk_test = 0;

                        /* clear ALL walk test flags */
                        idc_status2[indexIDC].idc_status_byte2 &= 0b01000000;
                        /* clear trouble LED  */
                        light_or_extinguish_idc_trouble_leds( EXTINGUISH, indexIDC);
                    }
                    else
                    {
                        /* make it not in walk test */
                        idc_status2[indexIDC].idc_status_bit2.walk_test = 1;
                        /* if zone in walk test, set trouble LED */
                        light_or_extinguish_idc_trouble_leds( LIGHT, indexIDC);
                    }
                }
            }

            /* check if walk test zone is resetting  */
            if ( walkTestReset )
            {
                /* pulse IDC trouble LED while in reset */
                light_or_extinguish_idc_trouble_leds( flash, indexIDC);

                /* handle each zone according to it's state */
                switch ( idcWalkTestState[indexIDC] )
                {
                    case WALK_TEST_RESET_START :

                        /* turn off NACs */
                        nac_active_handler(CLEAR_STATUS);

                        /* done annunciating zone */
                        idc_status2[indexIDC].idc_status_bit2.walk_test_annun = 0;

                        /* reset zone for X seconds */
                        idcWalkTestTmr[indexIDC] = resetZoneSecs;

                        /* turn off IDCs */
                        latch4.latch4_output_bit.smoke_reset = 1;

                        /* inhibit aux power for X seconds  */
                        auxTimerOC = resetAuxSecs;

                        /* ignore peripherals, for nore than aux power, during reset period */
                        powerUpSecs = auxTimerOC + 1;

                        /* turn off aux power */
                        latch3.latch3_output_bit.aux_power = 0;

                        /* next state - wait for reset to finish */
                        idcWalkTestState[indexIDC]++;
                        break;

                    /* wait here until timeout, then turn on zones and aux power */
                    case WALK_TEST_RESET_RESET :

                        /* wait for timeout */
                        if ( !idcWalkTestTmr[indexIDC] )
                        {
                            /* make sure IDC power ON */
                            latch4.latch4_output_bit.smoke_reset = 0;

                            /* make sure aux power ON */
                            latch3.latch3_output_bit.aux_power = 1;

                            /* refresh peripheral status */
                            ClearPeripheralStatus( WALKTEST_REFRESH );

                            /* if panel buzzer OFF, turn RSI buzzer OFF */
                            if ( !buzzer_on_flag )
                            {
                                peripheral_state.peripheral_state_bit.trouble_silence = 1;
                            }
                            else
                            {
                                peripheral_state.peripheral_state_bit.trouble_silence = 0;
                            }

// BOC CSM 2/26/2007
                            /* debounce zone restore for X seconds */
                            idcWalkTestTmr[indexIDC] = SEC_2;
// EOC CSM 2/26/2007

                            /* next state - wait for "green" zone */
                            idcWalkTestState[indexIDC]++;
                        }
                        break;

                    /* wait here until trouble and tripped go away */
                    case WALK_TEST_RESET_WAIT :

                        /* wait for zone to be NOT tripped, and NOT in trouble, before annunciating new walk test */
                        if ( !(idc_status2[indexIDC].idc_status_byte2 & 0b00100010) && !trouble.trouble_bit.gnd_fault )
                        {
// BOC CSM 2/26/2007
//                            /* light associated trouble LED */
//                            light_or_extinguish_idc_trouble_leds(LIGHT, indexIDC);

//                            /* clear count */
//                            idcWalkTestCnt[indexIDC] = 0;

//                            /* clear zone in reset flag AND walk test trouble active flag */
//                            idc_status2[indexIDC].idc_status_byte2 &= 0b01101111;

//                            idcWalkTestState[indexIDC] = WALK_TEST_RESET_START;

                            /* zone restore debounce complete? */
                            if ( !idcWalkTestTmr[indexIDC] )
                            {
                                /* light associated trouble LED */
                                light_or_extinguish_idc_trouble_leds(LIGHT, indexIDC);

                                /* clear count */
                                idcWalkTestCnt[indexIDC] = 0;

                                /* clear zone in reset flag AND walk test trouble active flag */
                                idc_status2[indexIDC].idc_status_byte2 &= 0b01101111;

                                idcWalkTestState[indexIDC] = WALK_TEST_RESET_START;
                            }
// EOC CSM 2/26/2007
                        }
// BOC CSM 2/26/2007
                        else
                        {
                            /* debounce zone restore for X seconds */
                            idcWalkTestTmr[indexIDC] = SEC_2;
                        }
// EOC CSM 2/26/2007
                        break;
                }
            }
            /* check if walk test zone is in trouble (or trouble has been activated) */
            else if ( ( idc_status2[indexIDC].idc_status_byte2 & 0b10100000 )
                    || ( trouble.trouble_bit.gnd_fault && idc_status2[indexIDC].idc_status_bit2.walk_test ) )
            {
                /******************************************/
                /* if NOT silent walk test, activate NACs */
                /******************************************/
                if ( !system_state.system_state_bit.silent_walk_test )
                {
                    /* handle NACs during trouble */
                    nac_active_handler( indexIDC );
                }

                /* NACs are currently not active */
                if ( !idc_status2[indexIDC].idc_status_bit2.walk_test_nac )
                {
                    /* set as active */
                    idc_status2[indexIDC].idc_status_bit2.walk_test_nac = 1;

                    /* set walk test trouble active flag */
                    idc_status2[indexIDC].idc_status_bit2.walk_test_trbl_active = 1;

                    /* keep NACs on for 2 second */
                    idcWalkTestTmr[indexIDC] = SEC_2;

                    /* set up reset times */
                    /* annunciate ground fault for 2 seconds */
                    resetZoneSecs = SEC_2;
                    resetAuxSecs = 2;

                    /* annunciate IDC troubles for 4 seconds */
                    if ( !trouble.trouble_bit.gnd_fault )
                    {
                        /* set up reset times */
                        resetZoneSecs = SEC_4;
                        resetAuxSecs = 4;
                    }
                }
                else
                {
                    /* after timeout */
                    if ( !idcWalkTestTmr[indexIDC] )
                    {
                        /* turn off NACs */
                        nac_active_handler(CLEAR_STATUS);

                        /* clear NAC active */
                        idc_status2[indexIDC].idc_status_bit2.walk_test_nac = 0;

                        /* reset the zone(s) */
                        idc_status2[indexIDC].idc_status_bit2.walk_test_reset = 1;

                        /* if trouble because of an open, wait for it to go away */
                        if ( idc_level[indexIDC].idc_level_bit.open )
                        {
                            idcWalkTestState[indexIDC] = WALK_TEST_RESET_WAIT;
                        }
                    }
                }
            }
            /* check if walk test zone in active state or annunciating zone number */
            else if ( idc_status2[indexIDC].idc_status_byte2 & 0b00001010 )
            {
                /* reset inactivity timer */
                walkTestInactiveMins = WALK_TEST_INACTIVITY;

                /* wait for timeout */
                if ( !idcWalkTestTmr[indexIDC] )
                {
                    /* if still need to annunciate current zone */
                    if ( idcWalkTestCnt[indexIDC] < indexIDC+1 )
                    {
                        /* annunciating zone number */
                        idc_status2[indexIDC].idc_status_bit2.walk_test_annun = 1;

                        /* if NACs active, turn off */
                        if ( idc_status2[indexIDC].idc_status_bit2.walk_test_nac )
                        {
                            idcWalkTestCnt[indexIDC]++;
                        }

                        /* toggle flag */
                        idc_status2[indexIDC].idc_status_bit2.walk_test_nac = !idc_status2[indexIDC].idc_status_bit2.walk_test_nac;

                        /* set timer for 500 ms */
                        idcWalkTestTmr[indexIDC] = MS500;

                        /* if NACs are to be turned ON */
                        if ( idc_status2[indexIDC].idc_status_bit2.walk_test_nac )
                        {
                            /* keep them on for 1.5 seconds */
                            idcWalkTestTmr[indexIDC] += SEC_1;
                        }
                    }
                    else
                    {
                        /* set up to do a reset */
                        idc_status2[indexIDC].idc_status_bit2.walk_test_reset = 1;

                        /* set up reset times */
                        resetZoneSecs = SEC_8;
                        resetAuxSecs = 8;
                    }
                }

                /* if NACs are to be deactivated */
                if ( !idc_status2[indexIDC].idc_status_bit2.walk_test_nac )
                {
                    nac_active_handler(CLEAR_STATUS);
                }
                /******************************************/
                /* if NOT silent walk test, activate NACs */
                /******************************************/
                else if ( !system_state.system_state_bit.silent_walk_test )
                {
                    /* activate NACs */
                    nac_active_handler( indexIDC );
                }
            }
        }
    }

    /* if timeout, entering programming mode, or in drill, exit walk test mode */
    if ( !walkTestInactiveMins || !RA4 || system_state.system_state_bit.drill )
    {
        ExitWalkTestMode();
    }
#endif
}


#if (DACT_CODE)
/**************************************************************************
 ** FUNCTION: UpdateDACT                                                 **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function will update the DACT with any system status delta.     **
 **                                                                      **
 ** PARAMETERS: updated system state                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void UpdateDACT(void)
{
    static unsigned char previousDACTIdcStatus[MAX_IDC], previousDACTNacStatus[MAX_NAC];
    static unsigned char previousDACTSysStatus, previousDACTSysStatus2, previousDACTModeStatus;
    static unsigned char DACTIdcStatus[MAX_IDC], DACTNacStatus[MAX_NAC];
    static unsigned char DACTSysStatus, DACTSysStatus2, DACTModeStatus;
	unsigned char index, dataBaseCtr;
	unsigned char i;

    /* Supervise DACT during all modes */
	load_poll_for_presence_data();	//loads "var_dact_data" array
    dialer_comm(READ_FROM_DIALER, var_dact_data);
    if ( commands_from_dact.commands_from_dact_bit.dact_has_data_for_panel )
    {
        if ( dialer_comm(READ_FROM_DIALER, request_data_from_dact) == DIALER_COMM_OK )
        {
            if ( commands_from_dact.commands_from_dact_bit.dact_trouble )
            {
                trouble2.trouble_bit2.dact_reported_troub = 1;	//signify that the DACT has reported a trouble cond
                if ( !system_state.system_state_bit.program && !system_state.system_state_bit.reset )
                {
                    // DACT will only resend on new events. refresh to force resound of system buzzers
                    ClearPeripheralStatus(REFRESH_CMN);
                }
            }
			if ( commands_from_dact.commands_from_dact_bit.end_dact_trouble )
			{
				trouble2.trouble_bit2.dact_reported_troub = 0;	//signify that DACT troub has restored
			}
			if (commands_from_dact.commands_from_dact_bit.end_dact_program_mode)
			{
				DACT_program_mode_active_flag = 0;	//don't send any more key presses to DACT (I2C comnd #5)
			}
            if (commands_from_dact.commands_from_dact_bit.dact_queue_empty)
			{
                DACTIdcEventLatch = 0; /* DACT sent all IDC activations, non-latching IDCs can clear (for FACOM/SSU) */
			}
			if (commands_from_dact.commands_from_dact_bit.produce_upload)
			{
                var_dact_data[0] = 0x33;      //#of bytes (including this)
                var_dact_data[1] = Config_Up; //command "SENDING CONFIGURATION UPLOAD"
                for(i = 0; i <= EEPROM_PERIPHERALS_RRM; i++)
				{

					var_dact_data[i+2] = ext_eeprom_read(i);	//read panel's E2 immage into array
				}
				dialer_comm(WRITE_TO_DIALER, var_dact_data);	//send it to dialer
			}
			if (commands_from_dact.commands_from_dact_bit.accept_download)
			{
				if(dialer_comm(READ_FROM_DIALER,provide_config_download) == DIALER_COMM_OK)
				{
                    for(i = 0; i <= EEPROM_LAST_CONFIG_ADDRESS; i++)
					{
						ext_eeprom_write(i, i2c_data_rcv_array[i+1]);	//write downloaded immage to E2
					}

                    dataBaseCtr = ext_eeprom_read(EEPROM_DATABASE_VERSION);
					dataBaseCtr++;
                    ext_eeprom_write(EEPROM_DATABASE_VERSION, dataBaseCtr);
				}
				//stay in this loop while "sniffing" for peripherals
				while( poll_for_peripherals(DACT_REQUESTED) != 1 )
				{
                    usart_comm();
                    DogWatch();
				}

				// upload EEPROM data (including peripherals list) back to DACT
                var_dact_data[0] = 0x33;      //#of bytes (including this)
                var_dact_data[1] = Config_Up; //command "SENDING CONFIGURATION UPLOAD"
                for(i = 0; i <= EEPROM_PERIPHERALS_RRM; i++)
				{

					var_dact_data[i+2] = ext_eeprom_read(i);	//read panel's E2 immage into array
				}
				dialer_comm(WRITE_TO_DIALER, var_dact_data);	//send it to dialer
			}
        }
    }
        for (index = IDC_1; index < MAX_IDC; index++)
		{
            /***********************************/
            /* update DACT status for each IDC */
            /***********************************/
            /* these are common to all communication */
            var_dact_data[0] = 5;         //#of bytes (including this)
            var_dact_data[1] = Stat_Delta;//command#
            //var_dact_data[3] = index + 1; //IDC/NAC number with "delta" status change

            if ( !idc_status2[index].idc_status_bit2.walk_test )
            {
                if ( nonLatchingIDC(index) )
                {
                    /* clear non-latching input bits */
                    DACTIdcStatus[index] = 0;
                }
                else
                {
                    /* clear non-latching input bits (alarm, sup and waterflow are latching) */
                    DACTIdcStatus[index] &= 0b00001011;
                }

                /* Set current active IDC bits */
                DACTIdcStatus[index] |= (idc_status[index].idc_status_byte & 0b00111111);

                /* Set service detector status bit */
                if ( idc_status3[index].idc_status_bit3.cleanMeActive )
                {
                    DACTIdcStatus[index] |= 0b01000000;
                }

                //see if IDC presently DISABLED
                if ( (DACTIdcStatus[index] & 0b00100000) || idcDisableDelaySecs[index] || comboDisableDelaySecs[index] )
                {
                    DACTIdcStatus[index] &= 0b00100000; //if so, clear all flags except disabled flag
                }

                /* if IDC currently has active waterflow */
                if ( idc_status[index].idc_status_bit.waterflow )
                {
                    /* DO NOT send the alarm bit set */
                    DACTIdcStatus[index] &= 0b11111110;
                }

                /* if in reset, clear DACT IDC status */
                if ( system_state.system_state_bit.reset )
                {
                    DACTIdcStatus[index] = 0;
                    DACTIdcEventLatch    = 0;
                }

                /* if ststus has changed, update DACT */
                if ( DACTIdcStatus[index] != previousDACTIdcStatus[index] )
                {
                    /* save current DACT IDC status for comparison later */
                    previousDACTIdcStatus[index] = DACTIdcStatus[index];

                    var_dact_data[2] = 1;                       //change type (1 = IDC change)
                    var_dact_data[3] = index + 1;   //compiler bug fix 4/23/02 (2 quick zone trips caused loss of this value
                    var_dact_data[4] = DACTIdcStatus[index];    //masked off bit field of current IDC status

                    dialer_comm(WRITE_TO_DIALER, var_dact_data);
                }
            }

            /***********************************/
            /* update DACT status for each NAC */
            /***********************************/
            if ( index < MAX_NAC )
            {
                /* clear NAC active bits */
                DACTNacStatus[index] = 0;

                /* if NOT in reset, update DACT NAC status */
                if ( !system_state.system_state_bit.reset )
                {
                    /* Set current active NAC bits */
                    DACTNacStatus[index] |= (nac_status[index].nac_status_byte & 0b00001111);
                }

				if(DACTNacStatus[index] & 0b00001000)	//see if NAC currently DISABLED
				{
					DACTNacStatus[index] &= 0b00001000;	//if so, clear all flags except disabled flag
				}

                if ( DACTNacStatus[index] != previousDACTNacStatus[index] )
                {
                    /* save current DACT NAC status for comparison later */
                    previousDACTNacStatus[index] = DACTNacStatus[index];

                    var_dact_data[2] = 2;                       //change type (2 = NAC change)
					var_dact_data[3] = index + 1;	//compiler bug fix 4/23/02 (2 quick zone trips caused loss of this value
                    var_dact_data[4] = DACTNacStatus[index];    //masked off bit field of current NAC status

                    dialer_comm(WRITE_TO_DIALER, var_dact_data);
                }
            }

        }


        /* Update DACT system status with any system trouble changes */
        DACTSysStatus = trouble.trouble_byte;
        DACTSysStatus2 = (trouble2.trouble_byte2 & 0b00000111);

        /* Update DACT mode status based on system's current mode */
        DACTModeStatus = 0;
        /* check for system in Reset mode */
        if ( system_state.system_state_bit.reset )
        {
            /* set bit 5 */
            DACTModeStatus |= 0b00100000;
        }
		else
		{
	        /* check for system in Walk Test mode */
	        if ( system_state.system_state_bit.walk_test )
	        {
	            /* set bit 7 */
	            DACTModeStatus |= 0b10000000;
	        }

	        /* check for system in Drill mode */
	        if ( system_state.system_state_bit.drill )
	        {
	            /* set bit 6 */
	            DACTModeStatus |= 0b01000000;
	        }

	        /* check for Alarms Silenced */
	        if ( latch2.latch2_output_bit.silenced && !button.button_bit.lamp_test && !system_state.system_state_bit.program )
	        {
	            /* set bit 4 */
	            DACTModeStatus |= 0b00010000;
	        }

	        /* check for active trouble silenced */
            if ( buzzerSilenced )
	        {
	            /* set bit 3 */
	            DACTModeStatus |= 0b00001000;
	        }

			/* check for system in lamp test */
			if( button.button_bit.lamp_test )
			{
				/* set bit 2 */
				DACTModeStatus |= 0b00000100;
			}

            /* check for ac fail delay */
            if ( ac_delay_timer != NOT_RUNNING_INT && ac_delay_timer )
            {
                /* set bit 1 */
                DACTModeStatus |= 0b00000010;
            }

            /* check for remote disconnect */
            if( trouble2.trouble_bit2.remoteDisconnect )
			{
                /* set bit 0 */
                DACTModeStatus |= 0b00000001;
			}
		}

        /* if system status has changed, up date DACT */
        if ( DACTSysStatus != previousDACTSysStatus
             || DACTSysStatus2 != previousDACTSysStatus2
             || DACTModeStatus != previousDACTModeStatus )
		{
            /* save current DACT system status for comparison later */
			previousDACTSysStatus = DACTSysStatus;
			previousDACTSysStatus2 = DACTSysStatus2;
            previousDACTModeStatus = DACTModeStatus;

            var_dact_data[0] = 7;               //#of bytes (including this)
            var_dact_data[1] = Stat_Delta;      //command#
            var_dact_data[2] = 3;               //change type (3 = SYSTEM change)
            var_dact_data[3] = 0;               //no number for SYSTEM, so use 0
			var_dact_data[4] = DACTSysStatus;	//bit field for SYSTEM STATUS
			var_dact_data[5] = DACTSysStatus2;	//bit field for SYSTEM STATUS2
            var_dact_data[6] = DACTModeStatus;  //bit field for MODE STATUS

            dialer_comm(WRITE_TO_DIALER, var_dact_data);
		}

        /* do we need to update RLCD with IDC text? */
        if ( updateRLCDTextIDC != MAX_IDC )
        {
            /* if previous text sent to RLCD, request next from DACT */
            if ( !updateRLCDText[0] )
            {
                var_dact_data[0] = 3;
                var_dact_data[1] = IDC_Text_Down;
                var_dact_data[2] = updateRLCDTextIDC;

                if(dialer_comm(READ_FROM_DIALER,var_dact_data) == DIALER_COMM_OK)
				{
                    for(i = 0; i < IDC_TEXT_LENGTH; i++)
					{
                        updateRLCDText[i] = i2c_data_rcv_array[i+1];
					}
                }
            }
            else
            {
                /* send updated text to RLCD...if present */
                if ( status_to_periph(UPDATE_RLCD_IDC_TEXT, updateRLCDTextIDC, 0, 0) == FINISHED )
                {
                    updateRLCDText[0] = 0;
                    updateRLCDTextIDC++;
                }
            }
        }
}
#endif

/*
 *
 **********************************************************
 *
 *   void DogWatch( void ) -
 *       service the watch-dog
 *
 */
void DogWatch(void)
{
    if ( RC0 == 1)
        RC0 = 0;
    else
        RC0 = 1;
}

/*
+------------------------------------------------------------------------------
| Purpose:
|       Initialize Coder data structures
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|       N/A
+------------------------------------------------------------------------------
| Parameters:
|       N/A
+------------------------------------------------------------------------------
| Return Value:
|       N/A
+------------------------------------------------------------------------------
*/
void OMCoderReset( void )
{
    /* Reset coder timer variables */
    omTimersCoderDigitCount = 0;
    omTimersCoderDigitPos   = 0;
    omTimersCoderRoundCount = 0;
    omTimersCoderPulseCount = 0;
    omTimersCoderPulseTimer = 0;
    omCoder                 = 0;

    /* Clear coder queue entries */
    (void)memset( omCoderQueue, 0, sizeof( omCoderQueue ) );
    (void)memset( omCoderIdcActive, 0, sizeof( omCoderIdcActive ) );

    /* Reset coder queue index variables */
    omCoderQueueReadIndex  = 0;
    omCoderQueueWriteIndex = 0;

    /* Reset flag to indicate final inter-round delay */
    omCoderFinalRoundDelay = FALSE;
}

/*
+------------------------------------------------------------------------------
| Purpose:
|       Retrieve next code from coder queue
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|       N/A
+------------------------------------------------------------------------------
| Parameters:
|       N/A
+------------------------------------------------------------------------------
| Return Value:
|       unsigned char - TRUE if code available, FALSE otherwise
+------------------------------------------------------------------------------
*/
static unsigned char OMCoderNextCode( void )
{
    /* Is a new code available ? */
    if ( omCoderQueue[omCoderQueueReadIndex][0] )
    {
        omTimersCoderPulseTimer = TIMER_CODER_DIGIT;
        omTimersCoderRoundCount = TIMER_CODER_MAXROUNDS;

        return TRUE;
    }
    return FALSE;
}

/*
+------------------------------------------------------------------------------
| Purpose:
|       Add code to coder queue
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|       Code dropped if queue full
+------------------------------------------------------------------------------
| Parameters:
|       unsigned char idc_num - IDC id
+------------------------------------------------------------------------------
| Return Value:
|       N/A
+------------------------------------------------------------------------------
*/
void OMCoderPutCode( unsigned char idc_num )
{
    unsigned char coderDigits,
                  digits1_3,
                  digits2_4;

    /* get digits 1 & 2 */
    coderDigits = ext_eeprom_read(eeprom_idc_location[idc_num] + EEPROM_IDC_CODER_DIGIT_1_2_OFFSET);
    digits1_3   = ( coderDigits >> IDC_CODER_DIGIT_1_3_SHIFT ) & IDC_CODER_MASK;

    /* valid code and entry available ? */
    if ( digits1_3 && !omCoderQueue[omCoderQueueWriteIndex][0] )
    {
        digits2_4 = coderDigits & IDC_CODER_MASK;

        omCoderQueue[omCoderQueueWriteIndex][0] = digits1_3;
        omCoderQueue[omCoderQueueWriteIndex][1] = digits2_4;

        /* get digits 3 & 4 */
        coderDigits = ext_eeprom_read(eeprom_idc_location[idc_num] + EEPROM_IDC_CODER_DIGIT_3_4_OFFSET);
        digits1_3   = ( coderDigits >> IDC_CODER_DIGIT_1_3_SHIFT ) & IDC_CODER_MASK;
        digits2_4   = coderDigits & IDC_CODER_MASK;

        omCoderQueue[omCoderQueueWriteIndex][2] = digits1_3;
        omCoderQueue[omCoderQueueWriteIndex][3] = digits2_4;

        /* set active IDC */
        omCoderIdcActive[omCoderQueueWriteIndex] = idc_num + 1;

        if ( ++omCoderQueueWriteIndex >= TIMER_CODER_MAXCODES )
        {
            omCoderQueueWriteIndex = 0;
        }
    }
}

/*
+------------------------------------------------------------------------------
| Purpose:
|       Clear IDC code from coder queue
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|       N/A
+------------------------------------------------------------------------------
| Parameters:
|       unsigned char idc_num - IDC id
+------------------------------------------------------------------------------
| Return Value:
|       N/A
+------------------------------------------------------------------------------
*/
void OMCoderClearCode( unsigned char idc_num )
{
    unsigned char index;

    for ( index = 0; index < TIMER_CODER_MAXCODES; index++ )
    {
        if ( omCoderIdcActive[index] == ( idc_num + 1 ) )
        {
            omCoderIdcActive[index] = 0;
            break;
        }
    }
}
