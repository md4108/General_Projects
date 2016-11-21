/***************************************************************************
** PROJECT:     Conventional Panel
**
** FILENAME:    io.h
**
** DESCRIPTION: Include file for inputs.c
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
** 10/18/05  C. McGill        In LATCH5_OUTPUT_BIT_FIELD corrected classA_1st
**                            and classA_2nd definitions. PTR 1472-1473.
** 06/15/01  Larry Kolb       Creation.
**
***************************************************************************/
/**************************************************************************/
/*** BEGIN: !!!!Version Control Information Section. DO NOT CHANGE!!!! ****/
/**************************************************************************
**
** $Workfile:   io.h  $
**
** $Author:   mcgill  $
**
** $Date:   Oct 18 2005 16:07:32  $
**
** $Modtime:   Oct 18 2005 14:28:12  $
**
** $Revision:   1.6  $
**
** $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\io.h-arc  $
**
***************************************************************************/
/*** END: !!!!Version Control Information Section. DO NOT CHANGE!!!! ******/
/**************************************************************************/

#ifndef H_INPUTS
    #define H_INPUTS

	//-------- Global Prototypes (extern statement in file: "externs.h") ---------------
    void button_handler(void);
    unsigned int ad_conversion(unsigned char);
    void scan_analog_inputs(void);
    void debounce_idc_levels(void);
    void debounce_nac_levels(void);
	//----------------------------------------------------------------------------------

#if ( PANEL_10_ZONE )

    struct BUTTON_BIT_FIELD
    {
        /* first unsigned int */
        unsigned int    remote_disconnect:  1;
        unsigned int    lamp_test:          1;
        unsigned int    walk_test:          1;
        unsigned int    trouble_silence:    1;
        unsigned int    alarm_sil_drill:    1;
        unsigned int    reset:              1;

        unsigned int    program:            1;

        unsigned int    idc_1_disable:      1;
        unsigned int    idc_2_disable:      1;
        unsigned int    idc_3_disable:      1;
        unsigned int    idc_4_disable:      1;
        unsigned int    idc_5_disable:      1;
        unsigned int    idc_6_disable:      1;
        unsigned int    idc_7_disable:      1;
        unsigned int    idc_8_disable:      1;
        unsigned int    idc_9_disable:      1;
    };
    union BUTTON_BIT_BYTE
    {
        struct BUTTON_BIT_FIELD button_bit;
        unsigned int button_int;
    };

    struct BUTTON_BIT_FIELD2
    {
        /* second unsigned int */
        unsigned int    idc_10_disable:     1;
        unsigned int    nac_1_disable:      1;
        unsigned int    nac_2_disable:      1;
        unsigned int    nac_3_disable:      1;
        unsigned int    nac_4_disable:      1;
        unsigned int    spare1:             8;
        unsigned int    spare2:             3;
    };
    union BUTTON_BIT_BYTE2
    {
        struct BUTTON_BIT_FIELD2 button_bit2;
        unsigned int button_int2;
    };

#elif ( PANEL_3_5_ZONE )
    struct BUTTON_BIT_FIELD
    {
        unsigned int    remote_disconnect:  1;
        unsigned int    lamp_test:          1;
        unsigned int    walk_test:          1;
        unsigned int    trouble_silence:    1;
        unsigned int    alarm_sil_drill:    1;
        unsigned int    reset:              1;

        unsigned int    program:            1;

        unsigned int    idc_1_disable:      1;
        unsigned int    idc_2_disable:      1;
        unsigned int    idc_3_disable:      1;
        unsigned int    idc_4_disable:      1;
        unsigned int    idc_5_disable:      1;
        unsigned int    nac_1_disable:      1;
        unsigned int    nac_2_disable:      1;
        unsigned int    spares:             2;
    };
    union BUTTON_BIT_BYTE
    {
        struct BUTTON_BIT_FIELD button_bit;
        unsigned int button_int;
    };
#endif

    struct IDC_LEVEL_BIT_FIELD
    {
        unsigned char open:         1;  //OPEN
        unsigned char normal:       1;  //NORMAL
        unsigned char supv:         1;  //SUPERVISORY (when programmed as waterflow/supervisory "combo")
        unsigned char verified:     1;  //VERIFIED ALARM
        unsigned char non_ver:      1;  //NON-VERIFIED, WATERFLOW(programmed combo or not),SUPERVISORY (when not
                                            //    programmed combo), MONITOR
                                        //***NOTE: button_press not implemented here (it's done separately)
        unsigned char spares:       3;
    };
    union IDC_LEVEL_BIT_BYTE
    {
        struct IDC_LEVEL_BIT_FIELD idc_level_bit;
        unsigned char idc_level_byte;
    };



    struct NAC_LEVEL_BIT_FIELD
    {
        unsigned char open:         1;
        unsigned char normal:       1;
        unsigned char shorted:      1;
		unsigned char over_current:	1;

        unsigned char spares:       4;
    };
    union NAC_LEVEL_BIT_BYTE
    {
        struct NAC_LEVEL_BIT_FIELD nac_level_bit;
        unsigned char nac_level_byte;
    };
