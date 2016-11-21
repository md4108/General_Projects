/*
+------------------------------------------------------------------------------
| Description: defines.h
| This module contains all the defines used within the panel code.
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
| 11/14/05  C. McGill        Modified definition of BATTERY_CHARGED to new tested
|                            value. Added define SP_IDC_DISABLE for use to send
|                            IDC disable status to RA10(C).
| 9/25/2005  C. McGill       Added EEPROM address locations for RSI subtypes.
| 8/24/2005  C. McGill       Modifed definition of RSI_DRILL to match new operation
|                            of UIM.
| 8/13/2005  C. McGill       Added define MAX_LATCH_HW to differentiate between
|                            software latch structures and actual hardware muxes.
| 8/10/2005  C. McGill       Updated clean me AD threshholds.
| 8/9/2005   C. McGill       Corrected RRM_COMMON_ALARM_MODE setting
| 8/5/2005   C. McGill       Updates to migrate to PIC18F2525.
| 06/12/02  C. McGill        Changed define REFRESH_SUP to REFRESH_CMN for use
|                             in refreshing peripheral for all new common
|                             activations.
| 08/09/02  C. McGill        Added define VERIFY for input_tripped_handler()
|                             state machine. PTR #812, #813, #815, #817.
|                             Added SYS_VERIFY_* defines for process_idc_level_status()
|                             alarm verify processing. PTR #806.
| 08/17/02  C. McGill        Increased IDC_DEBOUNCE time to 500mS. See if this
|                             prevents light key presses from looking like alarms
|                             PTR #851
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   defines.h  $
|     $Date:   Nov 17 2005 09:39:14  $
|  $Modtime:   Nov 14 2005 13:25:30  $
| $Revision:   1.108  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\defines.h-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/
/* EEPROM options  */

/* bit masks for EEPROM address 0x00 */
// BOC CSM 5/15/2005
//#define AUTO_NAC_SILENCE            0b00000001
//#define AUX_POWER_RESET             0b00000010
//#define NAC_SILENCE_INHIBIT         0b00000100
//#define DACT_PRESENT                0b10000000

//#define CLASS_A                     0b00010000
//#define ALARM_SILENCE_INHIBIT       0b00000100
//#define SILENCEABLE                 0b00100000
//#define CONTINUOUS                  0b00000001
//#define TEMPORAL                    0b00000010
//#define GENESIS                     0b00000100
//#define WATERFLOW_SUP_COMBO         0b00110000
//#define WATERFLOW_ANY               0b00111100
//#define WATERFLOW_RETARD            0b00010100
//#define SUP_NOT_COMBO               0b01000000
//#define MONITOR                     0b10000000
//#define ALARM_VERIFIED              0b00000010
//#define ALARM_NONVERIFIED           0b00000001
#define AUTO_NAC_SILENCE            0b00011111
#define NAC_SILENCE_INHIBIT         0b00100000
#define PASSCODE_ENABLED            0b01000000
#define DACT_PRESENT                0b10000000
#define WATERFLOW_DELAY             0b00011111
#define AC_FAIL_DELAY               0b00001111

#define CLASS_A                     0b00010000
#define IDC_NONLATCHING             0b00100000
#define IDC_CLEAN_ME_ENABLED        0b01000000
#define PARALLEL                    0b00100000
#define SILENCEABLE                 0b10000000

#define CONTINUOUS                  0b00000001
#define TEMPORAL                    0b00000010
#define GENESIS                     0b00000011
#define GENESIS_AV                  0b00000100
#define CODER                       0b00000101
#define CITY_TIE                    0b00000110
#define NAC_TYPE_MASK               0b00001111

#define CROSS_ZONED_ALARM           0b00001011
#define REMOTE_AC_FAIL              0b00001010
#define REMOTE_SILENCE_DRILL        0b00001001
#define MONITOR                     0b00001000
#define SUP_NOT_COMBO               0b00000111
#define WATERFLOW_SUP_COMBO         0b00000110
#define WATERFLOW_WR_SUP_COMBO      0b00000101
#define WATERFLOW                   0b00000100
#define WATERFLOW_WR                0b00000011
#define ALARM_VERIFIED              0b00000010
#define ALARM_NONVERIFIED           0b00000001
#define IDC_TYPE_MASK               0b00001111

#define IDC_RRM_MASK                0b00000111
#define IDC_RRM_SHIFT               4

#define IDC_CODER_MASK              0b00001111
#define IDC_CODER_DIGIT_1_3_SHIFT   4
#define IDC_CODER_DIGIT_2_4_SHIFT   0

