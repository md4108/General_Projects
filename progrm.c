/*
+------------------------------------------------------------------------------
| Description: progrm.c
| This module contains functions to program  the panel.
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
| 12/8/2005 C. McGill        In update_lcd_bot(), corrected token index for
|                            coder digit entry.
| 11/14/05  C. McGill        In enter_choice(), modified to (re-)set IDC type for
|                            CROSS_ZONED companion. This changes the companion type
|                            when changed from CROSS_ZONED type as well. PTR 1489.
|                            In idcCompanion(), corrected to prevent IDC_3 companion
|                            set for 3-zoned panel only. (was doing it for 5-zone also)
| 9/25/2005  C. McGill       Modified poll_for_peripherals() to write RSI subtype
|                            to EEPROM.
| 8/10/2005  C. McGill       Corrected to pulse latch 9 LED for 10 zone.
| 8/9/2005   C. McGill       Corrected Disable Lockout set. Added new non-latching
|                            IDC update.
| 08/29/02  C. McGill        Added global dactPresent for use in determining
|                            whether to attempt dact comm and to reduce i2c
|                            traffic.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   progrm.c  $
|     $Date:   Dec 08 2005 15:06:34  $
|  $Modtime:   Dec 08 2005 15:00:02  $
| $Revision:   1.76  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\progrm.c-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/

#include "config.h"
#include <pic18.h>
#include "defines.h"
#include "main.h"
#include "externs.h"
#include "progrm.h"
#include "io.h"
#include "string.h"

//=== Global Variable Declarations (extern statement in file: "externs.h") ========
#if (PANEL_10_ZONE)
const unsigned char eeprom_idc_location[MAX_IDC]  = {EEPROM_IDC_1, EEPROM_IDC_2, EEPROM_IDC_3, EEPROM_IDC_4, EEPROM_IDC_5,
                                                     EEPROM_IDC_6, EEPROM_IDC_7, EEPROM_IDC_8, EEPROM_IDC_9, EEPROM_IDC_10};
const unsigned char eeprom_idc_companion[MAX_IDC] = {IDC_2, IDC_1, IDC_4, IDC_3, IDC_6, IDC_5, IDC_8, IDC_7, IDC_10, IDC_9};
const unsigned char eeprom_idc_class_a_companion[MAX_IDC] = {IDC_3, IDC_3, IDC_1, IDC_1, IDC_7, IDC_7, IDC_5, IDC_5, IDC_9, IDC_9};
const unsigned char eeprom_nac_location[MAX_NAC]  = {EEPROM_NAC_1, EEPROM_NAC_2, EEPROM_NAC_3, EEPROM_NAC_4};
const unsigned char eeprom_nac_companion[MAX_NAC] = {NAC_2, NAC_1, NAC_4, NAC_3};
#elif (PANEL_3_5_ZONE)
const unsigned char eeprom_idc_location[MAX_IDC]  = {EEPROM_IDC_1, EEPROM_IDC_2, EEPROM_IDC_3, EEPROM_IDC_4, EEPROM_IDC_5};
const unsigned char eeprom_idc_companion[MAX_IDC] = {IDC_2, IDC_1, IDC_4, IDC_3, IDC_5};
const unsigned char eeprom_idc_class_a_companion[MAX_IDC] = {IDC_3, IDC_3, IDC_1, IDC_1, IDC_5};
const unsigned char eeprom_nac_location[MAX_NAC]  = {EEPROM_NAC_1, EEPROM_NAC_2};
const unsigned char eeprom_nac_companion[MAX_NAC] = {NAC_2, NAC_1};
#endif

unsigned char programCleanUp = 0;
volatile unsigned char programmingSecs;

unsigned char DACT_program_mode_active_flag;

//=================================================================================

//======= Local (to module) prototypes =================================
static void enter_choice(void);
static void display_current_programmed_choice(void);
#if 0
static unsigned char convert_bit_position_to_number(unsigned char bit_field);
static unsigned char convert_number_to_bit_position(unsigned char number);
#endif
static void update_top_row(unsigned char display_number);
static void update_bot_row(unsigned char display_number);
static void update_NAC_display(unsigned char NAC_number);
static void update_lcd_top(unsigned char led_top_row);
static void update_lcd_bot(unsigned char topNum, unsigned char botNum);
static void update_common_top( void );
static void update_common_bottom( void );
//======================================================================

//=== Local (to module) variable declarations =============
static unsigned char led_top_row, led_bot_row;
static unsigned char idc_program_number, nac_program_number;
// BOC CSM 5/14/2005
//static const unsigned char number_program_options[12] = {0, 2, 2, 2, 2, 2, 8, 2, 2, 3, 2, 2};
static const unsigned char number_program_options[NUM_PROGRAM_STEPS] = {0,   // BLANK_DISPLAY
                                                                        2,   // LOAD_DEFAULTS
                                                                        0,   // EDIT_PASSCODE
                                                                        2,   // ENABLE_PASSCODE
                                                                        3,   // PANEL_OPERATING_MODE
                                                                        2,   // NAC_SILENCE_INHIBIT_TIMER
                                                                        15,  // AC_FAIL_DELAY_TIMER
                                                                        30,  // WATERFLOW_DELAY_TIMER
                                                                        4,   // DISABLE_LOCKOUT
                                                                        2,   // FIND_PERIPHERALS
                                                                        2,   // MULTI_RRM_MODE_SELECT
                                                                        11,  // IDC_ZONE_TYPES
                                                                        2,   // IDC_CLASS
                                                                        2,   // IDC_NON_LATCHING
                                                                        2,   // IDC_CLEAN_ME
                                                                        5,   // IDC_RRM_ASSIGNMENT
                                                                        15,  // IDC_CODER_DIGIT1_ASSIGNMENT
                                                                        15,  // IDC_CODER_DIGIT2_ASSIGNMENT
                                                                        15,  // IDC_CODER_DIGIT3_ASSIGNMENT
                                                                        15,  // IDC_CODER_DIGIT4_ASSIGNMENT
                                                                        0,   // IDC_DESCRIPTION
                                                                        6,   // NAC_OUTPUT
                                                                        3,   // NAC_CLASS
                                                                        30,  // AUTO_NAC_SILENCE_TIMER
                                                                        2,   // NAC_SILENCEABLE
                                                                        2};  // PROGRAM_DACT

static const unsigned char program_min_zero_option[NUM_PROGRAM_STEPS] = {0,  // BLANK_DISPLAY
                                                                         0,  // LOAD_DEFAULTS
                                                                         0,  // EDIT_PASSCODE
                                                                         0,  // ENABLE_PASSCODE
                                                                         0,  // PANEL_OPERATING_MODE
                                                                         0,  // NAC_SILENCE_INHIBIT_TIMER
                                                                         1,  // AC_FAIL_DELAY_TIMER
                                                                         1,  // WATERFLOW_DELAY_TIMER
                                                                         0,  // DISABLE_LOCKOUT
                                                                         0,  // FIND_PERIPHERALS
                                                                         0,  // MULTI_RRM_MODE_SELECT
                                                                         0,  // IDC_ZONE_TYPES
                                                                         0,  // IDC_CLASS
                                                                         0,  // IDC_NON_LATCHING
                                                                         0,  // IDC_CLEAN_ME
                                                                         1,  // IDC_RRM_ASSIGNMENT
                                                                         1,  // IDC_CODER_DIGIT1_ASSIGNMENT
                                                                         1,  // IDC_CODER_DIGIT2_ASSIGNMENT
                                                                         1,  // IDC_CODER_DIGIT3_ASSIGNMENT
                                                                         1,  // IDC_CODER_DIGIT4_ASSIGNMENT
                                                                         0,  // IDC_DESCRIPTION
                                                                         0,  // NAC_OUTPUT
                                                                         0,  // NAC_CLASS
                                                                         1,  // AUTO_NAC_SILENCE_TIMER
                                                                         0,  // NAC_SILENCEABLE
                                                                         0}; // PROGRAM_DACT


static const unsigned char tokenValues[NUM_PROGRAM_STEPS] = {            0,  // BLANK_DISPLAY
                                                                         3,  // LOAD_DEFAULTS
                                                                         0,  // EDIT_PASSCODE
                                                                         4,  // ENABLE_PASSCODE
                                                                         6,  // PANEL_OPERATING_MODE
                                                                         16, // NAC_SILENCE_INHIBIT_TIMER
                                                                         19, // AC_FAIL_DELAY_TIMER
                                                                         20, // WATERFLOW_DELAY_TIMER
                                                                         21, // DISABLE_LOCKOUT
                                                                         5,  // FIND_PERIPHERALS
                                                                         10, // MULTI_RRM_MODE_SELECT
                                                                         26, // IDC_ZONE_TYPES
                                                                         40, // IDC_CLASS
                                                                         38, // IDC_NON_LATCHING
                                                                         39, // IDC_CLEAN_ME
                                                                         44, // IDC_RRM_ASSIGNMENT
                                                                         45, // IDC_CODER_DIGIT1_ASSIGNMENT
                                                                         46, // IDC_CODER_DIGIT2_ASSIGNMENT
                                                                         47, // IDC_CODER_DIGIT3_ASSIGNMENT
                                                                         48, // IDC_CODER_DIGIT4_ASSIGNMENT
                                                                          0, // IDC_DESCRIPTION
                                                                         50, // NAC_OUTPUT
                                                                         49, // NAC_CLASS
                                                                         13, // AUTO_NAC_SILENCE_TIMER
                                                                         57, // NAC_SILENCEABLE
                                                                         58};// PROGRAM_DACT
// EOC CSM 5/14/2005
static union NACS_ACTIVATED_BIT_BYTE nacs_activated;

//+++++++ compiler bug fix 8/20/01
static bit enable_poll_for_peripherals;
static unsigned char program_sub_state_save = NOT_RUNNING;
static unsigned char idc_program_number_save;
static unsigned char nac_program_number_save;
static unsigned char led_top_row_save;

//=========================================================

/**************************************************************************
 ** FUNCTION: program                                                    **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles all the steps necessary to program the panel.  **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  PRGM_FINISHED - programming complete                       **
 **           PRGM_STILL_IN_PROGRESS - programming not complete          **
 **                                                                      **
 **************************************************************************/
