/*
+------------------------------------------------------------------------------
| Description:
|   This module provides a means to communicate with the DIALER, via the I2C buss
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
| 11/1/2006  C. McGill       New 18F2525 for DACT has been released. Restoring
|                            address 1.
| 5/11/2006  C. McGill       Added compile switch for DACT address. New microchip
|                            part does not support old DACT address of 1. Waiting
|                            on fix to 18F2525. Testing with 18F2620 (eng sample with
|                            address 1 restored).
| 8/13/2005  C. McGill       In i2c_comm(), corrected time delay between errors.
|                            Added slight delay between i2c read and write to improve
|                            communication performance between panel and dialer.
| 06/12/02  C. McGill        In UpdateEepromRam(), removed internal watchdog function
|                            In wait_SSPIF(), removed unnecessary wait loop.
|                            In i2c_comm(), corrected I2CBSY wait loop.
|                            In ext_eeprom_read() and ext_eeprom_write(),
|                             corrected I2CBSY wait loop. Corrected error handling.
|                             Added error restoration.
|                            In dialer_comm(), added comm trouble restoration.
| 06/29/02  C. McGill        In dialer_comm(), SetEEPROMBad(), SetEEPROMGood()
|                             modified to reassert SCL and SDA as inputs on
|                             error to prevent locking up the bus. PTR #683,
|                             PTR #671
| 07/14/02  C. McGill        Replaced watchdog process with new WatchDog() function.
|                             Modified comm routines to initialize MSSP disabled.
| 07/30/02  C. McGill        In wait_SSPIF(), SetEEPROMBad() and dialer_comm(),
|                             modified to send end command on failure.
| 07/31/02  C. McGill        In ext_eeprom_read() and ext_eeprom_write(), modified
|                             to reset error counter on successful operation
|                             to allow exit from process loop. A single error
|                             with successful reads after would result in
|                             a fatal loop trap.
| 08/07/02  C. McGill        Added function i2c_buss_reset() for buss collision
|                             recovery. In i2c_comm(), ext_eeprom_read(),
|                             ext_eeprom_write() and wait_SSPIF() added calls
|                             to i2c_buss_reset on error condition. Removed
|                             07/30/02 change. New change renders it unnecessary.
| 08/19/02  C. McGIll        In dialer_comm(), modified to provide wait time between
|                             error detection.
| 08/25/02  C. McGill        In i2c_comm(), modified to validate incomming packet
|                             size. If a comm hit occurred on this byte, it is
|                             possible to read this count in error and therefore
|                             read in more data than can be held in the input
|                             buffer, thus overwritting other data. In simulation,
|                             latch data ( amongst others ) can be overwritten
|                             resulting in the "Christmas Tree" effect.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   i2c_mst.c  $
|     $Date:   Nov 01 2006 18:06:46  $
|  $Modtime:   Nov 01 2006 17:59:18  $
| $Revision:   1.60  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\i2c_mst.c-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/

#include "config.h"
#if (PIC18)
#include <pic18.h>
#else
#include <pic.h>
#endif
#include <string.h>
#include "defines.h"
#include "main.h"
#include "externs.h"
#include "i2c_mst.h"

extern void DogWatch(void);


//======= Local (to module) prototypes =================================
//* If DACT programmed at address 1, set DACT_ADDR_1 to 1
#define DACT_ADDR_1     1
//* If DACT programmed at address 8, set DACT_ADDR_8 to 1
#define DACT_ADDR_8     0

static bit i2c_comm(unsigned char destination, unsigned char read_write, unsigned char num_xmit_bytes);
    #define I2C_READ    1
    #define I2C_WRITE   0
    #define DUMMY       0

    #define I2C_ERROR   1
    #define I2C_OK      0

    #define DATA_GOOD_ACK   0x01    //response from slave

	#define DACT		1
	#define EEPROM		2


static bit wait_SSPIF(void);
    #define SSPIF_STUCK         1
    #define I2C_COLLISION       1
    #define SSPIF_RESPONDED     0

static unsigned int crc_update(unsigned int , unsigned char );
// BOC CSM 8/8/2002
static void         i2c_buss_reset(void);
// EOC CSM 8/8/2002
//=======================================================================

//========== Local (to module) variables =======================
//==============================================================


//=== Global Variable Declarations (extern statement in file: "externs.h") ========
#if (PIC18)
unsigned char i2c_data_xmit_array[I2C_XMIT_DATA_SIZE];  //max size of buffer #defined in .h file
unsigned char i2c_data_rcv_array[I2C_RCV_DATA_SIZE];

volatile unsigned char exit_timer;  //volatile because this varaible is dec'd in interrupt
volatile unsigned char i2c_wait_timer;
union COMMANDS_FROM_DACT_BIT_BYTE commands_from_dact;
#else
unsigned char bank1 i2c_data_xmit_array[I2C_XMIT_DATA_SIZE];  //max size of buffer #defined in .h file
volatile bank2 unsigned char exit_timer;  //volatile because this varaible is dec'd in interrupt
#endif
//=================================================================================


//====== Constant Global Varaibles used for DIALER communications (also in "externs.h") =================
//    xmit data packet is as follows:  <# of bytes(including this)> <data>...<data>
//                                <CRC high byte> <CRC low byte>
//    The following constant expressions contain ONLY the data bytes to be xmitted to DIALER. The two
//      CRC bytes will be appended by "dialer_com()". In addition, the <#of bytes> will be adjusted
//      to reflect the 2 extra CRC bytes

//---------- use this global array for DACT comm parameters that are variable -----
unsigned char var_dact_data[I2C_XMIT_DATA_SIZE];
//---------------------------------------------------------------------------

//----------- use with read_write parameter = READ_FROM_DIALER --------------------------------
#if (DACT_CODE)
//const unsigned char poll_dialer_for_presence[] = {2,DACT_Poll};
const unsigned char request_data_from_dact[]      = {2,DACT_Request};
const unsigned char provide_config_download[]     = {2,Config_Down};
//const unsigned char provide_idc_text[]            = {3,IDC_Text_Down,0};
//---------------------------------------------------------------------------------------------

//----------- use with read_write parameter = WRITE_TO_DIALER -------------------------------
const unsigned char enter_DACT_program_mode[]     = {3,DACT_Cmd,1};
const unsigned char exit_DACT_program_mode[]      = {3,DACT_Cmd,2};
const unsigned char disable_DACT[]                = {3,DACT_Cmd,3};
const unsigned char enable_DACT[]                 = {3,DACT_Cmd,4};
const unsigned char enter_DACT_edit_passcode[]    = {3,DACT_Cmd,5};
const unsigned char enter_DACT_program_IDC_mode[] = {4,DACT_Cmd,6};   // Specific IDC address should be appended
const unsigned char continue_DACT_program_mode[]  = {3,DACT_Cmd,7};
const unsigned char key_press_ADVANCE_TOP[]       = {3,Key_Press,QueIncrement};
const unsigned char key_press_DECREMENT_TOP[]     = {3,Key_Press,QueDecrement};
const unsigned char key_press_ADVANCE_BOTTOM[]    = {3,Key_Press,DataIncrement};
const unsigned char key_press_DECREMENT_BOTTOM[]  = {3,Key_Press,DataDecrement};
const unsigned char key_press_ENTER[]             = {3,Key_Press,DataEnter};
const unsigned char key_press_IDC1_DISABLE[]      = {3,Key_Press,Key_IDCZ01};
const unsigned char key_press_IDC2_DISABLE[]      = {3,Key_Press,Key_IDCZ02};
const unsigned char key_press_IDC3_DISABLE[]      = {3,Key_Press,Key_IDCZ03};
const unsigned char key_press_IDC4_DISABLE[]      = {3,Key_Press,Key_IDCZ04};
const unsigned char key_press_IDC5_DISABLE[]      = {3,Key_Press,Key_IDCZ05};
const unsigned char key_press_IDC6_DISABLE[]      = {3,Key_Press,Key_IDCZ06};
const unsigned char key_press_IDC7_DISABLE[]      = {3,Key_Press,Key_IDCZ07};
const unsigned char key_press_IDC8_DISABLE[]      = {3,Key_Press,Key_IDCZ08};
const unsigned char key_press_IDC9_DISABLE[]      = {3,Key_Press,Key_IDCZ09};
const unsigned char key_press_IDC10_DISABLE[]     = {3,Key_Press,Key_IDCZ10};
const unsigned char key_press_NAC1_DISABLE[]      = {3,Key_Press,Key_IDCN01};
const unsigned char key_press_NAC2_DISABLE[]      = {3,Key_Press,Key_IDCN02};
const unsigned char key_press_NAC3_DISABLE[]      = {3,Key_Press,Key_IDCN03};
const unsigned char key_press_NAC4_DISABLE[]      = {3,Key_Press,Key_IDCN04};

//-------------------------------------------------------------------------------------------
//========================================================================================================



/*
+------------------------------------------------------------------------------
| Purpose:
|   Calculate the CRC for a given block of data
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|   Function calls:
|       crc_update()    [stack useage: 1]
|                       [TOTAL STACK: 1]
|
+------------------------------------------------------------------------------
| Parameters:
|   s 	- pointer to the start of the block of data
|   len - length of the block of data
|   RxTx - Receive Packet OR Transmit Packet (defined in crc.h)
|       NOTE: len should include the CRC bytes themselves if it is a Received packet
|			  the resultant CRC should be zero.
|
|
+------------------------------------------------------------------------------
| Return Value:
|   the calculated CRC (2-byte integer)
|
+------------------------------------------------------------------------------
*/
unsigned int calc_CRC (unsigned char * s, unsigned char len, unsigned char RxTx)
{
unsigned char *cp;
unsigned int crc_code;

    crc_code = 0;
	for (cp = s; len; cp++, len--)
	{
		crc_code = crc_update (crc_code, *cp);
	}

	if ( RxTx == TX_PACKET )
	{
		crc_code = crc_update( crc_code, '\0' );
		crc_code = crc_update( crc_code, '\0' );
	}

	return ( crc_code );
}


