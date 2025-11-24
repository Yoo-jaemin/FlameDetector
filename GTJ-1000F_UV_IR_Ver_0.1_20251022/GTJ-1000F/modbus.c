/* *********************************************************************************
 *  GTJ-1000F.c
 *
 *	Created: 2025-09-30 
 *	Author : jmyoo
 *  Description : GTF1100 <-> Jig (Modbus communication)
  
 ********************************************************************************* */ 

#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "global.h"
#include "dbg_printf.h"

#include "modbus.h"
#include "uart2560.h"
#include "gpio.h"
#include "timer.h"
#include "mmi.h"
#include "lcd.h"
#include "eeprom.h"
#include "mkey.h"

#define MAX_WRITE_REG	80	


const U16 wCRCTable[] = {
   0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
   0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
   0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
   0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
   0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
   0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
   0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
   0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
   0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
   0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
   0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
   0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
   0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
   0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
   0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
   0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
   0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
   0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
   0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
   0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
   0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
   0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
   0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
   0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
   0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
   0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
   0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
   0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
   0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
   0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
   0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
   0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};


S16	write_register_data_arry[512];
U8	Det_Address, Dvc_Address;
U8	Det_BPS;

s_ModBus_RTU_Master modbus_rtu_master;
s_ModBus_Mutiple_Wr multiple_wr;

void Com_Read_Input_Register_Slave( U8 *rxbuf );
void Com_Read_Holding_Register_Slave( U8 *rxbuf );
void Com_Write_Single_Register_Slave( U8 *rxbuf );
void Com_Write_Multiple_Register_Slave( U8 *rxbuf );