#define NO_LOCKOUT                  0b00000000
#define IDC_LOCKOUT                 0b00000001
#define NAC_LOCKOUT                 0b00000010
#define REMOTE_DISCONNECT_LOCKOUT   0b00000100
#define IDC_NAC_LOCKOUT             ( IDC_LOCKOUT | NAC_LOCKOUT )
#define ALL_LOCKOUT                 ( IDC_NAC_LOCKOUT | REMOTE_DISCONNECT_LOCKOUT )
#define DISABLE_LOCKOUT_MASK        ALL_LOCKOUT
#define DISABLE_LOCKOUT_SHIFT       5

#define RRM_COMMON_ALARM_MODE       0b01000000
#define RRM_MATRIX_MODE             0b00000000

#define PANEL_MODE_FACP             0b00000001
#define PANEL_MODE_FACOM            0b00000010
#define PANEL_MODE_SSU              0b00000011
#define PANEL_OPERATING_MODE_MASK   0b00000011
#define PANEL_OPERATING_MODE_SHIFT  4

/********************
 * EEPROM Addresses *
 ********************/
#define EEPROM_PANEL_OPTIONS1       0x00
#define EEPROM_PANEL_OPTIONS2       0x01
#define EEPROM_PANEL_OPTIONS3       0x02
#define EEPROM_DACT_INSTALLED       EEPROM_PANEL_OPTIONS1
#define EEPROM_ENABLE_PASSCODE      EEPROM_PANEL_OPTIONS1
#define EEPROM_PANEL_OP_MODE        EEPROM_PANEL_OPTIONS1
#define EEPROM_AC_FAIL_DELAY        EEPROM_PANEL_OPTIONS1
#define EEPROM_RRM_COMMON_MODE      EEPROM_PANEL_OPTIONS2
#define EEPROM_NAC_SILENCE_INHIBIT  EEPROM_PANEL_OPTIONS2
#define EEPROM_AUTO_NAC_SILENCE     EEPROM_PANEL_OPTIONS2
#define EEPROM_WATERFLOW_DELAY      EEPROM_PANEL_OPTIONS3
#define EEPROM_DISABLE_LOCKOUT      EEPROM_PANEL_OPTIONS3

#define EEPROM_IDC_1                0x03
#define EEPROM_IDC_2                0x07
#define EEPROM_IDC_3                0x0B
#define EEPROM_IDC_4                0x0F
#define EEPROM_IDC_5                0x13
#define EEPROM_IDC_6                0x17
#define EEPROM_IDC_7                0x1B
#define EEPROM_IDC_8                0x1F
#define EEPROM_IDC_9                0x23
#define EEPROM_IDC_10               0x27

#define EEPROM_IDC_OPTIONS_OFFSET           1
#define EEPROM_IDC_CODER_DIGIT_1_2_OFFSET   2
#define EEPROM_IDC_CODER_DIGIT_3_4_OFFSET   3

#define EEPROM_NAC_1                0x2B
#define EEPROM_NAC_2                0x2C
#define EEPROM_NAC_3                0x2D
#define EEPROM_NAC_4                0x2E
#define EEPROM_LAST_CONFIG_ADDRESS  EEPROM_NAC_4

#define EEPROM_PERIPHERALS_RSI_RZI  0x2F
#define EEPROM_PERIPHERALS_RRM      0x30

#define EEPROM_DEFAULT_START        EEPROM_PERIPHERALS_RRM + 1
#define EEPROM_DEFAULT_END          EEPROM_DEFAULT_START + EEPROM_LAST_CONFIG_ADDRESS

#define EEPROM_DATABASE_VERSION     EEPROM_DEFAULT_END + 1
// EOC CSM 5/17/2005
// BOC CSM 9/25/2005
#define EEPROM_RSI2_SUBTYPE         EEPROM_DATABASE_VERSION + 1
#define EEPROM_RSI1_SUBTYPE         EEPROM_RSI2_SUBTYPE + 1
// EOC CSM 9/25/2005

/************************************************************/
/* I2C_MST.c                                                */
/************************************************************/
#define READ_FROM_DIALER            1
#define WRITE_TO_DIALER             0

#define DIALER_COMM_ERROR           1
#define DIALER_COMM_OK              0

#define I2C_XMIT_DATA_SIZE          55  //max size of "i2c_data_xmit_array[]" used for exchange of data between
                                        //main panel and dialer
#define I2C_RCV_DATA_SIZE           55

#define SCL							RC3
#define I2C_BUSY_NOT 			   	RC5