bit program(void)
{
    static unsigned char program_sub_state;
    static unsigned char periphUpdated, dbCtrIncDone;
    static unsigned char idcTextExit = 0;
    static unsigned char waitForValidPasscode = 0;
    unsigned char index, index2, e2_data, upperLimit,dataBaseCtr, coderPresent;
    unsigned int bitMask;


    //check if PROGRAM jumper removed
    if ( !system_state.system_state_bit.program )
    {
        program_sub_state = PROGRAM_START;
        program_sub_state_save = NOT_RUNNING;

		dbCtrIncDone = 0;	//initialize flag that handles incrementing Database counter only once per prgm session

        programCleanUp = 0;

		if (DACT_program_mode_active_flag)
		{
#if (DACT_CODE)
            dialer_comm(WRITE_TO_DIALER, exit_DACT_program_mode);
#endif
			DACT_program_mode_active_flag = 0;
		}
		else
		{
			 update_lcd_top(BLANK_DISPLAY);
			 update_lcd_bot(BLANK_DISPLAY, BLANK_DISPLAY);
		}
// BOC CSM 5/31/2005

        //
        // Update Class A IDC Addresses
        //
        UpdatePeriphClassAIDCAddress();

        //
        // Update Non-latching IDCs
        //
        UpdateNonLatchingIDCs();
// EOC CSM 5/31/2005
        //signify programming is finished & we should go to NORMAL STATE
        return(PRGM_FINISHED);
    }

    //see if function has been requested to run
    if (enable_poll_for_peripherals)
	{
        //run function until done polling
        if ( poll_for_peripherals(FRONT_PANEL_BUTTON_PRESS) )
		{
            //disable further function calls
            enable_poll_for_peripherals = 0;
		}
		else
		{
            //signify programming not finished yet (this done in case program jumper
           return(PRGM_STILL_IN_PROGRESS);
           //pulled out while in the middle of sniffing for peripherals)
		}
	}

    switch(program_sub_state)
    {
        case PROGRAM_START:
            //command top row of LEDs to flash
            latch1_pulse.latch1_pulse_byte = 0b00011111;

            //clear all flags requesting top row LEDs on steady
            latch1.latch1_output_byte = 0;

            //command bottom row of LEDs to flash
//            latch2_pulse.latch2_pulse_byte = 0b00011111;
            latch2_pulse.latch2_pulse_byte = 0b00011100;
#if (PANEL_3_5_ZONE)
            latch6_pulse.latch6_pulse_byte = 0b01010000;
#elif (PANEL_10_ZONE)
            latch9_pulse.latch9_pulse_byte = 0b00001100;
#endif

            //clear all flags requesting bottom row of LEDs on steady
            latch2.latch2_output_byte = 0;

            /* keep aux power ON */
            latch3.latch3_output_byte = 0b10000000;


            /* turn off all other LEDs */
            latch4.latch4_output_byte = 0;
            latch5.latch5_output_byte = 0;
#if (PANEL_10_ZONE)
            latch6.latch6_output_byte = 0;
            latch7.latch7_output_byte = 0;
            latch8.latch8_output_byte = 0;
#endif

            //activate buzzer
            buzzer_on_flag = 1;

            //activate trouble relay
            latch4.latch4_output_bit.trouble_relay = 1;

			//clear any flags that might interfere with LED indications during programming
            trouble_LED_flag = 1;
			pulse_2x20_flag = 0;
			RRM_pulse_2x20_flag = 0;
			alarm_pulse_2x20_flag = 0;

            program_sub_state = PROGRAM_FIRST;  //set next state active
            periphUpdated = 0;
            waitForValidPasscode = 0;

            //------- initializations -------------
            idc_program_number = IDC_1;
            nac_program_number = NAC_1;
            //DACT_program_mode_active_flag = 0;
            //-------------------------------------

			//auto detect if DACT installed
            //if( dialer_comm(READ_FROM_DIALER, poll_dialer_for_presence) == DIALER_COMM_OK )
#if (DACT_CODE)
            load_poll_for_presence_data();  //loads "var_dact_data" array
            if( dialer_comm(READ_FROM_DIALER, var_dact_data) == DIALER_COMM_OK )
            {
                //if DACT installed, log it in eeprom. It will then be supervised
// BOC CSM 6/4/2005
//                e2_data = ext_eeprom_read(0x00);
//                e2_data |= DACT_PRESENT;
//                ext_eeprom_write(0x00,e2_data);
                e2_data = ext_eeprom_read(EEPROM_DACT_INSTALLED);
                e2_data |= DACT_PRESENT;
                ext_eeprom_write(EEPROM_DACT_INSTALLED,e2_data);
// EOC CSM 6/4/2005
// BOC CSM 8/29/2002
                dactPresent = 1;
// EOC CSM 8/29/2002
            }
            else
#endif
			{
                //DACT not detected it will no longer be supervised, so remove it and clear Passcode Enable from EEPROM.
                // Default panel operating mode to FACP.
// BOC CSM 6/4/2005
//                e2_data = ext_eeprom_read(0x00);
//                e2_data &= 0b00001111;
//                ext_eeprom_write(0x00,e2_data);
                e2_data  = ext_eeprom_read(EEPROM_PANEL_OPTIONS1) & 0b00001111;
                e2_data |= PANEL_MODE_FACP << PANEL_OPERATING_MODE_SHIFT;
                ext_eeprom_write(EEPROM_PANEL_OPTIONS1,e2_data);
// EOC CSM 6/4/2005
// BOC CSM 8/29/2002
                dactPresent = 0;
// EOC CSM 8/29/2002
			}
            break;

        case PROGRAM_FIRST:

            /* send new trouble status to peripherals, to tell them in programming mode */
            if ( !periphUpdated )
            {
                if ( status_to_periph(SYS_STAT_CHANGE, 0, 0b00001000, 0) == FINISHED )
                {
                    /* flag command complete */
                    periphUpdated = 1;
                }
            }

//            if (button.button_bit.remote_disconnect)
//            {
//                button.button_bit.remote_disconnect = 0;
            if (button.button_bit.alarm_sil_drill)
            {
                button.button_bit.alarm_sil_drill = 0;

                //silence buzzer
                buzzer_on_flag = 0;

                //stop top two rows of LEDs from flashing
                latch1_pulse.latch1_pulse_byte = 0;
                latch2_pulse.latch2_pulse_byte = 0;
#if (PANEL_3_5_ZONE)
                latch6_pulse.latch6_pulse_byte = 0;
#elif (PANEL_10_ZONE)
                latch9_pulse.latch9_pulse_byte = 0;
#endif
                trouble_LED_flag = 0;

                if ( program_sub_state_save != NOT_RUNNING )
                {
                    program_sub_state = program_sub_state_save;
                    idc_program_number = idc_program_number_save;
                    nac_program_number = nac_program_number_save;
                    led_top_row = led_top_row_save;
                    program_sub_state_save = NOT_RUNNING;
	                display_current_programmed_choice();
                }
                else
                {
                    led_top_row = LOAD_DEFAULTS;
                    led_bot_row = 1;
                    program_sub_state = PROGRAM_BUTTONS;
                }

                if ( ext_eeprom_read(EEPROM_ENABLE_PASSCODE) & PASSCODE_ENABLED )
                {
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, enter_DACT_program_mode);
#endif
                    DACT_program_mode_active_flag = 1;
                    waitForValidPasscode = 1;
                    update_top_row(0);
                    update_bot_row(0);
                }
                else
                {
                    update_top_row(led_top_row);
                    update_lcd_top(led_top_row);

                    update_bot_row(led_bot_row);
                    update_lcd_bot(led_top_row, led_bot_row);
                }

                periphUpdated = 0;
            }

            break;

        case PROGRAM_BUTTONS:
            /* silence trouble */
            if ( !periphUpdated )
            {
                if ( status_to_periph(TROUBLE_SILENCE, 0, 0, 0) == FINISHED )
                {
                    /* flag command complete */
                    periphUpdated = 1;
                }
            }

            /* If passcode enabled and valid passcode entered, DACT will return control */
            if ( waitForValidPasscode && !DACT_program_mode_active_flag )
            {
                waitForValidPasscode = 0;

                update_top_row(led_top_row);
                update_lcd_top(led_top_row);

                update_bot_row(led_bot_row);
                update_lcd_bot(led_top_row, led_bot_row);
            }

/*			if (DACT_program_mode_active_flag)
			{
                //if( //dialer_comm(READ_FROM_DIALER, poll_dialer_for_presence) == //dialer_comm_OK )
				load_poll_for_presence_data();	//loads "var_dact_data" array
                if( //dialer_comm(READ_FROM_DIALER, var_dact_data) == //dialer_comm_OK )
				{
					if(commands_from_dact.commands_from_dact_bit.dact_has_data_for_panel)
					{
                        if( //dialer_comm(READ_FROM_DIALER, request_data_from_dact) == //dialer_comm_OK )
						{
							if(commands_from_dact.commands_from_dact_bit.end_dact_program_mode)
							{
								DACT_program_mode_active_flag = 0;
							}
							if(commands_from_dact.commands_from_dact_bit.blip_enter_buzzer)
							{
							    //command buzzer to emit long ("enter") sound
							    buzzer_enter_flag = 1;
							}
						}
					}
				}

			}
*/
            // if control passed to DACT for programming IDC text, panel tracks if queue up/down key pressed,
            // which will cause DACT to exit IDC text programming mode. panel will need to advance/decrement
            // panel program accordingly
            if ( ( led_top_row == IDC_DESCRIPTION || led_top_row == EDIT_PASSCODE ) && !DACT_program_mode_active_flag )
            {
                if ( idcTextExit == PROGRAM_ADVANCE_TOP )
                {
                    button.button_bit.alarm_sil_drill = 1;
                }
                else if ( idcTextExit == PROGRAM_DECREMENT_TOP )
                {
                    button.button_bit.remote_disconnect = 1;
                }
                idcTextExit = 0;
            }

            //check for ADVANCE TOP
//            if (button.button_bit.remote_disconnect)
//            {
//                button.button_bit.remote_disconnect = 0;
            if (button.button_bit.alarm_sil_drill)
            {
                button.button_bit.alarm_sil_drill = 0;
                if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_ADVANCE_TOP);
#endif

                    // if control passed to DACT for programming IDC text, need to track if queue up/down key pressed.
                    // this will cause DACT to exit IDC text programming mode and panel will need to advance/decrement
                    // panel program accordingly
                    if ( led_top_row == IDC_DESCRIPTION || led_top_row == EDIT_PASSCODE )
                    {
                        idcTextExit = PROGRAM_ADVANCE_TOP;
                    }
				}
				else
				{
                    upperLimit = NAC_SILENCEABLE;
// BOC CSM 8/29/2002
//                    if (ext_eeprom_read(0x00) & 0b10000000)
                    if ( dactPresent )
// EOC CSM 8/29/2002
                    {
                        upperLimit = PROGRAM_DACT;
                    }

                    if ( ++led_top_row > upperLimit )
	                {
                        led_top_row = LOAD_DEFAULTS;
	                }

// BOC CSM 5/18/2005
                    if ( led_top_row == EDIT_PASSCODE )
                    {
                        // If passcode not enabled, then skip...
                        if (!( ext_eeprom_read(EEPROM_ENABLE_PASSCODE) & PASSCODE_ENABLED ))
                        {
                            led_top_row++;
                        }
                    }

                    // If DACT not present, then skip...
                    if ( !dactPresent )
                    {
                        while ( led_top_row == ENABLE_PASSCODE || led_top_row == PANEL_OPERATING_MODE )
                        {
                            led_top_row++;
                        }
                    }

                    if ( led_top_row == MULTI_RRM_MODE_SELECT )
                    {
                        // If no Matrix RRMs are present, then skip...
                        if ( !(ext_eeprom_read(EEPROM_PERIPHERALS_RRM) & 0b01000100))
                        {
                            led_top_row++;
                        }
                    }

#if ( PANEL_3_5_ZONE )
                    if ( !five_zone_panel && led_top_row == IDC_CLASS )
                    {
                        led_top_row++;
                    }
#endif

                    if ( led_top_row == IDC_RRM_ASSIGNMENT )
                    {
                        // If no Matrix RRMs are present or Matrix RRM Mode is Common Alarm, then skip...
                        if ( !(ext_eeprom_read(EEPROM_PERIPHERALS_RRM) & 0b01000100) || rrmCommonAlarm )
                        {
                            led_top_row++;
                        }
                    }

                    if ( led_top_row == IDC_CODER_DIGIT1_ASSIGNMENT )
                    {
                        coderPresent = 0;
                        for ( index = NAC_1; index < MAX_NAC; index++ )
                        {
                            if (( ext_eeprom_read(eeprom_nac_location[index]) & NAC_TYPE_MASK ) == CODER )
                            {
                                coderPresent = 1;
                            }
                        }

                        if ( !coderPresent )
                        {
                            led_top_row += 4;
                        }
                    }

                    // If DACT not present, then skip...
                    if ( !dactPresent )
                    {
                        if ( led_top_row == IDC_DESCRIPTION )
                        {
                            led_top_row++;
                        }
                    }

#if ( PANEL_3_5_ZONE )
                    if ( !five_zone_panel && led_top_row == NAC_CLASS )
                    {
                        led_top_row++;
                    }
#endif
                    update_common_top();
//                    update_top_row(led_top_row);
//                    update_lcd_top(led_top_row);
//                    display_current_programmed_choice();
// EOC CSM 5/18/2005
				}
            }

            //check for DECREMENT TOP
