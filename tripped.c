/*
+------------------------------------------------------------------------------
| Description: tripped.c
| This module handles all annunciation of any event that occurs on an IDC or
| NAC.
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
| 2/25/2007  C. McGill       In process_idc_level_status(), modified to allow Class A
|                            open fault to restore if in walk-test. PTR1149.
| 2/25/2007  C. McGill       In input_tripped_handler(), modified to check for continued
|                            active supervisory state before activating NACs. Non-latching
|                            sup combo may clear before peripherals have been updated,
|                            thus function behaves as if sup still active but now
|                            sup combo indication has been cleared then allowed NACs to
|                            activate briefly. PTR1850. Removed previous edit marks.
| 12/8/2005 C. McGill        In nac_active_handler(), modified to delay supervision
|                            after turning off NAC due to restoration of non-latching
|                            IDC. When NAC configured as Temporal or Coder, can get
|                            brief false fault. In input_tripped_handler(), added
|                            call to new function OMCoderClearCode() to clear coder
|                            entry when non-latching IDC restores. PTR 1503.
| 11/29/05  C. McGill        In process_nac_level_status(), added modification to
|                            NAC_LEVEL_OPEN case. Special case with new hardware
|                            in which if negative side of Class A NAC is opened,
|                            only low NAC will show trouble. For consistency with
|                            all other Class A NAC troubles, force high NAC into
|                            trouble as well.
| 11/17/05  C. McGill        In process_idc_level_status(), corrected condition
|                            when in Class A with odd IDC in trouble, circuit would
|                            toggle in/out of trouble. Condition occurred when IDC
|                            configured for alarm and ~1.5Kohm load applied. Circuit
|                            would detect supervisory activation and leave trouble
|                            state, but since circuit was configured for alarm would
|                            return to trouble state. In process_idc_level_status(),
|                            corrected waterflow delay error. PTR 1495. In
|                            input_tripped_handler(), corrected to send Sup indication
|                            to peripherals in Sup Combo configuration.
| 11/14/05  C. McGill        In process_idc_level_status(), corrected to process
|                            remote drill from IDC supervisory state and remote
|                            signal silence from IDC alarm state. PTR 1490. In
|                            input_tripped_handler(), modified to send zone trouble
|                            and disable state to RA10(C).
| 10/24/05  C. McGill        In process_idc_level_status(), modified to latch class A
|                            open trouble. Corrects case where increasing current load
|                            with positive side of the circuit open can toggle in/out of
|                            trouble.
| 8/14/2005  C. McGill       In nac_active_handler(), corrected to restart all silenced
|                            NACs on new alarm. Resolves issue where panel is taken out of
|                            signal silence by new alarm, but some NACs still silenced because
|                            they are not activated by current IDC.
| 8/12/2005  C. McGill       In nac_active_handler(), moved check for walk test
|                            until after idc has been validated.
| 8/11/2005  C. McGill       Updated clean me function.
| 8/9/2005   C. McGill       Updates for non-latching IDC, correction for walktest.
| 06/12/02  C. McGill        In input_tripped_handler(), modified to refresh
|                             peripherals on new event activation.
| 06/28/02  C. McGill        In input_tripped_handler(), corrected to clear zone
|                             sup LED on START condition, light zone sup LED if
|                             non-walk test activation and return zone to disable
|                             state if in enable hold-off period and disable button
|                             is pressed again. PTR #676.
| 06/29/02  C. McGill        In ProcessDrill(), modified to call nac_active_handler()
|                             with a default of the first IDC. nac_active_handler()
|                             will then determine system is in drill and use the
|                             NAC programmed values when activating the NACs.
|                             In nac_active_handler(), modified to shut down DRILL
|                             NACs not associated with new alarm activation.
|                             PTR #672, PTR #673.
| 07/15/02  C. McGill        In process_idc_level_status(), corrected to latch
|                             supervisory activation. PTR #695.
| 07/26/02  C. McGill        In ProcessReset(), modified to call debounce_idc_levels()
|                             and debounce_nac_levels() to reset respective
|                             state machines.
| 07/29/02  C. McGill        In process_idc_level_status(), corrected to clear
|                             supervisory activation in walktest. Also corrects
|                             lingering sup latch issues. In nac_active_handler(),
|                             corrected to start NACs on walktest indication.
|                             PTR #754.
| 07/30/02  C. McGill        In process_idc_level_status(), modified to check for
|                             verify level activation on circuit configured as
|                             waterflow. PTR #769. In input_tripped_handler(), modified
|                             switch statement so that waterflow is handled with
|                             same timing as alarm to ensure alarm buzzer is
|                             processed correctly for waterflow activation. PTR #772.
| 08/04/02  C. McGill        In input_tripped_handler(), added call to
|                             on_or_off_idc_sup_leds() to handle clearing of
|                             Monitor LED condition. PTR #803. Removed panel
|                             silence state influence over disabled IDC
|                             participation in walktest.
| 08/08/02  C. McGill        In ProcessReset(), removed set of resetTimerSeconds
|                             at the end of reset period. In input_tripped_handler()
|                             and nac_active_handler, removed resetTimerSeconds
|                             based processing. Was supposedly to ensure
|                             active point disable after reset, but disable
|                             survives reset anyway. Was causing ability to clear
|                             new alarm by use of disable. PTR #810, #816, #818.
|                             Removed old edit marks. In nac_active_handler(),
|                             removed reset of global idcNacTrouble. No troubles
|                             are tracked here, and function is called by idc
|                             handler, thus an idc activation would result in
|                             an idc trouble clear inadvertently. PTR #808, #811
|                             In process_idc_level_status(), modified to clear internal
|                             state on reset. In ProcessReset() added calls to
|                             process_idc_level_status() and process_nac_level_status()
|                             to reset their respective internal states. PTR #807, #809
| 08/09/02  C. McGill        In process_idc_level_status(), corrected alarm
|                             verify processing. PTR #806. In input_tripped_handler(),
|                             modified to add verify state processing to light
|                             led if re-enabling zone, thereby alerting user
|                             that zone is active in verify and may go into alarm.
|                             PTR #812, #813, #815, #817.
| 08/12/02  C. McGill        In input_tripped_handler(), modified not to reset
|                             NACs during disable period. PTR #831.
| 08/14/02  C. McGill        In nac_active_handler(), modified to track if
|                             disabled in startup state and if so, when
|                             re-enabled check if NACs are silenced and if so,
|                             go straight to silenced state. PTR #849.
| 08/17/02  C. McGill        In process_idc_level_status(), modified to latch
|                             waterflow indication. PTR #838. In input_tripped_handler(),
|                             modified to check if zone in walktest before
|                             resounding buzzers. PTR #860.
| 08/19/02  C. McGill        In nac_input_handler(), optimized function and modified
|                             to process disable in trouble state. PTR #867.
| 08/23/02  C. McGill        In process_idc_level_status(), corrected to check if
|                             waterflow actually still active at end of retard period.
|                             This is an after effect of latching the waterflow
|                             activation.
| 08/27/02  C. McGill        In input_tripped_handler(), modified to reset NACs
|                             on combo zone transition from supervisory to
|                             waterflow alarm. PTR #869. In process_idc_level_status(),
|                             corrected check for waterflow activation. PTR #884,
|                             #887. In process_idc_level_status(), corrected to
|                             latch combo zone supervisory. PTR #885.
| 08/29/02  C. McGill        Added global dactPresent for use in determining
|                             whether to attempt dact comm and to reduce i2c
|                             traffic.
| 09/03/02  C. McGill        Added global comboDisableDelaySecs[]. In
|                             input_tripped_handler(), modified to use new global
|                             comboDisableDelaySecs[] to process combo zone disable
|                             after supv side activation. PTR #893.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   tripped.c  $
|     $Date:   Jun 18 2007 07:50:12  $
|  $Modtime:   Feb 26 2007 11:40:20  $
| $Revision:   1.184  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\tripped.c-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/

#include "config.h"
#include <pic18.h>
#include "defines.h"
#include "main.h"
#include "externs.h"
#include "tripped.h"

#define VERIFY_4_WIRE_SMOKE     0
//#define USE_TEST_LEDS

//=== Global Variable Declarations (extern statement in file: "externs.h") ========
union IDC_STATUS_BIT_BYTE  idc_status[MAX_IDC];
union IDC_STATUS_BIT_BYTE2 idc_status2[MAX_IDC];
union IDC_STATUS_BIT_BYTE3 idc_status3[MAX_IDC];
union NAC_STATUS_BIT_BYTE  nac_status[MAX_NAC];

volatile unsigned char idc_x_timer[MAX_IDC];  //volatile because timers are dec'd in interrupt
volatile unsigned char alarm_sil_inhibit_timer;
volatile unsigned int  auto_sig_sil_timer = NOT_RUNNING_INT;
volatile unsigned char idcAlarmVerifySecs;
volatile unsigned char idcNonWalkTestVerifActive;
volatile unsigned char resetTimerSeconds;
volatile unsigned char idcDisableDelaySecs[MAX_IDC];
volatile unsigned char comboDisableDelaySecs[MAX_IDC];

unsigned char nonLatchingIDCs[MAX_IDC];

static unsigned char ackAlmSil = 0;
static unsigned char NACSilence = 0;
static unsigned char alarmPresent;
static unsigned char forceNACOn;
static unsigned int  waterFlowActiveIDC = 0;
static unsigned char waterFlowActiveNAC = 0;
static unsigned char clearIDCstatus;
static near unsigned char e2_data;
static near unsigned char idc_ctr;
static bit idcNacTrouble;

unsigned char drillTemporal;
unsigned char DACTIdcEventLatch;
//=================================================================================

//======= Local (to module) prototypes =================================
void on_or_off_idc_sup_leds(unsigned char);
void light_idc_trouble_leds(unsigned char);

void light_or_extinguish_idc_trouble_leds(unsigned char l_or_e, unsigned char idc_ctr);
void light_or_extinguish_nac_trouble_leds(unsigned char l_or_e, unsigned char nac_ctr);
void light_or_extinguish_idc_alarm_leds( unsigned char indexIDC, unsigned char stateLED );
void light_or_extinguish_idc_sup_leds(unsigned char stateLED, unsigned char indexIDC);

void idc_nac_disabled_handler(unsigned char idc_or_nac, unsigned char idc_ctr);

void nac_active_handler(unsigned char idc);
void TurnNACOff( unsigned char indexNAC );
//======================================================================

/**************************************************************************
 ** FUNCTION: ClearDisabledNotification                                  **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function clears trouble notification on IDCs.                   **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ClearDisabledNotification( unsigned char idc )
{
    pulse_2x20_flag = 0;
    latch1.latch1_output_bit.com_trbl = 0;
    latch4.latch4_output_bit.trouble_relay = 0;
    light_or_extinguish_idc_trouble_leds( EXTINGUISH, idc);
    idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 0;
}

/**************************************************************************
 ** FUNCTION: input_tripped_handler                                      **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles annunciation of IDC events.                    **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void input_tripped_handler(void)
{
#if (PANEL_10_ZONE)
    static near unsigned char idc_x_state[MAX_IDC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#elif (PANEL_3_5_ZONE)
    static near unsigned char idc_x_state[MAX_IDC] = {0, 0, 0, 0, 0};
#endif
    unsigned char idcState;
    unsigned char supCombo;
    unsigned char idcDisabled;

    alarmPresent = 0;
    idcNacTrouble = 0;
    waterFlowActiveNAC = 0;
    alarm_pulse_2x20_flag = 0;

    latch1.latch1_output_bit.com_alarm     = 0;
    latch3.latch3_output_bit.alarm_relay   = 0;
    latch1.latch1_output_bit.com_sup       = 0;
    latch3.latch3_output_bit.sup_relay     = 0;
#if (PANEL_3_5_ZONE)
    latch6.latch6_output_bit.com_waterflow = 0;
#elif (PANEL_10_ZONE)
    latch9.latch9_output_bit.com_waterflow = 0;
#endif

    //loop checks each IDC zone
    for (idc_ctr = IDC_1; idc_ctr < MAX_IDC; idc_ctr++)
    {
        /* if in reset, clear IDC status */
        if ( system_state.system_state_bit.reset )
        {
            /* save IDC disabled bit */
            idc_status[idc_ctr].idc_status_byte &= 0b00100000;
            idc_status2[idc_ctr].idc_status_byte2 = 0;
            idc_status3[idc_ctr].idc_status_byte3 = 0;
            idc_x_state[idc_ctr] = START;
            continue;
        }
        idcState = idc_x_state[idc_ctr];
        idcDisabled = idc_status[idc_ctr].idc_status_bit.disabled;
        supCombo    = idc_status2[idc_ctr].idc_status_bit2.sup_combo;

        if ( idcDisabled && idcDisableDelaySecs[idc_ctr] )
        {
            idcState = DISABLED1;
            idcDisableDelaySecs[idc_ctr] = 0;
        }

        if ( idcState == START )
        {
                if ( !idc_status[idc_ctr].idc_status_bit.cross_zoned_pending )
                {
                    /* turn off zone's alarm LED */
                    light_or_extinguish_idc_alarm_leds( idc_ctr, EXTINGUISH );

                    /* turn off zone's sup LED */
                    light_or_extinguish_idc_sup_leds( EXTINGUISH, idc_ctr);
                }

                /* get current state */
                idcState = (idc_status[idc_ctr].idc_status_byte & 0b00111111);

                if ( !idcDisableDelaySecs[idc_ctr] )
                {
                    if ( idcState & 0b00111011 && !idc_status2[idc_ctr].idc_status_bit2.walk_test )
                    {
                        // refresh to force resound of periph buzzers
                        ClearPeripheralStatus(REFRESH_CMN);
                    }

                    /* if new alarm (alarm or waterflow), set so that NACs are turned back ON */
                    if ( idcState & 0b00001011 )
                    {
                        forceNACOn = 1;
                        /* set flag to indicate that non-latching IDCs cannot clear yet (for FACOM/SSU) */
                        DACTIdcEventLatch = 1;
                    }
                }
                else
                {
                    if ( idc_status[idc_ctr].idc_status_bit.verifying || idc_x_timer[idc_ctr] )
                    {
                        idcState = ALARM1;
                    }
                }
        }

        switch ( idcState )
        {
            //WATERFLOW state
            case WATERFLOW1:
            case WATERFLOW_ALARM:
                idcState = ALARM1;

            //ALARM state
            case ALARM1:

                /* turn on zone's alarm LED */
                light_or_extinguish_idc_alarm_leds( idc_ctr, LIGHT );

                /* if this zone is NOT in walk test */
                if ( !idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    /* if zone just enabled, and goes into alarm, hold off for a while */
                    if ( !idcDisableDelaySecs[idc_ctr] )
                    {
                        /* DACT NOT present */
                        if ( !dactPresent )
                        {
                            /* if remote disconnect active, DO NOT activate alarm relay */
                            latch3.latch3_output_bit.alarm_relay = !trouble2.trouble_bit2.remoteDisconnect;
                        }
						else
						{
							latch3.latch3_output_bit.alarm_relay = 1;	//if DACT present, activate alarm relay
						}

                        //turn on commom alarm LED
                        latch1.latch1_output_bit.com_alarm = 1;

                        //turn on common waterflow LED if applicable
                        if ( idc_status[idc_ctr].idc_status_bit.waterflow )
                        {
#if (PANEL_3_5_ZONE)
                            latch6.latch6_output_bit.com_waterflow = 1;
#elif (PANEL_10_ZONE)
                            latch9.latch9_output_bit.com_waterflow = 1;
#endif
                        }

                        // activate appropriate NACs for this IDC
                        // (NOTE: this MUST be done prior to periperals because peripherals take a long
                        //   time to update and the NAC activations are waiting on them in state ALARM2)
                        nac_active_handler( idc_ctr );

                        //send ALARM to peripherals (keep calling function till comm completed)
                        if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_ALARM, 0) == FINISHED )
                        {
                            //send code to coder
                            OMCoderPutCode( idc_ctr );

                            //once comm completed, go to next state
                            idcState = ALARM2;
                        }
                    }
                    else
                    {
                        //restart
                        idcState = START;
                    }
                }
                /* in walk test, watch for alarm to reset */
                else
                {
                    idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 1;
                    /* if alarm and waterflow goes away during walk test mode */
                    if ( !(idc_status[idc_ctr].idc_status_byte & 0b00001001 ) )
                    {
                        idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 0;
                        //restart
                        idcState = START;
                    }
                }
                break;

			case ALARM2:
                /* turn on zone's alarm LED */
                light_or_extinguish_idc_alarm_leds( idc_ctr, LIGHT );

                /* DACT NOT present */
                if ( !dactPresent )
                {
                    /* if remote disconnect active, DO NOT activate alarm relay */
                    latch3.latch3_output_bit.alarm_relay = !trouble2.trouble_bit2.remoteDisconnect;
                }
				else
				{
					latch3.latch3_output_bit.alarm_relay = 1;	//if DACT present, activate alarm relay
				}

                //turn on commom alarm LED
                latch1.latch1_output_bit.com_alarm = 1;

                //turn on common waterflow LED if applicable
                if ( idc_status[idc_ctr].idc_status_bit.waterflow )
                {
#if (PANEL_3_5_ZONE)
                    latch6.latch6_output_bit.com_waterflow = 1;
#elif (PANEL_10_ZONE)
                    latch9.latch9_output_bit.com_waterflow = 1;
#endif
                }

                //activate appropriate NACs for this IDC
                nac_active_handler( idc_ctr );

                if ( idcDisabled )
                {
                        idc_nac_disabled_handler(IDC, idc_ctr);
                        /* flag that IDC was disabled */
                        idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 1;
                }
                /* if IDC was renabled, clear annunciation and flag */
                else if ( idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped )
                {
                    ClearDisabledNotification(idc_ctr);
                }
                else if (!(idc_status[idc_ctr].idc_status_byte & 0b00001001))
                {
                    //send clear to peripherals (keep calling function till comm completed)
                    if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_CLEAR, 0) == FINISHED )
                    {
                        //clear code from coder
                        OMCoderClearCode( idc_ctr );

                        //once comm completed, go to next state
                        idcState = START;
                    }
                }
                break;

            //SUPERVISORY state
            case SUPERVISORY1:

                /* if this zone is NOT in walk test */
                if ( !idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    //if so, turn on appropriate IDC supv LED steady
                    light_or_extinguish_idc_sup_leds( LIGHT, idc_ctr);

                    /* if zone just enabled, and goes into alarm, hold off for a while */
                    if ( !idcDisableDelaySecs[idc_ctr] )
                    {
                        //set bit to light common supervisory LED
                        latch1.latch1_output_bit.com_sup = 1;
                        latch3.latch3_output_bit.sup_relay = 1;

                        /* if combo, let waterflow turn on NACs */
                        if ( !supCombo )
                        {
// BOC CSM 2/25/2007
//                            //activate appropriate NACs for this IDC, even in SUPERVISORY
//                            nac_active_handler( idc_ctr );
                            // verify still active, may have cleared while trying to update peripherals, and should let
                            // peripheral update process compelete. PTR1850.
                            if (idc_status[idc_ctr].idc_status_byte & 0b00001010)
                            {
                                //activate appropriate NACs for this IDC, even in SUPERVISORY
                                nac_active_handler( idc_ctr );
                            }
// EOC CSM 2/25/2007
                        }
                        //send SUPERVISORY to peripherals (keep calling function till comm completed)
                        if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_SUPERVISORY, 0) == FINISHED )
                        {
                            //once comm completed, go to next state
                            idcState = SUPERVISORY2;
                        }
                    }
                    else
                    {
                        //restart
                        idcState = START;
                    }
                }
                /* in walk test, watch for supervisory to reset */
                else
                {
                    idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 1;

                    /* if currently in supervisory state */
                    if ( idc_status[idc_ctr].idc_status_bit.supervisory )
                    {
                        light_or_extinguish_idc_sup_leds( LIGHT, idc_ctr);
                    }
                    else
                    {
                        idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 0;
                        /* turn off sup LED */
                        light_or_extinguish_idc_sup_leds( EXTINGUISH, idc_ctr);
                        idcState = START;
                    }
                }
                break;

            //comm with periphs is completed in previous state
            case SUPERVISORY2:

                //if so, turn on appropriate IDC supv LED steady
                light_or_extinguish_idc_sup_leds( LIGHT, idc_ctr);

                //set bit to light common supervisory LED
                latch1.latch1_output_bit.com_sup   = 1;
                latch3.latch3_output_bit.sup_relay = 1;

                if ( idcDisabled )
                {
                        idc_nac_disabled_handler(IDC, idc_ctr);
                        /* flag that IDC was disabled */
                        idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 1;
                }
                /* if IDC was renabled, clear annunciation and flag */
                else if ( idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped )
                {
                    ClearDisabledNotification(idc_ctr);

                    // if combo zone, handle disable while sup side active
                    if ( supCombo )
                    {
                        /* set up timer to delay going into alarm after zone enabled */
                        comboDisableDelaySecs[idc_ctr] = 30;
                        idc_status[idc_ctr].idc_status_bit.waterflow = 0;
                    }
                }
                else if (!(idc_status[idc_ctr].idc_status_byte & 0b00001010))
                {
                    //send clear to peripherals (keep calling function till comm completed)
                    if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_CLEAR, 0) == FINISHED )
                    {
                        //once comm completed, go to next state
                        idcState = START;
                    }
                    break;
                }

                /* if combo, let waterflow turn on NACs */
                if ( !supCombo )
                {
                    //activate appropriate NACs for this IDC, even in SUPERVISORY
                    nac_active_handler( idc_ctr );
                }

                //see if combo zone, in supv, turned WF
                if ( idc_status[idc_ctr].idc_status_bit.waterflow )
                {
                    if ( !idcDisabled )
                    {
                        if ( comboDisableDelaySecs[idc_ctr] )
                        {
                            /* turn on zone's alarm LED */
                            light_or_extinguish_idc_alarm_leds( idc_ctr, LIGHT );
                        }
                        else
                        {
                            idcState = WATERFLOW1;
                            //  treat as new alarm (waterflow), set so that NACs are turn back ON
                            forceNACOn = 1;
                            // refresh to force resound of periph buzzers
                            ClearPeripheralStatus( REFRESH_CMN );
                        }
                    }
                }
                else
                {
                    //turn off alarm led, maybe on for combo waterflow activation
                    light_or_extinguish_idc_alarm_leds( idc_ctr, EXTINGUISH );
                }
                break;

            //MONITOR state
            case MONITOR1:

                /* if this zone is NOT in walk test */
                if ( !idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    /* if zone just enabled, and goes into alarm, hold off for a while */
                    if ( !idcDisableDelaySecs[idc_ctr] )
                    {
                        //send MONITOR to peripherals (keep calling function till comm completed)
                        if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_MONITOR, 0) == FINISHED )
                        {
                            //once comm completed, go to next state
                            idcState = MONITOR2;
                        }
                    }
                }
                /* in walk test, watch for monitor to reset */
                else
                {
                on_or_off_idc_sup_leds(idc_ctr);
                    idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 1;
                    if ( !idc_status[idc_ctr].idc_status_bit.monitor )
                    {
                        idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 0;
                        /* turn off sup LED */
                        light_or_extinguish_idc_sup_leds( EXTINGUISH, idc_ctr);
                        //once comm completed, go to next state
                        idcState = START;
                    }
                }
                break;

            //MONITOR state
            case MONITOR2:

                /* special clean me monitor state? */
                if ( idc_status3[idc_ctr].idc_status_bit3.cleanMeActive )
                {
                    /* set flag to turn on service detector */
                    alarm_pulse_2x20_flag = 1;
#if (PANEL_3_5_ZONE)
                    light_or_extinguish_idc_sup_leds((latch6.latch6_output_bit.com_service_detector ? LIGHT : EXTINGUISH), idc_ctr);
#elif (PANEL_10_ZONE)
                    light_or_extinguish_idc_sup_leds((latch9.latch9_output_bit.com_service_detector ? LIGHT : EXTINGUISH), idc_ctr);
#endif
                    /* this is a special background case, check if status should be superceded (alarm, sup, wf, trouble)*/
                    if ( idc_status[idc_ctr].idc_status_byte & 0b00011011 )
                    {
                        idcState = MONITOR_CLEAR;
                    }
                }
                else
                {
                    /* annunciate monitor state */
                    on_or_off_idc_sup_leds(idc_ctr);
                }

                /* send peripherals monitor state */
                peripheral_state.peripheral_state_bit.monitor = 1;

                /* if monitor goes away, clear annunciation */
                if ( !idc_status[idc_ctr].idc_status_bit.monitor )
                {
                    peripheral_state.peripheral_state_bit.monitor = 0;
                    idcState = MONITOR_CLEAR;
                }
                /* if disable pressed while monitor active */
                if ( idcDisabled )
                {
                        idc_nac_disabled_handler(IDC, idc_ctr);
                        /* set flag that IDC was disabled */
                        idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 1;
                }
                /* if IDC was renabled, clear annunciation and flag */
                else if ( idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped )
                {
                    ClearDisabledNotification(idc_ctr);
                }
                break;

            case MONITOR_CLEAR:

                idc_status[idc_ctr].idc_status_bit.monitor = 0;
                on_or_off_idc_sup_leds(idc_ctr);

                peripheral_state.peripheral_state_bit.monitor = 0;
                idc_status2[idc_ctr].idc_status_bit2.walk_test_tripped = 0;

                //send SUPERVISORY to peripherals (keep calling function till comm completed)
                if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_CLEAR, 0) == FINISHED )
                {
                    //once comm completed, go to next state
                    idcState = START;

                    if ( idcDisabled )
                    {
                        idcState = DISABLED1;
                    }
                }
                break;

            case TROUBLE1:

                /* if this zone is NOT in walk test */
                if ( !idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    //set flag to light COMMON TROUB LED
                    idcNacTrouble = 1;

                    //activate TROUBLE RELAY
                    //latch4.latch4_output_bit.trouble_relay = 1;

                    //light appropriate IDC TROUBLE LED
                    light_or_extinguish_idc_trouble_leds(LIGHT, idc_ctr);

                    //send TROUBLE to peripherals (keep calling function till comm completed)
                    if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_TROUBLE, 0) == FINISHED )
                    {
                        //once comm completed, go to next state
                        idcState = TROUBLE2;
                    }
                }
                /* zone in walk test and in trouble */
                else
                {
                    /* set bit to annunciate walk test trouble */
                    idc_status2[idc_ctr].idc_status_bit2.walk_test_trouble = 1;

                    //see if TROUBLE condition restored (IDC OPEN is non-latching)
                    if ( !idc_status[idc_ctr].idc_status_bit.trouble )
                    {
                        idc_status2[idc_ctr].idc_status_bit2.walk_test_trouble = 0;
                        idcState = START;
                    }
                }
                break;


           //trouble buzzer is silent in this state
            case TROUBLE2:

                //set flag to start COMMON TROUB LED to flash (20spm)
                idcNacTrouble = 1;

                //activate TROUBLE RELAY
                //latch4.latch4_output_bit.trouble_relay = 1;

                //light appropriate IDC TROUBLE LED
                light_or_extinguish_idc_trouble_leds(LIGHT, idc_ctr);

                if ( !idc_status[idc_ctr].idc_status_bit.trouble )
                {
                    idcState = TROUBLE3;
                }

                //see if DISABLE condition go to disable
                if ( idcDisabled )
                {
                    idcState = DISABLED1;
                }
                break;

            case TROUBLE3:

                //turn off appropriate IDC TROUBLE LED
                light_or_extinguish_idc_trouble_leds(EXTINGUISH, idc_ctr);

                //send TROUBLE to peripherals (keep calling function till comm completed)
                if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_CLEAR, 0) == FINISHED )
                {
                    //once comm completed, go to next state
                    idcState = START;
                }
                break;

            case DISABLED1:

                /* check if NOT currently in walk test */
                if ( !system_state.system_state_bit.walk_test )
                {
                    idc_nac_disabled_handler(IDC, idc_ctr);

                    //send DISABLE to peripherals (keep calling function till comm completed)
                    if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_IDC_DISABLE, 0) == FINISHED )
                    {
                        //once comm completed, go to next state
                        idcState = DISABLED2;
                    }
                }
                /* currently in walk test, do nothing */
                else
                {
                    idcState = START;
                }
                break;

            case DISABLED2:

                /* check if NOT currently in walk test */
                if ( !system_state.system_state_bit.walk_test )
                {
                    idc_nac_disabled_handler(IDC, idc_ctr);

                    /* if IDC status needs to be cleared */
                    if ( clearIDCstatus )
                    {
                        /* send clear for this IDC */
                        if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_CLEAR, 0) == FINISHED )
                        {
                            clearIDCstatus = 0;
                        }
                    }
                    /* handle special conditions during disable */
                    else
                    {
                        //see if DISABLE button pressed (while disabled)
                        if ( !idcDisabled )
                        {
                            //send clear to peripherals (keep calling function till comm completed)
                            if ( status_to_periph(IDC_STAT_CHANGE, idc_ctr, SP_CLEAR, 0) == FINISHED )
                            {
                                //once comm completed, go to next state
                                idcState = DISABLED3;
                            }
                        }
                    }
                }
                /* currently in walk test, do nothing */
                else
                {
                    idcState = START;
                }
                break;

            case DISABLED3:

                /* stop disable annunciation */
                ClearDisabledNotification(idc_ctr);

                /* reset any active status bits */
                idc_status[idc_ctr].idc_status_byte = 0;

                /* idc may have been a monitor, need to clear
                 * special monitor led handling.
                 */
                on_or_off_idc_sup_leds(idc_ctr);

                /* set up timer to delay going into alarm after zone enabled */
                idcDisableDelaySecs[idc_ctr] = 30;

                idcState = START;
                break;

        }//end switch statement
        idc_x_state[idc_ctr] = idcState;
    }//end for loop
    // process NACs that may need to be turned off due to non-latching IDCs
    nac_active_handler( CLEAR_NONLATCHING );

    /* let trouble handler know there is a trouble in the system */
    trouble2.trouble_bit2.idcTrouble = idcNacTrouble;

    /* if alarm silence/drill pressed, Acknowledge button press here */
    /* this so each zone does not have to be silenced individually  */
    if ( ackAlmSil )
    {
        /* clear flag */
        ackAlmSil = 0;

        /* turn off the auto silence timer */
        auto_sig_sil_timer = NOT_RUNNING_INT;

        //clear bit flag
        button.button_bit.alarm_sil_drill = 0;
    }
}//end function

