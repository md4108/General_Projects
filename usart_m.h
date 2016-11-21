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
| 8/9/2005   C. McGill       Modified PERIPH_COMM defines to set command last.
|
+------------------------------------------------------------------------------
|  PVCS Version Control Section, Do Not Modify
|
| $Workfile:   usart_m.h  $
|     $Date:   Aug 09 2005 13:47:04  $
|  $Modtime:   Aug 09 2005 13:46:22  $
| $Revision:   1.3  $
|  $Archive:   \\amusbrapvcs\ENG\PVCS\vm67\vmProjects\FireShield\archives\7400037 - Conventional Panel-3 & 5 Zone\Source\usart_m.h-arc  $
|   $Author:   mcgill  $
+------------------------------------------------------------------------------
*/


#ifndef H_USART_M
    #define H_USART_M

        //========== Global Prototypes (extern statement in file: "externs.h") ===========
        void usart_comm(void);
        void init_usart(void);
        void supervise_peripherals(void);

        unsigned char status_to_periph(unsigned char sys_or_idc_or_nac, unsigned char idc_num, unsigned char sys_idc_nac_status, unsigned char sys_status2);

        //================================================================================


		struct XMIT_PACKET_TYPE
		{
		    unsigned char status;	//this byte not transmitted, for internal use only
		    unsigned char address;
		    unsigned char command;  //command = 0 means this packet buffer loc is empty
            unsigned char data[IDC_TEXT_LENGTH+1]; //max data size transmitted...
		};

		struct RECEIVE_PACKET_TYPE
		{
		    unsigned char address;
            unsigned char data[MAX_RCV_PACKET_DATA_INDEX+1];
		    unsigned char check_sum;
		};

        //===== Macros for peripheral communications ==============================

        //Use this macro to initiate communication with addressed peripheral. It loads struct with data and sets task request
        //    bit for real time kernal. Before using, check status = SERVICE_AVAILABLE.
//        #define PERIPH_COMM(addr,comnd,data0,data1,data2)  xmit_packet.address = (addr); xmit_packet.command = (comnd);\
//                                    xmit_packet.data[0] = (data0); xmit_packet.data[1] = (data1); xmit_packet.data[2] = (data2);

//        #define PERIPH_COMM_TEXT(addr,comnd,data0,data1)  xmit_packet.address = (addr); xmit_packet.command = (comnd);\
//                                    xmit_packet.data[0] = (data0); memcpy( &xmit_packet.data[1], (data1), IDC_TEXT_LENGTH );
        #define PERIPH_COMM(addr,comnd,data0,data1,data2)  xmit_packet.address = (addr); xmit_packet.data[0] = (data0);\
                                    xmit_packet.data[1] = (data1); xmit_packet.data[2] = (data2); xmit_packet.command = (comnd);

        #define PERIPH_COMM_TEXT(addr,comnd,data0,data1)  xmit_packet.address = (addr); xmit_packet.data[0] = (data0);\
                                    memcpy( &xmit_packet.data[1], (data1), IDC_TEXT_LENGTH ); xmit_packet.command = (comnd);


        //Use this macro to acknowledge a communication failure
        #define CLEAR_COMM_FAILURE xmit_packet.status = COMM_FAILURE_ACKED

        //Use this macro to acknowledge that valid data has been read
        #define ACK_RCV_DATA_VALID xmit_packet.status = RECEIVE_DATA_READ
        //==========================================================================
#endif