//            else if (button.button_bit.walk_test)
//            {
//                button.button_bit.walk_test = 0;
            else if (button.button_bit.remote_disconnect)
            {
                button.button_bit.remote_disconnect = 0;
                if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_DECREMENT_TOP);
#endif

                    // if control passed to DACT for programming IDC text, need to track if queue up/down key pressed.
                    // this will cause DACT to exit IDC text programming mode and panel will need to advance/decrement
                    // panel program accordingly
                    if ( led_top_row == IDC_DESCRIPTION || led_top_row == EDIT_PASSCODE )
                    {
                        idcTextExit = PROGRAM_DECREMENT_TOP;
                    }
				}
				else
				{
                    if ( --led_top_row < LOAD_DEFAULTS )
	                {
                        led_top_row = NAC_SILENCEABLE;
// BOC CSM 8/29/2002
//                        if (ext_eeprom_read(0x00) & 0b10000000)
                        if ( dactPresent )
// EOC CSM 8/29/2002
                        {
                            led_top_row = PROGRAM_DACT;
                        }
	                }

#if ( PANEL_3_5_ZONE )
                    if ( !five_zone_panel && led_top_row == NAC_CLASS  )
                    {
                        led_top_row --;
                    }
#endif
// BOC CSM 5/18/2005
                    // If DACT not present, then skip...
                    if ( !dactPresent )
                    {
                        if ( led_top_row == IDC_DESCRIPTION )
                        {
                            led_top_row--;
                        }
                    }

                    if ( led_top_row == IDC_CODER_DIGIT4_ASSIGNMENT )
                    {
                        coderPresent = 0;
                        for ( index = NAC_1; index < MAX_NAC; index++ )
                        {
                            if (( ext_eeprom_read(eeprom_nac_location[index]) & NAC_TYPE_MASK ) == CODER )
                            {
                                coderPresent = 1;
                            }
                        }

                        if ( !coderPresent )
                        {
                            led_top_row -= 4;
                        }
                    }

                    if ( led_top_row == IDC_RRM_ASSIGNMENT )
                    {
                        // If no Matrix RRMs are present or Matrix RRM Mode is Common Alarm, then skip...
                        if ( !(ext_eeprom_read(EEPROM_PERIPHERALS_RRM) & 0b01000100) || rrmCommonAlarm )
                        {
                            led_top_row--;
                        }
                    }

#if ( PANEL_3_5_ZONE )
                    if ( !five_zone_panel && led_top_row == IDC_CLASS  )
                    {
                        led_top_row --;
                    }
#endif

                    if ( led_top_row == MULTI_RRM_MODE_SELECT )
                    {
                        // If no Matrix RRMs are present, then skip...
                        if ( !(ext_eeprom_read(EEPROM_PERIPHERALS_RRM) & 0b01000100))
                        {
                            led_top_row--;
                        }
                    }

                    // If DACT not present, then skip...
                    if ( !dactPresent )
                    {
                        while ( led_top_row == ENABLE_PASSCODE || led_top_row == PANEL_OPERATING_MODE )
                        {
                            led_top_row--;
                        }
                    }

                    if ( led_top_row == EDIT_PASSCODE )
                    {
                        // If passcode not enabled, then skip...
                        if (!( ext_eeprom_read(EEPROM_ENABLE_PASSCODE) & PASSCODE_ENABLED ))
                        {
                            led_top_row--;
                        }
                    }

                    update_common_top();
//                    update_top_row(led_top_row);
//                    update_lcd_top(led_top_row);
//                    display_current_programmed_choice();
// EOC CSM 5/18/2005
				}
            }

            //check for ADVANCE BOTTOM
//            else if (button.button_bit.reset)
//            {
//                button.button_bit.reset = 0;
            else if (button.button_bit.trouble_silence)
            {
                button.button_bit.trouble_silence = 0;
                if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_ADVANCE_BOTTOM);
#endif
				}
				else
				{
                    if ( ++led_bot_row > number_program_options[led_top_row] )
	                {
	                    led_bot_row = 1;
                        if ( program_min_zero_option[led_top_row] )
                        {
                            led_bot_row = 0;
                        }
	                }
                    update_common_bottom();
                }
            }

            //check for DECREMENT BOTTOM
//            else if (button.button_bit.alarm_sil_drill)
//            {
//                button.button_bit.alarm_sil_drill = 0;
            else if (button.button_bit.walk_test)
            {
                button.button_bit.walk_test = 0;
                if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_DECREMENT_BOTTOM);
#endif
				}
				else
                {
                    if ( program_min_zero_option[led_top_row] )
                    {
                        if ( led_bot_row )
                        {
                            led_bot_row--;
                        }
                        else
                        {
                            led_bot_row = number_program_options[led_top_row];
                        }
                    }
                    else
                    {
                        if ( --led_bot_row < 1 )
                        {
                            led_bot_row = number_program_options[led_top_row];
                        }
                    }
                    update_common_bottom();
                }
            }

            //check for ENTER
//            else if (button.button_bit.trouble_silence)
//            {
//                button.button_bit.trouble_silence = 0;
            else if (button.button_bit.reset)
            {
                button.button_bit.reset = 0;
                if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    if( dialer_comm(WRITE_TO_DIALER, key_press_ENTER) != DIALER_COMM_ERROR)
                    {
                        buzzer_enter_flag = 1;
                    }
#endif
				}
				else
                {
                    enter_choice();
					if(!dbCtrIncDone)	//see if Database counter should be incremented 1 time per prgm session
					{
                        dataBaseCtr = ext_eeprom_read(EEPROM_DATABASE_VERSION);
						dataBaseCtr++;
                        ext_eeprom_write(EEPROM_DATABASE_VERSION, dataBaseCtr);
						dbCtrIncDone = 1;
					}
                }
            }

            for ( index = IDC_1; index < MAX_IDC; index++ )
            {
                bitMask = (1 << (index+IDC_BIT_OFFSET));
#if (PANEL_10_ZONE)
                /* IDC 1-9 in button.button_bit */
                if ( index < IDC_10 )
                {
#endif
                    if ( button.button_int & bitMask )
                    {
                        button.button_int &= ~bitMask;
		                if (DACT_program_mode_active_flag)
						{
#if (DACT_CODE)
							switch(index)
							{
								case IDC_1:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC1_DISABLE);
									break;
								case IDC_2:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC2_DISABLE);
									break;
								case IDC_3:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC3_DISABLE);
									break;
								case IDC_4:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC4_DISABLE);
									break;
								case IDC_5:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC5_DISABLE);
									break;
								case IDC_6:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC6_DISABLE);
									break;
								case IDC_7:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC7_DISABLE);
									break;
								case IDC_8:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC8_DISABLE);
									break;
								case IDC_9:
                                    dialer_comm(WRITE_TO_DIALER, key_press_IDC9_DISABLE);
									break;
							}
#endif

                            // if control passed to DACT for programming IDC text, need to track which IDC is being programmed
                            // for front panel display purposes (keep consistent with panel programming of IDC elements)
                            if ( led_top_row == IDC_DESCRIPTION )
                            {
                                idc_program_number = index;
                                for ( index2 = IDC_1; index2 < MAX_IDC; index2++ )
                                {
                                    light_or_extinguish_idc_trouble_leds(EXTINGUISH, index2);
                                }
                                light_or_extinguish_idc_trouble_leds(LIGHT, idc_program_number);
                            }
						}
						else
		                {
	                        idc_program_number = index;
	                        display_current_programmed_choice();
						}
	                    break;

                    }
#if (PANEL_10_ZONE)
                }
                /* IDC 10 in button2.button_bit2 */
                else
                {
                    if ( button2.button_int2 & 0x0001 )
                    {
                        button2.button_int2 &= 0xfffe;
		                if (DACT_program_mode_active_flag)
						{
#if (DACT_CODE)
                            dialer_comm(WRITE_TO_DIALER, key_press_IDC10_DISABLE);
#endif

                            // if control passed to DACT for programming IDC text, need to track which IDC is being programmed
                            // for front panel display purposes (keep consistent with panel programming of IDC elements)
                            if ( led_top_row == IDC_DESCRIPTION )
                            {
                                idc_program_number = index;
                                for ( index2 = IDC_1; index2 < MAX_IDC; index2++ )
                                {
                                    light_or_extinguish_idc_trouble_leds(EXTINGUISH, index2);
                                }
                                light_or_extinguish_idc_trouble_leds(LIGHT, idc_program_number);
                            }
						}
						else
						{
	                        idc_program_number = index;
	                        display_current_programmed_choice();
						}
	                    break;
                    }
                }
#endif
            }

#if (PANEL_10_ZONE)
            if (button2.button_bit2.nac_1_disable)
            {
                button2.button_bit2.nac_1_disable = 0;
				if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_NAC1_DISABLE);
#endif
				}
				else
				{
	                //see if we're currently programming NACs that activate
	                if (led_top_row == IDC_ZONE_TYPES)
	                {
	                    nacs_activated.nacs_activated_bit.nac1 = !nacs_activated.nacs_activated_bit.nac1;
	                    //dummy parameter passed
	                    update_NAC_display(PRGM_NO_NAC);
	                }
	                //else we're programming the info for the NAC itself
	                else
	                {
	                    nac_program_number = NAC_1;
	                    display_current_programmed_choice();
	                }
				}
            }
