/* *********************************************************************************
 *  GTJ-1000F.c
 *
 *	Created: 2025-09-30 
 *	Author : jmyoo
  
 ********************************************************************************* */ 

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "global.h"
#include "dbg_printf.h"

#include "uart2560.h"
#include "gpio.h"
#include "timer.h"
#include "modbus.h"
#include "mmi.h"
#include "lcd.h"
#include "eeprom.h"
#include "mkey.h"


FILE 	*pUart;					//Uart Pointer ����
UART_VARIABLE	Uart_dbg;		//DBG

U8	Rx1Buf[2][MAX_USART1_RX_BUFF], Rx1Cnt, Rx1FifoHp, Rx1FifoTp;
U8	Rx1_Packet_Check;
U8	Tx1Buf[MAX_USART1_TX_BUFF], Tx1Cnt;
U8	Tx1BusyFlag, *pTx1Buf;

U8  Det_Rx2Buf[2][MAX_USART2_RX_BUFF];
U8  DET_Rx2Cnt, Det_Rx2FifoHp, Det_Rx2FifoTp;
U8	Rx2_Packet_Check;

U8	Tx2Buf[MAX_USART2_TX_BUFF], Tx2Cnt;
U8	Tx2BusyFlag, *pTx2Buf;

U16	Det_Modbus_Buf[Det_Modbus_Max_Buf];
U16	Det_Input_Reg_Data[MAX_MODBUS_INPUT_MAX], Det_Holding_Reg_Data[MAX_MODBUS_HOLDING_MAX];


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Initialize_UARTBps(void)
{
	U16	wBaudRate;
	U8	i;

	// _DEBUG_PORT
	Uart_dbg.bReceiveTail = 0;	
	Uart_dbg.bReceiveHead = 0;

	UCSR0A = ( 1 << U2X0 );  /* Double Speed */
	UCSR0B = ( 1 << RXCIE0 ) | ( 1 << RXEN0  ) | ( 1 << TXEN0 );	//RX INT,RX �ο��̺�,TX ����
	UCSR0C = ( 1 << USBS0  ) | ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );	//UCSR0C Write Register Set,�񵿱�,none�и�Ƽ,8��Ʈ,1�����Ʈ

	wBaudRate = (U16)(( ATMEGA_XTAL / ( 8 * (U32)_DBG_BAUD_SPEED ) ) - 1);	// ��żӵ� ����
	UBRR0H = (U8)((wBaudRate >> 8 ) & 0xFF );
	UBRR0L = (U8)((wBaudRate & 0xFF));

	// _MODBUS_PORT
	MODBUS_DIR_RX();
	UCSR1A = ( 1 << U2X1 ); /* Double Speed */
	UCSR1B = ( 1 << RXCIE1 ) | ( 1 << RXEN1  ) | ( 1 << TXCIE1 ) | ( 1 << TXEN1 );
	UCSR1C = ( 1 << USBS1  ) | ( 1 << UCSZ11 ) | ( 1 << UCSZ10 );

	wBaudRate = (U16)(( ATMEGA_XTAL / ( 8 * (U32)_MODBUS_BAUD_SPEED ) ) - 1);	// ��żӵ� ����
	UBRR1H = (U8)((wBaudRate >> 8 ) & 0xFF );
	UBRR1L = (U8)((wBaudRate & 0xFF));

	// _RS485_PORT
	RS485_DIR_RX();
	UCSR2A = ( 1 << U2X2 ); /* Double Speed */
	UCSR2B = ( 1 << RXCIE2 ) | ( 1 << RXEN2  ) | ( 1 << TXCIE2 ) | ( 1 << TXEN2 );						// Rx Interrupt enable
	UCSR2C = ( 1 << USBS2  ) | ( 1 << UCSZ21 ) | ( 1 << UCSZ20 );

	if (!Det_BPS)				wBaudRate = (U16)(( ATMEGA_XTAL / ( 8 * (U32)_RS485_BAUD_SPEED_9600 ) ) - 1);	// ��żӵ� ����
	else if (Det_BPS == 1)		wBaudRate = (U16)(( ATMEGA_XTAL / ( 8 * (U32)_RS485_BAUD_SPEED_57600 ) ) - 1);	// ��żӵ� ����
	else						wBaudRate = (U16)(( ATMEGA_XTAL / ( 8 * (U32)_RS485_BAUD_SPEED_115200 ) ) - 1);	// ��żӵ� ����
	
	UBRR2H = (U8)((wBaudRate >> 8 ) & 0xFF );
	UBRR2L = (U8)((wBaudRate & 0xFF));
	
	Tx1BusyFlag = 0;
	
	for (i = 0; i < Det_Modbus_Max_Buf; i++)
		Det_Modbus_Buf[i] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(USART0_RX_vect)		//DBG
{
	Uart_dbg.bReceiveBuff[ Uart_dbg.bReceiveHead ] = UDR0;				// 16����Ʈ �����ۿ� ���Ű� ����
	Uart_dbg.bReceiveHead = ( Uart_dbg.bReceiveHead + 1 ) % _MAX_UART_BUFF;		// 16����Ʈ �����ۿ� ���Ű� ����, ��ⷯ ������
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(USART1_RX_vect)
{
	U8	bStatus, bData;
	
	bStatus = UCSR1A;
	bData = UDR1;
	
	if ( !( bStatus & ( FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN )) )
	{
		if( Rx1Cnt >= MAX_USART1_RX_BUFF ){
			Rx1Cnt = 0;
		}
			
		Rx1Buf[Rx1FifoHp&1][Rx1Cnt] = bData;
		Rx1Cnt++;
		
		Rx1_Packet_Check = 0x83; // 4ms
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(USART1_TX_vect)
{
	if ( !Tx1Cnt ) {
		MODBUS_DIR_RX();
		Tx1BusyFlag = 0;
	}
	else {
		UDR1 = *pTx1Buf++;
		Tx1Cnt--;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(USART2_RX_vect) 	//AIH Interface
{
	U8	bStatus, bData;	
	bStatus = UCSR2A;
	bData = UDR2;
	
	if ( !( bStatus & ( FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN )) )
	{
		if (DET_Rx2Cnt >= _MAX_UART_DET_BUFF)
			DET_Rx2Cnt = 0;
		
		Det_Rx2Buf[Det_Rx2FifoHp & 1][DET_Rx2Cnt] = bData;
		DET_Rx2Cnt++;
		Rx2_Packet_Check = 0x83; // 4ms
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(USART2_TX_vect)
{
	if (!Tx2Cnt) {
		RS485_DIR_RX();
		Tx2BusyFlag = 0;
	}
	else {
		UDR2 = *pTx2Buf++;
		Tx2Cnt--;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Putchar_U0(char ch, FILE *stream)	 	//DBG
{
	if ( ch == '\n' )	Putchar_U0('\r', stream );         

	while( !(UCSR0A & ( 1 << UDRE0 )) );   //Uart Tx Buff Not Empty
	UDR0 = ch;                             //Uart Tx Buff Transmitts

	return 0 ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void USART1_Rx_Packet_Check( void )
{
	if( Rx1_Packet_Check & 0x80 )
	{
		if( Rx1_Packet_Check == 0x80 )
		{
			Rx1_Packet_Check = 0;

			if( Rx1Cnt > 7 )
			{
				if( Rx1Buf[Rx1FifoHp&1][0] == Dvc_Address )
				{
					sTimerTick[TMR_MODBUS_TIMEOUT] = 0x8000+99; // 100ms
					sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
					Rx1FifoHp++;
				}
				else
				{
					sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
					sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
				}
			}
			Rx1Cnt = 0;
		}
		else	Rx1_Packet_Check--;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void USART1_Rx_Handler( void )
{
	U8		bHp, bTp;
	
	bHp = Rx1FifoHp & 1;
	bTp = Rx1FifoTp & 1;
	
	if( bHp != bTp ){
		Rx1FifoTp++;
		MODBUS_Packet_Decoder( &Rx1Buf[bTp][0] );
	}
}

void USART1_Packet_Send( U8 *pData, U8 Len )
{
	U16		i;
	
	sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
	
	if( sTimerFlag[TMR_MODBUS_TIMEOUT] ){
		sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
	}
	else {
		if( !Tx1BusyFlag ){
			MODBUS_DIR_TX();
			for( i=0; i<Len; i++ ){
				Tx1Buf[i] = *(pData+i);
			}
			pTx1Buf = Tx1Buf;
			Tx1Cnt = Len-1;
			UDR1 = *pTx1Buf++;
			Tx1BusyFlag = 1;
		}
	}
}

// ==================================================================================================================================

void USART2_Rx_Packet_Check(void)
{
	if (Rx2_Packet_Check & 0x80)
	{
		if (Rx2_Packet_Check == 0x80) { // All received
			Rx2_Packet_Check = 0;
			Det_Rx2FifoHp++;
			DET_Rx2Cnt = 0;	
		} else
			Rx2_Packet_Check--;		
	}	
}

void MODBUS_RTU_Master_Rx_Handler(void)
{
	U8		bHp, bTp;
	
	bHp = Det_Rx2FifoHp & 1;
	bTp = Det_Rx2FifoTp & 1;

	if (bHp != bTp) {
		DPRINT(" << ");
		Det_Rx2FifoTp++;
		MODBUS_Packet_Decoder_Master( &Det_Rx2Buf[bTp][0] );
	}
}

void USART2_Packet_Send( U8 *pData, U8 Len )
{
	U16 i;
	
	if (!Tx2BusyFlag) {
		RS485_DIR_TX();
		DPRINT(" >> ");
		
		for (i = 0; i < Len; i++) {
			Tx2Buf[i] = *(pData+i);
			DPRINT1(" %x ", Tx2Buf[i]);
		}
		DPRINT("\r\n");
		
		pTx2Buf = Tx2Buf;
		Tx2Cnt = Len-1;
		UDR2 = *pTx2Buf++;
		Tx2BusyFlag = 1;
	}
}

void MODBUS_RTU_Master_Write_Handler(void)
{	
	Det_Modbus_Buf[Det_Comm_Flow_Set] = Det_Modbus_Buf[Det_Comm_Flow];
	
	switch (Det_Modbus_Buf[Det_Comm_Flow_Set])
	{
		case Commnication_Default_Set:
		{
			switch (modbus_rtu_master.write_step)
			{
				case _1STEP_:
					Det_Write_Single_Func(DET_EEP_UV_DET_LMT,1200);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = _2STEP_;
					break;
				case _2STEP_:
					Det_Write_Single_Func(DET_EEP_CAL_BUF_UV_CPS,100);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = _3STEP_;
					break;
				case _3STEP_:
					Det_Write_Multi_Func(DET_EEP_CAL_BUF_IR430_01, DET_EEP_CAL_BUF_IR530_07);			
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = THE_END;
					break;
				case WAIT:
					if (!modbus_rtu_master.write_flag)
						modbus_rtu_master.write_step = modbus_rtu_master.write_next_step;
					break;		
			}
		} break;
			
		case Commnication_IR_Calribration_Set:
			Det_Write_Multi_Func(DET_EEP_CAL_BUF_IR430_01, DET_EEP_CAL_BUF_IR530_07);
			break;
			
		case Commnication_IR_Size_Set:
		{	
			switch (modbus_rtu_master.write_step)
			{
				case _1STEP_:
					Det_Write_Multi_Func(DET_EEP_CAL_VAL_IR430_01, DET_EEP_CAL_VAL_IR530_07);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = _2STEP_;	
					break; 
				case _2STEP_:
					Det_Write_Single_Func(DET_EEP_TRIP_IR_SIZE_LMT, 35);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = THE_END;
					break;
				case WAIT:
					if (!modbus_rtu_master.write_flag)
						modbus_rtu_master.write_step = modbus_rtu_master.write_next_step;
					break;
			}
		} break; 	
			
		case Commnication_UV_Calribration_Set:
		{
			switch (modbus_rtu_master.write_step)
			{
				case _1STEP_:
					if (Det_Input_Reg_Data[DET_InputReg_FlameType] == NON_HYDROCARBON) {
							
						if ((uv_cps >= 50) && (uv_cps < 70))			uv_cps = 70;
						else if ((uv_cps > 140) && (uv_cps <= 250))		uv_cps = 140;
					}
					Det_Write_Single_Func(DET_EEP_CAL_BUF_UV_CPS, uv_cps);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = _2STEP_;
					break;
				case _2STEP_:
					Det_Write_Single_Func(DET_EEP_UV_DET_LMT, uv_det_limit);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = _3STEP_;
					break;
				case _3STEP_:
					Det_Write_Single_Func(DET_EEP_UV_PWR_CTRL,1);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = _4STEP_;	
					break;
				case _4STEP_:
					Det_Write_Single_Func(DET_EEP_UV_BIT_FINISH_TIME, CLEAR_EEP_UV_BIT_FINISH_TIME);
					modbus_rtu_master.write_step = WAIT;
					modbus_rtu_master.write_next_step = THE_END;
					break;		
				case WAIT:
					if (!modbus_rtu_master.write_flag) 
						modbus_rtu_master.write_step = modbus_rtu_master.write_next_step;
					break;
			}
		} break;		 
	}
}

void MODBUS_RTU_Master_Holding_Register_Mapping(void)
{
	det_ir_reponsivity_466 = Det_Holding_Reg_Data[DET_EEP_CAL_BUF_IR430_RESPONSIVITY];
	alm_relay_operation	   = Det_Holding_Reg_Data[DET_EEP_ALM_RLY_OPERATION];
	trb_relay_operation	   = Det_Holding_Reg_Data[DET_EEP_TRB_RLY_OPERATION];
	bit_operation		   = Det_Holding_Reg_Data[DET_EEP_BIT_MODE];
	det_pwr_ctr_value	   = Det_Holding_Reg_Data[DET_EEP_UV_PWR_CTRL];
	det_log_mode_value     = Det_Holding_Reg_Data[DET_EEP_LOG_MODE];
	FlameType              = Det_Holding_Reg_Data[DET_EEP_FLAME_TYPE];
	FlameDetDelay_Sec      = Det_Holding_Reg_Data[DET_EEP_FLAME_DETECTION_DELAY_TIME];
	
	//DPRINT4(" %d, %d, %d, %d", det_ir_reponsivity_466, alm_relay_operation, trb_relay_operation, bit_operation);
	//DPRINT4(" ,%d, %d, %d, %d \r\n", det_pwr_ctr_value, det_log_mode_value, FlameType, FlameDetDelay_Sec);
}

void Det_Write_Single_Func(U16 addr, S16 data)
{
	modbus_rtu_master.write_flag = 1;
	Det_Modbus_Buf[Det_Write_Register_Addr] = addr;
	Det_Modbus_Buf[Det_Write_Register_Data] = data;
	Det_Modbus_Buf[Det_Write_Register_Flag] = WRITE_SINGLE_REGISTER;
}

void Det_Write_Multi_Func(U16 s_addr, U16 e_addr)
{
	modbus_rtu_master.write_flag = 1;
	Det_Modbus_Buf[Det_Write_Register_Addr] = s_addr;
	Det_Modbus_Buf[Det_Write_Register_Data] = e_addr + 1;
	Det_Modbus_Buf[Det_Write_Register_Flag] = WRITE_MULTIPLE_REGISTER;
}