/************************************************************/
/* Debounce times - 1 count = 10 ms                         */
/************************************************************/
#define     MS20                    2
#define     MS40                    4
#define     MS50                    5
#define     MS100                   10
#define     MS200                   20
#define     MS250                   25
#define     MS500                   50
#define     MS750                   75
#define     SEC_1                   100
#define     SEC_2                   200
#define     SEC_3                   300
#define     SEC_4                   400
#define     SEC_8                   800

/* temporal counts 5 = half second */           /* temporal time frame:  */
#define     OFF_PHASE_ONE           MS500       /* half second           */
#define     OFF_PHASE_TWO           SEC_1+MS500 /* one and half seconds  */
#define     OFF_PHASE_THREE         SEC_2+MS500 /* two and half seconds  */
#define     ON_PHASE_ONE            SEC_1       /* one second            */
#define     ON_PHASE_TWO            SEC_2       /* two seconds           */
#define     ON_PHASE_THREE          SEC_4       /* four seconds          */

/************************************************************/
/* IO.c                                                     */
/************************************************************/
#define     IDC_1                   0
#define     IDC_2                   1
#define     IDC_3                   2
#define     IDC_4                   3
#define     IDC_5                   4
#define     IDC_6                   5
#define     IDC_7                   6
#define     IDC_8                   7
#define     IDC_9                   8
#define     IDC_10                  9

#define     IDC_DISABLED            975

#define     IDC_START               0
#define     IDC_OPEN                1
#define     IDC_NORMAL              2
#define     IDC_SUPERVISORY         3
#define     IDC_VERIFIED_ALARM      4
#define     IDC_NONVERIFIED_ALARM   5
#define     IDC_BIT_OFFSET          6
// BOC CSM 8/17/2002
//#define     IDC_DEBOUNCE            MS250
#define     IDC_DEBOUNCE            MS500
// EOC CSM 8/17/2002
#if (PANEL_10_ZONE)
#define     BIT_MASK_1ST_INT        16
#endif

#define     VERIFYING_ALARM         0xff
#define     NOT_VERIFYING_ALARM     0

#define     NAC_1                   0
#define     NAC_2                   1
#define     NAC_3                   2
#define     NAC_4                   3
#define     NAC_CLEAR_FLAGS         0b10000000

#define     NAC_OC_VALUE            154
#define     NAC_DISABLED            975

#define     NAC_START               0
#define     NAC_OPEN                1
#define     NAC_NORMAL              2
#define     NAC_SHORTED             3
#define     NAC_OVERCURRENT         4
#define     NAC_OVERCURRENT_WAIT    5

#if (PANEL_10_ZONE)
#define     NAC_DEBOUNCE            MS50
#define     NAC_BIT_OFFSET          1
#elif (PANEL_3_5_ZONE)
#define     NAC_DEBOUNCE            MS100
#define     NAC_BIT_OFFSET          12
#endif

#if (PANEL_10_ZONE)
#define     BUTTON_DEBOUNCE         10
#define     BUTTON_DEBOUNCE_2       25
#define     DRILL_BUTTON_DEBOUNCE   50
#elif (PANEL_3_5_ZONE)
#define     BUTTON_DEBOUNCE         20
#define     BUTTON_DEBOUNCE_2       100
#define     DRILL_BUTTON_DEBOUNCE   200
#endif
#define     BUTTON_NOT_PRESSED      0xff

#if (PANEL_10_ZONE)
#define     MAX_MUX                 3
#define     MUX_1                   0
#define     MUX_2                   1
#define     MUX_3                   2
#endif
#define     MUX_ADDRESS_0           0
#define     MUX_ADDRESS_1           1
#define     MUX_ADDRESS_2           2
#define     MUX_ADDRESS_3           3
#define     MUX_ADDRESS_4           4
#define     MUX_ADDRESS_5           5
#define     MUX_ADDRESS_6           6
#define     MUX_ADDRESS_7           7
#define     NUM_MUX_INPUTS          8

/* clean me debounce times/counts */
#define     CLEAN_ME_INITIATE_TIME          2    /* 2 second initiation period                                    */
#define     CLEAN_ME_VERIFY_TIME            128  /* 128 second validation period                                  */
#define     CLEAN_ME_RESTORE_TIME           5    /* 5 minute restoration period                                   */
#define     CLEAN_ME_VERIFY_TRIP_MIN        2    /* minimum number of valid pulses required to trip CleanMe state */
#define     CLEAN_ME_VERIFY_TRIP_MAX        30   /* maximum number of valid pulses required to trip CleanMe state */
#define     CLEAN_ME_PULSE_DETECT_COUNT     3    /* minumum # samples required to declare a valid pulse           */
#define     CLEAN_ME_PULSE_VALID_COUNT_MAX  0xFE
#define     CLEAN_ME_MIN_THRESHOLD          10   /* AD value for minimum Clean Me pulse                           */
#define     CLEAN_ME_MAX_THRESHOLD          200  /* AD value for maximum Clean Me pulse                           */

