/*
+------------------------------------------------------------------------------
| Description: trouble.h
| Header file for trouble.c
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
| $Workfile:   trouble.h  $
|     $Date:   Aug 05 2005 08:12:40  $
|  $Modtime:   May 01 2002 11:03:24  $
| $Revision:   1.15  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\trouble.h-arc  $
|   $Author:   MCGILL  $
+------------------------------------------------------------------------------
*/

#ifndef H_TROUBLE
    #define H_TROUBLE


    unsigned char trouble_handler(void);


	struct TROUBLE_BIT_FIELD
	{
        unsigned char   periph_supv:        1;  //failure in "supervise_peripherals()"
		unsigned char	periph_cmnd:		1;	//failure in "status_to_periph()"
		unsigned char	battery:			1;
		unsigned char	batt_placement:		1;
		unsigned char	ac_loss:			1;
		unsigned char	charger:			1;
        unsigned char   gnd_fault:          1;
#if (PANEL_10_ZONE)
        unsigned char   transformer2:       1;
#elif (PANEL_3_5_ZONE)
        unsigned char   spare :             1;
#endif
    };
	union TROUBLE_BIT_BYTE
	{
		struct TROUBLE_BIT_FIELD trouble_bit;
		unsigned char trouble_byte;
	};

    struct TROUBLE_BIT_FIELD2
	{
        unsigned char   auxOC:              	1;
        unsigned char   CatSysFailure :     	1;	//caused by ext E2 failure, RAM failure, or WD timeout
        unsigned char   disabledRRM :       	1;
        unsigned char   walk_test :         	1;
        unsigned char   dact_reported_troub :	1;	//as reported FROM the DACT
        unsigned char   remoteDisconnect :  	1;
        unsigned char   idcTrouble :        	1;
        unsigned char   nacTrouble :        	1;
    };
    union TROUBLE_BIT_BYTE2
	{
        struct TROUBLE_BIT_FIELD2 trouble_bit2;
        unsigned char trouble_byte2;
	};


	struct DACT_TROUBLE_BIT_FIELD
	{
		unsigned char	dactHasI2CBusyStuckLow:	1;	//latching
		unsigned char	dactCommFail:			1;	//latching
		unsigned char	spares:					6;
	};
	union DACT_TROUBLE_BIT_BYTE
	{
		struct DACT_TROUBLE_BIT_FIELD dact_trouble_bit;
		unsigned char dact_trouble_byte;
	};

#endif


