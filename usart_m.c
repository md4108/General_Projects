/*
+------------------------------------------------------------------------------
| Description: usart_m.c
| This module contains all functions associated with USART communication
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
| 11/27/2006 C. McGill       Modified status_to_periph() to exit if no devices
|                            are configured.
| 9/25/2005  C. McGill       Modified UpdatePeriphClassAIDCAddress() to read RSI
|                            subtype from EEPROM.
| 8/14/2005  C. McGill       In supervise_peripherals(), modified to extend life
|                            of lamp test activation and removed single activation
|                            restriction.
| 8/9/2005   C. McGill       Corrected to clear e3_data on reset. Added errata
|                            changes for PIC18F2525.
| 06/30/02  C. McGill        In supervise_peripherals(), modified to use ANSI
|                             unsigned char variables instead of compiler specific
|                             'bit' variables. Wary that this might be causing
|                             false peripheral trouble indications. PTR #682.
| 08/04/02  C. McGill        In supervise_peripherals(), added debounce of
|                             comm error detection.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   usart_m.c  $
|     $Date:   Jan 05 2007 17:00:56  $
|  $Modtime:   Dec 18 2006 19:29:56  $
| $Revision:   1.42  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\usart_m.c-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/


#include "config.h"
#include <pic18.h>
#include <string.h>
#include "defines.h"
#include "main.h"
#include "externs.h"
#include "usart_m.h"

#define BAD_COMM_TEST   0

//=== Global Variable Declarations (extern statement in file: "externs.h") ========
struct XMIT_PACKET_TYPE xmit_packet;

struct RECEIVE_PACKET_TYPE receive_packet;

const unsigned char addresses[ADDRESSES_SIZE] =
{
    /* valid RSI addresses */
    RSI1, RSI2,

    /* valid RZI addresses */
    RZI1Z1_5, RZI1Z6_10, RZI1Z11_15, RZI2Z1_5, RZI2Z6_10, RZI2Z11_15,

    /* valid RRM addresses */
    RRM1Z1_5, RRM1Z6_10, RRM1Z11_15, RRM1_COM, RRM2Z1_5, RRM2Z6_10, RRM2Z11_15, RRM2_COM
};

volatile unsigned char usart_exit_timer;
volatile unsigned int  status_to_periph_exit_timer;
unsigned char ignorePeripherals = 0;
// BOC CSM 5/31/2005
unsigned char periphClassAIDCAddress[MAX_IDC];
unsigned char updateRLCDTextIDC = MAX_IDC;
unsigned char updateRLCDText[IDC_TEXT_LENGTH] = "";
unsigned char periphRSISubType[2] =
{
    /* Assume original style RSI */
    RSI_ORIGINAL,
    RSI_ORIGINAL
};
// EOC CSM 5/31/2005
//=================================================================================

//=== Local (to module) variable declarations =============
static unsigned char command = NO_COMMAND, currentIDC = NO_IDC;
static unsigned char indexPeripheral = 0;
static unsigned char statePoll = SUPERVISE_START;
static unsigned char stateStatus = STATUS_START;
//=========================================================

#if (BAD_COMM_TEST)
static unsigned char numBad = 0;
#endif

/**************************************************************************
 ** FUNCTION: ClearCurrentUsartCommand                                   **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function clears the current command being processed by e        **
 ** status_to_periph.                                                    **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ClearCurrentUsartCommand(void)
{
#if (USART_CODE)
    /* no active command */
    command = NO_COMMAND;

    /* no active IDC */
    currentIDC = NO_IDC;

    /* start at first peripheral */
    indexPeripheral = 0;

    /* restart state machines */
    statePoll = SUPERVISE_START;
    stateStatus = STATUS_START;

#endif
}
// BOC CSM 5/31/2005

/**************************************************************************
 ** FUNCTION: UpdatePeriphClassAIDCAddress                               **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function adjusts the IDC address based on whether configured as **
 ** Class A. This reduces the address sent to the peripherals such that  **
 ** a 10z panel configured as all Class A, uses only 5 IDC addresses and **
 ** thus only one RZI.                                                   **
 **                                                                      **
 ** PARAMETERS: N/A                                                      **
 **                                                                      **
 ** RETURNS:  N/A                                                        **
 **                                                                      **
 **************************************************************************/