/*
+------------------------------------------------------------------------------
| Purpose:
|   Update the partially completed CRC with the given byte of data
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|   Declared static (local to module)
|
|   Description:
|       -Function uses the CRC 16 polynomial:
|			x16 + x15 + x2 + 1
|       -This routine will take the given byte and add to the calculation of the
|       current/given CRC.
|       -This function must be called once for each character which is to be included in the CRC
|       for packets that are transmitted.
|       -This function must be called once for each character which is to be included in the CRC
|       for packets that are received, AND once for each of the two CRC bytes received.  If the
|       resulting CRC is zero, then the message has been correctly received.
|
|   Function calls:
|       NONE
+------------------------------------------------------------------------------
| Parameters:
|   crc_code 	- current partially completed CRC
|   crc_char	- next byte to add to the current CRC
|
+------------------------------------------------------------------------------
| Return Value:
|   the partially completed CRC (2-byte integer)
|
+------------------------------------------------------------------------------
*/
static unsigned int crc_update (unsigned int crc_code, unsigned char crc_char)
{
    unsigned long x = (unsigned long) crc_code;
    unsigned char i;

	// "x" will contain the character to be processed in bits 0-7 and the CRC
	// in bits 8-23. Bit 24 will be used to test for overflow, and then cleared
	// to prevent the sign bit of "x" from being set to 1. Bits 25-31 are not
	// used. ("x" is treated as though it is a 32 bit register)
	x <<= 8;
	x += (unsigned long) crc_char;

    // Repeat the following loop 8 times (for the 8 bits of the character).
    for ( i = 0; i < 8; i++ )
	{
        // Shift the high-order bit of the character into the low-order bit of the
        // CRC, and shift the high-order bit of the CRC into bit 24.
        x <<= 1;

        // Test to see if the old high-order bit of the CRC was a 1.
        // If the old high-order bit of the CRC was a 1, exclusive-or it with a one
        // to set it to 0, and exclusive-or the CRC with hex 1021 to produce the
        // CCITT-recommended CRC generator of: X**16 + X**12 + X**5 + 1. To produce
        // the CRC generator of: X**16 + X**15 + X**2 + 1, change the constant from
        // 0x01102100 to 0x01800500. This will exclusive-or the CRC with hex 8005
        // and produce the same CRC that IBM uses for their synchronous transmission
        // protocols.

        // Test bit 24 of "x"
        if (x & 0x01000000)
        {
			x = x ^ 0x01102100;	// Exclusive-or "x" with a...
        }
	}

    /* Return the CRC as the 16 low-order bits of this function's value.         */
    /* AND off the unneeded bits and... */
	x &= 0x00ffff00;
    x >>= 8;
	return ((unsigned int) x);
}





