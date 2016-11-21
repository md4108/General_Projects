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
| 12/8/2005  C. McGill       Added prototype for OMCoderClearCode(). PTR 1503.
| 8/11/2005  C. McGill       Added idcStatus3 extern and added new clean me timers.
| 8/10/2005  C. McGill       Added software latch 9 for 10 zone panel.
| 8/9/2005   C. McGill       Added prototypes for non-latching IDC functions.
| 08/29/02  C. McGill        Added global dactPresent for use in determining
|                            whether to attempt dact comm and to reduce i2c
|                            traffic.
| 09/03/02  C. McGill        Added global comboDisableDelaySecs[] for use in
|                            processing combo zone disable after supv side
|                            activation. PTR #893.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   externs.h  $
|     $Date:   Dec 08 2005 15:05:02  $
|  $Modtime:   Dec 08 2005 15:03:46  $
| $Revision:   1.92  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\externs.h-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/

#ifndef H_EXTERNS
    #define H_EXTERNS

#include "io.h"
#include "tripped.h"
#include "usart_m.h"
#include "trouble.h"
#include "i2c_mst.h"


    //==== External Prototypes ======================================================
    extern void usart_comm(void);
    extern void init_usart(void);

    extern void init_i2c(void);
	extern void ext_eeprom_write(unsigned char, unsigned char);
	extern unsigned char ext_eeprom_read(unsigned char address);
    extern bit dialer_comm(unsigned char, const unsigned char *);
	extern void load_poll_for_presence_data(void);

    extern bit program(void);
    extern bit poll_for_peripherals(unsigned char);


    extern void button_handler(void);
    extern unsigned int ad_conversion(unsigned char);
    extern void scan_analog_inputs(void);
    extern void debounce_idc_levels(void);
    extern void debounce_nac_levels(void);

    extern void input_tripped_handler(void);
    extern void nac_input_handler(void);
    extern void process_idc_level_status(void);
    extern void process_nac_level_status(void);
    extern void load_thresh_levels(void);

    extern unsigned char trouble_handler(void);

    extern void normal_handler(void);

    extern void light_or_extinguish_idc_trouble_leds(unsigned char l_or_e, unsigned char idc_ctr);
    extern void light_or_extinguish_nac_trouble_leds(unsigned char l_or_e, unsigned char nac_ctr);
    extern void light_or_extinguish_idc_alarm_leds( unsigned char indexIDC, unsigned char stateLED );

    extern void ClearPeripheralStatus( unsigned char function );
    //===============================================================================


    //=== Global variables =====================================
    extern unsigned char programCleanUp;
    extern volatile unsigned char programmingSecs;


    //-------- Globals from "io.c" ------------------------------
    extern volatile unsigned char pulse_120spm_timer; //LED flash rate timer (volatile because it is also used in interrupt)
    extern volatile unsigned char pulse_2x20_timer;    //2x20 spm timer (volatile because it is dec'd in interrupt)

	extern bit buzzer_on_flag;

    extern bit buzzer_blip_flag;
//    extern unsigned char buzzer_blip_flag;  //fix compiler bug

    extern bit buzzer_enter_flag;
    extern bit trouble_LED_flag;
    extern bit pulse_2x20_flag;
    extern bit RRM_pulse_2x20_flag;
    extern bit alarm_pulse_2x20_flag;
// BOC CSM 5/17/2005
    extern bit buzzer_lockout_flag;
// EOC CSM 5/17/2005

    extern union LATCH1_BIT_BYTE latch1;
    extern union LATCH2_BIT_BYTE latch2;
    extern union LATCH3_BIT_BYTE latch3;
    extern union LATCH4_BIT_BYTE latch4;
    extern union LATCH5_BIT_BYTE latch5;
    extern union LATCH6_BIT_BYTE latch6;
#if (PANEL_10_ZONE)
    extern union LATCH7_BIT_BYTE latch7;
    extern union LATCH8_BIT_BYTE latch8;
    extern union LATCH9_BIT_BYTE latch9;
#endif

    extern union LATCH1_PULSE_BIT_BYTE latch1_pulse;
    extern union LATCH2_PULSE_BIT_BYTE latch2_pulse;
    extern union LATCH4_PULSE_BIT_BYTE latch4_pulse;
    extern union LATCH5_PULSE_BIT_BYTE latch5_pulse;
    extern union LATCH6_PULSE_BIT_BYTE latch6_pulse;
#if (PANEL_10_ZONE)
    extern union LATCH7_PULSE_BIT_BYTE latch7_pulse;
    extern union LATCH8_PULSE_BIT_BYTE latch8_pulse;
    extern union LATCH9_PULSE_BIT_BYTE latch9_pulse;
#endif
	//-----------------------------------------------------------------

    //-------- Globals from "io.c" ------------------------------
    extern near union BUTTON_BIT_BYTE button;
#if (PANEL_10_ZONE)
    extern union BUTTON_BIT_BYTE2 button2;