#elif (PANEL_3_5_ZONE)
            if (button.button_bit.nac_1_disable)
            {
                button.button_bit.nac_1_disable = 0;
				if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_NAC1_DISABLE);
#endif
				}
				else
				{
	                //see if we're currently programming NACs that activate
	                if(led_top_row == IDC_ZONE_TYPES)
	                {
	                    nacs_activated.nacs_activated_bit.nac1 = !nacs_activated.nacs_activated_bit.nac1;
	                    //dummy parameter passed
	                    update_NAC_display(PRGM_NO_NAC);
	                }
	                //else we're programming the info for the NAC itself
	                else
	                {
	                    nac_program_number = NAC_1;
	                    display_current_programmed_choice();
	                }
				}
            }
#endif

#if (PANEL_10_ZONE)
            else if (button2.button_bit2.nac_2_disable)
            {
                button2.button_bit2.nac_2_disable = 0;
				if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_NAC2_DISABLE);
#endif
				}
				else
				{
	                //see if we're currently programming NACs that activate
	                if (led_top_row == IDC_ZONE_TYPES)
	                {
	                    nacs_activated.nacs_activated_bit.nac2 = !nacs_activated.nacs_activated_bit.nac2;
	                    //dummy parameter passed
	                    update_NAC_display(PRGM_NO_NAC);
	                }
	                //else we're programming the info for the NAC itself
	                else
	                {
	                    nac_program_number = NAC_2;
	                    display_current_programmed_choice();
	                }
				}
            }
#elif (PANEL_3_5_ZONE)
            else if (button.button_bit.nac_2_disable)
            {
                button.button_bit.nac_2_disable = 0;
				if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_NAC2_DISABLE);
#endif
				}
				else
				{
	                //see if we're currently programming NACs that activate
	                if (led_top_row == IDC_ZONE_TYPES)
	                {
	                    nacs_activated.nacs_activated_bit.nac2 = !nacs_activated.nacs_activated_bit.nac2;
	                    //dummy parameter passed
	                    update_NAC_display(PRGM_NO_NAC);
	                }
	                //else we're programming the info for the NAC itself
	                else
	                {
	                    nac_program_number = NAC_2;
	                    display_current_programmed_choice();
	                }
				}
            }
#endif

#if (PANEL_10_ZONE)
            else if (button2.button_bit2.nac_3_disable)
            {
                button2.button_bit2.nac_3_disable = 0;
				if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_NAC3_DISABLE);
#endif
				}
				else
				{
	                //see if we're currently programming NACs that activate
	                if (led_top_row == IDC_ZONE_TYPES)
	                {
	                    nacs_activated.nacs_activated_bit.nac3 = !nacs_activated.nacs_activated_bit.nac3;
	                    //dummy parameter passed
	                    update_NAC_display(PRGM_NO_NAC);
	                }
	                //else we're programming the info for the NAC itself
	                else
	                {
	                    nac_program_number = NAC_3;
	                    display_current_programmed_choice();
	                }
				}
            }

            else if (button2.button_bit2.nac_4_disable)
            {
                button2.button_bit2.nac_4_disable = 0;
				if (DACT_program_mode_active_flag)
				{
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, key_press_NAC4_DISABLE);
#endif
				}
				else
				{
	                //see if we're currently programming NACs that activate
	                if (led_top_row == IDC_ZONE_TYPES)
	                {
	                    nacs_activated.nacs_activated_bit.nac4 = !nacs_activated.nacs_activated_bit.nac4;
	                    //dummy parameter passed
	                    update_NAC_display(PRGM_NO_NAC);
	                }
	                //else we're programming the info for the NAC itself
	                else
	                {
	                    nac_program_number = NAC_4;
	                    display_current_programmed_choice();
	                }
				}
            }
#endif

    }//end switch statement

    /* return to program LED blink state during inactivity */
    if ( !programmingSecs && program_sub_state != PROGRAM_FIRST )
    {
        program_sub_state_save = program_sub_state;
        idc_program_number_save = idc_program_number;
        nac_program_number_save = nac_program_number;
        led_top_row_save = led_top_row;
        program_sub_state = PROGRAM_START;
    }
    return(PRGM_STILL_IN_PROGRESS);  //signify programming not finished yet
}

// BOC CSM 5/29/2005
static void update_common_top( void )
{
    update_top_row(led_top_row);
    update_lcd_top(led_top_row);
    display_current_programmed_choice();
}

static void update_common_bottom( void )
{
    unsigned char e2_data;

#if (PANEL_3_5_ZONE)
    // Selection potentially Class A...
    if ( led_bot_row == 2 )
    {
        // Class A not available on 3 zone board
        if ( !five_zone_panel )
        {
            // Force back to Class B IDC...
            if ( led_top_row == IDC_CLASS )
            {
                led_bot_row = 1;
            }
            // Skip to Parallel NAC...
            else if ( led_top_row == NAC_CLASS )
            {
                led_bot_row = 3;
            }
        }
        else
        {
            // Class A not available on IDC 5 on five zone board. Force back to Class B IDC...
            if ( led_top_row == IDC_CLASS && idc_program_number == IDC_5 )
            {
                led_bot_row = 1;
            }
        }
    }
#endif
    /* make sure NAC leds go out if monitor selected */
    if (led_top_row == IDC_ZONE_TYPES)
    {
        /* if changing to supervisory, clear NACs. User must reselect NACs. Should be clear by default. */
        if ( led_bot_row == SUP_NOT_COMBO )
        {
            nacs_activated.nacs_activated_byte = 0;
        }
        update_NAC_display(PRGM_NO_NAC);
    }

    /* NACs not silenceable for Coder and City Tie */
    if (led_top_row == NAC_SILENCEABLE)
    {
        e2_data = ext_eeprom_read(eeprom_nac_location[nac_program_number]) & NAC_TYPE_MASK;
        if ( e2_data == CODER || e2_data == CITY_TIE )
        {
            led_bot_row = 1;
        }
    }

    /* IDCs non-latching for monitor, remote silence/drill and remote ac fail */
    if ( led_top_row == IDC_NON_LATCHING )
    {
        e2_data = ext_eeprom_read(eeprom_idc_location[idc_program_number]) & IDC_TYPE_MASK;
        if ( e2_data >= MONITOR && e2_data <= REMOTE_AC_FAIL )
        {
            led_bot_row = 2;
        }
    }

    /* update leds and lcd */
    update_bot_row(led_bot_row);
    update_lcd_bot(led_top_row, led_bot_row);
}
// EOC CSM 5/29/2005

/**************************************************************************
 ** FUNCTION: update_lcd_top                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the token value for the top row of LEDs for the**
 ** current programming step.                                            **
 **                                                                      **
 ** PARAMETERS:  led_top_row - decimal programming step value            **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void update_lcd_top(unsigned char led_top_row)
{
// BOC CSM 5/16/2005
//    const unsigned char tokenValues[12] =
//    {
//        0,  /* BLANK_DISPLAY             */
//        3,  /* LOAD_DEFAULTS             */
//        4,  /* FIND_PERIPHERALS          */
//        5,  /* AUTO_NAC_SILENCE_TIMER    */
//        8,  /* AUX_PWR_RESET             */
//        9,  /* NAC_SILENCE_INHIBIT_TIMER */
//        11, /* IDC_ZONE_TYPES            */
//        20, /* IDC_CLASS                 */
//        23, /* NAC_CLASS                 */
//        24, /* NAC_OUTPUT                */
//        28, /* NAC_SILENCEABLE           */
//        29, /* PROGRAM_DACT              */
//    };
// EOC CSM 5/16/2005

// BOC CSM 8/29/2002
//    var_dact_data[0] = 4;
//    var_dact_data[1] = 0x03;
//    var_dact_data[2] = tokenValues[led_top_row];
//    var_dact_data[3] = 0;   //designate top line

//    //dialer_comm(WRITE_TO_DIALER, var_dact_data);
    if ( dactPresent && led_top_row != IDC_DESCRIPTION && led_top_row != EDIT_PASSCODE  )
    {
        var_dact_data[0] = 4;
        var_dact_data[1] = DACT_LCD_Token;
        var_dact_data[2] = tokenValues[led_top_row];
        var_dact_data[3] = DACT_LCD_Line_1;   //designate top line

#if (DACT_CODE)
        dialer_comm(WRITE_TO_DIALER, var_dact_data);
#endif
    }
// EOC CSM 8/29/2002
}


/**************************************************************************
 ** FUNCTION: update_lcd_bot                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the token value, for the current programmable  **
 ** step.                                                                **
 **                                                                      **
 ** PARAMETERS:  led_top_row - decimal programming step value            **
 **              led_bot_row - decimal programming step information value**
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void update_lcd_bot(unsigned char topNum, unsigned char botNum)
{
	unsigned char token, i;
	unsigned char e2_data1, e2_data2;
    unsigned char numericChoice = 0xFF;

// BOC CSM 8/29/2002
//    switch(topNum)
//    {
//        case BLANK_DISPLAY:
//            token = 0;
//            break;
//        case LOAD_DEFAULTS:
//        case AUX_PWR_RESET:
//        case NAC_SILENCEABLE:
//        case PROGRAM_DACT:

//            /* token = 1  "no"  */
//            /* token = 2  "yes" */
//            token = botNum;
//            break;

//        case FIND_PERIPHERALS:
//            if ( botNum == 1 )
//            {
//                e2_data1 = ext_eeprom_read(0x23);
//                e2_data2 = ext_eeprom_read(0x24);


//                var_dact_data[2] = 0;   //initialize count value of # on buss
//                for(i = 0; i <= 7; i++)
//                {
//                    if( e2_data1 & (1 << i) )
//                    {
//                        var_dact_data[2]++;
//                    }
//                    if( e2_data2 & (1 << i) )
//                    {
//                        var_dact_data[2]++;
//                    }
//                }

//                //display "## now on buss"
//                var_dact_data[0] = 3;
//                var_dact_data[1] = 0x0c;
//                //var_dact_data[2] is number now on buss and is calculated above
//            }
//            else
//            {
//                //display "find devices"
//                var_dact_data[0] = 4;
//                var_dact_data[1] = 0x03;
//                var_dact_data[2] = 30;  //"find devices"
//                var_dact_data[3] = 1;   //display on bottom line
//            }
//            //dialer_comm(WRITE_TO_DIALER, var_dact_data);
//            break;

//        case AUTO_NAC_SILENCE_TIMER:

//            /* token = 6  "none"  */
//            /* token = 7  "sil after 5 mins" */
//            token = botNum + 5;
//            break;

//        case NAC_SILENCE_INHIBIT_TIMER:
//            if ( botNum == 1 )
//            {
//                token = 6;  //"none"
//            }
//            else
//            {
//                token = 10; //"inhibit one min"
//            }
//            break;