/**************************************************************************
 ** FUNCTION: nac_active_handler                                         **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles annunciation of NAC events, when a NAC becomes **
 ** active.                                                              **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void nac_active_handler(unsigned char idc)
{
#if (PANEL_10_ZONE)
    static near unsigned char nac_x_active_state[MAX_NAC] = {NOT_ACTIVATED, NOT_ACTIVATED, NOT_ACTIVATED, NOT_ACTIVATED};
#elif (PANEL_3_5_ZONE)
    static near unsigned char nac_x_active_state[MAX_NAC] = {NOT_ACTIVATED, NOT_ACTIVATED};
#endif
    static unsigned char numActivation = 0;
    static unsigned char nacSelected = 0;
    unsigned char nac, maskNAC;
    static unsigned char idcWalkTest = 0;

    /* loop through available NACs */
    for ( nac = NAC_1; nac < MAX_NAC; nac++ )
    {
        /* set up bit mask for the NAC we are looking at */
        maskNAC = ( 0x01 << nac );

        /* if in reset, clear NAC status */
        if ( idc == CLEAR_STATUS )
        {
            TurnNACOff( nac );
            nac_x_active_state[nac] = NOT_ACTIVATED;
            numActivation = 0;
            idcWalkTest = 0;
            continue;
        }
        else if ( idc == CLEAR_NONLATCHING )
        {
            //if (!( nacSelected & maskNAC ))
            if ( !( nacSelected & maskNAC ) && !idcWalkTest && !system_state.system_state_bit.drill )
            {
                TurnNACOff( nac );
                if ( nac_x_active_state[nac] != NOT_ACTIVATED )
                {
                    // Delay NAC supervision...otherwise may supervise too soon and get a false fault
                    powerUpSecs = 4;
                    nac_x_active_state[nac] = NOT_ACTIVATED;
                }
            }
            continue;
        }

        if ( idc_status2[idc].idc_status_bit2.walk_test )
        {
            idcWalkTest = 1;
        }

        //what NACs activate for this IDC
        e2_data = ext_eeprom_read(eeprom_idc_location[idc] + EEPROM_IDC_OPTIONS_OFFSET);
        nacSelected |= e2_data & maskNAC;

        /* if this NAC associated with this IDC OR in Drill mode */
        if ( (e2_data & maskNAC) || idcWalkTest || system_state.system_state_bit.drill )
        {
            /* if this IDC is a water flow and it is active, flag as a waterflow NAC */
            if ( waterFlowActiveIDC & ( 0x01 << idc ) )
            {
                waterFlowActiveNAC |= maskNAC;
            }

            /* only restart NACs associated with this IDC when there is a new alarm or in drill */
            if ( forceNACOn )
            {
                // New alarm, cancel drill NACs leaving required NACs on
                alarmPresent = 1;
                if ( system_state.system_state_bit.drill && (!(e2_data & maskNAC)) )
                {
                    TurnNACOff( nac );
                    nac_x_active_state[nac] = NOT_ACTIVATED;
                    numActivation = 0;
                    continue;
                }
                /* restart the state machine */
                nac_x_active_state[nac] = NAC_ACTIVE_TIMER_START;

                /* clear timer so that silence pulses go out now (not at 2 min update) */
                reSynchGen = 1;

                /* clear all currently silenced NACs */
                NACSilence = 0;
            }
        }
        /* NAC NOT associated with IDC, skip processing */
        else
        {
            if ( !forceNACOn || nac_x_active_state[nac] != NAC_SILENCED )
            continue;
        }

        /* get programmed data for current NAC */
        e2_data = ext_eeprom_read(eeprom_nac_location[nac]) & NAC_TYPE_MASK;
        if ( e2_data == CITY_TIE && trouble2.trouble_bit2.remoteDisconnect )
        {
            TurnNACOff( nac );
            nac_x_active_state[nac] = NOT_ACTIVATED;
            continue;
        }

        /* if IDC in walk test, force continuous mode */
        if ( idcWalkTest )
        {
            e2_data = CONTINUOUS;
        }

        switch ( nac_x_active_state[nac] )
        {
            /* do nothing if NAC not active */
            case NOT_ACTIVATED:
                //what NACs activate for this IDC
                e2_data = ext_eeprom_read(eeprom_idc_location[idc] + EEPROM_IDC_OPTIONS_OFFSET);

                if ( (e2_data & maskNAC) || idcWalkTest || system_state.system_state_bit.drill )
                {
                    nac_x_active_state[nac] = NAC_ACTIVE_TIMER_START;
                }
                break;

            case NAC_ACTIVE_TIMER_START:

                e2_data = ext_eeprom_read(EEPROM_NAC_SILENCE_INHIBIT);

                /* is ALARM SILENCE INHIBIT enabled AND the first alarm activation */
                if ( (e2_data & NAC_SILENCE_INHIBIT) && !numActivation )
                {
                    //if so, start ALARM SILENCE INHIBIT TIMER (1 minute)
                    alarm_sil_inhibit_timer = 60;

                    /* timer already started, no more until after reset */
                    numActivation = 1;
                }

                /* advance this NAC to next state */
                nac_x_active_state[nac] = NAC_ACTIVE_START;
                break;

            case NAC_ACTIVE_START:

                //see if NAC DISABLED, SHORTED, Over Current
                if ( !(nac_status[nac].nac_status_byte & 0b00001110) )
                {
                    if ( nac_status[nac].nac_status_bit.was_disabled )
                    {
                        nac_status[nac].nac_status_bit.was_disabled = 0;
                        if ( latch2.latch2_output_bit.silenced )
                        {
                            /* advance this NAC to next state */
                            nac_x_active_state[nac] = NAC_SILENCED;
                            break;
                        }
                    }

                    //turn off COMMON SILENCED LED
                    latch2.latch2_output_bit.silenced = 0;

                    /* this is to clear NAC flags */
                    TurnNACOff( nac+NAC_CLEAR_FLAGS );

                    //start AUTO SIGNAL SILENCE TIMER (>20 minutes)
                    e2_data = ext_eeprom_read( EEPROM_AUTO_NAC_SILENCE ) & AUTO_NAC_SILENCE;

                    if ( e2_data )
                    {
                        auto_sig_sil_timer = e2_data * 60;
                    }

                    /* advance this NAC to next state */
                    nac_x_active_state[nac] = LOOK_FOR_SILENCE;
                }
                else
                {
                    nac_status[nac].nac_status_bit.was_disabled = nac_status[nac].nac_status_bit.disabled;
                }

                break;

            case LOOK_FOR_SILENCE:

                //if active NAC detected in OVER CURRENT condition...shut it off
                if ( nac_status[nac].nac_status_bit.over_current )
                {
                    //turn NAC off
                    TurnNACOff( nac );

					//see if current NAC is programmed CLASS A
                    e2_data = ext_eeprom_read( eeprom_nac_location[nac] );

                    /* if class A, and odd NAC, turn off in pairs */
                    if ( (e2_data & CLASS_A) && !(nac & 0x01) )
                    {
                        TurnNACOff( nac+1 );
                    }

                    /* advance this NAC back to active start */
                    nac_x_active_state[nac] = NAC_ACTIVE_START;
				}
                else
                {
                    if ( e2_data == CONTINUOUS || e2_data == CITY_TIE )
                    {
                        switch ( nac )
                        {
                            case NAC_1:
                                //set NAC1 output to CONTINUOUS
                                continuous_nac1 = 1;
                                break;

                            case NAC_2:
                                //set NAC2 output to CONTINUOUS
                                continuous_nac2 = 1;
                                break;
#if (PANEL_10_ZONE)
                            case NAC_3:
                                //set NAC3 output to CONTINUOUS
                                continuous_nac3 = 1;
                                break;

                            case NAC_4:
                                //set NAC4 output to CONTINUOUS
                                continuous_nac4 = 1;
                                break;
#endif
                        }
                    }

                    if ( e2_data == TEMPORAL )
                    {
                        switch (nac)
                        {
                            case NAC_1:
                                //set NAC1 output to TEMPORAL
                                temporal_nac1 = 1;
                                break;

                            case NAC_2:
                                //set NAC2 output to TEMPORAL
                                temporal_nac2 = 1;
                                break;
#if (PANEL_10_ZONE)
                            case NAC_3:
                                //set NAC3 output to TEMPORAL
                                temporal_nac3 = 1;
                                break;

                            case NAC_4:
                                //set NAC4 output to TEMPORAL
                                temporal_nac4 = 1;
                                break;
#endif
                        }
                    }

                    if ( e2_data == CODER )
                    {
                        switch (nac)
                        {
                            case NAC_1:
                                //set NAC1 output to CODER
                                coder_nac1 = 1;
                                break;

                            case NAC_2:
                                //set NAC2 output to CODER
                                coder_nac2 = 1;
                                break;
#if (PANEL_10_ZONE)
                            case NAC_3:
                                //set NAC3 output to CODER
                                coder_nac3 = 1;
                                break;

                            case NAC_4:
                                //set NAC4 output to CODER
                                coder_nac4 = 1;
                                break;
#endif
                        }
                    }
                    if ( e2_data == GENESIS || e2_data == GENESIS_AV )
                    {
                        switch (nac)
                        {
                            case NAC_1:
                                //set NAC1 output to GENESIS SYNCH
                                genesis_synch_nac1 = 1;

                                /* if NAC1 silenced, resound */
                                if ( genesis_silence_nac1 )
                                {
                                    genesis_turn_on_nac1 = 1;
                                    genesis_silence_nac1 = 0;
                                }
                                break;

                            case NAC_2:
                                //set NAC2 output to GENESIS SYNCH
                                genesis_synch_nac2 = 1;

                                /* if NAC2 silenced, resound */
                                if( genesis_silence_nac2 )
                                {
                                    genesis_turn_on_nac2 = 1;
                                    genesis_silence_nac2 = 0;
                                }
                                break;
#if (PANEL_10_ZONE)
                            case NAC_3:
                                //set NAC3 output to GENESIS SYNCH
                                genesis_synch_nac3 = 1;

                                /* if NAC3 silenced, resound */
                                if ( genesis_silence_nac3 )
                                {
                                    genesis_turn_on_nac3 = 1;
                                    genesis_silence_nac3 = 0;
                                }
                                break;

                            case NAC_4:
                                //set NAC4 output to GENESIS SYNCH
                                genesis_synch_nac4 = 1;

                                /* if NAC4 silenced, resound */
                                if ( genesis_silence_nac4 )
                                {
                                    genesis_turn_on_nac4 = 1;
                                    genesis_silence_nac4 = 0;
                                }
                                break;
#endif
                        }
                    }

                    /* moved here so Walk Test works correctly for a 3/5 zone panel */
                    switch ( nac )
                    {
                        case NAC_1:
                            //activate NAC1 RELAY
                            latch3.latch3_output_bit.nac1 = 1;
                            break;

                        case NAC_2:
                            //activate NAC2 RELAY
                            latch3.latch3_output_bit.nac2 = 1;
                            break;
#if (PANEL_10_ZONE)
                        //activate NAC3 RELAY
                        case NAC_3:
                            latch7.latch7_output_bit.nac3 = 1;
                            break;

                        //activate NAC4 RELAY
                        case NAC_4:
                            latch7.latch7_output_bit.nac4 = 1;
                            break;
#endif
                    }
                }

                /* if panel silenced pressed AND not in Drill AND NAC not proecessed */
                /* OR Auto NAC silence time has gone off, silence current NAC        */
                if ( ( button.button_bit.alarm_sil_drill && !system_state.system_state_bit.drill && !(ackAlmSil & maskNAC) )
                        || !auto_sig_sil_timer || ( NACSilence & maskNAC ) )
                {
                    /* Acknowledge button press later */
                    ackAlmSil |= maskNAC;

                    //only allow silence if ALARM SIL INHIBIT TMR expired
                    if ( !alarm_sil_inhibit_timer )
                    {
                        /* flag NAC to be silenced (for waterflow) */
                        NACSilence |= maskNAC;

                        /* no Waterflow active */
                        if ( !( waterFlowActiveNAC & maskNAC ) )
                        {
                            //get programmed data for current NAC
                            e2_data = ext_eeprom_read(eeprom_nac_location[nac]);

                            //if current NAC programmed as SILENCEABLE
                            if ( e2_data & SILENCEABLE )
                            {
                                //turn on COMMON SILENCED LED
                                latch2.latch2_output_bit.silenced = 1;

                                /* advance this NAC to next state */
                                nac_x_active_state[nac] = NAC_SILENCED;

                                /* set up to clear timer so that silence pulses go out now (not at 2 min update) */
                                /* reSynchGen gets checked every 10 ms, hold off synch for 30 ms (= 3 counts) */
                                reSynchGen = 3;

                                /* clear this NAC's "need to be silenced" flag */
                                NACSilence &= ~maskNAC;
                            }
                        }
                    }
                }
                break;

            case NAC_SILENCED:

                switch ( nac )
                {
                    case NAC_1 :
                        //see if NAC1 programmed as GENESIS type
                        if ( e2_data == GENESIS )
                        {
                            //silence via GENESIS pulses
                            genesis_silence_nac1 = 1;
                        }
                        else
                        {
                            //silence via opening NAC1 relay
                            latch3.latch3_output_bit.nac1 = 0;

                            //shut temporal off just in case (otherwise relay active in int)
                            temporal_nac1 = 0;
                        }
                        break;

                    case NAC_2 :
                        //see if NAC2 programmed as GENESIS type
                        if ( e2_data == GENESIS )
                        {
                            //silence via GENESIS pulses
                            genesis_silence_nac2 = 1;
                        }
                        else
                        {
                            //silence via opening NAC2 relay
                            latch3.latch3_output_bit.nac2 = 0;

                            //shut temporal off just in case (otherwise relay active in int)
                            temporal_nac2 = 0;
                        }
                        break;
#if (PANEL_10_ZONE)
                    case NAC_3 :
                        //see if NAC3 programmed as GENESIS type
                        if ( e2_data == GENESIS )
                        {
                            //silence via GENESIS pulses
                            genesis_silence_nac3 = 1;
                        }
                        else
                        {
                            //silence via opening NAC3 relay
                            latch7.latch7_output_bit.nac3 = 0;

                            //shut temporal off just in case (otherwise relay active in int)
                            temporal_nac3 = 0;
                        }
                        break;

                    case NAC_4 :
                        //see if NAC4 programmed as GENESIS type
                        if ( e2_data == GENESIS )
                        {
                            //silence via GENESIS pulses
                            genesis_silence_nac4 = 1;
                        }
                        else
                        {
                            //silence via opening NAC4 relay
                            latch7.latch7_output_bit.nac4 = 0;

                            //shut temporal off just in case (otherwise relay active in int)
                            temporal_nac4 = 0;
                        }
                        break;
#endif
                }

#if (PANEL_3_5_ZONE)
                /* if possible overcurrent because of AC loss, don't do anything */
                if ( nac_status[nac].nac_status_bit.OC_wait )
                {
                    switch ( nac )
                    {
                        //deactivate NAC1 RELAY
                        case NAC_1:
                            latch3.latch3_output_bit.nac1 = 0;
                            break;

                        //deactivate NAC2 RELAY
                        case NAC_2:

                            latch3.latch3_output_bit.nac2 = 0;
                            break;
                    }
                    break;
                }
                else
                {
                    /* make sure nac ON if Genesis device */
                    if ( e2_data == GENESIS )
                    {
                        switch ( nac )
                        {
                            case NAC_1:
                                //activate NAC1 RELAY
                                latch3.latch3_output_bit.nac1 = 1;
                                break;

                            case NAC_2:
                                //activate NAC2 RELAY
                                latch3.latch3_output_bit.nac2 = 1;
                                break;
                        }
                    }
                }
#endif

                /* if NAC in OC */
                if ( nac_status[nac].nac_status_bit.over_current )
				{
                    /* go back to LOOK_FOR_SILENCE state */
                    nac_x_active_state[nac] = LOOK_FOR_SILENCE;
                }

                /* if alarm silence pressed and NOT acknowledged yet for this NAC */
                /* OR active waterflow associated with this NAC */
                if ( ( button.button_bit.alarm_sil_drill && !( ackAlmSil & maskNAC ) ) || ( waterFlowActiveNAC & maskNAC ) || forceNACOn )
                {
                    /* Acknowledge button press later */
                    ackAlmSil |= maskNAC;

                    //make sure TROUBLE BUZZER is off
                    buzzer_on_flag = 0;

                    /* synch right now */
                    reSynchGen = 1;

                    switch (nac)
                    {
                        case NAC_1:
                            //set NAC1 output to GENESIS SYNCH
                            genesis_turn_on_nac1 = 1;
                            break;

                        case NAC_2:
                            //set NAC2 output to GENESIS SYNCH
                            genesis_turn_on_nac2 = 1;
                            break;
#if (PANEL_10_ZONE)
                        case NAC_3:
                            //set NAC3 output to GENESIS SYNCH
                            genesis_turn_on_nac3 = 1;
                            break;

                        case NAC_4:
                            //set NAC4 output to GENESIS SYNCH
                            genesis_turn_on_nac4 = 1;
                            break;
#endif
                    }

                    /* restart state machine for NAC */
                    nac_x_active_state[nac] = NAC_ACTIVE_START;
                }
                break;
        }// end switch statement
    }// end for loop

    if ( idc == CLEAR_NONLATCHING )
    {
        if ( !nacSelected )
        {
            numActivation = 0;
            //turn off COMMON SILENCED LED
            latch2.latch2_output_bit.silenced = 0;
        }
        nacSelected = 0;
    }

    /* clear for Nac On notification */
    forceNACOn = 0;
}