#endif

#ifndef H_OUTPUTS
    #define H_OUTPUTS

	//-------- Global Prototypes (extern statement in file: "externs.h") ---------------
	void output_refresh(void);
	//----------------------------------------------------------------------------------


	struct LATCH1_OUTPUT_BIT_FIELD
	{
	    unsigned char   com_disable:            1;
        unsigned char   ac_power:               1;
	    unsigned char   com_sup:                1;
	    unsigned char   com_trbl:               1;
	    unsigned char   com_alarm:              1;
	    unsigned char   logic_battery_back_up:  1;
	    unsigned char   nac1_trbl:              1;
	    unsigned char   nac2_trbl:              1;
	};
	union LATCH1_BIT_BYTE
	{
	    struct LATCH1_OUTPUT_BIT_FIELD latch1_output_bit;
	    unsigned char latch1_output_byte;
	};
#if (PIC18)
    union LATCH1_PULSE_BIT_BYTE
#else
	bank1 union LATCH1_PULSE_BIT_BYTE
#endif
	{
	    struct LATCH1_OUTPUT_BIT_FIELD latch1_pulse_bit;
	    unsigned char latch1_pulse_byte;
	};



	struct LATCH2_OUTPUT_BIT_FIELD
	{
	    unsigned char   silenced:               1;
	    unsigned char   walk_test:              1;
	    unsigned char   gnd_fault:              1;
	    unsigned char   batt_trbl:              1;
	    unsigned char   ann_trbl:               1;
	    unsigned char   main_battery:           1;
	    unsigned char   charger:                1;
	    unsigned char   buzzer:                 1;
	};
	union LATCH2_BIT_BYTE
	{
	    struct LATCH2_OUTPUT_BIT_FIELD latch2_output_bit;
	    unsigned char latch2_output_byte;
	};
#if (PIC18)
    union LATCH2_PULSE_BIT_BYTE
#else
	bank1 union LATCH2_PULSE_BIT_BYTE
#endif
	{
	    struct LATCH2_OUTPUT_BIT_FIELD latch2_pulse_bit;
	    unsigned char latch2_pulse_byte;
	};


#if (PANEL_10_ZONE)
	struct LATCH3_OUTPUT_BIT_FIELD
	{
        unsigned char   alarm_relay:            1;
	    unsigned char   nac1:                   1;
	    unsigned char   nac2:                   1;
        unsigned char   sup_relay:              1;
	    unsigned char   idc1_alarm:             1;
	    unsigned char   idc2_alarm:             1;
	    unsigned char   idc3_alarm:             1;
	    unsigned char   aux_power:              1;
	};
#elif (PANEL_3_5_ZONE)
	struct LATCH3_OUTPUT_BIT_FIELD
	{
	    unsigned char   sup_relay:              1;
	    unsigned char   nac1:                   1;
	    unsigned char   nac2:                   1;
	    unsigned char   alarm_relay:            1;
	    unsigned char   idc1_alarm:             1;
	    unsigned char   idc2_alarm:             1;
	    unsigned char   idc3_alarm:             1;
	    unsigned char   aux_power:              1;
	};
#endif
	union LATCH3_BIT_BYTE
	{
	    struct LATCH3_OUTPUT_BIT_FIELD latch3_output_bit;
	    unsigned char latch3_output_byte;
	};


	struct LATCH4_OUTPUT_BIT_FIELD
	{
	    unsigned char   trouble_relay:          1;
	    unsigned char   idc1_sup:               1;
	    unsigned char   idc2_sup:               1;
	    unsigned char   idc3_sup:               1;
	    unsigned char   idc1_trbl:              1;
	    unsigned char   idc2_trbl:              1;
	    unsigned char   idc3_trbl:              1;
	    unsigned char   smoke_reset:            1;
	};
	union LATCH4_BIT_BYTE
	{
	    struct LATCH4_OUTPUT_BIT_FIELD latch4_output_bit;
	    unsigned char latch4_output_byte;
	};
#if (PIC18)
    union LATCH4_PULSE_BIT_BYTE
#else
	bank1 union LATCH4_PULSE_BIT_BYTE
#endif
	{
	    struct LATCH4_OUTPUT_BIT_FIELD latch4_pulse_bit;
	    unsigned char latch4_pulse_byte;
	};

	struct LATCH5_OUTPUT_BIT_FIELD
	{
#if (PANEL_10_ZONE)
        unsigned char   classA_2nd:             1;
        unsigned char   classA_1st:             1;
#elif (PANEL_3_5_ZONE)
        unsigned char   classA_1st:             1;
        unsigned char   classA_2nd:             1;
#endif
	    unsigned char   idc4_trbl:              1;
	    unsigned char   idc5_trbl:              1;
	    unsigned char   idc4_sup:               1;
	    unsigned char   idc5_sup:               1;
	    unsigned char   idc4_alarm:             1;
	    unsigned char   idc5_alarm:             1;
	};
	union LATCH5_BIT_BYTE
	{
	    struct LATCH5_OUTPUT_BIT_FIELD latch5_output_bit;
	    unsigned char latch5_output_byte;
	};
