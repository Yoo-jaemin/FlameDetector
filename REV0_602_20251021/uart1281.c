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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "dbg_printf.h"

#include "uart1281.h"
#include "gpio.h"
#include "modbus.h"
#include "sw_lib.h"
#include "ir.h"
#include "adc.h"
#include "eep.h"
#include "ir.h"
#include "uv.h"
#include "timer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FILE      	*pUart;      		//Uart_dbg Pointer dG

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U8	Rx0Buf[MAX_USART0_RX_BUFF], Rx0Cnt;
U8	Rx1Buf[2][MAX_USART1_RX_BUFF], Rx1Cnt, Rx1FifoHp, Rx1FifoTp;
U8	Rx1_Packet_Check;
U8	Tx1Buf[MAX_USART1_TX_BUFF], Tx1Cnt;
U8	Tx1BusyFlag, *pTx1Buf;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Initialize_UART( void )
{
	U16		wBaudRate;
	
	UCSR0A = ( 1 << U2X0 ); 		/* Double Speed */
	UCSR0B = ( 1 << RXCIE1 ) | ( 1 << RXEN1 ) | ( 1 << TXEN0 );
	//RX INT,RX Nï¿½ï¿½Lï¿½ë«”X ï¿½ï¿½ï¿½Ç?
	UCSR0C = ( 1 << USBS0 ) | ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );
	//UCSR0C Write Register Set,ï¿½ñµ¿±à«®one?ï¿½ï¿½Æ¼,8ï¿½ò†¬?1ï¿½ï¿½Æ©ï¿½ò†¬?
	wBaudRate = (U16)(( ATMEGA_XTAL / ( 8 * DEBUG_BAUD_SPEED ) ) - 1);
	UBRR0H = (U8)((wBaudRate >> 8 ) & 0xFF );
	UBRR0L = (U8)((wBaudRate & 0xFF));
	
	Initialize_MODBUS();
	
	RS485_DIR_RX();
	
	UCSR1A = ( 1 << U2X1 ); 		/* Double Speed */
	UCSR1B = ( 1 << RXCIE1 ) | ( 1 << RXEN1 ) | ( 1 << TXCIE1 ) | ( 1 << TXEN1 );
	//RX INT,RX Nï¿½ï¿½Lï¿½ë«”X ï¿½ï¿½ï¿½Ç?
	UCSR1C = ( 1 << USBS1 ) | ( 1 << UCSZ11 ) | ( 1 << UCSZ10 );
	//UCSR0C Write Register Set,ï¿½ñµ¿±à«®one?ï¿½ï¿½Æ¼,8ï¿½ò†¬?1ï¿½ï¿½Æ©ï¿½ò†¬?
	
	wBaudRate = (U16)(( ATMEGA_XTAL / ( 8 * Modbus_Baud ) ) - 1);
	UBRR1H = (U8)((wBaudRate >> 8 ) & 0xFF );
	UBRR1L = (U8)((wBaudRate & 0xFF));
	
	Rx1Cnt = 0;
	Rx1FifoHp = 0;
	Rx1FifoTp = 0;
	Rx1_Packet_Check = 0;
	Tx1BusyFlag = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(USART0_RX_vect)
{
	U8	bStatus, bData, *pBuf;
	
	bStatus = UCSR0A;
	bData = UDR0;
	
	if ( !( bStatus & ( FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN )) ){
		
        if( Rx0Cnt >= MAX_USART0_RX_BUFF ){
			Rx0Cnt = 0;
		}
		
		Rx0Buf[Rx0Cnt++] = bData;
		pBuf = &Rx0Buf[Rx0Cnt];
		
		if( *(pBuf-6) == '@' && *(pBuf-1) == ':' ){
			if( *(pBuf-5) == 'K' && *(pBuf-4) == 'P' && *(pBuf-3) == 'L' && *(pBuf-2) == 'H' ){
				LogMode = LMODE_RAW;
				FlameRawFifoHp = 0;
				FlameRawFifoTp = 0;
			}
			else {
				LogMode = LMODE_NO;
			}
			Rx0Cnt = 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(USART1_RX_vect)
{
	U8	bStatus, bData;
	
	bStatus = UCSR1A;
	bData = UDR1;
	
	if ( !( bStatus & ( FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN )) ){
		
        if( Rx1Cnt >= MAX_USART1_RX_BUFF ){
			Rx1Cnt = 0;
		}
		
		Rx1Buf[Rx1FifoHp&1][Rx1Cnt] = bData;
		Rx1Cnt++;
		
		if( Modbus_Baud == 9600 ){
			Rx1_Packet_Check = 0x83; // 4ms
		}
		else {
			Rx1_Packet_Check = 0x81; // 2ms
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(USART1_TX_vect)
{
	
	if( !Tx1Cnt ){
		RS485_DIR_RX();
		Tx1BusyFlag = 0;
	}
	else {
		UDR1 = *pTx1Buf++;
		Tx1Cnt--;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USART1_Rx_Packet_Check( void )
{	
	if( Rx1_Packet_Check & 0x80 ){
		if( Rx1_Packet_Check == 0x80 ){
			Rx1_Packet_Check = 0;
			
			if( Rx1Cnt > 7 ){
				//UART_TP_H();
				if( Rx1Buf[Rx1FifoHp&1][0] == Modbus_Addr ){
					if( Rx1Buf[Rx1FifoHp&1][1] != MB_FUNC_RATIO_WRITE)
					{
						sTimerTick[TMR_MODBUS_TIMEOUT] = 0x8000+99; // 100ms
						sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
					}
					Rx1FifoHp++;
				}
				else {
					sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
					sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
				}
			}
			Rx1Cnt = 0;
		}
		else {
			Rx1_Packet_Check--;
		}
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
		//UART_TP_L();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USART1_Packet_Send( U8 *pData, U8 Len )
{
	U16		i;
	
	sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
	
	if( sTimerFlag[TMR_MODBUS_TIMEOUT] ){
		sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
	}
	else {
		if( !Tx1BusyFlag ){
			RS485_DIR_TX();
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Putchar( char ch, FILE *stream )
{
	while( !(UCSR0A & ( 1 << UDRE0 )) );   //Uart_dbg Tx Buff Not Empty
	
	
	if( ch == '\n' ){
		UDR0 = '\r';
		while( !(UCSR0A & ( 1 << UDRE0 )) );
		UDR0 = '\n';
	}
	else {
		UDR0 = ch;
	}
	
	return 0;
}