/**************************************************************************
 ** FUNCTION: nac_input_handler                                          **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles annunciation of NAC trouble events and         **
 ** communication of these events to other functions.                    **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void nac_input_handler(void)
{
#if (PANEL_10_ZONE)
    static near unsigned char nac_x_state[MAX_NAC] = {0, 0, 0, 0};
#elif (PANEL_3_5_ZONE)
    static near unsigned char nac_x_state[MAX_NAC] = {0, 0};
#endif

    unsigned char nac_ctr;
    unsigned char nacState;

    idcNacTrouble = 0;

    for ( nac_ctr = NAC_1; nac_ctr < MAX_NAC ; nac_ctr++ )
    {
        /* if in reset, clear IDC status */
        if ( system_state.system_state_bit.reset )
        {
            nac_status[nac_ctr].nac_status_byte &= 0b00001000;
            nac_x_state[nac_ctr] = NAC_INPUT_START;
            continue;
        }

        nacState = nac_x_state[nac_ctr];
        switch ( nacState )
        {
            case NAC_INPUT_START:

                /* get current state */
                nacState = (nac_status[nac_ctr].nac_status_byte & 0b00001111);
                if ( nacState )
                {
                    // refresh to force resound of periph buzzers
                    ClearPeripheralStatus(REFRESH_CMN);
                }
                break;

            case NAC_INPUT_DISABLED1:

                idc_nac_disabled_handler(NAC, nac_ctr);

                //see if DISABLE button pressed (while disabled)
                if ( !nac_status[nac_ctr].nac_status_bit.disabled )
                {
                    nacState = NAC_INPUT_DISABLED3;
                }
                break;

            case NAC_INPUT_DISABLED3:

                pulse_2x20_flag = 0;
                //latch4.latch4_output_bit.trouble_relay = 0;

                //make sure NAC TROUBLE LEDs off
                light_or_extinguish_nac_trouble_leds(EXTINGUISH, nac_ctr);
                nacState = NAC_INPUT_START;
                break;

            //this state used for all troubles
            case NAC_INPUT_TROUBLE1:
            case NAC_INPUT_SHORTED1:
            case NAC_INPUT_OVER_CURRENT1:

                //set flag to start COMMON TROUB LED to flash (20spm)
                idcNacTrouble = 1;

                //light appropriate NAC TROUBLE LED
                light_or_extinguish_nac_trouble_leds(LIGHT, nac_ctr);

                //see if TROUBLE condition restored
                if ( !( nac_status[nac_ctr].nac_status_byte & 0b00000111 ) )
                {
                    nacState = NAC_INPUT_TROUBLE3;
                }

                //see if DISABLE button pressed
                if ( nac_status[nac_ctr].nac_status_bit.disabled )
                {
                    nacState = NAC_INPUT_DISABLED1;
                }
                break;

            //trouble has restored in this state
            case NAC_INPUT_TROUBLE3:

                //turn off appropriate NAC TROUBLE LED
                light_or_extinguish_nac_trouble_leds(EXTINGUISH, nac_ctr);
                nacState = NAC_INPUT_START;
                break;


        }
        nac_x_state[nac_ctr] = nacState;
    }

    /* let trouble handler know there is a trouble in the system */
    trouble2.trouble_bit2.nacTrouble = idcNacTrouble;
}

