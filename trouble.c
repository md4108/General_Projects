/*
+------------------------------------------------------------------------------
| Description: trouble.c
| This module handles all system trouble annunciation.
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
| 04/16/03  C. McGill        In trouble_handler() corrected reset of variable
|                             that tracks previous trouble state. Was not being
|                             reset until all troubles were cleared, therefore
|                             if a trouble remained in the system, a subsequent
|                             trouble that activated, restored and activated
|                             again would not have been cleared when it restored,
|                             thus the buzzer would not resound when that point
|                             reactivated.  PTR 898. Reversed History w/most
|                             recent change first. Removed old edit marks.
| 08/17/02  C. McGill        In trouble_handler() modified buzzer handling.
| 07/29/02  C. McGill        In ClearTroubles(), modified to clear trouble
|                             silence counter. Off normal condition will be
|                             resounded after reset and counter should start
|                             cleared. PTR #697, #760.
| 07/14/02  C. McGill        In ClearTroubles(), modified not to clear remote
|                             disconnect on reset. PTR #702.
| 06/14/02  C. McGill        In trouble_handler(), modified to refresh
|                             peripherals on new event activation. Corrected
|                             to check alarm and sup state before clearing
|                             buzzer.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   trouble.c  $
|     $Date:   Aug 05 2005 08:12:32  $
|  $Modtime:   Jul 25 2005 20:31:36  $
| $Revision:   1.35  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\trouble.c-arc  $
|   $Author:   MCGILL  $
+------------------------------------------------------------------------------
*/
#include "config.h"
#if (PIC18)
#include <pic18.h>
#else
#include <pic.h>
#endif
#include "defines.h"
#include "main.h"
#include "externs.h"
#include "tripped.h"

//=== Global Variable Declarations (extern statement in file: "externs.h") ========
near union TROUBLE_BIT_BYTE trouble;
near union TROUBLE_BIT_BYTE2 trouble2;
near union DACT_TROUBLE_BIT_BYTE dact_trouble;
unsigned char oldTrouble, oldTrouble2;
unsigned char oldDactTroubs;
// BOC CSM 5/16/2005
unsigned char newTroubleCount = 0;
unsigned char localACLoss = 0, remoteACLoss = 0;
volatile unsigned int ac_delay_timer = NOT_RUNNING_INT;
// EOC CSM 5/16/2005
volatile unsigned int troubleSilenceMinutes = NOT_RUNNING_INT;
//=================================================================================
bit buzzerSilenced = 0;
//=================================================================================

/**************************************************************************
 ** FUNCTION: ClearTroubles                                              **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function clears all system trouble bits.                        **
 **                                                                      **
 ** PARAMETERS:  none.                                                   **
 **                                                                      **
 ** RETURNS:  none.                                                      **
 **                                                                      **
 **************************************************************************/
void ClearTroubles( void )
{
    /* clear all troubles, except for AC power loss */
    trouble.trouble_byte &= 0b00010000;
    /* clear all troubles, except for Remote Disconnect */
    trouble2.trouble_byte2 &= 0b00100000;

	dact_trouble.dact_trouble_byte = 0;

    /* clear old saved trouble variables */
    oldTrouble = 0;
    oldTrouble2 = 0;
	oldDactTroubs = 0;
// BOC CSM 5/16/2005
    newTroubleCount = 0;
// EOC CSM 5/16/2005
    /* stop timer */
    troubleSilenceMinutes = NOT_RUNNING_INT;
}

/**************************************************************************
 ** FUNCTION: trouble_handler                                            **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function set or clears trouble annunciation LEDs based on       **
 ** current system, IDC or NAC trouble status.                           **
 **                                                                      **
 ** PARAMETERS:  none.                                                   **
 **                                                                      **
 ** RETURNS:  none.                                                      **
 **                                                                      **
 **************************************************************************/
