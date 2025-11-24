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

#define MB_ADDRESS_BROADCAST				 (   0 ) /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN						 (   1 ) /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX						 ( 247 ) /*! Biggest possible slave address. */
#define MB_FUNC_NONE						 (   0 )
#define MB_FUNC_READ_COILS					 (   1 )
#define MB_FUNC_READ_DISCRETE_INPUTS		 (   2 )
#define MB_FUNC_WRITE_SINGLE_COIL			 (   5 )
#define MB_FUNC_WRITE_MULTIPLE_COILS		 (  15 )
#define MB_FUNC_READ_HOLDING_REGISTER		 (   3 )
#define MB_FUNC_READ_INPUT_REGISTER			 (   4 )
#define MB_FUNC_WRITE_REGISTER				 (   6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS     (  16 )
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS (  23 )
#define MB_FUNC_DIAG_READ_EXCEPTION			 (   7 )
#define MB_FUNC_DIAG_DIAGNOSTIC				 (   8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT		 (  11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG		 (  12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID		 (  17 )
#define MB_FUNC_ERROR						 ( 128 )

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

// Function Code
#define MTX_FUNCODE_INPUT							4
#define MTX_FUNCODE_HOLDING							3
#define MTX_FUNCODE_WRITE_SINGLE					6
#define MTX_FUNCODE_WRITE_MULTIPLE					16
#define MTX_FUNCODE_WRITE_MULTIPLE_RATIO_LMT		0x41

#define INPUT_REG_MAX_CNT				65
#define HOLDING_REG_MAX_CNT				125 
#define MAX_INPUT_REGISTER				65

#define FW_SUB_VER		1


//==========================================================================================================================
// jmyoo

/* ********************************************************************
- 1바이트 = 1 Start + 8 Data + 1 Parity + 1 Stop = 11 bits
- 전송 속도 9600bps → 1bit 전송 시간 = 1 / 9600 ≈ 104.17 µs
- 1 byte 전송 시간 ≈ 11 × 104.17 µs ≈ 1.146 ms
- 3.5 Character Time ≈ 1.146 ms × 3.5 ≈ 4.01 ms
******************************************************************** */
#define	WARMUP_TIME				100
#define	MODBUS_RTU_TIME_OUT		1000  

#define	NOT_CONNECTED_TIME		200

enum {
	MODBUS_RTU_MASTER_READY,
	MODBUS_RTU_MASTER_DELAY,
	READ_INPUT_REGISTER_REQUEST,
	READ_HOLDING_REGISTER_REQUEST,
	WRITE_REGISTER_REQUEST
} e_ModBus_RTU_Master_State;

enum {
	NONE,
	_1STEP_,
	_2STEP_,
	_3STEP_,
	_4STEP_,
	WAIT,
	THE_END
} e_ModBus_RTU_Master_Write_Step;

enum {
	LOW_ADDRESS,
	HI_ADDRESS	
};

typedef struct {
	U8	seq;
	U8	next_seq;
	
	U8	comm_status;
	U8	holding_reg_addr;	
	
	U8	write_flag;
	U8	write_step;
	U8  write_next_step;
	U8  timeout;
	U8  connect_chk_cnt;
	U16	delay_ms;
	
} s_ModBus_RTU_Master;

typedef struct {

	U16 startAddress;
	U16 endAddress;
	U16 to_send;
	U16 totalSend;
} s_ModBus_Mutiple_Wr;

enum {
	MODBUS_RTU_MASTER_OK,
	MODBUS_RTU_MASTER_BUSY,
	MODBUS_RTU_MASTER_ADDRESS_MISMATCH,
	MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE,
	MODBUS_RTU_MASTER_CRC_ERROR,
	MODBUS_RTU_MASTER_TIMEOUT,
};


extern s_ModBus_RTU_Master modbus_rtu_master;
extern s_ModBus_Mutiple_Wr multiple_wr;

extern void MODBUS_Packet_Decoder_Master( U8* pBuf );
extern void MODBUS_RTU_Master_Init(void);
extern void MODBUS_RTU_Master_Task(void);
//==========================================================================================================================

extern U16	Modbus_InputReg[MAX_INPUT_REGISTER];
extern U32	Modbus_Baud;
extern U8	Modbus_Addr;
extern S16	write_register_data_arry[512];
extern U8	Det_Address, Dvc_Address;
extern U8	Det_BPS;

extern U16 TOSEnd;

extern void Initialize_MODBUS( void );
extern U16 CRC16( const U8 *nData, U16 wLength );
extern void MODBUS_Packet_Decoder( U8* pBuf );


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//__MODBUS_H__