void UpdatePeriphClassAIDCAddress( void )
{
#if (USART_CODE)
    unsigned char indexIDC, addressAdjustmentClassA = 0;

    for ( indexIDC = IDC_1; indexIDC < MAX_IDC; indexIDC++ )
    {
        /* set adjusted address */
        periphClassAIDCAddress[indexIDC] = indexIDC - addressAdjustmentClassA;

        /* check class a config */
        if ( ext_eeprom_read( eeprom_idc_location[indexIDC] + EEPROM_IDC_OPTIONS_OFFSET ) & CLASS_A )
        {
            /* update address adjustment factor */
            addressAdjustmentClassA++;

            /* mark class a secondary as invalid */
            indexIDC++;
            periphClassAIDCAddress[indexIDC] = MAX_IDC;
        }
    }

    /* we MAY need to update RLCD with IDC text */
    periphRSISubType[RSI1] = ext_eeprom_read( EEPROM_RSI1_SUBTYPE );
    periphRSISubType[RSI2] = ext_eeprom_read( EEPROM_RSI2_SUBTYPE );
    if ( periphRSISubType[RSI1] == RSI_RLCD || periphRSISubType[RSI2] == RSI_RLCD )
    {
        updateRLCDTextIDC = IDC_1;
    }
#endif
}

// EOC CSM 5/31/2005
/**************************************************************************
 ** FUNCTION: status_to_periph                                           **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function sends a command to the appropriate peripheral.         **
 **                                                                      **
 ** PARAMETERS: peripheral_command - command to be sent                  **
 **             idc_num - IDC (zone) that is to be updated               **
 **             sys_idc_nac_status - bit masked IDC status               **
 **                                                                      **
 ** RETURNS:  USART_BUSY - has not completed sending command to all      **
 **                        peripherals.                                  **
 **           COMMAND_REJECT - command rejected until current command    **
 **                            completes                                 **
 **           FINISHED - current command has completed.                  **
 **                                                                      **
 **************************************************************************/