/**************************************************************************
 ** FUNCTION: load_poll_for_presence_data                                **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function loads "var_dact_data" array with the contents to       **
 ** poll the DACT with the "poll_for_presence_command" (cmnd #0x01)      **
 **                                                                      **
 ** PARAMETERS:  none                                                    **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void load_poll_for_presence_data(void)
{
    var_dact_data[0] = 8;        //#of bytes (including this)
    var_dact_data[1] = DACT_Poll;//command #
#if (PANEL_10_ZONE)
    var_dact_data[2] = PANEL_TYPE_10ZONE;
#else
	if(five_zone_panel)
        var_dact_data[2] = PANEL_TYPE_5ZONE;
	else
        var_dact_data[2] = PANEL_TYPE_3ZONE;
#endif
	var_dact_data[3] = rev_number[0];
	var_dact_data[4] = rev_number[1];
	var_dact_data[5] = rev_number[2];
	var_dact_data[6] = (unsigned char)system_state.system_state_bit.program;
    var_dact_data[7] = ext_eeprom_read(EEPROM_DATABASE_VERSION);
}





/*
+------------------------------------------------------------------------------
| Purpose: Communicate, via I2C buss, with DIALER
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|   function calls:
|       memcpy()   [stack useage: 1]
|       i2c_comm() [stack useage: 2]
|       calc_CRC() [stack useage: 2]
|                  [TOTAL STACK: 2]
|
+------------------------------------------------------------------------------
| Parameters:  1) read_write values:
|                   WRITE_TO_DIALER
|                   READ_FROM_DIALER
|              2) ptr to constant data array values:
|                   *defined at top of this file
|
+------------------------------------------------------------------------------
| Return Value: bit flag values:
|                   DIALER_COMM_ERROR
|                   DIALER_COMM_OK
|
|
+------------------------------------------------------------------------------
*/
bit dialer_comm(unsigned char read_write, const unsigned char *ptr)
{
    unsigned char i2c_error_ctr;
    unsigned int crc_int;
    unsigned char crc_high_byte;
    unsigned char crc_low_byte;


    //----------- assemble data in "i2c_data_xmit_array[]" for transmission to slave ------------------
    //load buffer with data, from *const, to be xmitted
    memcpy(i2c_data_xmit_array, ptr, *ptr );

    //adjust <#bytes> to reflect 2 appended CRC bytes
    i2c_data_xmit_array[0] = i2c_data_xmit_array[0] + 2;
    crc_int = calc_CRC(i2c_data_xmit_array, ((i2c_data_xmit_array[0]) - 2), TX_PACKET);

    //convert ret'd int to two bytes
    //NOTE: ints are stored little endian (LSB,MSB)
    crc_high_byte = (unsigned char)((crc_int >> 8) & 0x00ff);
    crc_low_byte = (unsigned char)crc_int & 0x00ff;

    //append 2 CRC bytes to end of packet
    i2c_data_xmit_array[ ((i2c_data_xmit_array[0]) - 2) ] = crc_high_byte;
    i2c_data_xmit_array[ ((i2c_data_xmit_array[0]) - 1) ] = crc_low_byte;
    //---------------------------------------------------------------------------------------------

    //----- xmit i2c_data_xmit_array[] ---------------------
    i2c_error_ctr = 0;
    while ( i2c_comm(DACT, I2C_WRITE, i2c_data_xmit_array[0] ) == I2C_ERROR )
    {
        //disable/reset synchronous serial port
        SSPEN = 0;
        i2c_error_ctr++;

// BOC CSM 8/19/2002
        //give time between re-tries
        exit_timer = 5;
        while ( exit_timer ) DogWatch();
// EOC CSM 8/19/2002

        //make 5 attempts (0 through 4)
        if ( i2c_error_ctr > 4)
        {
			dact_trouble.dact_trouble_bit.dactCommFail = 1;
// BOC CSM 8/8/2002
            i2c_buss_reset();
// EOC CSM 8/8/2002
            return(DIALER_COMM_ERROR);
        }
    }
    //-------------------------------------------------
    //give time between re-tries
    exit_timer = 2;
    while ( exit_timer ) DogWatch();

    //----------- receive slave's response into "i2c_data_rcv_array[]" -------
    i2c_error_ctr = 0;
    while(i2c_comm(DACT, I2C_READ, DUMMY) == I2C_ERROR)
    {
        //disable/reset synchronous serial port
        SSPEN = 0;
        i2c_error_ctr++;

// BOC CSM 8/19/2002
        //give time between re-tries
        exit_timer = 5;
        while ( exit_timer ) DogWatch();
// EOC CSM 8/19/2002

        //make 5 attempts (0 through 4)
        if (i2c_error_ctr > 4)
        {
			dact_trouble.dact_trouble_bit.dactCommFail = 1;
// BOC CSM 8/8/2002
            i2c_buss_reset();
// EOC CSM 8/8/2002
            return(DIALER_COMM_ERROR);
        }
    }
    //--------------------------------------------------------------------

    switch(read_write)
    {
        //if we were writing to slave...did slave ACK that CRC received, was good?
        case WRITE_TO_DIALER:

            if (i2c_data_rcv_array[1] != DATA_GOOD_ACK)
            {
				dact_trouble.dact_trouble_bit.dactCommFail = 1;
                return(DIALER_COMM_ERROR);
            }
            break;


        //if we were reading from slave...is CRC, sent from slave, good?
        case READ_FROM_DIALER:

            crc_int = calc_CRC(i2c_data_rcv_array, i2c_data_rcv_array[0], RX_PACKET);

            if ( crc_int )
            {
				dact_trouble.dact_trouble_bit.dactCommFail = 1;
                return(DIALER_COMM_ERROR);
            }
    }

	commands_from_dact.commands_from_dact_byte = 0;	//initialize bit field
    if ( i2c_data_xmit_array[1] == 0x01 && i2c_data_rcv_array[1] == 0x02 )
	{
		commands_from_dact.commands_from_dact_bit.dact_has_data_for_panel = 1;
	}

    if ( i2c_data_xmit_array[1] == 0x02 )
	{
        switch ( i2c_data_rcv_array[1] )
		{
			case 0x40:
				commands_from_dact.commands_from_dact_bit.accept_download = 1;
				break;

			case 0x41:
				commands_from_dact.commands_from_dact_bit.produce_upload = 1;
				break;

			case 0x42:
                commands_from_dact.commands_from_dact_bit.dact_queue_empty = 1;
				break;

			case 0x43:
				commands_from_dact.commands_from_dact_bit.end_dact_program_mode = 1;
				break;

			case 0x45:
				commands_from_dact.commands_from_dact_bit.dact_trouble = 1;
				break;

			case 0x46:
				commands_from_dact.commands_from_dact_bit.end_dact_trouble = 1;
				break;
		}
	}
    dact_trouble.dact_trouble_bit.dactCommFail = 0;   //comm O.K., so restore failure flag
    return(DIALER_COMM_OK);
}

