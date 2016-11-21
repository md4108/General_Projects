/*
+------------------------------------------------------------------------------
| Description:
|
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
| 8/15/2005  C. McGill       Corrected to light remote disconnect on lamp test.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   normal.c  $
|     $Date:   Aug 15 2005 10:03:50  $
|  $Modtime:   Aug 15 2005 10:02:38  $
| $Revision:   1.11  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\normal.c-arc  $
|   $Author:   mcgill  $
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
#include "normal.h"
#include "io.h"

static unsigned char latchState[MAX_LATCH];
static bit latchesSaved = 0;

/**************************************************************************
 ** FUNCTION: normal_handler                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function handles saving of latch information during lamp test   **
 ** and restoring latch info when lamp test complete.                    **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void normal_handler(void)
{
    /* if lamp test occurring or in programming mode */
    if ( button.button_bit.lamp_test )
    {
        /* lamp test just started, save current output configuration */
        if ( !latchesSaved )
        {
            latchesSaved = 1;
            latchState[LATCH_1] = latch1.latch1_output_byte;
            latchState[LATCH_2] = latch2.latch2_output_byte;
            latchState[LATCH_3] = latch3.latch3_output_byte;
            latchState[LATCH_4] = latch4.latch4_output_byte;
            latchState[LATCH_5] = latch5.latch5_output_byte;
            latchState[LATCH_6] = latch6.latch6_output_byte;
#if (PANEL_10_ZONE)
            latchState[LATCH_7] = latch7.latch7_output_byte;
            latchState[LATCH_8] = latch8.latch8_output_byte;
            latchState[LATCH_9] = latch9.latch9_output_byte;
#endif
        }

        if ( latch1.latch1_output_byte != 0b11111111 )
        {
            //while buttons remain pressed
            latch1.latch1_output_byte = 0b11111111;
            latch2.latch2_output_byte |= 0b00011111;
            latch3.latch3_output_byte |= 0b11110000;
            latch4.latch4_output_byte |= 0b01111110;
            latch5.latch5_output_byte |= 0b11111100;
#if (PANEL_10_ZONE)
            latch6.latch6_output_byte |= 0b11111100;
            latch7.latch7_output_byte |= 0b11111100;
            latch8.latch8_output_byte |= 0b11111000;
            latch9.latch9_output_byte |= 0b00111100;
#else
            latch6.latch6_output_byte |= 0b01011000;
#endif
        }
        peripheral_state.peripheral_state_bit.lamp_test = 1;
    }
    /* lamp test just stopped, restore output configuration */
    else if ( latchesSaved )
    {
        /* restart any USARt communication */
        ClearCurrentUsartCommand();

        /* clear flag */
        latchesSaved = 0;

        /* restore current output configuration */
        latch1.latch1_output_byte = latchState[LATCH_1];
        latch2.latch2_output_byte = latchState[LATCH_2];
        latch3.latch3_output_byte = latchState[LATCH_3];
        latch4.latch4_output_byte = latchState[LATCH_4];
        latch5.latch5_output_byte = latchState[LATCH_5];
        latch6.latch6_output_byte = latchState[LATCH_6];
#if (PANEL_10_ZONE)
        latch7.latch7_output_byte = latchState[LATCH_7];
        latch8.latch8_output_byte = latchState[LATCH_8];
        latch9.latch9_output_byte = latchState[LATCH_9];
#endif
    }
}

