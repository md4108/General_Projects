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
|
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   main.h  $
|     $Date:   Aug 05 2005 08:12:28  $
|  $Modtime:   Jun 07 2005 21:25:44  $
| $Revision:   1.6  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\main.h-arc  $
|   $Author:   MCGILL  $
+------------------------------------------------------------------------------
*/

#ifndef H_MAIN
    #define H_MAIN


	//-------- Global Prototypes (extern statement in file: "externs.h") ---------------
        void interrupt tc_int(void);
	//----------------------------------------------------------------------------------


        struct SYSTEM_STATE_BIT_FIELD
        {
            unsigned char input_tripped:    1;
            unsigned char trouble:          1;
            unsigned char reset:            1;
            unsigned char program:          1;
            unsigned char drill:            1;
            unsigned char walk_test:        1;

            unsigned char nac_off_normal:   1;

            unsigned char silent_walk_test: 1;
        };
        union SYSTEM_STATE_BIT_BYTE
        {
            struct SYSTEM_STATE_BIT_FIELD system_state_bit;
            unsigned char system_state_byte;
        };

        struct PERIPHERAL_STATE_BIT_FIELD
        {
            unsigned char alarm:            1;
            unsigned char supervisory:      1;
            unsigned char monitor:          1;
            unsigned char trouble:          1;
            unsigned char ac_loss:          1;
            unsigned char ground_fault:     1;
            unsigned char trouble_silence:  1;
            unsigned char lamp_test:        1;
        };
        union PERIPHERAL_STATE_BIT_BYTE
        {
            struct PERIPHERAL_STATE_BIT_FIELD peripheral_state_bit;
            unsigned char peripheral_state_byte;
        };

        struct PERIPHERAL_STATE2_BIT_FIELD
        {
            unsigned char panel_silence:    1;
            unsigned char signal_silence:   1;
            unsigned char reset:            1;
            unsigned char spares:           5;
        };
        union PERIPHERAL_STATE2_BIT_BYTE
        {
            struct PERIPHERAL_STATE2_BIT_FIELD peripheral_state_bit;
            unsigned char peripheral_state_byte;
        };
#endif