// BOC CSM 8/8/2002
/**************************************************************************
 ** FUNCTION: i2c_buss_reset                                             **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function resets the i2c buss from a bus collision induced latch **
 ** condition.                                                           **
 **                                                                      **
 ** PARAMETERS: none                                                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
unsigned char dactbclif;
static void i2c_buss_reset(void)
{
    if ( !I2C_BUSY_NOT )
    {
        SSPCON1  = 0;
        PORTC   &= 0b11100111;
        while ( !(PORTC & 0b00010000) )
        {
            TRISC   &= 0b11110111;
            i2c_wait_timer = 1;
            while(i2c_wait_timer);
            TRISC   |= 0b00001000;
            i2c_wait_timer = 1;
            while(i2c_wait_timer);
        }

        TRISC   &= 0b11101111;
        i2c_wait_timer = 1;
        while(i2c_wait_timer);
        TRISC   &= 0b11100111;
        i2c_wait_timer = 1;
        while(i2c_wait_timer);
        TRISC   |= 0b00001000;
        i2c_wait_timer = 1;
        while(i2c_wait_timer);
        TRISC   |= 0b00010000;
        dactbclif++;
    }
}
// EOC CSM 8/8/2002
#endif
/**************************************************************************
 ** FUNCTION: ext_eeprom_write                                           **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function writes a byte of information to the EEPROM at the      **
 ** specified address.                                                   **
 **                                                                      **
 ** PARAMETERS: address - address in EEPROM wher data to be written      **
 **             data - data to be written to EEPROM.                     **
 **                                                                      **
 ** RETURNS:  none                                                       **
 **                                                                      **
 **************************************************************************/