/**************************************************************************
 ** FUNCTION: idc_nac_disabled_handler                                   **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles annunciation of an IDC or NAC that has been    **
 ** disabled via the disable button.                                     **
 **                                                                      **
 ** PARAMETERS:  idc_or_nac = IDC or NAC                                 **
 **              index = the IDC or NAC number                           **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void idc_nac_disabled_handler(unsigned char idc_or_nac, unsigned char index)
{
    //turn TROUBLE LED on
    idcNacTrouble = 1;

    //set flag that causes COMMON DISABLE LED to flash (2 x 20 spm)
    pulse_2x20_flag = 1;

    if ( idc_or_nac == IDC )
    {
        //IDC TROUBLE LEDs follow cadence of COMMON DISABLE LED
        light_or_extinguish_idc_trouble_leds( ( latch1.latch1_output_bit.com_disable ? LIGHT : EXTINGUISH), index);
    }
    else
    {
        //NAC TROUBLE LEDs follow cadence of COMMON DISABLE LED
        light_or_extinguish_nac_trouble_leds( ( latch1.latch1_output_bit.com_disable ? LIGHT : EXTINGUISH), index);
    }
}

/**************************************************************************
 ** FUNCTION: light_or_extinguish_idc_alarm_leds                         **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function lights or extinguishes the associated IDC alarm LED    **
 ** according to parameter indexIDC.                                     **
 **                                                                      **
 ** PARAMETERS:  indexIDC = the IDC number                               **
 **              stateLED = LIGHT or EXTINGUISH                          **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void light_or_extinguish_idc_alarm_leds( unsigned char indexIDC, unsigned char stateLED )
{
#ifndef USE_TEST_LEDS
    /* stateLED :     */
    /* LIGHT      = 1 */
    /* EXTINGUISH = 0 */
    switch ( indexIDC )
    {
        case IDC_1:
            latch3.latch3_output_bit.idc1_alarm = stateLED;
            break;

        case IDC_2:
            latch3.latch3_output_bit.idc2_alarm = stateLED;
            break;

        case IDC_3:
            latch3.latch3_output_bit.idc3_alarm = stateLED;
            break;

        case IDC_4:
            latch5.latch5_output_bit.idc4_alarm = stateLED;
            break;

        case IDC_5:
            latch5.latch5_output_bit.idc5_alarm = stateLED;
            break;

#if (PANEL_10_ZONE)
        case IDC_6:
            latch6.latch6_output_bit.idc6_alarm = stateLED;
            break;

        case IDC_7:
            latch6.latch6_output_bit.idc7_alarm = stateLED;
            break;

        case IDC_8:
            latch7.latch7_output_bit.idc8_alarm = stateLED;
            break;

        case IDC_9:
            latch7.latch7_output_bit.idc9_alarm = stateLED;
            break;

        case IDC_10:
            latch8.latch8_output_bit.idc10_alarm = stateLED;
            break;
#endif
    }