/*----------------------------------------------*/
/* Panel common                                 */
/*----------------------------------------------*/
#define     LATCH_1                 3
#define     LATCH_2                 2
#define     LATCH_3                 1
#define     LATCH_4                 0
#define     LATCH_5                 4
#define     LATCH_6                 5
#define     LATCH_7                 6
#define     LATCH_8                 7
#define     LATCH_9                 8

#define     MAX_BITS                8

#define     DATA_SET                RB3

#define     CLEAR_OUTPUT_LATCH      0b11111000

/*----------------------------------------------*/
/* 10 zone panel specific                       */
/*----------------------------------------------*/
#if (PANEL_10_ZONE)

#define     CLEAR_ADDRESS_LATCH     0b10001111

#define     MAX_LATCH               9   // 9 output latches, however....
#define     MAX_LATCH_HW            8   // only 8 actual hardware muxes...the 9th is a software construct!

#define     ADDRESS_LATCH_STROBE    RC1
#define     ADDRESS_DATA_SET        RB7

#define     NAC3_SYNC               RC2
#define     NAC4_SYNC               RA3

#define     NAC1_2_CLASS_A          RD0
#define     NAC3_4_CLASS_A          RD1

#define     COMMON_WATERFLOW        RD2
#define     COMMON_SERVICE_DETECTOR RD3
#define     AUX_POWER_CONSTANT      RD4
#define     REMOTE_DISCONNECT       RD5

#if (FSPLUS)
#define     PANEL_TYPE_10ZONE       11
#else
#define     PANEL_TYPE_10ZONE       10
#endif

#endif
/*----------------------------------------------*/
/* 3/5 zone panel specific                      */
/*----------------------------------------------*/
#if (PANEL_3_5_ZONE)
//#define     MAX_LATCH               5
#define     MAX_LATCH               6
#define     MAX_LATCH_HW            MAX_LATCH

#define     LATCH1_STROBE           RC2
#define     LATCH2_STROBE           RC1
#define     LATCH3_STROBE           RB5
#define     LATCH4_STROBE           RB4
#define     LATCH5_STROBE           RB7

#if (FSPLUS)
#define     PANEL_TYPE_3ZONE        4
#define     PANEL_TYPE_5ZONE        6
#else
#define     PANEL_TYPE_3ZONE        3
#define     PANEL_TYPE_5ZONE        5
#endif

#endif
/*----------------------------------------------*/

#define     BATTERY_SUPERVISE_OFF   0
#define     BATTERY_SUPERVISE_ON    1
#define     BATTERY_SUPERVISE_CHECK 2
#define     BATTERY_CHARGE          3
#define     BATTERY_CHARGE_OFF      4
#define     BATTERY_CHARGE_CHECK    5

//#define     BATTERY_CHARGED         645     //27.3 v
//#define     BATTERY_CHARGED         624     //26.4 v
#define     BATTERY_CHARGED         637     //26.4 v
#define     BATTERY_LOW_RESTORE     529     //22.4 v
#define     BATTERY_LOW_SET         506     //21.4 v
#define     BATTERY_SHED_LOADS      474     //20.2 v
#define     BATTERY_REALLY_LOW      458     //19.4 v

/* Battery Level defines */
#define     V19_4                   0
#define     V20_4                   1
#define     V21_4                   2
#define     V22_4                   3
#define     V23_4                   4
#define     V24_4                   5
#define     V25_4                   6
#define     V26_4                   7
#define     V27_4                   8
#define     BATTERY_MAX_VOLTAGES    9

#define     BATTERY_VOLTAGE         0
#define     BATTERY_IDC_OPEN        1
#define     BATTERY_IDC_SUP         2
#define     BATTERY_IDC_VER         3
#define     BATTERY_IDC_NON         4
#define     BATTERY_NAC_B_OPEN      5
#define     BATTERY_NAC_B_SHORT     6
#define     BATTERY_NAC_A_OPEN      7
#define     BATTERY_NAC_A_SHORT     8
#define     BATTERY_GF_LOW          9
#define     BATTERY_GF_HIGH         10
#define     BATTERY_AUX_OC          11
#define     BATTERY_MAX_ENTRIES     12

