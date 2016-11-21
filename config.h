/***************************************************************************
** PROJECT:     Conventional Panel
**
** FILENAME:    config.h
**
** DESCRIPTION: Compiler configuration file.
**
** AUTHOR:      James Monczynski
**
** DATE:        06/15/2001
**
***************************************************************************
**
** COPYRIGHT: This software is the proprietary property of:
**
**             Edwards Systems Technology
**             6411 Parkland Drive
**             Sarasota, FL 34243
**             (941) 739-4200
**
**             Copyright (c) 2001.
**             ALL RIGHTS RESERVED
**
**             The use, duplication, or disclosure of this material in any
**             form without permission from Edwards Systems Technology is
**             strictly forbidden.
**
***************************************************************************/
/**************************************************************************
**  Maintenance History
**
**    Provide the date, name of the person performing the maintenance, and
**    a short paragraph about the change.
**
**  Date      Name             Description
** ========  ===============  ===========================================
** 08/9/05   C. McGill        Added defines for micro, FSPlus...
** 06/15/01  James Monczynski Creation.
**
***************************************************************************/
/**************************************************************************/
/*** BEGIN: !!!!Version Control Information Section. DO NOT CHANGE!!!! ****/
/**************************************************************************
**
** $Workfile:   config.h  $
**
** $Author:   mcgill  $
**
** $Date:   Aug 09 2005 13:19:08  $
**
** $Modtime:   Aug 09 2005 13:19:00  $
**
** $Revision:   1.6  $
**
** $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\config.h-arc  $
**
***************************************************************************/
/*** END: !!!!Version Control Information Section. DO NOT CHANGE!!!! ******/
/**************************************************************************/

/**************************************************************************/
/* Only 1 define should be set to one, the other should be 0              */
/**************************************************************************/

/* if compiling for the PIC18F252, set PROC_18F252 to 1 */
#define     PROC_18F252     0

/* if compiling for the FireShield Plus, set FSPLUS to 1 */
#define     FSPLUS          1

/* if compiling a 3/5 zone panel, set PANEL_3_5_ZONE to 1 */
#define     PANEL_3_5_ZONE  1

/* if compiling a 10 zone panel, set PANEL_10_ZONE to 1 */
#define     PANEL_10_ZONE   0

#if (PANEL_10_ZONE)
#define     MAX_IDC         10
#define     MAX_NAC         4
#elif (PANEL_3_5_ZONE)
#define     MAX_IDC         5
#define     MAX_NAC         2
#endif

/**************************************************************************/
/* PIC18  1 = PIC18 controller, 0 = PIC16 controller                      */
/**************************************************************************/
#define     PIC18           1
/**************************************************************************/
/* DACT_CODE  1 = include DACT access code, 0 = no code added             */
/**************************************************************************/
#define     DACT_CODE       1
/**************************************************************************/
/* EEPROM_BURN  1 = include EEPROM default burning code, 0 = no code added*/
/**************************************************************************/
#define     EEPROM_BURN     0
/**************************************************************************/
/* USART_TEST  1 = include USART status LED code, 0 = no code added       */
/**************************************************************************/
#define     USART_TEST      0
/**************************************************************************/
/* BUZZER_TEST  1 = Special buzzer test for UL, 0 = normal buzzer ops     */
/**************************************************************************/
#define     BUZZER_TEST     0
/***********************************************************************************/
/* BATT_SHED_CODE  1 = include ability to turn off Aux power with low battery code */
/***********************************************************************************/
#define     BATT_SHED_CODE  0
/**************************************************************************/
/* CODER_CODE  1 = include coder code, 0 = no code added                  */
/**************************************************************************/
#define     CODER_CODE      1
/**************************************************************************/
/* USART_CODE  1 = include USART code, 0 = no code added                  */
/**************************************************************************/
#define     USART_CODE      1