#endif
    extern volatile unsigned char button_valid_timer;
    extern unsigned int an4_buttons_ad_int_value;
    extern union IDC_LEVEL_BIT_BYTE idc_level[]; //current voltage level (between thresholds) of each idc zone
    extern union NAC_LEVEL_BIT_BYTE nac_level[]; //current voltage level (between thesholds) of each NAC
    extern unsigned int idc_ad_int_value[];
    extern unsigned int nac_ad_int_value[];
    extern unsigned int batt_ad_int_value;
    extern unsigned int aux_power_ad_int_value;
    extern unsigned int ground_ad_int_value;
    extern unsigned int ac_ad_int_value;

    extern unsigned char auxTimerOC;
    extern unsigned char powerLossDebounce;
    extern unsigned char powerRestoreDebounce;

    extern volatile unsigned char battery_check_timer;
    extern volatile unsigned char idcAlarmVerifySecs;

    extern unsigned char idc_x_debounce_ctr[MAX_IDC];
    extern unsigned char nac_x_debounce_ctr[MAX_NAC];

    extern unsigned char cleanMePulseValid[MAX_IDC];
    extern unsigned char cleanMeInitiationTimer[MAX_IDC];
    extern unsigned char cleanMeValidationTimer[MAX_IDC];
    extern unsigned char cleanMeRestorationTimer[MAX_IDC];
	//---------------------------------------------------------------

	//-------- Globals from "tripped.c" ----------------------------------------
    extern union IDC_STATUS_BIT_BYTE idc_status[];
    extern union IDC_STATUS_BIT_BYTE2 idc_status2[];
    extern union IDC_STATUS_BIT_BYTE3 idc_status3[];
    extern union NAC_STATUS_BIT_BYTE nac_status[];
    extern volatile unsigned char idc_x_timer[];  //volatile because timers are dec'd in interrupt

    extern volatile unsigned char alarm_sil_inhibit_timer;
// BOC CSM 5/15/2005
//    extern volatile unsigned char auto_sig_sil_timer;
    extern volatile unsigned int  auto_sig_sil_timer;
// EOC CSM 5/15/2005
    extern volatile unsigned char resetTimerSeconds;
    extern volatile unsigned char idcDisableDelaySecs[MAX_IDC];
// BOC CSM 9/3/2002
    extern volatile unsigned char comboDisableDelaySecs[MAX_IDC];
// EOC CSM 9/3/2002
    extern unsigned char drillTemporal;
    extern unsigned char DACTIdcEventLatch;

    extern void ProcessReset(void);
    extern void ProcessDrill(void);
    extern void nac_active_handler(unsigned char idc);
    extern void TurnNACOff( unsigned char indexNAC );
    extern void UpdateNonLatchingIDCs(void);
    extern unsigned char nonLatchingIDC(unsigned char idc_num);

	//--------------------------------------------------------------------------

	//-------- Globals from "usart_m.c" ----------------------------------------------------
    extern struct XMIT_PACKET_TYPE xmit_packet;
    extern struct RECEIVE_PACKET_TYPE receive_packet;
    extern const unsigned char addresses[];
    extern volatile unsigned char usart_exit_timer;
    extern volatile unsigned int  status_to_periph_exit_timer;
    extern unsigned char ignorePeripherals;
    extern unsigned char periphClassAIDCAddress[MAX_IDC];
    extern unsigned char updateRLCDTextIDC;
    extern unsigned char updateRLCDText[IDC_TEXT_LENGTH];
    extern unsigned char periphRSISubType[];

    extern unsigned char status_to_periph(unsigned char, unsigned char, unsigned char, unsigned char);
    extern void ClearCurrentUsartCommand(void);
    extern void UpdatePeriphClassAIDCAddress( void );

	//--------------------------------------------------------------------------------------

	//-------- Globals from "i2c_mst.c" -------------------------------------------
    extern unsigned char i2c_data_xmit_array[];
	extern unsigned char i2c_data_rcv_array[];
    extern volatile unsigned char exit_timer;
	extern volatile unsigned char i2c_wait_timer;
	extern unsigned char var_dact_data[];
	//-----------------------------------------------------------------------------
    //========= External CONSTANTS (from "i2c_mst.c") =========================================
    //extern const unsigned char poll_dialer_for_presence[];
	extern const unsigned char rev_number[];
	extern const unsigned char request_data_from_dact[];
	extern const unsigned char enter_DACT_program_mode[];
	extern const unsigned char exit_DACT_program_mode[];
    extern const unsigned char disable_DACT[];
	extern const unsigned char enable_DACT[];
    extern const unsigned char enter_DACT_edit_passcode[];
    extern const unsigned char enter_DACT_program_IDC_mode[];
    extern const unsigned char continue_DACT_program_mode[];
	extern const unsigned char provide_config_download[];
    extern const unsigned char key_press_ADVANCE_TOP[];
    extern const unsigned char key_press_DECREMENT_TOP[];
    extern const unsigned char key_press_ADVANCE_BOTTOM[];
    extern const unsigned char key_press_DECREMENT_BOTTOM[];
    extern const unsigned char key_press_ENTER[];
	extern const unsigned char key_press_IDC1_DISABLE[];
	extern const unsigned char key_press_IDC2_DISABLE[];
	extern const unsigned char key_press_IDC3_DISABLE[];
	extern const unsigned char key_press_IDC4_DISABLE[];
	extern const unsigned char key_press_IDC5_DISABLE[];
	extern const unsigned char key_press_IDC6_DISABLE[];
	extern const unsigned char key_press_IDC7_DISABLE[];
	extern const unsigned char key_press_IDC8_DISABLE[];
	extern const unsigned char key_press_IDC9_DISABLE[];
	extern const unsigned char key_press_IDC10_DISABLE[];
	extern const unsigned char key_press_NAC1_DISABLE[];
	extern const unsigned char key_press_NAC2_DISABLE[];
	extern const unsigned char key_press_NAC3_DISABLE[];
	extern const unsigned char key_press_NAC4_DISABLE[];
    extern const unsigned char provide_idc_text[];

    //=========================================================================================

	//-------- Globals from "main.c" ------------------------------------------------------