#endif
}

/**************************************************************************
 ** FUNCTION: light_or_extinguish_idc_trouble_leds                       **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function lights or extinguishes the associated IDC trouble LED  **
 ** according to parameter indexIDC.                                     **
 **                                                                      **
 ** PARAMETERS:  indexIDC = the IDC number                               **
 **              stateLED = LIGHT or EXTINGUISH                          **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void light_or_extinguish_idc_trouble_leds(unsigned char stateLED, unsigned char indexIDC)
{
#ifndef USE_TEST_LEDS
    //light appropriate IDC TROUBLE LED if "stateLED" parameter set to "LIGHT"
    /* stateLED :     */
    /* LIGHT      = 1 */
    /* EXTINGUISH = 0 */
    switch ( indexIDC )
    {
        case IDC_1:
            latch4.latch4_output_bit.idc1_trbl = stateLED;
            break;

        case IDC_2:
            latch4.latch4_output_bit.idc2_trbl = stateLED;
            break;

        case IDC_3:
            latch4.latch4_output_bit.idc3_trbl = stateLED;
            break;

        case IDC_4:
            latch5.latch5_output_bit.idc4_trbl = stateLED;
            break;

        case IDC_5:
            latch5.latch5_output_bit.idc5_trbl = stateLED;
            break;

#if (PANEL_10_ZONE)
        case IDC_6:
            latch6.latch6_output_bit.idc6_trbl = stateLED;
            break;

        case IDC_7:
            latch6.latch6_output_bit.idc7_trbl = stateLED;
            break;

        case IDC_8:
            latch7.latch7_output_bit.idc8_trbl = stateLED;
            break;

        case IDC_9:
            latch7.latch7_output_bit.idc9_trbl = stateLED;
            break;

        case IDC_10:
            latch8.latch8_output_bit.idc10_trbl = stateLED;
            break;
#endif
    }
#endif
}

/**************************************************************************
 ** FUNCTION: light_or_extinguish_idc_sup_leds                           **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function lights or extinguishes the associated IDC supervisory  **
 ** LED according to parameter indexIDC.                                 **
 **                                                                      **
 ** PARAMETERS:  indexIDC = the IDC number                               **
 **              stateLED = LIGHT or EXTINGUISH                          **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void light_or_extinguish_idc_sup_leds(unsigned char stateLED, unsigned char indexIDC)
{
#ifndef USE_TEST_LEDS
    //light appropriate IDC TROUBLE LED if "stateLED" parameter set to "LIGHT"
    /* stateLED :     */
    /* LIGHT      = 1 */
    /* EXTINGUISH = 0 */
    switch ( indexIDC )
    {
        case IDC_1:
            latch4.latch4_output_bit.idc1_sup = stateLED;
            break;

        case IDC_2:
            latch4.latch4_output_bit.idc2_sup = stateLED;
            break;

        case IDC_3:
            latch4.latch4_output_bit.idc3_sup = stateLED;
            break;

        case IDC_4:
            latch5.latch5_output_bit.idc4_sup = stateLED;
            break;

        case IDC_5:
            latch5.latch5_output_bit.idc5_sup = stateLED;
            break;

#if (PANEL_10_ZONE)
        case IDC_6:
            latch6.latch6_output_bit.idc6_sup = stateLED;
            break;

        case IDC_7:
            latch6.latch6_output_bit.idc7_sup = stateLED;
            break;

        case IDC_8:
            latch7.latch7_output_bit.idc8_sup = stateLED;
            break;

        case IDC_9:
            latch7.latch7_output_bit.idc9_sup = stateLED;
            break;

        case IDC_10:
            latch8.latch8_output_bit.idc10_sup = stateLED;
            break;
#endif
    }
#endif
}

/**************************************************************************
 ** FUNCTION: light_or_extinguish_nac_trouble_leds                       **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function lights or extinguishes the associated NAC trouble LED  **
 ** according to parameter indexNAC.                                     **
 **                                                                      **
 ** PARAMETERS:  indexNAC = the NAC number                               **
 **              stateLED = LIGHT or EXTINGUISH                          **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void light_or_extinguish_nac_trouble_leds(unsigned char stateLED, unsigned char indexNAC)
{
#ifndef USE_TEST_LEDS
    //light appropriate NAC TROUBLE LED if "stateLED" parameter set to "LIGHT"
    switch (indexNAC)
    {
        case NAC_1:
            latch1.latch1_output_bit.nac1_trbl = stateLED;
            break;

        case NAC_2:
            latch1.latch1_output_bit.nac2_trbl = stateLED;
            break;

#if (PANEL_10_ZONE)
        case NAC_3:
            latch8.latch8_output_bit.nac3_trbl = stateLED;
            break;

        case NAC_4:
            latch8.latch8_output_bit.nac4_trbl = stateLED;
            break;
#endif
    }
#endif
}

/**************************************************************************
 ** FUNCTION: on_or_off_idc_sup_leds                                     **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function flashes or extinguishes the associated IDC supervisory **
 ** LED, based on whether supervisory is active or not.                  **
 **                                                                      **
 ** PARAMETERS:  indexIDC = the IDC number                               **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void on_or_off_idc_sup_leds(unsigned char indexIDC)  //called from MONITOR state
{
#ifndef USE_TEST_LEDS
    unsigned char ledState = LIGHT;

    //see if MONITOR condition has restored (MONITOR is non-latching)
    if ( !idc_status[indexIDC].idc_status_bit.monitor )
    {
        //clear flags that flash IDC sup LEDS
        //and clear flags that keep the LEDs on steady (make sure they're off from flashing)
        light_or_extinguish_idc_sup_leds( EXTINGUISH, indexIDC);
        ledState = EXTINGUISH;
    }

    switch ( indexIDC )
    {
        case IDC_1:
            latch4_pulse.latch4_pulse_bit.idc1_sup = ledState;
            break;

        case IDC_2:
            latch4_pulse.latch4_pulse_bit.idc2_sup = ledState;
            break;

        case IDC_3:
            latch4_pulse.latch4_pulse_bit.idc3_sup = ledState;
            break;

        case IDC_4:
            latch5_pulse.latch5_pulse_bit.idc4_sup = ledState;
            break;

        case IDC_5:
            latch5_pulse.latch5_pulse_bit.idc5_sup = ledState;
            break;

#if (PANEL_10_ZONE)
        case IDC_6:
            latch6_pulse.latch6_pulse_bit.idc6_sup = ledState;
            break;

        case IDC_7:
            latch6_pulse.latch6_pulse_bit.idc7_sup = ledState;
            break;

        case IDC_8:
            latch7_pulse.latch7_pulse_bit.idc8_sup = ledState;
            break;

        case IDC_9:
            latch7_pulse.latch7_pulse_bit.idc9_sup = ledState;
            break;

        case IDC_10:
            latch8_pulse.latch8_pulse_bit.idc10_sup = ledState;
            break;
#endif
    }
#endif
}

/**************************************************************************
 ** FUNCTION: nonLatchingIDC                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function determines if IDC meets qualifications for non-        **
 ** latching.                                                            **
 **                                                                      **
 ** PARAMETERS:  idc_num - idc number                                    **
 **                                                                      **
 ** RETURNS:  1 - if idc can be unlatched                                **
 **                                                                      **
 **************************************************************************/
void UpdateNonLatchingIDCs(void)
{
    unsigned char nonLatching, index,
                  panelOpMode = ( ext_eeprom_read(EEPROM_PANEL_OP_MODE) >> PANEL_OPERATING_MODE_SHIFT ) & PANEL_OPERATING_MODE_MASK;

    for ( index = IDC_1; index < MAX_IDC; index++ )
    {
        nonLatching = ext_eeprom_read(eeprom_idc_location[index] + EEPROM_IDC_OPTIONS_OFFSET) & IDC_NONLATCHING;

        // if configured non-latching, allow if panel mode is FACP.  If not FACP, non-latching IDCs cannot clear
        // until DACT has sent event(s)
        if ( nonLatching )
        {
            if ( panelOpMode == PANEL_MODE_FACP )
            {
                nonLatchingIDCs[index] = 1; // Allow outright...
            }
            else
            {
                nonLatchingIDCs[index] = 2; // Depends on DACTIdcEventLatch....
            }
        }
        else
        {
            nonLatchingIDCs[index] = 0;     // Latching.....
        }
    }
}

/**************************************************************************
 ** FUNCTION: nonLatchingIDC                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function determines if IDC meets qualifications for non-        **
 ** latching.                                                            **
 **                                                                      **
 ** PARAMETERS:  idc_num - idc number                                    **
 **                                                                      **
 ** RETURNS:  1 - if idc can be unlatched                                **
 **                                                                      **
 **************************************************************************/
unsigned char nonLatchingIDC(unsigned char idc_num)
{
    // if configured non-latching, allow if panel mode is FACP.  If not FACP, non-latching IDCs cannot clear
    // until DACT has sent event(s)
    if ( idc_status2[idc_num].idc_status_bit2.walk_test )
    {
        return 1;
    }
    else
    {
        if ( nonLatchingIDCs[idc_num] > 1 )
        {
            return ( !DACTIdcEventLatch );
        }
        else
        {
            return ( nonLatchingIDCs[idc_num] );
        }
    }
}

