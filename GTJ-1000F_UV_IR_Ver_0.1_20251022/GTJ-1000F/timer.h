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

#ifndef __timer_H__
#define __timer_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define	TMR3_RES			(0xFFFF)
#define TMR3_1MS_CNT		(ATMEGA_XTAL/1000)
#define TMR3_REG_VALUE		(TMR3_RES+1-TMR3_1MS_CNT)

#define NORMAL_RETURN_CNT	40
#define LONG_RETURN_CNT		40

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum {
	TMR_1MS = 0,
	TMR_10MS,
	TMR_100MS,
	TMR_250MS,
	TMR_1000MS,
	TMR_1000MS_String,
	TMR_ADC_TASK,
	TMR_SELFTEST,
	TMR_NORMAL_RETURN,
	TMR_MODBUS_TIMEOUT,
	TMR_DET_SYSTEM_CHECK,
	TMR_ENC_FREQ,
	TMR_DAC_DELAY,
	TMR_BUFF_MAX
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern U16	sTimerTick[ TMR_BUFF_MAX ];
extern U16	sTimerFlag[ TMR_BUFF_MAX ];
extern U16	EncoderFreqTick[2];
extern U8	timer_test;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_Timer( void );
extern void FullScale_Mode(U8 mode, U8 cnt);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __timer_H__