//        case IDC_ZONE_TYPES:

//            /* token = 12 "alarm-unverified" */
//            /* token = 13 "alarm-verified"   */
//            /* token = 14 "retard-waterflow" */
//            /* token = 15 "waterflow"        */
//            /* token = 16 "ret-wf/superv"    */
//            /* token = 17 "wf-supervisory"   */
//            /* token = 18 "supervisory"      */
//            /* token = 19 "monitor"          */
//            token = botNum + 11;
//            break;

//        case IDC_CLASS:
//        case NAC_CLASS:
//            /* token = 21  "class B"  */
//            /* token = 22  "class A" */
//            token = botNum + 20;
//            break;

//        case NAC_OUTPUT:

//            /* token = 25 "continuous" */
//            /* token = 26 "temporal"   */
//            /* token = 27 "GENESIS"    */
//            token = botNum + 24;
//            break;
//    }

//    //diplay the token chosen above (except for FIND PERIPHERALS, this is done via special LCD message above
//    if ( led_top_row != FIND_PERIPHERALS )
//    {
//        var_dact_data[0] = 4;
//        var_dact_data[1] = 0x03;
//        var_dact_data[2] = token;
//        var_dact_data[3] = 1;   //display on bottom line

//        //dialer_comm(WRITE_TO_DIALER, var_dact_data);
//    }
    if ( dactPresent )
    {
        switch(topNum)
        {
            case BLANK_DISPLAY:
                token = 0;
                break;
            case LOAD_DEFAULTS:
            case NAC_SILENCEABLE:
            case PROGRAM_DACT:
            case ENABLE_PASSCODE:
            case IDC_NON_LATCHING:
            case IDC_CLEAN_ME:
                /* token = 1  "no"  */
                /* token = 2  "yes" */
                token = botNum;
                break;

            case FIND_PERIPHERALS:
                if ( botNum == 1 )
                {
                    e2_data1 = ext_eeprom_read(EEPROM_PERIPHERALS_RSI_RZI);
                    e2_data2 = ext_eeprom_read(EEPROM_PERIPHERALS_RRM);


                    var_dact_data[2] = 0;   //initialize count value of # on buss
                    for(i = 0; i <= 7; i++)
                    {
                        if( e2_data1 & (1 << i) )
                        {
                            var_dact_data[2]++;
                        }
                        if( e2_data2 & (1 << i) )
                        {
                            var_dact_data[2]++;
                        }
                    }

                    //display "## now on buss"
                    var_dact_data[0] = 3;
                    var_dact_data[1] = SpecialDisplay1;
                    //var_dact_data[2] is number now on buss and is calculated above
                }
                else
                {
                    //display "find devices"
                    var_dact_data[0] = 4;
                    var_dact_data[1] = DACT_LCD_Token;
                    var_dact_data[2] = 59;  //"find devices"
                    var_dact_data[3] = DACT_LCD_Line_2;   //display on bottom line
                }
#if (DACT_CODE)
                dialer_comm(WRITE_TO_DIALER, var_dact_data);
#endif
                break;

// BOC CSM 5/16/2005
            case PANEL_OPERATING_MODE:
                /* token = 7  "FACP"              */
                /* token = 8  "FACOM"             */
                /* token = 9  "SSU"               */
            case MULTI_RRM_MODE_SELECT:
                /* token = 11 "Common Alarm"      */
                /* token = 12 "Matrix"            */
            case NAC_SILENCE_INHIBIT_TIMER:
                /* token = 17 "None"              */
                /* token = 18 "Inhibit 1 Minute   */
            case DISABLE_LOCKOUT:
                /* token = 22 "None"              */
                /* token = 23 "All"               */
                /* token = 24 "IDC/NAC"           */
                /* token = 25 "Remote Disconnect" */
            case IDC_ZONE_TYPES:
                /* token = 27 "alarm-unverified" */
                /* token = 28 "alarm-verified"   */
                /* token = 29 "retard-waterflow" */
                /* token = 30 "waterflow"        */
                /* token = 31 "ret-wf/superv"    */
                /* token = 32 "wf-supervisory"   */
                /* token = 33 "supervisory"      */
                /* token = 34 "monitor"          */
                /* token = 35 "rem silence/drill */
                /* token = 36 "rem ac fail       */
                /* token = 37 "cross zoned       */
            case IDC_CLASS:
                /* token = 41  "class B"  */
                /* token = 42  "class A"  */
            case NAC_OUTPUT:
                /* token = 51 "continuous" */
                /* token = 52 "temporal"   */
                /* token = 53 "GENESIS"    */
                /* token = 54 "GENESIS AV" */
                /* token = 55 "coder"      */
                /* token = 56 "city tie"   */
                token = botNum + tokenValues[led_top_row];
                break;

            case NAC_CLASS:
                /* token = 41  "class B"  */
                /* token = 42  "class A"  */
                /* token = 43  "parallel" */
                token = botNum + tokenValues[IDC_CLASS];
                break;

            case AUTO_NAC_SILENCE_TIMER:
                /* token = x  "0,1,2,...,30"  */
                numericChoice = botNum;
                token = 64;
                break;

            case AC_FAIL_DELAY_TIMER:
                /* token = x  "0,1,2,...,15"  */
                numericChoice = botNum;
                token = 65;
                break;

            case WATERFLOW_DELAY_TIMER:
                /* token = x  "0,5,10,...,150"  */
                numericChoice = botNum * 5;
                token = 63;
                break;

            case IDC_RRM_ASSIGNMENT:
                /* token = x  "0,1,2,...,5"  */
            case IDC_CODER_DIGIT1_ASSIGNMENT:
            case IDC_CODER_DIGIT2_ASSIGNMENT:
            case IDC_CODER_DIGIT3_ASSIGNMENT:
            case IDC_CODER_DIGIT4_ASSIGNMENT:
                /* token = x  "0,1,2,...,15"  */
                numericChoice = botNum;
                token = 66;
                break;
        }

        //diplay the token chosen above (except for FIND PERIPHERALS and IDC_DESCRIPTION, this is done via special LCD message)
        if ( led_top_row != FIND_PERIPHERALS && led_top_row != IDC_DESCRIPTION && led_top_row != EDIT_PASSCODE )
        {
            var_dact_data[0] = 4;
            var_dact_data[1] = DACT_LCD_Token;
            var_dact_data[2] = token;
            var_dact_data[3] = DACT_LCD_Line_2;   //display on bottom line

            if ( numericChoice != 0xff )
            {
                var_dact_data[1] = SpecialDisplay3;
                var_dact_data[3] = numericChoice;
            }

#if (DACT_CODE)
            dialer_comm(WRITE_TO_DIALER, var_dact_data);
#endif
        }
    }
// EOC CSM 8/29/2002

}


/**************************************************************************
 ** FUNCTION: update_top_row                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the  top row of LEDs with current              **
 ** programming step's binary value.                                     **
 **                                                                      **
 ** PARAMETERS:  display_number - decimal value to be displayed on top   **
 **                               row of LEDs.                           **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void update_top_row(unsigned char display_number)
{
    latch1.latch1_output_byte &= 0b11100000;
    latch1.latch1_output_byte |= display_number;
}

/**************************************************************************
 ** FUNCTION: update_bot_row                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the bottom row of LEDs with current            **
 ** programming step's programmed binary value.                          **
 **                                                                      **
 ** PARAMETERS:  display_number - decimal value to be displayed on bottom**
 **                               row of LEDs.                           **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void update_bot_row(unsigned char display_number)
{
//    latch2.latch2_output_byte &= 0b11100000;
//    latch2.latch2_output_byte |= display_number;
    latch2.latch2_output_byte &= 0b11100000;
#if (PANEL_3_5_ZONE)
    latch6.latch6_output_bit.com_waterflow = 0;
    latch6.latch6_output_bit.com_service_detector = 0;
#elif (PANEL_10_ZONE)
    latch9.latch9_output_bit.com_waterflow = 0;
    latch9.latch9_output_bit.com_service_detector = 0;
#endif

    if ( display_number & 0x01 )
    {
#if (PANEL_3_5_ZONE)
        latch6.latch6_output_bit.com_service_detector = 1;
#elif (PANEL_10_ZONE)
        latch9.latch9_output_bit.com_service_detector = 1;
#endif
    }
    if ( display_number & 0x10 )
    {
#if (PANEL_3_5_ZONE)
        latch6.latch6_output_bit.com_waterflow = 1;
#elif (PANEL_10_ZONE)
        latch9.latch9_output_bit.com_waterflow = 1;
#endif
    }
    latch2.latch2_output_byte |= ( display_number << 1 ) & 0xFC;
}

/**************************************************************************
 ** FUNCTION: display_current_programmed_choice                          **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the top and bottom row of LEDs with their      **
 ** current programming step and programmed value.                       **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void display_current_programmed_choice(void)
{
    unsigned char e2_data1; //, e2_data2;

    //clear idc 1,2 & 3 trouble LEDs
    latch4.latch4_output_byte &= 0b10001111;

    //clear idc 4 & 5 trouble LEDs
    latch5.latch5_output_byte &= 0b11110011;

    //clear NAC1 & 2 TROUBLE LED displays
    latch1.latch1_output_bit.nac1_trbl = 0;
    latch1.latch1_output_bit.nac2_trbl = 0;

#if (PANEL_10_ZONE)
    //clear idc 6 & 7 trouble LEDs
    latch6.latch6_output_byte &= 0b11110011;

    //clear idc 8 & 9 trouble LEDs
    latch7.latch7_output_byte &= 0b11110011;

    //clear idc 10 trouble LED
    latch8.latch8_output_byte &= 0b11011111;

    //clear NAC3 & 4 TROUBLE LED displays
    latch8.latch8_output_bit.nac3_trbl = 0;
    latch8.latch8_output_bit.nac4_trbl = 0;
#endif

    // default selection to first option
    led_bot_row = 1;

    //switch on current display on TOP ROW of LEDs
    switch (led_top_row)
    {
        case LOAD_DEFAULTS:
        case FIND_PERIPHERALS:
		case PROGRAM_DACT:
            /* token = 1  "no"  */
            break;

        case EDIT_PASSCODE:
#if (DACT_CODE)
            dialer_comm(WRITE_TO_DIALER, enter_DACT_edit_passcode);