unsigned char trouble_handler(void)
{
    unsigned char retValue;
    unsigned char offNormal;

    /* if buzzer turned on somewhere else, let it be on */
    if ( buzzer_on_flag && buzzerSilenced )
    {
        /* let buzzer sound */
        buzzerSilenced = 0;

        /* stop timer */
        troubleSilenceMinutes = NOT_RUNNING_INT;
    }

// BOC CSM 5/16/2005
    // process AC Fail delay
    if ( trouble.trouble_bit.ac_loss )
    {
        if ( ac_delay_timer == NOT_RUNNING_INT )
        {
            // set ac fail delay (in hours)
            ac_delay_timer = ( ext_eeprom_read(EEPROM_AC_FAIL_DELAY) & AC_FAIL_DELAY ) * 3600;
        }
    }

    // clear if ac returns or battery trouble occurs while ac loss
    if ( !trouble.trouble_bit.ac_loss || trouble.trouble_bit.battery || trouble.trouble_bit.batt_placement )
    {
        ac_delay_timer = NOT_RUNNING_INT;
    }

// EOC CSM 5/16/2005
    if ( trouble.trouble_byte || trouble2.trouble_byte2 || dact_trouble.dact_trouble_byte )
	{
        //turn COMMON TROUBLE LED on steady
        trouble_LED_flag = 1;
// BOC CSM 5/16/2005
//        latch4.latch4_output_bit.trouble_relay = 1;
        // process AC Fail delay
        if ( ac_delay_timer == NOT_RUNNING_INT || newTroubleCount > 1 || !ac_delay_timer )
        {
            latch4.latch4_output_bit.trouble_relay = 1;
        }
// EOC CSM 5/16/2005


        /* if new trouble activation, turn on buzzer */
        if ( ( ( trouble.trouble_byte           ^ oldTrouble    ) & trouble.trouble_byte           ) ||
             ( ( trouble2.trouble_byte2         ^ oldTrouble2   ) & trouble2.trouble_byte2         ) ||
             ( ( dact_trouble.dact_trouble_byte ^ oldDactTroubs ) & dact_trouble.dact_trouble_byte ) )
        {
            /* let buzzer sound */
            buzzerSilenced = 0;
            /* stop timer */
            troubleSilenceMinutes = NOT_RUNNING_INT;
            // refresh to force resound of periph buzzers
            ClearPeripheralStatus(REFRESH_CMN);
// BOC CSM 5/16/2005
            // process AC Fail delay
            newTroubleCount++;
// EOC CSM 5/16/2005
// BOC CSM 4/16/2003 - PTR #898
//            // reset tracking variables
//            oldTrouble = trouble.trouble_byte;
//            oldTrouble2 = trouble2.trouble_byte2;
//            oldDactTroubs = dact_trouble.dact_trouble_byte;
// EOC CSM 4/16/2003 - PTR #898
        }

        //signify that trouble handler is not finished
        retValue = 0;
	}
	else
	{
        /* stop timer */
        troubleSilenceMinutes = NOT_RUNNING_INT;

        //turn COMMON TROUBLE LED off
        trouble_LED_flag = 0;
        latch4.latch4_output_bit.trouble_relay = 0;

// BOC CSM 5/16/2005
        // process AC Fail delay
        newTroubleCount = 0;
// EOC CSM 5/16/2005
// BOC CSM 4/16/2003 - PTR #898
//        oldTrouble = 0;
//        oldTrouble2 = 0;
//        oldDactTroubs = 0;
// EOC CSM 4/16/2003 - PTR #898

        //signify that trouble handler is finished
        retValue = 1;
	}
// BOC CSM 4/16/2003 - PTR #898
    // reset tracking variables
    oldTrouble = trouble.trouble_byte;
    oldTrouble2 = trouble2.trouble_byte2;
    oldDactTroubs = dact_trouble.dact_trouble_byte;
// EOC CSM 4/16/2003 - PTR #898

    //look for TROUBLE SILENCE button press
    if ( button.button_bit.trouble_silence )
    {
        buzzerSilenced = 1;
    }
    //check if system is offnormal
    offNormal = 0;
    if ( trouble_LED_flag || latch1.latch1_output_bit.com_alarm || latch1.latch1_output_bit.com_sup )
    {
        offNormal = 1;
    }
    else
    {
        buzzerSilenced = 0;
    }
    /* if not currently in Drill State */
    if ( !drillTemporal )
    {
        buzzer_on_flag = 0;
        /* if trouble needs to be annunciated */
        if ( !buzzerSilenced && offNormal )
        {
            //activate TROUBLE BUZZER
            buzzer_on_flag = 1;
        }
    }

    /* if peripheral supervision or command trouble (peripheral not responding) */
    //turn ANNUNCIATOR TROUBLE LED on
    latch2.latch2_output_bit.ann_trbl = 1;
    /* if no trouble, turn LED off */
    if ( !(trouble.trouble_byte & 0b00000011) )
    {
        //turn ANNUNCIATOR TROUBLE LED off
        latch2.latch2_output_bit.ann_trbl = 0;
    }

    //default - set flag that causes COMMON DISABLE LED to flash (2 x 20 spm)
    RRM_pulse_2x20_flag = 1;
    /* if RRM disabled, also start disable LED blinking */
    if ( !trouble2.trouble_bit2.disabledRRM )
    {
        RRM_pulse_2x20_flag = 0;
    }

    /* if battery NOT there */
    if ( trouble.trouble_bit.batt_placement )
    {
        //turn BATTERY TROUBLE LED on STEADY
        latch2.latch2_output_bit.batt_trbl = 1;

        //overide (shut off) BATT TROUB LED from FLASHING
        latch2_pulse.latch2_pulse_bit.batt_trbl = 0;
    }
    /* if low battery or charger trouble */
    else if ( trouble.trouble_byte & 0b00100100 )
    {
        //solid BATTERY TROUBLE LED, if in alarm
        if ( latch1.latch1_output_bit.com_alarm )
        {
            latch2.latch2_output_bit.batt_trbl = 1;
            //turn off flash BATTERY TROUBLE LED
            latch2_pulse.latch2_pulse_bit.batt_trbl = 0;
        }
        //flash BATTERY TROUBLE LED, if not in alarm
        else
        {
            latch2_pulse.latch2_pulse_bit.batt_trbl = 1;
        }
    }
    /* battery os functioning normally */
    else
    {
        //turn BATTERY TROUBLE LED OFF
        latch2.latch2_output_bit.batt_trbl = 0;
        //overide (shut off) BATT TROUB LED from FLASHING
        latch2_pulse.latch2_pulse_bit.batt_trbl = 0;
    }

    if ( trouble.trouble_bit.gnd_fault )
    {
        //turn GROUND FAULT LED on
        latch2.latch2_output_bit.gnd_fault = 1;
    }
    else
    {
        //turn GROUND FAULT LED off
        latch2.latch2_output_bit.gnd_fault = 0;
    }

// BOC CSM 5/15/2005
//    /* pulse disable LED */
//    alarm_pulse_2x20_flag = 1;
//    /* remote disconnect NOT active */
//    if ( !trouble2.trouble_bit2.remoteDisconnect )
//    {
//        /* DO NOT pulse disable LED (for remote disconnect) */
//        alarm_pulse_2x20_flag = 0;
//    }
    // Set Remote Disconnect LED
#if (PANEL_3_5_ZONE)
    latch6.latch6_output_bit.rem_disc = 0;
    if ( trouble2.trouble_bit2.remoteDisconnect )
    {
        latch6.latch6_output_bit.rem_disc = 1;
    }
#elif (PANEL_10_ZONE)
    latch9.latch9_output_bit.rem_disc = 0;
    if ( trouble2.trouble_bit2.remoteDisconnect )
    {
        latch9.latch9_output_bit.rem_disc = 1;
    }
#endif
// EOC CSM 5/15/2005
    //return whether or not this function is finished yet
    return( retValue );
}
