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

#ifndef __gpio_H__
#define __gpio_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ALM_LED_ON()		(PORTG |=  BIT3)
#define ALM_LED_OFF()		(PORTG &= ~BIT3)
#define RED_LED_ON()		(PORTD |=  BIT7)
#define RED_LED_OFF()		(PORTD &= ~BIT7)
#define GRN_LED_ON()		(PORTD |=  BIT5)
#define GRN_LED_OFF()		(PORTD &= ~BIT5)

#define WHT_LED_ON()		(PORTF |=  BIT5)
#define WHT_LED_OFF()		(PORTF &= ~BIT5)

#define ALM_RLY_H()			(PORTB |=  BIT4)
#define ALM_RLY_L()			(PORTB &= ~BIT4)
#define ALM_RLY_CHK()		(PORTB &   BIT4)
#define TRB_RLY_H()			(PORTB |=  BIT5)
#define TRB_RLY_L()			(PORTB &= ~BIT5)

#define TLUV_ON()			(PORTE &= ~BIT6)
#define TLUV_OFF()			(PORTE |=  BIT6)
#define TLUV_PORT_CHK()		(PORTE & BIT6)
#define TLIR_ON()			(PORTE |=  BIT7)
#define TLIR_OFF()			(PORTE &= ~BIT7)
#define TLIR_PORT_CHK()		(PORTE & BIT7)

#define RS485_DIR_RX()		(PORTD &= ~BIT4)
#define RS485_DIR_TX()		(PORTD |=  BIT4)

#define MA_OUT_ENA()		(PORTG &= ~BIT4)
#define MA_OUT_DIS()		(PORTG |=  BIT4)

#define TMR_TP_H()			(PORTA |=  BIT3)
#define TMR_TP_L()			(PORTA &= ~BIT3)
#define TMR_TP_T()			(PORTA ^=  BIT3)
#define ADC_TP_H()			(PORTA |=  BIT4)
#define ADC_TP_L()			(PORTA &= ~BIT4)
#define ADC_TP_T()			(PORTA ^=  BIT4)
#define UART_TP_H()			(PORTA |=  BIT5)
#define UART_TP_L()			(PORTA &= ~BIT5)
#define UART_TP_T()			(PORTA ^=  BIT5)

#define SMPS_ON()			(PORTF |=  BIT4)
#define SMPS_OFF()			(PORTF &= ~BIT4)
#define SMPS_PORT_CHK()		(PORTF & BIT4)

#define BIT_PORT_CHK()		(PINB & BIT3)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RLY_ENERGIZED		1

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_GPIO(void);
extern void Alarm_Relay_On( void );
extern void Alarm_Relay_Off( void );
extern U8 Alarm_Relay_Check( void );
extern void Trouble_Relay_On( void );
extern void Trouble_Relay_Off( void );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __gpio_H__