#define     BATTERY2_IDC_OPEN_ODD   0
#define     BATTERY2_IDC_SUP_ODD    1
#define     BATTERY2_IDC_VER_ODD    2
#define     BATTERY2_IDC_NON_ODD    3
#define     BATTERY2_NAC_P_OPEN     4
#define     BATTERY2_NAC_P_SHORT    5
#define     BATTERY2_MAX_ENTRIES    6

#define     NAC_1_2                 0b00000110
#define     NAC_3_4                 0b00000011

#define     LOW                     0
#define     HIGH                    1

#define     NOT_RUNNING             0xff
#define     NOT_RUNNING_INT         0xffff

/* trouble silence annuciate after 24 hours (in minutes) */
/* if silence pressed and trouble still present.         */
#define     TROUBLE_SILENCE_MINUTES 1440 /* 1440 minutes = 24 hours */

/************************************************************/
/* Main.c                                                   */
/************************************************************/
#define     REMAIN_IN_PRESENT_STATE 0
#define     NORMAL_STATE            1
#define     INPUT_TRIPPED_STATE     2
#define     TROUBLE_STATE           3
#define     RESET_STATE             4
#define     PROGRAM_STATE           5
#define     DRILL_STATE             6
#define     WALK_TEST_STATE         7

#define     WALK_TEST_RESET_START   0
#define     WALK_TEST_RESET_RESET   1
#define     WALK_TEST_RESET_WAIT    2

#define     WALK_TEST_INACTIVITY    30

#define     IDC_WALK_TEST           0b10000000

#define     REFRESH                 0x80
#define     WALKTEST_REFRESH        0x40
#define     REFRESH_CMN             0x20
#define     ALL                     0x01

/************************************************************/
/* Normal.c                                                 */
/************************************************************/

/************************************************************/
/* Progrm.c                                                 */
/************************************************************/
#define PROGRAM_START               0
#define PROGRAM_UPDATE_PERIPHERALS  1
#define PROGRAM_FIRST               2
#define PROGRAM_TROUBLE_SILENCE     3
#define PROGRAM_BUTTONS             4

#define PRGM_STILL_IN_PROGRESS      0
#define PRGM_FINISHED               1
#define PRGM_NO_NAC                 0xff

#define POLL_START                  0
#define POLL_PERIPHERAL             1
#define POLL_WAIT_FOR_ACK           2
#define POLL_NEXT_PERIPHERAL        3

#define FRONT_PANEL_BUTTON_PRESS	0
#define DACT_REQUESTED				1

#define PROGRAM_ADVANCE_TOP         1
#define PROGRAM_DECREMENT_TOP       2
/*----------------------------------------------*/
/* programming steps                            */
/*----------------------------------------------*/
// BOC CSM 5/16/2005
//#define BLANK_DISPLAY               0
//#define LOAD_DEFAULTS               1
//#define FIND_PERIPHERALS            2
//#define AUTO_NAC_SILENCE_TIMER      3
//#define AUX_PWR_RESET               4
//#define NAC_SILENCE_INHIBIT_TIMER   5
//#define IDC_ZONE_TYPES              6
//#define IDC_CLASS                   7
//#define NAC_CLASS                   8
//#define NAC_OUTPUT                  9
//#define NAC_SILENCEABLE             10
//#define PROGRAM_DACT                11
#define BLANK_DISPLAY				0
#define LOAD_DEFAULTS               1
#define EDIT_PASSCODE               2
#define ENABLE_PASSCODE             3
#define PANEL_OPERATING_MODE        4
#define NAC_SILENCE_INHIBIT_TIMER   5
#define AC_FAIL_DELAY_TIMER         6
#define WATERFLOW_DELAY_TIMER       7
#define DISABLE_LOCKOUT             8
#define FIND_PERIPHERALS            9
#define MULTI_RRM_MODE_SELECT       10
#define IDC_ZONE_TYPES              11
#define IDC_CLASS                   12
#define IDC_NON_LATCHING            13
#define IDC_CLEAN_ME                14
#define IDC_RRM_ASSIGNMENT          15
#define IDC_CODER_DIGIT1_ASSIGNMENT 16
#define IDC_CODER_DIGIT2_ASSIGNMENT 17
#define IDC_CODER_DIGIT3_ASSIGNMENT 18
#define IDC_CODER_DIGIT4_ASSIGNMENT 19
#define IDC_DESCRIPTION             20
#define NAC_OUTPUT                  21
#define NAC_CLASS                   22
#define AUTO_NAC_SILENCE_TIMER      23
#define NAC_SILENCEABLE             24
#define PROGRAM_DACT                25
#define NUM_PROGRAM_STEPS           26
// EOC CSM 5/16/2005

