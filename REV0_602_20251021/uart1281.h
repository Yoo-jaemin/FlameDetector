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

#ifndef __uart1281_H__
#define __uart1281_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define print(str,...)          ( printf_P(PSTR(str),##__VA_ARGS__) )
                                //Uart_dbg Program Memory Use
                                //내부RAM메모리를 사용하지 않기위함
                                //printf( PSTR"   " , ... )을 정의

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_BAUD_SPEED			115200L

#define UPE 						2
#define OVR 						3
#define FE 							4

#define MAX_USART0_RX_BUFF			16
#define MAX_USART1_TX_BUFF			256
#define MAX_USART1_RX_BUFF			256

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FRAMING_ERROR				(1<<FE)
#define PARITY_ERROR 				(1<<UPE)
#define DATA_OVERRUN 				(1<<OVR)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_UART( void );
extern void USART1_Rx_Packet_Check( void );
extern void USART1_Rx_Handler( void );
extern void USART1_Packet_Send( U8 *pData, U8 Len );
extern int Putchar( char ch, FILE *stream );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __uart1281_H__
