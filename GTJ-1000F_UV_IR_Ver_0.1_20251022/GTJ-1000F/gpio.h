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

// Test Point
#define TP3_H()					(PORTG |= BIT5)
#define TP3_L()					(PORTG &= ~BIT5)
#define TP3_T()					(PORTG ^= BIT5)

#define TP4_H()					(PORTC |= BIT7)
#define TP4_L()					(PORTC &= ~BIT7)
#define TP4_T()					(PORTC ^= BIT7)

#define TP5_H()					(PORTH |= BIT7)
#define TP5_L()					(PORTH &= ~BIT7)
#define TP5_T()					(PORTH ^= BIT7)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Control
#define EXT_BIT_ON()			(PORTJ |= BIT2) 
#define EXT_BIT_OFF()			(PORTJ &= ~BIT2)

#define P24V_OUT_ON()			(PORTJ |= BIT7) 
#define P24V_OUT_OFF()			(PORTJ &= ~BIT7)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LED

#define PWR_LED_ON()			(PORTC |= BIT6) 
#define PWR_LED_OFF()			(PORTC &= ~BIT6)
#define PWR_LED_T()				(PORTC ^= BIT6) 

#define STATUS_LED_ON()			(PORTL |= BIT0) 
#define STATUS_LED_OFF()		(PORTL &= ~BIT0)
#define STATUS_LED_T()			(PORTL ^= BIT0) 

#define LCD_BL_ON()				(PORTK |= BIT7) 
#define LCD_BL_OFF()			(PORTK &= ~BIT7)
#define LCD_BL_T()				(PORTK ^= BIT7)

#define ALARM_LED_ON()			(PORTL |= BIT1) 
#define ALARM_LED_OFF()			(PORTL &= ~BIT1) 
#define ALARM_LED_T()			(PORTL ^= BIT1) 

#define TROUBLE_LED_ON()		(PORTL |= BIT2) 
#define TROUBLE_LED_OFF()		(PORTL &= ~BIT2) 
#define TROUBLE_LED_T()			(PORTL ^= BIT2) 

#define RS485_LED_ON()			(PORTL |= BIT3) 
#define RS485_LED_OFF()			(PORTL &= ~BIT3) 
#define RS485_LED_T()			(PORTL ^= BIT3) 

#define TEST_RD_LED_ON()		(PORTL |= BIT4) 
#define TEST_RD_LED_OFF()		(PORTL &= ~BIT4) 
#define TEST_RD_LED_T()			(PORTL ^= BIT4) 

#define TEST_GN_LED_ON()		(PORTL |= BIT5) 
#define TEST_GN_LED_OFF()		(PORTL &= ~BIT5) 
#define TEST_GN_LED_T()			(PORTL ^= BIT5) 

#define TEST_BL_LED_ON()		(PORTL |= BIT6) 
#define TEST_BL_LED_OFF()		(PORTL &= ~BIT6) 
#define TEST_BL_LED_T()			(PORTL ^= BIT6) 

#define TEST_LED_ALL_ON()		(PORTL |= (BIT4 | BIT5 | BIT6)) 
#define TEST_LED_ALL_OFF()		(PORTL &= ~(BIT4 | BIT5 | BIT6)) 
#define TEST_LED_ALL_T()		(PORTL ^= (BIT4 | BIT5 | BIT6)) 

#define ANALOG_RD_LED_ON()		(PORTC |= BIT0) 
#define ANALOG_RD_LED_OFF()		(PORTC &= ~BIT0) 
#define ANALOG_RD_LED_T()		(PORTC ^= BIT0) 

#define ANALOG_GN_LED_ON()		(PORTC |= BIT1) 
#define ANALOG_GN_LED_OFF()		(PORTC &= ~BIT1) 
#define ANALOG_GN_LED_T()		(PORTC ^= BIT1) 

#define ANALOG_BL_LED_ON()		(PORTC |= BIT2) 
#define ANALOG_BL_LED_OFF()		(PORTC &= ~BIT2) 
#define ANALOG_BL_LED_T()		(PORTC ^= BIT2) 

#define ANALOG_LED_ALL_ON()		(PORTC |= (BIT0 | BIT1 | BIT2)) 
#define ANALOG_LED_ALL_OFF()	(PORTC &= ~(BIT0 | BIT1 | BIT2)) 
#define ANALOG_LED_ALL_T()		(PORTC ^= (BIT0 | BIT1 | BIT2)) 

#define DET_OUT_RD_LED_ON()		(PORTC |= BIT3) 
#define DET_OUT_RD_LED_OFF()	(PORTC &= ~BIT3) 
#define DET_OUT_RD_LED_T()		(PORTC ^= BIT3) 

#define DET_OUT_GN_LED_ON()		(PORTC |= BIT4) 
#define DET_OUT_GN_LED_OFF()	(PORTC &= ~BIT4) 
#define DET_OUT_GN_LED_T()		(PORTC ^= BIT4) 

#define DET_OUT_BL_LED_ON()		(PORTC |= BIT5) 
#define DET_OUT_BL_LED_OFF()	(PORTC &= ~BIT5) 
#define DET_OUT_BL_LED_T()		(PORTC ^= BIT5) 

#define DET_OUT_LED_ALL_ON()	(PORTC |= (BIT3 | BIT4 | BIT5)) 
#define DET_OUT_LED_ALL_OFF()	(PORTC &= ~(BIT3 | BIT4 | BIT5)) 
#define DET_OUT_LED_ALL_T()		(PORTC ^= (BIT3 | BIT4 | BIT5)) 

#define MODBUS_DIR_RX()			(PORTH &= ~BIT2)
#define MODBUS_DIR_TX()			(PORTH |=  BIT2)

#define RS485_DIR_RX()			(PORTH &= ~BIT3)
#define RS485_DIR_TX()			(PORTH |=  BIT3)

#define POTENTIOMETER_SCL_H()	(PORTH |= BIT5)
#define POTENTIOMETER_SCL_L()	(PORTH &= ~BIT5)
#define POTENTIOMETER_SDA_H()	(PORTH |= BIT6)
#define POTENTIOMETER_SDA_L()	(PORTH &= ~BIT6)
#define POTENTIOMETER_SDA_R()	(PINH >> 6 & 0x01)

#define DAC_SCL_H()				(PORTA |= BIT0)
#define DAC_SCL_L()				(PORTA &= ~BIT0)
#define DAC_SDA_H()				(PORTA |= BIT1)
#define DAC_SDA_L()				(PORTA &= ~BIT1)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_GPIO(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __gpio_H__