/************************************************************/
/* Tripped.c                                                */
/************************************************************/
#define START                       0x00   //0
#define ALARM1                      0x01   //1
#define ALARM2                      100    //100
#define ALARM3                      115    //115
#define SUPERVISORY1                0x02   //2
#define SUPERVISORY2                101    //101
#define SUPERVISORY3                102    //102
#define SUPERVISORY4                103    //103
#define SUPERVISORY_DISABLED        104    //104
#define MONITOR1                    0x04   //4
#define MONITOR2                    105    //105
#define MONITOR_CLEAR               106    //106
#define MONITOR_DISABLE             116    //106
#define WATERFLOW1                  0x08   //8
#define WATERFLOW_ALARM             0x09   //9
#define DISABLED1                   0x20   //32
#define TROUBLE1                    0x10   //16
#define TROUBLE2                    107    //107
#define TROUBLE3                    108    //108
#define DISABLED2                   109    //109
#define DISABLED3                   110    //110
#define SHORTED1                    111    //111
#define OVER_CURRENT1               112    //112
#define OVER_CURRENT2               113    //113
#define SUPERVISORY_COMBO           114    //114
// BOC CSM 8/9/2002
#define VERIFY                      0x40
// EOC CSM 8/9/2002

#define NAC_INPUT_START             0
#define NAC_INPUT_TROUBLE1          0x01
#define NAC_INPUT_TROUBLE2          3
#define NAC_INPUT_TROUBLE3          5
#define NAC_INPUT_SHORTED1          0x02
#define NAC_INPUT_OVER_CURRENT1     0x04
#define NAC_INPUT_OVER_CURRENT2     6
#define NAC_INPUT_DISABLED1         0x08
#define NAC_INPUT_DISABLED2         7
#define NAC_INPUT_DISABLED3         9

#define AC_ON_THRESH_0              40
#define AC_ON_THRESH_1              241
#define AC_ON_THRESH_2              412
#define AC_ON_THRESH_3              602
#define AC_ON_THRESH_4              922

#define AC_ON_NAC_THRESH_0          196 // <: open
#define AC_ON_NAC_THRESH_1          606 // >: short

#define AC_ON_NAC_A_THRESH_0        106 // <: open
#define AC_ON_NAC_A_THRESH_1        398 // >: short

#define AC_ON_NAC_THRESH_2          154 // <: over current (same for both CLASS A&B)

#define IDC_LEVEL_START                 0
#define IDC_LEVEL_SUPERVISORY           1
#define IDC_LEVEL_PROGRAMMED            2
#define IDC_LEVEL_UNVERIFIED_ALARM      5
#define IDC_LEVEL_VERIFYING_UNVER_ALARM 6
#define IDC_LEVEL_VER_UNVER_ALARM       7
#define IDC_LEVEL_WATERFLOW1            8
#define IDC_LEVEL_SUPERVISORY_NOT_COMBO 9
#define IDC_LEVEL_MONITOR               10
#define IDC_LEVEL_OPEN                  11
#define IDC_LEVEL_SUPERVISORY_COMBO     12
#define IDC_LEVEL_VERIFYING_ALARM       13
#define IDC_LEVEL_VERIFIED_ALARM        14
#define IDC_LEVEL_WATERFLOW2            15
#define IDC_LEVEL_WATERFLOW3            16
#define IDC_LEVEL_WATERFLOW4            17
#define IDC_LEVEL_REMOTE_AC_FAIL        18
#define IDC_LEVEL_REMOTE_SILENCE_DRILL  19
#define IDC_LEVEL_REMOTE_SILENCE_DRILL2 20
#define IDC_LEVEL_CROSS_ZONED_ALARM     21

#define NAC_LEVEL_START             0
#define NAC_LEVEL_OPEN              0x01
#define NAC_LEVEL_NORMAL            0x02
#define NAC_LEVEL_SHORTED           0x04
#define NAC_LEVEL_OVERCURRENT       0x08

#define PULSE                       0b10000000
#define LIGHT                       1   //#defines for "l_or_e" parameter
#define EXTINGUISH                  0
#define IDC                         0   //#defines for "idc_or_nac" parameter
#define NAC                         1

