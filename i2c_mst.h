/*
+------------------------------------------------------------------------------
| Description:
|   This module provides a means to communicate with the DIALER, via the I2C buss
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
| 1/26/01   Larry Kolb       initial release
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   i2c_mst.h  $
|     $Date:   Aug 05 2005 08:12:28  $
|  $Modtime:   Jun 15 2005 12:47:48  $
| $Revision:   1.5  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\i2c_mst.h-arc  $
|   $Author:   MCGILL  $
+------------------------------------------------------------------------------
*/

#ifndef CRC_H
    #define CRC_H


    unsigned int calc_CRC( unsigned char *, unsigned char , unsigned char );
        #define TX_PACKET 0
        #define RX_PACKET 1

#endif

#ifndef H_I2CMST
    #define H_I2CMST

	//-------- Global Prototypes (extern statement in file: "externs.h") ---------------
		void ext_eeprom_write(unsigned char, unsigned char);
		unsigned char ext_eeprom_read(unsigned char address);

        bit dialer_com(unsigned char, const unsigned char *);
		void load_poll_for_presence_data(void);
	//-----------------------------------------------------------------------------------


	struct COMMANDS_FROM_DACT_BIT_FIELD
	{
		unsigned char dact_has_data_for_panel:	1;
        unsigned char dact_queue_empty:         1;
		unsigned char end_dact_program_mode:	1;
		unsigned char dact_trouble:				1;
		unsigned char end_dact_trouble:			1;
		unsigned char accept_download:			1;
		unsigned char produce_upload:			1;
        unsigned char spare:                    1;
	};
	union COMMANDS_FROM_DACT_BIT_BYTE
	{
		struct COMMANDS_FROM_DACT_BIT_FIELD commands_from_dact_bit;
		unsigned char commands_from_dact_byte;
	};

#endif