unsigned char status_to_periph(unsigned char peripheral_command, unsigned char idc_num, unsigned char sys_idc_nac_status, unsigned char sys_status2)
{
// BOC CSM 5/19/2005
//    static unsigned char e2_data1, e2_data2;
    static unsigned char e2_data1, e2_data2, e2_data3 = 0;
    unsigned char orig_idc_num = idc_num;
// EOC CSM 5/19/2005
    static bit command_not_sent = 0;
// BOC CSM 5/18/2005
#if (USART_CODE)
// EOC CSM 5/18/2005
// BOC CSM 5/31/2005
    /* adjust incomming IDC address */
    idc_num = periphClassAIDCAddress[idc_num];

    /* skip Class A secondary */
    if ( idc_num == MAX_IDC )
    {
        return(FINISHED);
    }
// EOC CSM 5/31/2005

    /* if no command current, process this command */
    if ( command == NO_COMMAND )
    {
        command = peripheral_command;
        currentIDC = idc_num;
        e2_data3 = 0;
    }
    /* need to hold off new commands until current command is processed for all peripherals */
    else if ( peripheral_command != command )
    {
        return(COMMAND_REJECT);
    }
    /* if IDC_STAT command the same */
    else if ( peripheral_command == IDC_STAT_CHANGE )
    {
        /* make sure IDC is the same */
        if ( currentIDC != idc_num )
        {
            /* if currentIDC not empty */
            if (currentIDC != NO_IDC )
            {
                /* no same IDC, reject until later */
                return(COMMAND_REJECT);
            }
            /* if currentIDC is empty, update, and continue on */
            else
            {
                currentIDC = idc_num;
            }
        }
    }

    /* if ignoring peripherals, do not try to communicate */
    if ( !ignorePeripherals )
    {
        /* IDCs start at 0, but peripherals need start at 1 */
        idc_num++;

        switch ( stateStatus )
        {
            case STATUS_START:

#if (USART_TEST)
                latch4.latch4_output_bit.idc1_trbl = 1;
                latch4.latch4_output_bit.idc2_trbl = 0;
                latch4.latch4_output_bit.idc3_trbl = 0;
                latch5.latch5_output_bit.idc4_trbl = 0;
#endif

                /* retrieve known peripherals from EEPROM */
                e2_data1 = ext_eeprom_read(EEPROM_PERIPHERALS_RSI_RZI);
                e2_data2 = ext_eeprom_read(EEPROM_PERIPHERALS_RRM);
// BOC CSM 11/22/2006
                if ( !e2_data1 && ! e2_data2 ) indexPeripheral = ADDRESSES_SIZE;
// EOC CSM 11/22/2006
                stateStatus = STATUS_ADDRESS;
                break;

            case STATUS_ADDRESS:

#if (USART_TEST)
                latch4.latch4_output_bit.idc1_trbl = 0;
                latch4.latch4_output_bit.idc2_trbl = 1;
                latch4.latch4_output_bit.idc3_trbl = 0;
                latch5.latch5_output_bit.idc4_trbl = 0;
#endif
                /* if bad peripheral, set flag (latching - can only be Reset) */
                if (command_not_sent)
                {
                    //set peripheral trouble flag
                    trouble.trouble_bit.periph_cmnd = 1;
                }

                /* clear command not sent flag */
                command_not_sent = 0;

                /* if all addresses not yet updated */
                if ( indexPeripheral < ADDRESSES_SIZE )
                {
                    /* if lamp test and looking at RRMs, bypass command, RRMs do have lamp test */
                    if ( peripheral_command == LAMP_TEST && indexPeripheral >= ADDRESS_RRM )
                    {
                        indexPeripheral = ADDRESSES_SIZE;
                    }
                    /* see if any peripherals present in system (1st EEPROM loc) */
                    else if ( (indexPeripheral <= 7) && (e2_data1 & (1 << indexPeripheral)) )
                    {
                        //if so, go see if the passed status info should be sent
                        stateStatus = STATUS_UPDATE;
                    }
                    /* see if any peripherals present in system (2nd EEPROM loc) */
                    else if ( indexPeripheral > 7 && (e2_data2 & (1 << (indexPeripheral-8))) )
                    {
                        /* if so, go see if the passed status info should be sent */
                        stateStatus = STATUS_UPDATE;
                    }
                    else
                    {
                        /* peripheral not in system, increment index and remain in this state */
                        indexPeripheral++;
                    }
                }
                /* address index out of range (all peripherals updated), done with command */
                else
                {
                    stateStatus = STATUS_START;

                    /* restart process */
                    ClearCurrentUsartCommand();
// BOC CSM 5/19/2005
                    e2_data3 = 0;
// EOC CSM 5/19/2005
#if (USART_TEST)
                    latch4.latch4_output_bit.idc1_trbl = 0;
                    latch4.latch4_output_bit.idc2_trbl = 0;
                    latch4.latch4_output_bit.idc3_trbl = 0;
                    latch5.latch5_output_bit.idc4_trbl = 0;
                    latch5.latch5_output_bit.idc5_trbl = 0;
#endif
                    return(FINISHED);
                }
                break;

            case STATUS_UPDATE:

#if (USART_TEST)
                latch4.latch4_output_bit.idc1_trbl = 0;
                latch4.latch4_output_bit.idc2_trbl = 0;
                latch4.latch4_output_bit.idc3_trbl = 1;
                latch5.latch5_output_bit.idc4_trbl = 0;
#endif
                /* give usart_comm 3 seconds to complete communication to 1 peripheral */
                status_to_periph_exit_timer = SEC_3;

                if (xmit_packet.status == SERVICE_AVAILABLE)
                {
                    if ( peripheral_command == CLEAR_STAT )
                    {
                        PERIPH_COMM(addresses[indexPeripheral], CLEAR_ALL, 0, 0, 0);
                        stateStatus = STATUS_RESPONSE;
                    }
                    else
                    {
                        //indexPeripheral contains number corresponding to peripheral found in previous state
                        switch (indexPeripheral)
                        {
                            //RSI
                            case 0:
                            case 1:

                                stateStatus = STATUS_RESPONSE;
                                if ( peripheral_command == SYS_STAT_CHANGE )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], SYSTEM_STATUS_CHANGE, sys_idc_nac_status, sys_status2, 0);
                                }
                                else if ( peripheral_command == TROUBLE_SILENCE )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], BUZZER_SIL, 0, 0, 0);
                                }
                                else if ( peripheral_command == LAMP_TEST )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], LAMP_TEST_COMMAND, 0, 0, 0);
                                }
                                else if ( peripheral_command == UPDATE_RLCD_IDC_TEXT && periphRSISubType[indexPeripheral] == RSI_RLCD )
                                {
                                    PERIPH_COMM_TEXT(addresses[indexPeripheral], IDC_TEXT_UPDATE, idc_num, updateRLCDText );
                                }
                                else
                                {
                                    indexPeripheral++;
                                    stateStatus = STATUS_ADDRESS;
                                }
                                break;

                            //RZI
                            case 2:
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                            case 7:

                                if ( peripheral_command == IDC_STAT_CHANGE )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], IDC_STATUS_CHANGE, idc_num, sys_idc_nac_status, 0);
                                    stateStatus = STATUS_RESPONSE;
                                }
                                else if ( peripheral_command == LAMP_TEST )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], LAMP_TEST_COMMAND, 0, 0, 0);
                                    stateStatus = STATUS_RESPONSE;
                                }
                                else
                                {
                                    indexPeripheral++;
                                    stateStatus = STATUS_ADDRESS;
                                }
                                break;

                            //RRM1 Zones
// BOC CSM 5/19/2005
//                            case 8:
//                            case 9:
//                            case 10:
//                            case 12:
//                            case 13:
//                            case 14:
                            case 8:
                            case 9:
                            case 12:
                            case 13:
// EOC CSM 5/19/2005

                                if ( peripheral_command == IDC_STAT_CHANGE )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], IDC_STATUS_CHANGE, idc_num, sys_idc_nac_status, 0);
                                    stateStatus = STATUS_RESPONSE;
                                }
                                else if ( peripheral_command == REQUEST_STAT )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], REQUEST_STATUS, 0, 0, 0);
                                    stateStatus = STATUS_RESPONSE;
                                }
                                else
                                {
                                    indexPeripheral++;
                                    stateStatus = STATUS_ADDRESS;
                                }
                                break;

                        //RRM 1 or 2 annunciating COMMON
                            case 11:
                            case 15:

                                if ( peripheral_command == SYS_STAT_CHANGE )
                                {
// BOC CSM 5/17/2005
                                    //ac fail - rrm trouble bit should follow panel relay
                                    if ( !latch4.latch4_output_bit.trouble_relay )
                                    {
                                        //clear trouble bit to rrm
                                        sys_idc_nac_status &= 0b11110111;
                                    }
// EOC CSM 5/17/2005
                                    PERIPH_COMM(addresses[indexPeripheral], SYSTEM_STATUS_CHANGE, sys_idc_nac_status, 0, 0);
                                    stateStatus = STATUS_RESPONSE;
                                }
                                else if ( peripheral_command == REQUEST_STAT )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], REQUEST_STATUS, 0, 0, 0);
                                    stateStatus = STATUS_RESPONSE;
                                }
                                else
                                {
                                    indexPeripheral++;
                                    stateStatus = STATUS_ADDRESS;
                                }
                                break;
// BOC CSM 5/19/2005
                        //RRM 1 or 2 Matrix
                            case 10:
                            case 14:

                                // If status change, check for alarm activation
                                if ( peripheral_command == SYS_STAT_CHANGE )
                                {
                                    if ( rrmCommonAlarm )
                                    {
                                        if ( !e2_data3 ) e2_data3 = 5;
                                        // Send current IDC alarm state
                                        PERIPH_COMM(addresses[indexPeripheral], IDC_STATUS_CHANGE, e2_data3 + 10, sys_idc_nac_status & 0b00000001, 0);
                                        e2_data3--;
                                        stateStatus = STATUS_RESPONSE;
                                    }
                                }
                                else if ( peripheral_command == IDC_STAT_CHANGE )
                                {
                                    if ( !rrmCommonAlarm )
                                    {
                                        e2_data3 = (ext_eeprom_read(eeprom_idc_location[orig_idc_num]) >> IDC_RRM_SHIFT ) & IDC_RRM_MASK;
                                        if ( e2_data3 )
                                        {
                                            // Send current IDC alarm/waterflow state
                                            PERIPH_COMM(addresses[indexPeripheral], IDC_STATUS_CHANGE, e2_data3 + 10, sys_idc_nac_status & 0b00001001, 0);
                                            e2_data3 = 0;
                                            stateStatus = STATUS_RESPONSE;
                                        }
                                    }
                                }
                                else if ( peripheral_command == REQUEST_STAT )
                                {
                                    PERIPH_COMM(addresses[indexPeripheral], REQUEST_STATUS, 0, 0, 0);
                                    stateStatus = STATUS_RESPONSE;
                                }
//                                else
//                                {
//                                    indexPeripheral++;
//                                    stateStatus = STATUS_ADDRESS;
//                                }
                                if ( stateStatus != STATUS_RESPONSE )
                                {
                                    indexPeripheral++;
                                    stateStatus = STATUS_ADDRESS;
                                }
                                break;
// EOC CSM 5/19/2005
                        } /* end if switch */
                    } /* end of else */
                } /* end of if (xmit_packet.status == SERVICE_AVAILABLE) */
                break;

            case STATUS_RESPONSE:

#if (USART_TEST)
                latch4.latch4_output_bit.idc1_trbl = 0;
                latch4.latch4_output_bit.idc2_trbl = 0;
                latch4.latch4_output_bit.idc3_trbl = 0;
                latch5.latch5_output_bit.idc4_trbl = 1;
#endif

                /* check for good data */
                if (xmit_packet.status == RECEIVE_DATA_VALID)
                {
                    ACK_RCV_DATA_VALID;
// BOC CSM 5/19/2005
//                    indexPeripheral++;
//                    stateStatus = STATUS_ADDRESS;
                    // MATRIX RRM in common alarm mode will have set e2_data3. Needs to loop to set all 5 relays.
                    if ( e2_data3 )
                    {
                        stateStatus = STATUS_UPDATE;
                    }
                    else
                    {
                        indexPeripheral++;
                        stateStatus = STATUS_ADDRESS;
                    }
// EOC CSM 5/19/2005
                }
                /* check for no data */
                else if (xmit_packet.status == COMM_FAILURE)
                {
                    CLEAR_COMM_FAILURE;
                    command_not_sent = 1;
// BOC CSM 5/19/2005
//                    indexPeripheral++;
//                    stateStatus = STATUS_ADDRESS;
                    // MATRIX RRM in common alarm mode will have set e2_data3. Needs to loop to set all 5 relays.
                    if ( e2_data3 )
                    {
                        stateStatus = STATUS_UPDATE;
                    }
                    else
                    {
                        indexPeripheral++;
                        stateStatus = STATUS_ADDRESS;
                    }
// EOC CSM 5/19/2005
                }
                break;
        }

