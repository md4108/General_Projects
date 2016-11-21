/*
+------------------------------------------------------------------------------
| Description: tripped.h
| Header file for tripped.c
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
| 8/11/2005  C. McGill       Added cleanMeSequence bit to idcStatus3 to track start
|                            of clean me initiation sequence.
| 08/14/02  C. McGill        Added bit field was_disabled to NAC_STATUS_BIT_FIELD
|                             so that nac_active_handler() can track if nac
|                             disabled in startup state and if so, when
|                             re-enabled check if NACs are silenced and if so,
|                             go straight to silenced state. PTR #849.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   tripped.h  $
|     $Date:   Aug 11 2005 19:54:30  $
|  $Modtime:   Aug 11 2005 19:54:22  $
| $Revision:   1.14  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\tripped.h-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/

#ifndef H_TRIPPED
    #define H_TRIPPED

	//-------- Global Prototypes (extern statement in file: "externs.h") ---------------
    void input_tripped_handler(void);
    void nac_input_handler(void);

    void process_idc_level_status(void);
    void process_nac_level_status(void);
    void load_thresh_levels(void);
	//----------------------------------------------------------------------------------

    struct IDC_STATUS_BIT_FIELD
    {
        unsigned char alarm:                1;
        unsigned char supervisory:          1;
        unsigned char monitor:              1;
        unsigned char waterflow:            1;
        unsigned char trouble:              1;
        unsigned char disabled:             1;
        unsigned char verifying:            1;
// BOC CSM 5/30/2005
        unsigned char cross_zoned_pending:  1;
//        unsigned char state_machine_active: 1;
// EOC CSM 5/30/2005
    };
    union IDC_STATUS_BIT_BYTE
    {
        struct IDC_STATUS_BIT_FIELD idc_status_bit;
        unsigned char idc_status_byte;
    };

    struct IDC_STATUS_BIT_FIELD2
    {
        unsigned char walk_test:            1;
        unsigned char walk_test_tripped:    1;
        unsigned char walk_test_nac:        1;
        unsigned char walk_test_annun:      1;
        unsigned char walk_test_reset:      1;
        unsigned char walk_test_trouble:    1;
        unsigned char sup_combo:            1;
        unsigned char walk_test_trbl_active:1;
    };
    union IDC_STATUS_BIT_BYTE2
    {
        struct IDC_STATUS_BIT_FIELD2 idc_status_bit2;
        unsigned char idc_status_byte2;
    };

    struct IDC_STATUS_BIT_FIELD3
    {
        unsigned char cleanMeValidate:      1;
        unsigned char cleanMeActive:        1;
        unsigned char cleanMeSequence:      1;
        unsigned char spares:               5;
    };
    union IDC_STATUS_BIT_BYTE3
    {
        struct IDC_STATUS_BIT_FIELD3 idc_status_bit3;
        unsigned char idc_status_byte3;
    };

    struct NAC_STATUS_BIT_FIELD
    {
        unsigned char trouble:      1;
        unsigned char shorted:      1;
		unsigned char over_current:	1;
        unsigned char disabled:     1;
        unsigned char battery_low:  1;
        unsigned char OC_wait:      1;
// BOC CSM 8/14/2002
//        unsigned char spares:       1;
        unsigned char was_disabled: 1;
// EOC CSM 8/14/2002
        unsigned char state_machine_active: 1;
    };
    union NAC_STATUS_BIT_BYTE
    {
        struct NAC_STATUS_BIT_FIELD nac_status_bit;
        unsigned char nac_status_byte;
    };


    struct NAC_TYPE_BIT_FIELD
    {
        unsigned char continuous:   1;
        unsigned char temporal:     1;
        unsigned char genesis:      1;
        unsigned char spare:        1;
        unsigned char class_a:      1;
        unsigned char silenceable:  1;
        unsigned char spares:       2;
    };
    union NAC_TYPE_BIT_BYTE
    {
        struct NAC_TYPE_BIT_FIELD nac_type_bit;
        unsigned char nac_type_byte;
    };


#endif