#define NOT_ACTIVATED               0
#define NAC_ACTIVE_TIMER_START      1
#define NAC_ACTIVE_START            2
#define LOOK_FOR_SILENCE            3
#define NAC_SILENCED                4
// BOC CSM 5/22/2005
#define CLEAR_NONLATCHING           0x60
// EOC CSM 5/22/2005
#define CLEAR_STATUS                0x70

#define ALARM_VERIFY_RESET          9
#define ALARM_VERIFY_STARTUP        16
#define ALARM_VERIFY_CONFIRM        80

#define RESET_FINISHED              0xff
#define RESET_PROCESS               0
#define RESET_TIMEOUT_RESET         9
#define RESET_TIMEOUT_STARTUP       16

#define RESET_START                 0
#define RESET_TURN_OFF              1
#define RESET_UPDATE_PERIPHERALS    2
#define RESET_TURN_ON_WAIT          3
#define RESET_TROUBLE               4
#define RESET_DONE                  5

#define DRILL_START                 0
#define DRILL_NACS_ON               1
#define DRILL_NACS_OFF              2
// BOC CSM 8/9/2002

#define SYS_VERIFY_IDLE             0
#define SYS_VERIFY_BEGIN            1
#define SYS_VERIFY_RESET            2
#define SYS_VERIFY_STARTUP          3
#define SYS_VERIFY_CONFIRM          4
// EOC CSM 8/9/2002

/************************************************************/
/* Trouble.c                                                */
/************************************************************/
#define SYSTEM_TROUBLES             0b11111111
#define SYSTEM_TROUBLES2            0b00011111

/************************************************************/
/* Usart_m.c                                                */
/************************************************************/
//use for "sys_or_idc_or_nac" parameter
#define NO_COMMAND                  0
#define NO_IDC                      0xff
#define IDC_STAT_CHANGE             1
#define SYS_STAT_CHANGE             2
#define NAC_STAT_CHANGE             3
#define LAMP_TEST                   4
#define TROUBLE_SILENCE             5
#define REQUEST_STAT                6
#define CLEAR_STAT                  7
#define UPDATE_RLCD_IDC_TEXT        8

#define OK_TO_POLL                  1
#define PERIPHERALS_BUSY            2

//use for "sys_idc_nac_status"
#define SP_ALARM                    0b00000001
#define SP_SUPERVISORY              0b00000010
#define SP_MONITOR                  0b00000100
#define SP_TROUBLE                  0b00001000
#define SP_AC_LOSS                  0b00010000
#define SP_IDC_DISABLE              0b00010000
#define SP_GND_FAULT                0b00100000
#define SP_CLEAR                    0b00000000

//return values
#define USART_BUSY                  0
#define FINISHED                    1
#define COMMAND_REJECT              2

//RSI subtypes - new peripherals supported here
#define RSI_ORIGINAL                0
#define RSI_RA10                    1
#define RSI_RA10C                   2
#define RSI_RLCD                    3
#define RSI_UIM                     4

#define IDC_TEXT_LENGTH             16

//======= xmit_packet commands ======================
#define IDC_STATUS_CHANGE           0x01
#define NAC_STATUS_CHANGE           0x02
#define SYSTEM_STATUS_CHANGE        0x03
#define POLL_FOR_PRESENCE           0x04
#define BUZZER_SIL                  0x05
#define REQUEST_STATUS              0x06
#define LAMP_TEST_COMMAND           0x07
#define CLEAR_ALL                   0x08
#define IDC_TEXT_UPDATE             0x09
//===================================================

//======= xmit packet command byte packet status ==========
#define XMIT_PACKET_EMPTY           0
//=========================================================

#define MAX_XMIT_PACKET_DATA_INDEX  2
//======= xmit_packet status conditions ============
#define SERVICE_AVAILABLE           0
#define IN_USE                      1
#define REATTEMPT_IN_PROGRESS       2
#define COMM_FAILURE                3
#define COMM_FAILURE_ACKED          4
#define RECEIVE_DATA_VALID          5
#define RECEIVE_DATA_READ           6
//===================================================

#define MAX_RCV_PACKET_DATA_INDEX   2
//======= receive packet commands ===================
#define RCV_ACK                     1
#define RCV_NACK                    2
//===================================================
//======= xmit_packet addresses =====================
#define RSI1                        0b00000001  //Remote System Indicator 1
#define RSI2                        0b00000000  //Remote System Indicator 2

#define RZI1Z1_5                    0b00001001  //Remote Zone Indicator 1 (zones 1 - 5)
#define RZI1Z6_10                   0b00001011  //                        (zones 6 - 10)
#define RZI1Z11_15                  0b00001101  //                        (zones 11 - 15)