/**************************************************************************
 ** FUNCTION: process_idc_level_status                                   **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function examines the IDC level bit settings and set the        **
 ** appropriate status bit for input_tripped_handler.                    **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void process_idc_level_status(void)
{
//    static bit classACheck;

    //state machine state for each IDC zone
#if (PANEL_10_ZONE)
    static near unsigned char idc_x_state[MAX_IDC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#elif (PANEL_3_5_ZONE)
    static near unsigned char idc_x_state[MAX_IDC] = {0, 0, 0, 0, 0};
#endif
    unsigned char idcState;
    unsigned char cleanMeIDC;
    static unsigned char sysIdcVerifyState = SYS_VERIFY_IDLE;

    waterFlowActiveIDC = 0;

    //
    // Process Alarm Verify State Machine
    //
    switch ( sysIdcVerifyState )
    {
        case SYS_VERIFY_IDLE:
            idcAlarmVerifySecs        = 0;
            idcNonWalkTestVerifActive = 0;
            break;
        case SYS_VERIFY_BEGIN:
            idcAlarmVerifySecs = ALARM_VERIFY_RESET;
            sysIdcVerifyState  = SYS_VERIFY_RESET;
            // turn off IDCs
            latch4.latch4_output_bit.smoke_reset = 1;
            break;
        case SYS_VERIFY_RESET:
            if ( !idcAlarmVerifySecs )
            {
                idcAlarmVerifySecs = ALARM_VERIFY_STARTUP;
                sysIdcVerifyState  = SYS_VERIFY_STARTUP;
                // turn on IDCs
                latch4.latch4_output_bit.smoke_reset = 0;
            }
            break;
        case SYS_VERIFY_STARTUP:
            if ( !idcAlarmVerifySecs )
            {
                idcAlarmVerifySecs = ALARM_VERIFY_CONFIRM;
                sysIdcVerifyState  = SYS_VERIFY_CONFIRM;
            }
            break;
        case SYS_VERIFY_CONFIRM:
            if ( !idcAlarmVerifySecs )
            {
                sysIdcVerifyState  = SYS_VERIFY_IDLE;
            }
            break;
        default:
            break;
    }

    for ( idc_ctr = IDC_1; idc_ctr < MAX_IDC; idc_ctr++ )
    {
        /* if in reset, clear IDC level status */
        if ( system_state.system_state_bit.reset )
        {
            idc_x_state[idc_ctr]      = IDC_LEVEL_START;
            sysIdcVerifyState         = SYS_VERIFY_IDLE;
            continue;
        }
        idcState = idc_x_state[idc_ctr];

        switch ( idcState )
        {
            case IDC_LEVEL_START:

                /* if this zone is in walk test */
                if ( idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    /* clear all status bits (except disable) for restoral */
                    idc_status[idc_ctr].idc_status_byte &= 0b11100000;
                }

				//if an IDC is NOT programmed CLASS A, make sure CLASS A relay is in default position
                e2_data = ext_eeprom_read( eeprom_idc_location[idc_ctr] + EEPROM_IDC_OPTIONS_OFFSET );

                //see if NOT programmed CLASS A
                switch ( idc_ctr )
                {
                    case IDC_1:
                        latch5.latch5_output_bit.classA_1st = 0;
                        if ( !(e2_data & CLASS_A) )
                        {
                            latch5.latch5_output_bit.classA_1st = 1;
                        }
                        break;
                    case IDC_3:
                        latch5.latch5_output_bit.classA_2nd = 0;
                        if ( !(e2_data & CLASS_A) )
                        {
                            latch5.latch5_output_bit.classA_2nd = 1;
                        }
                        break;
#if (PANEL_10_ZONE)
                    case IDC_5:
                        latch8.latch8_output_bit.classA_3rd = 0;
                        if ( !(e2_data & CLASS_A) )
                        {
                            latch8.latch8_output_bit.classA_3rd = 1;
                        }
                        break;
                    case IDC_7:
                        latch8.latch8_output_bit.classA_4th = 0;
                        if ( !(e2_data & CLASS_A) )
                        {
                            latch8.latch8_output_bit.classA_4th = 1;
                        }
                        break;
                    case IDC_9:
                        latch8.latch8_output_bit.classA_5th = 0;
                        if ( !(e2_data & CLASS_A) )
                        {
                            latch8.latch8_output_bit.classA_5th = 1;
                        }
                        break;
#endif
				}
                // process Clean Me as background Normal case
                cleanMeIDC = e2_data & IDC_CLEAN_ME_ENABLED;

                 //check if current IDC zone is in the NON-VERIFIED voltage range
                if ( idc_level[idc_ctr].idc_level_bit.non_ver )
                {
                    //if so, go to valid NON-VERIFIED condition state
                    idcState = IDC_LEVEL_PROGRAMMED;
                }
                //check if current IDC zone is in VERIFIED voltage range (smoke detector trip)
                else if ( idc_level[idc_ctr].idc_level_bit.verified )
                {
                    e2_data = ext_eeprom_read(eeprom_idc_location[idc_ctr]) & IDC_TYPE_MASK;
                    if (e2_data == ALARM_VERIFIED)
					{
	                    //if so, go to VERIFY ALARM condition state(verify smoke det)
                        idcState = IDC_LEVEL_VERIFYING_ALARM;
					}
                    else if (e2_data == SUP_NOT_COMBO)
					{
						//if SUP ZONE (2 wire duct detector maybe) this voltage level is acceptible
                        idcState = IDC_LEVEL_SUPERVISORY_NOT_COMBO;
					}
                    //see if zone programmed for WATERFLOW
                    else if ( e2_data >= WATERFLOW_WR && e2_data <= WATERFLOW_SUP_COMBO )
                    {
                        //go to WATERFLOW1 state
                        idcState = IDC_LEVEL_WATERFLOW1;
                    }
                    //see if zone programmed MONITOR
                    else if ( e2_data == MONITOR )
                    {
                        //go to MONITOR state
                        idcState = IDC_LEVEL_MONITOR;
                    }
                    else if ( e2_data == REMOTE_AC_FAIL )
                    {
                        //go to REMOTE AC FAIL state
                        idcState = IDC_LEVEL_REMOTE_AC_FAIL;
                    }
                    else if ( e2_data == REMOTE_SILENCE_DRILL )
                    {
                        //go to REMOTE SILENCE DRILL state
                        idcState = IDC_LEVEL_REMOTE_SILENCE_DRILL;
                    }
                    else if ( e2_data == CROSS_ZONED_ALARM )
                    {
                        //go to REMOTE SILENCE DRILL state
                        idcState = IDC_LEVEL_CROSS_ZONED_ALARM;
                    }
					else
					{
                        //if above two failed, it's a ALARM or CROSS ZONED ALARM
		                /* alarm active */
		                idc_status[idc_ctr].idc_status_bit.alarm = 1;
                        idcState = IDC_LEVEL_VERIFIED_ALARM;
					}
                }
                //check if current IDC zone is in the SUPERVISORY voltage range
                else if ( idc_level[idc_ctr].idc_level_bit.supv )
                {
                    //go to SUPERVISORY condition state
                    idcState = IDC_LEVEL_SUPERVISORY;
                }
                else if ( idc_level[idc_ctr].idc_level_bit.open )
                {
                    //go to OPEN condition state
                    idcState = IDC_LEVEL_OPEN;
                }
                /* if this IDC in verifying mode */
                else if ( idc_status[idc_ctr].idc_status_bit.verifying )
                {
                    /* and NOT verifying an alarm (IDC_LEVEL_PROGRAMMED might be programmed to verify) */
                    /* hold clearing the verifying bit for ALARM_VERIFY_CONFIRM seconds */
#if (VERIFY_4_WIRE_SMOKE)
                    if ( idcState != IDC_LEVEL_VERIFIED_ALARM && idcState != IDC_LEVEL_PROGRAMMED )
#else
                    if ( !idcAlarmVerifySecs && idcState != IDC_LEVEL_VERIFIED_ALARM )
#endif
                    {
                        /* alarm went away, clear flag */
                        idc_status[idc_ctr].idc_status_bit.verifying = 0;
                    }
                }

                // process Clean Me as background Normal case
                if ( cleanMeIDC )
                {
                    // has validation timer expired?
                    if ( !cleanMeValidationTimer[idc_ctr] )
                    {
                        // have we seen Clean Me initiate sequence?
                        if ( idc_status3[idc_ctr].idc_status_bit3.cleanMeSequence )
                        {
                            if ( !idc_status3[idc_ctr].idc_status_bit3.cleanMeValidate )
                            {
                                idc_status3[idc_ctr].idc_status_bit3.cleanMeValidate = 1;
                            }
                            else
                            {
                                // set to process as special monitor....
                                idc_status3[idc_ctr].idc_status_bit3.cleanMeActive = 1;
                                idc_status[idc_ctr].idc_status_bit.monitor = 1;
                            }
                            cleanMeValidationTimer[idc_ctr]  = CLEAN_ME_VERIFY_TIME;
                            cleanMeRestorationTimer[idc_ctr] = CLEAN_ME_RESTORE_TIME;
                        }
                        else
                        {
                            idc_status3[idc_ctr].idc_status_bit3.cleanMeValidate = 0;
                        }
                        idc_status3[idc_ctr].idc_status_bit3.cleanMeSequence = 0;
                    }

                    // has restoration timer expired?
                    if ( !cleanMeRestorationTimer[idc_ctr] )
                    {
                        // no valid pulses seen, clear clean me status flags
                        idc_status3[idc_ctr].idc_status_bit3.cleanMeActive = 0;
                        idc_status3[idc_ctr].idc_status_bit3.cleanMeValidate = 0;

                        // clear special monitor
                        idc_status[idc_ctr].idc_status_bit.monitor = 0;
                    }
                }
                break;

            //SUPERVISORY condition state (IDC zone within supv voltage range)
            case IDC_LEVEL_SUPERVISORY:
                e2_data = ext_eeprom_read( eeprom_idc_location[idc_ctr] ) & IDC_TYPE_MASK;

                //see if programmed "combo" supv & water flow
                if ( e2_data == WATERFLOW_SUP_COMBO || e2_data == WATERFLOW_WR_SUP_COMBO )
                {
                    //go to SUPERVISORY(combo) state
                    idcState = IDC_LEVEL_SUPERVISORY_COMBO;
                }
                else if ( e2_data == REMOTE_SILENCE_DRILL )
                {
                    //go to REMOTE SILENCE DRILL state
                    idcState = IDC_LEVEL_REMOTE_SILENCE_DRILL;
                }
                else
                {
                    //go back to beginning state
                    idcState = IDC_LEVEL_START;
                }
                break;

            //current IDC zone is in valid non verified alarm voltage range
            //find out how current IDC zone is programmed
            case IDC_LEVEL_PROGRAMMED:

                //pre-load state back to beginning state (overridden if match below)
                idcState = IDC_LEVEL_START;

                e2_data = ext_eeprom_read( eeprom_idc_location[idc_ctr] ) & IDC_TYPE_MASK;

                //see if current zone programmed unverified alarm
                if ( e2_data == ALARM_NONVERIFIED || e2_data == CROSS_ZONED_ALARM )
                {
                    //go to UNVERIFIED ALARM state
                    idcState = IDC_LEVEL_UNVERIFIED_ALARM;
                }

                //see if current zone programmed verified alarm
                if ( e2_data == ALARM_VERIFIED )
                {
#if (VERIFY_4_WIRE_SMOKE)
                    //go to verified UNVERIFIED ALARM state
                    idcState = IDC_LEVEL_VER_UNVER_ALARM;
#else
                    //go to UNVERIFIED ALARM state
                    idcState = IDC_LEVEL_UNVERIFIED_ALARM;
#endif
                }

                //see if zone programmed for WATERFLOW
                if ( e2_data >= WATERFLOW_WR && e2_data <= WATERFLOW_SUP_COMBO )
                {
                    //go to WATERFLOW1 state
                    idcState = IDC_LEVEL_WATERFLOW1;
                }

                //see if zone programmed SUPERVISORY (not in combo with waterflow)
                if ( e2_data == SUP_NOT_COMBO )
                {
                    //go to SUPERVISORY state
                    idcState = IDC_LEVEL_SUPERVISORY_NOT_COMBO;
                }

                //see if zone programmed MONITOR
                if ( e2_data == MONITOR )
                {
                    //go to MONITOR state
                    idcState = IDC_LEVEL_MONITOR;
                }

                if ( e2_data == REMOTE_AC_FAIL )
                {
                    //go to REMOTE AC FAIL state
                    idcState = IDC_LEVEL_REMOTE_AC_FAIL;
                }

                if ( e2_data == REMOTE_SILENCE_DRILL )
                {
                    //go to REMOTE SILENCE DRILL state
                    idcState = IDC_LEVEL_REMOTE_SILENCE_DRILL;
                }

                if ( e2_data == CROSS_ZONED_ALARM )
                {
                    //go to REMOTE SILENCE DRILL state
                    idcState = IDC_LEVEL_CROSS_ZONED_ALARM;
                }
                break;

            case IDC_LEVEL_REMOTE_AC_FAIL:
                if ( idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    idc_status[idc_ctr].idc_status_bit.monitor = 1;
                    idcState = IDC_LEVEL_START;
                }
                else
                {
                    remoteACLoss = 1;
                    trouble.trouble_bit.ac_loss = localACLoss | remoteACLoss;
                    //see if condition restored. may have entered through either the verified or non-verified path
                    if ( !(idc_level[idc_ctr].idc_level_byte & 0b00011000) )
                    {
                        //clear flag if panel not in local ac fail...
                        //if ( nonLatchingIDC(idc_ctr) && latch1.latch1_output_bit.ac_power )
                        if ( nonLatchingIDC(idc_ctr) )
                        {
                            remoteACLoss = 0;
                            trouble.trouble_bit.ac_loss = localACLoss | remoteACLoss;
                        }
                        idcState = IDC_LEVEL_START;
                    }
                }
                break;

            case IDC_LEVEL_REMOTE_SILENCE_DRILL:
                if ( idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    idc_status[idc_ctr].idc_status_bit.monitor = 1;
                    idcState = IDC_LEVEL_START;
                }
                else
                {
                    // if not in alarm or supv state and drill side of silence/drill combo...
                    if ( !(latch1.latch1_output_byte & 0b00010100) && idc_level[idc_ctr].idc_level_bit.supv )
                    {
                        system_state.system_state_bit.drill = 1;
                        button.button_bit.alarm_sil_drill = 1;
                        button_valid_timer = 50;
                    }
                    // if in alarm or supv state and silence side of silence/drill combo...
                    else if ((latch1.latch1_output_byte & 0b00010100) && !idc_level[idc_ctr].idc_level_bit.supv )
                    {
                        button.button_bit.alarm_sil_drill = 1;
                        button_valid_timer = 50;
                    }
                    idcState = IDC_LEVEL_REMOTE_SILENCE_DRILL2;
                }
                break;

            case IDC_LEVEL_REMOTE_SILENCE_DRILL2:
                if ( !(idc_level[idc_ctr].idc_level_byte & 0b00011100) )
                {
                    idcState = IDC_LEVEL_START;
                }
                break;

            case IDC_LEVEL_CROSS_ZONED_ALARM:
                if ( idc_status2[idc_ctr].idc_status_bit2.walk_test )
                {
                    idc_status[idc_ctr].idc_status_bit.alarm = 1;
                    idcState = IDC_LEVEL_START;
                }
                else
                {
                    if ( idc_status2[idc_ctr].idc_status_bit2.walk_test || !idc_status[idc_ctr].idc_status_bit.disabled )
                    {
                        /* turn on zone's alarm LED */
                        light_or_extinguish_idc_alarm_leds( idc_ctr, LIGHT );
                        idc_status[idc_ctr].idc_status_bit.cross_zoned_pending = 1;
                    }

                    e2_data = ext_eeprom_read( eeprom_idc_location[idc_ctr] + EEPROM_IDC_OPTIONS_OFFSET );

                    //see if programmed CLASS A
                    if ( e2_data & CLASS_A )
                    {
                        e2_data = eeprom_idc_class_a_companion[idc_ctr];
                    }
                    else
                    {
                        e2_data = idcCompanion(idc_ctr);
                    }

                    //if cross zoned companion is in alarm, then allow alarm.
                    if ( idc_x_state[e2_data] == IDC_LEVEL_CROSS_ZONED_ALARM )
                    {
                        idc_status[idc_ctr].idc_status_bit.alarm = 1;
                        idc_status[idc_ctr].idc_status_bit.cross_zoned_pending = 0;
                    }

                    if ( !(idc_level[idc_ctr].idc_level_byte & 0b00011000) )
                    {
                        //clear flag
                        if ( nonLatchingIDC(idc_ctr) )
                        {
                            idc_status[idc_ctr].idc_status_bit.alarm = 0;
                            idc_status[idc_ctr].idc_status_bit.cross_zoned_pending = 0;
                            /* turn off zone's alarm LED */
                            light_or_extinguish_idc_alarm_leds( idc_ctr, EXTINGUISH );
                            idcState = IDC_LEVEL_START;
                        }
                    }
                }
                break;

            //UNVERIFIED ALARM state
            case IDC_LEVEL_UNVERIFIED_ALARM:

                idc_status[idc_ctr].idc_status_bit.alarm = 1;

                //see if condition restored
                if ( !idc_level[idc_ctr].idc_level_bit.non_ver )
                {
                    //clear flag
                    if ( nonLatchingIDC(idc_ctr) )
                    {
                        idc_status[idc_ctr].idc_status_bit.alarm = 0;
                        idcState = IDC_LEVEL_START;
                    }
                }
                break;

#if (VERIFY_4_WIRE_SMOKE)
            //wait routine for current IDC zone in UNVERIFIED ALARM condition
            case IDC_LEVEL_VERIFYING_UNVER_ALARM:

                /* if done resetting the smoke */
                if ( !idcAlarmVerifySecs )
                {
                    /* if aux power are turned off */
                    if ( !latch3.latch3_output_bit.aux_power )
                    {
                        /* turn aux power back ON */
                        latch3.latch3_output_bit.aux_power = 1;
                        ignorePeripherals = 0;

                        /* wait for smoke to reset (range: 10 - 60 seconds) */
                        idcAlarmVerifySecs = ALARM_VERIFY_WAIT_SECS;
                    }
                    /* if IDC power already restored, look for alarm again */
                    else
                    {
                        /* wait for alarm to occur again */
                        idcState = IDC_LEVEL_START;
                    }
                }
                break;

            //a VERIFIED UNVERIFIED ALARM state
            case IDC_LEVEL_VER_UNVER_ALARM:

                /* alarm active */
                if ( idc_status[idc_ctr].idc_status_bit.alarm )
                {
                    //see if condition restored
                    if ( !idc_level[idc_ctr].idc_level_bit.non_ver )
                    {
                        //clear flag
                        if ( nonLatchingIDC(idc_ctr) )
                        {
                            idc_status[idc_ctr].idc_status_bit.alarm = 0;
                        idcState = IDC_LEVEL_START;
                        }
                    }
                }
                /* alarm verification process not started */
                else if ( !idc_status[idc_ctr].idc_status_bit.verifying )
                {
                    /* turn off aux power */
                    latch3.latch3_output_bit.aux_power = 0;

                    /* inhibit aux power */
                    auxTimerOC = ALARM_VERIFY_OFF_SECS+1;

                    /* flag that we are verifying an alarm */
                    idc_status[idc_ctr].idc_status_bit.verifying = 1;

                    /* keep IDCs off for ALARM_VERIFY_OFF_SECS seconds */
                    idcAlarmVerifySecs = ALARM_VERIFY_OFF_SECS;

                    /* wait state for verifying an alarm */
                    idcState = IDC_LEVEL_VERIFYING_UNVER_ALARM;

                    idc_level[idc_ctr].idc_level_bit.non_ver = 0;
                }
                /* alarm verification complete, alarm verified */
                else
                {
                    idc_status[idc_ctr].idc_status_bit.alarm = 1;

                    /* flag that we are verifying an alarm */
                    idc_status[idc_ctr].idc_status_bit.verifying = 0;
                }
                break;
#endif

            //wait routine for current IDC zone in VERIFIED ALARM condition
            case IDC_LEVEL_VERIFYING_ALARM:

                e2_data = ext_eeprom_read(eeprom_idc_location[idc_ctr]) & IDC_TYPE_MASK;

                // if unverified alarm OR verified alarm in confirm state, go into alarm
                if ( ( e2_data == ALARM_NONVERIFIED ) ||
                        ( ( sysIdcVerifyState == SYS_VERIFY_CONFIRM ) && ( idc_level[idc_ctr].idc_level_bit.verified ) ) )
                {
                    idcState = IDC_LEVEL_VERIFIED_ALARM;
                    // if walktest point and no real verified points active, reset
                    // timer for next activation.
                    if ( idc_status2[idc_ctr].idc_status_bit2.walk_test && !idcNonWalkTestVerifActive )
                    {
                        sysIdcVerifyState = SYS_VERIFY_IDLE;
                    }
                }
                // else, assume verified alarm
                else
                {
                    // if first verify activation
                    if ( sysIdcVerifyState == SYS_VERIFY_IDLE )
                    {
                        sysIdcVerifyState = SYS_VERIFY_BEGIN;
                    }
                    // If new real (non-walktest) verified alarm, reset timer (may have been started by
                    // walktest point activation
                    else if ( !idc_status2[idc_ctr].idc_status_bit2.walk_test && !idcNonWalkTestVerifActive )
                    {
                        idcNonWalkTestVerifActive = 1;
                        sysIdcVerifyState         = SYS_VERIFY_BEGIN;
                    }
                    // flag that we are verifying an alarm
                    idc_status[idc_ctr].idc_status_bit.verifying = 1;
                    idc_level[idc_ctr].idc_level_bit.verified    = 0;
                    //clear flag
                    idcState = IDC_LEVEL_START;
                }
                break;

            // VERIFIED ALARM state
            case IDC_LEVEL_VERIFIED_ALARM:

                // alarm active
                idc_status[idc_ctr].idc_status_bit.alarm     = 1;
                idc_status[idc_ctr].idc_status_bit.verifying = 0;
                //see if condition restored
                if ( !idc_level[idc_ctr].idc_level_bit.verified )
                {
                    //clear flag
                    if ( nonLatchingIDC(idc_ctr) )
                    {
                        idc_status[idc_ctr].idc_status_bit.alarm = 0;
                    idcState = IDC_LEVEL_START;
                    }
                }
                break;

            //WATERFLOW1 state
            case IDC_LEVEL_WATERFLOW1:

                e2_data = ext_eeprom_read(eeprom_idc_location[idc_ctr]) & IDC_TYPE_MASK;

                //see if either RETARD type programmed
                if ( e2_data == WATERFLOW_WR || e2_data == WATERFLOW_WR_SUP_COMBO )
                {
                    //go to WATERFLOW2 state (start retard timer)
                    idcState = IDC_LEVEL_WATERFLOW2;
                }
                else
                {
                    //go to WATERFLOW4 state (valid tripped WATERFLOW)
                    idcState = IDC_LEVEL_WATERFLOW4;
                }
                break;

            //WATERFLOW2 state (start retard timer)
            case IDC_LEVEL_WATERFLOW2:

                //start this IDC zone's timer with WATERFLOW RETARD time
                e2_data = ext_eeprom_read( EEPROM_WATERFLOW_DELAY ) & WATERFLOW_DELAY;

                if ( e2_data )
                {
                    idc_x_timer[idc_ctr] = e2_data * 5;
                }

                idcState = IDC_LEVEL_WATERFLOW3;
                break;

            //WATERFLOW3 state (wait for retard timer for expiration)
            case IDC_LEVEL_WATERFLOW3:

                if ( !idc_level[idc_ctr].idc_level_bit.non_ver )
                {
                    idcState = IDC_LEVEL_START;
                }
                else if ( !idc_x_timer[idc_ctr] )
                {
                    idcState = IDC_LEVEL_WATERFLOW4;
                }
                break;

            //WATERFLOW4 state (tripped waterflow)
            case IDC_LEVEL_WATERFLOW4:

                if ( !(idc_level[idc_ctr].idc_level_byte & 0b00011000) )
                {
                    //clear flag
                    if ( nonLatchingIDC(idc_ctr) )
                    {
                        idc_status[idc_ctr].idc_status_bit.waterflow = 0;
                    idcState = IDC_LEVEL_START;
                    }
                }
                else
                {
                    idc_status[idc_ctr].idc_status_bit.waterflow = 1;
                    waterFlowActiveIDC |= ( 0x01 << idc_ctr );
                }
                break;

            //SUPERVISORY state (not supv from combo zone)
            case IDC_LEVEL_SUPERVISORY_NOT_COMBO:

                idc_status[idc_ctr].idc_status_bit.supervisory = 1;

                /* check for unverified and verified signals to go away */
                if ( !(idc_level[idc_ctr].idc_level_byte & 0b00011000) )
                {
                    if ( nonLatchingIDC(idc_ctr) )
                    {
                        //clear flag
                        idc_status[idc_ctr].idc_status_bit.supervisory = 0;
                        idcState = IDC_LEVEL_START;
                    }
                }
                break;

            //MONITOR state
            case IDC_LEVEL_MONITOR:

                idc_status[idc_ctr].idc_status_bit.monitor = 1;

                /* check for unverified and verified signals to go away */
                if ( !(idc_level[idc_ctr].idc_level_byte & 0b00011000) )
                {
                    //clear flag
                    idc_status[idc_ctr].idc_status_bit.monitor = 0;
                    idcState = IDC_LEVEL_START;
                }
                break;

            //OPEN state
            case IDC_LEVEL_OPEN:

                e2_data = ext_eeprom_read( eeprom_idc_location[idc_ctr] + EEPROM_IDC_OPTIONS_OFFSET );

                //see if programmed CLASS A
                if ( e2_data & CLASS_A )
                {
                    //possible 2nd half of CLASS A zone pair
                    /* IDCs 2,4,6,8,10 (idc_ctr actual value = 1,3,5,7,9)*/
                    if ( idc_ctr & 0x01 )
                    {
                        //if so, 2nd half of zone pair will always be OPEN
                        idcState = IDC_LEVEL_START;

                        //clear trouble flag, no troubles for even numbered IDCs in class A
                        idc_status[idc_ctr].idc_status_bit.trouble = 0;

                        //don't do anything
                        break;
                    }
                    //possible 1st half of CLASS A zone pair
                    else
                    {
                        //IDC(lower of pair) OPEN & CLASS A...switch appropriate relay for the current IDC
                        switch (idc_ctr)
                        {
                            case IDC_1:
                                latch5.latch5_output_bit.classA_1st = 1;
                                break;

                            case IDC_3:
                                latch5.latch5_output_bit.classA_2nd = 1;
                                break;
#if (PANEL_10_ZONE)
                            case IDC_5:
                                latch8.latch8_output_bit.classA_3rd = 1;
                                break;

                            case IDC_7:
                                latch8.latch8_output_bit.classA_4th = 1;
                                break;

                            case IDC_9:
                                latch8.latch8_output_bit.classA_5th = 1;
                                break;
#endif
                        }
                    }
                }

                idc_status[idc_ctr].idc_status_bit.trouble = 1;

                if ( !idc_level[idc_ctr].idc_level_bit.open )
                {
                    //clear flag
                    if ((!( e2_data & CLASS_A )) || ( idc_level[idc_ctr].idc_level_byte & 0x18 ))
                    {
                        idc_status[idc_ctr].idc_status_bit.trouble = 0;
                        idcState = IDC_LEVEL_START;
                    }
                    else if ( idc_level[idc_ctr].idc_level_bit.supv )
                    {
                        e2_data = ext_eeprom_read( eeprom_idc_location[idc_ctr] ) & IDC_TYPE_MASK;
                        if ( e2_data == WATERFLOW_SUP_COMBO || e2_data == WATERFLOW_WR_SUP_COMBO || e2_data == REMOTE_SILENCE_DRILL )
                        {
                            idc_status[idc_ctr].idc_status_bit.trouble = 0;
                            idcState = IDC_LEVEL_START;
                        }
                    }
                }

// BOC CSM 2/25/2007
                if ( idc_status2[idc_ctr].idc_status_bit2.walk_test_reset )
                {
                    idcState = IDC_LEVEL_START;
                }
// EOC CSM 2/25/2007

                break;

            //SUPERVISORY(combo) state
            case IDC_LEVEL_SUPERVISORY_COMBO:

                idc_status[idc_ctr].idc_status_bit.supervisory = 1;
                idc_status2[idc_ctr].idc_status_bit2.sup_combo = 1;

                if ( !idc_level[idc_ctr].idc_level_bit.supv )
                {
                    if ( nonLatchingIDC(idc_ctr) )
                    {
                        //clear flag
                        idc_status[idc_ctr].idc_status_bit.supervisory = 0;
                        idc_status2[idc_ctr].idc_status_bit2.sup_combo = 0;
                    }
                    idcState = IDC_LEVEL_START;
                }
                break;
        }//end switch statement

        // Special handling for Class A secondary IDC
        if ( idc_ctr & 0x01 )
        {
            //see if programmed CLASS A
            e2_data = ext_eeprom_read( eeprom_idc_location[idc_ctr] + EEPROM_IDC_OPTIONS_OFFSET );

            // If Class A and active, implies Class A circuit was in (open) trouble and now
            //  monitoring on "both" sides. (class A relay opened, now 2 class B circuits)
            //  An activation has come in on the secondary side.
            if ( ( e2_data & CLASS_A ) && ( idc_status[idc_ctr].idc_status_byte & 0x0F ) )
            {
                // If Primary side of Class A circuit is not already active, report secondary side activation
                //  as primary side.
                if ( !( idc_status[idc_ctr - 1].idc_status_byte & 0x0F ) )
                {
                    // Copy activation to primary IDC
                    idc_x_state[idc_ctr - 1] = idcState;
                    idc_status[idc_ctr - 1]  = idc_status[idc_ctr];
                    idc_status2[idc_ctr - 1] = idc_status2[idc_ctr];
                }

                // Restart....
                idcState = IDC_LEVEL_START;

                // Clear active flags, no activations for even numbered IDCs in class A
                idc_status[idc_ctr].idc_status_byte &= 0xF0;
            }
        }
        idc_x_state[idc_ctr] = idcState ;
    }//end for loop
}