static void MODBUS_RTU_Read_Register_Packet_Decoder_Master( U8* pBuf );
static void MODBUS_RTU_Write_Register_Packet_Decoder_Master( U8* pBuf );
static void MODBUS_RTU_Input_Register_Packet_Send_Master(void);
static void MODBUS_RTU_Holding_Register_Packet_Send_Master(void);
static void MODBUS_RTU_Write_Register_Packet_Send_Master(void);
static void MODBUS_RTU_Master_Register_Clear(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Initialize_MODBUS( void )
{
	memset( write_register_data_arry, 0, sizeof(write_register_data_arry) );
}

U16 CRC16( const U8 *nData, U16 wLength )
{
	U8 nTemp;
	U16 wCRCWord = 0xFFFF;
	
	while (wLength--)
	{
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord  ^= wCRCTable[nTemp];
	}
	return wCRCWord;
}

void MODBUS_RTU_Master_Init(void)
{
	modbus_rtu_master.seq					= MODBUS_RTU_MASTER_READY;
	modbus_rtu_master.holding_reg_addr		= HI_ADDRESS;
	modbus_rtu_master.delay_ms				= 0;
	modbus_rtu_master.connect_chk_cnt		= 0;
	modbus_rtu_master.write_flag			= 0;
	modbus_rtu_master.write_step			= NONE +1;
	modbus_rtu_master.write_next_step		= THE_END;
	Det_Modbus_Buf[Det_Comm_Flow]			= Commnication_Normal; 
	
	multiple_wr.startAddress				= 0;
	multiple_wr.endAddress					= 0;
	multiple_wr.to_send						= 0;
	multiple_wr.totalSend					= 0;
}

void MODBUS_Packet_Decoder_Master( U8* pBuf )
{			
	switch( *(pBuf+MRX_FUNCODE) )
	{
		case 0x03:	case 0x04:
			MODBUS_RTU_Read_Register_Packet_Decoder_Master( pBuf );
			break;
		case 0x06:	case 0x10: case 0x41:
			MODBUS_RTU_Write_Register_Packet_Decoder_Master( pBuf );		
			break;
	}
}

static void MODBUS_RTU_Read_Register_Packet_Decoder_Master( U8* pBuf )
{
	U16 bc = *(pBuf+2);
	U16 wCRC = CRC16(pBuf, 3+bc);
	U16 i;
	U8 FC = *(pBuf+1);
	
	if ( ((U8)(wCRC & 255) == *(pBuf+3+bc)) && ((U8)(wCRC>>8 & 255) == *(pBuf+4+bc)) )
	{
		for (i=0; i<bc/2; i++) 
		{
			if (FC == 0x04)	Det_Input_Reg_Data[i] = ((U16)*(pBuf+3+i*2)) <<8 | (U16)*(pBuf+4+i*2);
			else {
				if (Det_Input_Reg_Data[DET_InputReg_FW_SUB_VERSION] == FW_SUB_VER)
					Det_Holding_Reg_Data[i + (modbus_rtu_master.holding_reg_addr * (HOLDING_REG_MAX_CNT-7))] = ((U16)*(pBuf+3+i*2)) <<8 | (U16)*(pBuf+4+i*2);
				else
					Det_Holding_Reg_Data[i + (modbus_rtu_master.holding_reg_addr * HOLDING_REG_MAX_CNT)] = ((U16)*(pBuf+3+i*2)) <<8 | (U16)*(pBuf+4+i*2);
			}
		}
		for (i=0; i<bc+5; i++)
			DPRINT1(" %x ", *(pBuf+i));
		DPRINT("\r\n");
		
		DPRINT1(" [FC: %d] Read reg OK \r\n\r\n",FC);
		modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_OK;
	}
	else
	{
		DPRINT(" CRC err\r\n\r\n");
		modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_CRC_ERROR;
	}
}

static void MODBUS_RTU_Write_Register_Packet_Decoder_Master( U8* pBuf )
{
	U16 wCRC = CRC16(pBuf, 6);
	U8 FC = *(pBuf+1);
	U16 i;
	
	if ( ((U8)(wCRC & 255) == *(pBuf+6)) && ((U8)(wCRC>>8 & 255) == *(pBuf+7)) ) {
		modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_OK;	
		for (i = 0; i < 8; i++)
			DPRINT1(" %x ", *(pBuf+i));
		DPRINT("\r\n");
		
		DPRINT1(" [FC: %d] Write reg OK \r\n\r\n",FC);
		if (Det_Modbus_Buf[Det_Write_Over_Cnt_Flag]) {
			modbus_rtu_master.write_flag = 1;
			Det_Modbus_Buf[Det_Write_Register_Addr] = multiple_wr.startAddress + multiple_wr.to_send;
		}		
		else	modbus_rtu_master.write_flag = 0; 
		
		if (Det_Modbus_Buf[Det_Write_Register_Flag] == WRITE_MULTIPLE_REGISTER) {
			multiple_wr.totalSend += multiple_wr.to_send;
			DPRINT1(" Rcv ok -> totalSend %d \r\n\r\n", multiple_wr.totalSend);
		}
	} 
	else {
		DPRINT(" CRC err\r\n\r\n");
		modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_CRC_ERROR;	
	}	
}

void MODBUS_RTU_Master_Task(void)
{	
	switch (modbus_rtu_master.seq)
	{
		case MODBUS_RTU_MASTER_READY:
			if (modbus_rtu_master.delay_ms >= WARMUP_TIME)
				modbus_rtu_master.seq = READ_INPUT_REGISTER_REQUEST;
			break;
		case MODBUS_RTU_MASTER_DELAY:
			if (modbus_rtu_master.delay_ms > MODBUS_RTU_TIME_OUT) 
			{
				modbus_rtu_master.delay_ms = 0;
				
				if (modbus_rtu_master.comm_status !=  MODBUS_RTU_MASTER_OK) {	
					MODBUS_RTU_Master_Register_Clear();
					DPRINT(" MODBUS Time-Out! \r\n");
					
					if (modbus_rtu_master.write_flag)	// 슬레이브 측에서 ack 받지 못했거나 CRC 에러이면
					{						
						if (Det_Modbus_Buf[Det_Write_Register_Flag] == WRITE_MULTIPLE_REGISTER)
							Det_Modbus_Buf[Det_Write_Register_Addr] = multiple_wr.startAddress;
						modbus_rtu_master.seq = WRITE_REGISTER_REQUEST;		// retry
					} 
				} 
					
				if (modbus_rtu_master.write_flag)
					modbus_rtu_master.seq = WRITE_REGISTER_REQUEST;
				else {
					Det_Modbus_Buf[Det_Comm_Flow] = Commnication_Normal;
					modbus_rtu_master.seq = modbus_rtu_master.next_seq;
					modbus_rtu_master.write_step =  NONE +1;
				}
			} break;
		case READ_INPUT_REGISTER_REQUEST:
			MODBUS_RTU_Input_Register_Packet_Send_Master();
			modbus_rtu_master.delay_ms = 0;
			modbus_rtu_master.seq = MODBUS_RTU_MASTER_DELAY;
			modbus_rtu_master.next_seq = READ_HOLDING_REGISTER_REQUEST;
			modbus_rtu_master.holding_reg_addr ^= 1;
			break;
		case READ_HOLDING_REGISTER_REQUEST:
			MODBUS_RTU_Holding_Register_Packet_Send_Master();
			modbus_rtu_master.delay_ms = 0;
			modbus_rtu_master.seq = MODBUS_RTU_MASTER_DELAY;
			modbus_rtu_master.next_seq = READ_INPUT_REGISTER_REQUEST;
			break;
		case WRITE_REGISTER_REQUEST:
			MODBUS_RTU_Write_Register_Packet_Send_Master();
			modbus_rtu_master.delay_ms = 0;
			modbus_rtu_master.seq = MODBUS_RTU_MASTER_DELAY;
			break;
	}
}

/* **********************************************************************************
[ Read Input Registers (Function Code 0x04) ]
Req : | Slave | Func | Start addr | Quantity reg | CRC ]
		  1	      1        2              2         2
Resp: | Slave | Func | Byte cnt | Data | CRC |
          1       1        1        N     2 
********************************************************************************** */ 
static void MODBUS_RTU_Input_Register_Packet_Send_Master(void)
{
	U8	bData[256];
	U16	index = 0;
	U16	wCRC;
	
	bData[index++] = Det_Address;
	bData[index++] = MTX_FUNCODE_INPUT;
	bData[index++] = 0;
	bData[index++] = 0;
	bData[index++] = 0;
	bData[index++] = INPUT_REG_MAX_CNT;
	wCRC = CRC16( &bData[0], 6 );
	bData[index++] = wCRC & 255;
	bData[index++] = wCRC >> 8 & 255;
	USART2_Packet_Send( bData, index );
	modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_BUSY;
}

/* **********************************************************************************
[ Read Holding Registers (Function Code 0x03) ]
Req : | Slave | Func | Start addr | Quantity reg | CRC ]
		  1	      1        2              2         2
Resp: | Slave | Func | Byte cnt | Data | CRC |
          1       1        1        N     2 
********************************************************************************** */
static void MODBUS_RTU_Holding_Register_Packet_Send_Master(void)
{
	U8	bData[256];
	U16	index = 0;
	U16	wCRC;
	
	bData[index++] = Det_Address;
	bData[index++] = MTX_FUNCODE_HOLDING;
	bData[index++] = 0;
	if (Det_Input_Reg_Data[DET_InputReg_FW_SUB_VERSION] == FW_SUB_VER)		bData[index++] = modbus_rtu_master.holding_reg_addr * (HOLDING_REG_MAX_CNT-7);  
	else																	bData[index++] = modbus_rtu_master.holding_reg_addr * (HOLDING_REG_MAX_CNT); 
	
	bData[index++] = 0;
	if (Det_Input_Reg_Data[DET_InputReg_FW_SUB_VERSION] == FW_SUB_VER)		bData[index++] = HOLDING_REG_MAX_CNT-7;
	else																	bData[index++] = HOLDING_REG_MAX_CNT;
	
	wCRC = CRC16( &bData[0], 6 );
	bData[index++] = wCRC & 255;
	bData[index++] = wCRC >> 8 & 255;
	USART2_Packet_Send( bData, index );	
	modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_BUSY;
}

/* **********************************************************************************
[ Write Single Register (Function Code: 0x06) ]
Req : | Slave | Func | Reg addr | Reg value | CRC |
	      1      1        2           2        2   
Resq: | Slave | Func | Reg addr | Reg value | CRC |
          1      1        2           2        2
 
[ Write Multiple Registers (Function Code: 0x10) ]
Req: | Slave | Func | Start addr | Quantity reg | Byte cnt | Reg value | CRC |
	    1	    1	      2              2           1           N        2 
Resp:| Slave | Func | Start addr | Quantity reg |  CRC  |
		1       1         2             2           2
********************************************************************************** */
/* **********************************************************************************
[ Master ]
9600 bps
Usart frame : 1 start bit + 8 data + 1 stop bit = 10bit / byte
1byte 전송시간 = 10 / 9600초 = 1.04 ms

[ Modbus RTU Write Multiple Register Packet Frame ]

Slave addr|	Func code |	St addr | Reg cnt |  Byte cnt |  Data  |  Crc  |
   1byte	  1byte	     2byte	   2byte	  1byte	    N byte	 2byte	  ->  9+N [byte]

ex)	Reg 20개 전송 : 51ms ( (40+9)*1.04 )     -> 51+3.7 = 55ms
    Reg 40개 전송 : 93ms
    Reg 80개 전송 : 166ms

Idle 시간 : 데이터 전송이 끝난 후, 3.5char 시간이 필요하며, 그래야지 다음 프레임 인식
1/9600*3.5 = 3.7ms

-> 슬레이브 수신 시간도 동일 -> + eep write, usart 버퍼 처리 시간
********************************************************************************** */

static void MODBUS_RTU_Write_Register_Packet_Send_Master(void)
{
	U8	bData[256];
	U16	index = 0;
	U16	wCRC;
	
	U16 remaining, toSend;
	U8 i;
	
	const S16* pTable = NULL;
	pTable = (const S16*)pgm_read_ptr(&DET_EEP_RATIO_LIMIT[mmi_tmp[line_2]]);
	
	switch (Det_Modbus_Buf[Det_Write_Register_Flag])
	{
		case WRITE_SINGLE_REGISTER:
			DPRINT2(" Single_WriteReg: %d, %d\r\n",Det_Modbus_Buf[Det_Write_Register_Addr], Det_Modbus_Buf[Det_Write_Register_Data]);
			bData[index++] = Det_Address;
			bData[index++] = MTX_FUNCODE_WRITE_SINGLE;
			bData[index++] = Det_Modbus_Buf[Det_Write_Register_Addr] >> 8 & 255;
			bData[index++] = Det_Modbus_Buf[Det_Write_Register_Addr] & 255;
			bData[index++] = Det_Modbus_Buf[Det_Write_Register_Data] >> 8 & 255;
			bData[index++] = Det_Modbus_Buf[Det_Write_Register_Data] & 255;
			wCRC = CRC16( &bData[0], index );
			bData[index++] = wCRC & 255;
			bData[index++] = wCRC >> 8 & 255;
			USART2_Packet_Send( bData, index );
			Det_Modbus_Buf[Det_Write_Over_Cnt_Flag] = 0;
			modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_BUSY;
			break;
			
		case WRITE_MULTIPLE_REGISTER:
			multiple_wr.startAddress = Det_Modbus_Buf[Det_Write_Register_Addr];
			multiple_wr.endAddress   = Det_Modbus_Buf[Det_Write_Register_Data];
			
			remaining = multiple_wr.endAddress - multiple_wr.startAddress;
			
			if (remaining > MAX_WRITE_REG) {
				toSend = MAX_WRITE_REG;
				Det_Modbus_Buf[Det_Write_Over_Cnt_Flag] = 1;   
			} else {
				toSend = remaining;
				Det_Modbus_Buf[Det_Write_Over_Cnt_Flag] = 0;   
			}
			
			bData[index++] = Det_Address;
			if (multiple_wr.startAddress >= TRIPLE_IR_REL_RATIO_395_SIZE_1)		bData[index++] = MTX_FUNCODE_WRITE_MULTIPLE_RATIO_LMT;	
			else																bData[index++] = MTX_FUNCODE_WRITE_MULTIPLE;
			bData[index++] = multiple_wr.startAddress >> 8 & 255;
			bData[index++] = multiple_wr.startAddress & 255;
			bData[index++] = toSend >> 8 & 255;
			bData[index++] = toSend & 255;
			bData[index++] = toSend * 2;
			
			// Write register
			if (Det_Modbus_Buf[Det_Comm_Flow_Set] == Commnication_IR_Calribration_Set) {
				for (i = 0; i < toSend; i++)
				{
					bData[index++] = Det_IrAvPtoP_Data[i] >> 8 & 255;
					bData[index++] = Det_IrAvPtoP_Data[i] & 255;
				}
			} else if (mmi_mode == FMODE_DET_TRIPLE_REL_RATIO_LIMIT_SET) {
				for (i = 0; i < toSend; i++)
				{
					bData[index++] = pgm_read_word(&pTable[(multiple_wr.startAddress - TRIPLE_IR_REL_RATIO_395_SIZE_1) +i]) >> 8 & 255;;
					bData[index++] = pgm_read_word(&pTable[(multiple_wr.startAddress - TRIPLE_IR_REL_RATIO_395_SIZE_1) +i]) & 255;		
					DPRINT2(" ** Addr: %d  Val: %d\r\n", (multiple_wr.startAddress - TRIPLE_IR_REL_RATIO_395_SIZE_1) +i, pgm_read_word(&pTable[(multiple_wr.startAddress - TRIPLE_IR_REL_RATIO_395_SIZE_1) +i]) );
				}
			} else {
				for (i = 0; i < toSend; i++)
				{
					bData[index++] = pgm_read_word( &DET_EEP_SYS_DEFAULT[multiple_wr.startAddress  + i] ) >> 8 & 255;
					bData[index++] = pgm_read_word( &DET_EEP_SYS_DEFAULT[multiple_wr.startAddress  + i] ) & 255;
				}	
			}
			
			wCRC = CRC16( &bData[0], index );
			bData[index++] = wCRC & 255;
			bData[index++] = wCRC >> 8 & 255;
			USART2_Packet_Send( bData, index );
			DPRINT2(" Multi_WriteReg s_addr: %d, toSend: %d \r\n", multiple_wr.startAddress, toSend);
			
			modbus_rtu_master.comm_status = MODBUS_RTU_MASTER_BUSY;
			multiple_wr.to_send = toSend;
			break;	 
	}
}

static void MODBUS_RTU_Master_Register_Clear(void)
{
	DET_Rx2Cnt = 0;
	memset(Det_Rx2Buf, 0, sizeof(Det_Rx2Buf));
	memset(Det_Input_Reg_Data, 0, sizeof(Det_Input_Reg_Data));
	memset(Det_Holding_Reg_Data, 0, sizeof(Det_Holding_Reg_Data));
}

//==========================================================================================================================

void MODBUS_Packet_Decoder( U8* pBuf )
{
	switch( *(pBuf+MRX_FUNCODE) )
	{
		case 0x03:	Com_Read_Holding_Register_Slave( pBuf );	break;
		case 0x04:	Com_Read_Input_Register_Slave( pBuf );		break;
		case 0x06:
			Det_Modbus_Buf[Det_Write_Register_Flag] = 1;
			Com_Write_Single_Register_Slave( pBuf );
			break;
		case 0x10:
			Det_Modbus_Buf[Det_Write_Register_Flag] = 2;
			Com_Write_Multiple_Register_Slave( pBuf );
			break;
	}
}

void Com_Read_Input_Register_Slave( U8 *rxbuf )
{
	U16		ad, wc;
	U16		wCRC;
	U16		i;
	U16		bc;
	U8		bData[256], Len;
	
	wCRC = CRC16( rxbuf, MRX_CHKSUM_LOW );
	if( !( ((wCRC&255) == *(rxbuf+MRX_CHKSUM_LOW)) && ((wCRC>>8&255) == *(rxbuf+MRX_CHKSUM_HGH)) )){
		DPRINT("CRC Input Fail\n");
		return;
	}
	
	ad = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	wc = (((U16)*(rxbuf+MRX_DATACHGH))<<8) | ((U16)*(rxbuf+MRX_DATACLOW));
	bc = wc*2;
	
	if( ad >= MAX_INPUT_REGISTER ){
		DPRINT("Buff Size 1 Fail\n");
		return;
	}
	if( wc == 0 || wc > 125 || wc > MAX_INPUT_REGISTER ){
		DPRINT("Buff Size 2 Fail\n");
		return;
	}
	if( ad+wc > MAX_INPUT_REGISTER ){
		DPRINT("Buff Size 3 Fail\n");
		return;
	}
	
	bData[MTX_STATNO] = *(rxbuf+MRX_STATNO);
	bData[MTX_FUNCODE] = *(rxbuf+MRX_FUNCODE);
	bData[MTX_BYTECNT] = bc;
	
	for( i=0; i<wc; i++ ){
		bData[MTX_DATACHGH+i*2] = Det_Input_Reg_Data[ad+i] >> 8 & 255;
		bData[MTX_DATACLOW+i*2] = Det_Input_Reg_Data[ad+i] & 255;
		//bData[MTX_DATACHGH+i*2] = epr_sm[ad+i] >> 8 & 255;
		//bData[MTX_DATACLOW+i*2] = epr_sm[ad+i] & 255;
	}
	
	wCRC = CRC16( &bData[0], bc+MTX_DATA );
	bData[bc+3] = wCRC & 255;
	bData[bc+4] = wCRC >> 8 & 255;
	
	Len = bc+5;
	
	USART1_Packet_Send( bData, Len );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Com_Read_Holding_Register_Slave( U8 *rxbuf )
{
	U16		ad, wc;
	U16		wCRC;
	U16		i;
	U16		bc;
	U8		bData[256], Len;
	
	wCRC = CRC16( rxbuf, MRX_CHKSUM_LOW );
	if( !( ((wCRC&255) == *(rxbuf+MRX_CHKSUM_LOW)) && ((wCRC>>8&255) == *(rxbuf+MRX_CHKSUM_HGH)) )){
		DPRINT("CRC holding Fail\n");
		return;
	}
	
	ad = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	wc = (((U16)*(rxbuf+MRX_DATACHGH))<<8) | ((U16)*(rxbuf+MRX_DATACLOW));
	bc = wc*2;
/*	
	if( ad >= EEP_MAX ){
		DPRINT("Buff Size 1 Fail\n");
		return;
	}
	if( wc == 0 || wc > 125 || wc > EEP_MAX ){
		DPRINT("Buff Size 2 Fail\n");
		return;
	}
	if( ad+wc > EEP_MAX ){
		DPRINT("Buff Size 3 Fail\n");
		return;
	}
*/	
	bData[MTX_STATNO] = *(rxbuf+MRX_STATNO);
	bData[MTX_FUNCODE] = *(rxbuf+MRX_FUNCODE);
	bData[MTX_BYTECNT] = bc;
	
	for( i=0; i<wc; i++ ){
		bData[MTX_DATACHGH+i*2] = Det_Holding_Reg_Data[ad+i] >> 8 & 255;
		bData[MTX_DATACLOW+i*2] = Det_Holding_Reg_Data[ad+i] & 255;
	}
	
	wCRC = CRC16( &bData[0], bc+MTX_DATA );
	bData[bc+3] = wCRC & 255;
	bData[bc+4] = wCRC >> 8 & 255;
	
	Len = bc+5;
	
	USART1_Packet_Send( bData, Len );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Com_Write_Single_Register_Slave( U8 *rxbuf )
{
U16		wCRC;
	
	//DPRINT("Single Write PC\n");
	wCRC = CRC16( rxbuf, MRX_CHKSUM_LOW );
	if( !( ((wCRC&255) == *(rxbuf+MRX_CHKSUM_LOW)) && ((wCRC>>8&255) == *(rxbuf+MRX_CHKSUM_HGH)) )){
		DPRINT("CRC Single Fail\n");
		return;
	}
	
	Det_Modbus_Buf[Det_Write_Register_Addr] = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	Det_Modbus_Buf[Det_Write_Register_Data] = (((S16)*(rxbuf+MRX_DATACHGH))<<8) | ((S16)*(rxbuf+MRX_DATACLOW));
	
	/*
	if(( ad <= EEP_PREFIX ) || ( ad >= EEP_MAX )){
		DPRINT("Buff Size 1 Fail\n");
		return;
	}
	*/	
	
	//Det_Input_Reg_Data[ad] = data;
	USART1_Packet_Send( rxbuf, 8 );
	Det_Modbus_Buf[Det_Comm_Flow] = Commnication_Dev_Single;
	Det_Write_Single_Func(Det_Modbus_Buf[Det_Write_Register_Addr],Det_Modbus_Buf[Det_Write_Register_Data]);
	
	/*
	if(( data >= (S16)pgm_read_word( &EEP_SYS_LO_LIMIT[ad] )) && ( data <= (S16)pgm_read_word( &EEP_SYS_UP_LIMIT[ad] ))){
		EEP_Sys_Wr( ad, data );
		if( !EepErrorFlag ){
			USART1_Packet_Send( rxbuf, 8 );
		}
	}
	else {
		DPRINT3(" OVER RANGE, IN: %d, LO: %d, UP: %d\n", data, pgm_read_word( &EEP_SYS_LO_LIMIT[ad] ), pgm_read_word( &EEP_SYS_UP_LIMIT[ad] ));
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Com_Write_Multiple_Register_Slave( U8 *rxbuf )
{
	S16		data;
	U16		wCRC;
	U16		i;
	U8		bc;
	U8		bData[8];
	
	//DPRINT("Multi Write PC\n");
	bc = *(rxbuf+MRX_BYTECNT);
	wCRC = CRC16( rxbuf, bc+7 );
	if( !( ((wCRC&255) == *(rxbuf+bc+7)) && ((wCRC>>8&255) == *(rxbuf+bc+8)) )){
		DPRINT("CRC multi Fail\n");
		return;
	}
	
	Det_Modbus_Buf[Det_Write_Register_Addr] = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	Det_Modbus_Buf[Det_Write_Register_Data] = (((U16)*(rxbuf+MRX_DATACHGH))<<8) | ((U16)*(rxbuf+MRX_DATACLOW));
	
	/*
	if(( ad <= EEP_PREFIX ) || ( ad >= EEP_MAX )){
		DPRINT("Buff Size 1 Fail\n");
		return;
	}
	
	if( wc == 0 || wc > EEP_MAX ){
		DPRINT("Buff Size 2 Fail\n");
		return;
	}
	
	if( ad+wc > EEP_MAX ){
		DPRINT("Buff Size 3 Fail\n");
		return;
	}
	*/
	
	for( i=0; i<Det_Modbus_Buf[Det_Write_Register_Data]; i++ ){
		data = (((S16)*(rxbuf+7+i*2))<<8) | ((S16)*(rxbuf+8+i*2));
		write_register_data_arry[i] = data;
		/*
		if(( data >= (S16)pgm_read_word( &EEP_SYS_LO_LIMIT[ad+i] )) && ( data <= (S16)pgm_read_word( &EEP_SYS_UP_LIMIT[ad+i] ))){
			epr_sm[ad+i] = data;
			EEP_Wr( ad+i, data );
		}
		else	DPRINT3(" OVER RANGE, IN: %d, LO: %d, UP: %d\n", data, pgm_read_word( &EEP_SYS_LO_LIMIT[ad+i] ), pgm_read_word( &EEP_SYS_UP_LIMIT[ad+i] ));
		*/
	}
	/*
	EEP_CRC_Wr();
	
	if (EEP_CRC_Check())	EepErrorFlag = ad;
	else					EepErrorFlag = 0;
	*/
	for (i=0; i<6; i++)
		bData[i] = *(rxbuf+i);
		
	wCRC = CRC16( bData, 6 );
	bData[6] = wCRC & 255;
	bData[7] = wCRC >> 8 & 255;
	USART1_Packet_Send( bData, 8 );
}