#endif
            DACT_program_mode_active_flag = 1;
            led_bot_row = 0;
            break;

        case PANEL_OPERATING_MODE:
            /* token = 1 "FACP " */
            /* token = 2 "FACOM" */
            /* token = 3 "SSU  " */
            led_bot_row = ( ext_eeprom_read(EEPROM_PANEL_OP_MODE) >> PANEL_OPERATING_MODE_SHIFT ) & PANEL_OPERATING_MODE_MASK;
            break;

        case AUTO_NAC_SILENCE_TIMER:
            led_bot_row = ext_eeprom_read(EEPROM_AUTO_NAC_SILENCE) & AUTO_NAC_SILENCE;
            break;

        case WATERFLOW_DELAY_TIMER:
            led_bot_row = ext_eeprom_read(EEPROM_WATERFLOW_DELAY) & WATERFLOW_DELAY;
            break;

        case AC_FAIL_DELAY_TIMER:
            led_bot_row = ext_eeprom_read(EEPROM_AC_FAIL_DELAY) & AC_FAIL_DELAY;
            break;

        case MULTI_RRM_MODE_SELECT:
            if (!( ext_eeprom_read(EEPROM_RRM_COMMON_MODE) & RRM_COMMON_ALARM_MODE ))
            {
                led_bot_row = 2;
            }
            /* else default = 1 */
            break;

        case DISABLE_LOCKOUT:
            e2_data1 = ( ext_eeprom_read(EEPROM_DISABLE_LOCKOUT) >> DISABLE_LOCKOUT_SHIFT ) & DISABLE_LOCKOUT_MASK;
            //stored 0-based, displayed 1-base
            if ( e2_data1 == ALL_LOCKOUT )
            {
                led_bot_row = 2;
            }
            else if ( e2_data1 == IDC_NAC_LOCKOUT )
            {
                led_bot_row = 3;
            }
            else if ( e2_data1 == REMOTE_DISCONNECT_LOCKOUT )
            {
                led_bot_row = 4;
            }
            break;

        case NAC_SILENCE_INHIBIT_TIMER:
            if ( ext_eeprom_read(EEPROM_NAC_SILENCE_INHIBIT) & NAC_SILENCE_INHIBIT )
            {
                led_bot_row = 2;
            }
            break;

        case ENABLE_PASSCODE:
            if ( ext_eeprom_read(EEPROM_ENABLE_PASSCODE) & PASSCODE_ENABLED )
            {
                led_bot_row = 2;
            }
            break;

        //IDC ZONE TYPES & NACs THAT ARE ACTIVATED
        case IDC_ZONE_TYPES:

            light_or_extinguish_idc_trouble_leds(LIGHT, idc_program_number);
            led_bot_row = ext_eeprom_read(eeprom_idc_location[idc_program_number]) & IDC_TYPE_MASK;

            //get/display NACs that activate for the current IDC
            e2_data1 = ext_eeprom_read( (eeprom_idc_location[idc_program_number]+EEPROM_IDC_OPTIONS_OFFSET) );
            nacs_activated.nacs_activated_byte = 0;
            nacs_activated.nacs_activated_byte |= (e2_data1 & 0b00001111);

            //dummy parameter sent
            update_NAC_display(PRGM_NO_NAC);
            break;

        //IDC ZONE SUPERVISION LEVEL
        case IDC_CLASS:
        //IDC ZONE LATCH OPERATION
        case IDC_NON_LATCHING:
        //IDC CLEAN ME OPERATION
        case IDC_CLEAN_ME:

            light_or_extinguish_idc_trouble_leds(LIGHT, idc_program_number);
            e2_data1 = ext_eeprom_read(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET);

            if ( led_top_row == IDC_NON_LATCHING )
            {
                if (e2_data1 & IDC_NONLATCHING)
                {
                    led_bot_row = 2;
                }
            }
            else if ( led_top_row == IDC_CLASS )
            {
                if (e2_data1 & CLASS_A)
                {
                    led_bot_row = 2;
                }
            }
            else
            {
                if (e2_data1 & IDC_CLEAN_ME_ENABLED)
                {
                    led_bot_row = 2;
                }
            }
            break;

        //IDC Trips what RRM Zone in Matrix mode
        case IDC_RRM_ASSIGNMENT:
            light_or_extinguish_idc_trouble_leds(LIGHT, idc_program_number);
            led_bot_row = (ext_eeprom_read(eeprom_idc_location[idc_program_number]) >> IDC_RRM_SHIFT ) & IDC_RRM_MASK;
            break;

        case IDC_CODER_DIGIT1_ASSIGNMENT:
        case IDC_CODER_DIGIT2_ASSIGNMENT:
        case IDC_CODER_DIGIT3_ASSIGNMENT:
        case IDC_CODER_DIGIT4_ASSIGNMENT:
            light_or_extinguish_idc_trouble_leds(LIGHT, idc_program_number);

            /* temp use of led_bot_row for data read */
            if ( led_top_row == IDC_CODER_DIGIT1_ASSIGNMENT || led_top_row == IDC_CODER_DIGIT2_ASSIGNMENT )
            {
                led_bot_row = EEPROM_IDC_CODER_DIGIT_1_2_OFFSET;
            }
            else
            {
                led_bot_row = EEPROM_IDC_CODER_DIGIT_3_4_OFFSET;
            }
            e2_data1 = ext_eeprom_read(eeprom_idc_location[idc_program_number] + led_bot_row);

            /* temp use of led_bot_row for data shift */
            if ( led_top_row == IDC_CODER_DIGIT1_ASSIGNMENT || led_top_row == IDC_CODER_DIGIT3_ASSIGNMENT )
            {
                led_bot_row = IDC_CODER_DIGIT_1_3_SHIFT;
            }
            else
            {
                led_bot_row = IDC_CODER_DIGIT_2_4_SHIFT;
            }
            led_bot_row = ( e2_data1 >> led_bot_row ) & IDC_CODER_MASK;
            break;

        //EDIT IDC DESCRIPTION
        case IDC_DESCRIPTION:
            /* light selected IDC and pass control to DACT for edit. IDC text stored in DACT. */
            light_or_extinguish_idc_trouble_leds(LIGHT, idc_program_number);
#if (DACT_CODE)
            memcpy( var_dact_data, enter_DACT_program_IDC_mode, 3 );
            var_dact_data[3] = idc_program_number;
            dialer_comm(WRITE_TO_DIALER, var_dact_data);
#endif
            DACT_program_mode_active_flag = 1;
            led_bot_row = 0;
            break;

        //NAC SUPERVISION LEVEL
        case NAC_CLASS:

            update_NAC_display(nac_program_number);
            e2_data1 = ext_eeprom_read(eeprom_nac_location[nac_program_number]);

            if (e2_data1 & CLASS_A)
            {
                led_bot_row = 2;
            }
            else if ( e2_data1 & PARALLEL )
            {
                led_bot_row = 3;
            }
            break;

        //NAC OUTPUT
        case NAC_OUTPUT:
            update_NAC_display(nac_program_number);
            led_bot_row = ext_eeprom_read(eeprom_nac_location[nac_program_number]) & NAC_TYPE_MASK;
            break;

        //NAC SILENCEABLE
        case NAC_SILENCEABLE:
            update_NAC_display(nac_program_number);
            if ( ext_eeprom_read(eeprom_nac_location[nac_program_number]) & SILENCEABLE )
            {
                led_bot_row = 2;
            }
            break;
    }

    /* update LEDs/LCD */
    update_bot_row(led_bot_row);
    update_lcd_bot(led_top_row, led_bot_row);
}

/**************************************************************************
 ** FUNCTION: update_NAC_display                                         **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function updates the NAC trouble LEDs for association with IDCs,**
 ** or currently selected NAC for programming.                           **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void update_NAC_display(unsigned char NAC_number)
{
    //see if we're programming NACs associated with IDCs (dummy paramenter (0) passed)
    if (led_top_row == IDC_ZONE_TYPES)
    {
        /* no NAC activations for monitor mode */
// BOC CSM 5/29/2005
//        if ( led_bot_row == 8 )
        if ( led_bot_row >= MONITOR && led_bot_row <= REMOTE_AC_FAIL )
// EOC CSM 5/29/2005
        {
            latch1.latch1_output_bit.nac1_trbl = 0;
            latch1.latch1_output_bit.nac2_trbl = 0;
#if (PANEL_10_ZONE)
            latch8.latch8_output_bit.nac3_trbl = 0;
            latch8.latch8_output_bit.nac4_trbl = 0;
#endif
        }
        else
        {
            latch1.latch1_output_bit.nac1_trbl = nacs_activated.nacs_activated_bit.nac1;
            latch1.latch1_output_bit.nac2_trbl = nacs_activated.nacs_activated_bit.nac2;
#if (PANEL_10_ZONE)
            latch8.latch8_output_bit.nac3_trbl = nacs_activated.nacs_activated_bit.nac3;
            latch8.latch8_output_bit.nac4_trbl = nacs_activated.nacs_activated_bit.nac4;
#endif
        }
    }
    //else we're programming the NACs themselves
    else
    {
        /* Don't know why I have to do this!!!  Add 1 here */
        NAC_number++;
        /* and subtract when passing to function, doesn't work otherwise */
        light_or_extinguish_nac_trouble_leds(LIGHT, (NAC_number-1));
    }
}

/**************************************************************************
 ** FUNCTION: convert_bit_position_to_number                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function will check which bit is turned ON in bit mask and      **
 ** return its position in bitmask.                                      **
 **                                                                      **
 ** PARAMETERS:  bit_field - bit mask to be converted to bit position    **
 **                                                                      **
 ** RETURNS:  ON bit position, in bit_field                              **
 **                                                                      **
 **************************************************************************/
#if 0
static unsigned char convert_bit_position_to_number(unsigned char bit_field)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        if (bit_field & (1 << i))
        {
            return(i+1);
        }
    }
    //return 0 if no bit set
    return(0);
}
#endif

/**************************************************************************
 ** FUNCTION: convert_number_to_bit_position                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function will turn on the selected bit position and turn off    **
 ** all other bit positions.                                             **
 **                                                                      **
 ** PARAMETERS:  number - bit position to be turned ON.                  **
 **                                                                      **
 ** RETURNS:  bit mask with "number" bit turned ON.                      **
 **                                                                      **
 **************************************************************************/
#if 0
static unsigned char convert_number_to_bit_position(unsigned char number)
{
    if (!number)
    {
        return(0);
    }
    return( (1<<(number-1)) );
}
#endif