#if (USART_TEST)
        latch5.latch5_output_bit.idc5_trbl = !latch5.latch5_output_bit.idc5_trbl;
#endif
    }
#if (BATT_SHED_CODE)
    /* if ignoring peripherals, check if Aux power turned off because of low battery (shed loads threshold) */
    else if ( batt_ad_int_value <= BATTERY_SHED_LOADS )
    {
        /* all commands will return complete, since peripherals are turned off */
        return(FINISHED);
    }
#endif
    return(USART_BUSY);
// BOC CSM 5/18/2005
#else
    return FINISHED;
#endif
// EOC CSM 5/18/2005
}

/**************************************************************************
 ** FUNCTION: supervise_peripherals                                      **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function sends a polling or request status command to known     **
 ** peripherals to verify they are still communicating. If any known     **
 ** peripherals fail to respond, a trouble is set.                       **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void supervise_peripherals(void)
{
// BOC CSM 5/18/2005
#if (USART_CODE)
// EOC CSM 5/18/2005
    static unsigned char temp0, temp1, i = 0;
    static unsigned char RRM_disabled = 0;
    static unsigned char bad_peripheral_found_flag = 0;
// BOC CSM 8/4/2002
    static unsigned char badTry = 0;
// EOC CSM 8/4/2002
    static unsigned char previousStatusData[2] = { 0, 0 };
           unsigned char statusData;

    /* if no command current, process this command */
    if ( command == NO_COMMAND || command == POLL_FOR_PRESENCE )
    {
        command = POLL_FOR_PRESENCE;
    }
    /* need to hold off new commands until current command is processed for peripherals */
    else
    {
        return;
    }

    switch (statePoll)
    {
        case SUPERVISE_START :

#if (USART_TEST)
            latch6.latch6_output_bit.idc6_trbl = 1;
            latch6.latch6_output_bit.idc7_trbl = 0;
            latch7.latch7_output_bit.idc8_trbl = 0;
            latch7.latch7_output_bit.idc9_trbl = 0;
#endif

            temp0 = ext_eeprom_read(EEPROM_PERIPHERALS_RSI_RZI);
            temp1 = ext_eeprom_read(EEPROM_PERIPHERALS_RRM);
            statePoll = SUPERVISE_ADDRESS;
            break;

        case SUPERVISE_ADDRESS :

#if (USART_TEST)
            latch6.latch6_output_bit.idc6_trbl = 0;
            latch6.latch6_output_bit.idc7_trbl = 1;
            latch7.latch7_output_bit.idc8_trbl = 0;
            latch7.latch7_output_bit.idc9_trbl = 0;
#endif
            /* if bad peripheral found, annunciate it - non-latching */
            if ( bad_peripheral_found_flag )
            {
                //set peripheral trouble flag
                trouble.trouble_bit.periph_supv = 1;
            }

            //start new round
            if ( i > ADDRESSES_SIZE )
            {
                /* if no bad peripherals found, clear trouble */
                if ( !bad_peripheral_found_flag )
                {
                    trouble.trouble_bit.periph_supv = 0;
                }

                /* if peripherals reporting trouble, set trouble flag */
                if (( previousStatusData[0] & RSI_TROUBLE ) || ( previousStatusData[1] & RSI_TROUBLE ))
                {
                    trouble.trouble_bit.periph_supv = 1;
                }

                /* check if any disabled RRMs have been found */
                if ( RRM_disabled )
                {
                    /* set trouble if RRM disabled */
                    trouble2.trouble_bit2.disabledRRM = 1;
                }
                else
                {
                    /* clear RRM disabled trouble */
                    trouble2.trouble_bit2.disabledRRM = 0;
                }

                i = 0;
                statePoll = SUPERVISE_START;

                //always start with flag cleared...set below if bad periph found
                bad_peripheral_found_flag = 0;

                //always start with flag cleared...set below if RRM disabled found
                RRM_disabled = 0;

                /* so status_to_periph can jump in */
                ClearCurrentUsartCommand();
            }
            //see if any peripherals present in system (1st EEPROM loc)
            else if ( (i <= 7) && (temp0 & (1 << i)) )
            {
                statePoll = SUPERVISE_POLL;
            }
            //see if any peripherals present in system (2nd EEPROM loc)
            else if( ((i>7) && (i<=15)) && (temp1 & (1 << (i-8))) )
            {
                statePoll = SUPERVISE_POLL;
            }
            /* peripheral not present, skip */
            else
            {
                i++;
            }
            break;

        case SUPERVISE_POLL :

#if (USART_TEST)
            latch6.latch6_output_bit.idc6_trbl = 0;
            latch6.latch6_output_bit.idc7_trbl = 0;
            latch7.latch7_output_bit.idc8_trbl = 1;
            latch7.latch7_output_bit.idc9_trbl = 0;
#endif

            //wait in this state till state machine is free
            if (xmit_packet.status == SERVICE_AVAILABLE)
            {
                /* give usart_comm 3 seconds to complete communication to 1 peripheral */
                status_to_periph_exit_timer = SEC_3;

                /* if RSI or RRM, get peripheral's status */
                if ( i < ADDRESS_RZI || i >= ADDRESS_RRM)
                {
                    PERIPH_COMM(addresses[i], REQUEST_STATUS, 0, 0, 0);
                }
                /* if RZI, poll for presence */
                else
                {
                    PERIPH_COMM(addresses[i], POLL_FOR_PRESENCE, 0, 0, 0);
                }
                statePoll = SUPERVISE_RESPONSE;
            }
            break;

        case SUPERVISE_RESPONSE :

#if (USART_TEST)
            latch6.latch6_output_bit.idc6_trbl = 0;
            latch6.latch6_output_bit.idc7_trbl = 0;
            latch7.latch7_output_bit.idc8_trbl = 0;
            latch7.latch7_output_bit.idc9_trbl = 1;
#endif

            if (xmit_packet.status == COMM_FAILURE)
            {
                CLEAR_COMM_FAILURE;
// BOC CSM 8/4/2002
//                bad_peripheral_found_flag = 1;
// EOC CSM 8/4/2002

#if (BAD_COMM_TEST)
                latch6.latch6_output_byte &= 0b00000011;
                latch7.latch7_output_byte &= 0b00000011;

                if ( i & 0b00000001 )
                {
                    latch6.latch6_output_bit.idc6_trbl = 1;
                }
                if ( i & 0b00000010 )
                {
                    latch6.latch6_output_bit.idc7_trbl = 1;
                }
                if ( i & 0b00000100 )
                {
                    latch7.latch7_output_bit.idc8_trbl = 1;
                }
                if ( i & 0b00001000 )
                {
                    latch7.latch7_output_bit.idc9_trbl = 1;
                }

                numBad++;
                if ( numBad & 0b00000001 )
                {
                    latch6.latch6_output_bit.idc6_sup = 1;
                }
                if ( numBad & 0b00000010 )
                {
                    latch6.latch6_output_bit.idc7_sup = 1;
                }
                if ( numBad & 0b00000100 )
                {
                    latch7.latch7_output_bit.idc8_sup = 1;
                }
                if ( numBad & 0b00001000 )
                {
                    latch7.latch7_output_bit.idc9_sup = 1;
                }
#endif

// BOC CSM 8/4/2002
//                /* poll next peripheral */
//                i++;
                badTry++;
                if ( badTry > 3 )
                {
                    /* poll next peripheral */
                    i++;
                    badTry = 0;
                    bad_peripheral_found_flag = 1;
                }
// EOC CSM 8/4/2002

                /* so status_to_periph can jump in */
                command = NO_COMMAND;

                statePoll = SUPERVISE_ADDRESS;

            }
            else if ( xmit_packet.status == RECEIVE_DATA_VALID )
            {
                ACK_RCV_DATA_VALID;
// BOC CSM 8/4/2002
                badTry = 0;
// EOC CSM 8/4/2002
                statusData = receive_packet.data[1];

                /* if RRM, get peripheral's status and see if RRM is disabled */
                if ( i >= ADDRESS_RRM && ( statusData & RRM_DISABLED ) )
                {
// BOC CSM 8/4/2002
//                    bad_peripheral_found_flag = 1;
// EOC CSM 8/4/2002
                    RRM_disabled = 1;
                }
                /* see if RSI initiated a lamp test */
                else if ( i < ADDRESS_RZI )
                {
                    /* process RSI status based on subtype */
                    if ( periphRSISubType[i] == RSI_ORIGINAL )
                    {
                        if ( statusData & RSI_ORIGINAL_LAMP_TEST )
                        {
                            /* start sending out lamp test commands to other peripherals */
                            peripheral_state.peripheral_state_bit.lamp_test = 1;
                        }
                    }
                    else
                    {
//                        if (( statusData & RSI_LAMP_TEST ) && !( previousStatusData[i] & RSI_LAMP_TEST ))
//                        {
//                            /* command system into lamp test */
//                            button.button_bit.lamp_test = 1;
//                            button_valid_timer = 50;
//                        }
                        if ( statusData & RSI_LAMP_TEST )
                        {
                            /* command system into lamp test */
                            button.button_bit.lamp_test = 1;
                            button_valid_timer = 255;
                        }
                        if (( statusData & RSI_DRILL ) && !( previousStatusData[i] & RSI_DRILL ))
                        {
                            /* if not in alarm, command system into drill */
                            if (!(latch1.latch1_output_byte & 0b00010100))
                            {
                                system_state.system_state_bit.drill = 1;
                                button.button_bit.alarm_sil_drill = 1;
                                button_valid_timer = 50;
                            }
                        }
                        if (( statusData & RSI_RESET ) && !( previousStatusData[i] & RSI_RESET ))
                        {
                            /* command system into reset */
                            button.button_bit.reset = 1;
                            button_valid_timer = 50;
                        }
                        if (( statusData & RSI_SIGNAL_SILENCE ) && !( previousStatusData[i] & RSI_SIGNAL_SILENCE ))
                        {
                            /* if in alarm, command system into drill */
                            if ( latch1.latch1_output_byte & 0b00010100 )
                            {
                                button.button_bit.alarm_sil_drill = 1;
                                button_valid_timer = 50;
                            }
                        }
                        if (( statusData & RSI_PANEL_SILENCE  ) && !( previousStatusData[i] & RSI_PANEL_SILENCE  ))
                        {
                            /* command system into panel silence */
                            button.button_bit.trouble_silence = 1;
                            button_valid_timer = 50;
                        }
                        previousStatusData[i] = statusData;
                    }
                }

                /* poll next peripheral */
                i++;

                /* so status_to_periph can jump in */
                command = NO_COMMAND;

                statePoll = SUPERVISE_ADDRESS;
            }
            break;
    }