void ext_eeprom_write(unsigned char address, unsigned char data)
{
    EEPROM_WRITE(address, data);
    return;
}

/**************************************************************************
 ** FUNCTION: ext_eeprom_read                                            **
 **                                                                      **
 ** DESCRIPTION:                                                         **
 ** This function reads a byte of information from the EEPROM at the     **
 ** specified address, and returns it.                                   **
 **                                                                      **
 ** PARAMETERS: address - address in EEPROM where data resides.          **
 **                                                                      **
 ** RETURNS:  data read from EEPROM at address location                  **
 **                                                                      **
 **************************************************************************/
unsigned char ext_eeprom_read(unsigned char address)
{
    unsigned char ret_val;

    ret_val = EEPROM_READ(address);
    return ret_val;
}

/*
+------------------------------------------------------------------------------
| Purpose: Low level communication with DIALER, via I2C buss
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|       Declared STATIC (local to module)
|
|       Function calls:
|           wait_SSPIF() [stack useage: 1]
|                        [TOTAL STACK: 1]
|
+------------------------------------------------------------------------------
| Parameters:   1) read_write values:
|                  I2C_READ
|                  I2C_WRITE
|
|               2) num_xmit_bytes:
|                   *used only when transmitting to DIALER. Use dummy value
|                       (DUMMY) when receiving (read_write = I2C_READ)
|
+------------------------------------------------------------------------------
| Return Value:
|       returned bit flag values:
|           I2C_ERROR
|           I2C_OK
|
+------------------------------------------------------------------------------
*/
#if (DACT_CODE)
static bit i2c_comm(unsigned char destination, unsigned char read_write, unsigned char num_xmit_bytes)
{
    signed char byte_counter;

    //SCL(RC3) & SDA(RC4) configured as inputs so that MSSP module can drive the pins
    TRISC |= 0b10011000;

    //baud rate generator set for 100K
    //SSPADD = 9;

    //+++ changed to 19K BAUD. 100K BAUD too fast for DACT processing overhead
    //SSPADD = 50;
    SSPADD = 8;

    //SSPM3:SSPM0=1000: I2C Master Mode, clock=Fosc/(4*(SSPADD+1))
    SSPCON1 = 0b00001000;


	//wait for DACT to free I2C buss (BUSY line allowed to float high when buss is free)
    //load timer with 110mS value so we don't wait forever
    exit_timer = 11;
    //disable/reset synchronous serial port while BUSY active
    while ( !I2C_BUSY_NOT && exit_timer ) DogWatch();  //toggle external watch dog timer
    if ( !exit_timer )
    {
        return(I2C_ERROR);
    }

    //re-enable in case turned off waiting for BUSY line to go high
    SSPEN = 1;

    //generate START
    SEN = 1;
    if (wait_SSPIF() != SSPIF_RESPONDED)
    {
        return(I2C_ERROR);
    }

    //load 7 bit address (address = 1,   address located in bits 7 - 1)
    //   + read/write command bit (read = 1, write = 0)
#if (DACT_ADDR_1)
    // Address set for 1
    SSPBUF = 0b00000010 | read_write;
#elif (DACT_ADDR_8)
    // Address set for 8
    SSPBUF = 0b00010000 | read_write;
#endif

    if (wait_SSPIF() != SSPIF_RESPONDED)
    {
        return(I2C_ERROR);
    }

    //check if slave didn't sent ACK
    if (ACKSTAT)
    {
        return(I2C_ERROR);
    }

    switch (read_write)
    {
        case I2C_READ:

            //set recieve enable
            RCEN = 1;

            //get number of bytes slave is going to send
            if ( wait_SSPIF() != SSPIF_RESPONDED )
            {
                return(I2C_ERROR);
            }

            //set ACK signal to verify reception of # of data bytes
            ACKDT = 0;

            //send the ACK signal to SLAVE
            ACKEN = 1;

            //wait for ACK to finish
            if ( wait_SSPIF() != SSPIF_RESPONDED )
            {
                return(I2C_ERROR);
            }

            //determine how many bytes slave is going to send (including 2 CRC bytes)
            i2c_data_rcv_array[0] = SSPBUF;
			//if all 1's clocked in (255) it means that no one responded (no one pulled data bits down
			//   while data was trying to be clocked in by MASTER)
// BOC CSM 8/25/2002
//            if(i2c_data_rcv_array[0] == 255)
            if( i2c_data_rcv_array[0] > I2C_RCV_DATA_SIZE )
// EOC CSM 8/25/2002
			{
				return(I2C_ERROR);
			}

            //this loop clocks in, byte by byte, slave's data
            for ( byte_counter=1; byte_counter < i2c_data_rcv_array[0]; byte_counter++ )
            {
                //set receive enable
                RCEN = 1;

                if ( wait_SSPIF() != SSPIF_RESPONDED )
                {
                    return(I2C_ERROR);
                }

                //put slave's data in buffer
                i2c_data_rcv_array[byte_counter] = SSPBUF;

                //check if we're at end of buffer
                if ( byte_counter == (i2c_data_rcv_array[0]-1) )
                {
                    //set NACK value to be sent to signal that we don't need any further slave data
                    ACKDT = 1;
                }
                else
                {
                    ACKDT = 0;
                }

                //send above selected ACK or NACK signal to slave
                ACKEN = 1;

                if ( wait_SSPIF() != SSPIF_RESPONDED )
                {
                    return(I2C_ERROR);
                }
            }
            break;

        case I2C_WRITE:

            //this loop sends, byte by byte, contents of xmit buffer
            for ( byte_counter=0; byte_counter < num_xmit_bytes; byte_counter++ )
            {
                SSPBUF = i2c_data_xmit_array[byte_counter];

                if ( wait_SSPIF() != SSPIF_RESPONDED )
                {
                    return(I2C_ERROR);
                }

                //check if slave didn't send ACK
                if ( ACKSTAT )
                {
                    return(I2C_ERROR);
                }
            }

    }

    //generate STOP
    PEN = 1;

    if ( wait_SSPIF() != SSPIF_RESPONDED )
    {
        return(I2C_ERROR);
    }

    //disable/reset synchronous serial port
    SSPEN = 0;

    return(I2C_OK);
}