/**************************************************************************
 ** FUNCTION: enter_choice                                               **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function will update EEPROM with the current programmed value   **
 ** of the current porgrammable step.                                    **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
static void enter_choice(void)
{
    unsigned char e2_data, e2_default_ptr, e2_ptr;

    //command buzzer to emit long ("enter") sound
    buzzer_enter_flag = 1;

    //switch on current setting for top row of leds
    switch (led_top_row)
    {
        //load defaults
        case LOAD_DEFAULTS:
            if (led_bot_row == 2)
            {
                //transfer E2 data from default area to working area
                for (e2_default_ptr = EEPROM_DEFAULT_START, e2_ptr = 0; e2_default_ptr <= EEPROM_DEFAULT_END; e2_default_ptr++, e2_ptr++)
                {
                    e2_data = ext_eeprom_read(e2_default_ptr);
                    if(e2_default_ptr == EEPROM_DEFAULT_START && dactPresent )
					{
						e2_data |= DACT_PRESENT;	//keep DACT present or not setting intact
					}
                    ext_eeprom_write(e2_ptr, e2_data);
                }
            }
            break;

        //find peripherals
        case FIND_PERIPHERALS:

            if ( led_bot_row == 2 )
			{
                //enable "poll_for_peripherals()" function to run
                enable_poll_for_peripherals = 1;

// BOC CSM 8/29/2002
//                //display on LCD "finding devices"
//                var_dact_data[0] = 4;
//                var_dact_data[1] = 0x03;
//                var_dact_data[2] = 31;  //"finding devices"
//                var_dact_data[3] = 1;   //display on bottom line
//                //dialer_comm(WRITE_TO_DIALER, var_dact_data);
                if ( dactPresent )
                {
                    //display on LCD "finding devices"
                    var_dact_data[0] = 4;
                    var_dact_data[1] = DACT_LCD_Token;
                    var_dact_data[2] = 60;  //"finding devices"
                    var_dact_data[3] = DACT_LCD_Line_2;   //display on bottom line
#if (DACT_CODE)
                    dialer_comm(WRITE_TO_DIALER, var_dact_data);
#endif
                }
// EOC CSM 8/29/2002

			}
            break;

        case PANEL_OPERATING_MODE:
            e2_data  = ext_eeprom_read(EEPROM_PANEL_OP_MODE) & ~( PANEL_OPERATING_MODE_MASK << PANEL_OPERATING_MODE_SHIFT );
            e2_data |= led_bot_row << PANEL_OPERATING_MODE_SHIFT;
            ext_eeprom_write(EEPROM_PANEL_OP_MODE, e2_data);

            // If selected operating mode is FACOM or SSU, unlatch appropriate IDCs
            if ( led_bot_row > PANEL_MODE_FACP )
            {
                for ( e2_default_ptr = IDC_1; e2_default_ptr < MAX_IDC; e2_default_ptr++ )
                {
                    // Unlatch IDC if FACOM. If SSU, unlatch IDC if Supervisory
                    e2_data = ext_eeprom_read(eeprom_idc_location[e2_default_ptr] + EEPROM_IDC_OPTIONS_OFFSET);
                    if ( led_bot_row == PANEL_MODE_FACOM
                            || (ext_eeprom_read(eeprom_idc_location[e2_default_ptr]) & IDC_TYPE_MASK) == SUP_NOT_COMBO )
                    {
                        e2_data |= IDC_NONLATCHING;
                    }
                    else
                    {
                        e2_data &= ~IDC_NONLATCHING;
                    }
                    ext_eeprom_write(eeprom_idc_location[e2_default_ptr] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);
                }
            }
            break;

        //auto signal silence timer
        case AUTO_NAC_SILENCE_TIMER:
            e2_data = ext_eeprom_read(EEPROM_AUTO_NAC_SILENCE) & ~AUTO_NAC_SILENCE;
            e2_data |= led_bot_row;

            ext_eeprom_write(EEPROM_AUTO_NAC_SILENCE, e2_data);
            break;

// BOC CSM 5/15/2005
        case WATERFLOW_DELAY_TIMER:
            e2_data = ext_eeprom_read(EEPROM_WATERFLOW_DELAY) & ~WATERFLOW_DELAY;
            e2_data |= led_bot_row;

            ext_eeprom_write(EEPROM_WATERFLOW_DELAY, e2_data);
            break;

        case AC_FAIL_DELAY_TIMER:
            e2_data = ext_eeprom_read(EEPROM_AC_FAIL_DELAY) & ~AC_FAIL_DELAY;
            e2_data |= led_bot_row;

            ext_eeprom_write(EEPROM_AC_FAIL_DELAY, e2_data);
            break;

        case MULTI_RRM_MODE_SELECT:
            e2_data = ext_eeprom_read(EEPROM_RRM_COMMON_MODE) & ~RRM_COMMON_ALARM_MODE;
            if ( led_bot_row == 1 )
            {
                e2_data |= RRM_COMMON_ALARM_MODE;
                rrmCommonAlarm = 1;
            }
            else
            {
                rrmCommonAlarm = 0;
            }

            ext_eeprom_write(EEPROM_RRM_COMMON_MODE, e2_data);
            break;

        case DISABLE_LOCKOUT:
            e2_data = ext_eeprom_read(EEPROM_DISABLE_LOCKOUT) & ~( DISABLE_LOCKOUT_MASK << DISABLE_LOCKOUT_SHIFT );
            // Displayed 1-based, stored 0-based
            switch ( led_bot_row )
            {
                case 1:
                    e2_ptr = NO_LOCKOUT;
                    break;
                case 2:
                    e2_ptr = ALL_LOCKOUT;
                    break;
                case 3:
                    e2_ptr = IDC_NAC_LOCKOUT;
                    break;
                case 4:
                    e2_ptr = REMOTE_DISCONNECT_LOCKOUT;
                    break;
            }
            e2_data |= e2_ptr << DISABLE_LOCKOUT_SHIFT;

            ext_eeprom_write(EEPROM_DISABLE_LOCKOUT, e2_data);
            break;

        //alarm silence inhibit timer
        case NAC_SILENCE_INHIBIT_TIMER:
            e2_data = ext_eeprom_read(EEPROM_NAC_SILENCE_INHIBIT);

            if (led_bot_row == 2)
            {
                e2_data |= NAC_SILENCE_INHIBIT;
            }
            else
            {
                e2_data &= ~NAC_SILENCE_INHIBIT;
            }

            ext_eeprom_write(EEPROM_NAC_SILENCE_INHIBIT, e2_data);
            break;

        //passcode enable
        case ENABLE_PASSCODE:
            e2_data = ext_eeprom_read(EEPROM_ENABLE_PASSCODE);

            if (led_bot_row == 2)
            {
                e2_data |= PASSCODE_ENABLED;
            }
            else
            {
                e2_data &= ~PASSCODE_ENABLED;
            }

            ext_eeprom_write(EEPROM_ENABLE_PASSCODE, e2_data);
            break;

        //IDC zone types and NACs that activate
        case IDC_ZONE_TYPES:
            e2_data = ( ext_eeprom_read(eeprom_idc_location[idc_program_number]) & ~IDC_TYPE_MASK ) | led_bot_row;
            ext_eeprom_write(eeprom_idc_location[idc_program_number], e2_data);

            e2_data = ( ext_eeprom_read(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET) & 0b11110000 ) | nacs_activated.nacs_activated_byte;
            if ( led_bot_row >= MONITOR && led_bot_row <= REMOTE_AC_FAIL )
            {
                e2_data |= IDC_NONLATCHING;
            }
            ext_eeprom_write(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);

            if ( ( e2_data & CLASS_A )
                    || ( led_bot_row == CROSS_ZONED_ALARM )
                    || ( ( ext_eeprom_read(eeprom_idc_location[idcCompanion(idc_program_number)]) & IDC_TYPE_MASK ) == CROSS_ZONED_ALARM ) )
            {
                ext_eeprom_write(eeprom_idc_location[idcCompanion(idc_program_number)], led_bot_row);
                ext_eeprom_write(eeprom_idc_location[idcCompanion(idc_program_number)] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);
//                ext_eeprom_write(eeprom_idc_location[eeprom_idc_companion[idc_program_number]], led_bot_row);
//                ext_eeprom_write(eeprom_idc_location[eeprom_idc_companion[idc_program_number]] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);
            }
            break;

        //IDC zone latch operation
        case IDC_NON_LATCHING:
        //IDC Clean Me selection
        case IDC_CLEAN_ME:
            e2_data = ext_eeprom_read(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET);

            if ( led_top_row == IDC_NON_LATCHING )
            {
                /* non-latching */
                if (led_bot_row == 2)
                {
                    e2_data |= IDC_NONLATCHING;
                }
                /* latching */
                else
                {
                    e2_data &= ~IDC_NONLATCHING;
                }
            }
            else
            {
                /* enabled */
                if (led_bot_row == 2)
                {
                    e2_data |= IDC_CLEAN_ME_ENABLED;
                }
                /* disabled */
                else
                {
                    e2_data &= ~IDC_CLEAN_ME_ENABLED;
                }
            }

            ext_eeprom_write(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);

            if ( e2_data & CLASS_A )
            {
                ext_eeprom_write(eeprom_idc_location[idcCompanion(idc_program_number)] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);
//                ext_eeprom_write(eeprom_idc_location[eeprom_idc_companion[idc_program_number]] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);
            }
            break;

        //IDC Trips what RRM Zone in Matrix mode
        case IDC_RRM_ASSIGNMENT:
            e2_data  = ext_eeprom_read(eeprom_idc_location[idc_program_number]) & ~( IDC_RRM_MASK << IDC_RRM_SHIFT );
            e2_data |= led_bot_row << IDC_RRM_SHIFT;
            ext_eeprom_write(eeprom_idc_location[idc_program_number], e2_data);

            if ( ext_eeprom_read(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET) & CLASS_A )
            {
                ext_eeprom_write(eeprom_idc_location[idcCompanion(idc_program_number)], e2_data);
//                ext_eeprom_write(eeprom_idc_location[eeprom_idc_companion[idc_program_number]], e2_data);
            }
            break;

        // IDC coding
        case IDC_CODER_DIGIT1_ASSIGNMENT:
        case IDC_CODER_DIGIT2_ASSIGNMENT:
        case IDC_CODER_DIGIT3_ASSIGNMENT:
        case IDC_CODER_DIGIT4_ASSIGNMENT:
            if ( led_top_row == IDC_CODER_DIGIT1_ASSIGNMENT || led_top_row == IDC_CODER_DIGIT2_ASSIGNMENT )
            {
                e2_ptr = EEPROM_IDC_CODER_DIGIT_1_2_OFFSET;
            }
            else
            {
                e2_ptr = EEPROM_IDC_CODER_DIGIT_3_4_OFFSET;
            }
            e2_data = ext_eeprom_read(eeprom_idc_location[idc_program_number] + e2_ptr);

            if ( led_top_row == IDC_CODER_DIGIT1_ASSIGNMENT || led_top_row == IDC_CODER_DIGIT3_ASSIGNMENT )
            {
                e2_default_ptr = IDC_CODER_DIGIT_1_3_SHIFT;
            }
            else
            {
                e2_default_ptr = IDC_CODER_DIGIT_2_4_SHIFT;
            }
            e2_data &= ~( IDC_CODER_MASK << e2_default_ptr );
            e2_data |= led_bot_row << e2_default_ptr;

            ext_eeprom_write(eeprom_idc_location[idc_program_number] + e2_ptr, e2_data);

            if ( ext_eeprom_read(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET) & CLASS_A )
            {
                ext_eeprom_write(eeprom_idc_location[idcCompanion(idc_program_number)] + e2_ptr, e2_data);
//                ext_eeprom_write(eeprom_idc_location[eeprom_idc_companion[idc_program_number]] + e2_ptr, e2_data);
            }
            break;

        //IDC zone supervision level
        case IDC_CLASS:
            e2_data = ext_eeprom_read(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET);

            /* class A */
            if (led_bot_row == 2)
            {
                e2_data |= CLASS_A;
            }
            /* class B */
			else
            {
                e2_data &= ~CLASS_A;
            }

            ext_eeprom_write(eeprom_idc_location[idc_program_number] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);

            if (!(e2_data & CLASS_A ))
            {
                // If not Set for Class A, ensure the companion IDC is also cleared (set Class B)
//                e2_data = ext_eeprom_read(eeprom_idc_location[eeprom_idc_companion[idc_program_number]] + EEPROM_IDC_OPTIONS_OFFSET);
                e2_data = ext_eeprom_read(eeprom_idc_location[idcCompanion(idc_program_number)] + EEPROM_IDC_OPTIONS_OFFSET);
                e2_data &= ~CLASS_A;
            }
            ext_eeprom_write(eeprom_idc_location[idcCompanion(idc_program_number)] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);