#if (USART_TEST)
    latch8.latch8_output_bit.idc10_trbl = !latch8.latch8_output_bit.idc10_trbl;
#endif
// BOC CSM 5/18/2005
#endif
// EOC CSM 5/18/2005
}

/**************************************************************************
 ** FUNCTION: usart_comm                                                 **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles all USART communication, including sending,    **
 ** receiving and error detection and annunciation.                      **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void usart_comm(void)
{
// BOC CSM 5/18/2005
#if (USART_CODE)
// EOC CSM 5/18/2005
    static unsigned char state = USART_START, rcv_state = 0;
    static unsigned char j;
    static unsigned char check_sum;
    static unsigned char xmit_attempt_ctr = 0;
           unsigned char xmit_size = MAX_XMIT_PACKET_DATA_INDEX;

    switch (state)
    {
        case USART_START :

            if (xmit_packet.status == SERVICE_AVAILABLE)
            {
                //if we're here, brand new transmission... clear error counters
                xmit_attempt_ctr = 0;
            }

            if (xmit_packet.command == XMIT_PACKET_EMPTY)
            {
                break;  //no data to send so quit
            }

            //---- initializations -----------------
            CREN = 0;   //clear overun flag by clearing & setting CREN
            CREN = 1;
            CREN = 0;   //disable reception till needed later
            j = RCREG;  //2 dummy reads to flush buffer & clear FERR bit
            j = RCREG;
            rcv_state = 0;
            //----------------------------------------

#if (!PROC_18F252)
            while (!TRMT);  // Errata on PIC2525/2620/4525/4620
#endif
            TX9D = 1;   //signify address byte being transmitted
            TXREG = xmit_packet.address;
            TXEN = 1;
            check_sum = xmit_packet.address;
            xmit_packet.status = IN_USE;
            state = USART_SEND_COMMAND;

            break;

        //send 2nd byte of packet data (command)
        case USART_SEND_COMMAND :

            if (TXIF)
            {
#if (!PROC_18F252)
                while (!TRMT);  // Errata on PIC2525/2620/4525/4620
#endif
                //signify this is a data byte (as opposed to address byte)
                TX9D = 0;
                TXREG = xmit_packet.command;
                check_sum += xmit_packet.command;
                state = USART_SEND_DATA;

                //initialize j for next state
                j = 0;

                //adjust transmit size if nec.
                if ( xmit_packet.command == IDC_TEXT_UPDATE )
                {
                    xmit_size = IDC_TEXT_LENGTH - 1;
                }
            }
            break;

        //send multiple bytes of packet data (data array)
        case USART_SEND_DATA :

            if (TXIF)
            {
                TXREG = xmit_packet.data[j];
                check_sum += xmit_packet.data[j];

                //do more array bytes need to be sent??
                if ( ++j > xmit_size )
                {
                    j = 0;
                    state = USART_SEND_CHECKSUM;
                }
            }
            break;

        //send check sum
        case USART_SEND_CHECKSUM :

            if (TXIF)
            {
                TXREG = check_sum;

                //enable usart reception
                CREN = 1;
                state = USART_RECEIVE;
                usart_exit_timer = 30;
            }
            break;

        //Wait in this state for reception of data from addressed peripheral
        case USART_RECEIVE :

            if (xmit_attempt_ctr >= 4)
            {
				xmit_attempt_ctr = 0;
				xmit_packet.status = COMM_FAILURE;

                //start a timer (1 sec). This timer sets "lifetime" of that data
                usart_exit_timer = 100;

                //signify data packet shouldn't be sent any more
                xmit_packet.command = XMIT_PACKET_EMPTY;
                state = USART_STATUS;
				break;
            }

            //don't wait forever
            if (!usart_exit_timer)
            {
                xmit_attempt_ctr++;
				xmit_packet.status = REATTEMPT_IN_PROGRESS;
                state = USART_START;
                break;
            }

            if (RCIF)
            {
                //ack_wait_cycle_ctr = 0;

                //check for overun error
                if (OERR)
                {
                    //clear overun by clearing & setting CREN
                    CREN = 0;   //clear overun flag by clearing & setting CREN
                    CREN = 1;
                    CREN = 0;   //disable reception till needed later
                    xmit_attempt_ctr++;
					xmit_packet.status = REATTEMPT_IN_PROGRESS;

                    //go back and re-transmit the same data packet again
                    state = USART_START;
                    break;
                }

                //check if framing error was detected
                if (FERR)
                {
                    //DUMMY READ to clear FERR bit
                    j = RCREG;  //2 dummy reads to flush buffer & clear FERR bit
                    j = RCREG;
                    xmit_attempt_ctr++;
					xmit_packet.status = REATTEMPT_IN_PROGRESS;

                    //go back and re-transmit the same data packet again
                    state = USART_START;
                    break;
                }

                switch(rcv_state)
                {
                    //receive peripheral's address
                    case 0 :
                        receive_packet.address = RCREG;
                        check_sum = receive_packet.address;
                        rcv_state = 1;
                        j = 0;
                        break;

                    //receive peripheral's data
                    case 1 :
                        receive_packet.data[j] = RCREG;
                        check_sum += receive_packet.data[j];
                        if ( ++j > MAX_RCV_PACKET_DATA_INDEX )
                        {
                            j = 0;
                            rcv_state = 2;
                        }
                        break;

                    //see if received data is good
                    case 2 :

                        if ( (receive_packet.address == xmit_packet.address) && (check_sum == RCREG) && (receive_packet.data[0] == RCV_ACK) )
                        {
                            xmit_packet.status = RECEIVE_DATA_VALID;

                            //start a timer (1 sec). This timer sets "lifetime" of that data
                            usart_exit_timer = 100;
                            state = USART_STATUS;

                            //data succesfully sent, set indication that this buffer loc is now free
                            xmit_packet.command = XMIT_PACKET_EMPTY;

                            xmit_attempt_ctr = 0;
							break;
                        }
                        else
                        {
                            xmit_attempt_ctr++;
							xmit_packet.status = REATTEMPT_IN_PROGRESS;

                            //go back and re-transmit the same data packet again
                            state = USART_START;
							break;
                        }
                }//END switch(rcv_state)
            }//END if(RCIF)
            break;

        //wait in this state for either data to be read or comm failure to be acknowledged
        //is data waiting to be read of comm failure waiting to be acknowledged
        case USART_STATUS:

            //disable USART
            TXEN = 0;

            //don't wait forever...did data "lifetime" timer expire?
            if ( xmit_packet.status == RECEIVE_DATA_READ || xmit_packet.status == COMM_FAILURE_ACKED || !usart_exit_timer )
			{
				xmit_packet.status = SERVICE_AVAILABLE;
				xmit_packet.command = XMIT_PACKET_EMPTY;
                state = USART_START;
			}

			break;

     }//END switch(state)
// BOC CSM 5/18/2005
#endif
// EOC CSM 5/18/2005
}//END void usart_comm(void)

/**************************************************************************
 ** FUNCTION: init_usart                                                 **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function initializes USART hardware.                            **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void init_usart(void)
{
    //----- SET UP USART FOR ASYNCH FULL DUPLEX OPERATION ---------------
    //set asynch baud rate (4MHz crystal) to 300baud should be SPBRG = 207 however...compensate for periphs and panel
    SPBRG = 200;
    //SPBRG = 230;    //++++++++++++++++ setting to compensate for RC clock error of Debugger ++++++++++++++++++
    BRGH = 0;
    SYNC = 0;
    SPEN = 1;
    TX9 = 1;
    RX9 = 0;
    //TXEN = 1;
    //CREN = 1; reception enabled in "periph_comm" function
}