/**************************************************************************
 ** FUNCTION: process_nac_level_status                                   **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function examines the NAC level bit settings and set the        **
 ** appropriate status bit for nac_active_handler.                       **
 ** has been restored.                                                   **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void process_nac_level_status(void)
{
#if (PANEL_10_ZONE)
    static near unsigned char nac_level_state[MAX_NAC] = {0, 0, 0, 0};
#elif (PANEL_3_5_ZONE)
    static near unsigned char nac_level_state[MAX_NAC] = {0, 0};
#endif
    unsigned char nac_ctr;
    unsigned char nacState;

    for ( nac_ctr = NAC_1; nac_ctr < MAX_NAC; nac_ctr++ )
    {
        /* if in reset, clear NAC level status */
        if ( system_state.system_state_bit.reset )
        {
            nac_level_state[nac_ctr] = NAC_LEVEL_START;
            continue;
        }

        nacState = nac_level_state[nac_ctr];
        switch ( nacState )
        {
            case NAC_LEVEL_START:
                //do nothing on NORMAL condition
                //go to OPEN condition state
                //go to SHORTED condition state
                //go to OVER CURRENT condition state
                nacState = (nac_level[nac_ctr].nac_level_byte & 0b00001101);
                if ( !nacState )
                {
                    nac_status[nac_ctr].nac_status_byte &= 0b11111000;;
                }
                break;

            //OPEN condition state
            case NAC_LEVEL_OPEN:

                nac_status[nac_ctr].nac_status_bit.trouble = 1;

                //check for restore
                if ( !nac_level[nac_ctr].nac_level_bit.open )
                {
                   nacState = NAC_LEVEL_START;
                }
                // Special case with new hardware in which if negative side of Class A NAC is opened, only low NAC will show
                // trouble. For consistency with all other Class A NAC troubles, force high NAC into trouble as well.
                else if (!( nac_ctr & 0x01 ) && ( ext_eeprom_read( eeprom_nac_location[nac_ctr] ) & CLASS_A ))
                {
                    nac_level_state[nac_ctr + 1] = NAC_LEVEL_OPEN;
                    nac_level[nac_ctr + 1].nac_level_bit.open = 1;
                }

                break;

            //SHORTED condition state
            case NAC_LEVEL_SHORTED:

                nac_status[nac_ctr].nac_status_bit.shorted = 1;

                //check for restore
                if ( !nac_level[nac_ctr].nac_level_bit.shorted )
                {
                    nacState = NAC_LEVEL_START;
                }
                break;

            //OVER CURRENT condition state
            case NAC_LEVEL_OVERCURRENT:

                /* this condition is latching */
				nac_status[nac_ctr].nac_status_bit.over_current = 1;

				break;
        }
        nac_level_state[nac_ctr] = nacState;
    }
}