#if (PIC18)
    union LATCH5_PULSE_BIT_BYTE
#else
	bank1 union LATCH5_PULSE_BIT_BYTE
#endif
	{
	    struct LATCH5_OUTPUT_BIT_FIELD latch5_pulse_bit;
	    unsigned char latch5_pulse_byte;
	};

	struct LATCH6_OUTPUT_BIT_FIELD
	{
	    unsigned char   nac1_sync:              1;
	    unsigned char   nac2_sync:              1;
#if (PANEL_10_ZONE)
        unsigned char   idc6_trbl:              1;
        unsigned char   idc7_trbl:              1;
        unsigned char   idc6_sup:               1;
        unsigned char   idc7_sup:               1;
        unsigned char   idc6_alarm:             1;
        unsigned char   idc7_alarm:             1;
#elif (PANEL_3_5_ZONE)
        unsigned char   nac_class_a:            1;
        unsigned char   rem_disc:               1;
        unsigned char   com_waterflow:          1;
        unsigned char   aux_power_constant:     1;
        unsigned char   com_service_detector:   1;
        unsigned char   unused:                 1;
#endif
	};
	union LATCH6_BIT_BYTE
	{
	    struct LATCH6_OUTPUT_BIT_FIELD latch6_output_bit;
	    unsigned char latch6_output_byte;
	};
#if (PIC18)
    union LATCH6_PULSE_BIT_BYTE
#else
    bank1 union LATCH6_PULSE_BIT_BYTE
#endif
	{
        struct LATCH6_OUTPUT_BIT_FIELD latch6_pulse_bit;
        unsigned char latch6_pulse_byte;
	};

#if (PANEL_10_ZONE)
    struct LATCH7_OUTPUT_BIT_FIELD
	{
        unsigned char   nac4:                   1;
        unsigned char   nac3:                   1;
        unsigned char   idc8_trbl:              1;
        unsigned char   idc9_trbl:              1;
        unsigned char   idc8_sup:               1;
        unsigned char   idc9_sup:               1;
        unsigned char   idc8_alarm:             1;
        unsigned char   idc9_alarm:             1;
	};
    union LATCH7_BIT_BYTE
	{
        struct LATCH7_OUTPUT_BIT_FIELD latch7_output_bit;
        unsigned char latch7_output_byte;
	};
#if (PIC18)
    union LATCH7_PULSE_BIT_BYTE
#else
    bank1 union LATCH7_PULSE_BIT_BYTE
#endif
	{
        struct LATCH7_OUTPUT_BIT_FIELD latch7_pulse_bit;
        unsigned char latch7_pulse_byte;
	};

    struct LATCH8_OUTPUT_BIT_FIELD
	{
        unsigned char   classA_3rd:             1;
        unsigned char   classA_4th:             1;
        unsigned char   classA_5th:             1;
        unsigned char   nac4_trbl:              1;
        unsigned char   nac3_trbl:              1;
        unsigned char   idc10_trbl:             1;
        unsigned char   idc10_sup:              1;
        unsigned char   idc10_alarm:            1;
	};
    union LATCH8_BIT_BYTE
	{
        struct LATCH8_OUTPUT_BIT_FIELD latch8_output_bit;
        unsigned char latch8_output_byte;
	};
#if (PIC18)
    union LATCH8_PULSE_BIT_BYTE
#else
    bank1 union LATCH8_PULSE_BIT_BYTE
#endif
	{
        struct LATCH8_OUTPUT_BIT_FIELD latch8_pulse_bit;
        unsigned char latch8_pulse_byte;
	};

    struct LATCH9_OUTPUT_BIT_FIELD                         // This is NOT a hardware latch, only for software purposes.....
	{
        unsigned char   nac_classA_1st:         1;
        unsigned char   nac_classA_2nd:         1;
        unsigned char   com_waterflow:          1;
        unsigned char   com_service_detector:   1;
        unsigned char   aux_power_constant:     1;
        unsigned char   rem_disc:               1;
        unsigned char   unused:                 2;
	};
    union LATCH9_BIT_BYTE
	{
        struct LATCH9_OUTPUT_BIT_FIELD latch9_output_bit;
        unsigned char latch9_output_byte;
	};
#if (PIC18)
    union LATCH9_PULSE_BIT_BYTE
#else
    bank1 union LATCH9_PULSE_BIT_BYTE
#endif
	{
        struct LATCH9_OUTPUT_BIT_FIELD latch9_pulse_bit;
        unsigned char latch9_pulse_byte;
	};
#endif

#endif

extern void ProcessSpecials(void);
