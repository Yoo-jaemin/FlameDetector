/*
*******************************************************************************
* COPYRIGHT (c) 1992-2012 GASTRON Co.,Ltd. All rights reserved.
*******************************************************************************
* Model Name	: GTF-1100 Series
* Module name	: 
* Description	: 
* Version		: REV 0.51
* Start Date	: 2017.01.17
* Modify Date	: 2018.06.14
* C-Compiler	: avr-gcc
* MPU			: ATmega1281
* Written by	: Min-Sung, Kang (mskang@gastron.com)
*******************************************************************************
*/

#ifndef __MODBUS_H__
#define __MODBUS_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mmi.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MB_ADDRESS_BROADCAST					(   0 ) /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN							(   1 ) /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX							( 247 ) /*! Biggest possible slave address. */
#define MB_FUNC_NONE							(   0 )
#define MB_FUNC_READ_COILS						(   1 )
#define MB_FUNC_READ_DISCRETE_INPUTS			(   2 )
#define MB_FUNC_WRITE_SINGLE_COIL				(   5 )
#define MB_FUNC_WRITE_MULTIPLE_COILS			(  15 )
#define MB_FUNC_READ_HOLDING_REGISTER			(   3 )
#define MB_FUNC_READ_INPUT_REGISTER				(   4 )
#define MB_FUNC_WRITE_REGISTER					(   6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS		(  16 )
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS	(  23 )
#define MB_FUNC_DIAG_READ_EXCEPTION				(   7 )
#define MB_FUNC_DIAG_DIAGNOSTIC					(   8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT			(  11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG			(  12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID			(  17 )
#define MB_FUNC_ERROR							( 128 )

#define MB_FUNC_RATIO_WRITE						( 0x41 ) // Ratio Write User Code

#define MRX_STATNO					0	//Station Number
#define MRX_FUNCODE					1	//Function Code
#define MRX_ADDHGH					2	//Address High
#define MRX_ADDLOW					3	//Address Low
#define MRX_DATACHGH				4	//Data Count High
#define MRX_DATACLOW				5	//Data Count Low
#define MRX_BYTECNT					6	//Byte Count
#define MRX_CHKSUM_LOW				6	//Checksum High
#define MRX_CHKSUM_HGH				7	//Checksum Low

#define MTX_STATNO					0	//Station Number
#define MTX_FUNCODE					1	//Function Code
#define MTX_ADDHGH					2	//Address High
#define MTX_BYTECNT					2	//Byte Count
#define MTX_ADDLOW					3	//Address Low
#define MTX_DATA					3	//Trans Data
#define MTX_DATACHGH				3	//Data High
#define MTX_DATACLOW				4	//Data Low

#define MAX_INPUT_REGISTER			65

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern U16	Modbus_InputReg[MAX_INPUT_REGISTER];
extern U32	Modbus_Baud;
extern U8	Modbus_Addr;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern U16 CRC16( const U8 *nData, U16 wLength );
extern U16 Ratio_CRC16( void );
extern void Initialize_MODBUS( void );
extern void FlameData_Mapping( FLAME_HandleTypeDef *f );
extern void MODBUS_Packet_Decoder( U8* pBuf );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//__MODBUS_H__