/*
+------------------------------------------------------------------------------
| Purpose:  Wait for micro's bit "SSPIF" to be set. If not set prior to "exit_timer"
|            decrementing to 0 (decrements done within interrupt), an ERROR has
|            ocurred.
|
+------------------------------------------------------------------------------
| Algorithms and Vital Information:
|           Declared static (local to module)
|
|           No function calls
|
+------------------------------------------------------------------------------
| Parameters: NONE
|
+------------------------------------------------------------------------------
| Return Value:
|   SSPIF_RESPONDED
|   SSPIF_STUCK
|   I2C_COLLISION
|
+------------------------------------------------------------------------------
*/
static bit wait_SSPIF(void)
{
    /* wait only 50ms for interrupt */
    exit_timer = 5;

    //toggle of external WATCHDOG
    //PORTC ^= (1 << 0);
    DogWatch();

    while ( !SSPIF )
    {
        if ( !exit_timer )
        {
// BOC CSM 8/8/2002
            i2c_buss_reset();
// EOC CSM 8/8/2002
            return( SSPIF_STUCK );
        }

#if (0)
        if ( WCOL )
        {
            WCOL = 0;
            return( I2C_COLLISION );
        }
#endif

        //see if I2C collision (DACT might be accessing its EEPROM)
        if ( BCLIF )
		{
			BCLIF = 0;

// BOC CSM 8/8/2002
            i2c_buss_reset();
// EOC CSM 8/8/2002
			return(I2C_COLLISION);
		}

    }
    SSPIF = 0;
    return( SSPIF_RESPONDED );
}
#endif