#define RZI2Z1_5                    0b00001000  //Remote Zone Indicator 2 (zones 1 - 5)
#define RZI2Z6_10                   0b00001010  //                        (zones 6 - 10)
#define RZI2Z11_15                  0b00001100  //                        (zones 11 - 15)

#define RRM1Z1_5                    0b00010001  //Remote Relay Module 1   (zones 1 - 5)
#define RRM1Z6_10                   0b00010011  //                        (zones 6 - 10)
#define RRM1Z11_15                  0b00010101  //                        (zones 11 - 15)
#define RRM1_COM                    0b00010111  //                        (common indications)

#define RRM2Z1_5                    0b00010000  //Remote Relay Module 2   (zones 1 - 5)
#define RRM2Z6_10                   0b00010010  //                        (zones 6 - 10)
#define RRM2Z11_15                  0b00010100  //                        (zones 11 - 15)
#define RRM2_COM                    0b00010110  //                        (common indications)

#define RRM_DISABLED                0b10000000  //RRM's disable jumper in place
#define RSI_ORIGINAL_LAMP_TEST      0b01000000  //RSI has started a lamp test, inform other peripherals
#define RSI_LAMP_TEST               0b00100000  //RSI (UIM/RA10) has started a lamp test, command system
//#define RSI_DRILL                   0b00010000  //RSI (UIM/RA10) has started drill, command system
#define RSI_TROUBLE                 0b00001000  //RSI (UIM/RA10) has indicated trouble, inform system
#define RSI_RESET                   0b00000100  //RSI (UIM/RA10) has started reset, command system
#define RSI_SIGNAL_SILENCE          0b00000010  //RSI (UIM/RA10) has started signal silence, command system
#define RSI_DRILL                   RSI_SIGNAL_SILENCE  //RSI (UIM/RA10) has started drill, command system
#define RSI_PANEL_SILENCE           0b00000001  //RSI (UIM/RA10) has started panel silence, command system
//===================================================
#define ADDRESS_RZI                 2
#define ADDRESS_RRM                 8
#define ADDRESSES_SIZE              16

//======= status peripherals ========================
#define STATUS_START                0
#define STATUS_ADDRESS              1
#define STATUS_UPDATE               2
#define STATUS_RESPONSE             3
//===================================================

//======= supervise_peripherals =====================
#define SUPERVISE_START             0
#define SUPERVISE_ADDRESS           1
#define SUPERVISE_POLL              2
#define SUPERVISE_RESPONSE          3
//===================================================

//======= usart_comm ================================
#define USART_START                 0
#define USART_SEND_COMMAND          1
#define USART_SEND_DATA             2
#define USART_SEND_CHECKSUM         3
#define USART_RECEIVE               4
#define USART_STATUS                5
//===================================================
/*
 **********************************************************
 *
 *	define the Panel->DACT command byte values
 *  from DACT i2cmsg.h
 */

#define DACT_Poll   	 1
#define DACT_Request   	 2
#define DACT_LCD_Token 	 3
#define Stat_Delta   	 4
#define Key_Press   	 5
#define Stat_Update   	 6
#define Config_Down   	 7
#define IDC_Text_Down    8
#define DACT_Cmd   		 9
#define DACT_LCD_Text  	10
#define Config_Up   	11
#define SpecialDisplay1	12
#define SpecialDisplay2	13
#define SpecialDisplay3 14
#define DACT_PassThru 	15
#define DACT_MaxCMD 	15

#define DACT_LCD_Line_1  0
#define DACT_LCD_Line_2  1

/*
 * Programming Keys ...
 * from DACT i2cmsg.h
 */

#define QueIncrement	0
#define QueDecrement	1
#define DataIncrement	2
#define DataDecrement	3
#define DataEnter		4
#define	Key_IDCZ01		5
#define Key_IDCZ02      6
#define Key_IDCZ03      7
#define Key_IDCZ04      8
#define Key_IDCZ05      9
#define Key_IDCZ06      10
#define Key_IDCZ07      11
#define Key_IDCZ08      12
#define Key_IDCZ09      13
#define Key_IDCZ10      14
#define Key_IDCZ11      15
#define Key_IDCZ12      16
#define Key_IDCZ13      17
#define Key_IDCZ14      18
#define Key_IDCZ15      19
#define Key_IDCN01      20
#define Key_IDCN02      21
#define Key_IDCN03      22
#define Key_IDCN04      23

