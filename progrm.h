/*
+------------------------------------------------------------------------------
| Description: progrm.h
| Header file for progrm.c
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
| $Workfile:   progrm.h  $
|     $Date:   Aug 05 2005 08:12:40  $
|  $Modtime:   Mar 26 2002 14:55:30  $
| $Revision:   1.3  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\progrm.h-arc  $
|   $Author:   MCGILL  $
+------------------------------------------------------------------------------
*/

#ifndef H_PROGRM
    #define H_PROGRM

    bit program(void);
	bit poll_for_peripherals(unsigned char);


    struct NACS_ACTIVATED_BIT_FIELD
    {
        unsigned char nac1:     1;
        unsigned char nac2:     1;
#if (PANEL_10_ZONE)
        unsigned char nac3:     1;
        unsigned char nac4:     1;
        unsigned char spares:   4;
#elif (PANEL_3_5_ZONE)
        unsigned char spares:   6;
#endif
    };
    union NACS_ACTIVATED_BIT_BYTE
    {
        struct NACS_ACTIVATED_BIT_FIELD nacs_activated_bit;
        unsigned char nacs_activated_byte;
    };




#endif