#if (PANEL_3_5_ZONE)
    extern unsigned char five_zone_panel;
#endif
    extern volatile bit continuous_nac1;
    extern volatile bit continuous_nac2;
    extern volatile bit temporal_nac1;
    extern volatile bit temporal_nac2;
    extern volatile bit coder_nac1;
    extern volatile bit coder_nac2;
    extern volatile bit genesis_synch_nac1;
    extern volatile bit genesis_synch_nac2;
    extern volatile bit genesis_silence_nac1;
    extern volatile bit genesis_silence_nac2;
    extern volatile unsigned char genesis_turn_on_nac1;
    extern volatile unsigned char  genesis_turn_on_nac2;
    //extern volatile bit ignoreNacOC;
	extern volatile unsigned char ignoreNacOC;	//compiler bug fix for NAC temporals turning off after 1 hour
#if (PANEL_10_ZONE)
    extern volatile bit continuous_nac3;
    extern volatile bit continuous_nac4;
    extern volatile bit temporal_nac3;
    extern volatile bit temporal_nac4;
    extern volatile bit coder_nac3;
    extern volatile bit coder_nac4;
    extern volatile bit genesis_synch_nac3;
    extern volatile bit genesis_synch_nac4;
    extern volatile bit genesis_silence_nac3;
    extern volatile bit genesis_silence_nac4;
    extern volatile unsigned char genesis_turn_on_nac3;
    extern volatile unsigned char genesis_turn_on_nac4;
#endif
    extern union SYSTEM_STATE_BIT_BYTE system_state;
    extern union PERIPHERAL_STATE_BIT_BYTE peripheral_state;
    extern union PERIPHERAL_STATE2_BIT_BYTE peripheral_state2;
	extern union COMMANDS_FROM_DACT_BIT_BYTE commands_from_dact;

    extern volatile unsigned char powerUpSecs;
    extern volatile unsigned char walkTestInactiveMins;
    extern volatile unsigned char reSynchGen;
    extern unsigned char previousStatus;
    extern unsigned char refreshStatus;
// BOC CSM 8/29/2002
    extern unsigned char dactPresent;
// EOC CSM 8/29/2002
// BOC CSM 5/19/2005
    extern unsigned char rrmCommonAlarm;
    extern void OMCoderPutCode( unsigned char idc_num );
    extern void OMCoderClearCode( unsigned char idc_num );
    extern void OMCoderReset( void );

// EOC CSM 5/19/2005

#if ( BUZZER_TEST )
    extern unsigned int buzzerToggleSecs;
    extern unsigned int buzzerPhaseMins;
#endif
	//-------------------------------------------------------------------------------------

	//-------- Globals from "progrm.c" ----------------------------------------------
    extern const unsigned char eeprom_idc_location[];
//    extern const unsigned char eeprom_idc_companion[];
    extern unsigned char idcCompanion( unsigned char idc_num );
    extern const unsigned char eeprom_idc_class_a_companion[];
    extern const unsigned char eeprom_nac_location[];
	extern unsigned char DACT_program_mode_active_flag;
	//-------------------------------------------------------------------------------

	//-------- Globals from "trouble.c" -----------------------------------------------------
    extern bit buzzerSilenced;
	extern near union TROUBLE_BIT_BYTE trouble;
    extern near union TROUBLE_BIT_BYTE2 trouble2;
	extern near union DACT_TROUBLE_BIT_BYTE dact_trouble;
    extern unsigned char oldTrouble;
    extern unsigned char oldTrouble2;
	extern unsigned char oldDactTroubs;
    extern volatile unsigned int troubleSilenceMinutes;
// BOC CSM 5/16/2005
    extern volatile unsigned int ac_delay_timer;
    extern unsigned char localACLoss, remoteACLoss;
// EOC CSM 5/16/2005
    extern void ClearTroubles(void);
	//---------------------------------------------------------------------------------------

	//-------- Globals from "trouble.c" -----------------------------------------------------
#if (PANEL_10_ZONE)
    extern unsigned int nacOverCurrent[MAX_NAC];
    extern unsigned int transformer2;
#endif
	//---------------------------------------------------------------------------------------

    //============================================================

#endif