/**************************************************************************
 ** FUNCTION: TurnNACOff                                                 **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function turns off a specific NAC.                              **
 **                                                                      **
 ** PARAMETERS: NAC index                                                **
 **                                                                      **
 ** RETURNS:  none.                                                      **
 **                                                                      **
 **************************************************************************/
void TurnNACOff( unsigned char indexNAC )
{
    unsigned char updateNac = 1;

    /* see if need to clear flags only */
    if ( indexNAC & NAC_CLEAR_FLAGS )
    {
        updateNac = 0;
        indexNAC -= NAC_CLEAR_FLAGS;
    }

    switch ( indexNAC )
    {
        case NAC_1:

            if ( updateNac )
            {
                //deactivate NAC1 RELAY
                latch3.latch3_output_bit.nac1 = 0;
                genesis_silence_nac1 = 0;
            }

            //clear NAC1 flags
            continuous_nac1 = 0;
            temporal_nac1 = 0;
            genesis_synch_nac1 = 0;
            coder_nac1 = 0;
            break;

        case NAC_2:

            if ( updateNac )
            {
                //deactivate NAC2 RELAY
                latch3.latch3_output_bit.nac2 = 0;
                genesis_silence_nac2 = 0;
            }

            //clear NAC2 flags
            continuous_nac2 = 0;
            temporal_nac2 = 0;
            genesis_synch_nac2 = 0;
            coder_nac2 = 0;
            break;

#if (PANEL_10_ZONE)
        //activate NAC3 RELAY
        case NAC_3:

            if ( updateNac )
            {
                latch7.latch7_output_bit.nac3 = 0;
                genesis_silence_nac3 = 0;
            }

            //clear NAC3 flags
            continuous_nac3 = 0;
            temporal_nac3 = 0;
            genesis_synch_nac3 = 0;
            coder_nac3 = 0;
            break;

        //activate NAC4 RELAY
        case NAC_4:

            if ( updateNac )
            {
                latch7.latch7_output_bit.nac4 = 0;
                genesis_silence_nac4 = 0;
            }

            //clear NAC4 flags
            continuous_nac4 = 0;
            temporal_nac4 = 0;
            genesis_synch_nac4 = 0;
            coder_nac4 = 0;
            break;
#endif
    } /* end of switch */
}

/**************************************************************************
 ** FUNCTION: ProcessReset                                               **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles reset button being pressed.                    **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  unsigned char = RESET_FINISHED - reset complete            **
 **                           RESET_PROCESS - still processing reset     **
 **                                                                      **
 **************************************************************************/
void ProcessReset(void)
{
    static near unsigned char resetState = RESET_START;

    switch ( resetState )
    {
        case RESET_START :

            /* clear reset button pressed */
            button.button_bit.reset =0;

            /* put system into reset button pressed mode */
            system_state.system_state_bit.reset = 1;

            /* blip buzzer */
            buzzer_blip_flag = 1;
            /* stop temporal buzzer */
            drillTemporal = 0;

            pulse_120spm_timer = 0;
            pulse_2x20_timer = 0;
            pulse_2x20_flag = 0;
            RRM_pulse_2x20_flag = 0;
            alarm_pulse_2x20_flag = 0;
            buzzer_on_flag = 0;
            buzzer_enter_flag = 0;
            buzzerSilenced = 0;
            trouble_LED_flag = 0;

            /* go to next state - turn ON all LEDs  */
            resetState++;

            break;

        case RESET_TURN_OFF :

            /* turn ON all LEDs and turn off ALL outputs */
            latch1.latch1_output_byte = 0b11111111;

            latch2.latch2_output_byte &= 0b00111111;
            latch2.latch2_output_byte |= 0b00011111;

            latch3.latch3_output_byte &= 0b11110000;
            latch3.latch3_output_byte |= 0b01110000;

            latch4.latch4_output_byte = 0b01111111;
            latch5.latch5_output_byte = 0b11111100;

            latch1_pulse.latch1_pulse_byte = 0;
            latch2_pulse.latch2_pulse_byte = 0;
            latch4_pulse.latch4_pulse_byte = 0;
            latch5_pulse.latch5_pulse_byte = 0;
            latch6_pulse.latch6_pulse_byte = 0;

#if (PANEL_10_ZONE)
            latch6.latch6_output_byte = 0b11111100;
            latch7.latch7_output_byte = 0b11111100;
            latch8.latch8_output_byte = 0b11111000;
            latch9.latch9_output_byte &= 0b00111111;
            latch9.latch9_output_byte |= 0b00111100;

            latch7_pulse.latch7_pulse_byte = 0;
            latch8_pulse.latch8_pulse_byte = 0;
            latch9_pulse.latch9_pulse_byte = 0;
#else
            latch6.latch6_output_byte &= 0b01111100;
            latch6.latch6_output_byte |= 0b01111000;
#endif

            /* turn off IDCs */
            resetTimerSeconds = RESET_TIMEOUT_RESET;

            latch4.latch4_output_bit.smoke_reset = 1;

            /* turn off aux power */
            latch3.latch3_output_bit.aux_power = 0;

            /* inhibit aux power turn on for more than resetTimerSeconds */
            auxTimerOC = RESET_TIMEOUT_RESET;

            /* clear current command variables */
            ClearCurrentUsartCommand();

            /* go to next state - clear all peripheral's status */
            resetState++;
            break;

        case RESET_UPDATE_PERIPHERALS :
            //send all clear to peripherals (keep calling function till comm completed)
            //if peripherals not working, resetTimerSeconds will get us out of this state
            if ( status_to_periph(CLEAR_STAT, 0, 0, 0) == FINISHED || !resetTimerSeconds )
            {
                /* go to next state - all LEDs ON */
                resetState++;
            }
            break;

        case RESET_TURN_ON_WAIT :

            /* wait for timeout */
            if ( !resetTimerSeconds )
            {
                /* turn IDCs back on */
                latch4.latch4_output_bit.smoke_reset = 0;

                /* keep power and trouble LEDs ON */
                latch1.latch1_output_byte = 0b00001010;
                /* keep aux power ON */
                latch3.latch3_output_byte = 0b10000000;
                ignorePeripherals = 0;

                /* turn on trouble relay (de-energize) */
                latch4.latch4_output_byte = 0b00000001;

                /* turn off all other LEDs */
                latch2.latch2_output_byte = 0;
                latch5.latch5_output_byte = 0;
#if (PANEL_10_ZONE)
                latch6.latch6_output_byte = 0;
                latch7.latch7_output_byte = 0;
                latch8.latch8_output_byte = 0;
                latch9.latch9_output_byte &= 0b00010011;
#else
                latch6.latch6_output_byte &= 0b00100100;
#endif

                /* turn on IDCs */
                resetTimerSeconds = RESET_TIMEOUT_STARTUP;

                /* clear aux power inhibit timer */
                auxTimerOC = NOT_RUNNING;

                /* clear IDC status - special handling during reset */
                debounce_idc_levels();
                process_idc_level_status();
                input_tripped_handler();

                /* clear NAC status - special handling during reset */
                debounce_nac_levels();
                process_nac_level_status();
                nac_input_handler();
                nac_active_handler(CLEAR_STATUS);

                /* clear trouble bits */
                ClearTroubles();

                /* clear peripheral status variables */
                ClearPeripheralStatus(ALL);

                /* clear current command variables */
                ClearCurrentUsartCommand();

                /* go to next state - power and trouble LEDs ON */
                resetState++;
            }
            break;

        case RESET_TROUBLE :
            /* wait for timeout */
            if ( !resetTimerSeconds )
            {
                /* reset coder */
                OMCoderReset();

                /* keep power and trouble LEDs ON */
                latch1.latch1_output_byte = 0b00000010;
                /* turn off trouble relay */
                latch4.latch4_output_byte = 0;

                /* blip buzzer */
                buzzer_blip_flag = 1;

                /* go to next state - all done */
                resetState++;
            }
            break;

        case RESET_DONE :

            /* restart state machine */
            resetState = RESET_START;

            /* delay some processing on powerup */
            powerUpSecs = 1;

            /* put system into normal mode */
            system_state.system_state_byte = 0;
            break;
    }
}

/**************************************************************************
 ** FUNCTION: ProcessDrill                                               **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles Drill state.                                   **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ProcessDrill(void)
{
    static unsigned char stateDrill = DRILL_START;

    switch ( stateDrill )
    {
        case DRILL_START :

            /* acknowledge button press */
            button.button_bit.alarm_sil_drill = 0;

            /* enter drill state */
            system_state.system_state_bit.drill = 1;

            /* go to next state - turn NACs on */
            stateDrill = DRILL_NACS_ON;

        case DRILL_NACS_ON :

            /* wait for drill button press to exit drill state or alarm to be recognized */
            if ( button.button_bit.alarm_sil_drill || alarmPresent )
            {
                /* acknowledge button press */
                button.button_bit.alarm_sil_drill = 0;

                /* if not in alarm, turn off NACs */
                if ( !alarmPresent )
                {
                    /* turn off NACS */
                    nac_active_handler( CLEAR_STATUS );

                    /* stop buzzer */
                    buzzer_on_flag = 0;
                }
                else
                {
                    /* do not stop buzzer */
                    buzzer_on_flag = 1;
                }

                /* exit drill mode */
                system_state.system_state_bit.drill = 0;

                /* return to starting state */
                stateDrill = DRILL_START;
            }
            else if ( !forceNACOn )
            {
                /* turn on NACS */
                nac_active_handler( 0 );
            }
            break;

       default:
            break;
    }
    /* start temporal buzzer (if need to) */
    drillTemporal = stateDrill;

}