//            ext_eeprom_write(eeprom_idc_location[eeprom_idc_companion[idc_program_number]] + EEPROM_IDC_OPTIONS_OFFSET, e2_data);

            if ( e2_data & CLASS_A )
            {
                e2_data = ext_eeprom_read(eeprom_idc_location[idc_program_number]);
                ext_eeprom_write(eeprom_idc_location[idcCompanion(idc_program_number)], e2_data);
//                ext_eeprom_write(eeprom_idc_location[eeprom_idc_companion[idc_program_number]], e2_data);
            }
            break;

        //NAC supervision level
        case NAC_CLASS:
            e2_data = ext_eeprom_read(eeprom_nac_location[nac_program_number]);

            /* class A */
            if (led_bot_row == 2)
            {
                e2_data |= CLASS_A;
            }
            /* class B */
            else
            {
                e2_data &= ~CLASS_A;
            }

            /* Parallel */
            if (led_bot_row == 3)
            {
                e2_data |= PARALLEL;
            }
            /* Not parallel */
            else
            {
                e2_data &= ~PARALLEL;
            }

            ext_eeprom_write(eeprom_nac_location[nac_program_number], e2_data);
            ext_eeprom_write(eeprom_nac_location[eeprom_nac_companion[nac_program_number]], e2_data);
            break;

        //NAC output
        case NAC_OUTPUT:
            e2_data = ( ext_eeprom_read(eeprom_nac_location[nac_program_number]) & ~NAC_TYPE_MASK ) | led_bot_row;

            // NAC not silenceable if CODER or CITY_TIE
            if ( led_bot_row == CODER || led_bot_row == CITY_TIE )
            {
                e2_data &= ~SILENCEABLE;
            }
            ext_eeprom_write(eeprom_nac_location[nac_program_number], e2_data);

            if (( e2_data & CLASS_A ) || ( e2_data & PARALLEL ))
            {
                ext_eeprom_write(eeprom_nac_location[eeprom_nac_companion[nac_program_number]], e2_data);
            }
            break;

        //NAC silenceable
        case NAC_SILENCEABLE:
            e2_data = ext_eeprom_read(eeprom_nac_location[nac_program_number]);

            if (led_bot_row == 2)
            {
                e2_data |= SILENCEABLE;
            }
            else
            {
                e2_data &= ~SILENCEABLE;
            }

            ext_eeprom_write(eeprom_nac_location[nac_program_number], e2_data);

            if (( e2_data & CLASS_A ) || ( e2_data & PARALLEL ))
            {
                ext_eeprom_write(eeprom_nac_location[eeprom_nac_companion[nac_program_number]], e2_data);
            }
            break;

		//Enter DACT Programming Mode
		case PROGRAM_DACT:
            if (led_bot_row == 2)
			{
#if (DACT_CODE)
                dialer_comm(WRITE_TO_DIALER, continue_DACT_program_mode);
#endif
				DACT_program_mode_active_flag = 1;
			}
			break;
    }
}

/**************************************************************************
 ** FUNCTION: poll_for_peripherals                                       **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function will poll all the available peripheral addresses and   **
 ** update EEPROM with the peripherals that answer.                      **
 **                                                                      **
 ** PARAMETERS:  frontPanelOrDact:
 **					0 - Front Panel button Press prompted this function  **
 **						call                                             **
 **					1 - DACT MANAGER requested this function call        **
 **                                                                      **
 ** RETURNS:  0 - polling of peripherals in progress                     **
 **           1 - polling complete                                       **
 **                                                                      **
 **************************************************************************/
bit poll_for_peripherals(unsigned char frontPanelOrDact)
{
    static unsigned char indexPeriph;
    static unsigned char state = POLL_START;
    static unsigned char e2_temp0, e2_temp1;
    unsigned char number_of_periphs, indexBit;
	static unsigned char old_periph_num;

	if(frontPanelOrDact == FRONT_PANEL_BUTTON_PRESS)
	{
	    //command bottom row of LEDs to flash
//        latch2_pulse.latch2_pulse_byte = 0b00011111;
        latch2_pulse.latch2_pulse_byte = 0b00011100;
#if (PANEL_3_5_ZONE)
        latch6_pulse.latch6_pulse_byte = 0b01010000;
#elif (PANEL_10_ZONE)
        latch9_pulse.latch9_pulse_byte = 0b00001100;
#endif
	}

    switch (state)
    {
        case POLL_START:

            if (xmit_packet.status == SERVICE_AVAILABLE)
			{
				if(frontPanelOrDact == FRONT_PANEL_BUTTON_PRESS)
				{
					//save old number of periphs before "sniffing" for new
                    e2_temp0 = ext_eeprom_read(EEPROM_PERIPHERALS_RSI_RZI);
                    e2_temp1 = ext_eeprom_read(EEPROM_PERIPHERALS_RRM);

					old_periph_num = 0;
					for(indexPeriph = 0; indexPeriph <= 7; indexPeriph++)
					{
						if( e2_temp0 & (1 << indexPeriph) )
						{
							old_periph_num++;
						}
						if( e2_temp1 & (1 << indexPeriph) )
						{
							old_periph_num++;
						}
					}
				}


                //clear all previously "logged" peripherals
                ext_eeprom_write(EEPROM_PERIPHERALS_RSI_RZI,0);
                ext_eeprom_write(EEPROM_PERIPHERALS_RRM,0);

                /* initialize variables */
                e2_temp0 = 0;
				e2_temp1 = 0;
                indexPeriph = 0;

                /* move on to next state */
                state = POLL_PERIPHERAL;
			}
			break;

        case POLL_PERIPHERAL :

            if (xmit_packet.status == SERVICE_AVAILABLE)
            {
                PERIPH_COMM(addresses[indexPeriph], POLL_FOR_PRESENCE, 0,0,0);
                state = POLL_WAIT_FOR_ACK;
            }
            break;

        case POLL_WAIT_FOR_ACK :

            if (xmit_packet.status == RECEIVE_DATA_VALID)
            {
                ACK_RCV_DATA_VALID;
                state = POLL_NEXT_PERIPHERAL;

                if ( indexPeriph <= 7 )
                {
                    /* set RSI Subtype */
                    if ( indexPeriph < ADDRESS_RZI )
                    {
                        ext_eeprom_write( EEPROM_RSI2_SUBTYPE + indexPeriph, receive_packet.data[2] );
                    }

                    e2_temp0 |= (1 << indexPeriph);
                }
                else if ( (indexPeriph > 7) && (indexPeriph <= 15) )
                {
                    e2_temp1 |= (1 << (indexPeriph-8));
                }
            }

            if (xmit_packet.status == COMM_FAILURE)
            {
                state = POLL_NEXT_PERIPHERAL;
				CLEAR_COMM_FAILURE;
            }
            break;

        case POLL_NEXT_PERIPHERAL :

            state = POLL_PERIPHERAL;

            if ( ++indexPeriph >= ADDRESSES_SIZE )
            {
                //write "discovered" peripheral devices to EEPROM
                ext_eeprom_write(EEPROM_PERIPHERALS_RSI_RZI,e2_temp0);
                ext_eeprom_write(EEPROM_PERIPHERALS_RRM,e2_temp1);

				if(frontPanelOrDact == FRONT_PANEL_BUTTON_PRESS)
				{
	                //command bottom row of LEDs to stop flashing
	                latch2_pulse.latch2_pulse_byte = 0;
#if (PANEL_3_5_ZONE)
                    latch6_pulse.latch6_pulse_byte = 0;
#elif (PANEL_10_ZONE)
                    latch9_pulse.latch9_pulse_byte = 0;
#endif

					number_of_periphs = 0;

	                /* check each bit fo the EEPROM data to see how many peripherals found */
	                for (indexBit = 0; indexBit < 8; indexBit++)
					{
	                    if (e2_temp0 & (1 << indexBit))
	                    {
							number_of_periphs++;
	                    }
	                    if (e2_temp1 & (1 << indexBit))
	                    {
							number_of_periphs++;
	                    }
					}

	                update_bot_row(number_of_periphs);    //++++COMPILER BUG FIX 8/9/01

// BOC CSM 8/29/2002
//                    //LCD display "was ##  now ##" for number of periphs previously in system & newly "sniffed" ones
//                    var_dact_data[0] = 4;
//                    var_dact_data[1] = 0x0d;
//                    var_dact_data[2] = old_periph_num;  //was #
//                    var_dact_data[3] = number_of_periphs;   //now #
//                    //dialer_comm(WRITE_TO_DIALER, var_dact_data);
                    if ( dactPresent )
                    {
                        //LCD display "was ##  now ##" for number of periphs previously in system & newly "sniffed" ones
                        var_dact_data[0] = 4;
                        var_dact_data[1] = SpecialDisplay2;
                        var_dact_data[2] = old_periph_num;  //was #
                        var_dact_data[3] = number_of_periphs;   //now #
#if (DACT_CODE)
                        dialer_comm(WRITE_TO_DIALER, var_dact_data);
#endif
                    }
// EOC CSM 8/29/2002
				}


                //clear for next time we run this function
                state = POLL_START;

                //clean up "usart_comm()" for "supervise_peripherals()"
                //xmit_packet.status = SERVICE_AVAILABLE;
				//xmit_packet.command = XMIT_PACKET_EMPTY;

                //signify we're done looking for all possible peripherals
                return(1);
            }
            break;
    }//end switch statement
    //signify we're not finished looking for peripherals yet
    return(0);
}

/**************************************************************************
 ** FUNCTION: idcCompanion                                               **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function will return the Class A/Cross Zoned pair for specified **
 ** IDC.                                                                 **
 **                                                                      **
 ** PARAMETERS:  idc_num                                                 **
 **                                                                      **
 ** RETURNS:  companion IDC                                              **
 **                                                                      **
 **************************************************************************/
unsigned char idcCompanion( unsigned char idc_num )
{
#if (PANEL_3_5_ZONE)
    if ( !five_zone_panel )
    {
        if ( idc_num == IDC_3 ) return IDC_3;
    }
#endif
    return eeprom_idc_companion[idc_num];
}
